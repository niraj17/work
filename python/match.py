#!/usr/bin/env python

import sys
import re

def match():
    text = "MY email is n.k@gmail.com ."

    m = re.search(r'([\w.]+)@([\w.]+)', text)
    if m:
        print m.group()
        print m.group(1)
        print m.group(2)
    else:
        print "Not found"

    lst = re.findall(r'([\w.]+)@([\w.]+)', text)
    if lst:
        print lst
    else:
        print "Not found"

def main():
    ans = match()

if __name__ == '__main__':
    main()
