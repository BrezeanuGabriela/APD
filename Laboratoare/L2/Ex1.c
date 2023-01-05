#include <stdio.h>
#include "mpi.h"

int main(int argc, char **argv)
{
	int count, myrank;
	
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &count);
	MPI_Comm_rank (MPI_COMM_WORLD, &myrank);
	
	printf("Eu sunt %d din %d\n", myrank, count);
	
	MPI_Finalize();
	
	return 0;
}
