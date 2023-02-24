#include "commands/create.hpp"

#include "commands/command.hpp"
#include "logger/logger.hpp"
#include "script/package_config.hpp"
#include "package.hpp"
#include "paths.hpp"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

    CreateCommand::CreateCommand(const std::string &name) : Command(name) {}

    void CreateCommand::run() {

        CPM_LOG_INFO("===== Starting create command =====");

        CPM_LOG_INFO("Creating {} file ...", paths::package_config.string());
        PackageConfig cpm_pack(this->context.cwd / paths::package_config);
        Package new_package = cpm_pack.create();

        CPM_INFO("Created package \"{}\"\n", new_package.get_name());

        CPM_LOG_INFO("===== Finished create. =====");
	}
}
