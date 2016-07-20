CMAKE_MINIMUM_REQUIRED( VERSION 3.2.3 )
PROJECT( data_model_test )

INCLUDE( WGToolsCoreProject )

SET( ALL_SRCS
	test_data.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


# Use ADD_CUSTOM_TARGET because this target has no cpp files
ADD_CUSTOM_TARGET( ${PROJECT_NAME} SOURCES
	${ALL_SRCS}
)
 
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Test Data" )

