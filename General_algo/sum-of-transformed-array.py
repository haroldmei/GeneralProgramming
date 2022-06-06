'''
Q1:  
You are given an input integer array of n-length: power[n]
for all subarrays: power[i:j] where 0 <= i <= j < n
sum it's transformed array of len n*(n-1): 

[min(power[i:j]) * sum(power[i:j]) for 0 <= i <= j < n]


Q2: https://leetcode.com/problems/sum-of-subarray-ranges/

https://drive.google.com/drive/folders/1Y2d-d8EYd_qbeBZOjwT34cK9DYvtkQOv

'''

# brute force solution O(n^2)
def sumOfTransformed(a):
    l = len(a)
    total = 0
    for i in range(l):
        s = 0
        m = 10**9+7
        for j in range(i, l):
            if a[j] < m:
                m = a[j]
            s = s + a[j]
            total += m*s
    return total

def sol(n):
    MOD = 10**9 + 7
    # L R
    R = [len(n)] * len(n)
    L = [0] * len(n)
    #
    monostack = []
    for i in range(len(n)):
        v = n[i]
        while monostack and monostack[-1][1] > v:
            R[monostack[-1][0]] = i
            monostack.pop()
        monostack.append((i, v))

    monostack = []
    for i in range(len(n)-1, -1, -1):
        v = n[i]
        while monostack and monostack[-1][1] >= v:
            L[monostack[-1][0]] = i+1
            monostack.pop()
            monostack.append((i, v))
    
    prefix = [0]
    for v in n:
        prefix.append((prefix[-1] + v) % MOD)

    l_triangle = [0]
    for i, v in enumerate(n):
        l_triangle.append((l_triangle[-1] + v * (i + 1)) % MOD)

    r_triangle = [0]
    for i, v in enumerate(n):
        r_triangle.append((r_triangle[-1] + v * (len(n) - i)) % MOD)
    
    def prefix_sum(l, r):
        return prefix[r]-prefix[l]

    def l_triangle_sum(l, r):
        return l_triangle[r] - l_triangle[l]

    def r_triangle_sum(l, r):
        return r_triangle[r] - r_triangle[l]

    res = 0
    for i in range(len(n)):
        r = R[i]
        l = L[i]
        #
        l_sum = l_triangle_sum(0, len(n)) - l_triangle_sum(0, l) - l_triangle_sum(i+1, len(n)) - prefix_sum(l, i+1)*l
        r_sum = r_triangle_sum(0, len(n)) - r_triangle_sum(0, i) - r_triangle_sum(r, len(n)) - prefix_sum(i, r)*(len(n)-r)
        l_r_sum = l_sum*(r-i) + r_sum*(i-l+1) - (r-i) * (i-l+1) * n[i]
        #
        res += l_r_sum*n[i]
        res %= MOD
    return res


print(sumOfTransformed([2,3,2,1]))
print(sol([2,3,2,1]))
print(sumOfTransformed([0,3,2,1,2,3,4]))
print(sol([0,3,2,1,2,3,4]))