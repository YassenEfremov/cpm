cmake_minimum_required(VERSION 3.1.0)
project(test_cpm VERSION 1.0.0)

# list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
# include(CodeCoverage)
# append_coverage_compiler_flags()

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/Catch2 ${CMAKE_BINARY_DIR}/lib/Catch2)

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/lib/Catch2/src
	# ${CMAKE_SOURCE_DIR}/lib/argparse/include
	${CMAKE_SOURCE_DIR}/lib/cpr/include
	${CMAKE_SOURCE_DIR}/lib/json/include
	${CMAKE_SOURCE_DIR}/lib/spdlog/include
	# ${CMAKE_SOURCE_DIR}/lib/sqlite3
	# ${CMAKE_SOURCE_DIR}/lib/zip/src
	${PROJECT_BINARY_DIR}
)

link_libraries(
	Catch2::Catch2WithMain
	# argparse::argparse
	cpr::cpr
	nlohmann_json::nlohmann_json
	spdlog::spdlog
	# sqlite3
	# zip::zip
)


add_executable(test_lockfile
	test_lockfile.cpp
	${CMAKE_SOURCE_DIR}/src/logger/logger.cpp
	${CMAKE_SOURCE_DIR}/src/dep-man/lockfile.cpp
	${CMAKE_SOURCE_DIR}/src/package.cpp
	${CMAKE_SOURCE_DIR}/src/paths.cpp
	${CMAKE_SOURCE_DIR}/src/semver.cpp
	${CMAKE_SOURCE_DIR}/src/util.cpp
)

add_executable(test_package_config
	test_package_config.cpp
	${CMAKE_SOURCE_DIR}/src/logger/logger.cpp
	${CMAKE_SOURCE_DIR}/src/script/package_config.cpp
	${CMAKE_SOURCE_DIR}/src/package.cpp
	${CMAKE_SOURCE_DIR}/src/paths.cpp
	${CMAKE_SOURCE_DIR}/src/semver.cpp
	${CMAKE_SOURCE_DIR}/src/util.cpp
)

add_executable(test_semver
	test_semver.cpp
	${CMAKE_SOURCE_DIR}/src/semver.cpp
	${CMAKE_SOURCE_DIR}/src/util.cpp
)

add_executable(test_util
	test_util.cpp
	${CMAKE_SOURCE_DIR}/src/util.cpp
)

# setup_target_for_coverage_gcovr_html(
# 	NAME coverage
# 	EXECUTABLE test_util
# 	# BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src"
# 	# EXCLUDE "dir2/*"
# )

add_test(
	NAME test_lockfile
	COMMAND test_lockfile
)

add_test(
	NAME test_package_config
	COMMAND test_package_config
)

add_test(
	NAME test_semver
	COMMAND test_semver
)

add_test(
	NAME test_util
	COMMAND test_util
)
