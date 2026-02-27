#ifndef VKTEST_FRAMEBUFFER_H_
#define VKTEST_FRAMEBUFFER_H_

#include <vector>
#include <iostream>

#include "VkTest/IncludeVolk.h"
#include "VkTest/GraphicsDevice.h"
#include "VkTest/SwapChain.h"
#include "VkTest/Pipeline.h"

namespace VkTest
{
    class Framebuffer
    {
    private:
        const GraphicsDevice& m_GraphicsDevice;

        std::vector<VkFramebuffer> m_Framebuffers;

        void Cleanup() noexcept;
    public:
        Framebuffer(const GraphicsDevice&, const SwapChain&, const Pipeline&);
        ~Framebuffer() noexcept { Cleanup(); }

        const std::vector<VkFramebuffer>& GetVkFramebuffers() const noexcept { return m_Framebuffers; }
    };
}

#endif