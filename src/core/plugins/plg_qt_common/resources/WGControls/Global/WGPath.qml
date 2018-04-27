pragma Singleton
import QtQuick 2.5

import WGControls 2.0

Item {
    WGComponent { type: "WGPath" }

	/* Replaces any / with \ and makes all lowercase for comparison */
    function normalisePath(path) {
        if (typeof(path) != "string") {
            return "";
        }
        function filterEmpty(str) { return str!= ""; }
        return path.replace(/[/]/g, "\\").split("\\").filter(filterEmpty).join("\\").toLowerCase();
    }

	/* Replaces any \ with / for friendly displaying */
    function toDisplayPath(path) {
        if (typeof(path) != "string") {
            return "";
        }
        function filterEmpty(str) { return str!= ""; }
        return path.replace(/[\\]/g, "/").split("/").filter(filterEmpty).join("/");
    }

	/* Converts path to url by replacing any \ with / and adding "file:///" */
    function pathToUrl(path) {
        if (typeof(path) != "string") {
            return "";
        }
        return "file:///" + Qt.resolvedUrl(toDisplayPath(path));
    }
	
	/* Converts url to a path by stripping the "file:///" from "file:///path/to/file */
    function urlToPath(url) {
        return url.toString().substring(8)
    }
	
	/* Get the filename of the given path with its extension */
	function getFileName(path) {
        var end = path.lastIndexOf("\\");
        if(end == -1) {
            end = path.lastIndexOf("/");
        }
        return end != -1 ? path.slice(end + 1) : path;		
	}

	/* Get the filename of the given path without its extension */
	function getFileNameWithoutExtension(path) {
        path = getFileName(path);
        return path.slice(0, path.lastIndexOf("."));
	}
	
    /* Get the parent of the given path */
    function getParentPath(path) {
        var end = path.lastIndexOf("\\");
        if(end == -1) {
            end = path.lastIndexOf("/");
        }
        return end != -1 ? path.slice(0, end) : path;
    }
	
	/* Gets a list of file paths in the following directory relative to that directory */
	function getFilesInFolder(path, filter) {
		var filters = extractFilters(filter).split('|');
		for(var i = 0; i < filters.length; ++i) {
			filters[i] = "*." + filters[i];
		}
		return filePathHelper.getFilesInFolder(path, filters);
	}

    /* Determines if a file exists */
	function fileExists(path) {
        return filePathHelper.fileExists(path);
    }

    /* Determines if multiple files exist, seperated by newline or return codes */
	function filesExist(paths) {
        return filePathHelper.filesExist(paths);
    }

	/* Gets a path relative to the module/exe. Will create if doesn't exist */
	function getApplicationFolder(folder) {
		var path = filePathHelper.applicationFolder + "/" + folder;
		if(!filePathHelper.folderExists(path)) {
			if(!filePathHelper.createFolder(path)) {
				return filePathHelper.applicationFolder;
			}
		}
		return path;
	}
	
	/* Converts a filter string from "Friendly Name (*.ext1;*.ext2)" to "ext1|ext2" */
	function extractFilters(filter) {
		filter = filter.substring(filter.indexOf("(") + 1, filter.indexOf(")"));
		filter = filter.replaceAll("*","");
		filter = filter.replaceAll(".","");
		// Support ';' and ' ' as separators for extensions
		filter = filter.replaceAll(";"," ");
		filter = filter.replaceAll("  "," ");
		filter = filter.replaceAll(" ","|");
		return filter;
	}
}
