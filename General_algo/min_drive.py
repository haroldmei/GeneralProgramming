

def calculate(prices, distances):
    length = len(distances)

    def recurse(curr, next, r):
        dist = sum(distances[curr: next])
        if next == length:
            cost = (dist - r) * prices[curr]
            #print('e-', cost)
            return cost

        if prices[next] < prices[curr]:
            if dist > r:
                cost = (dist - r) * prices[curr]
                #print('a-', cost)
                return cost + recurse(next, next+1, 0)
            else:
                cost = 0
                #print('b-', cost)
                return recurse(next, next+1, r - dist)
        else:
            if dist + distances[next] > 50: # full
                cost = (50 - r) * prices[curr]
                #print('c-', cost)
                return cost + recurse(next, next + 1, 50 - dist)
            else:
                cost = 0
                #print('d-', cost)
                return recurse(curr, next + 1, r)
    cost = recurse(0, 1, 0)
    return cost


prices = [8, 19, 18,18,18,18]
distances = [40, 30, 5,5,5,5]

print(calculate(prices, distances))

