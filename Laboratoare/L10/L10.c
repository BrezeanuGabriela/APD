#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>

int main (int argc, char** argv){
	int id;

	double A[3][3] = {
			{1, 2, 1},
			{2, 1, 4},
			{1, 1, 7}
			};

	double B[3] = {1, 3, 2};
	
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	int n =3;
	int k,j,i;

	//for(k=0; k<=n-1;k++)
	//{

	k = id;
	for(j=k+1;j<=n-1;j++)
	{
		A[k][j] = A[k][j] / A[k][k];
		for(i=k+1; i<=n-1;i++)
		{
			MPI_Send(&A[k][i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
	}

	B[k] = B[k] / A[k][k];

	A[k][k] = 1;

	for(j=k+1;j<=n-1;j++)
	{
		MPI_Send(&B[k], 1, MPI_INT, j, 0, MPI_COMM_WORLD);
	}

	for(i=0;i<k;i++)
	{
		for(j=i+1;j<n;j++)
		{
			MPI_Recv(&A[i][j], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			A[k][j] = A[k][j] - A[k][i] * A[i][j];
		}
		MPI_Recv(&B[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
		B[k] = B[k] - A[k][i] * B[i];
		A[k][i] = 0;
	}


	//for(i=0;i<n;i++)
	//{
		printf("%f\n", B[k]);
	//}



	MPI_Finalize();
	return 0;
}