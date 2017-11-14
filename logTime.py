#!/usr/bin/env python

import sys

fileTime = open("Logs/logTime_Set.log", "r")
fileFor = open("Logs/logTime_For.log", "r")

sumTime_set=0.0
sumCycle_set=0

sumTime_for=0.0
sumCycle_for=0

for line in fileTime:
   array =  line.strip()
   array2 = array.split(" ")
   sumTime_set += float(array2[3])
   sumCycle_set += int(array2[1])

for line in fileFor:
   array =  line.strip()
   array2 = array.split(" ")
   sumTime_for += float(array2[3])
   sumCycle_for += int(array2[1])


print ("Time\nSet: "+ str(sumTime_set) + " \t For: " + str(sumTime_for))
print ("\n")
print ("Cycle\nSet: "+ str(sumCycle_set) + " \t For: " + str(sumCycle_for))


