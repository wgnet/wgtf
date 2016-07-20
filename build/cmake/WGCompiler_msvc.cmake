### MSVC COMPILER FLAGS ###

# Flags used by C and C++ compilers for all build types
SET( BW_COMPILER_FLAGS
	# Preprocessor definitions
	/DWIN32
	/D_WINDOWS

	# General
	/W3		# Warning level 3
	/Zi		# Always generate debug information
	/MP		# Enable parallel builds
	/WX		# Enable warnings as errors
	
	# Code generation
	/Gy		# Enable function level linking

	/w34302 # Enable warning 'conversion': truncation from 'type1' to 'type2'
	/wd4251 # Disable warning 'type1' needs to have dll-interface to be used by clients of class 'type2'
	/d2Zi+	# Put local variables and inline functions into the PDB
	)

# Flags used by C and C++ compilers for specific architectures
IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
	LIST( APPEND BW_COMPILER_FLAGS
		# Code generation
		/arch:SSE2	# Streaming SIMD extensions 2
		)
ELSEIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	LIST( APPEND BW_COMPILER_FLAGS
		# Preprocessor definitions
		/D_WIN64
		)
ENDIF()

# Flags used by C and C++ compilers for Debug builds
SET( BW_COMPILER_FLAGS_DEBUG
	# Preprocessor definitions
	/D_DEBUG

	# Optimization
	/Od		# Disable optimization
	/Ob0	# Disable inline function expansion

	# Code generation
	/MDd	# Multi-threaded debug DLL runtime library
	/RTC1	# Basic runtime checks
	)

# Flags used by C and C++ compilers for Hybrid Consumer_Release builds
SET( BW_COMPILER_FLAGS_OPTIMIZED
	# Preprocessor definitions
	/DNDEBUG

	# Optimization
	/Ox		# Full optimization
	/Ob2	# Any suitable inline function expansion
	/Oi		# Enable intrinsic functions
	/Ot		# Favor fast code

	# Code generation
	/GF		# Enable string pooling
	/MD		# Multi-threaded debug DLL runtime library
	)

# Flags used by C and C++ compilers for Hybrid builds
SET( BW_COMPILER_FLAGS_HYBRID ${BW_COMPILER_FLAGS_OPTIMIZED} )
SET( BW_COMPILER_FLAGS_RELEASE ${BW_COMPILER_FLAGS_OPTIMIZED} )

# Flags used by C and C++ compilers for Consumer_Release builds
SET( BW_COMPILER_FLAGS_CONSUMER_RELEASE ${BW_COMPILER_FLAGS_OPTIMIZED} )

# Flags used by the C compiler
SET( BW_C_FLAGS $ENV{CFLAGS} ${BW_COMPILER_FLAGS} )
SET( BW_C_FLAGS_DEBUG ${BW_COMPILER_FLAGS_DEBUG} )
SET( BW_C_FLAGS_HYBRID ${BW_COMPILER_FLAGS_HYBRID} )
SET( BW_C_FLAGS_RELEASE ${BW_COMPILER_FLAGS_RELEASE} )
SET( BW_C_FLAGS_CONSUMER_RELEASE ${BW_COMPILER_FLAGS_CONSUMER_RELEASE} )

# Flags used by the C++ compiler
SET( BW_CXX_FLAGS $ENV{CXXFLAGS} ${BW_COMPILER_FLAGS}
	# Language
	/GR		# Enable Runtime Type Information

	# Code generation
	/EHsc	# Enable C++ exceptions

	# Additional options
	/w34263 # Enable virtual function is hidden warning at /W3

	# Fix for errors in Visual Studio 2012
	# "c1xx : fatal error C1027: Inconsistent values for /Ym between creation
	# and use of precompiled header"
	# http://www.ogre3d.org/forums/viewtopic.php?f=2&t=60015
	/Zm282
	)
SET( BW_CXX_FLAGS_DEBUG ${BW_COMPILER_FLAGS_DEBUG} )
SET( BW_CXX_FLAGS_HYBRID ${BW_COMPILER_FLAGS_HYBRID} )
SET( BW_CXX_FLAGS_RELEASE ${BW_COMPILER_FLAGS_RELEASE} )
SET( BW_CXX_FLAGS_CONSUMER_RELEASE ${BW_COMPILER_FLAGS_CONSUMER_RELEASE} )


### MSVC Linker Flags ###

# Flags used by the linker for all build types
SET( BW_LINKER_FLAGS

	# BWT-29980. Fix linker errors and warnings due to nvtt and new umbraruntime.lib
	# for nvtt disagreement on CRT libraries (built with libcmt, rest of libs built with msvcrt).
	# adding umbra seems to have changed linking order, which brings up this error?...
	/NODEFAULTLIB:libcmt
)

# Flags used by the linker for different arch types
IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
	LIST( APPEND BW_LINKER_FLAGS
		/MACHINE:X86 )
ELSEIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	LIST( APPEND BW_LINKER_FLAGS
		/MACHINE:X64 )
ENDIF()

# Flags used by the linker for Debug builds
SET( BW_LINKER_FLAGS_DEBUG
	# General
	/INCREMENTAL	# Enable incremental linking

	# Debugging
	/DEBUG			# Generate debug info
	)

