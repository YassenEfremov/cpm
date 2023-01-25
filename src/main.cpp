#include "cli/parser.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/pattern_formatter.h"

#include <exception>
#include <memory>

#include <cstdlib>


int main(int argc, char *argv[]) {

    auto stdout_logger = spdlog::stdout_color_mt("stdout_logger");
    stdout_logger->set_formatter(std::make_unique<spdlog::pattern_formatter>(
        "%v", spdlog::pattern_time_type::local, ""
    ));
    spdlog::set_default_logger(stdout_logger);

    auto stderr_logger = spdlog::stderr_color_mt("stderr_logger");
    stderr_logger->set_pattern("%^%l%$: %v");


    try {
        cpm::parse_args(argc, argv);
        cpm::commands[argv[1]]->run();

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
