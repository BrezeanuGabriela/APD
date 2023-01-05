from mpi4py import MPI

comm = MPI.COMM_WORLD
size = comm.Get_size()

rank = comm.Get_rank()

a = [
    [1,2,1,4],
    [2,1,4,7],
    [1,1,7,9]
]
# solutiile sunt 1,1,1

# primesc de la procesele de dinainte noii termeni si ii trimit mai departe
if rank > 0:
    # print(f"{rank} are de primit de {2 * rank + 1} ori")

    # primeste de la procesele de deasupra sa
    for i in range(0, rank):
        for j in range(1 + i,  size + 1):
            aux = comm.recv(source=rank - 1)

            # print(f"procesul {rank} a primit pe pozitia {j} - {aux} de la {i}")

            # trimite mai departe
            if rank < size - 1:
                comm.send(aux, dest=rank+1)

            # j e elementul care sufera modificarea
            # i e procesul originar de la care primeste aux
            a[rank][j] -= a[rank][i] * aux

    for i in range(0, rank):
        a[rank][i] = 0

for i in range(rank+1, size+1):
    a[rank][i] = a[rank][i] / a[rank][rank]

a[rank][rank] = 1

# trimite noii coeficientii calculati
if rank < size :
    for i in range(rank+1, size):
        for j in range(rank+1, size +1):
            # print(f"send: {rank} - > {i}  elementul de pe poz {j}")
            comm.send(a[rank][j], dest=i)

# incepe etapa de aflare a solutiilor
for i in range(size-1, rank, -1):
    # print(f"{rank} asteapta solutia de la {rank+1}")
    aux = comm.recv(source=rank+1)

    # trimite solutia mai departe (in sus)
    if rank > 0:
        comm.send(aux, rank - 1)

    # calculeaza inlocuirile cu solutiile primite
    for i in range(rank+1, size):
        a[rank][i] *= aux

# calcularea solutiei
if rank < size - 1:
    # scade valorile inlocuite
    for i in range(rank+1, size):
        a[rank][size] -= a[rank][i]

    if rank > 0:
        comm.send(a[rank][size], rank-1)

else:
    comm.send(a[rank][size], rank-1)

# print(f"{rank} - {a[rank]}")
print(f"Solutia x[{rank}]: {a[rank][size]}")
MPI.Finalize()



