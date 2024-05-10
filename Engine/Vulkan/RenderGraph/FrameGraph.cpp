#include "FrameGraph.h"

#include "ResourceNode.h"

#include "Engine/Vulkan/Window/SwapChain.h"

namespace Engine::Vulkan::RenderGraph {

    namespace {
        class SwapChainImageResourceState {
        public:
            SwapChainImageResourceState(Ref<Surface>& surface) : m_Surface(surface) {
                m_SwapChain = SwapChain::Create(m_Surface);
            }

            void Instantiate() {
                if (!m_Instantiated) {
                    m_Instantiated = true;

                    if (m_SubOptimal) {
                        m_SubOptimal = false;
                        RecreateSwapChain();
                    }
                    while (true) {
                        auto [result, image] = m_SwapChain->AcquireImage(VK_NULL_HANDLE, VK_NULL_HANDLE);
                        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                            RecreateSwapChain();
                        } else {
                            if (result == VK_SUBOPTIMAL_KHR) {
                                m_SubOptimal = true;
                            }
                            m_CurrentImage = std::move(image);
                            break;
                        }
                    }
                }
            }
            void Claim() {
                m_Instantiated = false;
                m_CurrentImage = nullptr;
            }

            Ref<IImage> GetImage();

        private:
            void RecreateSwapChain() {
                m_SwapChain = SwapChain::Create(m_Surface, m_SwapChain);
            }

        private:
            Ref<Surface>   m_Surface;
            Ref<SwapChain> m_SwapChain;
            Ref<IImage>    m_CurrentImage;
            bool           m_Instantiated = false;
            bool           m_SubOptimal   = false;
        };

        class SwapChainImageResource : public ResourceNode {
        public:
            SwapChainImageResource(Ref<SwapChainImageResourceState> state, const std::string& name) : m_Name(name) {}

            virtual const std::string& GetName() const override {
                return m_Name;
            }

            virtual void Instantiate() override {
                m_State->Instantiate();
            };
            virtual void Claim() override {
                m_State->Claim();
            };

        private:
            std::string m_Name;

            Ref<SwapChainImageResourceState> m_State;
        };

        class InternalFrameGraph : public RenderGraph {
        public:
            virtual const std::string& GetName() const override {
                static const std::string name = "InternalFrameGraph";
                return name;
            }

            void SetInputNode(SwapChainImageResource* node) {
                AddReadWriteInput(*node);
            }
            void SetOutputNode(SwapChainImageResource* node) {
                AddOutput(*node);
            }
        };

    }  // namespace

    FrameGraph::FrameGraph(Ref<Surface> surface) {
        auto shared_state = CreateRef<SwapChainImageResourceState>(surface);

        auto* input_node  = CreateEnrty<SwapChainImageResource>(shared_state, "SwapChainInputImage");
        auto* output_node = CreateEnrty<SwapChainImageResource>(shared_state, "SwapChainOutputImage");

        auto* internal_graph = CreateEnrty<InternalFrameGraph>();
        internal_graph->SetInputNode(input_node);
        internal_graph->SetOutputNode(output_node);

        m_InternalGraph = internal_graph;
    }

    const std::string& FrameGraph::GetName() const {
        static const std::string name = "FrameGraph";
        return name;
    }

    RenderGraph* FrameGraph::GetInternalGraph() const {
        return m_InternalGraph;
    }

}  // namespace Engine::Vulkan::RenderGraph