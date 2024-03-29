#ifndef INSTALL_H
#define INSTALL_H

#include "commands/command.hpp"
#include "package.hpp"

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
 * This command installs the specified package
 */
class InstallCommand : virtual public Command {

	public:

	/**
	 * @brief Constructor for install command
	 * 
	 * @param name the name of the command
	 */
	InstallCommand(const std::string &name);

	/**
	 * @brief Install cpm packages from GitHub
	 */
	void run() override;


	protected:

	/**
	 * @brief Install the specified package and its dependencies. Before calling
	 * 		  this command the package should be initialized using it's init()
	 * 		  method
	 * 
	 * @param package the package to install
	 * @param output_dir the output directory
	 * 
	 * @return The number of records modified in the repository
	 */
	virtual int install_package(const Package &package,
								const fs::path &output_dir);

	/**
	 * @brief Perform some necessary checks before continuing with the
	 * 		  installation
	 *
	 * @param package the package to check
	 * 
	 * @return true if installed, false otherwise
	 */
	virtual bool check_if_installed(const Package &package);

	/**
	 * @brief Install the specified package and its dependencies
	 * 
	 * @param package the package to install
	 * @param location a GitHub profile from which the package could be
	 * 				   downloaded
	 * @param output_dir the directory in which to install the package
	 */
	void install_all(const Package &package, const std::string &location,
					 const fs::path &output_dir);

	/**
	 * @brief Download the specified package repository
	 * 
	 * @param package the package to download
	 * @param location a GitHub profile from which the package could be
	 * 				   downloaded
	 * @param progress a function to show the download progress
	 * 
	 * @return A response object
	 */
	cpr::Response download_package(
		const Package &package, const std::string location,
		std::function<bool(
			cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow,
			cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow,
			std::intptr_t userdata
		)> download_progress
	);

	/**
	 * @brief Extract the specified package raw zip data
	 * 
	 * @param stream the package raw zip data
	 * @param output_dir the output directory (including the package name!)
	 * @param on_extract a function to show the extraction progress
	 */
	void extract_package(
		const std::string &stream, const fs::path &output_dir,
		std::function<bool(int currentEntry, int totalEntries)> on_extract
	);

	/**
	 * @brief Register the specified package
	 * 
	 * @param package the package to register
	 * 
	 * @return The number of records modified in the repository
	 */
	int register_package(const Package &package);

	/**
	 * @brief Print a final message after the command has finished executing
	 * 
	 * @param packages a list of the previously installed packages
	 */
	virtual void final_message(
		const std::unordered_set<cpm::Package, cpm::Package::Hash> &packages
	);
};

} // namespace cpm


#endif	// INSTALL_H
