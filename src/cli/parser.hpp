#ifndef PARSER_H
#define PARSER_H

#include "../commands/command.hpp"
#include "../package.hpp"

#include <map>
#include <string>
#include <vector>


namespace cpm {

	/**
	 * @brief A map containing the command names and their corresponding objects
	 */
	extern std::map<std::string, Command*> commands;

	/**
	 * @brief Parse command line arguments
	 * 
	 * @return A list containing the values given to the current command
	 */
	const std::vector<Package> parse_args(int argc, char *argv[]);
}


#endif	// PARSER_H
