#ifndef PARSER_H
#define PARSER_H

#include "../commands/command.hpp"

#include <map>
#include <string>


namespace cpm {

	/**
	 * @brief A map containing the command names and their corresponding objects
	 */
	extern std::map<std::string, Command *> commands;

	/**
	 * @brief Parse command line arguments
	 * 
	 * @param argc the number of arguments (equivelent to main's argc)
	 * @param argv a list of the arguments (equivelent to main's argv)
	 */
	void parse_args(int argc, char *argv[]);
}


#endif	// PARSER_H
