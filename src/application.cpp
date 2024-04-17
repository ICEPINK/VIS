#include "application.hpp"

#include <iostream>

namespace Vis {

Application::Application(const std::vector<std::string_view> &args) {
    if (handle_args(args)) {
        return;
    }
}

Application::~Application() {}

[[nodiscard]] auto
Application::handle_args(const std::vector<std::string_view> &args) -> bool {
    [[maybe_unused]] int i = 0;
    for (const auto &arg : args) {
        if (arg == "-h" || arg == "--help") {
            return print_help();
        }
        if (arg == "-v" || arg == "--version") {
            return print_version();
        }
        ++i;
    }
    return false;
}

auto Application::print_help() -> bool {
    std::cout << "VIS HELP:\n";
    std::cout << " --help, -h: print help\n";
    std::cout << " --version, -v: print version\n";
    return true;
}

auto Application::print_version() -> bool {
    std::cout << "VIS version: 0.0.0\n";
    return true;
}

} // namespace Vis
