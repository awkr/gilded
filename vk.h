#pragma once

#include <vulkan/vulkan.h>

#define LOAD_INSTANCE_PROC(instance, name) (PFN_##name) vkGetInstanceProcAddr(instance, #name);
#define LOAD_DEVICE_PROC(device, name) (PFN_##name) vkGetDeviceProcAddr(device, #name);

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
};

void set_object_name(VkContext *context, void * object_handle, VkObjectType object_type, const char *name);
