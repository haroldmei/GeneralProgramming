'''
test:
cat test/test1.txt | python cache.py
'''

import sys

def lru(element, cache):
    try:
        idx = cache.index(element)
        tmp = cache[idx]
        for i in reversed(range(idx)):
            cache[i + 1] = cache[i]
        cache[0] = tmp
        return False
    except:
        cache.pop()
        cache.insert(0, element)
        return True
    
    

if __name__ == "__main__":
    lines = []
    for line in sys.stdin:
        line = line.rstrip('\r\n')
        lines.append(line)
    num, sz = map(lambda i: int(i), lines[0].split(' '))
    
    if len(lines)>2:
        ll = lines[1:]
        elements = list(map(lambda i: int(i), ll))
    else:
        ll = lines[1]
        elements = list(map(lambda i: int(i), ll.split(' ')))
    fp = 0
    cache = [None]*sz
    for e in elements:
        #print(e)
        if lru(e, cache):
            fp += 1
    while not cache[-1]:
        cache.pop()
        
    print(fp)
    print(' '.join(map(lambda a: str(a), cache)))