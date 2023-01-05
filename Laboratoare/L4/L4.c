#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char *argv[] )
{
    int errs = 0, i, k;
    int noNodes;
    int *index, *edges, *outindex, *outedges;
    MPI_Comm comm1, comm2;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &wsize );

	// citesc nodurile
	//citesc edges
	
    // create index and edges
        index = (int*)malloc(noNodes * sizeof(int) );
        edges = (int*)malloc(noNodes * 2 * sizeof(int) );
        if (!index || !edges) {
            printf( "Unable to allocate memory for index or edges\n");
            fflush(stdout);
            MPI_Abort( MPI_COMM_WORLD, 1 );
        }
        
        index[0] = 3;
        for (i=1; i< noNodes; i++) {
            index[i] = 3 + index[i-1];
        }
        k=0;
        for (i=0; i<wsize; i++) {
            edges[k++] = (i-1+wsize) % wsize;
            edges[k++] = (i+1) % wsize;
        }
        
        
        MPI_Graph_create( MPI_COMM_WORLD, wsize, index, edges, 0, &comm1 );
        MPI_Comm_dup( comm1, &comm2 );
        MPI_Topo_test( comm2, &topo_type );
        if (topo_type != MPI_GRAPH) {
            errs++;
            printf( "Topo type of duped graph was not graph\n" );fflush(stdout);
        }
        else {
            int nnodes, nedges;
            MPI_Graphdims_get( comm2, &nnodes, &nedges );
            if (nnodes != wsize) {
                errs++;
                printf( "Nnodes = %d, should be %d\n", nnodes, wsize );fflush(stdout);
            }
            if (nedges != 2*wsize) {
                errs++;
                printf( "Nedges = %d, should be %d\n", nedges, 2*wsize );fflush(stdout);
            }
            outindex = (int*)malloc(wsize * sizeof(int) );
            outedges = (int*)malloc(wsize * 2 * sizeof(int) );
            if (!outindex || !outedges) {
                printf( "Unable to allocate %d words for outindex or outedges\n", 3 * wsize );fflush(stdout);
                MPI_Abort( MPI_COMM_WORLD, 1 );
            }

            MPI_Graph_get( comm2, wsize, 2*wsize, outindex, outedges );
            for (i=0; i<wsize; i++) {
                if (index[i] != outindex[i]) {
                    printf( "%d = index[%d] != outindex[%d] = %d\n", index[i], i, i, outindex[i] );fflush(stdout);
                    errs++;
                }
            }
            for (i=0; i<2*wsize; i++) {
                if (edges[i] != outedges[i]) {
                    printf( "%d = edges[%d] != outedges[%d] = %d\n", edges[i], i, i, outedges[i] );fflush(stdout);
                    errs++;
                }
            }
            free( outindex );
            free( outedges );
        }
        free( index );
        free( edges );
        MPI_Comm_free( &comm2 );
        MPI_Comm_free( &comm1 );
    }

    MPI_Finalize();
    return 0;
}
