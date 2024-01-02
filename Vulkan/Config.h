#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"

namespace Vulkan::Config {

    std::vector<const char*> GetRequiredLayers();
    std::vector<const char*> GetRequiredInstanceExtensions();

    void CheckLayersSupport();
    void CheckInstanceExtensionSupport();

};  // namespace Vulkan::Config