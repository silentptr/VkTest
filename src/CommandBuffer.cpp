#include "VkTest/CommandBuffer.h"

namespace VkTest
{
    CommandBuffer::CommandBuffer(const GraphicsDevice& gd) : m_GraphicsDevice(gd),
    m_CommandPool(VK_NULL_HANDLE), m_CommandBuffer(VK_NULL_HANDLE)
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_GraphicsDevice.GetGPU()->GetGraphicsQueueIndex();

        if (vkCreateCommandPool(m_GraphicsDevice.GetVkDevice(), &poolInfo, NULL, &m_CommandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool");
        }

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        if (vkAllocateCommandBuffers(m_GraphicsDevice.GetVkDevice(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
        {
            Cleanup();
            throw std::runtime_error("failed to allocate command buffers");
        }

        std::cout << "Created the command buffer.\n";
    }

    void CommandBuffer::Cleanup() noexcept
    {
        if (m_CommandPool != VK_NULL_HANDLE)
        {
            vkDestroyCommandPool(m_GraphicsDevice.GetVkDevice(), m_CommandPool, nullptr);
        }
    }

    void CommandBuffer::BeginRenderPass(const SwapChain& swapChain, const Framebuffer& framebuffer, const Pipeline& pipeline, uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = pipeline.GetVkRenderPass();
        renderPassInfo.framebuffer = framebuffer.GetVkFramebuffers()[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain.GetExtent();

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(m_CommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetVkPipeline());

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain.GetExtent().width);
        viewport.height = static_cast<float>(swapChain.GetExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(m_CommandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChain.GetExtent();
        vkCmdSetScissor(m_CommandBuffer, 0, 1, &scissor);            

        vkCmdDraw(m_CommandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(m_CommandBuffer);

        if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer");
        }
    }
}