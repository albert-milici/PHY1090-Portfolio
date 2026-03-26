#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// function declarations
void root_task(int my_rank, int uni_size);
void client_task(int my_rank, int uni_size);
void check_uni_size(int my_rank, int uni_size);
void check_task(int my_rank, int uni_size);

double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv) 
{
	// declare and initialise error handling variable
	int ierror = 0;
	
	// declare and initialise rank and size varibles
	int my_rank, uni_size;
	my_rank = uni_size = 0;

	// intitalise MPI
	ierror = MPI_Init(&argc, &argv);

	// gets the rank and world size
	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

	// checks the universe size is correct
	check_uni_size(my_rank, uni_size);

	// checks what task to do and does it
	check_task(my_rank, uni_size);

	// finalise MPI
	ierror = MPI_Finalize();
	return 0;
}

void root_task(int my_rank, int uni_size)
{
	// creates and initialises transmission variables
	int recv_message, count, source, tag;
	recv_message = source = tag = 0;
	count = 1;
	MPI_Status status;

	// iterates through all the other ranks
	for (int their_rank = 1; their_rank < uni_size; their_rank++)
	{
		// sets the source argument to the rank of the sender
		source = their_rank;

		// receives the messages
		MPI_Recv(&recv_message, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

		// prints the message from the sender
		printf("Hello, I am %d of %d. Received %d from Rank %d\n",
				my_rank, uni_size, recv_message, source);
	} // end for (int their_rank = 1; their_rank < uni_size; their_rank++)
}

void client_task(int my_rank, int uni_size)
{
	// creates and initialises transmission variables
	int send_message, count, dest, tag;
	send_message = dest = tag = 0;
	count = 1;

	// creates and initialises the timing variables
	struct timespec start_time, end_time, time_diff;
	double runtime = 0.0;

	// sets the destination for the message
	dest = 0; // destination is root

	// creates the message
	send_message = my_rank * 10;

	// gets the time before the send
	timespec_get(&start_time, TIME_UTC);
	// sends the message
	MPI_Send(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD);
	// gets the time after the send
	timespec_get(&end_time, TIME_UTC);

	// calculates the runtime
	time_diff = calculate_runtime(start_time, end_time);
	runtime = to_second_float(time_diff);

	// prints the message from the sender and the runtime
	printf("Hello, I am %d of %d. Sent %d to Rank %d in %lf seconds\n",
			my_rank, uni_size, send_message, dest, runtime);
}

void check_uni_size(int my_rank, int uni_size)
{
	// sets the minimum uni size
	int min_uni_size = 2;

	// checks there are sufficient tasks to communicate with
	if (uni_size >= min_uni_size)
	{
		return;
	} // end if (uni_size >= min_uni_size)
	else // i.e. uni_size < min_uni_size
	{
		// raise an error
		printf("Unable to communicate with less than 2 processes. MPI communicator size = %d\n", uni_size);

		// and exit
		exit(-1);
	}
}

void check_task(int my_rank, int uni_size)
{
	// checks which process is running and calls the appropriate task
	if (0 == my_rank)
	{
		root_task(my_rank, uni_size);
	} // end if (0 == my_rank)
	else // i.e. (0 != my_rank)
	{
		client_task(my_rank, uni_size);
	} // end else // i.e. (0 != my_rank)
}

double to_second_float(struct timespec in_time)
{
	// creates and initialises the variables
	float out_time = 0.0;
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
	long int seconds, nanoseconds;                                                                                                       seconds = nanoseconds = 0;
	double runtime = 0.0;

	// extracts the elements from start_time and end_time
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