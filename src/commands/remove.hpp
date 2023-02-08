#ifndef REMOVE_H
#define REMOVE_H

#include "commands/command.hpp"

#include <string>


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
		 * @brief Remove the specified packages
		 */
		void run() override;
	};
}


#endif	// REMOVE_H
