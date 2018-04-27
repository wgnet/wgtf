CMAKE_MINIMUM_REQUIRED( VERSION 3.1.1 )
PROJECT( generic_app )

INCLUDE( WGTFAppMacros )

SET( PLUGIN_CONFIG_FILES
	config/testing/*.txt
	config/automated/*.txt
)

SET( PLUGIN_SETTINGS_FILES
	config/automated/*.settings
)

WG_APP_MAKE_GENERIC()
