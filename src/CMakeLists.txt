target_sources(moebius PRIVATE
    vk.cpp
    initvk.cpp
    io.cpp
    draw.cpp
    window.cpp
    vertex.cpp
    descriptor.cpp
)

target_include_directories(moebius PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)