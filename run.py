#!/usr/bin/env python
"""Código para automatizar a geração da população inicial, inicialização do algoritmo genético e validação dos logs de saídas"""

from subprocess import call
import os

population_size = 50

tours = ["pcb442"]

#tour_name = "pcb442"

for tour_name in tours:
    for i in range(1):

        print(str(i)+" Generating population for problem "+tour_name+" with population size "+str(population_size)+"\n")

        #gerar a pop inicial
        call(["./bin/linkern","-Q" ,"-S"+tour_name, "-R"+str(population_size*0.2), tour_name+".tsp"])

        print("\n\nStarting GA\n")

        #chamar o GA
        call(["./bin/GA", tour_name, str(population_size), str(i),str(6)])


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