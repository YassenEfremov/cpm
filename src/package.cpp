#include "package.hpp"

#include "logger/logger.hpp"
#include "paths.hpp"
#include "util.hpp"

#include "cpr/cpr.h"
#include "nlohmann/json.hpp"
#include "spdlog/fmt/ostr.h"

#include <filesystem>
#include <fstream>
#include <functional>
#include <stdexcept>
#include <string>

#include <cstddef>

namespace fs = std::filesystem;
using json = nlohmann::ordered_json;


namespace cpm {

std::size_t Package::Hash::operator()(const Package &package) const noexcept {
	std::size_t h1 = std::hash<std::string>{}(package.get_name());
	return h1 << 1;
}

Package::Package(const std::string &name) : name(name) {}

Package::Package(const std::string &name, const SemVer &version)
	: name(name), version(version) {}

std::string Package::get_name() const { return this->name; }
SemVer Package::get_version() const { return this->version; }
std::string Package::get_location() const { return this->location; }

std::string Package::string() const {
	return fmt::format("{}@{}", this->name, this->version.string());
}

void Package::init() {
	CPM_LOG_INFO("Looking for an available GitHub profile ...");
	if (!fs::exists(paths::global_dir / paths::package_locations)) {
		throw std::runtime_error(fmt::format(
			"package locations file not found!\n"
			"Make sure you've installed cpm correctly!"
		));
	}
	std::ifstream package_locations_file(paths::global_dir / paths::package_locations);
	auto package_locations_json = json::parse(package_locations_file);
	auto package_locations = package_locations_json["package_locations"].get<std::vector<std::string>>();
	this->location = "";
	for (const auto &package_location : package_locations) {
		CPM_LOG_INFO("Trying {}", package_location);
		cpr::Response response = cpr::Head(
			cpr::Url{(paths::api_url / package_location / this->name).string()}
		);
		if (response.status_code == cpr::status::HTTP_OK) {
			CPM_LOG_INFO("Success (status {}) using {}",
						 response.status_code, package_location);
			this->location = package_location;
			break;

		} else {
			CPM_LOG_INFO("Failed (status {}) {}",
						 response.status_code, package_location);
		}
	}
	if (this->location.empty()) {
		throw std::invalid_argument(fmt::format("{}: package not found!", this->name));
	}

	if (!this->version.is_specified()) {
		CPM_LOG_INFO("Getting latest version for package {} ...", this->name);
		cpr::Response response = cpr::Get(
			cpr::Url{(paths::api_url / location / this->name /
					  paths::gh_content / paths::package_config).string()}
		);
		json package_config_json;
		if (response.status_code == cpr::status::HTTP_OK) {
			json response_json = json::parse(response.text);
			std::string package_config_str = util::base64_decode(response_json["content"].get<std::string>());
			package_config_json = json::parse(package_config_str);
			this->version = SemVer(package_config_json["version"].get<std::string>());

		} else {
			throw std::runtime_error(fmt::format(
				"{}: package doesn't contain a {} file!",
				this->name, paths::package_config.string()
			));
		}

	} else {
		CPM_LOG_INFO("Checking version {} for package {}", this->version.string(), this->name);
		cpr::Response response = cpr::Head(
			cpr::Url{(paths::api_url / this->location / this->name /
					  paths::gh_zip / this->version.string()).string()}
		);
		if (response.status_code != cpr::status::HTTP_OK) {
			throw std::invalid_argument(fmt::format(
				"{}: version {} not found!", this->name, this->version.string()
			));
		}
	}
}

bool operator==(const Package &lhs, const Package &rhs) {
	return lhs.get_name() == rhs.get_name();
}

} // namespace cpm
