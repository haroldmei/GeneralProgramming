import ast
from typing import List

class Solution:
    def maximumScore(self, scores: List[int], edges: List[List[int]]) -> int:
        neibors = {}
        for i,j in edges:
            if i not in neibors:
                neibors[i] = set([])
            neibors[i].add(j)
            if j not in neibors:
                neibors[j] = set([])
            neibors[j].add(i)
        
        #visited = set([])
        seg_neib = {}
        max_score = -1
        for v1, v in neibors.items():
            for v2 in v:
                if v2 == v1:
                    continue
                for v3 in neibors[v2]:
                    if v3 == v1 or v3 == v2:
                        continue
                    if tuple([v1, v2]) not in seg_neib:
                        seg_neib[tuple([v1, v2])] = set([])
                    seg_neib[tuple([v1, v2])].add(tuple([v2, v3]))
        
        for v1, v in seg_neib.items():
            for v2 in v:
                if v2 == v1:
                    continue
                for v3 in seg_neib[v2]:
                    if v3 == v1 or v3 == v2:
                        continue
                    score = scores[v1[0]] + scores[v1[1]] + scores[v3[0]] + scores[v3[1]]
                    #print(scores[v1] ,scores[v2] , scores[v3] , scores[v4], score)
                    if score > max_score:
                        max_score = score
        return max_score

        
with open('data/maximum-score-of-a-node-sequence.txt') as f:
    data = f.read()
scores, edges = data.split('\n')
scores, edges = ast.literal_eval(scores), ast.literal_eval(edges)

sln = Solution()
sln.maximumScore(scores, edges)