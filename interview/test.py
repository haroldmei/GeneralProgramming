def mergeArray(a1, a2):
    i = 0 
    j = 0
    l1 = len(a1)
    l2 = len(a2)
    rs = []
    while i < l1 and j < l2:
        if a1[i] < a2[j]:
            rs.append(a1[i])
            i+=1
        else:
            rs.append(a2[j])
            j+=1
    
    if i < l1:
        rs = rs + a1[i:]
    elif j < l2:
        rs = rs + a2[j:]
    
    return rs


