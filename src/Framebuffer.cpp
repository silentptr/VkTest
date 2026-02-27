#include "VkTest/Framebuffer.h"

namespace VkTest
{
    Framebuffer::Framebuffer(const GraphicsDevice& gd, const SwapChain& swapChain, const Pipeline& pipeline) : m_GraphicsDevice(gd)
    {
        m_Framebuffers.resize(swapChain.GetSwapChainImageViews().size(), VK_NULL_HANDLE);

        for (size_t i = 0; i < swapChain.GetSwapChainImageViews().size(); i++)
        {
            VkImageView attachments[] = {swapChain.GetSwapChainImageViews()[i]};

            VkFramebufferCreateInfo framebufferCreateInfo{};
            framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferCreateInfo.renderPass = pipeline.GetVkRenderPass();
            framebufferCreateInfo.attachmentCount = 1;
            framebufferCreateInfo.pAttachments = attachments;
            framebufferCreateInfo.width = swapChain.GetExtent().width;
            framebufferCreateInfo.height = swapChain.GetExtent().height;
            framebufferCreateInfo.layers = 1;

            if (vkCreateFramebuffer(m_GraphicsDevice.GetVkDevice(), &framebufferCreateInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
            {
                Cleanup();
                throw std::runtime_error("failed to create framebuffer");
            }
        }

        std::cout << "Framebuffer created.\n";
    }

    void Framebuffer::Cleanup() noexcept
    {
        for (const auto& framebuffer : m_Framebuffers)
        {
            if (framebuffer == VK_NULL_HANDLE) { continue; }
            vkDestroyFramebuffer(m_GraphicsDevice.GetVkDevice(), framebuffer, nullptr);
        }
    }
}