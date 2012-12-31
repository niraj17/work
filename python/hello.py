#!/usr/bin/env python

import sys

def hello(s):
  if s == 'A' or s == 'B':
    s = s + "???"
  else:
    DoesNotExist(s)
  print "Hello", s

def main():
  print "This is main"
  if 'A' in sys.argv:
    print "It has A"
  for i in sys.argv:
    print i
  hello(sys.argv[1])

if __name__ == '__main__':
  main()
