#!/usr/bin/env python
import shutil
import os
import errno

def main(): 
	try:
		os.mkdir('../../.git/hooks')
	except OSError as exc: 
		#already exists
	  if exc.errno == errno.EEXIST:
	    #already exists
	    pass
	  else: raise

	shutil.copy('pre-commit', '../../.git/hooks/pre-commit')


if __name__ == '__main__':
  main()