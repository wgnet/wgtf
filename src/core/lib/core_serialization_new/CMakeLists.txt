CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_serialization_new )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	serializer_new.cpp
	serializer_new.hpp
	serializationdocument.hpp
	serializationdocument.cpp
	serializationnode.hpp
	serializationnode.cpp
	serializationhandlermanager.hpp
	serializationhandlermanager.cpp
	serializationhandler.hpp
	serializationhandler.cpp
	xmlserialization/xmlserializationdocument.hpp
	xmlserialization/xmlserializationdocument.cpp
	xmlserialization/xmlserializationnode.hpp
	xmlserialization/xmlserializationnode.cpp
	serializationhandlers/variantstreamhandler.hpp
	serializationhandlers/variantstreamhandler.cpp
	serializationhandlers/reflectedhandler.hpp
	serializationhandlers/reflectedhandler.cpp
	serializationhandlers/collectionhandler.hpp
	serializationhandlers/collectionhandler.cpp
	serializationhandlers/primitivehandlers/primitivedoublehandler.hpp
	serializationhandlers/primitivehandlers/primitivedoublehandler.cpp
	serializationhandlers/primitivehandlers/primitiveinthandler.hpp
	serializationhandlers/primitivehandlers/primitiveinthandler.cpp
	serializationhandlers/primitivehandlers/primitivestringhandler.hpp
	serializationhandlers/primitivehandlers/primitivestringhandler.cpp
	serializationhandlers/primitivehandlers/primitiveuinthandler.hpp
	serializationhandlers/primitivehandlers/primitiveuinthandler.cpp
	${WG_TOOLS_INTERFACE_DIR}/${PROJECT_NAME}/ISerializerNew.hpp
	${WG_TOOLS_INTERFACE_DIR}/${PROJECT_NAME}/ISerializationHandlerManager.hpp
	${WG_TOOLS_INTERFACE_DIR}/${PROJECT_NAME}/v0/ISerializerNew_v0.hpp
)
WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

WG_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_LIBRARY( core_serialization_new ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE
	tinyxml2
)

BW_PROJECT_CATEGORY( core_serialization_new "WGT Libs" )
