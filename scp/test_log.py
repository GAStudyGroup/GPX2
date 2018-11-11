#!/usr/bin/env python
import glob
from sys import argv

TOKEN = 'Found best fitness!'
# TOKEN = 'Found best without GA'

try:
    if len(argv) == 2:
        print('log_*_'+argv[0]+'_'+argv[1]+'_2.log')
        list = glob.glob('log_*_'+argv[0]+'_'+argv[1]+'_2.log')
    elif len(argv) == 3:
        list = glob.glob('log_*_'+argv[0]+'_'+argv[1]+'_LK-'+argv[2]+'.000000_2.log')

    print('Found '+str(len(list))+' files.')
    cont = 0
    for fileName in list:
        if TOKEN in open(fileName).read():
            cont = cont + 1
    print(str(cont)+' runs have reached the best fitness')
except:
    print('No arguments passed')
