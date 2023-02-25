#include "package.hpp"

#include "paths.hpp"
#include "util.hpp"

#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
#include "spdlog/fmt/ostr.h"

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

	std::string Package::get_name() const { return this->name; }
	SemVer Package::get_version() const { return this->version; }
	std::string Package::get_location() const { return this->location; }

	std::shared_ptr<std::unordered_set<Package, Package::Hash>> Package::get_dependencies() const {
		return this->dependencies;
	}

	std::string Package::string() const {
		return fmt::format("{}@{}", this->name, this->version.string());
	}

	void Package::find_location() {
		if (!fs::exists(paths::global_dir / paths::package_locations)) {
			throw std::runtime_error(fmt::format("package locations file not found!"));
		}
        std::ifstream package_repos_file(paths::global_dir / paths::package_locations);
        auto package_config = json::parse(package_repos_file);
		for (const auto &package_location : package_config["package_locations"].get<std::vector<std::string>>()) {
			cpr::Response response = cpr::Head(
				cpr::Url{(paths::api_url / package_location / this->name).string()}
			);
			if (response.status_code == cpr::status::HTTP_OK) {
			    this->location = package_location;
				return;
			}
		}

        throw std::invalid_argument(fmt::format(
            "{}: package not found!", this->name
        ));
	}

	void Package::init() {
		if (!this->version.is_specified()) {
			cpr::Response res = cpr::Get(
				cpr::Url{(paths::api_url / this->location / this->name / "tags").string()}
			);
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
				cpr::Url{(paths::api_url / this->location / this->name / "zipball" / this->version.string()).string()}
			);
			if (res.status_code != cpr::status::HTTP_OK) {
				throw std::invalid_argument(fmt::format(
					"{}: version {} not found!", this->name, this->version.string()
				));
			}
		}

        cpr::Response res = cpr::Get(
            cpr::Url{(paths::api_url / this->location / this->name / "contents" / paths::package_config).string()}
        );
        json res_json = json::parse(res.text);
        json config_json = json::parse(util::base64_decode(res_json["content"].get<std::string>()));
        if (config_json.contains("dependencies")) {
			for (const auto &[name, version_str] : config_json["dependencies"].get<std::map<std::string, std::string>>()) {
				Package package_dep(name, SemVer(version_str));
				package_dep.location = this->location;
				package_dep.init();
				this->dependencies->insert(package_dep);
			}
        }
	}

	bool operator==(const Package &lhs, const Package &rhs) {
		return lhs.get_name() == rhs.get_name();
	}
}
