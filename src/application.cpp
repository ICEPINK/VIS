#include "application.hpp"

#include <cstdlib>
#include <iostream>

namespace Vis {

Application::Application() {
    std::cout << "App -> VIS\n";
    std::cout << "App -> Init\n";
}
Application::~Application() {}

int Application::run() {
    std::cout << "App -> Start\n";

    std::cout << "App -> Exit success\n";
    return EXIT_SUCCESS;
}

} // namespace Vis
