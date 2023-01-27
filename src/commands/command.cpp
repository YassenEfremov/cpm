#include "command.hpp"

#include "argparse/argparse.hpp"

#include <string>


namespace cpm {

    Command::Command(const std::string &name)
        : ArgumentParser(name, "", argparse::default_arguments::help) {}
}
