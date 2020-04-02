cmake_minimum_required(VERSION 3.15)
project(vulkan-test)

set(CMAKE_CXX_STANDARD 17)

find_package(Vulkan REQUIRED)

add_subdirectory(glfw-3.3/)
include_directories(glfw-3.3/include)

add_executable(vulkan-test test-vulkan.cpp)
target_link_libraries(vulkan-test Vulkan::Vulkan)

add_executable(vulkan-test2 test-glfwglm.cpp)
target_link_libraries(vulkan-test2 Vulkan::Vulkan glfw)

add_executable(vulkan-base main.cpp)
target_link_libraries(vulkan-base Vulkan::Vulkan glfw)
