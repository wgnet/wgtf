#!/usr/bin/env python
import compileall
import os
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

	# Compile new *.pyc files
	print "Compiling *.py files in", sourceDir
	compileall.compile_dir(sourceDir)

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

