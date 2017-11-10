from subprocess import call
#import subprocess
import os

population_size = 10
tour_name = "kroB100"

print("Generating population for problem "+tour_name+" with population size "+str(population_size)+"\n")

#gerar a pop inicial
call(["./linkern", "-S "+tour_name, "-R "+str(population_size), tour_name+".tsp"])

print("\n\nStarting GA\n")

#chamar o GA
call(["./bin/GA", tour_name, str(population_size)])

print("\n\nclean up\n")

#apagar a pop inicial
for f in os.listdir("."): 
    if f.endswith(".dat"):  
        os.remove(f)
