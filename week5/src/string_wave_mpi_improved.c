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

	// physical variables
	double k;
	double m;
	double damping;
	double length;

} Args;

// function declarations
Args check_args(int argc, char **argv);
void check_uni_size(int uni_size);
void initialise_vector(double vector[], int size, double initial);

void update_positions(double* positions, double* velocities, int chunk,
	double dt, double k, double m, double dx, double damping,
	double left_boundary, double right_boundary);

int generate_timestamps(double* time_stamps, int time_steps, double step_size);
double driver(double time);
void print_header(FILE** p_out_file, int points);

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

	// creates a 1D non-periodic Cartesian topology
	int dims = uni_size;
	int periods = 0;
	int reorder = 0;
	MPI_Comm cart_comm;
	MPI_Cart_create(MPI_COMM_WORLD, 1, &dims, &periods, reorder, &cart_comm);

	// finds left and right neighbours using Cart_shift
	int left_rank, right_rank;
	MPI_Cart_shift(cart_comm, 0, 1, &left_rank, &right_rank);

	// calculates derived values from the arguments
	int time_steps = args.cycles * args.samples + 1;
	double step_size = 1.0 / args.samples;

	// physical variables
	double k = args.k;
	double m = args.m;
	double length = args.length;
	double dx = length / (args.points - 1);

	// creates timestamps (all ranks need these for the driver)
	double* time_stamps = (double*) malloc(time_steps * sizeof(double));
	initialise_vector(time_stamps, time_steps, 0.0);
	generate_timestamps(time_stamps, time_steps, step_size);

	// calculates chunk size for each process
	int chunk = args.points / uni_size;

	// allocates local arrays for this rank's chunk
	double* local_positions = (double*) malloc(chunk * sizeof(double));
	double* local_velocities = (double*) malloc(chunk * sizeof(double));
	initialise_vector(local_positions, chunk, 0.0);
	initialise_vector(local_velocities, chunk, 0.0);

	// root allocates full array for gathering and opens the output file
	double* full_positions = NULL;
	FILE* out_file = NULL;
	if (0 == my_rank)
	{
		full_positions = (double*) malloc(args.points * sizeof(double));
		out_file = fopen(args.output_path, "w");
		print_header(&out_file, args.points);
	}

	// creates boundary variables
	double left_boundary = 0.0;
	double right_boundary = 0.0;
	MPI_Status status;

	// start timing
	struct timespec start_time, end_time;
	timespec_get(&start_time, TIME_UTC);

	// iterates through each time step
	for (int i = 0; i < time_steps; i++)
	{
		// sets driven end and pinned end BEFORE update
		if (0 == my_rank)
		{
			local_positions[0] = driver(time_stamps[i]);
			local_velocities[0] = 0.0;
		}
		if (my_rank == uni_size - 1)
		{
			local_positions[chunk - 1] = 0.0;
			local_velocities[chunk - 1] = 0.0;
		}

		// bidirectional boundary exchange
		MPI_Sendrecv(&local_positions[chunk - 1], 1, MPI_DOUBLE, right_rank, 0,
			&left_boundary, 1, MPI_DOUBLE, left_rank, 0,
			cart_comm, &status);
		MPI_Sendrecv(&local_positions[0], 1, MPI_DOUBLE, left_rank, 1,
			&right_boundary, 1, MPI_DOUBLE, right_rank, 1,
			cart_comm, &status);

		// updates positions using spring forces
		update_positions(local_positions, local_velocities, chunk,
			step_size, k, m, dx, args.damping,
			left_boundary, right_boundary);

		// re-applies driven end and pinned end AFTER update
		if (0 == my_rank)
		{
			local_positions[0] = driver(time_stamps[i]);
			local_velocities[0] = 0.0;
		}
		if (my_rank == uni_size - 1)
		{
			local_positions[chunk - 1] = 0.0;
			local_velocities[chunk - 1] = 0.0;
		}

		// gathers all local chunks to root in rank order
		MPI_Gather(local_positions, chunk, MPI_DOUBLE,
			full_positions, chunk, MPI_DOUBLE, 0, cart_comm);

		// root writes the gathered data to file
		if (0 == my_rank)
		{
			fprintf(out_file, "%d, %lf", i, time_stamps[i]);
			for (int j = 0; j < args.points; j++)
			{
				fprintf(out_file, ", %lf", full_positions[j]);
			}
			fprintf(out_file, "\n");
		}
	}

	// finish timing
	timespec_get(&end_time, TIME_UTC);
	long int seconds = end_time.tv_sec - start_time.tv_sec;
	long int nanoseconds = end_time.tv_nsec - start_time.tv_nsec;
	if (nanoseconds < 0) { seconds--; nanoseconds += 1000000000L; }
	double elapsed = seconds + nanoseconds / 1e9;
	if (0 == my_rank)
	{
		printf("Elapsed: %.9lf s\n", elapsed);
	}

	// frees allocated memory
	free(time_stamps);
	free(local_positions);
	free(local_velocities);
	if (0 == my_rank)
	{
		free(full_positions);
		fclose(out_file);
	}

	// frees the Cartesian communicator and finalises MPI
	MPI_Comm_free(&cart_comm);
	ierror = MPI_Finalize();
	return 0;
}

