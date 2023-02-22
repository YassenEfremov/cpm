#include "commands/create.hpp"

#include "commands/command.hpp"
#include "logger/logger.hpp"
#include "script/cpm_pack.hpp"
#include "package.hpp"
#include "paths.hpp"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

    CreateCommand::CreateCommand(const std::string &name) : Command(name) {}

    void CreateCommand::run() {

        CPM_LOG_INFO(">>>>> Starting create command >>>>>");

        CPMPack cpm_pack(this->context.cwd / paths::package_config);
        Package new_package = cpm_pack.create();

        CPM_LOG_INFO("Created {} for package {}", paths::package_config.string(), new_package.get_name());
        CPM_INFO("Created package \"{}\"\n", new_package.get_name());

        CPM_LOG_INFO(">>>>> Finished create. >>>>>");
	}
}
