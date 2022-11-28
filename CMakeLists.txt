cmake_minimum_required(VERSION 3.1.0)
project(cpm VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


configure_file(cmake/version.h.in ${PROJECT_BINARY_DIR}/version.h)

add_executable(cpm
	src/main.cpp
)

target_include_directories(cpm PUBLIC ${PROJECT_BINARY_DIR})
