#include "parser.hpp"

#include "version.hpp"

#include "../commands/command.hpp"
#include "../commands/install.hpp"
#include "../commands/remove.hpp"
#include "../commands/list.hpp"
#include "../commands/create.hpp"

#include "argparse/argparse.hpp"

#include <stdexcept>
#include <string>


namespace cpm {

    std::map<std::string, Command *> commands;

	void parse_args(int argc, char *argv[]) {

        argparse::ArgumentParser parser("cpm", CPM_VERSION);


        static InstallCommand install_command("install");
        install_command.add_description("Install the specified package/s");
        install_command.add_argument("packages")
            .help("Packages to install")
            .required()
            .nargs(argparse::nargs_pattern::at_least_one);
        install_command.add_argument("-g", "--global")
            .help("installs package/s globally")
            .default_value(false)
            .implicit_value(true);


        static RemoveCommand remove_command("remove");
        remove_command.add_description("Remove the specified package/s");
        remove_command.add_argument("packages")
            .help("Packages to remove")
            .required()
            .nargs(argparse::nargs_pattern::at_least_one);
        remove_command.add_argument("-g", "--global")
            .help("removes package/s globally")
            .default_value(false)
            .implicit_value(true);


        static ListCommand list_command("list");
        list_command.add_description("List all installed packages");
        list_command.add_argument("-g", "--global")
            .help("lists globally installed package/s")
            .default_value(false)
            .implicit_value(true);

        static CreateCommand create_command("create");
        create_command.add_description("Create a new package");


        // argparse::ArgumentParser update_command("update");
        // update_command.add_description("Update the specified package/s");
        // update_command.add_argument("packages")
        //     .help("updates the specified package/s")
        //     .required()
        //     .nargs(argparse::nargs_pattern::at_least_one);

        parser.add_subparser(install_command);
        parser.add_subparser(remove_command);
        parser.add_subparser(list_command);
        parser.add_subparser(create_command);
        // parser.add_subparser(update_command);
        commands.insert({"install", &install_command});
        commands.insert({"remove", &remove_command});
        commands.insert({"list", &list_command});
        commands.insert({"create", &create_command});
        // commands.insert({"update", &update_command});

        if (argc < 2) {
            throw std::runtime_error(parser.help().str());
        }
        parser.parse_args(argc, argv);
    }
}
