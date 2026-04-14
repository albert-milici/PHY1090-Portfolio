
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv)
{
    // creates and initialises the variables
    int i, input;
    char *file_name;
    FILE *output_file;
    struct timespec start_time, end_time, time_diff;
    double runtime;

    i = input = 0;
    file_name = NULL;
    output_file = NULL;
    runtime = 0.0;

    // checks if there are the right number of arguments
    if (3 == argc)
    {
        // converts the arguments
        input = atoi(argv[1]);
        file_name = argv[2];
    }
    else
    {
        // raises an error
        fprintf(stderr, "ERROR: Incorrect arguments.\n");
        fprintf(stderr, "Usage: %s [NUM] [FILE]\n", argv[0]);
        fprintf(stderr, "e.g. %s 1000 data/c_output.txt\n", argv[0]);

        // and crashes out
        exit(-1);
    }

    // opens the file for writing
    output_file = fopen(file_name, "w");

    if (NULL == output_file)
    {
        fprintf(stderr, "ERROR: Could not open file %s for writing.\n", file_name);
        exit(-1);
    }

    // gets the start time for the write
    timespec_get(&start_time, TIME_UTC);

    // writes the numbers to the file
    for (i = 0; i < input; i++)
    {
        fprintf(output_file, "%d\n", i);
    }

    // closes the file so buffered output is written
    fclose(output_file);

    // gets the end time for the write
    timespec_get(&end_time, TIME_UTC);

    // calculates the runtime
    time_diff = calculate_runtime(start_time, end_time);
    runtime = to_second_float(time_diff);

    // prints the runtime
    printf("Runtime for file write: %lf seconds.\n", runtime);

    return 0;
}

double to_second_float(struct timespec in_time)
{
    // creates and initialises the variables
    double out_time;

    out_time = 0.0;

    // calculates the time in seconds
    out_time = (double) in_time.tv_sec + ((double) in_time.tv_nsec / 1e9);

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
