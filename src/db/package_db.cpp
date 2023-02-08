#include "db/package_db.hpp"

#include "package.hpp"
#include "repository.hpp"

#include "sqlite3.h"

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_set>

#include <cstdlib>

namespace fs = std::filesystem;


namespace cpm {
	
	PackageDB::PackageDB(const fs::path &filename) : Repository(filename) {
		fs::create_directories(filename.parent_path());
		int err = sqlite3_open(filename.string().c_str(), &this->package_db);
		if (err) {
			sqlite3_close(this->package_db);
			throw std::runtime_error("Can't open package DB!");
		}

		char *err_msg;
		err = sqlite3_exec(this->package_db,
			"CREATE TABLE IF NOT EXISTS installed_packages("
				"name VARCHAR(100) NOT NULL PRIMARY KEY"
			");"
		, nullptr, 0, &err_msg);
		if (err != SQLITE_OK) {
			sqlite3_free(err_msg);
			throw std::runtime_error("Can't create packages table!");
		}

		sqlite3_free(err_msg);
	}

	PackageDB::~PackageDB() {
		sqlite3_close(this->package_db);
	}

	int PackageDB::add(const cpm::Package &package) {
		sqlite3_stmt *stmt;
		sqlite3_prepare(this->package_db,
			"INSERT INTO installed_packages VALUES (?);"
		, -1, &stmt, nullptr);
		
		sqlite3_bind_text(stmt, 1, package.name.c_str(), package.name.length(), SQLITE_STATIC);
		sqlite3_step(stmt);
		int rows_modified = sqlite3_total_changes(this->package_db);

		sqlite3_finalize(stmt);
		return rows_modified;
	}

	int PackageDB::remove(const cpm::Package &package) {
		sqlite3_stmt *stmt;
		sqlite3_prepare(this->package_db,
			"DELETE FROM installed_packages WHERE name = ?;"
		, -1, &stmt, nullptr);
		
		sqlite3_bind_text(stmt, 1, package.name.c_str(), package.name.length(), SQLITE_STATIC);
		sqlite3_step(stmt);
		int rows_modified = sqlite3_total_changes(this->package_db);
		
		sqlite3_finalize(stmt);
		return rows_modified;
	}

	std::unordered_set<Package, Package::PackageHash> PackageDB::list() {
		std::unordered_set<Package, Package::PackageHash> packages;
		char *err_msg;
        int err = sqlite3_exec(this->package_db, "SELECT name FROM installed_packages;",
            [](void *packages, int cols, char **col_vals, char **col_names) {
				Package package{col_vals[0]};
				static_cast<std::unordered_set<Package, Package::PackageHash> *>(packages)->insert(package);
                return EXIT_SUCCESS;
            }, &packages, &err_msg);
		if (err != SQLITE_OK) {
			sqlite3_free(err_msg);
			throw std::runtime_error("Can't get installed packages!");
		}

		sqlite3_free(err_msg);
		return packages;
	}
}
