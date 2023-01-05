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
	
	int sum = 0; //e pt cui trimit
	
	for(i=0; i<nv; i++)
	{
		sum += vecini[i];
	}	

	printf("my_rank = %d, sum = %d\n", my_rank, sum);

	int ss = 0; //e pt ce primesc
	
	// conditia de frunza = nr de vecini este 1 si my_rank != root
	// frunza doar trimite la parinte
	if(nv == 1 && my_rank != root)
	{
		MPI_Send(&my_rank, 1, MPI_INT, vecini[0], 0, comm); //trimite la parinte
	}
	else //e nod interior
	{
		for(i=0; i < nv-1; i++) //asteapta mesajele de la frunze; -1 pt ca nu vreau sa primesc si de la parinte, lui ii trimit mesaj
		{
			// recv from MPI_ANY_SOURCE
			int mess;
			MPI_Recv(&mess, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm, &status);
			
			printf ( "I am %d and I received from %d; sum=%d \n", my_rank, status.MPI_SOURCE, sum);
			sum -= status.MPI_SOURCE; // tot scad de la cine am primit, iar la final in sum ramane parintele catre care trebuie sa trimita
			ss += mess; // adun ce am primit, iar ss e mesajul pe care il trimit mai sus la parinte
		}

		if(my_rank != root)
		{
			// send to sum - in sum a ramas parintele
			// in ss se aduna suma rank-urilor (aka de la children)
			int ceva1 = my_rank + ss;
			MPI_Send(&ceva1, 1, MPI_INT, sum, 0, comm);
		}
		else // e root
		{
			int mess;
			MPI_Recv(&mess, 1, MPI_INT, MPI_ANY_SOURCE, 0, comm, &status);
			ss += mess + my_rank;
			printf("I am %d and I received: %d\n", my_rank, ss) ;
		}
	}
	
	
	MPI_Finalize();
	
	return 0;
}
