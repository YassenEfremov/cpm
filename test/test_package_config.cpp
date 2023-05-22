#include "script/package_config.hpp"
#include "semver.hpp"
#include "package.hpp"

#include "catch2/catch_test_macros.hpp"

#include <filesystem>
#include <fstream>
#include <unordered_set>	
#include <vector>

namespace fs = std::filesystem;
using json = nlohmann::ordered_json;


SCENARIO("package config basic operations", "[package_config]") {

	GIVEN("an empty package config file") {

		fs::remove("test_cpm_pack.json");
		cpm::PackageConfig package_config("test_cpm_pack.json");

		WHEN("a package is added") {
			cpm::Package package("test-package", cpm::SemVer("1.0.0"));
			package_config.add(package);
			json package_config_json = json::parse(std::ifstream("test_cpm_pack.json"));

			THEN("the package config contains it") {
				CHECK(package_config_json["dependencies"].contains(package.get_name()));
				CHECK(package_config_json["dependencies"][package.get_name()] == package.get_version().string());
			}
		}

		WHEN("a package is removed") {
			cpm::Package package("test-package", cpm::SemVer("1.0.0"));
			package_config.add(package);
			package_config.remove(package);
			json package_config_json = json::parse(std::ifstream("test_cpm_pack.json"));

			THEN("the package config doesn't contain it") {
				CHECK_FALSE(package_config_json["dependencies"].contains(package.get_name()));
			}
		}

		WHEN("packages are listed") {
			cpm::Package package("test-package", cpm::SemVer("1.0.0"));
			package_config.add(package);
			auto listed_dependencies = package_config.list();
			json package_config_json = json::parse(std::ifstream("test_cpm_pack.json"));
    		decltype(listed_dependencies) actual_dependencies = {
				package
			};

			THEN("a list of them is returned") {
				CHECK(listed_dependencies == actual_dependencies);
			}
		}

		WHEN("checking if the package config contains a package") {
			cpm::Package package("test-package", cpm::SemVer("1.0.0"));
			package_config.add(package);
			json package_config_json = json::parse(std::ifstream("test_cpm_pack.json"));

			THEN("a result of true of false is returned") {
				CHECK(package_config.contains(package));
				CHECK(package_config_json["dependencies"].contains(package.get_name()));
				CHECK(package_config_json["dependencies"][package.get_name()] == package.get_version().string());
			}
		}

		WHEN("a default package config is created") {
			package_config.create_default();
			json package_config_json = json::parse(std::ifstream("test_cpm_pack.json"));

			THEN("its name and version have default values") {
				CHECK(package_config_json["name"] == fs::current_path().filename().string());
				CHECK(package_config_json["version"] == "0.1.0");
			}
		}
	}
}
