#include "install.hpp"

#include "spdlog/spdlog.h"
#include "cpr/cpr.h"
#include "sqlite3.h"

extern "C" {
    #include "zip.h"
}

#include <string>
#include <stdexcept>
#include <filesystem>

#include <cstdlib>

namespace fs = std::filesystem;


namespace cpm {

    void install(const std::string &package) {

        fs::path repository = fs::path(package).stem();

        spdlog::get("stdout_logger")->info(
            "Installing package into {} ...",
            (fs::current_path() / "lib" / repository / "").string()
        );

        // Check if the target directory already exists
        if (fs::exists(fs::current_path() / "lib" / repository / "")) {             // TODO: check the local package DB instead
            spdlog::get("stdout_logger")->info("\n");
            throw std::invalid_argument("Package directory already exists!");
        }

        // Download the repository as a zip archive (same as the green button on GitHub)
        cpr::Response response = cpr::Get(cpr::Url{package + "/archive/refs/heads/master.zip"});    // TODO: this should use the github API

        spdlog::get("stdout_logger")->info(" done.\n");

        // Extract the entries from the archive into <cwd>/lib/<repo-name>-master
        struct zip_t *zip = zip_stream_open(response.text.c_str(), response.text.size(), 0, 'r');
        zip_stream_extract(response.text.c_str(), response.text.size(),
            (fs::current_path() / "lib" / "").string().c_str(),
            [](const char *filename, void *arg) {
                struct zip_t *zip = static_cast<struct zip_t *>(arg);
                static int i = 1;
                spdlog::get("stdout_logger")->info(
                    "\rExtracting archive entries {}/{}", i++, zip_entries_total(zip)
                );
                return EXIT_SUCCESS;
            }, zip
        );
        spdlog::get("stdout_logger")->info(" done.\n");

        // Rename the target directory to remove the "-master" at the end
        fs::rename(fs::current_path() / "lib" / repository += "-master/",           // TODO: the in-memory archive name should be changed instead
                   fs::current_path() / "lib" / repository / "");

        // Add the installed package to the local DB
        sqlite3 *db;
        char *err;
        sqlite3_open("lib/packages.db3", &db);

        sqlite3_exec(db,
            "CREATE TABLE IF NOT EXISTS installed_packages("
                "name VARCHAR(100) NOT NULL PRIMARY KEY"
            ");"
        , nullptr, 0, &err);

        sqlite3_stmt *stmt;
        sqlite3_prepare(db,
            "INSERT INTO installed_packages VALUES (?);"
        , -1, &stmt, nullptr);
        
        sqlite3_bind_text(stmt, 1, repository.string().c_str(), repository.string().length(), SQLITE_STATIC);
        sqlite3_step(stmt);
        int rows_modified = sqlite3_total_changes(db);

        sqlite3_free(err);
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        spdlog::get("stdout_logger")->info(
            "DB: modified {} rows\n", rows_modified
        );
    }
}
