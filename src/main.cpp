#include "cli/parser.hpp"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <iostream>
#include <exception>

#include <cstdlib>


int main(int argc, char *argv[]) {

    auto stdout_logger = spdlog::stdout_color_mt("stdout_logger");
    stdout_logger->set_pattern("%v");
    auto stderr_logger = spdlog::stderr_color_mt("stderr_logger");
    stderr_logger->set_pattern("%^%l%$: %v");

    try {
        cpm::parse_args(argc, argv);

    } catch (const std::exception &e) {
        if (argc < 2) {
            // Print usage
            spdlog::get("stdout_logger")->error(e.what());

        } else {
            spdlog::get("stderr_logger")->error(e.what());
        }
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
