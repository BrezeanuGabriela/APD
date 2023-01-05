#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
  
int main(int argc, char* argv[])
{
	int nthreads, tid, x=0;
 	int A[] = {0,0,0,0,0,0,0,0,3,7,8,3,9,2,3,1};
 	int B[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
 	
 	// dimensiunea lui a e 2 ^ m
 	// daca noi avem 8 valori in A, avem nevoie de fapt de 16 elem, 
 	// iar in ultimele 8 sunt valorile initiale
  	//omp_set_num_threads(16);
  	int m = 3; // imi zice cate etape am
  	int k, j;
  	
	// Begin of parallel region
	// k e 2, 1, 0
	for(k = m - 1; k>=0; k--)
	{
		//initial am 4 threads, dupa 2, dupa 1
		#pragma omp parallel for num_threads(1<<k)
		// fiecare thread executa for-ul
		// for(j=pow(2, k); j<pow(2, k+1); j++)
		for(j = (1<<k); j<= (1<<(k+1))-1;j++)
    	{
    		A[j] = A[2*j] + A[2*j+1];
    	}	
    }// acolada => toate firele trebuie sa isi termine executia ca sa se inainteze
    	
    //calcul prefixe
    B[1] = A[1];
    
    // k e 1, 2, 3
    for(k=1; k<=m;k++)
    {
    	#pragma omp parallel for num_threads(1<<k)
    	// for(j=pow(2, k); j<pow(2, k+1); j++)
    	for(j=(1<<k); j<= (1<<(k+1))-1;j++)
    	{
    		if(j % 2 == 1)
    		{
    			B[j] = B[(j-1)/2];
    		}
    		else
    		{
    			B[j] = B[j/2] - A[j+1];
    		}
    	}
    }
    	
    int i;
    for (i = 0; i < 16; i++) 
    {     
       	printf("A[%d] = %d \n", i,A[i]);     
    }      
    
    printf("\n--------------");
    	
    for (i = 0; i < 16; i++) 
    {     
       	printf("B[%d] = %d \n", i,B[i]);     
    } 

	return 0;
}
