target_sources(moebius PRIVATE
    init.cpp
    io.cpp
)

target_include_directories(moebius PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)