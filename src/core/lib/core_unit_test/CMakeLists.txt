CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_unit_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )


SET( ALL_SRCS
	TestResultBWOut.cpp
	TestResultBWOut.hpp
	multi_proc_test_case.cpp
	multi_proc_test_case.hpp
	pch.cpp
	pch.hpp
	test_application.cpp
	test_application.hpp
	unit_test.cpp
	unit_test.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_ADD_LIBRARY( core_unit_test
	${ALL_SRCS}
)


BW_TARGET_LINK_LIBRARIES( core_unit_test INTERFACE ${RESMGR_CSTDMF_TARGET_LIBS} CppUnitLite2 )

BW_PRECOMPILED_HEADER( core_unit_test pch.hpp )
BW_PROJECT_CATEGORY( core_unit_test "Unit Tests" )
