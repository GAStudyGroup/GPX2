#!/usr/bin/env python
import glob
from sys import argv

TOKEN = 'Found best fitness!'

try:
    list = glob.glob(argv[1]+'*'+argv[2]+'*')
    print('Found '+str(len(list))+' files.')
    cont = 0
    for fileName in list:
        if TOKEN in open(fileName).read():
            cont = cont + 1
    print(str(cont)+' runs have reached the best fitness')
except:
    print('No arguments passed')