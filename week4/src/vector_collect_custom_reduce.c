#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

// function declarations
int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size);
int sum_vector(int vector[], int size);
void check_uni_size(int uni_size);
void my_sum(void *in, void *inout, int *len, MPI_Datatype *dtype);
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

	// scatters chunks of the array from root to all processes
	MPI_Scatter(full_vector, chunk, MPI_INT, local_chunk, chunk, MPI_INT, 0, MPI_COMM_WORLD);

	// root can free the full vector now
	if (0 == my_rank)
	{
		free(full_vector);
	}

	// sums this rank's local chunk
	int partial_sum = sum_vector(local_chunk, chunk);

	// frees the local chunk
	free(local_chunk);

	// creates the custom MPI operation
	MPI_Op custom_op;
	MPI_Op_create(my_sum, 1, &custom_op);

	// variable to hold the final sum (only meaningful on root)
	int total_sum = 0;

	// creates and initialises the timing variables
	struct timespec start_time, end_time, time_diff;
	double collect_time = 0.0;

	// times the collection step
	timespec_get(&start_time, TIME_UTC);
	// reduces all partial sums to a single total on root using custom op
	MPI_Reduce(&partial_sum, &total_sum, 1, MPI_INT, custom_op, 0, MPI_COMM_WORLD);
	timespec_get(&end_time, TIME_UTC);

	// calculates the collection time
	time_diff = calculate_runtime(start_time, end_time);
	collect_time = to_second_float(time_diff);

	// frees the custom operation
	MPI_Op_free(&custom_op);

	// root prints the result
	if (0 == my_rank)
	{
		printf("Sum: %d\n", total_sum);
		printf("Custom reduce collection time: %.9lf s\n", collect_time);
	}

	// finalise MPI
	ierror = MPI_Finalize();
	return 0;
}

void my_sum(void *in, void *inout, int *len, MPI_Datatype *dtype)
{
	// casts the void pointers to int pointers
	int *in_vals = (int *) in;
	int *inout_vals = (int *) inout;

	// element-wise adds in into inout
	for (int i = 0; i < *len; i++)
	{
		inout_vals[i] = inout_vals[i] + in_vals[i];
	}
}

int check_args(int argc, char **argv)
{
	int num_arg = 0;
	if (argc == 2)
	{
		num_arg = atoi(argv[1]);
	}
	else
	{
		fprintf(stderr, "ERROR: You did not provide a numerical argument!\n");
		fprintf(stderr, "Correct use: %s [VECTOR_SIZE]\n", argv[0]);
		exit(-1);
	}
	return num_arg;
}

void check_uni_size(int uni_size)
{
	int min_uni_size = 2;
	if (uni_size >= min_uni_size)
	{
		return;
	}
	else
	{
		fprintf(stderr, "ERROR: Unable to communicate with fewer than %d processes.", min_uni_size);
		fprintf(stderr, " MPI communicator size = %d\n", uni_size);
		exit(-1);
	}
}

void initialise_vector(int vector[], int size)
{
	for (int i = 0; i < size; i++)
	{
		vector[i] = i + 1;
	}
}

int sum_vector(int vector[], int size)
{
	int sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum += vector[i];
	}
	return sum;
}

double to_second_float(struct timespec in_time)
{
	double out_time = 0.0;
	long int seconds, nanoseconds;
	seconds = nanoseconds = 0;
	seconds = in_time.tv_sec;
	nanoseconds = in_time.tv_nsec;
	out_time = seconds + nanoseconds/1e9;
	return out_time;
}

struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time)
{
	struct timespec time_diff;
	long int seconds, nanoseconds;
	seconds = nanoseconds = 0;
	seconds = end_time.tv_sec - start_time.tv_sec;
	nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
	if (nanoseconds < 0)
	{
		seconds = seconds - 1;
		nanoseconds = ((long int) 1e9) - nanoseconds;
	}
	time_diff.tv_sec = seconds;
	time_diff.tv_nsec = nanoseconds;
	return time_diff;
}