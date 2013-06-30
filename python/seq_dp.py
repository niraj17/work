#!/usr/bin/env python
#
# Dynamic programming for Longest Increasing Subsequence
# See http://en.wikipedia.org/wiki/Longest_increasing_subsequence
#

import sys

def seq_dp(seq):
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
    seq = [0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7, 15]
    
    print "Solving longest seq problem using Dynamic Programming ..."
    print "Seq =", seq

    (ans, last) = seq_dp(seq)
    print "Ans=", ans, "(",

if __name__ == '__main__':
    main()
