# GPX2  (Generalized Partition Crossover 2) for solving the TSP (Travelling Salesperson Problem)

Repository for the development and testing of the Generalized Partition Crossover 2 (GPX2) algorithm.
GPX2 is a recombination operator to be used in the solution of the traveling salesman problem (TSP).
The main features of gpx are:


  * respectful - all equal edges in both parents are present in the child;
  * transmit edges - all edges of the child belong to some of parents, no new edges are created.


In this project, two local search algorithms were used to generate the initial population of the genetic algorithm, 2-opt and Lin-Kernighan(LK).
The GPX theory is available on the [wiki page](https://github.com/GAStudyGroup/GPX2/wiki/GPX2-steps).

There are three options to generate a new population:
  
  * Option 0:  Cross everyone with everyone and apply 2-opt in the results;
  * Option 1:  Cross the n best with everyone and save the result to the next generation, the rest of the population is generated using 2-opt and random beginning;
  * Option 2:   Cross everyone with everyone and save the n best to the next generation and reset the rest of the population using 2-opt and random beginning.

In this project we used the concorde implementation of LK, the binary is available in the support folder.

## Usage

1. Compile the code

Use make in th project folder.

```Bash
make
```

2. Move the LK binary from support to bin

If you want to use LKH to generate the first population you have to move it to the bin folder.

```Bash
mv support/linkern bin/linkern
```

3. Execute the binary

The binary accepts eight arguments, but only two are required to run the program.

  * '-n' Tour name;
  * '-s' Population size;
  * '-lib' Path to the folder with the tsp files;
  * '-id' run ID (used to generate log);
  * '-lk' Percentage of the population generated using LK (0 to use only 2-opt);
  * '-np' New generation method (0, 1 or 2);
  * '-nb' N best to be preserved for the next generation (used in new generation method 1 and 2);
  * '-bf' Best finess know for the problem (used to terminate the program when the best is reached).

The only two arguments required to run the GA are the tour name and the population size:

```Bash
./bin/GA -n berlin52 -n 50
```
Example using all:

```Bash
./bin/GA -n berlin52 -n 50 -bf 7544 -nb 5 -np 2 -id 2 -lib lib/ -lk 0.2
```

## Using with run script

You can use the run script inside the scripts folder to run the program more easily. You can set the variables inside the configs file. The arrays are used to test the values automatically.

```Python
POP_SIZE =          [50,75,100]
LK_PERCENTAGE =     [0,0.1]
NEW_POP_TYPE =      [1]
N_BEST =            [3]
```  

It's also possible to specify a list of tours to test and how many times to test each configuration.

```Python
TOURS = ["a280", "att48", "berlin52", "burma14", "ch150", "eil101", "gr137", "pbd984", "pcb442", "u1432"]
NUMBER_OF_RUNS = 1
```

These last options have to be changed only if you change the binary output file in the Makefile or change the folder of the tsp lib.

```Python
LIBS_PATH = "./lib/"
BIN_PATH = "bin/"
```

## Logs

The program saves data from the run on log files, located inside the folder Logs, they are separated by the method used to generated the new population.

It prints general information about the run, the time spent in each section and the information of each generation. In the end it prints the last population of the GA.

```
Run id: 0
Genetic Algoritm for problem berlin52 with population size 10
Using new generation method: 3 best vs All GPX2 crossover with reset population.

Population created in:
	1008.8 milliseconds
	1.0088 seconds

First fitness 8079
gen 1 best fitness 7861
gen 2 best fitness 7740
gen 3 best fitness 7542
gen 4 best fitness 7542

GA execution time:
	2966.14 milliseconds
	2.96614 seconds

Total execution time:
	3975.07 milliseconds
	3.97507 seconds

THE END
first best fitness: 8079
gen 5 best fitness 7542
=========================

Last population
===================================================
1 22 31 18 3 17 21 42 7 2 30 23 20 50 29 16 46 44 34 35 36 39 40 37 38 48 24 5 15 6 4 25 12 28 27 26 47 13 14 52 11 51 33 43 10 9 8 41 19 45 32 49 
Fitness: 7542
.
.
.
```
