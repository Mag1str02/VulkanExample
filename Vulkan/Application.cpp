#include "Application.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <unordered_set>
#include <vector>

#include "Debugger.h"
#include "Helpers.h"
#include "Instance.h"
#include "Utils/Assert.h"
#include "Window.h"

namespace {}  // namespace

Application::Application() {
    InitGLFW();
    InitVulkan();
    OnStartUp();
}
Application::~Application() {
    OnShutDown();
    TerminateVulkan();
    TerminateGLFW();
}
void Application::Run() {
    OnStartUp();
    while (!m_Window->ShouldClose()) {
        Loop();
    }
    OnShutDown();
}

void Application::Loop() {
    glfwPollEvents();
    OnLoop();
}

void Application::InitGLFW() {
    glfwInit();

    m_Window = Window::Create();
}
void Application::TerminateGLFW() {
    m_Window = nullptr;
    glfwTerminate();
}

void Application::InitVulkan() {
    m_Renderer  = CreateRef<Vulkan::Renderer>();
    m_SwapChain = Vulkan::SwapChain::Create(m_Window, m_Renderer);

    // Get image views
    {
        m_SwapChainImageViews.resize(m_SwapChain->GetImageCount());
        for (size_t i = 0; i < m_SwapChain->GetImageCount(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image                           = m_SwapChain->GetImage(i);
            createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format                          = m_SwapChain->GetFormat();
            createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;

            auto res = vkCreateImageView(m_Renderer->GetLogicDevice(), &createInfo, nullptr, &m_SwapChainImageViews[i]);
            DE_ASSERT(res == VK_SUCCESS, "Failed to create image view");
        }
    }
}
void Application::TerminateVulkan() {
    for (auto imageView : m_SwapChainImageViews) {
        vkDestroyImageView(m_Renderer->GetLogicDevice(), imageView, nullptr);
    }
    m_Renderer = nullptr;
}