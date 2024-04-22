cmake_minimum_required(VERSION 3.27.0)
project(cpm_test VERSION 1.0.1)

# list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
# include(CodeCoverage)
# append_coverage_compiler_flags()

add_subdirectory(${cpm_SOURCE_DIR}/lib/Catch2 ${cpm_BINARY_DIR}/lib/Catch2)

include_directories(
	${cpm_SOURCE_DIR}/src
	${cpm_SOURCE_DIR}/lib/Catch2/src
	# ${cpm_SOURCE_DIR}/lib/argparse/include
	${cpm_SOURCE_DIR}/lib/cpr/include
	${cpm_SOURCE_DIR}/lib/json/include
	${cpm_SOURCE_DIR}/lib/spdlog/include
	# ${cpm_SOURCE_DIR}/lib/sqlite3
	# ${cpm_SOURCE_DIR}/lib/zip/src
	${cpm_BINARY_DIR}
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
	${cpm_SOURCE_DIR}/src/logger/logger.cpp
	${cpm_SOURCE_DIR}/src/dep-man/lockfile.cpp
	${cpm_SOURCE_DIR}/src/package.cpp
	${cpm_SOURCE_DIR}/src/paths.cpp
	${cpm_SOURCE_DIR}/src/semver.cpp
	${cpm_SOURCE_DIR}/src/util.cpp
)

add_executable(test_package_config
	test_package_config.cpp
	${cpm_SOURCE_DIR}/src/logger/logger.cpp
	${cpm_SOURCE_DIR}/src/script/package_config.cpp
	${cpm_SOURCE_DIR}/src/package.cpp
	${cpm_SOURCE_DIR}/src/paths.cpp
	${cpm_SOURCE_DIR}/src/semver.cpp
	${cpm_SOURCE_DIR}/src/util.cpp
)

add_executable(test_semver
	test_semver.cpp
	${cpm_SOURCE_DIR}/src/semver.cpp
	${cpm_SOURCE_DIR}/src/util.cpp
)

add_executable(test_util
	test_util.cpp
	${cpm_SOURCE_DIR}/src/util.cpp
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
