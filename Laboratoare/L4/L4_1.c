
int *index, *edges;
index = (int*)malloc(noNodes * sizeof(int) );
edges = (int*)malloc(noNodes * 2 * sizeof(int) );

index[0] = 3;
index[1] = 5;
if(my_rank == r)
{
	//doar procesul radacina intra aici
	for(int i=0; i < nv; i++)
	{
		MPI_Send(M, vecini[i], comm);
	}
}
else
{
	//nod interior + frunze
	// recv Mesaj from MPI_ANY_SOURCE
	for(int i=0; i< nv; i++)
	{
		if(st.MPI_SOURCE != v[i] 
		{
			send(M, v[i], .. )
		}
	}
}
	
