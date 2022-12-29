#include "parser.hpp"

#include "version.hpp"

#include "version.hpp"

#include "../commands/install.hpp"
#include "../commands/remove.hpp"

#include "argparse/argparse.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>


namespace cpm {

    void parse_args(int argc, char *argv[]) {

        argparse::ArgumentParser parser("cpm", CPM_VERSION);

        // Install command
        argparse::ArgumentParser install_command("install");
        install_command.add_description("Install the given package/s");
        install_command.add_argument("packages")
            .help("Link to a GitHub repository")
            .required()
            .nargs(argparse::nargs_pattern::at_least_one)
            .action(cpm::install);

        // Remove command
        argparse::ArgumentParser remove_command("remove");
        remove_command.add_description("Remove the given package/s");
        remove_command.add_argument("packages")
            .help("Packages to remove")
            .required()
            .nargs(argparse::nargs_pattern::at_least_one)
            .action(cpm::remove);

        // List command
        argparse::ArgumentParser list_command("list");
        list_command.add_description("List all installed packages");

        // Update command
        argparse::ArgumentParser update_command("update");
        update_command.add_description("Update the given package/s");
        update_command.add_argument("packages")
            .help("Packages to update")
            .required()
            .nargs(argparse::nargs_pattern::at_least_one);

        parser.add_subparser(install_command);
        parser.add_subparser(remove_command);
        parser.add_subparser(list_command);
        parser.add_subparser(update_command);

        if (argc < 2) {
            throw std::runtime_error(parser.help().str());
        }
        parser.parse_args(argc, argv);
    }
}
