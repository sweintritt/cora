find_program(CPPCHECK_EXECUTABLE NAMES cppcheck)
get_property(include_dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

set(CPPCHECK_INCLUDE_DIRS "")
foreach(include IN LISTS includes)
    set(CPPCHECK_INCLUDE_DIRS -I ${include} ${CPPCHECK_INCLUDE_DIRS})
endforeach()

set(CPPCHECK_FLAGS --std=c++11 --verbose --suppress=missingIncludeSystem --force --enable=all --language=c++)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cppcheck)
set(CPPCHECK_LOG_FILE  "${CMAKE_CURRENT_BINARY_DIR}/cppcheck/cppcheck.log")
set(CPPCHECK_XML_FILE  "${CMAKE_CURRENT_BINARY_DIR}/cppcheck/cppcheck.xml")

add_custom_command(
    OUTPUT  ${CPPCHECK_LOG_FILE}
    COMMAND ${CPPCHECK_EXECUTABLE}
       ${CMAKE_CURRENT_SOURCE_DIR}/src
       ${CPPCHECK_FLAGS}
       ${CPPCHECK_INCLUDE_DIRS}
       2> ${CPPCHECK_LOG_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating cppcheck report: ${CPPCHECK_LOG_FILE}")

add_custom_command(
    OUTPUT  ${CPPCHECK_XML_FILE}
    COMMAND ${CPPCHECK_EXECUTABLE}
       ${CMAKE_CURRENT_SOURCE_DIR}/src
       ${CPPCHECK_FLAGS}
       --xml
       ${CPPCHECK_INCLUDE_DIRS}
       2> ${CPPCHECK_XML_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating cppcheck xml report: ${CPPCHECK_XML_FILE}")

add_custom_target(cppcheck DEPENDS ${CPPCHECK_LOG_FILE} ${CPPCHECK_XML_FILE})
