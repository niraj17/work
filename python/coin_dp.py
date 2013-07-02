#!/usr/bin/env python
# Dynamic programming for coin change problem
# see http://www.ccs.neu.edu/home/jaa/CSG713.04F/Information/Handouts/dyn_prog.pdf
#

import sys

def coin_dp(sum, coins):
    c = 0 # last coin used in sol
    last = []

    sol = [0 for i in range(0, sum+1)]

    for i in range(1, sum+1):
        min = sys.maxint
        for coin in coins:
            if (i >= coin):
                if (min > sol[i-coin]):
                    min = 1 + sol[i-coin] 
                    c = coin
        sol[i] = min
        last.append(c)

    return (min, last)

def main():
    sum = 59
    coins = [1,2,5,10,20,50,100]
    
    if (len(sys.argv) >= 2):
        sum = int(sys.argv[1])

    print "Solving coin problem using Dynamic Programming ..."
    print "Total value=", sum, " coins =", coins

    (ans, last) = coin_dp(sum, coins)
    print "Ans=", ans, "(",
    while (True):
        if sum <= 0:
            break;
        print last[sum -1],;
        sum = sum - last[sum -1]
    print ")"

if __name__ == '__main__':
    main()
