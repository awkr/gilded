#include "vk.h"
#include <cassert>
#include <iostream>
#include <vector>
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

VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *user_data) {
    std::cerr << "validation layer: ";
    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::cerr << "error: ";
    } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << "warning: ";
    } else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        std::cerr << "info: ";
    } else {
        std::cerr << "verbose: ";
    }
    std::cerr << callback_data->pMessage << std::endl;
    return VK_FALSE;
}

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

void choose_physical_device_with_graphics_queue(const std::vector<VkPhysicalDevice> &physical_devices,
                                                VkPhysicalDevice *selected_physical_device,
                                                uint32_t *selected_queue_family_index) {
    for (const auto physical_device: physical_devices) {
        VkPhysicalDeviceProperties physical_device_properties;
        vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
        if (physical_device_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            continue; // prefer discrete GPU
        }

        uint32_t queue_family_count;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties.data());

        // inspect the list of queue properties to see if the physical device supports graphics
        for (uint32_t i = 0; i < queue_family_count; i++) {
            if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                *selected_physical_device = physical_device;
                *selected_queue_family_index = i;
                return;
            }
        }
    }
    assert(false);
}

void create_vk_device(VkContext *context) {
    uint32_t physical_device_count = 0;
    VkResult result = vkEnumeratePhysicalDevices(context->instance, &physical_device_count, nullptr);
    assert(result == VK_SUCCESS);

    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    result = vkEnumeratePhysicalDevices(context->instance, &physical_device_count, physical_devices.data());
    assert(result == VK_SUCCESS);

    choose_physical_device_with_graphics_queue(physical_devices, &context->physical_device,
                                               &context->queue_family_index);

    // describe the device queue
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo device_queue_create_info{};
    device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    device_queue_create_info.queueFamilyIndex = context->queue_family_index;
    device_queue_create_info.queueCount = 1;
    device_queue_create_info.pQueuePriorities = &queue_priority;

    // describe the device
    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = &device_queue_create_info;
    device_create_info.queueCreateInfoCount = 1;

    std::vector<const char *> device_layers;
    device_layers.push_back("VK_LAYER_KHRONOS_validation");

    std::vector<const char *> device_extensions;

    // 1. 基础平台扩展
    device_extensions.push_back("VK_KHR_swapchain");

    // 2. 光线追踪底层依赖
    device_extensions.push_back("VK_KHR_deferred_host_operations"); // 用于异步构建 AS
    device_extensions.push_back("VK_KHR_acceleration_structure"); // 核心 RT 基础

    // 3. 光线追踪高级功能
    device_extensions.push_back("VK_KHR_ray_query"); // 依赖 AS
    device_extensions.push_back("VK_KHR_pipeline_library"); // 被 ray tracing pipeline 使用
    device_extensions.push_back("VK_KHR_ray_tracing_pipeline"); // 顶层功能

    device_create_info.enabledLayerCount = device_layers.size();
    device_create_info.ppEnabledLayerNames = device_layers.data();
    device_create_info.enabledExtensionCount = device_extensions.size();
    device_create_info.ppEnabledExtensionNames = device_extensions.data();

    // enable extension features
    VkPhysicalDeviceRobustness2FeaturesEXT robustness_features{};
    robustness_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
    robustness_features.nullDescriptor = VK_TRUE; // allow null descriptors in descriptor sets

    VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features{};
    acceleration_structure_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    acceleration_structure_features.pNext = &robustness_features;
    acceleration_structure_features.accelerationStructure = VK_TRUE;

    VkPhysicalDeviceRayQueryFeaturesKHR ray_query_features{};
    ray_query_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
    ray_query_features.pNext = &acceleration_structure_features;
    ray_query_features.rayQuery = VK_TRUE;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR ray_tracing_pipeline_features{};
    ray_tracing_pipeline_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    ray_tracing_pipeline_features.pNext = &acceleration_structure_features;
    ray_tracing_pipeline_features.rayTracingPipeline = VK_TRUE;
    ray_tracing_pipeline_features.rayTracingPipelineTraceRaysIndirect = VK_TRUE;
    ray_tracing_pipeline_features.rayTraversalPrimitiveCulling = VK_TRUE;

    device_create_info.pNext = &ray_tracing_pipeline_features;

    result = vkCreateDevice(context->physical_device, &device_create_info, nullptr, &context->device);
    assert(result == VK_SUCCESS);

    context->vkSetDebugUtilsObjectNameEXT = LOAD_DEVICE_PROC(context->device, vkSetDebugUtilsObjectNameEXT);

    vkGetDeviceQueue(context->device, context->queue_family_index, 0, &context->queue);
    assert(context->queue);

    set_object_name(context, context->device, VK_OBJECT_TYPE_DEVICE, "my_vk_device");
    set_object_name(context, context->queue, VK_OBJECT_TYPE_QUEUE, "my_vk_queue");
    set_object_name(context, context->surface, VK_OBJECT_TYPE_SURFACE_KHR, "my_vk_surface");
}

void create_device(VkContext *context, GLFWwindow *window) {
    create_vk_instance(context);
    create_vk_surface(context, window);
    create_vk_device(context);
}

void vk_cleanup(VkContext *context) {
    vkDestroyDevice(context->device, nullptr);
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
