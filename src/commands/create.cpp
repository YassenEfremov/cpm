#include "commands/create.hpp"

#include "commands/command.hpp"
#include "script/cpm_pack.hpp"
#include "package.hpp"
#include "paths.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

    CreateCommand::CreateCommand(const std::string &name) : Command(name) {}

    void CreateCommand::run() {

        CPMPack cpm_pack(this->context.cwd / paths::package_config);
        Package new_package = cpm_pack.create();

        spdlog::info("Created package \"{}\"\n", new_package.name);
	}
}
