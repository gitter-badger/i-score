cmake_minimum_required(VERSION 3.0)

project(iscore_base)

include_directories(lib)

add_subdirectory(lib)
add_subdirectory(plugins)
if(ANDROID)
	add_subdirectory(app-android)
else()
	add_subdirectory(app)
endif()
