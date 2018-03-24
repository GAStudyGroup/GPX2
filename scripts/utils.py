import os

import constants

def getBest(name):
    try:
        bestFile = open(constants.LIBS_PATH+"/best.tsp", "r")
    except OSError:
        pass
    
    for line in bestFile:
        if line.find(name) != -1:
            # print ("found name "+name)
            array = line.split(":")
            return int(array[1])


def printInformations(name,id,best):
    print(str(id)+" Generating population for problem "+name+" with population size "+str(constants.POP_SIZE)+", using "+str(constants.LK_PERCENTAGE*100)+"% of LK generated tours and using:")
        
    if(constants.NEW_POP_TYPE==0):
        print("All vs All GPX2 crossover\n")
    elif(constants.NEW_POP_TYPE==1):
        print("N best vs All GPX2 crossover with reset population\n")
    else:
        print("All vs ALL with N best saved to the next population and the rest reseted\n")
        
    print("best fitness found: "+str(best))
        

def eraseTours():
    #apagar a pop inicial
        for f in os.listdir("."):
            if f.endswith(".dat"):
                os.remove(f)