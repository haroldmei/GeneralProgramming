#!/bin/python

import math
import os
import random
import re
import sys

'''
Given a set of distinct integers, print the size of a maximal subset of S where the sum of any 2 numbers in S' is not evenly divisible by K.
For example, the array [19,10,12,10,24,25,22] and k=4. One of the arrays that can be created is [10,12,25]. Another is [19,22,24];
After testing all permutations, the maximum length solution array has 3 elements.
'''
# Complete the nonDivisibleSubset function below.
def nonDivisibleSubset(k, S):
    r = [0] * k
     
    for value in S:
        r[value%k] += 1
     
    result = 0
    for a in range(1, (k+1)//2):
        result += max(r[a], r[k-a]) # single them out
    if k % 2 == 0 and r[k//2]:      # odd number of the median, count one
        result += 1
    if r[0]:                        # 
        result += 1
    return result

S = [19,10,12,10,24,25,22]
k = 4
r = nonDivisibleSubset(k, S)
print (r)