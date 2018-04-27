CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( curve_editor_unit_test )

INCLUDE( WGToolsCoreProject )
INCLUDE_DIRECTORIES(../)

SET( PLUGIN_SRCS
    ../models/curve_editor.hpp
    ../models/curve_editor.cpp
    ../models/curve.hpp
    ../models/curve.cpp
    ../models/point.hpp
    ../models/point.cpp
    ../models/bezier_point.hpp
    ../models/bezier_point.cpp
	../metadata/i_curve_editor.mpp
	../metadata/curve_editor.mpp
	../interpolators/interpolator_factory.cpp
	../interpolators/interpolator_factory.hpp
	../interpolators/cubic_bezier_interpolator.cpp
	../interpolators/cubic_bezier_interpolator.hpp
	../interpolators/linear_interpolator.cpp
	../interpolators/linear_interpolator.hpp
)
SOURCE_GROUP( "Plugin Source" FILES ${PLUGIN_SRCS} )

SET( ALL_SRCS
	main.cpp
	test_curve_editor.cpp
    ${PLUGIN_SRCS}
)

WG_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )
BW_ADD_EXECUTABLE( ${PROJECT_NAME} ${BLOB_SRCS} )

IF( BW_PLATFORM_WINDOWS )
	SET( PLATFORM_LIBRARIES shlwapi )
ELSEIF( BW_PLATFORM_MAC )
	SET( PLATFORM_LIBRARIES core_common )
ENDIF()

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_unit_test
	core_data_model

	# external libraries
	${PLATFORM_LIBRARIES}    
)

BW_ADD_TOOL_TEST( ${PROJECT_NAME} )
BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Unit Tests" )

