#include "VkTest/GraphicsDevice.h"

namespace VkTest
{
    GraphicsDevice::GraphicsDevice(const AppSession& sesh, const Window& win, const std::vector<const char*>& deviceExtensions) :
    m_AppSession(sesh), m_Window(win), m_GPU(nullptr), m_VkDevice(VK_NULL_HANDLE), m_GraphicsQueue(VK_NULL_HANDLE), m_PresentQueue(VK_NULL_HANDLE)
    {
        std::uint32_t enumSize;
        vkEnumeratePhysicalDevices(sesh.GetVkInst(), &enumSize, nullptr);

        if (enumSize == 0) { throw std::runtime_error("no GPUs found"); }

        std::vector<VkPhysicalDevice> physicalDevices(enumSize);
        vkEnumeratePhysicalDevices(sesh.GetVkInst(), &enumSize, physicalDevices.data());

        for (const auto& device : physicalDevices)
        {
            const GPU& gpu = m_GPUs.emplace_back(device, m_Window.GetSurface());
            std::cout << "Found GPU: " << gpu << '\n';
        }

        if (m_GPUs.size() == 1 && m_GPUs[0].IsDeviceSuitable())
        {
            m_GPU = &m_GPUs[0];
        }
        else
        {
            // look for discrete first

            for (auto& g : m_GPUs)
            {
                if (g.IsDiscrete() && g.IsDeviceSuitable())
                {
                    m_GPU = &g;
                    break;
                }
            }

            if (m_GPU == nullptr)
            {
                // then look for integrated

                for (auto& g : m_GPUs)
                {
                    if (g.IsIntegrated() && g.IsDeviceSuitable())
                    {
                        m_GPU = &g;
                        break;
                    }
                }

                if (m_GPU == nullptr)
                {
                    // fall back to anything that is suitable
                    
                    for (auto& g : m_GPUs)
                    {
                        if (g.IsDeviceSuitable()) { m_GPU = &g; break; }
                    }
                }
            }
        }
        
        if (m_GPU == nullptr) { throw std::runtime_error("none of the gpus are suitable"); }

        // create logical device and queues

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<std::uint32_t> queueFamilyIndexes = {m_GPU->GetGraphicsQueueIndex(), m_GPU->GetPresentQueueIndex()};
        float queuePriority = 1.0f;

        for (std::uint32_t queueFamilyIndex : queueFamilyIndexes)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(std::move(queueCreateInfo));
        }

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<std::uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<std::uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(m_GPU->GetPhysicalDevice(), &createInfo, nullptr, &m_VkDevice) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device");
        }

        std::cout << "Logical device created.\n";
        vkGetDeviceQueue(m_VkDevice, m_GPU->GetGraphicsQueueIndex(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_VkDevice, m_GPU->GetPresentQueueIndex(), 0, &m_PresentQueue);
    }

    GraphicsDevice::~GraphicsDevice() noexcept
    {
        if (m_VkDevice != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_VkDevice, nullptr);
        }
    }
}