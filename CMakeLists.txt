cmake_minimum_required(VERSION 3.1.0)
project(cpm VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


configure_file(cmake/version.h.in ${PROJECT_BINARY_DIR}/version.h)

add_executable(cpm
	src/main.cpp
	src/cli/parser.cpp
)

add_subdirectory(lib/argparse)


target_include_directories(cpm
	PRIVATE
		${PROJECT_BINARY_DIR}
	 	lib/argparse/include
)

target_link_libraries(cpm
	PRIVATE
		argparse
)
