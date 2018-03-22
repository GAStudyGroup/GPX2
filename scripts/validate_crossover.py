#!/usr/bin/env python
import sys

try:
    file = open("Logs/logRun_"+ str(sys.argv[2]) + "_" + str(sys.argv[1]) +".log","r")
    fileLog = open("Logs/1_LOGPython_validateCross" + str(sys.argv[2]) + ".log", "a")
except OSError:
    pass

i=0
for line in file:
    i+=1
    if line.find("CROSSOVER ")!=-1:
        array = line.split(" ")
        if (int(array[3]) > int(array[1])) or (int(array[3]) > int(array[2])):
            print("Run_"+ str(sys.argv[1])+ " : " +str(line), file=fileLog)
            print("Erro na linha " + str(i))