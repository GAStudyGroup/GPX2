file = open("log_0.txt","r")
for line in file:
    if line.find("BestFitness: ")!=-1:
        print(line.split(" ")[1])
        #print(line)


