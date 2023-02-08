#include "commands/sync.hpp"

#include "commands/command.hpp"
#include "commands/install.hpp"
#include "commands/remove.hpp"
#include "script/cpm_pack.hpp"
#include "paths.hpp"

#include "spdlog/spdlog.h"

#include <filesystem>
#include <functional>
#include <string>
#include <set>

#include <cstdint>
#include <cstdlib>

namespace fs = std::filesystem;


namespace cpm {

    SyncCommand::SyncCommand(const std::string &name) : Command(name), InstallCommand(name), RemoveCommand(name) {}

    void SyncCommand::run() {

        CPMPack cpm_pack(fs::current_path() / paths::package_config);
        auto packages = cpm_pack.list();

        spdlog::info("Synchronizing packages with {} ...\n", paths::package_config.string());

        for (const auto &package : packages) {

            if (fs::exists(this->context.cwd / paths::packages_dir / package.name / "")) {             // TODO: check the local package database instead
                spdlog::info("{}: Already installed, skipping ...\n", package.name);
                continue;
            }

            spdlog::info(
                "{}: Installing into {} ...\n", package.name,
                (this->context.cwd / paths::packages_dir / package.name / "").string()
            );

            cpr::Response response = this->download(package,
                [](cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow,
				   cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow, std::intptr_t userdata) {
                    double downloaded = static_cast<double>(downloadNow);
                    double total = static_cast<double>(downloadTotal);
                    std::string unit = "B";
                    if (downloadNow >= 1000000) {
                        downloaded /= 1000000;
                        total /= 1000000;
                        unit = "MB";
                        spdlog::info("\r                                                        ");

                    } else if (downloadNow >= 1000) {
                        downloaded /= 1000;
                        total /= 1000;
                        unit = "KB";
                        spdlog::info("\r                                                        ");
                    }
                    if (total < downloaded) {
                        total = downloaded;
                    }
                    spdlog::info("\r    Downloading repository {0:.2f}/{1:.2f} {2}", downloaded, total, unit);
                    return true;
                }
            );
            spdlog::info(" done.\n");

            this->extract(
                response.text,
                this->context.cwd / paths::packages_dir / package.name,
                [](int currentEntry, int totalEntries) {
                    spdlog::info("\r    Extracting archive entries {}/{}", currentEntry, totalEntries);
                    return true;
                }
            );
            spdlog::info(" done.\n");
        }

        for (const auto &dir_entry : fs::directory_iterator(this->context.cwd / paths::packages_dir)) {

            Package package{dir_entry.path().filename().string()};

            if (packages.find(package) == packages.end()) {
                spdlog::info("{}: Not specified, removing ...\n", package.name);
                std::uintmax_t n = fs::remove_all(this->context.cwd / paths::packages_dir / package.name / "");
                spdlog::info("    Removed {} entries\n", n);
            }
        }
	}
}
