#!/usr/bin/env python

import fnmatch
import os
import subprocess
import errno
import sys

cwd = os.getcwd()
if os.name == 'nt':
	execName = 'clang-format.exe'
else:
	execName = 'clang-format'

sources = ['../../src/core/interfaces', '../../src/core/lib', '../../src/core/plugins', '../../src/core/testing']
for source in sources:
	for root, dirnames, filenames in os.walk(source):
		for ext in ['cpp', 'hpp', 'mpp']:
			for filename in fnmatch.filter(filenames, '*.'+ext):
				file = os.path.join(root, filename)
				proc = subprocess.Popen([cwd+'/'+execName, '-i', '--style=file', file], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
				proc.communicate()
				
