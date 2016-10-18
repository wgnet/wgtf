CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_qt_script )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )
find_package(QT5)

SET( ALL_SRCS
	qobject_qt_type_converter.hpp
	wg_list_iterator.cpp
	wg_list_iterator.hpp
	qt_scripting_engine.cpp
	qt_scripting_engine.hpp
	qt_script_object.cpp
	qt_script_object.hpp
	script_qt_type_converter.cpp
	script_qt_type_converter.hpp
	collection_qt_type_converter.cpp
	collection_qt_type_converter.hpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_qt_script ${BLOB_SRCS}
)

BW_TARGET_LINK_LIBRARIES( core_qt_script PRIVATE
	Qt5::Core
	Qt5::Gui
	Qt5::Quick
	Qt5::Widgets
)

INCLUDE_DIRECTORIES(${Qt5Core_PRIVATE_INCLUDE_DIRS})

BW_PROJECT_CATEGORY( core_qt_script "WGT Libs" )

