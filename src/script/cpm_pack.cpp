#include "cpm_pack.hpp"

#include "../package.hpp"
#include "../repository.hpp"
#include "../util.hpp"

#include "nlohmann/json.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>

namespace fs = std::filesystem;


namespace cpm {
	
	CPMPack::CPMPack(const fs::path &filename) : Repository(filename) {	
		if (fs::exists(this->filename) && !fs::is_empty(this->filename)) {
			std::ifstream cpm_pack_in(this->filename);
			this->cpm_pack_json = nlohmann::ordered_json::parse(cpm_pack_in);
		}
	}

	int CPMPack::add(const cpm::Package &package) {
		this->cpm_pack_json["dependencies"] += package.get_name();
		this->save();
		return 1;
	}

	int CPMPack::remove(const cpm::Package &package) {
		for (auto &[i, p] : this->cpm_pack_json["dependencies"].items()) {
			if (p.get<std::string>() == package.get_name()) {
				this->cpm_pack_json["dependencies"].erase(std::stoi(i));
				break;
			}
		}
		this->save();
		return 1;
	}

	std::vector<Package> CPMPack::list() {
		auto packages = this->cpm_pack_json["dependencies"].get<std::vector<std::string>>();
		return std::vector<Package>(packages.begin(), packages.end());
	}

	Package CPMPack::create() {
		std::string default_package_name = fs::current_path().stem().string();
        this->cpm_pack_json["name"] = default_package_name;
        this->cpm_pack_json["version"] = "0.1.0";
		this->cpm_pack_json["url"] = "";
        this->cpm_pack_json["description"] = "";
        this->cpm_pack_json["author"] = "";
        this->cpm_pack_json["license"] = "";
		this->save();
		return Package(default_package_name);
	}

	void CPMPack::save() {
		std::ofstream cpm_pack_out(this->filename);
		cpm_pack_out << std::setw(4) << this->cpm_pack_json;
	}
}
