#include "cli/parser.hpp"
#include "config.hpp"
#include "logger/logger.hpp"

#include <chrono>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <unordered_set>

#include <cstdlib>


using namespace std::chrono;
using namespace std::chrono_literals;


/**
 * @brief Execute and time the specified cpm command
 * 
 * @param command the cpm command to time
 * 
 * @return The command execution time in milliseconds
 */
milliseconds time_execution(std::function<void()> command) {
    auto start_time = high_resolution_clock::now();
    command();
    auto end_time = high_resolution_clock::now();
    return duration_cast<milliseconds>(end_time - start_time);
}


int main(int argc, char *argv[]) {

    cpm::CLILogger::init();
    cpm::FileLogger::init();
    cpm::Config::init();

    try {
        cpm::Parser::parse_args(argc, argv);

        std::unordered_set<std::string> timed_commands = {
            "install", "remove", "sync"
        };

        if (timed_commands.find(argv[1]) != timed_commands.end()) {
            auto command = [&]() { cpm::Parser::commands[argv[1]]->run(); };
            auto execution_time = time_execution(command);
            if (execution_time < 1s) {
                CPM_INFO("Finished in {}ms\n",
                         execution_time.count());
            } else {
                CPM_INFO("Finished in {}s\n",
                         duration<double>(execution_time).count());
            }

        } else {
            cpm::Parser::commands[argv[1]]->run();
        }

    } catch (const std::exception &e) {
        if (argc < 2) {
            // Used to print usage (not formatted as an error)
            CPM_INFO(e.what());

        } else {
            CPM_LOG_ERR(e.what());
            CPM_ERR(e.what());
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
