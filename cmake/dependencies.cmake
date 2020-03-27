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
            if (EXISTS "${SQLITE3_ROOT_DIR}")
                set(CMAKE_POSITION_INDEPENDENT_CODE ON)
                find_package(Threads REQUIRED)

                message(INFO "--- SQLITE3_ROOT_DIR:${SQLITE3_ROOT_DIR}")
                ExternalProject_Add(project_sqlite3
                    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/sqlite3
                    SOURCE_DIR ${SQLITE3_ROOT_DIR}
                    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/sqlite3
                    CONFIGURE_COMMAND ${SQLITE3_ROOT_DIR}/configure
                    BUILD_COMMAND make
                    INSTALL_COMMAND "")

                ExternalProject_Get_Property(project_sqlite3 install_dir)
                add_library(sqlite3 STATIC IMPORTED)
                set_property(TARGET sqlite3 PROPERTY IMPORTED_LOCATION ${install_dir}/libsqlite3.la)
                add_dependencies(sqlite3 project_sqlite3)

                set(SQLITE3_FOUND TRUE)
                set(SQLITE3_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/sqlite3)
                set(SQLITE3_LIBRARY sqlite3)
                message(INFO "--- SQLITE3_INCLUDE_DIR:${SQLITE3_INCLUDE_DIR}")
                message(INFO "--- SQLITE3_LIBRARY:${SQLITE3_LIBRARY}")
                link_directories(${CMAKE_CURRENT_BINARY_DIR}/sqlite3 ${LINK_DIRECTORIES})
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
