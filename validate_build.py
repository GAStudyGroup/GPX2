#!/usr/bin/env python
file = open("logBuild.log", "r")
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
            print (lineStr)
            print ("Deu merda, linha" + str(i))
        else:
            red = lineStr[3]
        
        if (int(blue) < int(lineStr[5])):
            print (lineStr)
            print ("Deu merda, linha"+ str(i))
        else:
            blue = lineStr[5]

