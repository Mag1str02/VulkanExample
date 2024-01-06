#pragma once

#include <vulkan/vulkan.h>

#include "Common.h"

namespace Vulkan {

    class Pipeline {
    public:
        ~Pipeline();

        VkPipeline Handle();

        NO_COPY_CONSTRUCTORS(Pipeline);
        NO_MOVE_CONSTRUCTORS(Pipeline);

    private:
        friend class Device;

        Pipeline(Ref<Device> device, const PipelineSpecification& spec);

        PipelineSpecification m_Specification;
        VkPipelineLayout      m_PipelineLayout = VK_NULL_HANDLE;
        VkPipeline            m_Pipeline       = VK_NULL_HANDLE;
        Ref<Device>           m_Device         = nullptr;
    };
}  // namespace Vulkan