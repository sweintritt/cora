if (CMAKE_COMPILER_IS_GNUCXX)
   find_program(GCOVR_EXECUTABLE NAMES gcovr)

   file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/coverage)
   set(COVERAGE_XML_FILE  "${CMAKE_CURRENT_BINARY_DIR}/coverage/coverage.xml")
   set(COVERAGE_HTML_FILE "${CMAKE_CURRENT_BINARY_DIR}/coverage/coverage.html")

   add_custom_command(
      OUTPUT  ${COVERAGE_HTML_FILE}
      COMMAND ${GCOVR_EXECUTABLE}
         --root=${CMAKE_CURRENT_SOURCE_DIR}
         --filter="${CMAKE_CURRENT_SOURCE_DIR}/.*"
         --exclude=".*/third_party/*."
         --exclude=".*/test/*."
         -o ${COVERAGE_HTML_FILE}
         --html --html-details
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating coverage HTML report: ${COVERAGE_HTML_FILE}")

   add_custom_command(
      OUTPUT  ${COVERAGE_XML_FILE}
      COMMAND ${GCOVR_EXECUTABLE}
         --root=${CMAKE_CURRENT_SOURCE_DIR}
         --filter="${CMAKE_CURRENT_SOURCE_DIR}/.*"
         --exclude=".*/third_party/.*"
         --exclude=".*/test/*."
         -o ${COVERAGE_XML_FILE}
         --xml
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating coverage XML report: ${COVERAGE_XML_FILE}")

   add_custom_target(coverage DEPENDS ${COVERAGE_XML_FILE} ${COVERAGE_HTML_FILE})
else()
   message(WARNING "Unable to generate coverage report with compiler ${CMAKE_CXX_COMPILER_ID}")
endif()
