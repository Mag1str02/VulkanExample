#pragma once

#include <vulkan/vulkan.h>

#include "Base.h"
#include "Queue.h"
namespace Vulkan {
    using QueuesSpecification = std::unordered_map<Queue::Family, uint32_t>;

    struct QueueFamilyIndices {
    public:
        std::optional<uint32_t>      GetFamilyIndex(Queue::Family family) const;
        std::unordered_set<uint32_t> GetUniqueIndicies() const;

        void AddMapping(Queue::Family family, uint32_t index);

        const std::unordered_map<Queue::Family, uint32_t>& GetFamilies() const { return m_Families; }

    private:
        std::unordered_map<Queue::Family, uint32_t> m_Families;
    };
}  // namespace Vulkan

namespace Vulkan::Helpers {

    std::vector<const char*> GetRequiredLayers();
    std::vector<const char*> GetRequiredInstanceExtensions();
    std::vector<const char*> GetRequiredDeviceExtensions();

    void CheckLayersSupport();
    void CheckInstanceExtensionSupport();
    bool CheckDevice(VkInstance instance, VkPhysicalDevice device, const QueuesSpecification& specification);

    QueueFamilyIndices GetDeviceQueueFamilies(VkInstance instance, VkPhysicalDevice device);

}  // namespace Vulkan::Helpers