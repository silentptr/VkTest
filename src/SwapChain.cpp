#include "VkTest/SwapChain.h"

namespace VkTest
{
    SwapChain::SwapChain(const Window& window, const GraphicsDevice& gDev) : m_GraphicsDevice(gDev)
    {
        const auto& surfaceCapabilities = m_GraphicsDevice.GetGPU()->GetSurfaceCapabilities();

        if (surfaceCapabilities.currentExtent.width != 0xFFFFFFFF)
        {
            m_SwapChainExtent.width = surfaceCapabilities.currentExtent.width;
            m_SwapChainExtent.height = surfaceCapabilities.currentExtent.height;
        }
        else
        {
            int w, h;
            glfwGetFramebufferSize(window.GetHandle(), &w, &h);
            m_SwapChainExtent.width = std::clamp(static_cast<std::uint32_t>(w), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
            m_SwapChainExtent.height = std::clamp(static_cast<std::uint32_t>(h), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        }

        std::uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

        if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        {
            imageCount = surfaceCapabilities.maxImageCount;
        }

        const auto& surfaceFormat = m_GraphicsDevice.GetGPU()->GetSurfaceFormat();

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = window.GetSurface();
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = m_SwapChainExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        std::uint32_t queueFamilyIndices[] = {m_GraphicsDevice.GetGPU()->GetGraphicsQueueIndex(), m_GraphicsDevice.GetGPU()->GetPresentQueueIndex()};
        
        if (m_GraphicsDevice.GetGPU()->GetGraphicsQueueIndex() != m_GraphicsDevice.GetGPU()->GetPresentQueueIndex())
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = surfaceCapabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = m_GraphicsDevice.GetGPU()->GetPresentMode();
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_GraphicsDevice.GetVkDevice(), &createInfo, NULL, &m_SwapChain) != VK_SUCCESS)
        {
            Cleanup();
            throw std::runtime_error("couldn't create swapchain");
        }

        std::uint32_t enumSize;
        vkGetSwapchainImagesKHR(m_GraphicsDevice.GetVkDevice(), m_SwapChain, &enumSize, nullptr);
        m_SwapChainImages.resize(enumSize);
        vkGetSwapchainImagesKHR(m_GraphicsDevice.GetVkDevice(), m_SwapChain, &enumSize, m_SwapChainImages.data());

        m_SwapChainImageViews.resize(m_SwapChainImages.size());

        for (std::uint32_t i = 0; i < m_SwapChainImages.size(); ++i)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_SwapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_GraphicsDevice.GetGPU()->GetSurfaceFormat().format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_GraphicsDevice.GetVkDevice(), &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
            {
                Cleanup();
                throw std::runtime_error("failed to create image views");
            }
        }

        std::cout << "Swap chain created.\n";
    }

    void SwapChain::Cleanup() noexcept
    {
        for (const auto& imageView : m_SwapChainImageViews)
        {
            vkDestroyImageView(m_GraphicsDevice.GetVkDevice(), imageView, nullptr);
        }

        if (m_SwapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_GraphicsDevice.GetVkDevice(), m_SwapChain, nullptr);
        }
    }
}