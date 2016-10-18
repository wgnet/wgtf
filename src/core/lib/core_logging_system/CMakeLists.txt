CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( core_logging_system )

INCLUDE( WGToolsCoreProject )
INCLUDE( WGToolsCoreLibrary )

SET( ALL_SRCS
	interfaces/metadata/i_logging_model.mpp
	interfaces/i_logger.hpp
	interfaces/i_logging_system.hpp
	interfaces/i_logging_model.hpp
	alerts/i_alert_presenter.hpp
	alerts/alert_manager.cpp
	alerts/alert_manager.hpp
	alerts/basic_alert_logger.cpp
	alerts/basic_alert_logger.hpp
	log_message.cpp
	log_message.hpp
	log_level.hpp
	logging_system.cpp
	logging_system.hpp
)

WG_AUTO_SOURCE_GROUPS( ${ALL_SRCS} )

BW_BLOB_SOURCES( BLOB_SRCS
	${ALL_SRCS}
)

BW_ADD_LIBRARY( ${PROJECT_NAME} ${BLOB_SRCS} )

BW_TARGET_LINK_LIBRARIES( ${PROJECT_NAME} INTERFACE
	core_dependency_system
	core_common
	core_reflection
)

BW_PROJECT_CATEGORY( core_logging_system "WGT Libs" )
