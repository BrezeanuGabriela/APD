import os
import glob

if __name__ == "__main__":
    # os.chdir("OutputMappers")
    print(glob.glob("OutputMappers/ear_*.txt"))
    # print(os.listdir("OutputMappers/blackened_*.txt"))
    files = list(os.scandir("Inputs"))
    print(files[0].name)