#!/usr/bin/env python
"""Código para automatizar a geração da população inicial, inicialização do algoritmo genético e validação dos logs de saídas"""

from subprocess import call
import os

LIBS_PATH = "./lib/"
BIN_PATH = "bin/"

population_size = 50
LK_PERCENTAGE = 0.1
NEW_POP_TYPE = 1

# tours = ["eil101","berlin52","a280","att48","pcb442","pbd984"]
tours = ["att48"]

for tour_name in tours:
    for i in range(1):

        print(str(i)+" Generating population for problem "+tour_name+" with population size "+str(population_size)+", using "+str(LK_PERCENTAGE*100)+"% of LK generated tours and using:")
        
        if(NEW_POP_TYPE==0):
            print("All vs All GPX2 crossover\n")
        else:
            print("N best vs All GPX2 crossover and reset population\n")

        #gerar a pop inicial
        if(LK_PERCENTAGE>0):
            call(["./"+BIN_PATH+"linkern","-Q" ,"-S"+tour_name, "-R "+str(population_size*LK_PERCENTAGE), LIBS_PATH+tour_name+".tsp"])

        print("\n\nStarting GA\n")

        #chamar o GA
        call(["./"+BIN_PATH+"GA", tour_name, LIBS_PATH, str(population_size), str(i), str(LK_PERCENTAGE), str(NEW_POP_TYPE)])


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