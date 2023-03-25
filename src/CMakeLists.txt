target_sources(moebius PRIVATE
    vko.cpp
    init.cpp
    io.cpp
    draw.cpp
    window.cpp
)

target_include_directories(moebius PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)