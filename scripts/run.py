#!/usr/bin/env python
"""Code to automate the generation of the initial population and the initialization of the genetic algorithm"""

from subprocess import call

import utils as u
import configs as c

for kl_percentage in c.LK_PERCENTAGE:
    for pop_size in c.POP_SIZE:
        for tour_name in c.TOURS:
            for i in range(c.NUMBER_OF_RUNS):
                best_fitness = u.getBest(tour_name)

                u.printInformations(tour_name,pop_size,kl_percentage,i,best_fitness)

                if(kl_percentage>0):
                    call(["./"+c.BIN_PATH+"linkern","-Q" ,"-S"+tour_name, "-R "+str(pop_size*kl_percentage), c.LIBS_PATH+tour_name+".tsp"])

                print("\n\nStarting GA\n")

                call(["./"+c.BIN_PATH+"GA", tour_name, c.LIBS_PATH, str(pop_size), str(i), str(kl_percentage), str(c.NEW_POP_TYPE), str(c.N_BEST), str(best_fitness)])

                print("\n\nclean up\n")

                if(kl_percentage>0):
                    u.eraseTours()
