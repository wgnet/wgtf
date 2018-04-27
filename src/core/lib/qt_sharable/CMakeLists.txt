CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( qt_sharable )

ADD_DEFINITIONS( -DNOMINMAX )
ADD_DEFINITIONS( -DQT_SHARABLE_LIB )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

find_package(QT5 ${QT_FIND_PACKAGE_VERSION})

SET( PLUGIN_QT_COMMON_DIR ${CMAKE_CURRENT_LIST_DIR}/../../plugins/plg_qt_common )
SET( LIB_QT_COMMON_DIR ${CMAKE_CURRENT_LIST_DIR}/../../lib/core_qt_common )

SET( ALL_SRCS
	wg_loader.hpp
	qt_sharable.hpp
	qt_sharable.cpp
	wg_context_menu_simple.hpp
	wg_context_menu_simple.cpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_LIBRARY( qt_sharable SHARED ${BLOB_SRCS} )
ADD_DEPENDENCIES(qt_sharable plg_qt_common)

IF( BW_PLATFORM_WINDOWS )
	SET( PLATFORM_LIBRARIES shlwapi)
ELSEIF( BW_PLATFORM_MAC )
	SET(PLATFORM_LIBRARIES core_common)
ENDIF()

IF (Qt5_WEB_ENGINE_SUPPORT)
	ADD_DEFINITIONS( -DUSE_Qt5_WEB_ENGINE=1 )
	SET(Qt5_WEB_ENGINE_LIBRARY Qt5::WebEngine)
ELSE()
	SET(Qt5_WEB_ENGINE_LIBRARY)
ENDIF()

BW_TARGET_LINK_LIBRARIES( qt_sharable PRIVATE
	core_qt_common
	core_reflection
	core_data_model
	wgtf_memory
	Qt5::Core
	Qt5::Gui
	Qt5::Qml
	Qt5::QuickWidgets
	Qt5::Quick
	Qt5::UiTools
	Qt5::Xml
	${Qt5_WEB_ENGINE_LIBRARY}

	# external libraries
	${PLATFORM_LIBRARIES}
)

BW_GET_EXECUTABLE_DST_DIR(OUT_DIR)
# Only generate the .qmltypes if not on the builder
IF(BW_PLATFORM_WINDOWS AND NOT "${NGT_COMPILE_MODE}" MATCHES "builder" )
	ADD_CUSTOM_COMMAND(TARGET qt_sharable 
						COMMAND PUSHD "\"${OUT_DIR}\"" 
						COMMAND attrib -r ./WGControls/WGControls.qmltypes
						COMMAND IF NOT $<CONFIG> == "Debug" "${QT5_DIRECTORY}/bin/qmlplugindump.exe" -nonrelocatable WGControls 2.0 ./ > ./WGControls/WGControls.qmltypes
						COMMAND POPD 
						POST_BUILD)
ENDIF()
INCLUDE_DIRECTORIES(${Qt5Core_PRIVATE_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${PLUGIN_QT_COMMON_DIR})
INCLUDE_DIRECTORIES(${LIB_QT_COMMON_DIR})

BW_PROJECT_CATEGORY( qt_sharable "WGT Libs" )
