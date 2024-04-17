#pragma once

#include <string_view>
#include <vector>

namespace Vis {

class Application {
  public:
    Application(const std::vector<std::string_view> &args);
    ~Application();

  private:
    [[nodiscard]] auto handle_args(const std::vector<std::string_view> &args)
        -> bool;
    auto print_help() -> bool;
    auto print_version() -> bool;
};

} // namespace Vis
