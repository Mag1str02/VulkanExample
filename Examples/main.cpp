#include <Engine/Application/Application.h>
#include <Engine/Vulkan/Renderer/Renderer.h>

#include <Engine/Vulkan/CommandPool.h>
#include <Engine/Vulkan/Concrete/CommandBuffer.h>
#include <Engine/Vulkan/Concrete/Fence.h>
#include <Engine/Vulkan/Concrete/SmartCommandBuffer.h>
#include <Engine/Vulkan/ImageView.h>

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

// #include "shaders_generated.h"

using namespace Engine;

class TestApplication : public Engine::Application {
public:
    TestApplication() = default;

    virtual void OnStartUp() override {
        auto command_pool = Vulkan::CommandPool::Create(m_Renderer->GetDevice(), m_Renderer->GetDevice()->GetGraphicsQueue()->FamilyIndex());
        m_CommandBuffer   = Vulkan::Concrete::SmartCommandBuffer::Create(command_pool);
    }
    virtual void OnShutDown() override {
        vkDeviceWaitIdle(m_Renderer->GetDevice()->GetLogicDevice());
    }
    virtual void OnLoop() override {
        PROFILER_SCOPE("TestApplication::OnLoop");
        // auto image      = m_Window->GetSwapChainImage();
        // auto image_view = CreateRef<Vulkan::ImageView>(image);
        // m_CommandBuffer->Begin();
        // {
        //     TracyVkZone(m_Renderer->GetDevice()->GetTracyCtx(), m_CommandBuffer->Handle(), "SwapChainClear");
        //     float time = glfwGetTime();
        //     Vec4  clearValue;
        //     clearValue.r = (cos(time) + 1) / 2;
        //     clearValue.g = (sin(time) + 1) / 2;
        //     clearValue.b = 0;
        //     clearValue.a = 1;

        //     m_CommandBuffer->ClearImage(image, clearValue);
        //     m_CommandBuffer->BeginRendering({image_view});
        //     m_CommandBuffer->EndRendering();
        // }
        // m_CommandBuffer->End();

        // {
        //     PROFILER_SCOPE("TestApplication::OnLoop (Submit)");
        //     VkCommandBuffer handle = m_CommandBuffer->Handle();

        //     VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        //     VkSubmitInfo         info       = {};
        //     info.sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        //     info.waitSemaphoreCount         = 0;
        //     info.pWaitSemaphores            = nullptr;
        //     info.pWaitDstStageMask          = &wait_stage;
        //     info.commandBufferCount         = 1;
        //     info.pCommandBuffers            = &handle;
        //     info.signalSemaphoreCount       = 0;
        //     info.pSignalSemaphores          = nullptr;
        //     Vulkan::Concrete::Fence fence(m_Renderer->GetDevice());

        //     VK_CHECK(vkQueueSubmit(m_Renderer->GetQueue()->Handle(), 1, &info, fence.Handle()));
        //     fence.Wait();
        // }
    }

private:
    Ref<Vulkan::Concrete::SmartCommandBuffer> m_CommandBuffer;
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