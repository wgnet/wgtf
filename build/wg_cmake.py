import argparse
import cStringIO
import getpass
import glob
import os
from stat import *
import platform
import subprocess
import sys
import misc_helper

BUILD_DIRECTORY = os.path.dirname( os.path.join( os.getcwd(), __file__ ) )
VC_XP_VARS_BAT = os.path.join( BUILD_DIRECTORY, 'vcxpvarsall.bat' )
SRC_DIRECTORY = os.path.normpath( os.path.join( BUILD_DIRECTORY, "..", 'src') )
ROOT_DIRECTORY = os.path.join( SRC_DIRECTORY, ".." )

PLATFORM = platform.system()
PLATFORM_WINDOWS = PLATFORM == 'Windows'
PLATFORM_MAC = PLATFORM == 'Darwin'

print "Platform:", PLATFORM

if PLATFORM_WINDOWS:
	CMAKE_RUN_BAT = 'rerun_cmake.bat'
	DEFAULT_CMAKE_EXE_PATH = os.path.join( SRC_DIRECTORY, 'core', 'third_party', 'cmake', 'cmake-win32-x86', 'bin' )
	CMAKE_EXE = 'cmake.exe'
elif PLATFORM_MAC:
	CMAKE_RUN_BAT = 'rerun_cmake.sh'
	DEFAULT_CMAKE_EXE_PATH = os.path.join( SRC_DIRECTORY, 'core', 'third_party', 'cmake', 'CMake.app', 'Contents', 'bin' )
	CMAKE_EXE = 'cmake'

DEFAULT_CONFIGS = [ 'Debug', 'Hybrid', 'Release' ]

# Set up MSVC x86 environment with XP support, see
# http://blogs.msdn.com/b/vcblog/archive/2012/10/08/windows-xp-targeting-with-c-in-visual-studio-2012.aspx
VC11_X86_XP_ENV = '@call %s vc11 x86\n' % (VC_XP_VARS_BAT)
VC12_X86_XP_ENV = '@call %s vc12 x86\n' % (VC_XP_VARS_BAT)
VC11_X86_64_XP_ENV = '@call %s vc11 x64\n' % (VC_XP_VARS_BAT)
VC12_X86_64_XP_ENV = '@call %s vc12 x64\n' % (VC_XP_VARS_BAT)
VC14_X86_XP_ENV = '@call %s vc14 x86\n' % (VC_XP_VARS_BAT)
VC14_X86_64_XP_ENV = '@call %s vc14 x64\n' % (VC_XP_VARS_BAT)

CMAKE_GENERATORS = dict(
	Windows = [
		dict(
			label = 'Visual Studio 2012 Win32',
			generator = 'Visual Studio 11 2012',
			dirsuffix = 'vc11_win32',
			toolset = 'v110_xp',
		),
		dict(
			label = 'Visual Studio 2012 Win64',
			generator = 'Visual Studio 11 2012',
			dirsuffix = 'vc11_win64',
			platform = "x64",
			toolset = 'v110_xp',
		),
		dict(
			label = 'Visual Studio 2013 Win32',
			generator = 'Visual Studio 12 2013',
			dirsuffix = 'vc12_win32',
			toolset = 'v120_xp',
		),
		dict(
			label = 'Visual Studio 2013 Win64',
			generator = 'Visual Studio 12 2013',
			dirsuffix = 'vc12_win64',
			platform = "x64",
			toolset = 'v120_xp',
		),
		dict(
			label = 'Visual Studio 2015 Win32',
			generator = 'Visual Studio 14 2015',
			dirsuffix = 'vc14_win32',
			toolset = 'v140_xp',
		),
		dict(
			label = 'Visual Studio 2015 Win64',
			generator = 'Visual Studio 14 2015',
			dirsuffix = 'vc14_win64',
			platform = "x64",
			toolset = 'v140_xp',
		)
	],

	Darwin = [
		dict(
			label = 'XCode',
			generator = 'Xcode',
			dirsuffix = 'xcode',
		),
		dict(
			label = 'Makefile',
			generator = 'Unix Makefiles',
			dirsuffix = 'make',
		),
	],
)

CMAKE_PLATFORM_GENERATORS = CMAKE_GENERATORS[ PLATFORM ]

QT_VERSIONS = [
	dict( label = 'Qt 5.4.2', version = '5.4.2' ),
	dict( label = 'Qt 5.5.0', version = '5.5.0' ),
	dict( label = 'Qt 5.5.1', version = '5.5.1' ),
	dict( label = 'Qt 5.6.0', version = '5.6.0' ),
	dict( label = 'Qt 5.6.1-1 (default)', version = '5.6.1-1' ),
]

