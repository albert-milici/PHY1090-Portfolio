#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
#include <time.h>

// struct to hold parsed command line arguments
typedef struct {
	int points;
	int cycles;
	int samples;
	char output_path[256];
} Args;

// function declarations
Args check_args(int argc, char **argv);
void check_uni_size(int uni_size);
void initialise_vector(double vector[], int size, double initial);
int generate_timestamps(double* time_stamps, int time_steps, double step_size);
double driver(double time);
void print_header(FILE** p_out_file, int points);
void update_positions(double* positions, int chunk, double boundary, int my_rank, double time);

int main(int argc, char **argv)
{
	// declare and initialise error handling variable
	int ierror = 0;

	// parses command line arguments into a struct
	Args args = check_args(argc, argv);

	// initialise MPI
	ierror = MPI_Init(&argc, &argv);

	// declare and initialise rank and size variables
	int my_rank, uni_size;
	my_rank = uni_size = 0;

	// gets the rank and world size
	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

	// checks the universe size is correct
	check_uni_size(uni_size);

	// calculates derived values from the arguments
	int time_steps = args.cycles * args.samples + 1;
	double step_size = 1.0 / args.samples;

	// creates timestamps (all ranks need these for the driver)
	double* time_stamps = (double*) malloc(time_steps * sizeof(double));
	initialise_vector(time_stamps, time_steps, 0.0);
	generate_timestamps(time_stamps, time_steps, step_size);

	// calculates chunk size for each process
	int chunk = args.points / uni_size;

	// allocates local positions array for this rank's chunk
	double* local_positions = (double*) malloc(chunk * sizeof(double));
	initialise_vector(local_positions, chunk, 0.0);

	// root allocates full array for gathering and opens the output file
	double* full_positions = NULL;
	FILE* out_file = NULL;
	if (0 == my_rank)
	{
		full_positions = (double*) malloc(args.points * sizeof(double));
		out_file = fopen(args.output_path, "w");
		print_header(&out_file, args.points);
	}

	// creates communication variables
	int tag = 0;
	MPI_Status status;
	double boundary = 0.0;

	// start timing
	struct timespec start_time, end_time;
	timespec_get(&start_time, TIME_UTC);

	// iterates through each time step
	for (int i = 0; i < time_steps; i++)
	{
		// sends last element to next rank for boundary
		if (my_rank < uni_size - 1)
		{
			MPI_Send(&local_positions[chunk - 1], 1, MPI_DOUBLE,
				my_rank + 1, tag, MPI_COMM_WORLD);
		}

		// receives boundary value from previous rank
		if (my_rank > 0)
		{
			MPI_Recv(&boundary, 1, MPI_DOUBLE,
				my_rank - 1, tag, MPI_COMM_WORLD, &status);
		}

		// updates positions for this timestep
		update_positions(local_positions, chunk, boundary, my_rank, time_stamps[i]);

		// gathers all local chunks to root in rank order
		MPI_Gather(local_positions, chunk, MPI_DOUBLE,
			full_positions, chunk, MPI_DOUBLE, 0, MPI_COMM_WORLD);

		// root writes the gathered data to file
		if (0 == my_rank)
		{
			// prints an index and time stamp
			fprintf(out_file, "%d, %lf", i, time_stamps[i]);

			// iterates over all of the points on the line
			for (int j = 0; j < args.points; j++)
			{
				// prints each y-position to a file
				fprintf(out_file, ", %lf", full_positions[j]);
			}
			// prints a new line
			fprintf(out_file, "\n");
		}
	}

	// finish timing
	timespec_get(&end_time, TIME_UTC);
	long int seconds = end_time.tv_sec - start_time.tv_sec;
	long int nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
	if (nanoseconds < 0) { seconds--; nanoseconds += 1000000000L; }
	double elapsed = seconds + nanoseconds / 1e9;
	// only get the root time
	if (0 == my_rank)
	{
    printf("Elapsed: %.9lf s\n", elapsed);
	}


	// frees allocated memory
	free(time_stamps);
	free(local_positions);
	if (0 == my_rank)
	{
		free(full_positions);
		fclose(out_file);
	}

	// finalise MPI
	ierror = MPI_Finalize();
	return 0;
}

// defines a function to update the positions
void update_positions(double* positions, int chunk, double boundary, int my_rank, double time)
{
	// creates a temporary vector variable for the new positions
	double* new_positions = (double*) malloc(chunk * sizeof(double));

	// updates first element of chunk
	if (0 == my_rank)
	{
		// rank 0 drives point 0 with the sine function
		new_positions[0] = driver(time);
	}
	else
	{
		// other ranks use the boundary value from previous chunk
		new_positions[0] = boundary;
	}

	// updates remaining elements (each takes previous value of left neighbour)
	for (int j = 1; j < chunk; j++)
	{
		new_positions[j] = positions[j - 1];
	}

	// copies new positions back to positions
	for (int j = 0; j < chunk; j++)
	{
		positions[j] = new_positions[j];
	}

	// frees the temporary array
	free(new_positions);
}

// prints a header to the file
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

// defines a set of timestamps
int generate_timestamps(double* timestamps, int time_steps, double step_size)
{
	for (int i = 0; i < time_steps ; i++)
	{
		timestamps[i]=i*step_size;
	}
	return time_steps;
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

// checks there are enough processes to run
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

// parses command line arguments into an Args struct
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