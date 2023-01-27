#include "package.hpp"

#include <string>


namespace cpm {

	Package::Package(const std::string &name) : name(name) {}

	const std::string &Package::get_name() const { return this->name; }
}
