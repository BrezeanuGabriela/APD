#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
	int count, my_rank;
	int index[8] = {3, 5, 6, 10, 11, 12, 13, 14};
	int edges[14] = {2, 3, 4, 3, 7, 0, 0, 1, 5, 6, 0, 3, 3, 1};
	char M[] = "hello";
	char message[20];
	int nv; //numarul de vecini
	int i;
	MPI_Status status;
	MPI_Comm comm; //comunicatorul
	
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &count); //determina numarul de procese
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank); //determina rank ul procesului
	
	MPI_Graph_create(MPI_COMM_WORLD, 8, index, edges, 0, &comm);
	
	MPI_Graph_neighbors_count(comm, my_rank, &nv); // imi da numarul de vecini ai procesului curent
	
	int *vecini = (int*)malloc(sizeof(int));
	
	MPI_Graph_neighbors(comm, my_rank, nv, vecini); // imi da vecinii
	
	int root = 5;
	
	// radacina trimite mesaj tuturor copiilor
	if(my_rank == root)
	{
		for(i = 0; i < nv; i++)
			MPI_Send(M, strlen(M)+1, MPI_CHAR, vecini[i], 0, comm); // trimite la copii
	}
	else //nu e radacina
	{
		// primeste mesajul de la parinte
		MPI_Recv(message, 100, MPI_CHAR, MPI_ANY_SOURCE, 0, comm, &status);
		printf ( "I am %d and I received: %s from %d \n", my_rank, message, status.MPI_SOURCE) ;

		// trimite mai departe copiilor
		for(i = 0; i< nv; i++)
		{
			if(status.MPI_SOURCE !=  vecini[i]) //ca sa nu ii trimit tot celui de la care am primit mesajul, adica la parinte
			{
				MPI_Send(M, strlen(M)+1, MPI_CHAR, vecini[i], 0, comm);
			}
		}
	}
	
	
	MPI_Finalize();
	
	return 0;
}
