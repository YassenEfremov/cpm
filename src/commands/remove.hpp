#ifndef REMOVE_H
#define REMOVE_H

#include "command.hpp"
#include "../package.hpp"

#include <string>
#include <vector>


namespace cpm {

	/**
	 * @brief A class representing the cpm remove command
	 * 
	 * This command removes the specified package
	 */
	class RemoveCommand : public Command {

		public:

		RemoveCommand(const std::string &name);

		/**
		 * @brief Remove the specified packages
		 * 
		 * @param packages list of packages to remove
		 */
		void run(const std::vector<Package> &packages) override;
	};
}


#endif	// REMOVE_H
