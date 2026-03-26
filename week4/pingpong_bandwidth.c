#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// function declarations
void root_task(int num_pings, int num_elements);
void client_task(int num_pings, int num_elements);
void check_args(int argc, char **argv, int *num_pings, int *array_size_bytes);
void check_uni_size(int uni_size);
void check_task(int my_rank, int num_pings, int num_elements);

double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv) 
{
	// declare and initialise error handling variable
	int ierror = 0;

	// declare and initialise argument variables
	int num_pings = 0;
	int array_size_bytes = 0;
	check_args(argc, argv, &num_pings, &array_size_bytes);

	// calculates the number of elements needed for the given byte size
	int num_elements = array_size_bytes / (int) sizeof(int);

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
	check_task(my_rank, num_pings, num_elements);

	// finalise MPI
	ierror = MPI_Finalize();
	return 0;
}

void root_task(int num_pings, int num_elements)
{
	// creates and initialises transmission variables
	int count, client_rank, tag;
	tag = 0;
	client_rank = 1;
	count = num_elements;
	MPI_Status status;

	// allocates and initialises the array to zero
	int *array = (int *) malloc(num_elements * sizeof(int));
	for (int i = 0; i < num_elements; i++) array[i] = 0;

	// creates and initialises the timing variables
	struct timespec start_time, end_time, time_diff;
	double elapsed_time = 0.0;
	double average_time = 0.0;

	// gets the time before the ping pong loop
	timespec_get(&start_time, TIME_UTC);

	// uses element 0 of the array as the counter
	while (array[0] < num_pings)
	{
		// sends the array to the client (ping)
		MPI_Send(array, count, MPI_INT, client_rank, tag, MPI_COMM_WORLD);
		// receives the array back from the client (pong)
		MPI_Recv(array, count, MPI_INT, client_rank, tag, MPI_COMM_WORLD, &status);
	} // end while (array[0] < num_pings)

	// gets the time after the ping pong loop
	timespec_get(&end_time, TIME_UTC);

	// calculates the elapsed and average times
	time_diff = calculate_runtime(start_time, end_time);
	elapsed_time = to_second_float(time_diff);
	average_time = elapsed_time / num_pings;

	// prints the final counter, size, elapsed time and average time
	printf("Counter: %d, Size: %d B, Elapsed: %lf s, Average: %.9lf s\n",
			array[0], num_elements * (int) sizeof(int), elapsed_time, average_time);

	// frees the array
	free(array);
}

void client_task(int num_pings, int num_elements)

	// creates and initialises transmission variables
	int count, root_rank, tag;
	tag = 0;
	root_rank = 0;
	count = num_elements;
	MPI_Status status;

	// allocates and initialises the array to zero
	int *array = (int *) malloc(num_elements * sizeof(int));
	for (int i = 0; i < num_elements; i++) array[i] = 0;

	// uses a local counter to track the loop
	int counter = 0;

	// loops through num_pings ping-pongs
	while (counter < num_pings)
	{
		// receives the array from root (ping)
		MPI_Recv(array, count, MPI_INT, root_rank, tag, MPI_COMM_WORLD, &status);
		// increments the counter stored in element 0
		array[0] = array[0] + 1;
		// sends the array back to root (pong)
		MPI_Send(array, count, MPI_INT, root_rank, tag, MPI_COMM_WORLD);
		// updates local counter from the array
		counter = array[0];
	} // end while (counter < num_pings)

	// frees the array
	free(array);
}

void check_args(int argc, char **argv, int *num_pings, int *array_size_bytes)
{
	// check the number of arguments
	if (argc == 3) // program name, num_pings, array_size_bytes
	{
		// converts the arguments to integers
		*num_pings = atoi(argv[1]);
		*array_size_bytes = atoi(argv[2]);
	}
	else // the number of arguments is incorrect
	{
		// raise an error
		fprintf(stderr, "ERROR: Incorrect number of arguments!\n");
		fprintf(stderr, "Correct use: %s [NUM_PINGS] [SIZE_BYTES]\n", argv[0]);

		// and exit 
		exit(-1);
	}
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

void check_task(int my_rank, int num_pings, int num_elements)
{
	// checks which process is running and calls the appropriate task
	if (0 == my_rank)
	{
		root_task(num_pings, num_elements);
	} // end if (0 == my_rank)
	else // i.e. (0 != my_rank)
	{
		client_task(num_pings, num_elements);
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