#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
	int count, my_rank;
	
	//printf("Eu sunt %d din %d\n", myrank, count);
	
	int index[8] = {3, 5, 6, 10, 11, 12, 13, 14};
	int edges[14] = {2, 3, 4, 3, 7, 0, 0, 1, 5, 6, 0, 3, 3, 1};
	char M[] = "hello";
	char message[20];
	int nv;
	int i;
	MPI_Status status;
	MPI_Comm comm;
	
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &count);
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	
	MPI_Graph_create(MPI_COMM_WORLD, 8, index, edges, 0, &comm);
	
	MPI_Graph_neighbors_count(comm, my_rank, &nv);
	
	int *vecini = (int*)malloc(sizeof(int));
	
	MPI_Graph_neighbors(comm, my_rank, nv, vecini);
	
	int root = 5;
	
	if(my_rank == root)
	{
		for(i = 0; i < nv; i++)
			MPI_Send(M, strlen(M)+1, MPI_CHAR, vecini[i], 0, comm);
	}
	else
	{
		MPI_Recv(message, 100, MPI_CHAR, MPI_ANY_SOURCE, 0, comm, &status);
		printf ( " received: %s\n " , message) ;
		for(i = 0; i< nv; i++)
			if(status.MPI_SOURCE !=  vecini[i])
				MPI_Send(M, strlen(M)+1, MPI_CHAR, vecini[i], 0, comm);
	}
	
	
	MPI_Finalize();
	
	return 0;
}
