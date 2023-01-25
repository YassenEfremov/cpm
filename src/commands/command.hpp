#ifndef COMMAND_H
#define COMMAND_H

#include "argparse/argparse.hpp"

#include <string>


namespace cpm {

	/**
	 * @brief A class representing a cpm command
	 */
	class Command : public argparse::ArgumentParser {

		public:

		Command(const std::string &name);
		virtual ~Command() = default;

		/**
		 * @brief Run the command
		 */
		virtual void run() = 0;
	};
}


#endif	// COMMAND_H