# Flags used by the linker for optimized builds
SET( BW_LINKER_FLAGS_OPTIMIZED
	# General
	/INCREMENTAL:NO	# Disable incremental linking

	# Debugging
	/DEBUG			# Generate debug info

	# Optimization
	/OPT:REF		# Eliminate unreferenced functions and data
	/OPT:ICF		# Enable COMDAT folding
	)
SET( BW_LINKER_FLAGS_HYBRID ${BW_LINKER_FLAGS_OPTIMIZED} )
SET( BW_LINKER_FLAGS_RELEASE ${BW_LINKER_FLAGS_OPTIMIZED} )
SET( BW_LINKER_FLAGS_CONSUMER_RELEASE ${BW_LINKER_FLAGS_OPTIMIZED} )

# Set up variables for EXE, MODULE, SHARED, and STATIC linking

# Flags used by the linker
SET( BW_EXE_LINKER_FLAGS ${BW_LINKER_FLAGS} )
SET( BW_EXE_LINKER_FLAGS_DEBUG ${BW_LINKER_FLAGS_DEBUG} )
SET( BW_EXE_LINKER_FLAGS_HYBRID ${BW_LINKER_FLAGS_HYBRID} )
SET( BW_EXE_LINKER_FLAGS_RELEASE ${BW_LINKER_FLAGS_RELEASE} )
SET( BW_EXE_LINKER_FLAGS_CONSUMER_RELEASE ${BW_LINKER_FLAGS_CONSUMER_RELEASE} )

# Flags used by the linker for the creation of modules.
SET( BW_MODULE_LINKER_FLAGS ${BW_LINKER_FLAGS} )
SET( BW_MODULE_LINKER_FLAGS_DEBUG ${BW_LINKER_FLAGS_DEBUG} )
SET( BW_MODULE_LINKER_FLAGS_HYBRID ${BW_LINKER_FLAGS_HYBRID} )
SET( BW_MODULE_LINKER_FLAGS_RELEASE ${BW_LINKER_FLAGS_RELEASE} )
SET( BW_MODULE_LINKER_FLAGS_CONSUMER_RELEASE ${BW_LINKER_FLAGS_CONSUMER_RELEASE} )

# Flags used by the linker for the creation of dll's.
SET( BW_SHARED_LINKER_FLAGS ${BW_LINKER_FLAGS} )
SET( BW_SHARED_LINKER_FLAGS_DEBUG ${BW_LINKER_FLAGS_DEBUG} )
SET( BW_SHARED_LINKER_FLAGS_HYBRID ${BW_LINKER_FLAGS_HYBRID} )
SET( BW_SHARED_LINKER_FLAGS_RELEASE ${BW_LINKER_FLAGS_RELEASE} )
SET( BW_SHARED_LINKER_FLAGS_CONSUMER_RELEASE ${BW_LINKER_FLAGS_CONSUMER_RELEASE} )

# Flags used by the linker for the creation of static libraries.
SET( BW_STATIC_LINKER_FLAGS "" )
SET( BW_STATIC_LINKER_FLAGS_DEBUG "" )
SET( BW_STATIC_LINKER_FLAGS_HYBRID "" )
SET( BW_STATIC_LINKER_FLAGS_RELEASE "" )
SET( BW_STATIC_LINKER_FLAGS_CONSUMER_RELEASE "" )


### MSVC Resource Compiler Flags ###

# Flags for the resource compiler
SET( BW_RC_FLAGS
	/nologo		# Suppress startup banner
	)


IF( ${CMAKE_GENERATOR} STREQUAL "Ninja" )
	# Force compiling against XP with Ninja
	# - see http://blogs.msdn.com/b/vcblog/archive/2012/10/08/windows-xp-targeting-with-c-in-visual-studio-2012.aspx
	ADD_DEFINITIONS( -D_USING_V110_SDK71_ )
	IF( ${BW_PLATFORM} STREQUAL "win32" )
		SET( CMAKE_CREATE_WIN32_EXE "/subsystem:windows,5.01" )
	ELSEIF( ${BW_PLATFORM} STREQUAL "win64" )
		SET( CMAKE_CREATE_WIN32_EXE "/subsystem:windows,5.02" )
	ENDIF()
ENDIF()

# Do compiler specific configuration. These are generally used to link
# against .libs for a specific version of MSVC.
IF( MSVC9 )
	SET( BW_COMPILER_TOKEN "vc9" )
ELSEIF( MSVC10 )
	SET( BW_COMPILER_TOKEN "vc10" )
ELSEIF( MSVC11 )
	SET( BW_COMPILER_TOKEN "vc11" )
ELSEIF( MSVC12 )
	SET( BW_COMPILER_TOKEN "vc12" )
ELSEIF( MSVC14 )
	SET( BW_COMPILER_TOKEN "vc14" )
ELSE()
	MESSAGE( FATAL_ERROR "Compiler '${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}' not currently supported by Wargaming." )
ENDIF()

ADD_DEFINITIONS( -DNGT_ALLOCATOR )
