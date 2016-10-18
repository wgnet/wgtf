CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( data_model_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	test_data.hpp
	test_tree_item.cpp
	test_tree_item.hpp
	test_tree_model.cpp
	test_tree_model.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_ADD_LIBRARY( ${PROJECT_NAME}
	${ALL_SRCS}
)
 
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Test Data" )

