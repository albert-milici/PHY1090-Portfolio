#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// function declarations
void root_task(int num_pings);
void client_task(int num_pings);
int check_args(int argc, char **argv);
void check_uni_size(int uni_size);
void check_task(int my_rank, int num_pings);
double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv) 
{
	// declare and initialise error handling variable
	int ierror = 0;

	// declare and initialise the numerical argument variable
	int num_pings = check_args(argc, argv);

	// intitalise MPI
	ierror = MPI_Init(&argc, &argv);

	// declare and initialise rank and size varibles
	int my_rank, uni_size;
	my_rank = uni_size = 0;

	// gets the rank and world size
	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

	// checks the universe size is correct
	check_uni_size(uni_size);

	// checks what task to do and does it
	check_task(my_rank, num_pings);

	// finalise MPI
	ierror = MPI_Finalize();
	return 0;
}

void root_task(int num_pings)
{
	// creates and initialises transmission variables
	int counter, count, client_rank, tag;
	counter = tag = 0;
	client_rank = 1;
	count = 1;
	MPI_Status status;

	// creates and initialises the timing variables
	struct timespec start_time, end_time, time_diff;
	double elapsed_time = 0.0;
	double average_time = 0.0;

	// gets the time before the ping pong loop
	timespec_get(&start_time, TIME_UTC);

	// loops through num_pings ping-pongs
	while (counter < num_pings)
	{
		// sends the counter to the client (ping)
		MPI_Send(&counter, count, MPI_INT, client_rank, tag, MPI_COMM_WORLD);
		// receives the counter back from the client (pong)
		MPI_Recv(&counter, count, MPI_INT, client_rank, tag, MPI_COMM_WORLD, &status);
	} // end while (counter < num_pings)

	// gets the time after the ping pong loop
	timespec_get(&end_time, TIME_UTC);

	// calculates the elapsed and average times
	time_diff = calculate_runtime(start_time, end_time);
	elapsed_time = to_second_float(time_diff);
	average_time = elapsed_time / num_pings;

	// prints the final counter, elapsed time and average time
    printf("Counter: %d, Elapsed: %lf s, Average: %.9lf s\n",
            counter, elapsed_time, average_time);
}

void client_task(int num_pings)
{
	// creates and initialises transmission variables
	int counter, count, root_rank, tag;
	counter = tag = 0;
	root_rank = 0;
	count = 1;
	MPI_Status status;

	// loops through num_pings ping-pongs
	while (counter < num_pings)
	{
		// receives the counter from root (ping)
		MPI_Recv(&counter, count, MPI_INT, root_rank, tag, MPI_COMM_WORLD, &status);
		// increments the counter
		counter = counter + 1;
		// sends the counter back to root (pong)
		MPI_Send(&counter, count, MPI_INT, root_rank, tag, MPI_COMM_WORLD);
	} // end while (counter < num_pings)
}

int check_args(int argc, char **argv)
{
	// declare and initialise the numerical argument
	int num_arg = 0;

	// check the number of arguments
	if (argc == 2) // program name and numerical argument
	{
		// declare and initialise the numerical argument
		num_arg = atoi(argv[1]);
	}
	else // the number of arguments is incorrect
	{
		// raise an error
		fprintf(stderr, "ERROR: You did not provide a numerical argument!\n");
		fprintf(stderr, "Correct use: %s [NUM_PINGS]\n", argv[0]);

		// and exit 
		exit(-1);
	}
	return num_arg;
}

void check_uni_size(int uni_size)
{
	// sets the required universe size
	int required_uni_size = 2;

	// checks the universe size is exactly as required
	if (uni_size == required_uni_size)
	{
		return;
	} // end if (uni_size == required_uni_size)
	else // i.e. uni_size != required_uni_size
	{
		// raise an error
		fprintf(stderr, "ERROR: pingpong requires exactly %d processes.", required_uni_size);
		fprintf(stderr, " MPI communicator size = %d\n", uni_size);

		// and exit 
		exit(-1);
	}
}

void check_task(int my_rank, int num_pings)
{
	// checks which process is running and calls the appropriate task
	if (0 == my_rank)
	{
		root_task(num_pings);
	} // end if (0 == my_rank)
	else // i.e. (0 != my_rank)
	{
		client_task(num_pings);
	} // end else // i.e. (0 != my_rank)
}

double to_second_float(struct timespec in_time)
{
	// creates and initialises the variables
	double out_time = 0.0;
	long int seconds, nanoseconds;
	seconds = nanoseconds = 0;

	// extracts the elements from in_time
	seconds = in_time.tv_sec;
	nanoseconds = in_time.tv_nsec;

	// calculates the time in seconds by adding the seconds and the nanoseconds divided by 1e9
	out_time = seconds + nanoseconds/1e9;

	// returns the time as a double
	return out_time;
}

struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time)
{
	// creates and initialises the variables
	struct timespec time_diff;
	long int seconds, nanoseconds;
	seconds = nanoseconds = 0;

	// extracts the differences between start_time and end_time
	seconds = end_time.tv_sec - start_time.tv_sec;
	nanoseconds = end_time.tv_nsec - start_time.tv_nsec;

	// if the ns part is negative
	if (nanoseconds < 0)
	{
		// "carry the one!"
		seconds = seconds - 1;
		nanoseconds = ((long int) 1e9) - nanoseconds;
	}

	// creates the runtime
	time_diff.tv_sec = seconds;
	time_diff.tv_nsec = nanoseconds;

	return time_diff;
}