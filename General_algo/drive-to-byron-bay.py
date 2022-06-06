
def drive_to_byron(price, dist):
    length = len(price)
    i = length - 1
    #preproc 
    while i > 1:
        if price[i] == price[i - 1]:
            dist[i - 1] += dist[i]
            price.remove(price[i])
            dist.remove(dist[i])
            length -= 1
        i -= 1
    
    cur_min = 0
    cur_max = 0
    arr_min = []
    arr_max = []
    i = 0
    ontherise = False
    while i < length - 1:
        if price[i+1] < price[i]:
            cur_min = i + 1
            if ontherise:
                arr_max.append(cur_max)
                ontherise = False
        else:
            cur_max = i + 1
            if not ontherise:
                arr_min.append(cur_min)
                ontherise = True
        i += 1
    
    print(arr_min, arr_max)
    return 0

prices = [2,3,4,5,6]
dists = [1,1,1,1,1]
drive_to_byron(prices, dists)