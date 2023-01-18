#include "parser.hpp"

#include "version.hpp"

#include "../commands/command.hpp"
#include "../commands/install.hpp"
#include "../commands/remove.hpp"
#include "../commands/list.hpp"
#include "../package.hpp"

#include "argparse/argparse.hpp"

#include <exception>
#include <stdexcept>
#include <string>
#include <vector>


namespace cpm {

    std::map<std::string, Command*> commands;

	const std::vector<Package> parse_args(int argc, char *argv[]) {

        argparse::ArgumentParser parser("cpm", CPM_VERSION);

        static InstallCommand install_command("install");
        install_command.add_description("Install the given package/s");
        install_command.add_argument("packages")
            .help("URL of a GitHub repository")
            .required()
            .nargs(argparse::nargs_pattern::at_least_one);

        static RemoveCommand remove_command("remove");
        remove_command.add_description("Remove the given package/s");
        remove_command.add_argument("packages")
            .help("Packages to remove")
            .required()
            .nargs(argparse::nargs_pattern::at_least_one);

        static ListCommand list_command("list");
        list_command.add_description("List all installed packages");

        // argparse::ArgumentParser update_command("update");
        // update_command.add_description("Update the given package/s");
        // update_command.add_argument("packages")
        //     .help("Packages to update")
        //     .required()
        //     .nargs(argparse::nargs_pattern::at_least_one);

        parser.add_subparser(install_command);
        parser.add_subparser(remove_command);
        parser.add_subparser(list_command);
        // parser.add_subparser(update_command);
        commands.insert({"install", &install_command});
        commands.insert({"remove", &remove_command});
        commands.insert({"list", &list_command});

        if (argc < 2) {
            throw std::runtime_error(parser.help().str());
        }
        parser.parse_args(argc, argv);

        try {
            auto package_names = commands[argv[1]]->get<std::vector<std::string>>("packages");
            return std::vector<Package>(package_names.begin(), package_names.end());

        } catch (std::exception &e) {
            return std::vector<Package>();
        }
    }
}
