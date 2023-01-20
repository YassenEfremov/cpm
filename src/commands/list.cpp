#include "list.hpp"

#include "command.hpp"
#include "../db/package_db.hpp"
#include "../package.hpp"
#include "../script/cpm_pack.hpp"
#include "../util.hpp"

#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;


namespace cpm {

    ListCommand::ListCommand(const std::string &name) : Command(name) {}

    void ListCommand::run(const std::vector<Package> &) {

        CPMPack cpm_pack(fs::current_path() / util::package_config);
        auto packages = cpm_pack.list();

        for (auto p : packages) {
            spdlog::info(p.get_name() + "\n");
        }

        // PackageDB db(fs::current_path() / util::packages_dir / util::packages_db);
        // auto packages = db.list();

        // for (auto package : packages) {
        //     spdlog::info(package.get_name() + "\n");
        // }
	}
}
