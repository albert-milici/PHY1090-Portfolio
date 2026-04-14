#include <stdio.h>
#include <mpi.h>

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
	ierror = MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
	ierror = MPI_Comm_size(MPI_COMM_WORLD,&uni_size);

	// prints to the screen
	printf("Hello, I am %d of %d\n",my_rank, uni_size);

	// finalise MPI
	ierror = MPI_Finalize();
	return 0;
}
