#!/usr/bin/env python

import sys

def fib(n):
    last2 = 0
    last  = 1
    if n == 0 or n == 1:
        return n
    for i in range(2, n+1):
        f = last + last2
        last2 = last
        last = f
    return f
def main():
    ans = fib(int(sys.argv[1]))
    print "Fib=", ans

if __name__ == '__main__':
    main()
