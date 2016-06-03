#!/usr/bin/env python

#
# Usage: qrc_generator.py <target.qrc> <prefix> <directory>
#

import os
import sys
import fnmatch

project = sys.argv[1]
qrc_file = sys.argv[2]
prefix = sys.argv[3]
directory = os.path.abspath(sys.argv[4]).replace( '\\', '/' )
resourcePath = os.path.realpath( '%s/..' % directory )

qrcPath = os.path.dirname(os.path.realpath( qrc_file ))
resourcePathFile = "%s/resource_paths.txt" % qrcPath

try:
	with open( resourcePathFile ) as f:
	    matching = [line for line in f if resourcePathFile in line]
except:
	matching = []

if not matching:
	with open( resourcePathFile, 'a' ) as f:
		f.write( resourcePath + "\n" )

if directory[-1] != '/':
	directory += '/'

if not os.path.isdir(directory):
	sys.exit(-1)

files = []

for base, dirnames, filenames in os.walk(directory):
	for filename in fnmatch.filter(filenames, '*'):
		# Ignore hidden files or temporary editor files
		if filename.startswith( "." ) or \
			filename.endswith( ".swp" ) or \
			filename.endswith( ".orig" ):
			continue
		files.append(os.path.join(base, filename).replace( '\\', '/' ))

with open(qrc_file,'w') as qrc:
	qrc.write( '<RCC>\n\t<qresource prefix="/%s">\n' % prefix )
	for f in files:
		qrc.write('\t\t<file alias="%s">%s</file>\n' % \
			( f[len(directory):], f ) )

	if project != prefix:
		qrc.write('\t</qresource>\n')
		qrc.write( '\t<qresource prefix="/%s">\n' % project )
	qrc.write('\t\t<file alias="resource_paths.txt">%s</file>\n' % resourcePathFile.replace( '\\', '/' ))
	qrc.write('\t</qresource>\n</RCC>\n')
