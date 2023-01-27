#ifndef INSTALL_H
#define INSTALL_H

#include "command.hpp"
#include "../package.hpp"

#include "cpr/cpr.h"

#include <filesystem>
#include <functional>
#include <string>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A class representing the cpm install command
	 * 
	 * This command downloads the specified GitHub repository
	 */
	class InstallCommand : public Command {

		public:

		/**
		 * @brief Constructor for install command
		 * 
		 * @param name the name of the command
		 */
		InstallCommand(const std::string &name);

		/**
		 * @brief Install the specified packages
		 */
		void run() override;


		private:

		/**
		 * @brief Download the specified package repository
		 * 
		 * @param package the package to download
		 * @param progress a function to show the download progress
		 * 
		 * @return A response object
		 */
		cpr::Response download(
			const Package &package,
			std::function<bool(
				cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow,
				cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow,
				std::intptr_t userdata
			)> progress
		);

		/**
		 * @brief Extract the specified package raw zip data
		 * 
		 * @param stream the package raw zip data
		 * @param output_dir the output directory (including the package name!)
		 * @param on_extract a function to show the extraction progress
		 */
		void extract(const std::string &stream, const fs::path &output_dir,
					 bool(*on_extract)(int currentEntry, int totalEntries));
	};
}


#endif	// INSTALL_H
