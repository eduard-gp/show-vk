cmake_minimum_required(VERSION 3.16)

project(show-vk
	VERSION 1.0.0
	DESCRIPTION "Utility program to show information about Vulkan" 
	LANGUAGES CXX)

include_directories("C:/VulkanSDK/1.3.261.1/Include")

add_executable(show-vk "show-vk.cpp" "instance-vk.cpp" "error-vk.cpp")

target_compile_features(show-vk PUBLIC cxx_std_17)

target_link_directories(show-vk PRIVATE "C:/VulkanSDK/1.3.261.1/Lib")
target_link_libraries(show-vk PRIVATE vulkan-1)
