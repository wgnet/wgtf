CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_common )

ADD_DEFINITIONS( -D_CRT_SECURE_NO_WARNINGS )

IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ELSE()
	INCLUDE( "CMakeLists.unix.txt" )
ENDIF()

SET( ALL_SRCS
	CMakeLists.unix.txt
	CMakeLists.windows.txt
	deprecated.hpp
	platform_dbg.hpp
	platform_dbg.cpp
	platform_dll.hpp
	platform_dll.cpp
	platform_env.hpp
	platform_env.cpp
	platform_path.hpp
	platform_path.cpp
	platform_std.hpp
	shared_library.cpp
	shared_library.hpp
	signal.cpp
	signal.hpp
	thread_local_value.hpp
	thread_local_value_impl.hpp
	thread_local_value.cpp
	wg_condition_variable.cpp
	wg_condition_variable.hpp
	wg_read_write_lock.cpp
	wg_read_write_lock.hpp
	${PLATFORM_SRCS}
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_common ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_common INTERFACE
	${PLATFORM_LIBRARIES}
)

#TARGET_COMPILE_DEFINITIONS( core_common PUBLIC WARN_DEPRECATED )

BW_PROJECT_CATEGORY( core_common "WGT Libs" )
