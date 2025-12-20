#ifndef VKTEST_GPU_H_
#define VKTEST_GPU_H_

#include <cstdint>
#include <vector>
#include <optional>
#include <iostream>
#include <string>

#include "VkTest/IncludeVolk.h"

namespace VkTest
{
    class GPU
    {
        friend std::ostream& operator<<(std::ostream&,const GPU&);
    private:
        VkPhysicalDevice m_PhysicalDevice;
        VkPhysicalDeviceProperties m_DeviceProperties;
        std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;

        std::optional<std::uint32_t> m_GraphicsQueueIndex;
        std::optional<std::uint32_t> m_PresentQueueIndex;
    public:
        inline GPU(VkPhysicalDevice pd, VkSurfaceKHR surface) noexcept : m_PhysicalDevice(pd)
        {
            vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_DeviceProperties);
            std::uint32_t enumSize;
            vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &enumSize, NULL);
            m_QueueFamilyProperties = std::vector<VkQueueFamilyProperties>(enumSize);
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
                vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, i, surface, &val);

                if (val == VK_TRUE)
                {
                    m_PresentQueueIndex = i;
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
    };

    std::ostream& operator<<(std::ostream&,const GPU&);
}

#endif