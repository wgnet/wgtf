#!/usr/bin/env python
import compileall
import os
import re
import shutil
import sys

def main():
	sourceDir = os.path.normpath(sys.argv[1])
	deploymentDir = os.path.normpath(sys.argv[2])

	# Remove old *.pyc files
	# This is useful if a *.py file was deleted, but the *.pyc remains
	print "Removing *.pyc files in", sourceDir
	for (dirPath, dirNames, fileNames) in os.walk(sourceDir):
		for fileName in fileNames:
			if fileName.endswith(".pyc"):
				os.remove(os.path.join(dirPath, fileName))

	# Ignore the following files in standard Python, because they produce errors
	ignoredFiles = (
		"py3_test_grammar\.py",
		"_mock_backport\.py",
		"bad_coding.*\.py",
		"badsyntax.*\.py",
		"test_compile.py",
		"test_grammar.py"
	)
	ignoredString = ""
	for i in ignoredFiles:
		ignoredString += "(" + i + ")|"
	ignoredString = ignoredString.rstrip("|")
	ignoredExpression = re.compile(ignoredString)

	# Compile new *.pyc files
	print "Compiling *.py files in", sourceDir
	result = compileall.compile_dir(sourceDir, quiet=1, rx=ignoredExpression)

	# Returns 0 on failure
	if result == 0:
		sys.exit( 1 )

	# Clear deployment directory
	print "Copying *.pyc files to", deploymentDir
	if os.path.exists(deploymentDir):
		shutil.rmtree(deploymentDir)
	os.makedirs(deploymentDir)

	# Copy new *.pyc files to deployment directory
	for (dirPath, dirNames, fileNames) in os.walk(sourceDir):
		for fileName in fileNames:
			if fileName.endswith(".pyc"):

				# Get the name of the sub-directory in the source folder
				subDir = dirPath.replace(sourceDir, "").lstrip("\\/")

				# Create a similar sub-directory in the deployment folder
				deploymentSubDir = os.path.join(deploymentDir, subDir)
				if not os.path.exists(deploymentSubDir):
					os.makedirs(deploymentSubDir)

				# Add file name to directory names
				sourceFile = os.path.join(dirPath, fileName)
				deploymentFile = os.path.join(deploymentSubDir, fileName)

				shutil.copyfile(sourceFile, deploymentFile)

if __name__ == '__main__':
	main()

