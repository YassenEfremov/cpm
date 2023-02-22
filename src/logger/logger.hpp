#ifndef LOGGER_H
#define LOGGER_H

#include "spdlog/spdlog.h"

#include <memory>


#define CPM_LOG_INFO(...) cpm::FileLogger::get_file_logger()->info(__VA_ARGS__)
#define CPM_LOG_ERR(...) cpm::FileLogger::get_file_logger()->error(__VA_ARGS__)

#define CPM_INFO(...) cpm::CLILogger::get_stdout_logger()->info(__VA_ARGS__)
#define CPM_ERR(...) cpm::CLILogger::get_stderr_logger()->error(__VA_ARGS__)
#define CPM_WARN(...) cpm::CLILogger::get_stderr_logger()->warn(__VA_ARGS__)


namespace cpm {

	/**
	 * @brief The cpm console logger
	 */
	class CLILogger {

		public:

		/**
		 * @brief Initialize the logger
		 */
		static void init();

        static std::shared_ptr<spdlog::logger> get_stdout_logger();
        static std::shared_ptr<spdlog::logger> get_stderr_logger();
	};

	/**
	 * @brief The cpm rotating file logger
	 */
	class FileLogger {

		public:

		/**
		 * @brief Initialize the logger
		*/
		static void init();

        static std::shared_ptr<spdlog::logger> get_file_logger();
	};
}


#endif	// LOGGER_H
