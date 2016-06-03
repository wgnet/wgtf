CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_curve_editor )

INCLUDE( WGToolsProject )
INCLUDE( WGQtLibrary )

BW_QT_ADD_QRC(plg_curve_editor resources)
BW_GLOB_RESOURCE_FILES()

FILE(GLOB_RECURSE QML_SRCS *.qml *.js *.ui *.xml)
SET(QT_RESOURCE_FILES
  ${QML_SRCS}
)

SET(INTERFACE_SRCS
	interfaces/curve_types.hpp
	interfaces/i_curve.hpp
	interfaces/i_curve_editor.hpp
	interfaces/i_curve_interpolator.hpp
)
SOURCE_GROUP("Interfaces" FILES ${INTERFACE_SRCS})

SET(MODEL_SRCS
	models/bezier_point.cpp
	models/bezier_point.hpp
	models/bezier_point_data.hpp
	models/point.cpp
	models/point.hpp
	models/curve.cpp
	models/curve.hpp
	models/curve_editor.cpp
	models/curve_editor.hpp
)
SOURCE_GROUP("Models" FILES ${MODEL_SRCS})

SET(METADATA_SRCS
	metadata/i_curve_editor.mpp
)
SOURCE_GROUP("Metadata" FILES ${METADATA_SRCS})

SET(INTERPOLATOR_SRCS
	interpolators/interpolator_factory.cpp
	interpolators/interpolator_factory.hpp
	interpolators/cubic_bezier_interpolator.cpp
	interpolators/cubic_bezier_interpolator.hpp
	interpolators/linear_interpolator.cpp
	interpolators/linear_interpolator.hpp
)
SOURCE_GROUP("Interpolators" FILES ${INTERPOLATOR_SRCS})

SET( ALL_SRCS
	plg_curve_editor.cpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )


BW_ADD_TOOL_PLUGIN( plg_curve_editor SHARED	
	${ALL_SRCS}
	${INTERFACE_SRCS}
	${MODEL_SRCS}
	${METADATA_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${INTERPOLATOR_SRCS}
	${QT_RESOURCE_FILES} 
)

BW_TARGET_LINK_LIBRARIES( plg_curve_editor PRIVATE
	core_generic_plugin
	core_qt_common
	core_qt_script
	core_ui_framework
	Qt5::Core
    Qt5::Gui
    Qt5::QuickWidgets
    Qt5::Quick

)

BW_PROJECT_CATEGORY( plg_curve_editor "Plugins" )

