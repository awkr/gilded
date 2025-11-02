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

void run(const Config &config) {
    GLFWwindow *window;
    create_window(config, &window);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
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
