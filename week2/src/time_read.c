#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv)
{
    // creates and initialises the variables
    char *file_name;
    FILE *input_file;
    char *file_contents;
    long int file_size;
    size_t bytes_read;
    struct timespec start_time, end_time, time_diff;
    double runtime;

    file_name = NULL;
    input_file = NULL;
    file_contents = NULL;
    file_size = 0;
    bytes_read = 0;
    runtime = 0.0;

    // checks if there are the right number of arguments
    if (2 == argc)
    {
        // gets the file name
        file_name = argv[1];
    }
    else
    {
        // raises an error
        fprintf(stderr, "ERROR: Incorrect arguments.\n");
        fprintf(stderr, "Usage: %s [FILE]\n", argv[0]);
        fprintf(stderr, "e.g. %s data/c_output.txt\n", argv[0]);

        // and crashes out
        exit(-1);
    }

    // opens the file for reading
    input_file = fopen(file_name, "rb");

    if (NULL == input_file)
    {
        fprintf(stderr, "ERROR: Could not open file %s for reading.\n", file_name);
        exit(-1);
    }

    // finds the file size
    fseek(input_file, 0L, SEEK_END);
    file_size = ftell(input_file);
    rewind(input_file);

    if (file_size < 0)
    {
        fprintf(stderr, "ERROR: Could not determine file size.\n");
        fclose(input_file);
        exit(-1);
    }

    // allocates memory for the file contents
    file_contents = malloc((size_t) file_size + 1);

    if (NULL == file_contents)
    {
        fprintf(stderr, "ERROR: Could not allocate memory for file contents.\n");
        fclose(input_file);
        exit(-1);
    }

    // gets the start time for the read
    timespec_get(&start_time, TIME_UTC);

    // reads the file into memory
    bytes_read = fread(file_contents, 1, (size_t) file_size, input_file);

    // gets the end time for the read
    timespec_get(&end_time, TIME_UTC);

    // closes the file
    fclose(input_file);

    // terminates the buffer as a string
    file_contents[bytes_read] = '\0';

    // calculates the runtime
    time_diff = calculate_runtime(start_time, end_time);
    runtime = to_second_float(time_diff);

    // prints the results
    printf("Read %zu bytes.\n", bytes_read);
    printf("Runtime for file read: %lf seconds.\n", runtime);

    // frees the memory
    free(file_contents);

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
