#ifndef LIST_H
#define LIST_H

#include "commands/command.hpp"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A class representing the cpm list command
	 * 
	 * This command lists all of the installed packages
	 */
	class ListCommand : public Command {

		public:

		/**
		 * @brief Constructor for list command
		 * 
		 * @param name the name of the command
		 */
		ListCommand(const std::string &name);

		/**
		 * @brief List all of the installed packages
		 */
		void run() override;


		private:

		/**
		 * @brief Print the dependencies of the specified package
		 * 
		 * @param package a package
		 * @param dir the directory where the package is installed
		 */
		void print_deps(const Package &package, const fs::path dir);
	};
}


#endif	// LIST_H
