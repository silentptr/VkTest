#include "VkTest/App.h"

namespace VkTest
{
    const std::vector<const char*> App::m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    App::App() : m_Window(m_AppSession), m_GraphicsDevice(m_AppSession, m_Window, m_DeviceExtensions), m_SwapChain(m_Window, m_GraphicsDevice), m_Pipeline(m_GraphicsDevice, m_SwapChain),
    m_Framebuffer(m_GraphicsDevice, m_SwapChain, m_Pipeline)
    {
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        m_ImgAvailSemaphores.fill(VK_NULL_HANDLE);
        m_InFlightFences.fill(VK_NULL_HANDLE);
        m_CmdBuffers.reserve(m_InFlightCount);

        for (int i = 0; i < m_InFlightCount; ++i)
        {
            if (vkCreateSemaphore(m_GraphicsDevice.GetVkDevice(), &semaphoreInfo, nullptr, &m_ImgAvailSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_GraphicsDevice.GetVkDevice(), &fenceInfo, nullptr, &m_InFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects");
            }

            m_CmdBuffers.emplace_back(m_GraphicsDevice);
        }

        m_RenderDoneSemaphores.resize(m_SwapChain.GetImageCount(), VK_NULL_HANDLE);

        for (uint32_t i = 0; i < m_SwapChain.GetImageCount(); ++i)
        {
            if (vkCreateSemaphore(m_GraphicsDevice.GetVkDevice(), &semaphoreInfo, nullptr, &m_RenderDoneSemaphores[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects");
            }
        }

        m_Window.Show();
        Loop();
    }

    App::~App() noexcept
    {
        for (int i = 0; i < m_InFlightCount; ++i)
        {
            if (m_ImgAvailSemaphores[i] != VK_NULL_HANDLE) { vkDestroySemaphore(m_GraphicsDevice.GetVkDevice(), m_ImgAvailSemaphores[i], nullptr); }
            if (m_InFlightFences[i] != VK_NULL_HANDLE) { vkDestroyFence(m_GraphicsDevice.GetVkDevice(), m_InFlightFences[i], nullptr); }
        }

        for (auto& sem : m_RenderDoneSemaphores)
        {
            if (sem != VK_NULL_HANDLE) { vkDestroySemaphore(m_GraphicsDevice.GetVkDevice(), sem, nullptr); }
        }
    }

    void App::Loop()
    {
        double lastUpdate = glfwGetTime();
        double now, delta;
        double lastFpsCount = 0.0;
        unsigned int frames = 0, fps;

        while (!glfwWindowShouldClose(m_Window.GetHandle()))
        {
            glfwPollEvents();

            now = glfwGetTime();
            delta = now - lastUpdate;
            lastUpdate = now;
            lastFpsCount += delta;

            if (lastFpsCount >= 1.0)
            {
                fps = frames;
                frames = 0;
                lastFpsCount = 0.0;
                std::cout << "FPS: " << std::to_string(fps) << '\n';
            }

            Draw();
            ++frames;
        }

        vkDeviceWaitIdle(m_GraphicsDevice.GetVkDevice());
    }

    void App::Draw()
    {
        vkWaitForFences(m_GraphicsDevice.GetVkDevice(), 1, &m_InFlightFences[m_InFlightIndex], VK_TRUE, UINT64_MAX);
        vkResetFences(m_GraphicsDevice.GetVkDevice(), 1, &m_InFlightFences[m_InFlightIndex]);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_GraphicsDevice.GetVkDevice(), m_SwapChain.GetVkSwapChain(), UINT64_MAX, m_ImgAvailSemaphores[m_InFlightIndex], VK_NULL_HANDLE, &imageIndex);

        m_CmdBuffers[m_InFlightIndex].ResetBuffer();
        m_CmdBuffers[m_InFlightIndex].BeginRenderPass(m_SwapChain, m_Framebuffer, m_Pipeline, imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_ImgAvailSemaphores[m_InFlightIndex]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CmdBuffers[m_InFlightIndex].GetVkCommandBuffer();

        VkSemaphore signalSemaphores[] = {m_RenderDoneSemaphores[imageIndex]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_GraphicsDevice.GetVkGraphicsQueue(), 1, &submitInfo, m_InFlightFences[m_InFlightIndex]) != VK_SUCCESS)
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

        m_InFlightIndex = (m_InFlightIndex + 1) % m_InFlightCount;
    }
}