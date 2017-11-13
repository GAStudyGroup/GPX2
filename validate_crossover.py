#!/usr/bin/env python
file = open("logRun_0.log","r")
i=0
for line in file:
    i+=1
    if line.find("CROSSOVER ")!=-1:
        array = line.split(" ")
        if (int(array[3]) > int(array[1])) or (int(array[3]) > int(array[2])):
            print("ERRRRRRROOOOOOOOOOO LINHA " + str(i))
            print(line)

        


