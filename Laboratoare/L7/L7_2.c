#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include <stdlib.h>


int main (int argc, char** argv)
{
	int my_rank;
	//vectorul intrare
	int A[7] = {3,2,3,8,5,6,4};
	int n=7;
	//ce primesc
	int mesaj;
	MPI_Status status;
	

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	for( int faza = 1; faza <= n; ++faza)
	{
		// daca faza e para, participa procesele de la 0 pana la final
		if(faza%2 == 1 && my_rank >=0 && my_rank <= 2*(n/2) - 1)
		{
			if(my_rank%2 == 0)
			{
				// send/recv la vecinul din dreapta (+1)
				MPI_Send(&A[my_rank], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
				MPI_Recv(&mesaj, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, &status);
				
				printf("my_rank =  %d - faza impara \n", my_rank);
				// memoreaza minim
				if(A[my_rank] > mesaj)
				{
					A[my_rank] = mesaj;
				}
			}
			else
			{
				MPI_Send(&A[my_rank], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
				MPI_Recv(&mesaj, 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD, &status);
				
				// memoreaza maxim
				if(A[my_rank] < mesaj)
				{
					A[my_rank] = mesaj;
				}
			}
		
		}
		// daca faza e impara participa procesele de la 1 pana la n-1-1
		if(faza%2 == 0 && my_rank >=1 && my_rank <= 2*((n-1)/2))
		{
			if(my_rank%2 == 1)
			{
				MPI_Send(&A[my_rank], 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD);
				MPI_Recv(&mesaj, 1, MPI_INT, my_rank + 1, 0, MPI_COMM_WORLD, &status);
				
				// memoreaza minim
				if(A[my_rank] > mesaj)
				{
					A[my_rank] = mesaj;
				}	
			}
			else
			{
				MPI_Send(&A[my_rank], 1, MPI_INT, my_rank - 1, 0, MPI_COMM_WORLD);
				MPI_Recv(&mesaj, 1, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD, &status);
				
				printf("my_rank =  %d - faza para \n", my_rank);
				
				// memoreaza maxim
				if(A[my_rank] < mesaj)
				{
					A[my_rank] = mesaj;
				}
			}
		
		}
		
	}
	
	printf("In procesul %d : %d\n", my_rank, A[my_rank]);
	
	MPI_Finalize();
	return 0;
}

