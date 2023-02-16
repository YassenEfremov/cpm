#include "package.hpp"

#include "paths.hpp"
#include "util.hpp"

#include "cpr/cpr.h"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <string>

#include <cstddef>

namespace fs = std::filesystem;
using json = nlohmann::ordered_json;


namespace cpm {

	std::size_t Package::Hash::operator()(const Package &package) const noexcept {
		std::size_t h1 = std::hash<std::string>{}(package.get_name());
		return h1 << 1;
	}

	Package::Package(const std::string &name)
		: name(name), dependencies(std::make_shared<std::unordered_set<Package, Package::Hash>>()) {}

	Package::Package(const std::string &name, const SemVer &version)
		: name(name), version(version), dependencies(std::make_shared<std::unordered_set<Package, Package::Hash>>()) {}

	std::string Package::get_name() const {
		return this->name;
	}

	SemVer Package::get_version() const {
		return this->version;
	}

	std::shared_ptr<std::unordered_set<Package, Package::Hash>> Package::get_dependencies() const {
		return this->dependencies;
	}

	void Package::init() {
		if (!this->version.is_specified()) {
			cpr::Response res = cpr::Get(
				cpr::Url{(paths::api_url / paths::owner_name / this->name / "tags").string()}
			);
			if (res.status_code != cpr::status::HTTP_OK) {
				throw std::invalid_argument(this->name + ": package not found!");
			}
			json res_json = json::parse(res.text);
			std::sort(res_json.begin(), res_json.end(),
				[](const json p1, const json p2) {
					SemVer v1(p1["name"].get<std::string>());
					SemVer v2(p2["name"].get<std::string>());

					return v1 > v2;
				}
			);
			this->version = SemVer(res_json[0]["name"].get<std::string>());

		} else {
			cpr::Response res = cpr::Head(
				cpr::Url{(paths::api_url / paths::owner_name / this->name / "zipball" / this->version.to_string()).string()}
			);
			if (res.status_code != cpr::status::HTTP_OK) {
				throw std::invalid_argument(this->name + ": version " + this->version.to_string() + " not found!");
			}
		}

        cpr::Response res = cpr::Get(
            cpr::Url{(paths::api_url / paths::owner_name / this->name / "contents" / paths::package_config).string()}
        );
        json res_json = json::parse(res.text);
        json config_json = json::parse(util::base64_decode(res_json["content"].get<std::string>()));
        if (config_json.contains("dependencies")) {
			for (const auto &[name, version_str] : config_json["dependencies"].get<std::map<std::string, std::string>>()) {
				Package package_dep(name, SemVer(version_str));
				package_dep.init();
				this->dependencies->insert(package_dep);
			}
        }
	}

	bool operator==(const Package &lhs, const Package &rhs) {
		return lhs.get_name() == rhs.get_name();
	}
}
