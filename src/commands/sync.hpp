#ifndef SYNC_H
#define SYNC_H

#include "commands/command.hpp"
#include "commands/install.hpp"
#include "commands/remove.hpp"

#include <string>


namespace cpm {

	/**
	 * @brief A class representing the cpm sync command
	 *
	 * This command parses the current cpm_pack.json and installs/removes the
	 * package dependencies listed in there
	 */
	class SyncCommand : virtual public Command, public InstallCommand, public RemoveCommand {

		public:

		/**
		 * @brief Constructor for sync command
		 * 
		 * @param name the name of the command
		 */
		SyncCommand(const std::string &name);

		/**
		 * @brief Parse the current cpm_pack.json and install/remove the package
		 * 		  dependencies listed in there
		 */
		void run() override;
	};
}


#endif	// SYNC_H
