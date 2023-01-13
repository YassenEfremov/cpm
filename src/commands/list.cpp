#include "list.hpp"

#include "command.hpp"
#include "../db/db.hpp"
#include "../package.hpp"
#include "../util.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;


namespace cpm {

    ListCommand::ListCommand(const std::string &name) : Command(name) {}

    void ListCommand::run(const std::vector<Package> &args) {

        // List all of the installed packages from the local DB
        PackageDB db(fs::current_path() / util::packages_dir / util::packages_db);
        auto packages = db.list();

        for (auto package : packages) {
            spdlog::info(package.get_name() + "\n");
        }
	}
}
