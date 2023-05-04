#include "commands/install.hpp"

#include "commands/command.hpp"
#include "cli/colors.hpp"
#include "cli/progress_bar.hpp"
#include "db/package_db.hpp"
#include "dep-man/lockfile.hpp"
#include "logger/logger.hpp"
#include "package.hpp"
#include "paths.hpp"
#include "script/package_config.hpp"
#include "semver.hpp"
#include "util.hpp"

#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
#include "spdlog/fmt/ostr.h"

extern "C" {
    #include "zip.h"
}

#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <cstdint>
#include <cstdlib>

namespace fs = std::filesystem;
using json = nlohmann::ordered_json;


namespace cpm {

InstallCommand::InstallCommand(const std::string &name) : Command(name) {}

void InstallCommand::run() {

    CPM_LOG_INFO("===== Starting install command =====");

    auto packages_str = this->get<std::vector<std::string>>("packages");
    CPM_LOG_INFO("args: {}", [&]() {
        std::string args = "[";
        for (const auto &arg : packages_str) {
            args += arg + ", ";
        }
        return args + "]";
    }());
    std::unordered_set<Package, Package::Hash> packages;
    for (const auto &package_str : packages_str) {
        auto tokens = util::split_string(package_str, "@");
        if (tokens.size() == 2) {
            CPM_LOG_INFO("Checking package {}@{} ...", tokens[0], tokens[1]);
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
                new_package.get_version().string(), new_package.get_name()
            );
            CPM_INFO(" found.\n");
            packages.insert(new_package);

        } else if (tokens.size() == 1) {
            CPM_LOG_INFO("Resolving version for package {} ...", tokens[0]);
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
                new_package.get_version().string(), new_package.get_name()
            );
            CPM_INFO(" found latest: " YELLOW_FG("{}") "\n", new_package.get_version().string());
            packages.insert(new_package);

        } else {
            throw std::invalid_argument(fmt::format(
                "{}: invalid package format!", package_str
            ));
        }
    }

    int records_modified = 0;
    for (const auto &package : packages) {
        try {
            records_modified += this->install_package(
                package, this->context.cwd / paths::packages_dir / ""
            );
        
        } catch(const std::exception &e) {
            CPM_LOG_ERR(e.what());
            CPM_ERR(e.what());
        }
    }

    CPM_INFO(
        "{}: modified {} record/s\n",
        this->context.repo->get_filename().filename().string(), records_modified
    );

    CPM_LOG_INFO("===== Finished install command. =====");

    this->final_message(packages);
}

int InstallCommand::install_package(const Package &package,
                                    const fs::path &output_dir) {

    CPM_LOG_INFO("Checking if package {} is already installed ...", package.get_name());
    if (this->check_if_installed(package)) {
        throw std::invalid_argument(fmt::format(
            "{}: package already installed!", package.get_name()
        ));
    }

    CPM_INFO(
        "Installing package into {} ...\n",
        (output_dir / package.get_name() / "").string()
    );

    CPM_LOG_INFO(
        "Installing package into {} ...",
        (output_dir / package.get_name() / "").string()
    );
    this->install_all(package, package.get_location(), output_dir);

    CPM_LOG_INFO("Adding package to {} ...", paths::package_config.string());
    return this->register_package(package);
}

bool InstallCommand::check_if_installed(const Package &package) {
    bool specified = this->context.repo->contains(package);
    bool downloaded = fs::exists(this->context.cwd / paths::packages_dir / package.get_name() / "");

    return specified && downloaded;
}

