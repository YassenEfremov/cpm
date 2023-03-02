#include "script/package_config.hpp"

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


namespace cpm {
	
	PackageConfig::PackageConfig(const fs::path &filename) : Repository(filename) {
		if (fs::exists(this->filename) && !fs::is_empty(this->filename)) {
			std::ifstream package_config_in(this->filename);
			this->config_json = nlohmann::ordered_json::parse(package_config_in);
		}
	}

	int PackageConfig::add(const Package &package) {
		if (this->contains(package)) {
			return 0;
		}

		this->config_json["dependencies"][package.get_name()] = package.get_version().string();
		this->save();
		return 1;
	}

	int PackageConfig::remove(const Package &package) {
		if (!this->contains(package)) {
			return 0;
		}

		this->config_json["dependencies"].erase(package.get_name());
		this->save();
		return 1;
	}

	std::unordered_set<Package, Package::Hash> PackageConfig::list() const {
		if (!this->config_json.contains("dependencies")) {
			return std::unordered_set<Package, Package::Hash>();
		}

		std::unordered_set<Package, Package::Hash> packages;
		for (const auto &[name, content] : this->config_json["dependencies"].items()) {
			packages.insert(Package(name, SemVer(content.get<std::string>())));
		}
		return packages;
	}

	bool PackageConfig::contains(const Package &package) const {
		if (this->list().find(package) != this->list().end()) {
			return true;
		} else {
			return false;
		}
	}

	Package PackageConfig::create_default() {
		std::string default_package_name = fs::current_path().stem().string();
		SemVer default_package_version("0.1.0");
        this->config_json["name"] = default_package_name;
        this->config_json["version"] = default_package_version.string();
		this->config_json["url"] = "";
        this->config_json["description"] = "";
        this->config_json["author"] = "";
        this->config_json["license"] = "";
		this->save();
		return Package(default_package_name, default_package_version);
	}

	void PackageConfig::save() {
		std::ofstream package_config_out(this->filename);
		package_config_out << std::setw(4) << this->config_json;
	}
}