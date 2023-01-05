#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int count, my_rank;

	int d = 4;
	int A[16] = {10,20,5,9,3,8,12,14,90,0,60,40,23,35,95,18};

	MPI_Init (&argc, &argv);
	MPI_Comm_size (MPI_COMM_WORLD, &count); //determina numarul de procese
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank); //determina rank ul procesului
	MPI_Status status;

	int id = my_rank;
	int f, k;

	for(f = 1; f <= d; ++f)
	{
		for(k = f; k >= 1; --k)
		{
			int alpha = (id >> f) & 1;
			int betha = (id >> (k-1)) & 1;
			int partener = id ^ (1 << (k-1));
			//printf("faza %d, eu sunt %d si partener = %d\n", f, my_rank, partener);
			int val;

			MPI_Send(&A[my_rank], 1, MPI_INT, partener, 0, MPI_COMM_WORLD);
			MPI_Recv(&val, 1, MPI_INT, partener, 0, MPI_COMM_WORLD, &status);
			
			//printf("faza %d : %d - %d\n", f,my_rank, val);
			if(alpha == betha)
			{
				A[my_rank] = (A[id] < val ? A[id] : val); 
			}
			else
			{
				A[my_rank] = (A[id] > val ? A[id] : val);
			}
		}
	}

	printf("In procesul %d : %d\n", my_rank, A[my_rank]);

	MPI_Finalize();
	return 0;
}