void InstallCommand::install_all(const Package &package, const std::string &location,
                                 const fs::path &output_dir) {

    CPM_LOG_INFO("obtaining lockfile for package {} ...", package.get_name());
    CPM_INFO( BLUE_FG(" v ") "Obtaining lockfile ...");
    std::unordered_set<Package, Package::Hash> packages_to_install{package};
    cpr::Response response = cpr::Get(
        cpr::Url{(paths::api_url / location / package.get_name() /
                  paths::gh_content / paths::lockfile).string()}
    );
    json package_lockfile_json;
    if (response.status_code == cpr::status::HTTP_OK) {
        json response_json = json::parse(response.text);
        std::string lockfile_str = util::base64_decode(response_json["content"].get<std::string>());
        package_lockfile_json = json::parse(lockfile_str);
        for (const auto &[name, content] : package_lockfile_json["dependencies"].items()) {
            packages_to_install.insert(Package(name, SemVer(content["version"].get<std::string>())));
        }

    }
    std::cout << " done.\n";

    std::vector<std::thread> threads;
    std::mutex printing_mutex;
    std::unordered_map<Package, ProgressBar, Package::Hash> all_progress;
    const auto refresh_all_progress = [&]() {
        std::lock_guard<std::mutex> lock(printing_mutex);
        std::cout << fmt::format("\x1b[{}F", all_progress.size() * 2);
        for (const auto &[package, progress_bar] : all_progress) {
            progress_bar.print_active_stage();
        }
    };
    for (int i = 0; i < packages_to_install.size() * 2; i++) std::cout << "\n";
    for (const auto &current_package : packages_to_install) {
        threads.emplace_back(std::thread([=, &all_progress]() {
            CPM_LOG_INFO("Checking dependency {} ...", current_package.get_name());
            if (this->context.lockfile->contains(current_package) &&
                fs::exists(this->context.cwd / paths::packages_dir / current_package.get_name() / "")) {
                CPM_LOG_INFO("Dependency {} is already satisfied! Skipping ...", current_package.get_name());
                all_progress.insert({
                    current_package,
                    ProgressBar(
                        fmt::format(" * {:<25}{:>15}",
                                    current_package.string(), "(Skipping)"),
                        10, '#'
                    )
                });
                all_progress.at(current_package).set_active_bar("   [   done   ]");
                all_progress.at(current_package).update_suffix(" satisfied");
                refresh_all_progress();
                return;
            }
            CPM_LOG_INFO("Dependency {} not found, installing ...", current_package.get_name());

            CPM_LOG_INFO("downloading package {} ...", current_package.string());
            all_progress.insert({
                current_package,
                ProgressBar(
                    fmt::format(BLUE_FG(" v ") "{:<25}{:>15}",
                                current_package.string(), "(Downloading)"),
                    10, '#'
                )
            });
            cpr::Response response = this->download_package(current_package, location,
                [=, &all_progress](cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow,
                                   cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow, std::intptr_t userdata) {
                    double downloaded = static_cast<double>(downloadNow);
                    double total = static_cast<double>(downloadTotal);
                    std::string unit = downloadNow >= 1e+6 ? "MB"
                                     : downloadNow >= 1e+3 ? "KB"
                                     : "B";
                    downloaded /= downloadNow >= 1e+6 ? 1e+6
                                : downloadNow >= 1e+3 ? 1e+3
                                : 1;
                    total /= downloadNow >= 1e+6 ? 1e+6
                           : downloadNow >= 1e+3 ? 1e+3
                           : 1;
                    if (total < downloaded) {
                        total = downloaded;
                    }
                    all_progress.at(current_package)++;
                    all_progress.at(current_package).update_suffix(fmt::format(
                        " {0:.2f} {1}     ", downloaded, unit
                    ));
                    refresh_all_progress();
                    return true;
                }
            );
            CPM_LOG_INFO("download complete, total: {} KB", response.text.size() / 1e+3);
            all_progress.at(current_package).set_active_bar("   [   done   ]");
            refresh_all_progress();

            CPM_LOG_INFO("extracting package {} ...", current_package.string());
            struct zip_t *zip = zip_stream_open(response.text.c_str(), response.text.size(), 0, 'r');
            std::size_t total_entries = zip_entries_total(zip);
            zip_stream_close(zip);
            all_progress.at(current_package).add_stage(
                fmt::format(GREEN_FG(" + ") "{:<25}{:>15}",
                            current_package.string(), "(Extracting)"),
                10, '#', total_entries
            );
            this->extract_package(response.text,
                this->context.cwd / paths::packages_dir / current_package.get_name(),
                [=, &all_progress](int currentEntry, int totalEntries) {
                    all_progress.at(current_package)++;
                    all_progress.at(current_package).update_suffix(fmt::format(
                        " {}/{}     ", currentEntry, totalEntries
                    ));
                    refresh_all_progress();
                    return true;
                }
            );
            CPM_LOG_INFO("extract complete");
            all_progress.at(current_package).set_active_bar("   [   done   ]");
            refresh_all_progress();
        }));
    }

    for (auto &thread : threads) {
        thread.join();
    }

    this->context.lockfile->add(package);

    if (packages_to_install.size() < 2) {
        return;
    }
    for (const auto &[name, content] : package_lockfile_json["dependencies"].items()) {
        this->context.lockfile->add_dep(package, Package(name, content["version"].get<std::string>()));
        CPM_LOG_INFO("symlinking direct dependency: {} ...", name);
        fs::create_directories(output_dir / package.get_name() / paths::packages_dir);
        fs::create_directory_symlink(
            output_dir / name,
            output_dir / package.get_name() / paths::packages_dir / name
        );
        CPM_LOG_INFO("symlink created");

        this->context.lockfile->add(Package(name, content["version"].get<std::string>()));
        if (content.contains("dependencies")) {
            for (const auto &[dep_name, dep_version] : content["dependencies"].items()) {
                this->context.lockfile->add_dep(Package(name, content["version"].get<std::string>()),
                                                Package(dep_name, dep_version.get<std::string>()));
                CPM_LOG_INFO("symlinking transitive dependency: {} ...", dep_name);
                fs::create_directories(output_dir / name / paths::packages_dir);
                fs::create_directory_symlink(
                    output_dir / dep_name,
                    output_dir / name / paths::packages_dir / dep_name
                );
                CPM_LOG_INFO("symlink created");
            }
        }
    }
}

