#ifndef COMMAND_H
#define COMMAND_H

#include "dep-man/lockfile.hpp"
#include "package.hpp"
#include "repository.hpp"

#include "argparse/argparse.hpp"

#include <filesystem>
#include <memory>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

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

		/**
		 * @brief A struct representing the context a cpm command
		 * 		  runs in
		 */
		struct Context {

			fs::path cwd;
			std::shared_ptr<Repository<Package, Package::Hash>> repo;
			std::shared_ptr<Lockfile> lockfile;
		};

		Command();

		Context context;
	};
}


#endif	// COMMAND_H
