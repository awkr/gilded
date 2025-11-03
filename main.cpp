#include <cassert>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>
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

#define LOAD_INSTANCE_PROC(instance, name) (PFN_##name)vkGetInstanceProcAddr(instance, #name);

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *pUserData) {
    std::cerr << "validation layer: " << callback_data->pMessage << std::endl;
    return VK_FALSE;
}

struct VkContext {
    VkInstance instance;
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    VkDebugUtilsMessengerEXT debug_utils_messenger;
    VkSurfaceKHR surface;
};

void create_vk_instance(VkContext *context) {
    int vk_supported = glfwVulkanSupported();
    assert(vk_supported == GLFW_TRUE);

    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char *> extensions;
    for (uint32_t i = 0; i < glfw_extension_count; i++) {
        extensions.push_back(glfw_extensions[i]);
    }
    extensions.push_back("VK_KHR_get_physical_device_properties2");
    extensions.push_back("VK_EXT_debug_utils");

    // describe the instance
    VkInstanceCreateInfo instance_create_info{};
    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.enabledExtensionCount = extensions.size();
    instance_create_info.ppEnabledExtensionNames = extensions.data();

    std::vector<const char *> layers;
    layers.push_back("VK_LAYER_KHRONOS_validation");

    instance_create_info.enabledLayerCount = layers.size();
    instance_create_info.ppEnabledLayerNames = layers.data();

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
    debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_create_info.pfnUserCallback = debug_callback;

    instance_create_info.pNext = &debug_create_info;

    // describe the application
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_API_VERSION_1_3;
    app_info.pApplicationName = "Gilded Application";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    app_info.pEngineName = "Gilded Engine";
    app_info.engineVersion = VK_MAKE_VERSION(0, 0, 1);

    instance_create_info.pApplicationInfo = &app_info;

    VkResult result = vkCreateInstance(&instance_create_info, nullptr, &context->instance);
    assert(result == VK_SUCCESS);

    context->vkCreateDebugUtilsMessengerEXT = LOAD_INSTANCE_PROC(context->instance, vkCreateDebugUtilsMessengerEXT);
    context->vkDestroyDebugUtilsMessengerEXT = LOAD_INSTANCE_PROC(context->instance, vkDestroyDebugUtilsMessengerEXT);

    context->vkCreateDebugUtilsMessengerEXT(context->instance, &debug_create_info, nullptr,
                                            &context->debug_utils_messenger);
}

void create_vk_surface(VkContext *context, GLFWwindow *window) {
    VkResult result = glfwCreateWindowSurface(context->instance, window, nullptr, &context->surface);
    assert(result == VK_SUCCESS);
}

void create_vk_device() {
}

void create_device(VkContext *context, GLFWwindow *window) {
    create_vk_instance(context);
    create_vk_surface(context, window);
    create_vk_device();
}

void vk_cleanup(VkContext *context) {
    vkDestroySurfaceKHR(context->instance, context->surface, nullptr);
    context->vkDestroyDebugUtilsMessengerEXT(context->instance, context->debug_utils_messenger, nullptr);
    vkDestroyInstance(context->instance, nullptr);
}

void run(const Config &config) {
    GLFWwindow *window;
    create_window(config, &window);
    VkContext context{};
    create_device(&context, window);
    init_input(window);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        poll_events(window);
    }
    vk_cleanup(&context);
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
