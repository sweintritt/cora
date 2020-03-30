set(cora_sources
    ${cora_source_dir}/media_player.cpp
    ${cora_source_dir}/qt_media_player.cpp
    ${cora_source_dir}/station.cpp
    ${cora_source_dir}/sqlite_stations_dao.cpp
)

set(cora_includes
    ${cora_source_dir}
)

CoraUseQtMultimedia()
CoraUseSQLite3()
CoraUseProtobuf()

include_directories(${cora_source_dir}
    ${SQLITE3_INCLUDE_DIR}
    ${PROTOBUF_INCLUDE_DIR})

set(cora_link_libraries
    Qt5::Core
    Qt5::Multimedia
    ${SQLITE3_LIBRARY}
    ${PROTOBUF_LIBRARY})


if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wextra -Wall -Wundef -Wunused -Wshadow -Wfatal-errors -Wfloat-equal -ftrapv -Wunreachable-code -Weffc++")

   if (CMAKE_BUILD_TYPE MATCHES "Debug")
      message(STATUS "Activating profiling and gdb options")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -p -pg -ggdb3 -O0 --coverage")
   endif()
else()
   message(WARNING, "Unsupported compiler ${CMAKE_CXX_COMPILER_ID}. No options set.")
endif()

set(MAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cora ${cora_source_dir}/main.cpp ${cora_sources} ${cora_includes})
add_executable(cora::cora ALIAS cora)
target_link_libraries(cora ${cora_link_libraries})
