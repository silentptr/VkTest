#include "VkTest/App.h"

namespace VkTest
{
    const std::vector<const char*> App::m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    App::App() : m_Window(m_AppSession), m_GraphicsDevice(m_AppSession, m_Window, m_DeviceExtensions), m_SwapChain(m_Window, m_GraphicsDevice), m_Pipeline(m_GraphicsDevice, m_SwapChain),
    m_Framebuffer(m_GraphicsDevice, m_SwapChain, m_Pipeline), m_CommandBuffer(m_GraphicsDevice),
    m_ImgAvailSemaphore(VK_NULL_HANDLE), m_RenderDoneSemaphore(VK_NULL_HANDLE), m_InFlightFence(VK_NULL_HANDLE)
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (vkCreateSemaphore(m_GraphicsDevice.GetVkDevice(), &semaphoreInfo, nullptr, &m_ImgAvailSemaphore) != VK_SUCCESS ||
            vkCreateSemaphore(m_GraphicsDevice.GetVkDevice(), &semaphoreInfo, nullptr, &m_RenderDoneSemaphore) != VK_SUCCESS ||
            vkCreateFence(m_GraphicsDevice.GetVkDevice(), &fenceInfo, nullptr, &m_InFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create synchronization objects");
        }

        m_Window.Show();
        Loop();
    }

    App::~App() noexcept
    {
        if (m_ImgAvailSemaphore != VK_NULL_HANDLE) { vkDestroySemaphore(m_GraphicsDevice.GetVkDevice(), m_ImgAvailSemaphore, nullptr); }
        if (m_RenderDoneSemaphore != VK_NULL_HANDLE) { vkDestroySemaphore(m_GraphicsDevice.GetVkDevice(), m_RenderDoneSemaphore, nullptr); }
        if (m_InFlightFence != VK_NULL_HANDLE) { vkDestroyFence(m_GraphicsDevice.GetVkDevice(), m_InFlightFence, nullptr); }
    }

    void App::Loop()
    {
        while (!glfwWindowShouldClose(m_Window.GetHandle()))
        {
            glfwPollEvents();
            Draw();
        }

        vkDeviceWaitIdle(m_GraphicsDevice.GetVkDevice());
    }

    void App::Draw()
    {
        vkWaitForFences(m_GraphicsDevice.GetVkDevice(), 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(m_GraphicsDevice.GetVkDevice(), 1, &m_InFlightFence);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_GraphicsDevice.GetVkDevice(), m_SwapChain.GetVkSwapChain(), UINT64_MAX, m_ImgAvailSemaphore, VK_NULL_HANDLE, &imageIndex);

        m_CommandBuffer.ResetBuffer();
        m_CommandBuffer.BeginRenderPass(m_SwapChain, m_Framebuffer, m_Pipeline, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_ImgAvailSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffer.GetVkCommandBuffer();

        VkSemaphore signalSemaphores[] = {m_RenderDoneSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_GraphicsDevice.GetVkGraphicsQueue(), 1, &submitInfo, m_InFlightFence) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_SwapChain.GetVkSwapChain()};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(m_GraphicsDevice.GetVkPresentQueue(), &presentInfo);
    }
}