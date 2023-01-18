#include "package.hpp"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

	Package::Package(const fs::path &url) : url(url), name(url.stem().string()) {}

	const fs::path &Package::get_url() const { return this->url; }
	const std::string &Package::get_name() const { return this->name; }
}
