#!/usr/bin/env python
import sys

try:
    file = open("Logs/logBuild_"+ str(sys.argv[2]) + "_" + str(sys.argv[1]) +".log", "r")
    fileLog = open("Logs/1_LOGPython_validateBuild_" + str(sys.argv[2]) + ".log", "a")
except OSError:
    pass

i = 0
for line in file:
    i+=1
    if line.find("INICIAL DISTANCE: ") != -1:
        array = line.split(" ")
        red = int(array[3])
        blue = int(array[5])
    elif line.find("FINAL DISTANCE: ") != -1:
        lineStr = line.split(" ")
        if (int(red) < int(lineStr[3])):
            print("Run_"+ str(sys.argv[1])+ " : " +str(line), file=fileLog)
            print ("Erro na linha " + str(i))
        else:
            red = lineStr[3]
        
        if (int(blue) < int(lineStr[5])):
            print("Run_"+ str(sys.argv[1])+ " : " +str(line), file=fileLog)
            print ("Erro na linha "+ str(i))
        else:
            blue = lineStr[5]

