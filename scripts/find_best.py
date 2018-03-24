def getBest(LIB_PATH,name):
    try:
        bestFile = open(LIB_PATH+"/best.tsp", "r")
    except OSError:
        pass
    
    for line in bestFile:
        if line.find(name) != -1:
            # print ("found name "+name)
            array = line.split(":")
            return int(array[1])