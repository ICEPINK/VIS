#include "main.hpp"
#include "application.hpp"

auto main(int argc, char** argv) -> int {
    const std::vector<std::string_view> args(argv, argv + argc);
    try { Vis::Application application(args); }
    catch (...) { return Vis::handle_exception(); }
}
