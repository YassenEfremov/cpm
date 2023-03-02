#include "cli/parser.hpp"

#include "commands/command.hpp"
#include "commands/create.hpp"
#include "commands/install.hpp"
#include "commands/list.hpp"
#include "commands/remove.hpp"
#include "commands/sync.hpp"
#include "db/package_db.hpp"
#include "dep-man/lockfile.hpp"
#include "paths.hpp"
#include "script/package_config.hpp"
#include "version.hpp"

#include "argparse/argparse.hpp"

#include <exception>
#include <memory>
#include <stdexcept>
#include <string>


namespace cpm {

    std::map<std::string, Command *> Parser::commands;

	void Parser::parse_args(int argc, char *argv[]) {

        argparse::ArgumentParser parser("cpm", CPM_VERSION);


        static InstallCommand install_command("install");
        install_command.add_description("Install the specified package/s");
        install_command.add_argument("packages")
            .help("Packages to install")
            .required()
            .nargs(argparse::nargs_pattern::at_least_one);
        install_command.add_argument("-g", "--global")
            .help("install package/s globally")
            .default_value(false)
            .implicit_value(true);


        static RemoveCommand remove_command("remove");
        remove_command.add_description("Remove the specified package/s");
        remove_command.add_argument("packages")
            .help("Packages to remove")
            .required()
            .nargs(argparse::nargs_pattern::at_least_one);
        remove_command.add_argument("-g", "--global")
            .help("remove package/s globally")
            .default_value(false)
            .implicit_value(true);


        static ListCommand list_command("list");
        list_command.add_description("List installed packages");
        list_command.add_argument("-g", "--global")
            .help("list globally installed package/s")
            .default_value(false)
            .implicit_value(true);
        list_command.add_argument("-a", "--all")
            .help("list all packages, including dependencies")
            .default_value(false)
            .implicit_value(true);


        static CreateCommand create_command("create");
        create_command.add_description("Create a new package");


        static SyncCommand sync_command("sync");
        sync_command.add_description(
            "Install the package/s specified in the current package config"
        );


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
        parser.add_subparser(sync_command);
        // parser.add_subparser(update_command);
        commands.insert({"install", &install_command});
        commands.insert({"remove", &remove_command});
        commands.insert({"list", &list_command});
        commands.insert({"create", &create_command});
        commands.insert({"sync", &sync_command});
        // commands.insert({"update", &update_command});

        if (argc < 2) {
            throw std::runtime_error(parser.help().str());
        }
        parser.parse_args(argc, argv);


        try {
            if (commands[argv[1]]->is_used("--global")) {
                commands[argv[1]]->context = Command::Context{
                    paths::global_dir,
                    std::make_shared<PackageDB>(
                        paths::global_dir / paths::package_db
                    ),
                    std::make_shared<Lockfile>(
                        paths::global_dir / paths::lockfile
                    )
                };

            } else {
                throw std::runtime_error("");
            }

        } catch(const std::exception &e) {
            commands[argv[1]]->context = Command::Context{
                fs::current_path(),
                std::make_shared<PackageConfig>(
                    fs::current_path() / paths::package_config
                ),
                std::make_shared<Lockfile>(
                    fs::current_path() / paths::lockfile
                )
            };
        }
    }
}
