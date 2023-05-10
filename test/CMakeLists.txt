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
	# ${CMAKE_SOURCE_DIR}/lib/cpr/include
	# ${CMAKE_SOURCE_DIR}/lib/json/include
	${CMAKE_SOURCE_DIR}/lib/spdlog/include
	# ${CMAKE_SOURCE_DIR}/lib/sqlite3
	# ${CMAKE_SOURCE_DIR}/lib/zip/src
	${PROJECT_BINARY_DIR}
)

link_libraries(
	Catch2::Catch2WithMain
	# argparse::argparse
	# cpr::cpr
	# nlohmann_json::nlohmann_json
	spdlog::spdlog
	# sqlite3
	# zip::zip
)


add_executable(test_semver
	test_semver.cpp
	${CMAKE_SOURCE_DIR}/src/semver.cpp
	${CMAKE_SOURCE_DIR}/src/util.cpp
)

add_executable(test_split_string
	test_split_string.cpp
	${CMAKE_SOURCE_DIR}/src/util.cpp
)

# setup_target_for_coverage_gcovr_html(
# 	NAME coverage
# 	EXECUTABLE test_split_string
# 	# BASE_DIRECTORY "${PROJECT_SOURCE_DIR}/src"
# 	# EXCLUDE "dir2/*"
# )

add_test(
	NAME test_semver
	COMMAND test_semver
)

add_test(
	NAME test_split_string
	COMMAND test_split_string
)
