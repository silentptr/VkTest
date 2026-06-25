#ifndef VKTEST_COMMANDBUFFER_H_
#define VKTEST_COMMANDBUFFER_H_

#include <cstdint>
#include <iostream>

#include "VkTest/IncludeVolk.h"
#include "VkTest/GraphicsDevice.h"
#include "VkTest/Pipeline.h"
#include "VkTest/Framebuffer.h"

namespace VkTest
{
    class CommandBuffer
    {
    private:
        const GraphicsDevice& m_GraphicsDevice;

        VkCommandPool m_CommandPool;
        VkCommandBuffer m_CommandBuffer;

        void Cleanup() noexcept;
    public:
        CommandBuffer(const GraphicsDevice&);
        CommandBuffer(CommandBuffer&& cb) noexcept : m_GraphicsDevice(cb.m_GraphicsDevice), m_CommandPool(cb.m_CommandPool), m_CommandBuffer(cb.m_CommandBuffer)
        {
            cb.m_CommandPool = VK_NULL_HANDLE;
        }
        CommandBuffer(const CommandBuffer&) = delete;
        ~CommandBuffer() noexcept { Cleanup(); }

        CommandBuffer& operator=(CommandBuffer&& cb) noexcept
        {
            //m_GraphicsDevice = cb.m_GraphicsDevice;
            m_CommandPool = cb.m_CommandPool;
            m_CommandBuffer = cb.m_CommandBuffer;
            cb.m_CommandPool = VK_NULL_HANDLE;
            return *this;
        }
        CommandBuffer& operator=(const CommandBuffer&) = delete;

        const VkCommandBuffer& GetVkCommandBuffer() const noexcept { return m_CommandBuffer; }

        void ResetBuffer() noexcept { vkResetCommandBuffer(m_CommandBuffer, /*VkCommandBufferResetFlagBits*/ 0); }
        void BeginRenderPass(const SwapChain&, const Framebuffer&, const Pipeline&, uint32_t);
    };
}

#endif