#include "commands/install.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/cpm_pack.hpp"
#include "semver.hpp"
#include "util.hpp"

#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
#include "spdlog/spdlog.h"

extern "C" {
    #include "zip.h"
}

#include <exception>
#include <filesystem>
#include <functional>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include <cstdint>
#include <cstdlib>

namespace fs = std::filesystem;
using json = nlohmann::ordered_json;


namespace cpm {

	InstallCommand::InstallCommand(const std::string &name) : Command(name) {}

	void InstallCommand::run() {

        auto packages_str = this->get<std::vector<std::string>>("packages");
        std::unordered_set<Package, Package::Hash> packages;
        for (const auto &package_str : packages_str) {
            auto tokens = util::split_string(package_str, "@");
            if (tokens.size() == 2) {
                spdlog::info("Checking package {}@{} ...", tokens[0], tokens[1]);
                Package new_package(tokens[0], SemVer(tokens[1]));
                try {
                    new_package.init();
                } catch(const std::exception &e) {
                    spdlog::info(" failed!\n");
                    throw std::invalid_argument(e.what());
                }
                spdlog::info(" done.\n");
                packages.insert(new_package);

            } else if (tokens.size() == 1) {
                spdlog::info("Resolving version for package {} ...", tokens[0]);
                Package new_package(tokens[0]);
                try {
                    new_package.init();
                } catch(const std::exception &e) {
                    spdlog::info(" failed!\n");
                    throw std::invalid_argument(e.what());
                }
                spdlog::info(" found latest: {}\n", new_package.get_version().to_string());
                packages.insert(new_package);

            } else {
                throw std::invalid_argument(package_str + ": invalid package format!");
            }
        }

        int records_modified = 0;
        for (auto package : packages) {
            try {
                records_modified += this->install_package(
                    package, this->context.cwd / paths::packages_dir / ""
                );
            
            } catch(const std::exception &e) {
                spdlog::get("stderr_logger")->error(e.what());
            }
        }

        spdlog::info(
            "{}: modified {} record/s\n",
            this->context.repo->get_filename().filename().string(), records_modified
        );
    }

	int InstallCommand::install_package(const Package &package,
                                        const fs::path &output_dir) {

        this->check_if_installed(package);

        spdlog::info(
            "Installing package into {} ...\n",
            (output_dir / package.get_name() / "").string()
        );

        this->install_deps(package, output_dir);

        return this->register_package(package);
    }

    void InstallCommand::check_if_installed(const Package &package) {
        bool specified = this->context.repo->contains(package);
        bool installed = fs::exists(this->context.cwd / paths::packages_dir / package.get_name() / "");

        if (specified && installed) {
            throw std::invalid_argument(package.get_name() + ": package already installed!");
        }
    }

	void InstallCommand::install_deps(const Package &package, const fs::path &output_dir) {
        cpr::Response response = this->download_package(package,
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

        this->extract_package(
            response.text, output_dir / package.get_name(),
            [](int currentEntry, int totalEntries) {
                spdlog::info("\r    Extracting archive entries {}/{}", currentEntry, totalEntries);
                return true;
            }
        );
        spdlog::info(" done.\n");

        for (const auto &dep : *package.get_dependencies()) {
            this->install_deps(dep, output_dir / package.get_name() / paths::packages_dir / "");
        }
    }

	cpr::Response InstallCommand::download_package(
        const Package &package,
        std::function<bool(
            cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow,
            cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow,
            std::intptr_t userdata
        )> download_progress
    ) {
        cpr::Response res = cpr::Head(
            cpr::Url{(paths::api_url / paths::owner_name / package.get_name() / "zipball" / package.get_version().to_string()).string()}
        );

        if (res.status_code != cpr::status::HTTP_OK) {
            throw std::invalid_argument(package.get_name() + ": package not found!");
        }

        res = cpr::Get(
            cpr::Url{(paths::api_url / paths::owner_name / package.get_name() / "zipball" / package.get_version().to_string()).string()},
            cpr::ProgressCallback(download_progress)
        );

        return res;
	}

	void InstallCommand::extract_package(
        const std::string &stream, const fs::path &output_dir,
        std::function<bool(int currentEntry, int totalEntries)> on_extract
    ) {
        struct zip_t *zip = zip_stream_open(stream.c_str(), stream.size(), 0, 'r');

        std::size_t n = zip_entries_total(zip);
        for (int i = 0; i < n; i++) {
            zip_entry_openbyindex(zip, i);

            std::string entry_name = zip_entry_name(zip);
            std::size_t first_slash = entry_name.find_first_of('/');
            if (zip_entry_isdir(zip)) {
                fs::create_directories(output_dir / entry_name.substr(first_slash + 1));
            }
            zip_entry_fread(zip, (output_dir / entry_name.substr(first_slash + 1)).string().c_str());
            on_extract(i + 1, n);

            zip_entry_close(zip);
        }

        zip_stream_close(zip);
	}

    int InstallCommand::register_package(const Package &package) {
        return this->context.repo->add(package);
    }
}
