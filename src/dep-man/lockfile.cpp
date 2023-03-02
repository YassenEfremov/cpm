#include "dep-man/lockfile.hpp"

#include "package.hpp"
#include "repository.hpp"
#include "semver.hpp"

#include "nlohmann/json.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_set>

namespace fs = std::filesystem;
using json = nlohmann::ordered_json;


namespace cpm {
	
	Lockfile::Lockfile(const fs::path &filename) : Repository(filename) {
		if (fs::exists(this->filename) && !fs::is_empty(this->filename)) {
			std::ifstream lockfile_in(this->filename);
			this->lockfile_json = json::parse(lockfile_in);
		}
	}

	int Lockfile::add(const Package &package) {
		if (this->contains(package)) {
			return 0;
		}

		this->lockfile_json
			["dependencies"]
				[package.get_name()]
					["version"] = package.get_version().string();
		this->save();
		return 1;
	}

	int Lockfile::add_dep(const Package &existing_package, const Package &dep) {
		if (!this->contains(existing_package) ||
			this->lockfile_json
				["dependencies"]
					[existing_package.get_name()]
						["dependencies"].contains(dep.get_name())) {
			return 0;
		}

		this->lockfile_json
			["dependencies"]
				[existing_package.get_name()]
					["dependencies"]
						[dep.get_name()] = dep.get_version().string();
		this->save();
		return 1;
	}

	int Lockfile::remove(const Package &package) {
		if (!this->contains(package)) {
			return 0;
		}

		this->lockfile_json
			["dependencies"].erase(package.get_name());
		this->save();
		return 1;
	}

	int Lockfile::remove_dep(const Package &existing_package, const Package &dep) {
		if (!this->contains(existing_package) ||
			!this->lockfile_json
				["dependencies"]
					[existing_package.get_name()]
						["dependencies"].contains(dep.get_name())) {
			return 0;
		}

		this->lockfile_json
			["dependencies"]
				[existing_package.get_name()]
					["dependencies"].erase(dep.get_name());
		this->save();
		return 1;
	}

	std::unordered_set<Package, Package::Hash> Lockfile::list() const {
		if (!this->lockfile_json.contains("dependencies")) {
			return std::unordered_set<Package, Package::Hash>();
		}

		std::unordered_set<Package, Package::Hash> packages;
		for (const auto &[name, content] : this->lockfile_json["dependencies"].items()) {
			packages.insert(Package(name, SemVer(content["version"].get<std::string>())));
		}
		return packages;
	}

	bool Lockfile::contains(const Package &package) const {
		if (this->list().find(package) != this->list().end()) {
			return true;
		} else {
			return false;
		}
	}

	bool Lockfile::contains_dep(const Package &package) {
		for (const auto &dep : this->lockfile_json["dependencies"]) {
			for (const auto &[name, version] : dep["dependencies"].items()) {
				if(name == package.get_name()) {
					return true;
				}
			}
		}
		return false;
	}

	void Lockfile::save() {
		std::ofstream lockfile_out(this->filename);
		lockfile_out << std::setw(4) << this->lockfile_json;
	}
}