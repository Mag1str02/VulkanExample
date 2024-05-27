#pragma once

#include "SwapChainNodesState.h"

#include "Engine/Vulkan/RenderGraph/Interface/Pass.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassCluster.h"

#include "Engine/Vulkan/RenderGraph/PassNode.h"

namespace Engine::Vulkan::RenderGraph {
    class SwapChainPresentPassNode : public PassNode {
    public:
        SwapChainPresentPassNode(Ref<SwapChainNodesState> state);

        virtual std::optional<std::string> Validate() const override;

    protected:
        virtual Scope<IPass>        CreatePass() override;
        virtual Scope<IPassCluster> CreatePassCluster() override;

    private:
        class Pass : public IPass {
        public:
            virtual void Prepare() override;

            void SetWaitSemaphore(Ref<IBinarySemaphore> semaphore);

            VkSwapchainKHR        GetSwapChainHandle();
            Ref<SwapChain::Image> GetPresentImage();
            VkSemaphore           GetWaitSemaphore();

        private:
            Pass(Ref<SwapChainNodesState> state);

        private:
            friend SwapChainPresentPassNode;

            Ref<IBinarySemaphore>               m_WaitSemaphore;
            Ref<SwapChainNodesState>            m_State;
            Ref<SwapChainNodesState::Iteration> m_Iteration;
        };

        class Cluster : public IPassCluster {
        public:
            Cluster() = default;

        protected:
            virtual void Submit(Executor* executor) override;
            virtual bool AddPass(IPass* pass) override;

            virtual void AddWaitSemaphore(Ref<IBinarySemaphore> semaphore) override;
            virtual void AddSignalSemaphore(Ref<IBinarySemaphore> semaphore) override;
            virtual bool IsCompleted() const override;

        private:
            SwapChainPresentPassNode::Pass* m_PresentPass = nullptr;
        };

    private:
        Ref<SwapChainNodesState> m_State;
    };
}  // namespace Engine::Vulkan::RenderGraph