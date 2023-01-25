#ifndef REMOVE_H
#define REMOVE_H

#include "command.hpp"

#include <string>


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
		 */
		void run() override;
	};
}


#endif	// REMOVE_H