YES_NO_OPTION = [
	dict( label = 'Yes', value = True ),
	dict( label = 'No' , value = False )
]

def writeBat( contents, outputPath ):
	out = open( outputPath, 'w' )
	out.write( contents )
	out.write('\n')
	out.close()
	os.chmod( outputPath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH  )


def runBat( batFile ):
	runDir = os.path.dirname( batFile )
	if PLATFORM_WINDOWS:
		process = subprocess.Popen( ['cmd.exe', '/C', batFile ], cwd=runDir )
	else:
		process = subprocess.Popen( ['/bin/sh', batFile ], cwd=runDir )
	process.communicate()
	return process.returncode == 0


def targetChoices():
	buildDir = os.path.dirname(os.path.abspath(__file__))
	targetPrefix = buildDir + '/cmake/WGConfiguration_'
	targetPrefix = targetPrefix.replace( '\\', '/' )

	def _stripName( n ):
		return n.replace( '\\', '/').replace( targetPrefix, '' ).replace( '.cmake', '' )

	targets = [ _stripName(x) for x in glob.glob( targetPrefix + '*.cmake' ) ]
	targets.sort()
	return targets


def findCMakeTargets():
	items = []
	for target in targetChoices():
		items.append( { "label" : target } )
	return items


def targetConfigs( target = None ):
	# configs to build for single config builders
	configs = DEFAULT_CONFIGS[:]
	# hardcode consumer release for client configs...
	if target == None or target == 'client' or target == 'client_blob':
		configs.append( 'Consumer_Release' )

	return configs


def generatorChoices():
	generators = []
	for generator in CMAKE_PLATFORM_GENERATORS:
		generators.append( generator['dirsuffix'] )
	generators.sort()
	return generators


def qtChoices():
	qtVersions = []
	for qtVersion in QT_VERSIONS:
		qtVersions.append( qtVersion[ 'version' ] )
	qtVersions.sort()
	return qtVersions


def chooseItem( prompt, items, deprecated = False, experimental = False, targets = None ):

	# filter out items we aren't interested in
	def displayItem( item ):
		if targets:
			for target in targets:
				if target in item.get( 'enableForTargets', [] ):
					return True
				if target in item.get( 'experimentalForTargets', [] ) and not experimental:
					return False
		if item.get( 'deprecated', False ) and not deprecated:
			return False
		if item.get( 'experimental', False ) and not experimental:
			return False
		return True
	items = filter(displayItem, items)

	if len(items) == 1:
		return items[0]

	print prompt
	for i in range( len(items) ):
		item = items[i]
		label = item['label']
		print '\t%d. %s' % (i + 1, label)
	choice = None
	while choice is None:
		try:
			choice = int(raw_input('> '))
			choice -= 1
			if choice < 0 or choice >= len( items ):
				choice = None
		except ValueError:
			choice = None

		if choice is None:
			print "Invalid option"
	return items[choice]


def editableList( prompt, items ):
	while True:
		print prompt
		for i in range( len(items) ):
			item = items[i]
			label = item[0]
			value = item[1]
			if type( value ) is tuple:
				value = value[0]
			print "\t%d. %s = %s" % (i+1, label, value)
		choice = None

		try:
			choice = int(raw_input('> '))
			choice -= 1
			if choice < 0 or choice >= len(items):
				choice = None
		except ValueError:
			choice = None

		if choice is None:
			break

		if len(items[choice]) == 2:
			items[choice][1] = raw_input(items[choice][0] + "> ")
		else:
			items[choice][1] = chooseItem(items[choice][0], items[choice][2])


def chooseMayaVersion():
	MAYA_VERSIONS = [
		#dict( label = 'Maya 2012', version = '2012' ),
		#dict( label = 'Maya 2013', version = '2013' ),
		dict( label = 'Maya 2014', version = '2014' ),
		dict( label = 'Maya 2015', version = '2015' ),
		dict( label = 'Maya 2016', version = '2016' ),
	]
	return chooseItem( "Which Maya version you want to build with ?", MAYA_VERSIONS )['version']

def chooseQtVersion():
	return chooseItem( "Which Qt version you want to build with ?", QT_VERSIONS )['version']


