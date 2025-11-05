#include "vk.h"

void set_object_name(VkContext *context, void *object_handle, VkObjectType object_type, const char *name) {
    VkDebugUtilsObjectNameInfoEXT name_info{};
    name_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    name_info.objectType = object_type;
    name_info.objectHandle = (uint64_t) object_handle;
    name_info.pObjectName = name;
    context->vkSetDebugUtilsObjectNameEXT(context->device, &name_info);
}
