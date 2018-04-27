CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_serialization_xml )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

find_package(Expat REQUIRED)

SET( ALL_SRCS
	simple_api_for_xml.cpp
	simple_api_for_xml.hpp
	xml_reader.cpp
	xml_reader.hpp
	xml_writer.cpp
	xml_writer.hpp
	xml_serializer.cpp
	xml_serializer.hpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_LIBRARY( core_serialization_xml ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_serialization_xml
	PUBLIC
	expat
)

ADD_DEFINITIONS( -DXML_STATIC )

BW_PROJECT_CATEGORY( core_serialization_xml "WGT Libs" )
