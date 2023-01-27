#ifndef LIST_H
#define LIST_H

#include "command.hpp"

#include <string>


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
	};
}


#endif	// LIST_H
