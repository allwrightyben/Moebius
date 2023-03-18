cmake_minimum_required(VERSION 3.24.0)
project(moebius VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(moebius main.cpp)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

target_link_libraries(moebius PRIVATE
    glfw
    Vulkan::Vulkan
)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
