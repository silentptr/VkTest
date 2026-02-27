#ifndef VKTEST_SWAPCHAIN_H_
#define VKTEST_SWAPCHAIN_H_

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include <iostream>

#include "VkTest/IncludeVolk.h"
#include "VkTest/AppSession.h"
#include "VkTest/Window.h"
#include "VkTest/GraphicsDevice.h"

#include <GLFW/glfw3.h>

namespace VkTest
{
    class SwapChain
    {
    private:
        const GraphicsDevice& m_GraphicsDevice;

        VkSwapchainKHR m_SwapChain;
        VkExtent2D m_SwapChainExtent;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;

        void Cleanup() noexcept;
    public:
        SwapChain(const Window&, const GraphicsDevice&);
        ~SwapChain() noexcept { Cleanup(); }

        const VkSwapchainKHR& GetVkSwapChain() const noexcept { return m_SwapChain; }
        const VkExtent2D& GetExtent() const noexcept { return m_SwapChainExtent; }
        const std::vector<VkImageView>& GetSwapChainImageViews() const noexcept { return m_SwapChainImageViews; }
    };
}

#endif