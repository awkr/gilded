#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#define LOAD_INSTANCE_PROC(instance, name) (PFN_##name) vkGetInstanceProcAddr(instance, #name);
#define LOAD_DEVICE_PROC(device, name) (PFN_##name) vkGetDeviceProcAddr(device, #name);

#define MAX_FRAMES_IN_FLIGHT 2

struct VkContext {
    VkInstance instance;
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    VkDebugUtilsMessengerEXT debug_utils_messenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    uint32_t queue_family_index;
    VkDevice device;
    PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT;
    VkQueue queue;
    VkCommandPool command_pool;
    std::vector<VkCommandBuffer> command_buffers;
};

void create_vk_instance(VkContext *context);

void create_vk_surface(VkContext *context, GLFWwindow *window);

void create_vk_device(VkContext *context);

void create_command_pool(VkContext *context, VkCommandPool *command_pool);

void create_command_buffers(VkContext *context);

void init_vk(VkContext *context, GLFWwindow *window);

void cleanup_vk(VkContext *context);

void choose_physical_device_with_graphics_queue(const std::vector<VkPhysicalDevice> &physical_devices,
                                                VkPhysicalDevice *selected_physical_device,
                                                uint32_t *selected_queue_family_index);

void set_object_name(VkContext *context, void *object_handle, VkObjectType object_type, const char *name);
