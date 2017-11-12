#!/usr/bin/env python
"""Código para automatizar a geração da população inicial e inicialização do algoritmo genético"""

from subprocess import call
import os

population_size = 20
tour_name = "pcb442"

for i in range(1):
    try:
        os.remove("logBuild.log")
    except OSError:
        pass

    print(str(i)+" Generating population for problem "+tour_name+" with population size "+str(population_size)+"\n")

    #gerar a pop inicial
    call(["./bin/linkern", "-S"+tour_name, "-R"+str(population_size), tour_name+".tsp"])

    print("\n\nStarting GA\n")

    #chamar o GA
    call(["./bin/GA", tour_name, str(population_size),str(i)])


    print("\n\nclean up\n")

    #apagar a pop inicial
    
    for f in os.listdir("."): 
        if f.endswith(".dat"):  
            os.remove(f)
   
    print("Search")
    call(["./search.py"])