if (CPP_UNIT_LITE_FOUND)
    return()
endif()

set(CPP_UNIT_LITE_FOUND 1)

add_subdirectory(${WG_TOOLS_SOURCE_DIR}/core/third_party/CppUnitLite2 CppUnitLite2)

# Make sure exceptions are enabled for the CppUnitLite2 target.
BW_APPEND_TARGET_PROPERTIES(CppUnitLite2 COMPILE_FLAGS "/EHsc")
