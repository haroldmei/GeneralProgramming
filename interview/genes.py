#!/bin/python3

import math
import os
import random
import re
import sys

if __name__ == '__main__':
    n = int(input())

    genes = input().rstrip().split()

    health = list(map(int, input().rstrip().split()))

    s = int(input())
    maxHealth = -sys.maxsize - 1
    minHealth = sys.maxsize
    for s_itr in range(s):
        firstLastd = input().split()

        first = int(firstLastd[0])

        last = int(firstLastd[1])

        d = firstLastd[2]
        curHealth = 0
        for i in range(first,last+1):
            g = genes[i]
            istart = d.find(g)
            while istart >= 0:
                curHealth = curHealth + health[i]
                istart = d.find(g, istart+1)

        if curHealth < minHealth:
            minHealth = curHealth
        
        if curHealth > maxHealth:
            maxHealth = curHealth
    
    print(minHealth, maxHealth)
