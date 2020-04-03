include(ExternalProject)

macro(CoraUseQtMultimedia)
    find_package(Qt5Core)
    find_package(Qt5Multimedia)
endmacro()

set(CORA_SQLITE3_PROVIDER "module" CACHE STRING "Provider of sqlite3 library")
set_property(CACHE CORA_SQLITE3_PROVIDER PROPERTY STRINGS "module" "package")

macro(CoraUseSQLite3)
    if ("${CORA_SQLITE3_PROVIDER}" STREQUAL "module")
        if (NOT SQLITE3_FOUND)
            set(SQLITE3_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite)
            set(SQLITE3_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sqlite3)
            if (EXISTS "${SQLITE3_ROOT_DIR}")
                message(STATUS "SQLITE3_ROOT_DIR:${SQLITE3_ROOT_DIR}")
                ExternalProject_Add(project_sqlite3
                    PREFIX ${SQLITE3_BINARY_DIR}
                    SOURCE_DIR ${SQLITE3_ROOT_DIR}
                    BINARY_DIR ${SQLITE3_BINARY_DIR}
                    CONFIGURE_COMMAND ${SQLITE3_ROOT_DIR}/configure
                    BUILD_COMMAND make
                    INSTALL_COMMAND "")

                set(SQLITE3_FOUND TRUE)
                set(SQLITE3_INCLUDE_DIR ${SQLITE3_BINARY_DIR})
                set(SQLITE3_LIBRARY sqlite3)
                message(STATUS "SQLITE3_INCLUDE_DIR:${SQLITE3_INCLUDE_DIR}")
                message(STATUS "SQLITE3_LIBRARY:${SQLITE3_LIBRARY}")
                link_directories(${CMAKE_CURRENT_BINARY_DIR}/sqlite3/.libs ${LINK_DIRECTORIES})
            else()
                message(WARNING "CORA_SQLITE3_PROVIDER is \"module\" but SQLITE3_ROOT_DIR is wrong: ${SQLITE3_ROOT_DIR}")
            endif()
        endif()
    elseif ("${CORA_SQLITE3_PROVIDER}" STREQUAL "package")
        if (NOT SQLITE3_FOUND)
           find_package(sqlite3)

           if (NOT SQLITE3_FOUND)
              find_package(PkgConfig REQUIRED)
              pkg_check_modules(SQLITE3 REQUIRED sqlite3)
           endif ()
       endif()
    endif()
endmacro()


set(CORA_PROTOBUF_PROVIDER "module" CACHE STRING "Provider of protobuf library")
set_property(CACHE CORA_PROTOBUF_PROVIDER PROPERTY STRINGS "module" "package")

macro(CoraUseProtobuf)
    if ("${CORA_PROTOBUF_PROVIDER}" STREQUAL "module")
        if (NOT Protobuf_FOUND)
            # This is called to include the find module for protobuf
            find_package(Protobuf QUIET)
            set(PROTOBUF_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/protobuf)
            set(PROTOBUF_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/protobuf)
            if (EXISTS "${PROTOBUF_ROOT_DIR}")
                message(STATUS "PROTOBUF_ROOT_DIR:${PROTOBUF_ROOT_DIR}")
                set(protobuf_BUILD_TESTS OFF CACHE BOOL "")
                add_subdirectory(${PROTOBUF_ROOT_DIR}/cmake ${PROTOBUF_BINARY_DIR})
                set(PROTOBUF_FOUND TRUE)
                set(PROTOBUF_INCLUDE_DIR ${PROTOBUF_ROOT_DIR}/protobuf/src)
                set(PROTOBUF_LIBRARY libprotobuf)
                set(Protobuf_PROTOC_EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/protobuf/protoc)

                message(STATUS "Protobuf_PROTOC_EXECUTABLE:${Protobuf_PROTOC_EXECUTABLE}")
                message(STATUS "PROTOBUF_INCLUDE_DIR:${PROTOBUF_INCLUDE_DIR}")
                message(STATUS "PROTOBUF_LIBRARY:${PROTOBUF_LIBRARY}")
                # TODO set protoc
            else()
                message(WARNING "CORA_PROTOBUF_PROVIDER is \"module\" but PROTOBUF_ROOT_DIR is wrong: ${PROTOBUF_ROOT_DIR}")
            endif()
        endif()
    elseif ("${CORA_PROTOBUF_PROVIDER}" STREQUAL "package")
        if (NOT Protobuf_FOUND)
           find_package(Protobuf)

           if (NOT Protobuf_FOUND)
              find_package(PkgConfig REQUIRED)
              pkg_check_modules(Protobuf REQUIRED protobuf)
           endif ()
       endif()
    endif()
endmacro()

