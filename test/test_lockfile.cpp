#include "dep-man/lockfile.hpp"
#include "semver.hpp"
#include "package.hpp"

#include "catch2/catch_test_macros.hpp"

#include <filesystem>
#include <fstream>
#include <unordered_set>	

namespace fs = std::filesystem;
using json = nlohmann::ordered_json;


SCENARIO("lockfile basic operations", "[lockfile]") {

	GIVEN("an empty lockfile") {

		fs::remove("test_cpm_lock.json");
		cpm::Lockfile lockfile("test_cpm_lock.json");

		WHEN("a direct dependency is added") {
			cpm::Package dep("test-dep", cpm::SemVer("1.0.0"));
			lockfile.add(dep);

			THEN("the lockfile contains it") {
				json lockfile_json = json::parse(std::ifstream("test_cpm_lock.json"));
				CHECK(
					lockfile_json
						["dependencies"].contains(dep.get_name())
				);
				CHECK(
					lockfile_json
						["dependencies"]
							[dep.get_name()]
								["version"] == dep.get_version().string()
				);
			}
		}

		WHEN("a transitive dependency is added") {
			cpm::Package dep("test-dep", cpm::SemVer("1.0.0"));
			cpm::Package trans_dep("test-trans-dep", cpm::SemVer("1.0.0"));
			lockfile.add(dep);
			lockfile.add_dep(dep, trans_dep);

			THEN("the lockfile contains it") {
				json lockfile_json = json::parse(std::ifstream("test_cpm_lock.json"));
				CHECK(
					lockfile_json
						["dependencies"].contains(dep.get_name())
				);
				CHECK(
					lockfile_json
						["dependencies"]
							[dep.get_name()]
								["version"] == dep.get_version().string()
				);
				CHECK(
					lockfile_json
						["dependencies"]
							[dep.get_name()]
								["dependencies"].contains(trans_dep.get_name())
				);
				CHECK(
					lockfile_json
						["dependencies"]
							[dep.get_name()]
								["dependencies"]
									[trans_dep.get_name()] == trans_dep.get_version().string()
				);
			}
		}

		WHEN("a direct dependency is removed") {
			cpm::Package dep("test-dep", cpm::SemVer("1.0.0"));
			lockfile.add(dep);
			lockfile.remove(dep);

			THEN("the lockfile doesn't contain it") {
				json lockfile_json = json::parse(std::ifstream("test_cpm_lock.json"));
				CHECK_FALSE(
					lockfile_json
						["dependencies"].contains(dep.get_name())
				);
			}
		}

		WHEN("a transitive dependency is removed") {
			cpm::Package dep("test-dep", cpm::SemVer("1.0.0"));
			cpm::Package trans_dep("test-trans-dep", cpm::SemVer("1.0.0"));
			lockfile.add(dep);
			lockfile.add_dep(dep, trans_dep);
			lockfile.remove_dep(dep, trans_dep);

			THEN("the lockfile doesn't contain it") {
				json lockfile_json = json::parse(std::ifstream("test_cpm_lock.json"));
				CHECK_FALSE(
					lockfile_json
						["dependencies"]
							[dep.get_name()]
								["dependencies"].contains(trans_dep.get_name())
				);
			}
		}

		WHEN("packages are listed") {
			cpm::Package dep("test-dep", cpm::SemVer("1.0.0"));
			lockfile.add(dep);
			auto listed_dependencies = lockfile.list();
			json lockfile_json = json::parse(std::ifstream("test_cpm_lock.json"));
    		decltype(listed_dependencies) actual_dependencies = {
				dep
			};

			THEN("a list of them is returned") {
				CHECK(listed_dependencies == actual_dependencies);
			}
		}

		WHEN("checking if the lockfile contains a direct dependency") {
			cpm::Package dep("test-dep", cpm::SemVer("1.0.0"));
			lockfile.add(dep);

			THEN("a result of true of false is returned") {
				CHECK(lockfile.contains(dep));
				json lockfile_json = json::parse(std::ifstream("test_cpm_lock.json"));
				CHECK(
					lockfile_json
						["dependencies"].contains(dep.get_name())
				);
				CHECK(
					lockfile_json
						["dependencies"]
							[dep.get_name()]
								["version"] == dep.get_version().string()
				);
			}
		}

		WHEN("checking if the lockfile contains a transitive dependency") {
			cpm::Package dep("test-dep", cpm::SemVer("1.0.0"));
			cpm::Package trans_dep("test-trans-dep", cpm::SemVer("1.0.0"));
			lockfile.add(dep);
			lockfile.add_dep(dep, trans_dep);

			THEN("a result of true of false is returned") {
				CHECK(lockfile.contains_dep(trans_dep));
				json lockfile_json = json::parse(std::ifstream("test_cpm_lock.json"));
				CHECK(
					lockfile_json
						["dependencies"]
							[dep.get_name()]
								["dependencies"].contains(trans_dep.get_name())
				);
				CHECK(
					lockfile_json
						["dependencies"]
							[dep.get_name()]
								["dependencies"]
									[trans_dep.get_name()] == trans_dep.get_version().string()
				);
			}
		}
	}
}

