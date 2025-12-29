cmake_minimum_required(VERSION 3.7)
project(VulkanTest VERSION 1.0.0 LANGUAGES C CXX)

option(VK_TEST_DEBUG "Enable debugging" OFF)

find_package(glfw3 3.4 REQUIRED)
find_package(Vulkan 1.3 REQUIRED COMPONENTS volk)

set(VKTEST_SRC_FILES
    src/App.cpp
    src/AppGraphics.cpp
    src/GPU.cpp
    src/Main.cpp
    src/VolkImpl.cpp
)

message(STATUS "Debugging: ${VK_TEST_DEBUG}")

add_executable(VkTest ${VKTEST_SRC_FILES})
target_compile_features(VkTest PRIVATE cxx_std_20)
target_include_directories(VkTest PRIVATE include)
target_link_libraries(VkTest PRIVATE glfw Vulkan::volk)
if(VK_TEST_DEBUG)
    target_compile_definitions(VkTest PRIVATE VK_TEST_DEBUG)
endif()