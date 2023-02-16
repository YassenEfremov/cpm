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
	 * @brief A struct representing the context a cpm runs in
	 */
	struct CommandContext {

		fs::path cwd;
		std::shared_ptr<Repository<Package, Package::Hash>> repo;
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
		 * @brief Execute the command
		 */
		virtual void run() = 0;

		friend class Parser;


		protected:

		Command();

		CommandContext context;
	};
}


#endif	// COMMAND_H
