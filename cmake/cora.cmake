set(cora_sources
    ${cora_source_dir}/cora.cpp
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
    ${cora_source_dir}/db/station.cpp
    ${cora_source_dir}/db/settings.cpp
    ${cora_source_dir}/db/dao.cpp
    ${cora_source_dir}/db/stations_dao.cpp
    ${cora_source_dir}/db/settings_dao.cpp
    ${cora_source_dir}/importer/importer.cpp
    ${cora_source_dir}/importer/radio_sure_importer.cpp
    ${cora_source_dir}/importer/radio_browser_importer.cpp
    ${cora_source_dir}/logging/message_only_formatter.cpp
    ${cora_source_dir}/player/media_player.cpp
    ${cora_source_dir}/player/gstreamer_media_player.cpp
)

CoraUseGstreamer()
CoraUseSQLite3()
CoraUseCurl()
CoraUseJson()
find_package(Threads REQUIRED)

configure_file(${cora_source_dir}/version.hpp.in ${CMAKE_BINARY_DIR}/generated/version.hpp)

set(cora_include_dirs
    ${cora_source_dir}
    ${CMAKE_BINARY_DIR}/generated/
    ${GSTREAMER_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/plog/include
    ${JSON_INCLUDE_DIRS}
)
include_directories(${cora_include_dirs})

set(cora_link_libraries
    ${GSTREAMER_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    ${CURL_LIBRARIES}
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    ${JSON_LIBRARIES}
)

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

   include_directories(SYSTEM ${cora_include_dirs} ${CMAKE_CURRENT_SOURCE_DIR}/test)

    set(cora_test_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/test/testutils/string_stream_appender.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/testutils/in_memory_stations_dao.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/testutils/in_memory_settings_dao.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/testutils/test_media_player.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/cli/cli_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/commands/play_command_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/db/settings_dao_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/db/stations_dao_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/test/utils_test.cpp
    )

   add_executable(cora_test ${CMAKE_CURRENT_SOURCE_DIR}/test/main.cpp ${cora_sources} ${cora_test_sources})
   target_link_libraries(cora_test ${cora_link_libraries} ${CPPUTEST_LIBRARIES})
   add_test(NAME cora_test_test COMMAND cora_test)
   set_tests_properties(cora_test_test PROPERTIES PASS_REGULAR_EXPRESSION "OK")
endif ()

