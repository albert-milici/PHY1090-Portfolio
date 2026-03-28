#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// struct to hold parsed command line arguments
typedef struct {

	int points;
	int cycles;
	int samples;
	char output_path[256];
	
} Args;

// declares the functions that will be called within main
Args check_args(int argc, char **argv);
void initialise_vector(double vector[], int size, double initial);
void print_vector(double vector[], int size);
int sum_vector(int vector[], int size);
void update_positions(double* positions, int points, double time);
int generate_timestamps(double* time_stamps, int time_steps, double step_size);
double driver(double time);
void print_header(FILE** p_out_file, int points);

int main(int argc, char **argv)
{
	// parses command line arguments into a struct
	Args args = check_args(argc, argv);

	// calculates derived values from the arguments
	int time_steps = args.cycles * args.samples + 1;
	double step_size = 1.0 / args.samples;

	// creates a vector for the time stamps in the data
	double* time_stamps = (double*) malloc(time_steps * sizeof(double));
	initialise_vector(time_stamps, time_steps, 0.0);
	generate_timestamps(time_stamps, time_steps, step_size);

	// creates a vector variable for the current positions
	double* positions = (double*) malloc(args.points * sizeof(double));
	// and initialises every element to zero
	initialise_vector(positions, args.points, 0.0);

	// opens the output file at the user specified path
	FILE* out_file;
	out_file = fopen(args.output_path, "w");
	print_header(&out_file, args.points);

	// iterates through each time step in the collection
	for (int i = 0; i < time_steps; i++)
	{
		// updates the position using a function
		update_positions(positions, args.points, time_stamps[i]);

		// prints an index and time stamp
		fprintf(out_file, "%d, %lf", i, time_stamps[i]);

		// iterates over all of the points on the line
		for (int j = 0; j < args.points; j++)
		{
			// prints each y-position to a file
			fprintf(out_file, ", %lf", positions[j]);
		}
		// prints a new line
		fprintf(out_file, "\n");
	}

	// if we use malloc, must free when done!
	free(time_stamps);
	free(positions);

	// closes the file
	fclose(out_file);

	return 0;
}

// prints a header to the file
// double-pointer used to allow this function to move the file pointer
void print_header(FILE** p_out_file, int points)
{
	fprintf(*p_out_file, "#, time");
	for (int j = 0; j < points; j++)
	{
		fprintf(*p_out_file, ", y[%d]", j);
	}
	fprintf(*p_out_file, "\n");
}

// defines a simple harmonic oscillator as the driving force
double driver(double time)
{
	double value = sin(time*2.0*M_PI);
	return(value);
}

// defines a function to update the positions
void update_positions(double* positions, int points, double time)
{
	// creates a temporary vector variable for the new positions
	double* new_positions = (double*) malloc(points * sizeof(double));

	// initialises the index
	int i = 0;
	new_positions[i] = driver(time);
	// creates new positions by setting value of previous element 
	for (i = 1; i < points; i++)
	{
		new_positions[i] = positions[i-1];
	}
	// propagates these new positions to the old ones
	for (i = 0; i < points; i++)
	{
		positions[i] = new_positions[i];
	}

	// frees the temporary vector
	free(new_positions);
}

// defines a set of timestamps
int generate_timestamps(double* timestamps, int time_steps, double step_size)
{
	for (int i = 0; i < time_steps ; i++)
	{
		timestamps[i]=i*step_size;
	}	
	return time_steps;
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
void initialise_vector(double vector[], int size, double initial)
{
	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// sets the elements of the vector to the initial value
		vector[i] = initial;
	}
}

// defines a function to print a vector of ints
void print_vector(double vector[], int size)
{
	// iterates through the vector
	for (int i = 0; i < size; i++)
	{
		// prints the elements of the vector to the screen
		printf("%d, %lf\n", i, vector[i]);
	}
}

// parses command line arguments into a Args struct
Args check_args(int argc, char **argv)
{
	// creates and initialises the arguments struct
	Args args;
	args.points = 0;
	args.cycles = 0;
	args.samples = 0;
	memset(args.output_path, 0, sizeof(args.output_path));

	// check the number of arguments
	if (argc == 5) // program name, points, cycles, samples, output path
	{
		args.points = atoi(argv[1]);
		args.cycles = atoi(argv[2]);
		args.samples = atoi(argv[3]);
		strncpy(args.output_path, argv[4], sizeof(args.output_path) - 1);
	}
	else // the number of arguments is incorrect
	{
		// raise an error
		fprintf(stderr, "ERROR: Incorrect number of arguments!\n");
		fprintf(stderr, "Correct use: %s [POINTS] [CYCLES] [SAMPLES] [OUTPUT_PATH]\n", argv[0]);
		fprintf(stderr, "Example: %s 50 5 25 week5/output/string_wave.csv\n", argv[0]);

		// and exit COMPLETELY
		exit(-1);
	}
	return args;
}