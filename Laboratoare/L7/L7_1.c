#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
  
int main(int argc, char* argv[])
{
	int i, j;
 	int A[] = {3,1,5,2};

 	int size_A = 4;
 	int nr_linii_R = 2 * size_A - 1;

 	int* R[nr_linii_R];
 	for(i = 0; i < 7; i++)
 	{
 		R[i] = (int*)malloc(4 * sizeof(int));
 	}

 	int P[4];
 	int n =4; // numarul de elemente de sortat din vector
 	int Results[4];
 	
  	
  	// n fire, n=nr de elemente
  	// for ul de mai jos de fapt face 4 thread-uri
	#pragma omp parallel for private (i) // pentru ca fiecare thread sa fie cu i-ul lui (nu e chiar shared)
  	
  	// cate un fir de executie pentru fiecare element al for-ului
  	for(j = 0; j < n; j++)
  	{
  	// ce e mai jos face fiecare fir de executie
  		// se populeaza mai intai a doua jumatate a matricei R
  		for(i = 0; i < n; i++) 
  		{	
  			R[i+n-1][j] = (A[i] < A[j] ? 1: 0);
  		}
  	}
  	
  	#pragma omp parallel for private(i)
  	// fiecare thread corespunde unui j
  	for(j = 0; j < n; j++) {
  		int suma = 0;
  		// facem sumele pe coloana in prima jumatate - 1 (n - 1) a matricei R // R are 2*n -1 linii
  		for(i = (int)nr_linii_R / 2 - 1; i >= 0; i--)
  		{
  			// adunam elementele de pe aceeasi coloana de pe liniile 2 * i +1 si +2
  			R[i][j] = R[2*i+1][j] + R[2*i+2][j];
  		}
  		
  		P[j]=R[0][j];
  	}
  	
  	
  	#pragma omp parallel
	for(j = 0; j < n; j++) 
	{
		//printf("%d ", P[j]);
		Results[P[j]] = A[j];
	}
	
	for(j=0;j<=n-1;j++) {
		printf("%d ", Results[j]);
		//A[P[j]] = A[j];
	}
	
	return 0;
}
