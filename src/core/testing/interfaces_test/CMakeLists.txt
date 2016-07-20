CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( interfaces_test )

INCLUDE( WGToolsCoreProject )

# Find includes in corresponding build directories
set( CMAKE_INCLUDE_CURRENT_DIR ON )


SET( V0_INTERFACES
	v0/auto_populate_v0.hpp
	v0/test_interface_v0.hpp
	v0/test_interface_v1.hpp
    v0/hello_interface_v0.hpp
    v0/hello_interface_v1.hpp
)
SOURCE_GROUP( "v0" FILES ${V0_INTERFACES} )


SET( V1_INTERFACES
	v1/test_interface_v0.hpp
)
SOURCE_GROUP( "v1" FILES ${V1_INTERFACES} )


SET( ALL_SRCS
	auto_populate.hpp
	test_interface.hpp
    hello_interface.hpp
	v0/auto_populate_v0.hpp
	v0/test_interface_v0.hpp
	v0/test_interface_v1.hpp
    v0/hello_interface_v0.hpp
    v0/hello_interface_v1.hpp
	v1/test_interface_v0.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

# Use ADD_CUSTOM_TARGET because this target has no cpp files
ADD_CUSTOM_TARGET( interfaces_test SOURCES
	${V0_INTERFACES}
	${V1_INTERFACES}
	${ALL_SRCS}
)
 
BW_PROJECT_CATEGORY( interfaces_test "Test Data" )

