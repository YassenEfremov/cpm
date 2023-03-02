#include "logger.hpp"

#include "paths.hpp"

#include "spdlog/pattern_formatter.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <memory>
#include <string>


namespace cpm {

void CLILogger::init() {
	auto stdout_logger = spdlog::stdout_color_mt("stdout_logger");
	stdout_logger->set_formatter(std::make_unique<spdlog::pattern_formatter>(
		"%v", spdlog::pattern_time_type::local, ""
	));
	spdlog::set_default_logger(stdout_logger);

	auto stderr_logger = spdlog::stderr_color_mt("stderr_logger");
	stderr_logger->set_pattern("%^%l%$: %v");
}

std::shared_ptr<spdlog::logger> CLILogger::get_stdout_logger() {
	return spdlog::get("stdout_logger");
}

std::shared_ptr<spdlog::logger> CLILogger::get_stderr_logger() {
	return spdlog::get("stderr_logger");
}


void FileLogger::init() {
	auto file_logger = spdlog::rotating_logger_mt(
		"file_logger",
		(cpm::paths::global_dir / "logs" / "cpm.log").string(),
		1024 * 1024 * 5, 3
	);
}

std::shared_ptr<spdlog::logger> FileLogger::get_file_logger() {
	return spdlog::get("file_logger");
}

} // namespace cpm
