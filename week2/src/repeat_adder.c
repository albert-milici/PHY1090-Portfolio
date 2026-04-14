#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) 
{
	// creates and initialises the variables
	int i, input, output, multiple;
	i = input = output = multiple = 0;

	// checks if there are the right number of arguments
	if (argc == 3)
	{
		// converts the first argument to an integer
		input = atoi(argv[1]);
		multiple = atoi(argv[2]);
	}
	else //(argc != 3)
	{
		// raises an error
		fprintf(stderr, "Incorrect arguments.  Usage: repeat_adder [NUM] [NUM]\ne.g. \n repeat_adder 2 3\n");
		// and crashes out
		exit(-1);
	}
	
	// iterates over all numbers up the input
	for (i = 0; i < input; i++)
	{
		// adds the index to the output
		output = output + multiple;
	}

	// prints the output
	printf("%d\n", output);
	return 0;
}
