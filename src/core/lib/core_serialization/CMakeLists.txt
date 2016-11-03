CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_serialization )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

ADD_DEFINITIONS( -DSERIALIZATION_EXPORT )

find_package(Expat REQUIRED)

SET( ALL_SRCS
	serializer/details/simple_api_for_xml.cpp
	serializer/details/simple_api_for_xml.hpp
	serializer/details/xml_reader.cpp
	serializer/details/xml_reader.hpp
	serializer/details/xml_writer.cpp
	serializer/details/xml_writer.hpp
	serializer/i_serializer.cpp
	serializer/i_serializer.hpp
	serializer/i_serialization_manager.hpp
	serializer/serialization_manager.cpp
	serializer/serialization_manager.hpp
	serializer/xml_serializer.cpp
	serializer/xml_serializer.hpp
	../../interfaces/${PROJECT_NAME}/i_file_system.hpp
	../../interfaces/${PROJECT_NAME}/i_file_info.hpp
	../../interfaces/${PROJECT_NAME}/i_file_utilities.hpp
    ../../interfaces/${PROJECT_NAME}/i_resource_system.hpp
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
	serialization_dll.hpp
	std_data_stream.hpp
	text_stream.cpp
	text_stream.hpp
	text_stream_manip.cpp
	text_stream_manip.hpp
	wg_types_binary_streaming.cpp
	wg_types_binary_streaming.hpp
	wg_types_text_streaming.cpp
	wg_types_text_streaming.hpp
)


IF( BW_PLATFORM_WINDOWS )
	INCLUDE( "CMakeLists.windows.txt" )
ELSEIF( BW_PLATFORM_MAC )
	INCLUDE( "CMakeLists.mac.txt" )
ENDIF()

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_LIBRARY( core_serialization ${BLOB_SRCS} )


BW_TARGET_LINK_LIBRARIES( core_serialization
	INTERFACE
	core_variant
	core_reflection

	PUBLIC
	expat
)

ADD_DEFINITIONS( -DXML_STATIC )

BW_PROJECT_CATEGORY( core_serialization "WGT Libs" )
