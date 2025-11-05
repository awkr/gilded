cmake_minimum_required(VERSION 4.0)
project(gilded)

set(CMAKE_CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
message(STATUS "vulkan version: ${Vulkan_VERSION}")

add_subdirectory(external/glfw)

add_executable(gilded main.cpp vk.cc)

target_link_libraries(gilded PRIVATE Vulkan::Vulkan glfw)
