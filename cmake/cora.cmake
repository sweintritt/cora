set(cora_sources
    ${cora_source_dir}/commands/command.cpp
    ${cora_source_dir}/commands/command_interpreter.cpp
    ${cora_source_dir}/commands/list_command.cpp
    ${cora_source_dir}/commands/play_command.cpp
    ${cora_source_dir}/commands/stop_command.cpp
    ${cora_source_dir}/media_player.cpp
    ${cora_source_dir}/qt_media_player.cpp
    ${cora_source_dir}/station.cpp
    ${cora_source_dir}/sqlite_stations_dao.cpp
    ${cora_source_dir}/utils.cpp
)

CoraUseQtMultimedia()
CoraUseSQLite3()
find_package(Threads REQUIRED)

include_directories(${cora_source_dir}
    ${SQLITE3_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/plog/include
)

set(cora_link_libraries
    Qt5::Core
    Qt5::Multimedia
    ${SQLITE3_LIBRARY}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT})

if(CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -Wextra -Wall -Wundef -Wunused")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow -Wfatal-errors -Wfloat-equal")
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftrapv -Wunreachable-code -Weffc++ -std=c++11")
   message(STATUS "compiler flags: ${CMAKE_CXX_FLAGS}")

   if (CMAKE_BUILD_TYPE MATCHES "Debug")
      message(STATUS "Activating profiling and gdb options")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -p -pg -ggdb3 -O0 --coverage")
   endif()
else()
   message(WARNING, "Unsupported compiler ${CMAKE_CXX_COMPILER_ID}. No options set.")
endif()

set(MAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cora ${cora_source_dir}/main.cpp ${cora_sources})
add_executable(cora::cora ALIAS cora)
target_link_libraries(cora ${cora_link_libraries})

if(CORA_BUILD_TESTS)
    enable_testing()
    CoraUseCpputest()

   include_directories(SYSTEM
      ${SQLITE3_INCLUDE_DIR}
      ${CPPUTEST_INCLUDE_DIR}
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/plog/include)

    set(cora_test_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/test/sqlite_stations_dao_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/utils_test.cpp)

   add_executable(cora_test ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp ${cora_sources} ${cora_test_sources})
   target_link_libraries(cora_test ${cora_link_libraries} ${CPPUTEST_LIBRARY})
   add_test(NAME cora_test_test COMMAND cora_test)
   set_tests_properties(cora_test_test PROPERTIES PASS_REGULAR_EXPRESSION "OK")
endif ()