def buildDir( targetName, generator, buildRoot ):
	path = os.path.normpath( os.path.join( buildRoot,
		('build_%s_%s' % (targetName, generator['dirsuffix'])).lower() ) )
	if 'maya' in generator:
		path = os.path.join( path, generator['maya'] )
	return path

def writeCMakeDefaultPath( out ):
	# Add default cmake exe path
	if PLATFORM_WINDOWS:
		out.write( "@SETLOCAL\n" )
		out.write( '@set "PATH=%s;%%PATH%%"\n' % ( DEFAULT_CMAKE_EXE_PATH, ) )

	elif PLATFORM_MAC:
		out.write( 'PATH="%s:$PATH"\n' % ( DEFAULT_CMAKE_EXE_PATH, ) )

def writeGenerateBat( targetName, generator, cmakeExe, buildRoot, dryRun ):
	# create output directory
	outputDir = buildDir( targetName, generator, buildRoot )

	if not dryRun and not os.path.isdir( outputDir ):
		os.makedirs( outputDir )

	# generate cmake command list
	cmd = [
		cmakeExe, '"%s"' % SRC_DIRECTORY,
		'-Wno-dev', # disable CMakeLists.txt developer warnings
		'-G"%s"' % generator['generator'],
		'-DBW_CMAKE_TARGET=%s' % targetName,
		'-DQT_VERSION=%s' % generator['qt']
	]

	# check for asset compiler allowed hosts
	#if misc_helper.isHostnameAllowed():
	#	cmd.append('-DBW_ASSET_COMPILER_OPTIONS_ENABLE_CACHE=ON')

	# optionally append maya version
	if 'maya' in generator:
		cmd.append( '-DMAYA_VERSION=%s' % generator['maya'] )

	# optionally append toolset
	if ('toolset' in generator):
		cmd.append( '-T' )
		cmd.append( generator['toolset'] )

	# optionally append platform
	if ('platform' in generator):
		cmd.append( '-A' )
		cmd.append( generator['platform'] )

	# for single config builders (make/ninja) we need a list of configs to generate
	if generator.get('singleConfig', False):
		configs = targetConfigs( targetName )
	else:
		configs = []

	# output out command
	cmdstr = ' '.join( cmd )

	# write and execute the cmake run bat file
	outputPath = os.path.join( outputDir, CMAKE_RUN_BAT )
	out = cStringIO.StringIO()

	batchenv = generator.get('batchenv', '')
	if batchenv:
		out.write( batchenv )

	if PLATFORM_WINDOWS:
		out.write( '@pushd %~dp0\n' )

	writeCMakeDefaultPath( out )

	if configs:
		# if single config builder then create subdirs for each config
		for config in configs:
			if PLATFORM_WINDOWS:
				mkdir = '@if not exist %s\\nul mkdir %s\n' % ( config, config )
			else:
				mkdir = 'mkdir -p %s\n' % config
			out.write( mkdir )

		for config in configs:
			if PLATFORM_WINDOWS:
				out.write( 'pushd %s\n' % ( config ) )
			out.write( '%s -DCMAKE_BUILD_TYPE=%s\n' % ( cmdstr, config ) )
			if PLATFORM_WINDOWS:
				out.write( 'popd\n' )
	else:
		# otherwise write out single command
		out.write( cmdstr )

	if PLATFORM_WINDOWS:
		out.write('\n@popd\n')

	print 'writing bat> ', outputPath
	if not dryRun:
		writeBat( out.getvalue(), outputPath )
	out.close()
	os.chmod( outputPath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH )
	return outputPath


