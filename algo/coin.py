#!/usr/bin/env python

coin = [1,2,5,10,20,50,100,500]
input = 15

print "hello", coin[0]

for n in range(1, input):
	min(solve(n-1), solve(n-2))
