#include "cli/parser.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/pattern_formatter.h"

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

    auto stdout_logger = spdlog::stdout_color_mt("stdout_logger");
    stdout_logger->set_formatter(std::make_unique<spdlog::pattern_formatter>(
        "%v", spdlog::pattern_time_type::local, ""
    ));
    spdlog::set_default_logger(stdout_logger);

    auto stderr_logger = spdlog::stderr_color_mt("stderr_logger");
    stderr_logger->set_pattern("%^%l%$: %v");


    try {
        cpm::Parser::parse_args(argc, argv);

        std::unordered_set<std::string> timed_commands = {
            "install", "remove", "sync"
        };

        if (timed_commands.find(argv[1]) != timed_commands.end()) {
            auto command = [&]() { cpm::Parser::commands[argv[1]]->run(); };
            auto execution_time = time_execution(command);
            if (execution_time < 1000ms) {
                spdlog::info("Finished in {}ms\n",
                             execution_time.count());
            } else {
                spdlog::info("Finished in {}s\n",
                             duration<double>(execution_time).count());
            }

        } else {
            cpm::Parser::commands[argv[1]]->run();
        }

    } catch (const std::exception &e) {
        if (argc < 2) {
            // Used to print usage (not formatted as an error)
            spdlog::get("stdout_logger")->error(e.what());

        } else {
            spdlog::get("stderr_logger")->error(e.what());
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
