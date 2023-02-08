#include "commands/command.hpp"

#include "argparse/argparse.hpp"

#include <string>
#include <utility>


namespace cpm {

	Command::Command(const std::string &name)
        : ArgumentParser(name, "", argparse::default_arguments::help) {}
	
	Command::Command() {}
}
