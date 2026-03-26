#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// function declarations
void root_task(int my_rank, int uni_size);
void client_task(int my_rank, int uni_size);
void check_uni_size(int my_rank, int uni_size);
void check_task(int my_rank, int uni_size);

int main(int argc, char **argv) 
{
	// declare and initialise error handling variable
	int ierror = 0;
	
	// declare and initialise rank and size varibles
	int my_rank, uni_size;
	my_rank = uni_size = 0;

	// intitalise MPI
	ierror = MPI_Init(&argc, &argv);

	// gets the rank and world size
	ierror = MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD, &uni_size);

	// checks the universe size is correct
	check_uni_size(my_rank, uni_size);

	// checks what task to do and does it
	check_task(my_rank, uni_size);

	// finalise MPI
	ierror = MPI_Finalize();
	return 0;
}

void root_task(int my_rank, int uni_size)
{
	// creates and initialises transmission variables
	int recv_message, count, source, tag;
	recv_message = source = tag = 0;
	count = 1;
	MPI_Status status;

	// iterates through all the other ranks
	for (int their_rank = 1; their_rank < uni_size; their_rank++)
	{
		// sets the source argument to the rank of the sender
		source = their_rank;

		// receives the messages
		MPI_Recv(&recv_message, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

		// prints the message from the sender
		printf("Hello, I am %d of %d. Received %d from Rank %d\n",
				my_rank, uni_size, recv_message, source);
	} // end for (int their_rank = 1; their_rank < uni_size; their_rank++)
}

void client_task(int my_rank, int uni_size)
{
	// creates and initialises transmission variables
	int send_message, count, dest, tag;
	send_message = dest = tag = 0;
	count = 1;

	// request handle for non-blocking send
	MPI_Request request;
	MPI_Status status;

	// sets the destination for the message
	dest = 0; // destination is root

	// creates the message
	send_message = my_rank * 10;

	// sends the message non-blocking
	MPI_Isend(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD, &request);

	// waits for the send to complete before using the buffer again
	MPI_Wait(&request, &status);

	// prints the message from the sender
	printf("Hello, I am %d of %d. Sent %d to Rank %d using Isend\n",
			my_rank, uni_size, send_message, dest);
}

void check_uni_size(int my_rank, int uni_size)
{
	// sets the minimum uni size
	int min_uni_size = 2;

	// checks there are sufficient tasks to communicate with
	if (uni_size >= min_uni_size)
	{
		return;
	} // end if (uni_size >= min_uni_size)
	else // i.e. uni_size < min_uni_size
	{
		// raise an error
		printf("Unable to communicate with less than 2 processes. MPI communicator size = %d\n", uni_size);

		// and exit
		exit(-1);
	}
}

void check_task(int my_rank, int uni_size)
{
	// checks which process is running and calls the appropriate task
	if (0 == my_rank)
	{
		root_task(my_rank, uni_size);
	} // end if (0 == my_rank)
	else // i.e. (0 != my_rank)
	{
		client_task(my_rank, uni_size);
	} // end else // i.e. (0 != my_rank)
}