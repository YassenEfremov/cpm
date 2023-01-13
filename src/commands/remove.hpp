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
	 * This command removes the given package
	 */
	class RemoveCommand : public Command {

		public:

		RemoveCommand(const std::string &name);

		void run(const std::vector<Package> &args) override;
	};
}


#endif	// REMOVE_H
