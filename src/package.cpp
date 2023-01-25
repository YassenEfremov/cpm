#include "package.hpp"

#include <filesystem>
#include <string>

namespace fs = std::filesystem;


namespace cpm {

	Package::Package(const fs::path &url) {
		std::string url_string = url.string();
		while (url_string.back() == '/') {
			url_string.pop_back();
		}
		this->url = url_string;
		this->name = this->url.stem().string();
	}

	const fs::path &Package::get_url() const { return this->url; }
	const std::string &Package::get_name() const { return this->name; }
}
