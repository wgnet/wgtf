CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_serialization )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

find_package(Expat REQUIRED)

SET( SERIALIZER_DETAILS_SRCS
	serializer/details/simple_api_for_xml.cpp
	serializer/details/simple_api_for_xml.hpp
	serializer/details/xml_reader.cpp
	serializer/details/xml_reader.hpp
	serializer/details/xml_writer.cpp
	serializer/details/xml_writer.hpp
)
SOURCE_GROUP( "Serializer\\details" FILES ${SERIALIZER_DETAILS_SRCS} )

SET( SERIALIZER_SRCS
	serializer/i_serializer.cpp
	serializer/i_serializer.hpp
	serializer/i_serialization_manager.hpp
	serializer/serialization_manager.cpp
	serializer/serialization_manager.hpp
	serializer/xml_serializer.cpp
	serializer/xml_serializer.hpp
)
SOURCE_GROUP( "Serializer" FILES ${SERIALIZER_SRCS} )

SET( INTERFACE_SRCS
	../../interfaces/${PROJECT_NAME}/i_file_system.hpp
	../../interfaces/${PROJECT_NAME}/i_file_info.hpp
	../../interfaces/${PROJECT_NAME}/i_file_utilities.hpp
)
SOURCE_GROUP( "Interfaces" FILES ${INTERFACE_SRCS} )

SET( ALL_SRCS
	basic_stream.cpp
	basic_stream.hpp
	binary_stream.cpp
	binary_stream.hpp
	datastreambuf.hpp
	file_data_stream.cpp
	file_data_stream.hpp
	file_info.hpp
	file_system.hpp
	fixed_memory_stream.hpp
	fixed_memory_stream.cpp
	i_datastream.cpp
	i_datastream.hpp
	resizing_memory_stream.hpp
	resizing_memory_stream.cpp
	std_data_stream.hpp
	text_stream.cpp
	text_stream.hpp
	text_stream_manip.cpp
	text_stream_manip.hpp
)

BW_BLOB_SOURCES( BLOB_SRCS ${ALL_SRCS} )

IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ELSEIF( BW_PLATFORM_MAC )
	INCLUDE( "CMakeLists.mac.txt" )
ENDIF()

SOURCE_GROUP( "Source" FILES ${BLOB_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${SERIALIZER_DETAILS_SRCS}
	${SERIALIZER_SRCS}
	${BLOB_SRCS}
	${INTERFACE_SRCS}
)

BW_ADD_LIBRARY( core_serialization ${BLOB_SRCS} )


BW_TARGET_LINK_LIBRARIES( core_serialization
	INTERFACE
	core_variant
	core_reflection

	PUBLIC
	expat
)

BW_PROJECT_CATEGORY( core_serialization "WGT Libs" )