cpr::Response InstallCommand::download_package(
    const Package &package, const std::string location,
    std::function<bool(
        cpr::cpr_off_t downloadTotal, cpr::cpr_off_t downloadNow,
        cpr::cpr_off_t uploadTotal, cpr::cpr_off_t uploadNow,
        std::intptr_t userdata
    )> download_progress
) {
    CPM_LOG_INFO(
        "GET {}/{}/{}/{}/{}",
        paths::api_url.string(), location, paths::gh_zip,
        package.get_name(), package.get_version().string()
    );
    cpr::Response response = cpr::Get(
        cpr::Url{(paths::api_url / location / package.get_name() /
                  paths::gh_zip / package.get_version().string()).string()},
        cpr::ProgressCallback(download_progress)
    );
    CPM_LOG_INFO("Response status: {}", response.status_code);
    if (response.status_code != cpr::status::HTTP_OK) {
        throw std::runtime_error(fmt::format(
            "{}: version {} not found!",
            package.get_name(), package.get_version().string()
        ));
    }

    return response;
}

void InstallCommand::extract_package(
    const std::string &stream, const fs::path &output_dir,
    std::function<bool(int currentEntry, int totalEntries)> on_extract
) {
    struct zip_t *zip = zip_stream_open(stream.c_str(), stream.size(), 0, 'r');

    std::size_t total_entries = zip_entries_total(zip);
    CPM_LOG_INFO("Total entries to extract: {}", total_entries);
    for (int i = 0; i < total_entries; i++) {
        zip_entry_openbyindex(zip, i);

        std::string entry_name = zip_entry_name(zip);
        CPM_LOG_INFO("extracting {} ...", entry_name);
        std::size_t first_slash = entry_name.find_first_of('/');
        if (zip_entry_isdir(zip)) {
            fs::create_directories(output_dir / entry_name.substr(first_slash + 1));
        }
        zip_entry_fread(zip, (output_dir / entry_name.substr(first_slash + 1)).string().c_str());
        on_extract(i + 1, total_entries);

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
    fs::path prefix = "${CMAKE_CURRENT_SOURCE_DIR}";
    if (this->context.cwd != fs::current_path()) {
        prefix = paths::global_dir;
    }

    CPM_INFO("\x1b[33m"
        "\n"
        "**Hint**\n"
        "To use the package/s add the following commands to your CMakeLists.txt:\n"
    );
    for (const auto &package : packages) {
        CPM_INFO(
            "    add_subdirectory({}/lib/{})\n",
            prefix.string(), package.get_name()
        );
    }

    CPM_INFO(
        "\n"
        "    target_include_directories(<target>\n"
        "       PRIVATE\n"
    );
    for (const auto &package : packages) {
        CPM_INFO(
            "            {}/lib/{}/include\n",
            prefix.string(), package.get_name()
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
        "\n"
        "\x1b[37m"
    );
}

} // namespace cpm
