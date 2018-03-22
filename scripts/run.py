#!/usr/bin/env python
"""Código para automatizar a geração da população inicial, inicialização do algoritmo genético e validação dos logs de saídas"""

from subprocess import call
import os

LIBS_PATH = "./lib/"
BIN_PATH = "bin/"

POP_SIZE = 20
LK_PERCENTAGE = 0.2
NEW_POP_TYPE = 2

# tours = ["a280","att48","berlin52","burma14","ch150","eil101","gr137","pbd984","pcb442","u1432"]
tours = ["eil101"]

for tour_name in tours:
    for i in range(1):

        print(str(i)+" Generating population for problem "+tour_name+" with population size "+str(POP_SIZE)+", using "+str(LK_PERCENTAGE*100)+"% of LK generated tours and using:")
        
        if(NEW_POP_TYPE==0):
            print("All vs All GPX2 crossover\n")
        elif(NEW_POP_TYPE==1):
            print("N best vs All GPX2 crossover and reset population\n")
        else:
            print("All vs ALL with N best saved to the next population and the rest reseted\n");

        #gerar a pop inicial
        if(LK_PERCENTAGE>0):
            call(["./"+BIN_PATH+"linkern","-Q" ,"-S"+tour_name, "-R "+str(POP_SIZE*LK_PERCENTAGE), LIBS_PATH+tour_name+".tsp"])

        print("\n\nStarting GA\n")

        #chamar o GA
        call(["./"+BIN_PATH+"GA", tour_name, LIBS_PATH, str(POP_SIZE), str(i), str(LK_PERCENTAGE), str(NEW_POP_TYPE)])


        print("\n\nclean up\n")
        
        #apagar a pop inicial
        for f in os.listdir("."):
            if f.endswith(".dat"):
                os.remove(f)
        '''
        print("Validate crossover")
        call(["./validate_crossover.py", str(i), str(tour_name)])
        print("end validate crossover")

        print("Validate build")
        call(["./validate_build.py", str(i), str(tour_name)])
        print("end validate build")
        '''