def writeBuildBat( targetName, config, generator, cmakeExe, buildRoot, rebuild, dryRun ):
	outputDir = buildDir( targetName, generator, buildRoot )

	assert os.path.isdir( outputDir ), "output directory doesn't exist"

	batchenv = generator.get('batchenv', '')

	if generator.get( 'singleConfig', False ):
		buildCmd = [ cmakeExe, '--build', '"%s"' % os.path.join( outputDir, config ) ]
		rebuildCmd = [ cmakeExe, '--build', '"%s"' % os.path.join( outputDir, config ), '--clean-first' ]
	else:
		buildCmd = [ cmakeExe, '--build', '"%s"' % outputDir, '--config', config ]
		rebuildCmd = [ cmakeExe, '--build', '"%s"' % outputDir, '--config', config, '--clean-first' ]

	buildCmdStr = ' '.join( buildCmd )
	rebuildCmdStr = ' '.join( rebuildCmd )

	if PLATFORM_WINDOWS:
		buildBatFile = 'build_%s.bat' % config.lower()
	else:
		buildBatFile = 'build_%s.sh' % config.lower()

	buildBatPath = os.path.join( outputDir, buildBatFile )

	if PLATFORM_WINDOWS:
		rebuildBatFile = 'rebuild_%s.bat' % config.lower()
	else:
		rebuildBatFile = 'rebuild_%s.sh' % config.lower()
	rebuildBatPath = os.path.join( outputDir, rebuildBatFile )

	def _writeBuildBat( outputPath, cmdstr ):
		out = cStringIO.StringIO()

		writeCMakeDefaultPath( out )

		if batchenv:
			out.write( batchenv )

		out.write( cmdstr )
		out.write('\n')
		print 'writing bat> ', outputPath
		if not dryRun:
			writeBat( out.getvalue(), outputPath )
		out.close()
		os.chmod( outputPath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH )

	_writeBuildBat( buildBatPath, buildCmdStr )
	_writeBuildBat( rebuildBatPath, rebuildCmdStr )

	# return the batch file that should be run, depending on build or rebuild option
	if rebuild:
		return rebuildBatPath
	else:
		return buildBatPath


def main():
	# parse command line options
	parser = argparse.ArgumentParser()
	parser.add_argument( 'builddir', nargs='?', type=str,
			default=ROOT_DIRECTORY, help='set root build directory' )
	parser.add_argument( '--target', action='append',
			choices=targetChoices(),
			help='generate the specified targets' )
	parser.add_argument( '--generator', action='append',
			choices=generatorChoices(),
			help='use the specified generators' )
	parser.add_argument( '--qt-version', type=str,
			choices=qtChoices(),
			help='use the specified Qt version' )
	parser.add_argument( '--build', action='store_true',
			help='build the generated targets' )
	parser.add_argument( '--rebuild', action='store_true',
			help='rebuild the generated targets' )
	parser.add_argument( '--config', action='append',
			choices=targetConfigs(),
			help='build the specified configs' )
	parser.add_argument( '--experimental', action='store_true',
			help='show experimental options' )
	parser.add_argument( '--deprecated', action='store_true',
			help='show deprecated options' )
	parser.add_argument( '--cmake-exe', type=str,
			help='specify a CMake exe to use' )
	parser.add_argument( '--dry-run', action='store_true',
			help='query build options but don\'t run CMake')
	args = parser.parse_args()

	targets = []
	generators = []

	# choose target project
	if args.target is None:
		targetItems = findCMakeTargets()
		target = chooseItem( "Which project do you want to build ?", targetItems )
		targets = [ target['label'] ]
	else:
		targets = args.target

	# choose generator
	if args.generator is None:
		generators = [ chooseItem( "Which compiler do you want to build with ?",
				CMAKE_PLATFORM_GENERATORS, args.deprecated, args.experimental,
				targets = targets ) ]
	else:
		for generator in args.generator:
			for cmake_generator in CMAKE_PLATFORM_GENERATORS:
				if cmake_generator['dirsuffix'] == generator:
					generators.append( cmake_generator )
		assert( len(generators) != 0 )

	# set cmake executable
	if args.cmake_exe:
		cmakeExe = os.path.normpath( args.cmake_exe )
	else:
		cmakeExe = CMAKE_EXE
	cmakeExe = '"%s"' % ( cmakeExe, )

	# lists of batch files to run for generation and build steps
	generateBats = []
	buildBats = []

	# write batch files
	for generator in generators:
		for target in targets:
			if args.qt_version is None:
				generator[ 'qt' ] = chooseQtVersion()
			else:
				generator[ 'qt' ] = args.qt_version
			generator[ 'dirsuffix' ] += '_qt%s' % generator[ 'qt' ]
			genBat = writeGenerateBat( target, generator, cmakeExe,
					args.builddir, args.dry_run );
			generateBats.append( genBat )
			configs = targetConfigs( target )
			for config in configs:
				buildBat = writeBuildBat( target, config, generator, cmakeExe,
						args.builddir, args.rebuild, args.dry_run )
				# only append batch file if we want to build
				if (args.build or args.rebuild) and (args.config is None or config in args.config):
					buildBats.append( buildBat )

	# run all generated batch files
	for bat in generateBats:
		print 'running bat>', bat
		if not args.dry_run:
			runBat( bat )
	for bat in buildBats:
		print 'running bat>', bat
		if not args.dry_run:
			runBat( bat )


if __name__ == '__main__':
	main()
