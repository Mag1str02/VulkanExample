#pragma once

#include "SwapChainNodesState.h"

#include "Engine/Vulkan/RenderGraph/Interface/Pass.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassCluster.h"
#include "Engine/Vulkan/RenderGraph/PassNode.h"

namespace Engine::Vulkan::RenderGraph {

    class SwapChainAquirePassNode : public PassNode {
    public:
        SwapChainAquirePassNode(Ref<SwapChainNodesState> state);

        virtual std::optional<std::string> Validate() const override;

    protected:
        virtual Scope<IPass>        CreatePass() override;
        virtual Scope<IPassCluster> CreatePassCluster() override;

    private:
        class Pass : public IPass {
        public:
            void SetSignalSemaphore(Ref<IBinarySemaphore> semaphore);

        protected:
            virtual void Prepare() override;

        private:
            Pass(Ref<SwapChainNodesState> state);

        private:
            friend SwapChainAquirePassNode;

            Ref<IBinarySemaphore>               m_SignalSemaphore;
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
            SwapChainAquirePassNode::Pass* m_AquirePass = nullptr;
        };

    private:
        Ref<SwapChainNodesState> m_State;
    };
}  // namespace Engine::Vulkan::RenderGraph