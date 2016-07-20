CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( plg_custom_xml_serialization_test )

INCLUDE( WGToolsCoreProject )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET( ALL_SRCS
	plg_custom_xml_serialization_test.cpp
	custom_xml_data.cpp
	custom_xml_data.hpp
	custom_xml_reader.cpp
	custom_xml_reader.hpp
	custom_xml_writer.cpp
	custom_xml_writer.hpp
	custom_xml_serializer.cpp
	custom_xml_serializer.hpp
)
SOURCE_GROUP( "" FILES ${ALL_SRCS} )

BW_ADD_TOOL_PLUGIN( plg_custom_xml_serialization_test SHARED
	${ALL_SRCS}
)

BW_TARGET_LINK_LIBRARIES( plg_custom_xml_serialization_test PRIVATE
	core_generic_plugin
	core_reflection
	)


BW_PROJECT_CATEGORY( plg_custom_xml_serialization_test "Plugins" )
