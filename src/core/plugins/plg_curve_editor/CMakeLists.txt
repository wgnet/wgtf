CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_curve_editor )

INCLUDE( WGToolsProject )
INCLUDE( WGQtLibrary )

SET(ALL_SRCS
	interfaces/curve_types.hpp
	interfaces/i_curve.hpp
	interfaces/i_curve_editor.hpp
	interfaces/i_curve_interpolator.hpp
	models/bezier_point.cpp
	models/bezier_point.hpp
	models/bezier_point_data.hpp
	models/point.cpp
	models/point.hpp
	models/curve.cpp
	models/curve.hpp
	models/curve_editor.cpp
	models/curve_editor.hpp
	metadata/i_curve_editor.mpp
	interpolators/interpolator_factory.cpp
	interpolators/interpolator_factory.hpp
	interpolators/cubic_bezier_interpolator.cpp
	interpolators/cubic_bezier_interpolator.hpp
	interpolators/linear_interpolator.cpp
	interpolators/linear_interpolator.hpp
	plg_curve_editor.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_QT_ADD_QRC(plg_curve_editor resources)
BW_GLOB_RESOURCE_FILES()

BW_ADD_TOOL_PLUGIN( plg_curve_editor SHARED	
	${ALL_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${QT_RESOURCE_FILES} 
)

BW_TARGET_LINK_LIBRARIES( plg_curve_editor PRIVATE
	core_generic_plugin
	core_data_model
)

BW_PROJECT_CATEGORY( plg_curve_editor "Plugins" )

