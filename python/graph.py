#!/usr/bin/env python
#
# see http://www.python.org/doc/essays/graphs/
#

import sys
import os

def find_path(graph, start, end, path=[]):
        path = path + [start]
        if start == end:
            return path
        if not graph.has_key(start):
            return None
        for node in graph[start]:
            if node not in path:
                newpath = find_path(graph, node, end, path)
                if newpath: return newpath
        return None

def main():

    graph = {'A': ['B', 'C'],
             'B': ['C', 'D'],
             'C': ['D'],
             'D': ['C'],
             'E': ['F'],
             'F': ['C']}

    path = find_path(graph, 'A', 'F')
    print path

if __name__ == '__main__':
    main()
