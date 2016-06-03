CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_logging_system )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( INTERFACES_SRC
	interfaces/i_logger.hpp
	interfaces/i_logging_system.hpp
)
SOURCE_GROUP( "interfaces" FILES ${INTERFACES_SRC} )

SET( ALERTS_SRC
	alerts/i_alert_presenter.hpp
	alerts/alert_manager.cpp
	alerts/alert_manager.hpp
	alerts/basic_alert_logger.cpp
	alerts/basic_alert_logger.hpp
)
SOURCE_GROUP( "alerts" FILES ${ALERTS_SRC} )

SET( ALL_SRCS
	log_message.cpp
	log_message.hpp
	log_level.hpp
	logging_system.cpp
	logging_system.hpp
)
SOURCE_GROUP( "Source" FILES ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${INTERFACES_SRC}
	${ALERTS_SRC}
	${ALL_SRCS}
)
BW_ADD_LIBRARY( core_logging_system ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( core_logging_system INTERFACE
	core_dependency_system
	core_common
)

BW_PROJECT_CATEGORY( core_logging_system "WGT Libs" )
