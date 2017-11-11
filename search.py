file = open("broken_map.txt","r")
for line in file:
    if line.find("connections: 1")!=-1 or line.find("connections: 0")!=-1:
        print("FOUND BUG")
        print(line)
