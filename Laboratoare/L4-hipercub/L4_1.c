#include <stdio.h>
#include <string.h>
#include "mpi.h"


int main (int argc, char** argv){
	int count, myrank;
	int d = 3; // numarul de dimensiuni
	int destinatie = 0;
	int sursa = 0; //root-ul(de unde se pleaca)
	int masca = (1 << d) - 1; //ma ajuta sa gasesc care e bitul diferit
	char M[] = "Hello world";

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &count);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Status status;

	
	//nodurile legate de o latura difera doar print-un singur bit
	
	for(int k = d - 1; k >= 0; k--){ // 2 1 0 - indicii bitilor din numarul nodului(binar)
		masca = masca ^ (1 << k);
		// pt masca:
		// pozitia bitului care se face 0 din masca coincide cu pozitia bitului de 1 din destinatie, restul bitilor fiind la fel
		if((myrank & masca) == 0) //cine participa la comunicatie
		{
			if((myrank & (1 << k)) == 0) // cine trimite
			{
				destinatie = myrank ^ (1 << k); 
				// pentru ca transmisia se poate face doar intre nodurile care difera print-un singur bit, restul fiind la fel
				MPI_Send(M, strlen(M) + 1, MPI_CHAR, destinatie, 0, MPI_COMM_WORLD);
			}
			else // cine primeste
			{
				sursa = myrank ^ (1 << k);
				// tot la fel, cele 2 noduri difera printr-un singur bit
				MPI_Recv(M, 100, MPI_CHAR, sursa, 0, MPI_COMM_WORLD, &status);
				printf("%d - %d \n", sursa, myrank);
				fflush(stdout);
			}
		}
	}
	//
	printf("Eu sunt %d din %d\n", myrank, count);
	MPI_Finalize();
	return 0;
}

// La pasul k ce noduri participa?
// cele care au bitul de pe pozitia k diferit, iar restul bitilor la fel

// Cum determinam cine trimite si cine primeste?
// trimite cine are bitul de pe pozitia k egal cu 0 si primeste cine are bitul de pe pozitia k egal cu 1

// Cum determinam partenerul de comunicare?
// pe pozitia k are bitul negat
