#include "commands/install.hpp"

#include "commands/command.hpp"
#include "db/package_db.hpp"
#include "logger/logger.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/cpm_pack.hpp"
#include "semver.hpp"
#include "util.hpp"

#include "cpr/cpr.h"
#include "nlohmann/json.hpp"

extern "C" {
    #include "zip.h"
}

#include <exception>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <iostream>
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

        CPM_LOG_INFO(
            ">>>>> Starting install command with args: {} >>>>>",
            [&]() {
                std::string packages_str = "[";
                for (const auto &p : this->get<std::vector<std::string>>("packages")) {
                    packages_str += p + ", ";
                }
                return packages_str + "]";
            }()
        );

        auto packages_str = this->get<std::vector<std::string>>("packages");
        std::unordered_set<Package, Package::Hash> packages;
        for (const auto &package_str : packages_str) {
            auto tokens = util::split_string(package_str, "@");
            if (tokens.size() == 2) {
                CPM_INFO("Checking package {}@{} ...", tokens[0], tokens[1]);
                Package new_package(tokens[0], SemVer(tokens[1]));
                try {
                    new_package.init();
                } catch(const std::exception &e) {
                    CPM_INFO(" failed!\n");
                    throw std::invalid_argument(e.what());
                }
                CPM_LOG_INFO(
                    "version {} for package {} is valid",
                    new_package.get_version().to_string(), new_package.get_name()
                );
                CPM_INFO(" done.\n");
                packages.insert(new_package);

            } else if (tokens.size() == 1) {
                CPM_INFO("Resolving version for package {} ...", tokens[0]);
                Package new_package(tokens[0]);
                try {
                    new_package.init();
                } catch(const std::exception &e) {
                    CPM_INFO(" failed!\n");
                    throw std::invalid_argument(e.what());
                }
                CPM_LOG_INFO(
                    "found version {} (latest) for package {}",
                    new_package.get_version().to_string(), new_package.get_name()
                );
                CPM_INFO(" found latest: \x1b[33m{}\x1b[37m\n", new_package.get_version().to_string());
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
                CPM_LOG_INFO(e.what());
                CPM_ERR(e.what());
            }
        }

        CPM_INFO(
            "{}: modified {} record/s\n",
            this->context.repo->get_filename().filename().string(), records_modified
        );

        CPM_LOG_INFO("<<<<< Finished install. <<<<<");

        this->final_message(packages);
    }

	int InstallCommand::install_package(const Package &package,
                                        const fs::path &output_dir) {

        this->check_if_installed(package);

        CPM_INFO(
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
        CPM_INFO(" \x1b[34mv\x1b[37m Downloading repository\n");
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
                    std::cout << std::setw(60) << "\r";

                } else if (downloadNow >= 1000) {
                    downloaded /= 1000;
                    total /= 1000;
                    unit = "KB";
                    std::cout << std::setw(60) << "\r";
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
                std::cout << "\r \x1b[32m+\x1b[37m Extracting archive entries " << currentEntry << "/" << totalEntries;
                return true;
            }
        );
        std::cout << " done.\n";
        CPM_LOG_INFO("extracted package {}", package.get_name());

        for (const auto &dep : *package.get_dependencies()) {
            CPM_LOG_INFO(
                "installing dependency of {}: {}",
                package.get_name(), dep.get_name()
            );
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
        CPM_LOG_INFO(
            "HEAD https://api.github.com/repos/.../{}/zipball/{}",
            package.get_name(), package.get_version().to_string()
        );
        cpr::Response res = cpr::Head(
            cpr::Url{(paths::api_url / paths::owner_name / package.get_name() /
                     "zipball" / package.get_version().to_string()).string()}
        );

        if (res.status_code != cpr::status::HTTP_OK) {
            throw std::invalid_argument(package.get_name() + ": package not found!");
        }

        CPM_LOG_INFO(
            "GET https://api.github.com/repos/.../{}/zipball/{}",
            package.get_name(), package.get_version().to_string()
        );
        res = cpr::Get(
            cpr::Url{(paths::api_url / paths::owner_name / package.get_name() /
                     "zipball" / package.get_version().to_string()).string()},
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

	void InstallCommand::final_message(
        const std::unordered_set<cpm::Package, cpm::Package::Hash> &packages
    ) {
        CPM_INFO("\x1b[33m"
            "\n"
            "**Hint**\n"
            "To use the package/s add the following commands to your CMakeLists.txt:\n"
        );
        for (const auto &package : packages) {
            CPM_INFO("    add_subdirectory(lib/{})\n", package.get_name());
        }

        CPM_INFO(
            "\n"
            "    target_include_directories(<target>\n"
            "       PRIVATE\n"
        );
        for (const auto &package : packages) {
            CPM_INFO(
                "            ${{CMAKE_CURRENT_SOURCE_DIR}}/lib/{}/include\n",
                package.get_name()
            );
        }
        CPM_INFO(
            "    )\n"
            "\n"
        );

        CPM_INFO(
            "    target_link_libraries(<target>\n"
            "       PRIVATE\n"
        );
        for (const auto &package : packages) {
            CPM_INFO(
                "            {}\n", package.get_name()
            );
        }
        CPM_INFO(
            "    )\n"
            "\x1b[37m\n"
        );
    }
}
