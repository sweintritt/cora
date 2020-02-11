set(cora_sources
    ${cora_source_dir}/qt_media_player.cpp
)

set(cora_includes
    ${cora_source_dir}
)

add_executable(cora ${cora_source_dir}/main.cpp ${cora_sources} ${cora_includes})

CoraUseQtMultimedia()

include_directories(${cora_source_dir})

set(cora_link_libraries
    Qt5::Core
    Qt5::Multimedia)

target_link_libraries(cora ${cora_link_libraries})

add_executable(cora::cora ALIAS cora)
