#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>

int main (int argc, char** argv){
	int id;

	int d=2; // nr de dimensiuni

	int A[2][2] = {
			{3, 2},
			{0, 1}
			};

	int B[2][2] = {
			{1, 2},
			{4, 1}
			};
	
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	int i = id/d;
	int j = id%d;

	int m1 = A[i][j];
	int m2 = B[i][j];

	int q = 4; // radical din nr de procese
	int procese = 16;

	for(int k = 0; k<i; k++)
	{
		//shiftez pe linii la mat a cu k poz
		MPI_Send(&m1, 1, MPI_INT, i*d+(j-1+d)%d, 0, MPI_COMM_WORLD);
		MPI_Recv(&m1, 1, MPI_INT, i*d+(j+1)%d, 0, MPI_COMM_WORLD, &status);
	}

	for(int k = 0; k<j; k++)
	{
		//shiftez pe linii la mat a cu k poz
		MPI_Send(&m2, 1, MPI_INT, ((i-1+d)%d)*d+j, 0, MPI_COMM_WORLD);
		MPI_Recv(&m2, 1, MPI_INT, ((i+1)%d)*d+j, 0, MPI_COMM_WORLD, &status);
	}

	int m3=0;

	for(int k = 0; k<d-1; k++)
	{
		m3=m3+m1*m2;
		MPI_Send(&m1, 1, MPI_INT, i*d+(j-1+d)%d, 0, MPI_COMM_WORLD);
		MPI_Recv(&m1, 1, MPI_INT, i*d+(j+1)%d, 0, MPI_COMM_WORLD, &status);
		MPI_Send(&m2, 1, MPI_INT, ((i-1+d)%d)*d+j, 0, MPI_COMM_WORLD);
		MPI_Recv(&m2, 1, MPI_INT, ((i+1)%d)*d+j, 0, MPI_COMM_WORLD, &status);
	
	}
	
	int c = m3+m1*m2;
	printf("c[%d][%d] = %d \n", i, j, c);
	MPI_Finalize();
	return 0;
}