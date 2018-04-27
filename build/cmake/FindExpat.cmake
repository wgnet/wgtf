if (EXPAT_FOUND)
    return()
endif()

set(EXPAT_FOUND 1)

add_subdirectory(${WG_TOOLS_SOURCE_DIR}/core/third_party/expat expat)
