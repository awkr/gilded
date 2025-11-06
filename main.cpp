#include "vk.h"
#include <cassert>
#include <iostream>
#include <GLFW/glfw3.h>

struct Config {
    uint32_t width;
    uint32_t height;
    std::string title;
};

void on_framebuffer_resized(GLFWwindow *window, int width, int height) {
}

void create_window(const Config &config, GLFWwindow **window) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    int width = (int) config.width;
    int height = (int) config.height;
    *window = glfwCreateWindow(width, height, config.title.c_str(), nullptr, nullptr);
    assert(*window);
    glfwSetFramebufferSizeCallback(*window, on_framebuffer_resized);
}

void destroy_window(GLFWwindow *window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void handle_keyboard_input(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void handle_mouse_position(GLFWwindow *window, double x, double y) {
}

void handle_mouse_button(GLFWwindow *window, int button, int action, int mods) {
}

void handle_mouse_scroll(GLFWwindow *window, double xoffset, double yoffset) {
}

void init_input(GLFWwindow *window) {
    glfwSetKeyCallback(window, handle_keyboard_input);
    glfwSetCursorPosCallback(window, handle_mouse_position);
    glfwSetMouseButtonCallback(window, handle_mouse_button);
    glfwSetScrollCallback(window, handle_mouse_scroll);
}

void poll_events(GLFWwindow *window) {
}

void run(const Config &config) {
    GLFWwindow *window;
    create_window(config, &window);
    init_input(window);
    VkContext context{};
    init_vk(&context, window);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        poll_events(window);
    }
    cleanup_vk(&context);
    destroy_window(window);
}

int main() {
    std::cout << "another age gilded. another world built on borrowed time." << std::endl;
    Config config{};
    config.width = 800;
    config.height = 600;
    config.title = "Gilded";
    run(config);
    return 0;
}
