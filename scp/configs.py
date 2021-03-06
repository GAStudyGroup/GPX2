#Arguments used by the algorithm
# POP_SIZE =          [25,50]
# LK_PERCENTAGE =     [0,0.2]
# NEW_POP_TYPE =      [0,1,2,3]
# N_BEST =            [5]
# RESET_PERCENTAGE =  [0.5]
# NEW_TOUR_MODE =     [2]

POP_SIZE =          [25]
LK_PERCENTAGE =     [0]
NEW_POP_TYPE =      [0,1,2]
N_BEST =            [5]
RESET_PERCENTAGE =  [0.5]
NEW_TOUR_MODE =     [2]

#run 0
# POP_SIZE =          [50, 75, 100]
# LK_PERCENTAGE =     [0, 0.1, 0.2]
# NEW_POP_TYPE =      0
# N_BEST =            3

#run 1
# POP_SIZE =          [50, 75, 100]
# LK_PERCENTAGE =     [0, 0.1, 0.2]
# NEW_POP_TYPE =      1
# N_BEST =            3

#run 2
# POP_SIZE =          [50, 75, 100]
# LK_PERCENTAGE =     [0, 0.1, 0.2]
# NEW_POP_TYPE =      2
# N_BEST =            3

#Number of times the algorithm should run in each set of configs
NUMBER_OF_RUNS = 1

#list of datasets for the algorithm
#TOURS = ["a280","att48","berlin52","burma14","ch150","eil101","gr137","pbd984","pcb442","u1432"]
#TOURS = ["att48","berlin52","eil101","ch150","a280","gr137"]
# TOURS = ['berlin52','att48','eil101']
TOURS = ['berlin52','att48']
# TOURS = ['ch150','a280']

#Path to TSP folder and executables, change only if you change the project folder structure
LIBS_PATH = "tsp/"
BIN_PATH = "bin/"
