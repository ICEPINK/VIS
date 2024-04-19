#include "main.hpp"
#include "application.hpp"

auto main(int argc, char **argv) -> int {
    const auto args = Vis::make_args(argc, argv);

    try {
        Vis::Application application(args);
        application.run_main_loop();
    } catch (...) {
        return Vis::handle_exception();
    }
    return EXIT_SUCCESS;
}
