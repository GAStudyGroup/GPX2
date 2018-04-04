#!/usr/bin/env python
"""Code to automate the generation of the initial population and the start of the genetic algorithm"""

from subprocess import call

import utils as u
import configs as c

for n_best in c.N_BEST:
    for new_pop_type in c.NEW_POP_TYPE:
        for kl_percentage in c.LK_PERCENTAGE:
            for pop_size in c.POP_SIZE:
                for tour_name in c.TOURS:
                    for i in range(c.NUMBER_OF_RUNS):
                        best_fitness = u.getBest(tour_name)

                        u.printInformations(tour_name,pop_size,kl_percentage,i,n_best,new_pop_type,best_fitness)

                        if(kl_percentage>0):
                            call(["./"+c.BIN_PATH+"linkern","-Q" ,"-S"+tour_name, "-R "+str(pop_size*kl_percentage), c.LIBS_PATH+tour_name+".tsp"])

                        print("\n\nStarting GA\n")

                        call(["./"+c.BIN_PATH+"GA", "-n "+tour_name, "-l "+c.LIBS_PATH, "-s "+str(pop_size), "-id "+str(i), "-lk "+str(kl_percentage), "-np "+str(new_pop_type), "-nb "+str(n_best), "-bf "+str(best_fitness)])

                        print("\n\nclean up\n")

                        if(kl_percentage>0):
                            u.eraseTours()
