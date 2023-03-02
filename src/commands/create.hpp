#ifndef CREATE_H
#define CREATE_H

#include "commands/command.hpp"

#include <string>


namespace cpm {

/**
 * @brief A class representing the cpm create command
 *
 * This command creates a new package in the current working directory
 */
class CreateCommand : public Command {

	public:

	/**
	 * @brief Constructor for create command
	 * 
	 * @param name the name of the command
	 */
	CreateCommand(const std::string &name);

	/**
	 * @brief Create a new package in the current working directory
	 */
	void run() override;
};

} // namespace cpm


#endif	// CREATE_H
