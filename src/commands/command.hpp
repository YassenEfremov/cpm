#ifndef COMMAND_H
#define COMMAND_H

#include "../package.hpp"

#include "argparse/argparse.hpp"

#include <string>
#include <vector>


namespace cpm {

	/**
	 * @brief A class representing a cpm command
	 */
	class Command : public argparse::ArgumentParser {

		public:

		Command(const std::string &name);
		virtual ~Command();
		

		public:

		/**
		 * @brief Run the command
		 * 
		 * @param args list of packages
		 */
		virtual void run(const std::vector<Package> &args) = 0;
	};
}


#endif	// COMMAND_H
