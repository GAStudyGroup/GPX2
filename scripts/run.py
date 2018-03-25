#!/usr/bin/env python
"""Code to automate the generation of the initial population and the initialization of the genetic algorithm"""

from subprocess import call

import utils as u
import configs as c

for tour_name in c.TOURS:
    for i in range(c.NUMBER_OF_RUNS):
        best_fitness = u.getBest(tour_name)

        u.printInformations(tour_name,i,best_fitness)

        if(c.LK_PERCENTAGE>0):
            call(["./"+c.BIN_PATH+"linkern","-Q" ,"-S"+tour_name, "-R "+str(c.POP_SIZE*c.LK_PERCENTAGE), c.LIBS_PATH+tour_name+".tsp"])

        print("\n\nStarting GA\n")

        call(["./"+c.BIN_PATH+"GA", tour_name, c.LIBS_PATH, str(c.POP_SIZE), str(i), str(c.LK_PERCENTAGE), str(c.NEW_POP_TYPE), str(c.N_BEST), str(best_fitness)])


        print("\n\nclean up\n")
        

        if(c.LK_PERCENTAGE>0):
            u.eraseTours()