// updates positions using spring forces between neighbouring points
void update_positions(double* positions, double* velocities, int chunk,
	double dt, double k, double m, double dx, double damping,
	double left_boundary, double right_boundary)
{
	// computes all accelerations first before updating anything
	double* accelerations = (double*) malloc(chunk * sizeof(double));

	for (int i = 0; i < chunk; i++)
	{
		// gets left neighbour
		double left;
		if (0 == i)
		{
			left = left_boundary;
		}
		else
		{
			left = positions[i - 1];
		}

		// gets right neighbour
		double right;
		if (i == chunk - 1)
		{
			right = right_boundary;
		}
		else
		{
			right = positions[i + 1];
		}

		// spring force with damping
		accelerations[i] = (k / m) * (left - 2.0 * positions[i] + right) / (dx * dx) - damping * velocities[i];
	}

	// updates all velocities then all positions
	for (int i = 0; i < chunk; i++)
	{
		velocities[i] += accelerations[i] * dt;
		positions[i] += velocities[i] * dt;
	}

	// frees the temporary array
	free(accelerations);
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
	for (int i = 0; i < size; i++)
	{
		vector[i] = initial;
	}
}

// checks there are enough processes to run
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

// parses command line arguments into an Args struct
Args check_args(int argc, char **argv)
{
	Args args;
	args.points = 0;
	args.cycles = 0;
	args.samples = 0;
	memset(args.output_path, 0, sizeof(args.output_path));

	// default physical parameters
	args.k = 2.0;
	args.m = 0.5;
	args.damping = 0.5;
	args.length = 50.0;

	if (argc >= 5 && argc <= 9)
	{
		args.points = atoi(argv[1]);
		args.cycles = atoi(argv[2]);
		args.samples = atoi(argv[3]);
		strncpy(args.output_path, argv[4], sizeof(args.output_path) - 1);

		// optional physical parameters
		if (argc >= 6) { args.k = atof(argv[5]); }
		if (argc >= 7) { args.m = atof(argv[6]); }
		if (argc >= 8) { args.damping = atof(argv[7]); }
		if (argc >= 9) { args.length = atof(argv[8]); }
	}
	else
	{
		fprintf(stderr, "ERROR: Incorrect number of arguments!\n");
		fprintf(stderr, "Correct use: %s [POINTS] [CYCLES] [SAMPLES] [OUTPUT_PATH] [K] [M] [DAMPING] [LENGTH]\n", argv[0]);
		fprintf(stderr, "Example: %s 500 20 400 output.csv 2.0 0.5 0.5 50.0\n", argv[0]);
		fprintf(stderr, "Physical parameters are optional and default to k=2.0 m=0.5 damping=0.5 length=50.0\n");
		exit(-1);
	}
	return args;
}