pragma Singleton
import QtQuick 2.5

import WGControls 2.0

Item {
    WGComponent { type: "WGLogger" }

	/* Logs an error */
	function logError(error) {
		globalLogger.logError(error);
	}
	
	/* Logs a warning */
	function logWarning(warning) {
		globalLogger.logWarning(warning);
	}

	/* Logs a message */
	function logInfo(info) {
		globalLogger.logInfo(info);
	}	
}
