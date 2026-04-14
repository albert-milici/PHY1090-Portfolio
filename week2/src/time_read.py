import sys
import time


def main():
    # checks if there are the right number of arguments
    try:
        # gets the file name
        file_name = sys.argv[1]
    except IndexError:
        # raises an error
        raise Exception(
            "Incorrect arguments.\n"
            "Usage: python3 src/time_read_file.py [FILE]\n"
            "e.g. python3 src/time_read_file.py data/py_output.txt"
        )

    # gets the start time for the read
    start_time = time.time()

    # reads the file into memory
    with open(file_name, "rb") as input_file:
        file_contents = input_file.read()

    # gets the end time for the read
    end_time = time.time()

    # gets the total time
    run_time = end_time - start_time

    # prints the results
    print("Read {} bytes.".format(len(file_contents)))
    print("Runtime for file read: {} seconds".format(run_time))


if __name__ == "__main__":
    main()
