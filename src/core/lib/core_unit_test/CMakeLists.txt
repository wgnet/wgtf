CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_unit_test )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

find_package(CppUnitLite2 REQUIRED)

SET( ALL_SRCS
	TestResultBWOut.cpp
	TestResultBWOut.hpp
	multi_proc_test_case.cpp
	multi_proc_test_case.hpp
	pch.cpp
	pch.hpp
	test_application.cpp
	test_application.hpp
	test_global_context.hpp
	test_global_context.cpp
	test_object_manager.hpp
	test_definition_manager.hpp
	test_meta_type_creator.hpp
	test_framework.hpp
	test_framework.cpp
	test_action.hpp
	unit_test.cpp
	unit_test.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_LIBRARY( core_unit_test
	${ALL_SRCS}
)


BW_TARGET_LINK_LIBRARIES( core_unit_test INTERFACE 
	core_generic_plugin_manager
	wgtf_memory
	core_object
	CppUnitLite2 )

WG_PRECOMPILED_HEADER( core_unit_test pch.hpp )
BW_PROJECT_CATEGORY( core_unit_test "Unit Tests/Framework" )
