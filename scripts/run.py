#!/usr/bin/env python
"""Código para automatizar a geração da população inicial, inicialização do algoritmo genético e validação dos logs de saídas"""

from subprocess import call

import utils as u
import constants as c

for tour_name in c.TOURS:
    for i in range(c.NUMBER_OF_RUNS):
        best_fitness = u.getBest(tour_name)

        u.printInformations(tour_name,i,best_fitness)

        #gerar a pop inicial
        if(c.LK_PERCENTAGE>0):
            call(["./"+c.BIN_PATH+"linkern","-Q" ,"-S"+tour_name, "-R "+str(c.POP_SIZE*c.LK_PERCENTAGE), c.LIBS_PATH+tour_name+".tsp"])

        print("\n\nStarting GA\n")

        #chamar o GA
        call(["./"+c.BIN_PATH+"GA", tour_name, c.LIBS_PATH, str(c.POP_SIZE), str(i), str(c.LK_PERCENTAGE), str(c.NEW_POP_TYPE), str(best_fitness)])


        print("\n\nclean up\n")
        
        u.eraseTours()
