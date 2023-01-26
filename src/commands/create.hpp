#ifndef CREATE_H
#define CREATE_H

#include "command.hpp"

#include <string>


namespace cpm {

	/**
	 * @brief A class representing the cpm create command
	 *
	 * This command creates a new package in the current working directory
	 */
	class CreateCommand : public Command {

		public:

		CreateCommand(const std::string &name);

		/**
		 * @brief Create a new package in the current working directory
		 */
		void run() override;
	};
}


#endif	// CREATE_H
