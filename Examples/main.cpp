#include <Vulkan/Application.h>
#include <Vulkan/Helpers.h>
#include <Vulkan/ShaderModule.h>
#include <Vulkan/Utils/Assert.h>
#include <Vulkan/VulkanWindow.h>
#include <Vulkan/Window.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <thread>

#include "shaders_generated.h"

using namespace Vulkan;

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice device) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    DE_ASSERT(false, "Failed to find suitable memory type!");
    return 0;
}
class Buffer {
public:
    Buffer(Ref<Device> device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) :
        m_Device(device), m_Size(size), m_MemoryProperties(properties) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size        = size;
        bufferInfo.usage       = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK(vkCreateBuffer(device->GetLogicDevice(), &bufferInfo, nullptr, &m_Buffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device->GetLogicDevice(), m_Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, m_Device->GetPhysicalDevice());

        VK_CHECK(vkAllocateMemory(m_Device->GetLogicDevice(), &allocInfo, nullptr, &m_Memory));
        VK_CHECK(vkBindBufferMemory(m_Device->GetLogicDevice(), m_Buffer, m_Memory, 0))
    }
    ~Buffer() {
        vkFreeMemory(m_Device->GetLogicDevice(), m_Memory, nullptr);
        vkDestroyBuffer(m_Device->GetLogicDevice(), m_Buffer, nullptr);
    }

    void SetData(void* data, uint64_t size) {
        DE_ASSERT(m_MemoryProperties | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                  "Cannot set data on buffer without VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT memory property");
        DE_ASSERT(size <= m_Size, "Set data size exceeds the size of buffer");

        void* mapped_memory;
        vkMapMemory(m_Device->GetLogicDevice(), m_Memory, 0, size, 0, &mapped_memory);
        memcpy(mapped_memory, data, size);
        vkUnmapMemory(m_Device->GetLogicDevice(), m_Memory);
    }

    VkBuffer Handle() {
        return m_Buffer;
    }

private:
    Ref<Device>           m_Device;
    VkBuffer              m_Buffer;
    VkDeviceMemory        m_Memory;
    uint64_t              m_Size;
    VkMemoryPropertyFlags m_MemoryProperties;
};

class Image {
public:
    Image(Ref<Device> device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkMemoryPropertyFlags properties) :
        m_Device(device), m_Width(width), m_Height(height) {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType     = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width  = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth  = 1;
        imageInfo.mipLevels     = 1;
        imageInfo.arrayLayers   = 1;
        imageInfo.format        = format;
        imageInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage         = usage;
        imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;

        VK_CHECK(vkCreateImage(m_Device->GetLogicDevice(), &imageInfo, nullptr, &m_Image));

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_Device->GetLogicDevice(), m_Image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize  = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties, m_Device->GetPhysicalDevice());

        VK_CHECK(vkAllocateMemory(m_Device->GetLogicDevice(), &allocInfo, nullptr, &m_Memory));
        VK_CHECK(vkBindImageMemory(m_Device->GetLogicDevice(), m_Image, m_Memory, 0));
    }
    ~Image() {
        vkFreeMemory(m_Device->GetLogicDevice(), m_Memory, nullptr);
        vkDestroyImage(m_Device->GetLogicDevice(), m_Image, nullptr);
    }

    VkImage Handle() {
        return m_Image;
    }
    Ref<Device> GetDevice() {
        return m_Device;
    }

private:
    Ref<Device>    m_Device;
    uint32_t       m_Width;
    uint32_t       m_Height;
    VkImage        m_Image;
    VkDeviceMemory m_Memory;
};

class ImageView {
public:
    ImageView(Ref<Image> image) : m_Image(image) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image                           = m_Image->Handle();
        viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format                          = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel   = 0;
        viewInfo.subresourceRange.levelCount     = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount     = 1;

        VK_CHECK(vkCreateImageView(m_Image->GetDevice()->GetLogicDevice(), &viewInfo, nullptr, &m_View));
    }
    ~ImageView() {
        vkDestroyImageView(m_Image->GetDevice()->GetLogicDevice(), m_View, nullptr);
    }

    VkImageView Handle() {
        return m_View;
    }

private:
    Ref<Image>  m_Image;
    VkImageView m_View;
};

class ImageSampler {
public:
    ImageSampler(Ref<Device> device) : m_Device(device) {
        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType        = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter    = VK_FILTER_NEAREST;
        samplerInfo.minFilter    = VK_FILTER_NEAREST;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.borderColor  = VK_BORDER_COLOR_INT_OPAQUE_BLACK;

        samplerInfo.anisotropyEnable        = VK_FALSE;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable           = VK_FALSE;
        samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias              = 0.0f;
        samplerInfo.minLod                  = 0.0f;
        samplerInfo.maxLod                  = 0.0f;

        VK_CHECK(vkCreateSampler(m_Device->GetLogicDevice(), &samplerInfo, nullptr, &m_Sampler));
    }
    ~ImageSampler() {
        vkDestroySampler(m_Device->GetLogicDevice(), m_Sampler, nullptr);
    }

    VkSampler Handle() {
        return m_Sampler;
    }

private:
    Ref<Device> m_Device;
    VkSampler   m_Sampler;
};

