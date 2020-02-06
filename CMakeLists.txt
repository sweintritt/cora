cmake_minimum_required(VERSION 3.14)

project(cora VERSION 0.1.0 LANGUAGES C CXX)

option(${PROJECT_NAME}_WALL      "Activate compiler warnings and errors"       ON)
option(${PROJECT_NAME}_WEXTRA    "Activate more compiler warnings and errors"  ON)
option(${PROJECT_NAME}_COVERAGE  "Activate to generate gcov output"            OFF)

find_package(Qt5Core)
find_package(Qt5Multimedia)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BIN_DIR ${CMAKE_CURRENT_BINARY_DIR})

if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

   if (CMAKE_BUILD_TYPE MATCHES "Debug")
      # debug option is added by cmake
      # generate profile information
      add_definitions(-p -pg -ggdb3 -O0)
   endif()

   if (${PROJECT_NAME}_WALL)
      add_definitions(-Wall -Wpedantic)
   endif()

   if (${PROJECT_NAME}_WEXTRA)
       add_definitions(-Wextra -Wfatal-errors -Wfloat-equal -Wunused)
   endif()

   if (${PROJECT_NAME}_COVERAGE)
       # adding with add_definitions leads to linker errors
       set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
   endif()
else()
   message(FATAL_ERROR, "Unsupported compiler ${CMAKE_CXX_COMPILER_ID}")
endif()


include_directories(
    ${SOURCE_DIR}
    ${BIN_DIR}
)

set(SOURCES
)

add_executable(${PROJECT_NAME} ${SOURCE_DIR}/main.cpp ${SOURCES})
target_link_libraries(${PROJECT_NAME}
    Qt5::Core
    Qt5::Multimedia)
