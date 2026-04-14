#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// declares the functions that will be called within main
int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size, int initial);
void print_vector(int vector[], int size);
int sum_vector(int vector[], int size);
double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv)
{
	// declare and initialise the numerical argument variable
	int num_arg = check_args(argc, argv);

	// declare and initialise the vector pointer
	int *my_vector = NULL;

	// declare and initialise the output variables
	int my_sum = 0;
	struct timespec start_time, end_time, time_diff;
	double runtime = 0.0;

	// creates a vector variable
	my_vector = malloc(num_arg * sizeof(int));

	// checks the memory allocation worked
	if (NULL == my_vector)
	{
		// raise an error
		fprintf(stderr, "ERROR: Could not allocate memory for vector.\n");

		// and exit completely
		exit(-1);
	}

	// initialises the vector non-trivially (element i = i + 1)
	// expected sum: n * (n + 1) / 2
	for (int i = 0; i < num_arg; i++)
	{
		my_vector[i] = i + 1;
	}

	// gets the time before summing the vector
	timespec_get(&start_time, TIME_UTC);

	// sums the vector
	my_sum = sum_vector(my_vector, num_arg);

	// gets the time after summing the vector
	timespec_get(&end_time, TIME_UTC);

	// calculates the runtime
	time_diff = calculate_runtime(start_time, end_time);
	runtime = to_second_float(time_diff);

	// prints the sum
	printf("Sum: %d\n", my_sum);

	// prints the runtime
	printf("Runtime for sum: %lf seconds.\n", runtime);

	// if we use malloc, must free when done
	free(my_vector);

	return 0;
}

// defines a function to sum a vector of ints into another int
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

// defines a function to initialise all values in a vector to a given initial value
void initialise_vector(int vector[], int size, int initial)
{
	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// sets the elements of the vector to the initial value
		vector[i] = initial;
	}
}

// defines a function to print a vector of ints
void print_vector(int vector[], int size)
{
	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// prints the elements of the vector to the screen
		printf("%d\n", vector[i]);
	}
}

// defines a function that checks your arguments to make sure they'll do what you need
int check_args(int argc, char **argv)
{
	// declare and initialise the numerical argument
	int num_arg = 0;

	// check the number of arguments
	if (2 == argc) // program name and numerical argument
	{
		// declare and initialise the numerical argument
		num_arg = atoi(argv[1]);
	}
	else // the number of arguments is incorrect
	{
		// raise an error
		fprintf(stderr, "ERROR: You did not provide a numerical argument!\n");
		fprintf(stderr, "Correct use: %s [NUMBER]\n", argv[0]);

		// and exit completely
		exit(-1);
	}
	return num_arg;
}

double to_second_float(struct timespec in_time)
{
	// creates and initialises the output variable
	double out_time = 0.0;

	// calculates the time in seconds
	out_time = (double) in_time.tv_sec + ((double) in_time.tv_nsec / 1e9);

	// returns the time as a double
	return out_time;
}

struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time)
{
	// creates and initialises the variables
	struct timespec time_diff;
	long int seconds, nanoseconds;

	seconds = end_time.tv_sec - start_time.tv_sec;
	nanoseconds = end_time.tv_nsec - start_time.tv_nsec;

	// if the ns part is negative
	if (nanoseconds < 0)
	{
		// carry the one
		seconds = seconds - 1;
		nanoseconds = nanoseconds + ((long int) 1e9);
	}

	// creates the runtime
	time_diff.tv_sec = seconds;
	time_diff.tv_nsec = nanoseconds;

	return time_diff;
}
