from typing import List

class Solution:
    def f(self, ct, ts, lap, accu, base, delta):
        if lap == 0:
            for i in range(len(base)):
                accu[i] = base[i]
            minimum = min(accu)
            ts.append(minimum)
        else:
            for i in range(len(base)):
                base[i] = base[i] * delta[i]
                accu[i] = accu[i] + base[i]
            minimum = min(accu)
            tmp = [ts[i] + ts[lap - i - 1] + ct for i in range(lap)] + [minimum]
            ts.append(min(tmp))
        #print(ts)
        
    def minimumFinishTime(self, tires: List[List[int]], changeTime: int, numLaps: int) -> int:
        tires1 = []
        for t in tires:
            if t not in tires1:
                tires1.append(t)
        base = [tires[i][0] for i in range(len(tires1))]
        delta = [tires[i][1] for i in range(len(tires1))]
        accu = [0 for i in range(len(tires1))]
        ts = []
        for i in range(numLaps):
            self.f(changeTime, ts, i, accu, base, delta)
        return ts[-1]
        

sln = Solution()

tires = [[2,3],[3,4]]
changeTime = 5
numLaps = 4
print(sln.minimumFinishTime(tires, changeTime, numLaps))

tires = [[1,10],[2,2],[3,4]]
changeTime = 6
numLaps = 5
print(sln.minimumFinishTime(tires, changeTime, numLaps))