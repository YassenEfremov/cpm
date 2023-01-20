#ifndef LIST_H
#define LIST_H

#include "command.hpp"
#include "../package.hpp"

#include <string>
#include <vector>


namespace cpm {

	/**
	 * @brief A class representing the cpm list command
	 * 
	 * This command lists all of the installed packages
	 */
	class ListCommand : public Command {

		public:

		ListCommand(const std::string &name);

		/**
		 * @brief List all of the installed packages
		 */
		void run(const std::vector<Package> &) override;
	};
}


#endif	// LIST_H
