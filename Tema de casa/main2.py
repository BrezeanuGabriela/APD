import datetime
import glob

from mpi4py import MPI
import time
import os
import regex as re
import json

comm = MPI.COMM_WORLD
size = comm.Get_size()

rank = comm.Get_rank()
status = MPI.Status()

FINISHED_TAG = 0
MAP_TAG = 1
REDUCE_TAG = 2
FINAL_TAG = 3

NO_WORKER = size - 1

# procesul cu rank-ul 0 e master-ul
MASTER_RANK = 0

# FILES = os.listdir("Inputs")
INPUT_PATH = "TestInputs"
# INPUT_PATH = "Inputs"
FILES = os.listdir(INPUT_PATH)
# FILES = ["25.txt"]
OUTPUT_MAPPER_DIR = "OutputMappers"
OUTPUT_REDUCE_DIR = "OutputReducers"

########################################     Faza de MAPARE     ###############################
if rank == 0:
    print(datetime.datetime.now())

    os.mkdir(OUTPUT_MAPPER_DIR)
    os.mkdir(OUTPUT_REDUCE_DIR)

    # se trimite cate un fisier tuturor workerilor
    for i in range(1, NO_WORKER+1):
        comm.send(FILES[0], dest=i, tag=MAP_TAG)
        FILES.pop(0)

else:
    # bucla in care worker-ul asteapta fisiere pentru a le parsa
    recv_files = True
    while recv_files:
        msg = comm.recv(source=MASTER_RANK,
                        tag=MPI.ANY_TAG,
                        status=status)
        tag = status.Get_tag()

        if tag == MAP_TAG:
            print(f'On process {rank}, data is {msg} received from {status.Get_source()} on tag {tag}')

            encoding = 'unicode_escape'
            f = open(f"{INPUT_PATH}/{msg}", "r", encoding=encoding)
            lines = f.readlines()
            f.close()

            lines = " ".join(lines)
            words = list(filter(lambda x: x != '' and len(x) > 2, re.findall("[a-zA-Z]*", lines)))
            words = list(map(lambda x: x.lower(), words))
            word_count_dict = dict()

            for word in words:
                word_count_dict[word] = word_count_dict.get(word, 0) + 1
#combiner
            for word, count in word_count_dict.items():
                timestamp = "".join(str(time.time()).split("."))

                # msg contine numele fisierului
                fileName = f"./{OUTPUT_MAPPER_DIR}/{word}_{msg}_{count}_{rank}_{timestamp}.txt"

                f = open(fileName, "w")
                f.close()

            # anunta Master-ul ca a terminat de parsat fisierul curent
            comm.send("Finished", dest=MASTER_RANK, tag=FINISHED_TAG)

        # mesajul de la Master pentru a iesi din while
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
    # plus se previn delay-urile dintre comunicatii - scris fisierele
    for i in range(1, NO_WORKER + 1):
        msg = comm.recv(source=MPI.ANY_SOURCE, tag=REDUCE_TAG, status=status)
        print(f"{msg} from {status.Get_source()}")

########################################     Faza de REDUCE     ###############################
    REDUCE_FILES = os.listdir(OUTPUT_MAPPER_DIR)

    # fiecare worker va face reduce pe cate un cuvant
    vocabulary = list(set(map(lambda fileName: fileName.split("_")[0], REDUCE_FILES)))
    # print(vocabulary)

    for i in range(1, NO_WORKER+1):
        word = vocabulary[0]
        vocabulary.pop(0)

        # trimit cuvantul de care se va ocupa
        comm.send(word, dest=i, tag=REDUCE_TAG)

    # se trimit in continuare fisiere pentru etapa de reduce catre workers care si-au terminat treaba
    while len(vocabulary) > 0:
        msg = comm.recv(source=MPI.ANY_SOURCE, tag=FINISHED_TAG, status=status)

        word = vocabulary[0]
        vocabulary.pop(0)

        # trimit cuvantul de care se va ocupa
        comm.send(word, dest=status.Get_source(), tag=REDUCE_TAG)

    # se anunta workerii ca nu mai au fisiere de parsat in faza de reduce
    for i in range(1, NO_WORKER + 1):
        comm.send("Finished", dest=i, tag=FINISHED_TAG)

else:
    recv_files = True
    while recv_files:
        msg = comm.recv(source=MASTER_RANK,
                        tag=MPI.ANY_TAG,
                        status=status)
        tag = status.Get_tag()

        # print(msg)
        # os.chdir("OutputMappers")
        # REDUCE_FILES = glob.glob(f"OutputMappers/{msg}_*.txt")
        REDUCE_FILES = list(os.scandir(f"{OUTPUT_MAPPER_DIR}"))
        # print(REDUCE_FILES)

        if tag == REDUCE_TAG:
            files_for_reduce = list(map(lambda x: x.name, filter(lambda x: msg in x.name, REDUCE_FILES)))
            # files_for_reduce = REDUCE_FILES

            file_count_dict = dict()

            for file in files_for_reduce:
                # file = file.split("/")[1]
                elements_of_fileName = file.split("_")
                word = elements_of_fileName[0]
                fileName = elements_of_fileName[1]
                count = elements_of_fileName[2]

                file_count_dict[fileName] = file_count_dict.get(fileName, 0) + int(count)

            timestamp = "".join(str(time.time()).split("."))
            fileName = f"./{OUTPUT_REDUCE_DIR}/{msg}_{rank}_{timestamp}.txt"

            # print(file_count_dict)
            content = ""
            for f, count in file_count_dict.items():
                content += f"{f}:{count}\n"

            content = content.strip()
            # print(content)

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
    final_dir = OUTPUT_REDUCE_DIR
    comm.send(f"Start final Phase-{final_dir}", dest=1, tag=FINAL_TAG)

elif rank == 1:
    msg = comm.recv(source=MASTER_RANK, tag=FINAL_TAG)
    print(msg)

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

    print("Procesarea s-a executat cu succes")
    print(datetime.datetime.now())

MPI.Finalize()









