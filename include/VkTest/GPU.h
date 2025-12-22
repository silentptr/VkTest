#ifndef VKTEST_GPU_H_
#define VKTEST_GPU_H_

#include <cstdint>
#include <cstring>
#include <vector>
#include <optional>
#include <iostream>
#include <string>
#include <algorithm>

#include "VkTest/IncludeVolk.h"

#include <GLFW/glfw3.h>

namespace VkTest
{
    class GPU
    {
        friend std::ostream& operator<<(std::ostream&,const GPU&);
    private:
        VkPhysicalDevice m_PhysicalDevice;
        VkSurfaceKHR m_Surface;
        VkPhysicalDeviceProperties m_DeviceProperties;
        std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
        std::vector<VkExtensionProperties> m_ExtensionProperties;

        std::optional<std::uint32_t> m_GraphicsQueueIndex;
        std::optional<std::uint32_t> m_PresentQueueIndex;

        bool m_HasSwapChainSupport;
        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
        std::vector<VkPresentModeKHR> m_PresentModes;
        std::optional<VkSurfaceFormatKHR> m_SurfaceFormat;
        std::optional<VkPresentModeKHR> m_PresentMode;
        VkSwapchainKHR m_SwapChain;
    public:
        inline GPU(VkPhysicalDevice pd, VkSurfaceKHR surf) noexcept : m_PhysicalDevice(pd), m_Surface(surf)
        {
            vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_DeviceProperties);
            std::uint32_t enumSize;
            vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &enumSize, NULL);
            m_QueueFamilyProperties.resize(enumSize);
            vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &enumSize, m_QueueFamilyProperties.data());

            for (std::uint32_t i = 0; i < enumSize; ++i)
            {
                if (m_QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    m_GraphicsQueueIndex = i;
                    break;
                }
            }

            for (std::uint32_t i = 0; i < enumSize; ++i)
            {
                VkBool32 val;
                vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, m_Surface, &val);

                if (val == VK_TRUE)
                {
                    m_PresentQueueIndex = i;
                    break;
                }
            }

            vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, NULL, &enumSize, NULL);
            m_ExtensionProperties.resize(enumSize);
            vkEnumerateDeviceExtensionProperties(m_PhysicalDevice, NULL, &enumSize, m_ExtensionProperties.data());

            for (auto& extension : m_ExtensionProperties)
            {
                if (std::strcmp(extension.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0)
                {
                    m_HasSwapChainSupport = true;
                    break;
                }
            }
            
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_PhysicalDevice, m_Surface, &m_SurfaceCapabilities);
            vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &enumSize, NULL);
            m_SurfaceFormats.resize(enumSize);
            vkGetPhysicalDeviceSurfaceFormatsKHR(m_PhysicalDevice, m_Surface, &enumSize, m_SurfaceFormats.data());
            vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &enumSize, NULL);
            m_PresentModes.resize(enumSize);
            vkGetPhysicalDeviceSurfacePresentModesKHR(m_PhysicalDevice, m_Surface, &enumSize, m_PresentModes.data());

            for (const auto& surfaceFormat : m_SurfaceFormats)
            {
                if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    m_SurfaceFormat = surfaceFormat;
                    break;
                }
            }

            for (const auto& presentMode : m_PresentModes)
            {
                if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                {
                    m_PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                    break;
                }
            }
        }

        inline VkPhysicalDevice GetPhysicalDevice() const noexcept { return m_PhysicalDevice; }
        inline const char* const GetDeviceName() const noexcept { return m_DeviceProperties.deviceName; }
        inline bool IsDiscrete() const noexcept { return m_DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU; }
        inline bool IsIntegrated() const noexcept { return m_DeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU; }
        inline bool HasGraphicsQueue() const noexcept { return m_GraphicsQueueIndex.has_value(); }
        inline std::uint32_t GetGraphicsQueueIndex() const noexcept { return m_GraphicsQueueIndex.value(); }
        inline bool HasPresentQueue() const noexcept { return m_PresentQueueIndex.has_value(); }
        inline std::uint32_t GetPresentQueueIndex() const noexcept { return m_PresentQueueIndex.value(); }
        inline bool HasSwapChainSupport() const noexcept { return m_HasSwapChainSupport; }
        inline const VkSurfaceCapabilitiesKHR& GetSurfaceCapabilities() const noexcept { return m_SurfaceCapabilities; }
        inline const VkSurfaceFormatKHR& GetSurfaceFormat() const noexcept { return m_SurfaceFormat.value(); }
        inline const VkPresentModeKHR& GetPresentMode() const noexcept { return m_PresentMode.value(); }

        inline bool IsDeviceSuitable() const noexcept { return HasGraphicsQueue() && HasPresentQueue() && HasSwapChainSupport() && m_SurfaceFormat.has_value() && m_PresentMode.has_value(); }
    };

    std::ostream& operator<<(std::ostream&,const GPU&);
}

#endif