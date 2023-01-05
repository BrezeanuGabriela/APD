#include <stdio.h>
#include <string.h>
#include "mpi.h"


int main (int argc, char** argv){
	int d = 3;
	int destinatie = 0;
	int sursa = 0;
	int masca = (1 << d) - 1;
	int s = 3;
	char M[] = "Hello world";
	int count, myrank;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Status status;

	int id_virtual = myrank ^ s; // pt root(s) id_virtual =3
	

	for(int k = d - 1; k >= 0; k--){
		masca = masca ^ (1 << k);
		// pozitia bitului care se face 0 din masca coincide cu cu pozitia bitului de 1 din destinatie, restul bitilor fiind la fel
		if((id_virtual & masca) == 0)
		{
			if((id_virtual & (1 << k)) == 0)
			{
				destinatie = id_virtual ^ (1 << k);
				//s ma ajuta sa refac numarul nodului destinatie
				MPI_Send(M, strlen(M) + 1, MPI_CHAR, destinatie ^ s, 0, MPI_COMM_WORLD);
			}
			else
			{
				sursa = id_virtual ^ (1 << k);
				//s ma ajuta sa refac numarul nodului sursa
				MPI_Recv(M, 100, MPI_CHAR, sursa ^ s, 0, MPI_COMM_WORLD, &status);
				printf("%d - %d \n", sursa ^ s, myrank);
				fflush(stdout);
			}
		}
	}
	//
	printf("Eu sunt %d din %d\n", myrank, count);
	MPI_Finalize();
	return 0;
}
