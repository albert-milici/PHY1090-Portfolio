#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// function declarations
int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size);
int sum_vector(int vector[], int size);
void root_task(int my_rank, int uni_size, int partial_sum);
void client_task(int my_rank, int partial_sum);
void check_uni_size(int uni_size);
void check_task(int my_rank, int uni_size, int partial_sum);
double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv) 
{
	// declare and initialise error handling variable
	int ierror = 0;

	// declare and initialise the numerical argument variable
	int num_arg = check_args(argc, argv);

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

	// calculates the chunk size for each process
	int chunk = num_arg / uni_size;

	// only root creates and initialises the full vector
	int *full_vector = NULL;
	if (0 == my_rank)
	{
		full_vector = (int *) malloc(num_arg * sizeof(int));
		initialise_vector(full_vector, num_arg);
	}

	// each process allocates a local buffer for its chunk
	int *local_chunk = (int *) malloc(chunk * sizeof(int));

	// creates and initialises the timing variables
	struct timespec start_time, end_time, time_diff;
	double dist_time = 0.0;

	// times the distribution step
	timespec_get(&start_time, TIME_UTC);
	// scatters chunks of the array from root to all processes
	MPI_Scatter(full_vector, chunk, MPI_INT, local_chunk, chunk, MPI_INT, 0, MPI_COMM_WORLD);
	timespec_get(&end_time, TIME_UTC);

	// calculates the distribution time
	time_diff = calculate_runtime(start_time, end_time);
	dist_time = to_second_float(time_diff);

	// root can free the full vector now
	if (0 == my_rank)
	{
		free(full_vector);
	}

	// sums this rank's local chunk
	int partial_sum = sum_vector(local_chunk, chunk);

	// frees the local chunk
	free(local_chunk);

	// checks what task to do and does it
	check_task(my_rank, uni_size, partial_sum);

	// root prints the distribution time
	if (0 == my_rank)
	{
		printf("Scatter distribution time: %.9lf s\n", dist_time);
	}

	// finalise MPI
	ierror = MPI_Finalize();
	return 0;
}

void root_task(int my_rank, int uni_size, int partial_sum)
{
	// creates and initialises transmission variables
	int recv_message, count, source, tag;
	recv_message = source = tag = 0;
	count = 1;
	MPI_Status status;

	// starts the total with root's own partial sum
	int total_sum = partial_sum;

	// iterates through all the other ranks
	for (source = 1; source < uni_size; source++)
	{
		// receives the partial sum from each client
		MPI_Recv(&recv_message, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
		// adds to the running total
		total_sum += recv_message;
	} // end for (source = 1; source < uni_size; source++)

	// prints the final result
	printf("Sum: %d\n", total_sum);
}

void client_task(int my_rank, int partial_sum)
{
	// creates and initialises transmission variables
	int count, dest, tag;
	dest = tag = 0;
	count = 1;

	// sends the partial sum to root
	MPI_Send(&partial_sum, count, MPI_INT, dest, tag, MPI_COMM_WORLD);
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
		fprintf(stderr, "Correct use: %s [VECTOR_SIZE]\n", argv[0]);

		// and exit COMPLETELY
		exit(-1);
	}
	return num_arg;
}

void check_uni_size(int uni_size)
{
	// sets the minimum universe size
	int min_uni_size = 2;

	// checks there are sufficient tasks to communicate with
	if (uni_size >= min_uni_size)
	{
		return;
	} // end if (uni_size >= min_uni_size)
	else // i.e. uni_size < min_uni_size
	{
		// raise an error
		fprintf(stderr, "ERROR: Unable to communicate with fewer than %d processes.", min_uni_size);
		fprintf(stderr, " MPI communicator size = %d\n", uni_size);

		// and exit COMPLETELY
		exit(-1);
	}
}

void check_task(int my_rank, int uni_size, int partial_sum)
{
	// checks which process is running and calls the appropriate task
	if (0 == my_rank)
	{
		root_task(my_rank, uni_size, partial_sum);
	} // end if (0 == my_rank)
	else // i.e. (0 != my_rank)
	{
		client_task(my_rank, partial_sum);
	} // end else // i.e. (0 != my_rank)
}

void initialise_vector(int vector[], int size)
{
	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// sets element to i+1 so the sum is n*(n+1)/2
		vector[i] = i + 1;
	}
}

int sum_vector(int vector[], int size)
{
	// creates a variable to hold the sum
	int sum = 0;

	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// adds each element to the running sum
		sum += vector[i];
	}

	// returns the sum
	return sum;
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

	// calculates the time in seconds
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

	// extracts the differences
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