set(cora_sources
    ${cora_source_dir}/cora.cpp
    
    ${cora_source_dir}/station.cpp
    ${cora_source_dir}/sqlite_stations_dao.cpp
    ${cora_source_dir}/radio_browser_info_stations_dao.cpp
    ${cora_source_dir}/utils.cpp
    ${cora_source_dir}/cli/cli.cpp
    ${cora_source_dir}/commands/command.cpp
    ${cora_source_dir}/commands/command_interpreter.cpp
    ${cora_source_dir}/commands/import_command.cpp
    ${cora_source_dir}/commands/find_command.cpp
    ${cora_source_dir}/commands/info_command.cpp
    ${cora_source_dir}/commands/help_command.cpp
    ${cora_source_dir}/commands/list_command.cpp
    ${cora_source_dir}/commands/play_command.cpp
    ${cora_source_dir}/commands/version_command.cpp
    ${cora_source_dir}/importer/importer.cpp
    ${cora_source_dir}/importer/radio_sure_importer.cpp
    ${cora_source_dir}/logging/message_only_formatter.cpp
    ${cora_source_dir}/player/media_player.cpp
    ${cora_source_dir}/player/gstreamer_media_player.cpp
)

CoraUseGstreamer()
CoraUseSQLite3()
CoraUseCurl()
find_package(Threads REQUIRED)

configure_file(${cora_source_dir}/version.hpp.in ${CMAKE_BINARY_DIR}/generated/version.hpp)

include_directories(
    ${cora_source_dir}
    ${CMAKE_BINARY_DIR}/generated/
    ${GSTREAMER_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/plog/include
)

set(cora_link_libraries
    ${GSTREAMER_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    ${CURL_LIBRARIES}
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
      ${SQLITE3_INCLUDE_DIRS}
      ${CPPUTEST_INCLUDE_DIRS}
      ${CMAKE_CURRENT_SOURCE_DIR}/third_party/plog/include)

    set(cora_test_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/test/cli_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/sqlite_stations_dao_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/utils_test.cpp)

   add_executable(cora_test ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp ${cora_sources} ${cora_test_sources})
   target_link_libraries(cora_test ${cora_link_libraries} ${CPPUTEST_LIBRARIES})
   add_test(NAME cora_test_test COMMAND cora_test)
   set_tests_properties(cora_test_test PROPERTIES PASS_REGULAR_EXPRESSION "OK")
endif ()

