#include "script/cpm_pack.hpp"

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
	
	CPMPack::CPMPack(const fs::path &filename) : Repository(filename) {
		if (fs::exists(this->filename) && !fs::is_empty(this->filename)) {
			std::ifstream cpm_pack_in(this->filename);
			this->config_json = nlohmann::ordered_json::parse(cpm_pack_in);
		}
	}

	int CPMPack::add(const cpm::Package &package) {
		if (this->contains(package)) {
			return 0;
		}

		this->config_json["dependencies"][package.get_name()] = package.get_version().to_string();
		this->save();
		return 1;
	}

	int CPMPack::remove(const cpm::Package &package) {
		if (!this->contains(package)) {
			return 0;
		}

		this->config_json["dependencies"].erase(package.get_name());
		this->save();
		return 1;
	}

	std::unordered_set<Package, Package::Hash> CPMPack::list() {
		std::map<std::string, std::string> package_map;
		try {
			package_map = this->config_json["dependencies"].get<std::map<std::string, std::string>>();
			
		} catch(const std::exception &e) {
			return std::unordered_set<Package, Package::Hash>();
		}

		std::unordered_set<Package, Package::Hash> packages;
		for (const auto &[name, version_str] : package_map) {
			packages.insert(Package(name, SemVer(version_str)));
		}
		return packages;
	}

	bool CPMPack::contains(const Package &package) {
		if (this->list().find(package) != this->list().end()) {
			return true;
		} else {
			return false;
		}
	}

	Package CPMPack::create() {
		std::string default_package_name = fs::current_path().stem().string();
		SemVer default_package_version("0.1.0");
        this->config_json["name"] = default_package_name;
        this->config_json["version"] = default_package_version.to_string();
		this->config_json["url"] = "";
        this->config_json["description"] = "";
        this->config_json["author"] = "";
        this->config_json["license"] = "";
		this->save();
		return Package(default_package_name, default_package_version);
	}

	void CPMPack::save() {
		std::ofstream cpm_pack_out(this->filename);
		cpm_pack_out << std::setw(4) << this->config_json;
	}
}