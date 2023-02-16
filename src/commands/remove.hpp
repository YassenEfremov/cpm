#ifndef REMOVE_H
#define REMOVE_H

#include "commands/command.hpp"
#include "package.hpp"

#include <filesystem>
#include <string>

#include <cstdint>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A class representing the cpm remove command
	 * 
	 * This command removes the specified package
	 */
	class RemoveCommand : virtual public Command {

		public:

		/**
		 * @brief Constructor for remove command
		 * 
		 * @param name the name of the command
		 */
		RemoveCommand(const std::string &name);

		/**
		 * @brief Remove installed packages
		 */
		void run() override;


		protected:

		/**
		 * @brief Remove the specified package and its dependencies
		 * 
		 * @param package the package to remove
		 * 
		 * @return The number of records modified in the repository
		 */
		virtual int remove_package(const Package &package);

		/**
		 * @brief Perform some necessary checks before continuing with the
		 * 		  removal
		 *
		 * @param package the package to check
		 */
		virtual void check_if_not_installed(const Package &package);

		/**
		 * @brief Delete the specified package and its dependencies from the
		 * 		  filesystem
		 * 
		 * @param package package to delete
		 * 
		 * @return The number of deleted entries
		 */
		std::uintmax_t delete_package(const Package &package);

		/**
		 * @brief Unregister the specified package
		 * 
		 * @param package the package to unregister
		 * 
		 * @return The number of records modified in the repository
		 */
		int unregister_package(const Package &package);
	};
}


#endif	// REMOVE_H
