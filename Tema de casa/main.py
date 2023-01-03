import datetime
import time

from mpi4py import MPI
import os
import regex as re
import json
import numpy as np

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
status = MPI.Status()

MAP_TAG = 1
REDUCE_TAG = 2
NO_WORKER = 3
MASTER_RANK = 0
FINISHED_TAG = 0
FINAL_TAG = 3

FILES = os.listdir("Inputs")

# procesul cu rank-ul 0 e master-ul

###################### Faza de MAPARE ##########################3
if rank == 0:
    os.mkdir("Outputs")
    os.mkdir("Outputs2")

    # se trimite cate un fisier tuturor workerilor
    for i in range(1, NO_WORKER+1):
        comm.send(FILES[0], dest=i, tag=MAP_TAG)
        FILES.pop(0)

else:
    # bucla in care worker-ul asteapta fisiere pentru a le parsa
    recv_files = True
    while recv_files:
        msg = comm.recv(source=MPI.ANY_SOURCE,
                        tag=MPI.ANY_TAG,
                        status=status)
        tag = status.Get_tag()

        if tag == MAP_TAG:
            print(f'On process {rank}, data is {msg} received from {status.Get_source()} on tag {tag}')
            # print(f'{rank} - proceseaza (map)')

            f = open(f"Inputs/{msg}","r")
            lines = f.readlines()
            f.close()

            lines = " ".join(lines)
            words = list(filter(lambda x: x != '', re.findall("[a-zA-Z]*", lines)))
            word_count_dict = dict()

            for word in words:
                word_count_dict[word] = word_count_dict.get(word, 0) + 1

            # print(word_count_dict)

            for word, count in word_count_dict.items():
                timestamp = "".join(str(time.time()).split("."))
                fileName = f"./Outputs/{word}_{rank}_{timestamp}.txt"
                content = f"{msg}:{count}"
                f = open(fileName, "w")
                f.write(content)
                f.close()

            comm.send("Finished", dest=MASTER_RANK, tag=FINISHED_TAG)
        elif tag == FINISHED_TAG:
            if msg == "Finished":
                # print(f'{rank} - mi-am terminat treaba ca map worker')
                recv_files = False
                comm.send("Stop Map Phase", dest=MASTER_RANK, tag=REDUCE_TAG)

if rank == 0:
    # master-ul continua sa trimita fisiere pentru parsare catre mappers
    while len(FILES) > 0:
        msg = comm.recv(source=MPI.ANY_SOURCE, tag=FINISHED_TAG, status=status)
        # print(f'\nMapper-ul {status.Get_source()} a terminat. va primi alt fisier')
        comm.send(FILES[0], dest=status.Get_source(), tag=MAP_TAG)
        FILES.pop(0)

    # se anunta workerii ca nu mai au fisiere de parsat in faza de map
    for i in range(1, NO_WORKER + 1):
        comm.send("Finished", dest=i, tag=FINISHED_TAG)

    # un fel de bariera pentru a fi siguri ca si-au terminat activitatea toti workerii din faza de mapare
    for i in range(1, NO_WORKER + 1):
        msg = comm.recv(source=MPI.ANY_SOURCE, tag=REDUCE_TAG, status=status)
        print(f"{msg} from {status.Get_source()}")

    # aici ar trebui sa inceapa faza de reduce

#################### Faza de REDUCE ###############################
    REDUCE_FILES = os.listdir("Outputs")

    # fiecare worker va face reduce pe cate un cuvant
    vocabulary = list(set(map(lambda fileName: fileName.split("_")[0], REDUCE_FILES)))
    print(vocabulary)

    for i in range(1, NO_WORKER+1):
        word = vocabulary[0]
        files = list(filter(lambda x: word in x, REDUCE_FILES))
        vocabulary.pop(0)
        mesaj = " ".join(files)
        comm.send(mesaj, dest=i, tag=REDUCE_TAG)

    print(vocabulary)

    while len(vocabulary) > 0:
        msg = comm.recv(source=MPI.ANY_SOURCE, tag=FINISHED_TAG, status=status)

        word = vocabulary[0]
        files = list(filter(lambda x: word in x, REDUCE_FILES))
        vocabulary.pop(0)
        mesaj = " ".join(files)

        comm.send(mesaj, dest=status.Get_source(), tag=REDUCE_TAG)

    # se anunta workerii ca nu mai au fisiere de parsat in faza de map
    for i in range(1, NO_WORKER + 1):
        comm.send("Finished", dest=i, tag=FINISHED_TAG)

else:
    recv_files = True
    while recv_files:
        msg = comm.recv(source=MPI.ANY_SOURCE,
                        tag=MPI.ANY_TAG,
                        status=status)
        tag = status.Get_tag()

        if tag == REDUCE_TAG:
            files_for_reduce = msg.split(" ")

            file_count_dict = dict()

            word = files_for_reduce[0].split("_")[0]
            for file in files_for_reduce:
                word = file.split("_")[0]
                f = open(f"Outputs/{file}", "r")
                content = f.readlines()
                content = " ".join(content).strip()
                fileName, count = content.split(":")

                file_count_dict[fileName] = file_count_dict.get(fileName, 0) + int(count)

            timestamp = "".join(str(time.time()).split("."))
            fileName = f"./Outputs2/{word}_{rank}_{timestamp}.txt"

            content = ""
            for f, count in file_count_dict.items():
                content += f"{f}:{count}\n"

            content = content.strip()

            f = open(fileName, "w")
            f.write(content)
            f.close()

            comm.send("Finished", dest=MASTER_RANK, tag=FINISHED_TAG)

        elif tag == FINISHED_TAG:
            if msg == "Finished":
                recv_files = False
                comm.send("Stop Reduce Phase", dest=MASTER_RANK, tag=FINAL_TAG)

if rank == 0:
    for i in range(1, NO_WORKER + 1):
        msg = comm.recv(source=MPI.ANY_SOURCE, tag=FINAL_TAG, status=status)
        print(f"{msg} from {status.Get_source()}")

    # poate incepe ultima faza de reducere
    final_dir = "Outputs2"
    comm.send(f"Start final Phase-{final_dir}", dest=1, tag=FINAL_TAG)

elif rank == 1:
    msg = comm.recv(source=0, tag=FINAL_TAG)
    dir = msg.split("-")[1]
    files = os.listdir(dir)

    final_dict = {}

    for file in files:
        word = file.split("_")[0]
        final_dict[word] = []

        f = open(f"{dir}/{file}", "r")
        lines = f.readlines()
        lines = list(map(lambda x: x.strip(), lines))
        f.close()

        final_dict[word].extend(lines)

    json_result = json.dumps(final_dict, indent=4)
    result_file = open(f"Results/results.json", "w")
    result_file.write(json_result)
    result_file.close()

MPI.Finalize()









