#ifndef COMMAND_H
#define COMMAND_H

#include "package.hpp"
#include "repository.hpp"

#include "argparse/argparse.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

	/**
	 * @brief A struct representing a cpm command context
	 */
	struct CommandContext {

		fs::path cwd;
		std::unique_ptr<Repository<Package, Package::PackageHash>> repo;
	};

	/**
	 * @brief A class representing a cpm command
	 * 
	 * All other cpm commands should derive from this class
	 */
	class Command : public argparse::ArgumentParser {

		public:

		/**
		 * @brief Constructor for command
		 * 
		 * @param name the name of the command
		 */
		Command(const std::string &name);
		virtual ~Command() = default;

		/**
		 * @brief Run the command
		 */
		virtual void run() = 0;

		friend class Parser;


		protected:

		Command();

		CommandContext context;
	};
}


#endif	// COMMAND_H
