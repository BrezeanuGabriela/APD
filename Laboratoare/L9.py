from mpi4py import MPI

comm = MPI.COMM_WORLD
size = comm.Get_size()

rank = comm.Get_rank()
status = MPI.Status()

A = [
    [1,2,0,3],
    [2,3,4,0],
    [1,0,1,0],
    [3,2,1,1]
]

B = [
    [1,0,1,0],
    [2,3,4,1],
    [0,1,1,1],
    [3,2,1,0]
]

C = [
    [0,0,0,0],
    [0,0,0,0],
    [0,0,0,0],
    [0,0,0,0]
]

dim = 4

# linia
i = int(rank / dim)
# coloana
j = rank % dim

m1 = A[i][j]
m2 = B[i][j]

# print(f"{rank} - {i} - {j} - {m1} - {m2}")

# alinierea initiala pe orizontala
for k in range(0, i):
    source = 0
    dest = 0

    # prima coloana
    if j == 0:
        dest = i * dim + dim - 1
        source = i * dim + 1
    # ultima coloana
    elif j == dim - 1:
        source = i * dim
        dest = i * dim + j - 1
    else:
        dest = i * dim + j - 1
        source = i * dim + j + 1

    # print(f"{rank} - {dest} - {source}")
    comm.send(m1, dest=dest)
    m1 = comm.recv(source=source)
    # print(f"{rank} - {i} - {j} - {m1} - {m2}")

# alinierea initiala pe verticala
for k in range(0, j):
    source = 0
    dest = 0

    if j != 0:
        # prima linie
        if i == 0:
            dest = dim * (dim-1) + j
            source = (i + 1) * dim + j
        # ultima linie
        elif i == dim - 1:
            source = j
            dest = (i - 1) * dim + j
        else:
            dest = (i - 1) * dim + j
            source = (i + 1) * dim + j

        # print(f"{i} - {j} : {rank} - {dest} - {source}")
        comm.send(m2, dest=dest)
        m2 = comm.recv(source=source)

m3 = 0

for k in range(0, dim):
    m3 += m1 * m2

    # trimit pe orizontala

    # prima coloana
    if j == 0:
        dest = i * dim + dim - 1
        source = i * dim + 1
    # ultima coloana
    elif j == dim - 1:
        source = i * dim
        dest = i * dim + j - 1
    else:
        dest = i * dim + j - 1
        source = i * dim + j + 1

    # print(f"{rank} - {dest} - {source}")
    comm.send(m1, dest=dest)
    m1 = comm.recv(source=source)

    #trimit pe verticala
    # prima linie
    if i == 0:
        dest = dim * (dim - 1) + j
        source = (i + 1) * dim + j
    # ultima linie
    elif i == dim - 1:
        source = j
        dest = (i - 1) * dim + j
    else:
        dest = (i - 1) * dim + j
        source = (i + 1) * dim + j

    comm.send(m2, dest=dest)
    m2 = comm.recv(source=source)

# m3 += m1 * m2
# C[i][j] = m3
print(f"C[{i}][{j}] = {m3}")

MPI.Finalize()

