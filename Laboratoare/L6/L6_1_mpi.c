#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
	int count, my_rank;

	//0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	int A[] = {0,0,0,0,0,0,0,0,3,7,8,3,9,2,3,1};
	int B[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//procesul myrank stie doar de elementul de pe pozitia myrank
	
	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &count); //determina numarul de procese
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank); //determina rank ul procesului
	MPI_Status status;
	int n = 16; //2 la a 4 a
	int m = 4;

	//prima etapa
	if (my_rank != 0) // nu e folosit
	{
	// conditia de frunza
		if(my_rank >= (n/2)) // toate nodurile frunza intra aici
		{
			MPI_Send(&A[my_rank], 1, MPI_INT, my_rank/2, 0, MPI_COMM_WORLD);
		}
		else
		{	
			int mess1, mess2;
			//receive x from id*2
			MPI_Recv(&mess1, 1, MPI_INT, my_rank * 2, 0, MPI_COMM_WORLD, &status);
			//receive y from id*2 +1
			MPI_Recv(&mess2, 1, MPI_INT, my_rank * 2 + 1, 0, MPI_COMM_WORLD, &status);
			
			//z = x compus cu y
			int z = mess1 + mess2;
			A[my_rank] = z;
			
			printf("I am %d and I received: %d - %d. Result: %d\n", my_rank, mess1, mess2, z) ;
			
			// z trebuite trimis in sus daca nu e radacina
			if(my_rank != 1)
			{
				//send to my_rank/2
				MPI_Send(&z, 1, MPI_INT, my_rank/2, 0, MPI_COMM_WORLD);
			}
			else
			{
				//print
				printf("I am %d and I received: %d - %d. Result: %d\n", my_rank, mess1, mess2, z) ;
			}
		}
	}
	
	// a doua etapa - calculul prefixelor
	if(my_rank != 0)
	{
		if(my_rank == 1)
		{
			B[my_rank] = A[my_rank];
			//send B[my_rank] to id*2, id * 2 + 1
			MPI_Send(&B[my_rank], 1, MPI_INT, my_rank * 2, 0, MPI_COMM_WORLD);
			MPI_Send(&B[my_rank], 1, MPI_INT, my_rank * 2 + 1, 0, MPI_COMM_WORLD);
		}
		else
		{	
			//receive B from id/2
			int b;
			MPI_Recv(&b, 1, MPI_INT, my_rank / 2, 0, MPI_COMM_WORLD, &status);
			
			if(my_rank % 2 == 0)
			{	
				int a;
				// primesc A de la id + 1
				MPI_Recv(&a, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, &status);
				B[my_rank] = b - a;
			}
			else
			{
				//send A[my_rank] to id-1
				MPI_Send(&A[my_rank], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
				B[my_rank] = b;
			}
				//verific sa nu fie frunza ca sa trimita in jos
			if(my_rank < (n/2)) 
			{
				//send B[my_rank] to id*2, id*2 + 1
				MPI_Send(&B[my_rank], 1, MPI_INT, my_rank * 2, 0, MPI_COMM_WORLD);
				MPI_Send(&B[my_rank], 1, MPI_INT, my_rank * 2 + 1, 0, MPI_COMM_WORLD);
			}
			else
			{
				//print(id, B[id]
				printf("my_rank = %d, B[my_rank] = %d \n", my_rank, B[my_rank]) ;
			}
			
		}
	}
	
	MPI_Finalize();
	return 0;
}
