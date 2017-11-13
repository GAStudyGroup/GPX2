#!/usr/bin/env python
import sys

file = open("Logs/logBuild_"+ str(sys.argv[1]) +".log", "r")
fileLog = open("Logs/1_LOGPython_validateBuild.log", "a")

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
            print (lineStr)
            print ("Deu merda, linha" + str(i))
        else:
            red = lineStr[3]
        
        if (int(blue) < int(lineStr[5])):
            print("Run_"+ str(sys.argv[1])+ " : " +str(line), file=fileLog)
            print (lineStr)
            print ("Deu merda, linha"+ str(i))
        else:
            blue = lineStr[5]

