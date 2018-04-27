CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_qt_common )

INCLUDE( WGToolsCoreProject )
find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

SET( ALL_SRCS
	plg_qt_common.cpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

SET(QRC_FILE
	resources.qrc
)
source_group("Resource Files" FILES ${QRC_FILE})
qt5_add_resources(COMPILED_RESOURCES ${QRC_FILE})
SOURCE_GROUP("Compiled Resources" FILES ${COMPILED_RESOURCES})

BW_QT_ADD_QRC(WGControls resources/WGControls)
BW_QT_ADD_QRC(private resources/private)
BW_QT_ADD_CONFIG_QRC()
BW_GLOB_RESOURCE_FILES()

FILE( GLOB PLUGIN_RESOURCES resources/fonts/*)
BW_COPY_PLUGIN_RESOURCES( "${PLUGIN_RESOURCES}" )

FILE( GLOB_RECURSE WGCONTROLS_RESOURCES resources/WGControls/*)

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_TOOL_PLUGIN( ${PROJECT_NAME} SHARED
	${BLOB_SRCS}
	${RESOURCE_FILES}
	${COMPILED_RESOURCES}
	${PLUGIN_RESOURCES}
)

IF(BW_PLATFORM_WINDOWS)
	SET(PLATFORM_LIBRARIES shlwapi)
ELSE()
	SET(PLATFORM_LIBRARIES)
ENDIF()

IF (Qt5_WEB_ENGINE_SUPPORT)
	ADD_DEFINITIONS( -DUSE_Qt5_WEB_ENGINE=1 )
	SET(Qt5_WEB_ENGINE_LIBRARY Qt5::WebEngine)
ELSE()
	SET(Qt5_WEB_ENGINE_LIBRARY)
ENDIF()

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	core_common
	core_generic_plugin
	core_qt_common
	core_ui_framework
	core_serialization_xml
	Qt5::Core
	Qt5::Gui
	Qt5::Quick
	Qt5::QuickWidgets
	Qt5::Widgets
	${Qt5_WEB_ENGINE_LIBRARY}

	# external libraries
	${PLATFORM_LIBRARIES}  
)

BW_GET_EXECUTABLE_DST_DIR(OUT_DIR)
BW_GET_DEBUG_POSTFIX( DEBUG_POSTFIX )
BW_GET_HYBRID_POSTFIX( HYBRID_POSTFIX )
BW_GET_RELEASE_POSTFIX( RELEASE_POSTFIX )

ADD_CUSTOM_COMMAND(TARGET plg_qt_common 
COMMAND ${CMAKE_COMMAND} -E copy_directory 
		${CMAKE_CURRENT_LIST_DIR}/resources/WGControls 
		${OUT_DIR}/WGControls
COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/WGControls.qrc ${OUT_DIR}
COMMAND attrib -r ${OUT_DIR}/WGControls/qmldir
COMMAND IF $<CONFIG> == "Debug" ${CMAKE_COMMAND} -E echo plugin qt_sharable${DEBUG_POSTFIX} ./../ >> ${OUT_DIR}/WGControls/qmldir
COMMAND IF $<CONFIG> == "Hybrid" ${CMAKE_COMMAND} -E echo plugin qt_sharable${HYBRID_POSTFIX} ./../ >> ${OUT_DIR}/WGControls/qmldir
COMMAND IF $<CONFIG> == "Release" ${CMAKE_COMMAND} -E echo plugin qt_sharable${RELEASE_POSTFIX} ./../ >> ${OUT_DIR}/WGControls/qmldir
COMMAND ${CMAKE_COMMAND} -E echo classname QtSharable >> ${OUT_DIR}/WGControls/qmldir
COMMAND ${CMAKE_COMMAND} -E echo typeinfo WGControls.qmltypes >> ${OUT_DIR}/WGControls/qmldir
		POST_BUILD)

BW_DEPLOY_QT()

BW_PROJECT_CATEGORY( ${PROJECT_NAME} "Plugins" )
