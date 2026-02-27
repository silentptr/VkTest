#ifndef VKTEST_PIPELINE_H_
#define VKTEST_PIPELINE_H_

#include <fstream>
#include <iostream>

#include "VkTest/IncludeVolk.h"
#include "VkTest/GraphicsDevice.h"
#include "VkTest/SwapChain.h"

namespace VkTest
{
    class Pipeline
    {
    private:
        const GraphicsDevice& m_GraphicsDevice;

        VkRenderPass m_RenderPass;
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;

        void Cleanup() noexcept;
    public:
        Pipeline(const GraphicsDevice&, const SwapChain&);
        ~Pipeline() noexcept { Cleanup(); }

        const VkRenderPass& GetVkRenderPass() const noexcept { return m_RenderPass; }
        const VkPipeline& GetVkPipeline() const noexcept { return m_Pipeline; }
    };
}

#endif