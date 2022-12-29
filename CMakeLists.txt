cmake_minimum_required(VERSION 3.1.0)
project(cpm VERSION 0.4.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


configure_file(cmake/version.hpp.in ${PROJECT_BINARY_DIR}/version.hpp)

add_executable(cpm
	src/main.cpp
	src/cli/parser.cpp
	src/commands/install.cpp
	src/commands/remove.cpp
)

add_subdirectory(lib/argparse)
add_subdirectory(lib/cpr)
add_subdirectory(lib/zip)
add_subdirectory(lib/spdlog)


target_include_directories(cpm
	PRIVATE
		${PROJECT_BINARY_DIR}
	 	lib/argparse/include
		lib/cpr/include
		lib/zip/src
		lib/spdlog/include
)

target_link_libraries(cpm
	PRIVATE
		argparse::argparse
		cpr::cpr
		zip::zip
		spdlog::spdlog
)
