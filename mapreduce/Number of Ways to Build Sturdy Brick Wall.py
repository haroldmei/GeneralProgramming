class Solution:
    def conf(self, width, bricks):
        if not width:
            return [[]]
        
        result = []
        for b in bricks:
            if width < b:
                continue
            tmp = self.conf(width-b, bricks)
            result = result + [[b] + l for l in tmp]
        return result
    
    def sturdy(self, r1, r2):
        i = 1
        j = 1
        s1 = r1[0]
        s2 = r2[0]
        while i < len(r1) and j < len(r2):
            if s1 == s2:
                return False
            if s1 < s2:
                s2 = s2 + r2[j]
                j += 1
            else:
                s1 = s1 + r1[i]
                i += 1
        return True
    
            
    def buildWall(self, height: int, width: int, bricks: List[int]) -> int:
        lst = self.conf(width, bricks)
        for i in lst:
            
        