#ifndef INSTALL_H
#define INSTALL_H

#include "command.hpp"
#include "../package.hpp"

#include "cpr/cpr.h"

#include <functional>
#include <string>
#include <vector>


namespace cpm {

	/**
	 * @brief A class representing the cpm install command
	 * 
	 * This command downloads the given GitHub repository
	 */
	class InstallCommand : public Command {

		private:

		/**
		 * @brief Download the given package repository
		 * 
		 * @param name the name of the package
		 * @param progress a function to show the download progress
		 */
		cpr::Response download(const std::string &repo_url, void(*progress)());

		/**
		 * @brief Extract the given package raw zip data
		 * 
		 * @param stream the package raw zip data
		 * @param on_extract a function to show the extraction progress
		 */
		void extract(const std::string &stream,
					 int(*on_extract)(const char *filename, void *archive));


		public:

		InstallCommand(const std::string &name);

		void run(const std::vector<Package> &args) override;
	};
}


#endif	// INSTALL_H
