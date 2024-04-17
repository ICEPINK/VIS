#pragma once

#include <iostream>
#include <vector>

namespace Vis {

[[nodiscard]] inline auto handle_exception() -> int {
    try {
        throw;
    } catch (const std::runtime_error &e) {
        std::cout << "ERROR: " << e.what() << '\n';
    } catch (const std::exception &e) {
        std::cout << "ERROR: " << e.what() << '\n';
    } catch (const int &e) {
        std::cout << "ERROR: Int exception (" << e << ")!\n";
        return e;
    } catch (...) {
        std::cout << "ERROR: Not handled!!!\n";
    }
    return EXIT_FAILURE;
}

[[nodiscard]] inline auto make_args(int argc, char **argv)
    -> std::vector<std::string_view> {
    std::vector<std::string_view> args;
    args.reserve(argc);
    for (size_t i = 0; i < static_cast<size_t>(argc); ++i) {
        args.push_back(argv[i]);
    }

    return args;
}

} // namespace Vis
