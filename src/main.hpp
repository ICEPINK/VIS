#pragma once

#include <iostream>

namespace Vis {

[[nodiscard]] inline auto handle_exception() -> int {
    try {
        throw;
    } catch (const std::runtime_error &e) {
        std::cout << "ERROR: " << e.what() << '\n';
    } catch (const std::out_of_range &e) {
        std::cout << "ERROR: " << e.what() << " out of range!" << '\n';
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

} // namespace Vis
