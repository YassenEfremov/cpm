cmake_minimum_required(VERSION 3.27.0)
project(cpm
	VERSION 1.2.0
	DESCRIPTION "A C/C++ package manager made for my diploma thesis"
	HOMEPAGE_URL "https://github.com/YassenEfremov/cpm"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(BUILD_SHARED_LIBS OFF)

option(CPM_BUILD_TESTS "Build tests" OFF)


configure_file(cmake/version.hpp.in version.hpp)
configure_file(cmake/install_prefix.hpp.in install_prefix.hpp)

add_executable(cpm
	src/cli/parser.cpp
	src/cli/progress_bar.cpp
	src/commands/command.cpp
	src/commands/create.cpp
	src/commands/install.cpp
	src/commands/list.cpp
	src/commands/remove.cpp
	src/commands/sync.cpp
	src/db/package_db.cpp
	src/dep-man/lockfile.cpp
	src/logger/logger.cpp
	src/script/package_config.cpp
	src/config.cpp
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
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND CPM_BUILD_TESTS)
	include(CTest)
	enable_testing()
    add_subdirectory(test)
endif()

target_include_directories(cpm
	PRIVATE
		${CMAKE_SOURCE_DIR}/src
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


install(
	TARGETS cpm
	DESTINATION bin
)

install(
	FILES ${CMAKE_SOURCE_DIR}/data/package_locations.json
	DESTINATION share/${PROJECT_NAME}
)


set(CPACK_INNOSETUP_USE_MODERN_WIZARD ON)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${cpm_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${cpm_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${cpm_VERSION_PATCH}")
include(CPack)
