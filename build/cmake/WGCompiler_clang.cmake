# Enable MACOSX_RPATH by default
cmake_policy(SET CMP0042 NEW)

### CLANG COMPILER FLAGS ###

# Flags used by C and C++ compilers for all build types
SET( BW_COMPILER_FLAGS
	-fvisibility-inlines-hidden
    #see https://developer.apple.com/library/mac/technotes/tn2185/_index.html
    # for detail
	-fvisibility-ms-compat
	)

# Flags used by C and C++ compilers for specific architectures	
IF( CMAKE_SIZEOF_VOID_P EQUAL 4 )
	LIST( APPEND BW_COMPILER_FLAGS 
		)
ELSEIF( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	LIST( APPEND BW_COMPILER_FLAGS
		)
ENDIF()

# Flags used by C and C++ compilers for Debug builds
SET( BW_COMPILER_FLAGS_DEBUG
	${BW_COMPILER_FLAGS}
	#-Werror
	# Preprocessor definitions
	-D_DEBUG
	-g
	-O0
	)

# Flags used by C and C++ compilers for Hybrid Consumer_Release builds
SET( BW_COMPILER_FLAGS_OPTIMIZED
	${BW_COMPILER_FLAGS}
	# Optimization
	-O3
	)

# Flags used by C and C++ compilers for Hybrid builds
SET( BW_COMPILER_FLAGS_HYBRID ${BW_COMPILER_FLAGS_OPTIMIZED} )

# Flags used by C and C++ compilers for Consumer_Release builds
SET( BW_COMPILER_FLAGS_CONSUMER_RELEASE ${BW_COMPILER_FLAGS_OPTIMIZED} )

# Flags used by the C compiler 
SET( BW_C_FLAGS $ENV{CFLAGS} ${BW_COMPILER_FLAGS} )
SET( BW_C_FLAGS_DEBUG ${BW_COMPILER_FLAGS_DEBUG} )
SET( BW_C_FLAGS_HYBRID ${BW_COMPILER_FLAGS_HYBRID} )
SET( BW_C_FLAGS_CONSUMER_RELEASE ${BW_COMPILER_FLAGS_CONSUMER_RELEASE} )

# Flags used by the C++ compiler
SET( BW_CXX_FLAGS $ENV{CXXFLAGS} ${BW_COMPILER_FLAGS}
	)
SET( BW_CXX_FLAGS_DEBUG ${BW_COMPILER_FLAGS_DEBUG} )
SET( BW_CXX_FLAGS_HYBRID ${BW_COMPILER_FLAGS_HYBRID} )
SET( BW_CXX_FLAGS_CONSUMER_RELEASE ${BW_COMPILER_FLAGS_CONSUMER_RELEASE} )


# Flags used by the linker for all build types
SET( BW_LINKER_FLAGS 

)

# Flags used by the linker for Debug builds
SET( BW_LINKER_FLAGS_DEBUG
	-g
	)

# Flags used by the linker for optimized builds
SET( BW_LINKER_FLAGS_OPTIMIZED
	)
SET( BW_LINKER_FLAGS_HYBRID ${BW_LINKER_FLAGS_OPTIMIZED} )
SET( BW_LINKER_FLAGS_CONSUMER_RELEASE ${BW_LINKER_FLAGS_OPTIMIZED} )

# Set up variables for EXE, MODULE, SHARED, and STATIC linking

# Flags used by the linker
SET( BW_EXE_LINKER_FLAGS ${BW_LINKER_FLAGS} )
SET( BW_EXE_LINKER_FLAGS_DEBUG ${BW_LINKER_FLAGS_DEBUG} )
SET( BW_EXE_LINKER_FLAGS_HYBRID ${BW_LINKER_FLAGS_HYBRID} )
SET( BW_EXE_LINKER_FLAGS_CONSUMER_RELEASE ${BW_LINKER_FLAGS_CONSUMER_RELEASE} )

# Flags used by the linker for the creation of modules.
SET( BW_MODULE_LINKER_FLAGS ${BW_LINKER_FLAGS} )
SET( BW_MODULE_LINKER_FLAGS_DEBUG ${BW_LINKER_FLAGS_DEBUG} )
SET( BW_MODULE_LINKER_FLAGS_HYBRID ${BW_LINKER_FLAGS_HYBRID} )
SET( BW_MODULE_LINKER_FLAGS_CONSUMER_RELEASE ${BW_LINKER_FLAGS_CONSUMER_RELEASE} )

# Flags used by the linker for the creation of dynamic libraries.
SET( BW_SHARED_LINKER_FLAGS ${BW_LINKER_FLAGS} )
SET( BW_SHARED_LINKER_FLAGS_DEBUG ${BW_LINKER_FLAGS_DEBUG} )
SET( BW_SHARED_LINKER_FLAGS_HYBRID ${BW_LINKER_FLAGS_HYBRID} )
SET( BW_SHARED_LINKER_FLAGS_CONSUMER_RELEASE ${BW_LINKER_FLAGS_CONSUMER_RELEASE} )

# Flags used by the linker for the creation of static libraries.
SET( BW_STATIC_LINKER_FLAGS "" )
SET( BW_STATIC_LINKER_FLAGS_DEBUG "" )
SET( BW_STATIC_LINKER_FLAGS_HYBRID "" )
SET( BW_STATIC_LINKER_FLAGS_CONSUMER_RELEASE "" )
