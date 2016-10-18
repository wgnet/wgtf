# Commands to assist joining static libs to a shared one

# CMAKE_CURRENT_LIST_DIR points to a directory of this script only in global scope.
# In function scope it will point to a directory of the caller script.
GET_FILENAME_COMPONENT( _EXTRACT_EXPORTS_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/ExtractExportsWorker.cmake" REALPATH )

# List exported symbols from static libs and put references to them in source
# file. The source file becomes target.
# Synopsis: EXTRACT_EXPORTS( <output source file name> <static lib 1> [<static lib 2> ...] )
# WARNING! this currently works only for MSVC toolchain!
FUNCTION( EXTRACT_EXPORTS OUT_REFERENCES_FILE )
	# get list of static lib files
	FOREACH( libname ${ARGN} )
		SET( LIB_FILES ${LIB_FILES} $<TARGET_FILE:${libname}> )
	ENDFOREACH()

	# extract dllexport'ed symbols from static libs
	ADD_CUSTOM_COMMAND( OUTPUT ${OUT_REFERENCES_FILE}
		COMMAND ${CMAKE_COMMAND} -D "IN_BINS:STRING=${LIB_FILES}" -D "OUT_FILE:STRING=${OUT_REFERENCES_FILE}" -P "${_EXTRACT_EXPORTS_SCRIPT}"
		DEPENDS ${ARGN} ${_EXTRACT_EXPORTS_SCRIPT}
		COMMENT "Listing exported symbols..."
		VERBATIM
	)
ENDFUNCTION()

