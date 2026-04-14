
import sys
import time


def main():
    # checks if there are the right number of arguments
    try:
        # converts the arguments
        in_arg = int(sys.argv[1])
        file_name = sys.argv[2]
    except (IndexError, ValueError):
        # raises an error
        raise Exception(
            "Incorrect arguments.\n"
            "Usage: python3 src/time_write_file.py [NUM] [FILE]\n"
            "e.g. python3 src/time_write_file.py 1000 data/py_output.txt"
        )

    # gets the start time for the write
    start_time = time.time()

    # writes the numbers to the file
    with open(file_name, "w") as output_file:
        for i in range(in_arg):
            output_file.write("{}\n".format(i))

    # gets the end time for the write
    end_time = time.time()

    # gets the total time
    run_time = end_time - start_time

    # prints the runtime
    print("Runtime for file write: {} seconds".format(run_time))


if __name__ == "__main__":
    main()
