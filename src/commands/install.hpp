#ifndef INSTALL_H
#define INSTALL_H

#include "command.hpp"
#include "../package.hpp"

#include "cpr/cpr.h"

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A class representing the cpm install command
	 * 
	 * This command downloads the specified GitHub repository
	 */
	class InstallCommand : public Command {

		private:

		/**
		 * @brief Download the specified package repository
		 * 
		 * @param url URL of a GitHub repository
		 * @param progress a function to show the download progress
		 * 
		 * @return A response object
		 */
		cpr::Response download(
			const fs::path &url,
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
		 * @param on_extract a function to show the extraction progress
		 */
		void extract(const std::string &stream,
					 int(*on_extract)(const char *filename, void *archive));


		public:

		InstallCommand(const std::string &name);

		/**
		 * @brief Install the specified packages
		 * 
		 * @param packages list of packages to install
		 */
		void run(const std::vector<Package> &packages) override;
	};
}


#endif	// INSTALL_H
