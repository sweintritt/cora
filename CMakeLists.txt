cmake_minimum_required(VERSION 3.14)

project(cora VERSION 0.7.0 LANGUAGES CXX)

option(CORA_BUILD_TESTS "Build unit tests"       ON)
option(CORA_INSTALL     "Build unit tests"       ON)
option(CORA_PACKAGE     "Build cora package"     ON)
option(CORA_DOC         "Build documentation"    ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wextra -Wall -Wundef -Wunused -Wshadow -Wfatal-errors -Wfloat-equal -ftrapv -Wunreachable-code -Weffc++")

   if (CMAKE_BUILD_TYPE MATCHES "Debug")
      message(STATUS "Activating profiling and gdb options")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -p -pg -ggdb3 -O0 --coverage")
   endif()
else()
   message(WARNING, "Unsupported compiler ${CMAKE_CXX_COMPILER_ID}. No options set.")
endif()

message(STATUS "Compiler flags: ${CMAKE_CXX_FLAGS}")

set(cora_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(cora_binary_dir ${CMAKE_CURRENT_BINARY_DIR})

include(cmake/dependencies.cmake)
include(cmake/cora.cmake)

###################
## Code Quality ###
###################
include(cmake/coverage.cmake)
include(cmake/cppcheck.cmake)

#####################
### Documentation ###
#####################
if(CORA_DOC)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
       # add a target to generate API documentation with Doxygen
       add_custom_target(doc
          ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
          COMMENT "Generating documentation with Doxygen" VERBATIM)
    endif ()
endif()

####################
### Installation ###
####################
if (CORA_INSTALL)
  if (BUILD_SHARED_LIBS)
    install(TARGETS cora
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib)
  else()
    install(TARGETS cora
            RUNTIME DESTINATION bin)
  endif()
endif ()

#################
### Packaging ###
#################
if (CORA_PACKAGE)
   include(InstallRequiredSystemLibraries)

   # General CPack settings
   set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Play internet radio streams.")
   # set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Keep track of your worktime.")
   # set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/DESCRIPTION)
   set(CPACK_PACKAGE_CONTACT "Stephan Weintritt <45856463+sweintritt@users.noreply.github.com>")
   set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
   set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
   set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
   set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

   # Debian
   set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
   # autogenerate dependency information
   set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

   # NSIS
   # set(CPACK_NSIS_INSTALL_ROOT "")
   set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME}")
   set(CPACK_NSIS_PACKAGE_NAME "${PROJECT_NAME}")
   include(CPack)
endif ()
