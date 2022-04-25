from typing import List

# time out error

class Solution:
    def __init__(self):
        self.max_split = 0
        self.result = None
    def split(self, max_num, l_even, final):
        if final == 0 and max_num > self.max_split:
            self.max_split = max_num
            self.result = l_even
                
        for i in range(1, final//2 + 1):
            ii = i*2
            if ii not in l_even and ii <= final:
                ll = l_even.copy()
                ll.append(ii)
                #print(ll, ii, final)
                self.split(max_num + 1, ll, final - ii)
            
    def maximumEvenSplit(self, finalSum: int) -> List[int]:
        if finalSum % 2 > 0:
            return []
        
        max_num = 0
        l = []
        self.split(max_num, l, finalSum)
        return self.result
        
