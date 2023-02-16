cmake_minimum_required(VERSION 3.1.0)
project(cpm VERSION 0.14.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


configure_file(cmake/version.hpp.in ${PROJECT_BINARY_DIR}/version.hpp)

add_executable(cpm
	src/cli/parser.cpp
	src/commands/command.cpp
	src/commands/create.cpp
	src/commands/install.cpp
	src/commands/list.cpp
	src/commands/remove.cpp
	src/commands/sync.cpp
	src/db/package_db.cpp
	src/script/cpm_pack.cpp
	src/main.cpp
	src/package.cpp
	src/paths.cpp
	src/semver.cpp
	src/util.cpp
)

add_subdirectory(lib/argparse)
add_subdirectory(lib/cpr)
add_subdirectory(lib/json)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/sqlite3)
add_subdirectory(lib/zip)


target_include_directories(cpm
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src
		${PROJECT_BINARY_DIR}
	 	lib/argparse/include
		lib/cpr/include
		lib/json/include
		lib/spdlog/include
		lib/sqlite3
		lib/zip/src
)

target_link_libraries(cpm
	PRIVATE
		argparse::argparse
		cpr::cpr
		nlohmann_json::nlohmann_json
		spdlog::spdlog
		sqlite3
		zip::zip
)
