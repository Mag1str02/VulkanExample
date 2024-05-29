#pragma once

#include "PassNode.h"

#include "Engine/Vulkan/RenderGraph/Interface/Pass.h"
#include "Engine/Vulkan/RenderGraph/Interface/PassCluster.h"

namespace Engine::Vulkan::RenderGraph {

    class CommandBufferPassNode : public PassNode {
    public:
        CommandBufferPassNode(Ref<Device> device);

        virtual std::optional<std::string> Validate() const override;

    protected:
        virtual Scope<IPass> CreatePass() override;

    private:
        virtual Scope<IPassCluster> CreatePassCluster() final override;

    protected:
        class Pass;

    private:
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
            std::vector<Pass*> m_Passes;

            std::vector<Ref<IBinarySemaphore>> m_WaitSemaphores;
            std::vector<Ref<IBinarySemaphore>> m_SignalSemaphores;

            Ref<IFence>            m_Fence;
            Ref<IRawCommandBuffer> m_PatchCommandBuffer;
        };

    protected:
        class Pass : public IPass {
        public:
            Pass(CommandBufferPassNode* node);

            virtual void Record(CommandBuffer& command_buffer);

        private:
            virtual void Prepare() override;

        private:
            friend class Cluster;
            Ref<CommandBuffer> m_CommandBuffer;
        };

    private:
        Ref<CommandPool> m_CommandPool;
    };
};  // namespace Engine::Vulkan::RenderGraph