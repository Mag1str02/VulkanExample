#include <Engine/Application/Application.h>
#include <Engine/Vulkan/Renderer/Renderer.h>

#include <Engine/Vulkan/CommandBuffer.h>
#include <Engine/Vulkan/CommandPool.h>
#include <Engine/Vulkan/Fence.h>
#include <Engine/Vulkan/ImageView.h>
#include <Engine/Vulkan/Interface/Image.h>
#include <Engine/Vulkan/RenderGraph/CommandBufferPassNode.h>
#include <Engine/Vulkan/RenderGraph/Interface/ImageResourceNode.h>

#include <GLFW/glfw3.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

// #include "shaders_generated.h"

using namespace Engine;

class ClearColorPassNode : public Vulkan::RenderGraph::CommandBufferPassNode {
public:
    ClearColorPassNode(Ref<Vulkan::Device> device) : Vulkan::RenderGraph::CommandBufferPassNode(std::move(device)) {}

private:
    class Pass : public Vulkan::RenderGraph::CommandBufferPassNode::Pass {
    public:
        Pass(ClearColorPassNode* node, Vulkan::RenderGraph::IImageResourceNode* image_node) :
            Vulkan::RenderGraph::CommandBufferPassNode::Pass(node), m_ImageNode(image_node) {}

        virtual void Record(Vulkan::CommandBuffer& buffer) {
            float time = glfwGetTime();
            Vec4  clearValue;
            clearValue.r = (cos(time) + 1) / 2;
            clearValue.g = (sin(time) + 1) / 2;
            clearValue.b = 0;
            clearValue.a = 1;
            buffer.RequestImageAccess(m_ImageNode->GetImage(), {}, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
            buffer.ClearImage(m_ImageNode->GetImage(), clearValue);
            buffer.RequestImageAccess(m_ImageNode->GetImage(), {}, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        }

    private:
        Vulkan::RenderGraph::IImageResourceNode* m_ImageNode;
    };

protected:
    virtual Scope<Vulkan::RenderGraph::IPass> CreatePass() override {
        auto* resource_node = GetExternalResource("SwapChainAquireImage", Vulkan::RenderGraph::DependencyType::ReadWriteInput);
        DE_ASSERT(resource_node->Is<Vulkan::RenderGraph::IImageResourceNode>(), "Bad input resource node");
        return CreateScope<Pass>(this, resource_node->As<Vulkan::RenderGraph::IImageResourceNode>());
    }
};

class TestApplication : public Engine::Application {
public:
    TestApplication() = default;

    virtual void OnStartUp() override {
        auto* graph = m_Window->GetRenderGraph();
        auto* pass  = graph->CreateEnrty<ClearColorPassNode>(m_Renderer->GetDevice());
        graph->CreateDependency(pass, "SwapChainAquireImage", Vulkan::RenderGraph::DependencyType::ReadWriteInput);
        graph->CreateDependency(pass, "SwapChainPresentImage", Vulkan::RenderGraph::DependencyType::Output);
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
        //     Vulkan::Fence fence(m_Renderer->GetDevice());

        //     VK_CHECK(vkQueueSubmit(m_Renderer->GetQueue()->Handle(), 1, &info, fence.Handle()));
        //     fence.Wait();
        // }
    }

private:
    Ref<Vulkan::CommandBuffer> m_CommandBuffer;
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