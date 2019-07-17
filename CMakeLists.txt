cmake_minimum_required(VERSION 2.8)
project(test_saber)

set(CMAKE_CXX_STANDARD 11)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE MINSIZEREL)
endif (NOT CMAKE_BUILD_TYPE)

add_executable(${PROJECT_NAME}
		main.cpp
		)
