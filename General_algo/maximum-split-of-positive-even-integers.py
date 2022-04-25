class Solution:
    def split(self, curr, result, lst, final):
        #for i in lst:
        i = lst[0]
        if i < final:
            lst1 = lst.copy()
            lst1.remove(i)
            curr.append(i)
            self.split(curr, result, lst1, final - i)
        elif i == final:
            curr.append(final)
        else:
            curr[-1] = curr[-1] + final
            
            
    def maximumEvenSplit(self, finalSum: int) -> List[int]:
        if finalSum % 2 > 0:
            return []
        
        lst = [i * 2 for i in range(1, finalSum//2+1)]
        curr = []
        result = []
        self.split(curr, result, lst, finalSum)
        #print(curr)
        return curr