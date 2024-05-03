#pragma once

#include "gui.hpp"
#include "image.hpp"
#include "texture.hpp"
#include "window.hpp"

#include <string_view>
#include <vector>

namespace Vis {

class Application {
  public:
    Application(const std::vector<std::string_view> &args = {});
    ~Application();

    auto run() -> void;
    [[nodiscard]] auto exit() const -> bool;

  private:
    [[nodiscard]] auto
    handle_args(const std::vector<std::string_view> &args) -> bool;
    auto arg_print_help() -> bool;
    auto arg_print_version() -> bool;
    auto arg_resolution(std::string_view resolution) -> void;
    auto make_gui(bool show_debug = false) -> void;
    auto handle_input() -> void;
    auto render_image() -> void;

  private:
    std::shared_ptr<Glfw> p_glfw{nullptr};
    std::unique_ptr<Window> p_window{nullptr};
    std::unique_ptr<Gui> p_gui{nullptr};
    std::unique_ptr<Texture> p_texture{nullptr};
    size_t m_width{800};
    size_t m_height{600};
    float m_panel_width{0.0f};
    float m_panel_height{0.0f};
    std::string m_title{"VIS"};
    bool m_alt_mode{false};
    bool m_exit{false};
    double m_mouse_pos_x{0.0};
    double m_mouse_pos_y{0.0};
    Image m_image{};
    double m_last_loop_time{0};
    double test_blue{0.0};
};

} // namespace Vis
