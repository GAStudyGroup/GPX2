#!/usr/bin/env python
import sys

file = open("Logs/logRun_"+ str(sys.argv[1]) +".log","r")
fileLog = open("Logs/LOGPython_validateCross.log", "a")

i=0
for line in file:
    i+=1
    if line.find("CROSSOVER ")!=-1:
        array = line.split(" ")
        if (int(array[3]) > int(array[1])) or (int(array[3]) > int(array[2])):
            print(str(line), file=fileLog)
            print("ERRRRRRROOOOOOOOOOO LINHA " + str(i))
            print(line)