// L11
// liderul e procesul cu rangul cel mai mare

#include <stdio.h>
#include "mpi.h"
#include <string.h>
#include <stdlib.h>

#define ID_NOU 1
#define LIDER 2

int main (int argc, char** argv){
	int my_rank;
	int stare = 0; //adormit; 1-treaz
	int statut =0; //non-lider, 1-lider
	int lider = 0;
	int id=0;
	
	//un vector cu vecinii din dreapta
	int vecini_dr[] = {2, 3, 5, 4, 0, 1};

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	//my_rank == idi
	// fiecare proces face la inceput un send in dreapta
	MPI_Send(&my_rank, 1, MPI_INT, vecini_dr[my_rank], ID_NOU, MPI_COMM_WORLD);

	while(1 == 1)
	{
		// merge cu any_source pentru ca oricum primesc doar din dreapta
		MPI_Recv(&id, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		// primesc pe canalul 1 un id nou
		if(status.MPI_TAG == 1)
		{	
			// daca e egal cu my_rank inseamna ca eu sunt lider
			if(id == my_rank)
			{	
				statut = 1;
				lider = my_rank;
				MPI_Send(&lider, 1, MPI_INT, vecini_dr[my_rank], LIDER, MPI_COMM_WORLD);
				break;
			}
			// daca e mai mare, trimit la vecinul din dreapta id-ul primit
			if(id > my_rank)
			{
				MPI_Send(&id, 1, MPI_INT, vecini_dr[my_rank], ID_NOU, MPI_COMM_WORLD);
			}
		}
		else
		{

		}
		// primesc pe canalul de lideri
		if(status.MPI_TAG == 2)
		{
			statut = 0;
			lider = id;
			MPI_Send(&lider, 1, MPI_INT, vecini_dr[my_rank], LIDER, MPI_COMM_WORLD);
			break;
		}
	}
	printf("Procesul %d - lider %d\n", my_rank, lider );

	MPI_Finalize();
	return 0;
}