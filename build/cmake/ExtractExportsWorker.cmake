cmake_minimum_required(VERSION 2.8)

set(UNDECORATE_SYMBOLS OFF)

file(REMOVE ${OUT_FILE} )

file(APPEND ${OUT_FILE} "// WARNING! This file is generated! Do NOT edit it as all your changes can be lost!\n\n")

foreach( IN_BIN ${IN_BINS} )
	# record source binary name
	get_filename_component(IN_NAME "${IN_BIN}" NAME)

	# dump and filter necessary info from binary
	execute_process(COMMAND dumpbin /directives ${IN_BIN} OUTPUT_VARIABLE DUMPBIN_OUTPUT)
	string(REGEX MATCHALL "/EXPORT:([^\n,]*)" EXPORTS ${DUMPBIN_OUTPUT})
	#message(STATUS "EXPORTS: ${EXPORTS}")

	list(SORT EXPORTS)
	list(REMOVE_DUPLICATES EXPORTS)

	# list found exports
	if(EXPORTS)
		# logging
		message(STATUS "Listing exports in ${IN_NAME}")

		file(APPEND ${OUT_FILE} "// ${IN_NAME}\n")

		foreach(EXPORT ${EXPORTS})
			string(REGEX MATCH "/EXPORT:(.*)" EXPORT_MATCH ${EXPORT})
			set(DECORATED_EXPORT ${CMAKE_MATCH_1})

			if(UNDECORATE_SYMBOLS)
				# undecorate symbol
				execute_process(COMMAND undname ${CMAKE_MATCH_1} OUTPUT_VARIABLE UNDNAME_OUTPUT)
				string(REGEX MATCH "is :- \"([^\"]*)\"" UNDNAME_MATCH ${UNDNAME_OUTPUT})
				set(UNDECORATED_EXPORT ${CMAKE_MATCH_1})

				# yield
				file(APPEND ${OUT_FILE} "#pragma comment(linker, \"/include:${DECORATED_EXPORT}\") // ${UNDECORATED_EXPORT}\n")

				# logging
				#message(STATUS "EXPORT[${IN_NAME}]: ${DECORATED_EXPORT} (${UNDECORATED_EXPORT})")
			else()
				# yield
				file(APPEND ${OUT_FILE} "#pragma comment(linker, \"/include:${DECORATED_EXPORT}\")\n")

				# logging
				#message(STATUS "EXPORT[${IN_NAME}]: ${DECORATED_EXPORT}")
			endif()
		endforeach()

		file(APPEND ${OUT_FILE} "\n")
	endif()
endforeach()

