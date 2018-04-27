CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_common )

# Setup wgCore
IF( WG_CORE_SUPPORT )
	SET( WG_CORE_COMPONENTS wg_debug wg_platform wg_utils wg_logging wg_types )
	SET( WG_DEBUG_IMPLEMENTATION_LIBRARY wg_debug_callback )
	SET( WG_BIN_DIR "${NGT_OUTPUT_DIR}" )

	# compile wgCore as a dll
	SET( WG_CORE_DLL ON )
	
	FIND_PACKAGE( wgCore COMPONENTS ${WG_CORE_COMPONENTS} REQUIRED )
	LIST( APPEND BW_PROJECTS ${WG_CORE_INTEGRATION_PROJECT} )	
ENDIF()

ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ELSE()
	INCLUDE( "CMakeLists.unix.txt" )
ENDIF()

SET( ALL_SRCS
	CMakeLists.unix.txt
	CMakeLists.windows.txt
	assert.hpp
	deprecated.hpp
	function_traits.hpp
	holder_collection.hpp
	connection_holder.hpp
	platform_dbg.hpp
	platform_dbg.cpp
	platform_dll.hpp
	platform_dll.cpp
	platform_env.hpp
	platform_env.cpp
	platform_export.hpp
	platform_path.hpp
	platform_path.cpp
	platform_std.hpp
	scoped_stop_watch.hpp
	scoped_stop_watch.cpp
	shared_library.cpp
	shared_library.hpp
	signal.cpp
	signal.hpp
	strong_type.hpp
	thread_local_value.hpp
	thread_local_value_impl.hpp
	thread_local_value.cpp
	wg_condition_variable.cpp
	wg_condition_variable.hpp
	wg_dlink.cpp
	wg_dlink.hpp
	wg_future.hpp
	wg_read_write_lock.cpp
	wg_read_write_lock.hpp
	objects_pool.hpp
	${PLATFORM_SRCS}
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_common ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_common INTERFACE
    core_string_utils
	${PLATFORM_LIBRARIES}
)

#TARGET_COMPILE_DEFINITIONS( core_common PUBLIC WARN_DEPRECATED )

BW_PROJECT_CATEGORY( core_common "WGT Libs" )

IF (WG_CORE_SUPPORT)
	WGCORE_USE( core_common )
	TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} PUBLIC ${WGCORE_INCLUDE_DIRECTORIES} )
ENDIF()