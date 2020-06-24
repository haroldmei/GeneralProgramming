
def pickingNumbers(a):
    a.sort()
    l = len(a)
    i1 = -1
    i2 = 0
    length = 0
    for i in range(l):
        if a[i] == a[i1] + 1:
            if a[i] != a[i2]:
                i2 = i
        elif a[i] > a[i2]:
            curlen = i - i1
            i1 = i2
            if a[i] > a[i2] + 1:
                i1 = i
            i2 = i
            if curlen > length:
                length = curlen
        elif a[i] == a[i1]:
            curlen = i - i1
            if curlen > length:
                length = curlen
            
    return length

ss = "1 1 1 1" #"66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66 66"
results = ss.split()
results = list(map(int, results))
res = pickingNumbers(results)
print (res)