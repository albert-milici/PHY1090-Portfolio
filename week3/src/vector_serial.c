#include <stdio.h>
#include <stdlib.h>

// declares the functions that will be called within main
// note how declaration lines are similar to the initial line
// of a function definition, but with a semicolon at the end;
int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size, int initial);
void print_vector(int vector[], int size);
int sum_vector(int vector[], int size);

int main(int argc, char **argv)
{
	// declare and initialise the numerical argument variable
	int num_arg = check_args(argc, argv);

	// creates a vector variable
	// int my_vector[num_arg]; // suffers issues for large vectors
	int* my_vector = malloc (num_arg * sizeof(int));
	// and initialises every element to zero
	// initialises the vector non-trivially (element i = i+1)
    // expected sum: n*(n+1)/2
    for (int i = 0; i < num_arg; i++)
    {
        my_vector[i] = i + 1;
    }


	// TODO: put some code here that makes a more meaningful vector

	// sums the vector
	int my_sum = sum_vector(my_vector, num_arg);

	// prints the sum
	printf("Sum: %d\n", my_sum);

	// if we use malloc, must free when done!
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
		// sets the elements of the vector to the initial value
		sum += vector[i];
	}

	// returns the sum
	return sum;
}

// defines a function to initialise all values in a vector to a given inital value
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
	if (argc == 2) // program name and numerical argument
	{
		// declare and initialise the numerical argument
		num_arg = atoi(argv[1]);
	}
	else // the number of arguments is incorrect
	{
		// raise an error
		fprintf(stderr, "ERROR: You did not provide a numerical argument!\n");
		fprintf(stderr, "Correct use: %s [NUMBER]\n", argv[0]);

		// and exit COMPLETELY
		exit (-1);
	}
	return num_arg;
}
