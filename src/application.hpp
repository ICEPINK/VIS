#pragma once

#include "gui.hpp"
#include "window.hpp"

#include <string_view>
#include <vector>

namespace Vis {

class Application {
  public:
    Application(const std::vector<std::string_view> &args);
    ~Application();

    auto run_main_loop() -> void;

  private:
    [[nodiscard]] auto handle_args(const std::vector<std::string_view> &args)
        -> bool;
    auto print_help() -> bool;
    auto print_version() -> bool;
    auto make_gui(bool show_debug) -> void;
    auto handle_input() -> void;

  private:
    std::shared_ptr<Glfw> p_glfw{nullptr};
    std::unique_ptr<Window> p_window{nullptr};
    std::unique_ptr<Gui> p_gui{nullptr};
    size_t m_width{800};
    size_t m_height{600};
    std::string m_title{"VIS"};
    bool m_alt_mode{false};
    double m_mouse_pos_x{0.0};
    double m_mouse_pos_y{0.0};
    double test_blue{0.0};
};

} // namespace Vis
