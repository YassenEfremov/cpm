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

# The argument EXCLUDE_FROM_ALL prevents these libraries from installing their
# own files together with CPM's files
add_subdirectory(lib/argparse EXCLUDE_FROM_ALL)
add_subdirectory(lib/cpr EXCLUDE_FROM_ALL)
add_subdirectory(lib/json EXCLUDE_FROM_ALL)
add_subdirectory(lib/spdlog EXCLUDE_FROM_ALL)
add_subdirectory(lib/sqlite3 EXCLUDE_FROM_ALL)
add_subdirectory(lib/zip EXCLUDE_FROM_ALL)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND CPM_BUILD_TESTS)
	include(CTest)
	enable_testing()
    add_subdirectory(test)
endif()

target_include_directories(cpm
	PRIVATE
		${PROJECT_SOURCE_DIR}/src
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


include(GNUInstallDirs)

install(
	TARGETS cpm
	DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
	FILES ${PROJECT_SOURCE_DIR}/data/package_locations.json
	DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
)


include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${cpm_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${cpm_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${cpm_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "${PROJECT_NAME}")

# !!WARNING!!
# When packaging on Windows MAKE SURE that CMAKE_INSTALL_PREFIX and the path
# used by the generated installer MATCH, e.g. they both point to either
# "Program Files" or "Program Files (x86)". You should check this by hand as
# currently I'm not aware of a better way to do it. We usually build a 64 bit
# executable, so on Windows the only way that I'm currently aware of to avoid
# the mismatch described above is to build with MSVC (NOT Ninja!) and specifying
# "-A x64" as the architecture. I've searched A LOT and the closest issues and
# related resources I've found are the following:

# https://stackoverflow.com/questions/68641624/why-does-cmake-default-to-program-files-x86-when-compiling-64-bit-program
# https://gitlab.kitware.com/cmake/cmake/blob/master/Source/cmLocalGenerator.cxx#L435 <-- the actual CMake code that sets CMAKE_INSTALL_PREFIX to its default value

# And in the CMake documentation
# https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html
# https://cmake.org/cmake/help/latest/cpack_gen/wix.html#variable:CPACK_WIX_ROOT_FOLDER_ID

# The variable CPACK_INSTALL_PREFIX is not documented AT ALL but the following line could fix all of out issues if it worked (it doesn't at the moment)
# set(CPACK_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

set(CPACK_WIX_UPGRADE_GUID "1EA4429B-1C1D-4488-A415-9438C71DEA1A")
list(APPEND CPACK_WIX_PATCH_FILE "${PROJECT_SOURCE_DIR}/cmake/system_path_entry_env.xml")

# Other installers
# set(CPACK_INNOSETUP_USE_MODERN_WIZARD ON)		# Requires CMake 3.27 or later
# set(CPACK_NSIS_MODIFY_PATH ON)	# Doesn't work, says that PATH is too long

include(CPack)