class TestApplication : public Application {
public:
    TestApplication() = default;

    virtual void OnStartUp() override {
        m_CommandPool = m_Renderer->GetDevice()->CreateCommandPool(QueueFamily::Graphics);
        CreateTexture();
    }
    virtual void OnShutDown() override {
        vkDeviceWaitIdle(m_Renderer->GetLogicDevice());
    }
    virtual void OnLoop() override {
        OnImGui();
    }

private:
    void CreateTexture() {
        // Create iamges
        {
            uint8_t image[] = {255, 0, 0, 255, 0, 255, 0, 255};

            m_StagingBuffer = CreateRef<Buffer>(m_Renderer->GetDevice(),
                                                sizeof(image),
                                                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            m_StagingBuffer->SetData(image, sizeof(image));

            m_Image        = CreateRef<Image>(m_Renderer->GetDevice(),
                                       2,
                                       1,
                                       VK_FORMAT_R8G8B8A8_SRGB,
                                       VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            m_ImageSampler = CreateRef<ImageSampler>(m_Renderer->GetDevice());
            m_ImageView    = CreateRef<ImageView>(m_Image);
        }

        // Transfer texture data
        {
            auto queue = m_Renderer->GetGraphicsQueue();
            auto cmd   = m_CommandPool->CreateCommandBuffer();

            cmd->Begin();
            // First image layout transition
            {
                VkImageMemoryBarrier2 barrier{};
                barrier.image                           = m_Image->Handle();
                barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
                barrier.srcStageMask                    = 0;
                barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                barrier.srcAccessMask                   = 0;
                barrier.dstAccessMask                   = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                barrier.oldLayout                       = VK_IMAGE_LAYOUT_UNDEFINED;
                barrier.newLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.baseMipLevel   = 0;
                barrier.subresourceRange.layerCount     = 1;
                barrier.subresourceRange.levelCount     = 1;

                VkDependencyInfo dependencyInfo{};
                dependencyInfo.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
                dependencyInfo.imageMemoryBarrierCount = 1;
                dependencyInfo.pImageMemoryBarriers    = &barrier;

                vkCmdPipelineBarrier2(cmd->Handle(), &dependencyInfo);
            }
            // Transfer image data from staging buffer to image
            {
                VkBufferImageCopy region{};
                region.bufferOffset      = 0;
                region.bufferRowLength   = 0;
                region.bufferImageHeight = 0;

                region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                region.imageSubresource.mipLevel       = 0;
                region.imageSubresource.baseArrayLayer = 0;
                region.imageSubresource.layerCount     = 1;

                region.imageOffset = {0, 0, 0};
                region.imageExtent = {2, 1, 1};
                vkCmdCopyBufferToImage(cmd->Handle(), m_StagingBuffer->Handle(), m_Image->Handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
            }
            // Second image layout transition
            {
                VkImageMemoryBarrier2 barrier{};
                barrier.image                           = m_Image->Handle();
                barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
                barrier.srcStageMask                    = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                barrier.dstStageMask                    = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
                barrier.srcAccessMask                   = VK_ACCESS_2_TRANSFER_WRITE_BIT;
                barrier.dstAccessMask                   = VK_ACCESS_2_SHADER_READ_BIT;
                barrier.oldLayout                       = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                barrier.newLayout                       = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                barrier.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
                barrier.subresourceRange.baseArrayLayer = 0;
                barrier.subresourceRange.baseMipLevel   = 0;
                barrier.subresourceRange.layerCount     = 1;
                barrier.subresourceRange.levelCount     = 1;

                VkDependencyInfo dependencyInfo{};
                dependencyInfo.sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
                dependencyInfo.imageMemoryBarrierCount = 1;
                dependencyInfo.pImageMemoryBarriers    = &barrier;

                vkCmdPipelineBarrier2(cmd->Handle(), &dependencyInfo);
            }
            cmd->End();

            VkSubmitInfo submitInfo{};
            submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers    = &cmd->Handle();

            vkQueueSubmit(queue->Handle(), 1, &submitInfo, VK_NULL_HANDLE);
            vkQueueWaitIdle(queue->Handle());
        }

        m_ImGuiTextureID = ImGui_ImplVulkan_AddTexture(m_ImageSampler->Handle(), m_ImageView->Handle(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    void OnImGui() {
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin("DockSpace",
                         NULL,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar |
                             ImGuiWindowFlags_NoDocking);
            ImGui::PopStyleVar(3);

            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
            ImGui::End();
        }

        ImGui::ShowDemoWindow();

        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Begin("Hello, world!");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::Image(m_ImGuiTextureID, {200, 100});
            ImGui::End();
        }
    }

    Ref<Buffer>       m_StagingBuffer;
    Ref<Image>        m_Image;
    Ref<CommandPool>  m_CommandPool;
    Ref<ImageView>    m_ImageView;
    Ref<ImageSampler> m_ImageSampler;
    VkDescriptorSet   m_ImGuiTextureID;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

int main() {
    {
        TestApplication app;
        app.Run();
    }
    {
        TestApplication app;
        app.Run();
    }
    return 0;
}