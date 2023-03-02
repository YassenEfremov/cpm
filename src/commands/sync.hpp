#ifndef SYNC_H
#define SYNC_H

#include "commands/command.hpp"
#include "commands/install.hpp"
#include "commands/remove.hpp"
#include "package.hpp"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A class representing the cpm sync command
	 *
	 * This command parses the current package config and installs/removes the
	 * package dependencies listed in there
	 */
	class SyncCommand : virtual public Command,
						public InstallCommand,
						public RemoveCommand {

		public:

		/**
		 * @brief Constructor for sync command
		 * 
		 * @param name the name of the command
		 */
		SyncCommand(const std::string &name);

		/**
		 * @brief Parse the current package config and install/remove the package
		 * 		  dependencies listed in there
		 */
		void run() override;


		private:

		/**
		 * @brief Install the specified package and its dependencies. Before
		 * 		  calling this command the package should be initialized using
		 * 		  it's init() method
		 * 
		 * @param package the package to install
		 * @param output_dir the output directory
		 * 
		 * @return The number of newly installed packages
		 */
		int install_package(const Package &package,
							const fs::path &output_dir) override;

		/**
		 * @brief Perform some necessary checks before continuing with the
		 * 		  installation
		 *
		 * @param package the package to check
		 * 
		 * @return true if installed, false otherwise
		 */
		bool check_if_installed(const Package &package) override;

		/**
		 * @brief Remove the specified package and its dependencies
		 * 
		 * @param package the package to remove
		 * 
		 * @return The number of removed unspecified packages
		 */
		int remove_package(const Package &package) override;
	};
}


#endif	// SYNC_H
