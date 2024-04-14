#pragma once

#include <volk.h>

#include "Vulkan/Base.h"

#define VK_CHECK(...)                               \
    {                                               \
        auto res = __VA_ARGS__;                     \
        DE_ASSERT(res == VK_SUCCESS, #__VA_ARGS__); \
    }

class Window;

namespace Vulkan {
    class ShaderModule;
    class Device;
    class Instance;
    class Queue;
    class Debugger;
    class Pipeline;
    class Renderer;
    class CommandPool;
    class CommandBuffer;
    class Image;
    class Buffer;
    class ImageSampler;
    class ImageView;
    class Window;

    enum class QueueFamily {
        Unknown = 0,
        Graphics,
        Presentation,
    };

    enum class ShaderStage {
        None = 0,
        Vertex,
        Fragment,
    };

    using ShaderStages        = std::unordered_map<ShaderStage, Ref<ShaderModule>>;
    using QueuesSpecification = std::unordered_map<QueueFamily, uint32_t>;

    struct AttachmentSpecification {
        std::vector<VkFormat> m_Colors;
    };

    struct PipelineSpecification {
        ShaderStages            m_Stages;
        AttachmentSpecification m_Attachments;
    };

    struct QueueFamilyIndices {
    public:
        std::optional<uint32_t>      GetFamilyIndex(QueueFamily family) const;
        std::unordered_set<uint32_t> GetUniqueIndicies() const;

        void AddMapping(QueueFamily family, uint32_t index);

        const std::unordered_map<QueueFamily, uint32_t>& GetFamilies() const {
            return m_Families;
        }

    private:
        std::unordered_map<QueueFamily, uint32_t> m_Families;
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR        m_Capabilities;
        std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
        std::vector<VkPresentModeKHR>   m_PresentationModes;
    };
}  // namespace Vulkan