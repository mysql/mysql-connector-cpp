#/usr/bin/env python
import sys

# read how many types to write msg
line = sys.stdin.readline()
max = int(line)
for i in range(1,max + 1):
  print('hello' +str(i))
  sys.stdout.flush()
