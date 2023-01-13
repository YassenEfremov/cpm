#include "list.hpp"

#include "command.hpp"
#include "../package.hpp"

#include "spdlog/spdlog.h"
#include "sqlite3.h"

#include <string>
#include <vector>


namespace cpm {

    ListCommand::ListCommand(const std::string &name) : Command(name) {}

    void ListCommand::run(const std::vector<Package> &args) {

        // List all of the installed packages from the local DB
        sqlite3 *db;
        char *err_msg;
        sqlite3_open("lib/packages.db3", &db);

        sqlite3_exec(db, "SELECT name FROM installed_packages;",
            [](void *data, int cols, char **col_vals, char **col_names) {
                spdlog::get("stdout_logger")->info(std::string(col_vals[0]) + "\n");
                return 0;
            }, 0, &err_msg
        );

        sqlite3_free(err_msg);
        sqlite3_close(db);
	}
}
