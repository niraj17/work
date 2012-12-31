#!/usr/bin/env python

import sys
import os

def hello(s):
    for filenames in os.listdir(s):
        print filenames
        print os.path.abspath(filenames)

def main():
    hello(sys.argv[1])

if __name__ == '__main__':
    main()
