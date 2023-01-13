#ifndef INSTALL_H
#define INSTALL_H

#include "command.hpp"
#include "../package.hpp"

#include <string>
#include <vector>


namespace cpm {

	/**
	 * @brief A class representing the cpm install command
	 * 
	 * This command downloads the given GitHub repository
	 */
	class InstallCommand : public Command {

		public:

		InstallCommand(const std::string &name);

		void run(const std::vector<Package> &args) override;
	};
}


#endif	// INSTALL_H
