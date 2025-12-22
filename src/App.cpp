#include "VkTest/App.h"

namespace VkTest
{
#ifdef VK_TEST_DEBUG
    const char* App::m_ValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };

    VKAPI_ATTR VkBool32 VKAPI_CALL App::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        //std::cerr << "Validation layer message: " << pCallbackData->pMessage << '\n';
        return VK_FALSE;
    }
#endif

    const std::vector<const char*> App::m_DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    void App::CreateLogicalDevice()
    {
        GPU* gpu = nullptr;

        if (m_GPUs.size() == 1 && m_GPUs[0].HasGraphicsQueue())
        {
            gpu = &m_GPUs[0];
        }
        else
        {
            // look for discrete first

            for (auto& g : m_GPUs)
            {
                if (g.IsDiscrete() && g.IsDeviceSuitable())
                {
                    gpu = &g;
                    break;
                }
            }

            if (gpu == nullptr)
            {
                // then look for integrated

                for (auto& g : m_GPUs)
                {
                    if (g.IsIntegrated() && g.IsDeviceSuitable())
                    {
                        gpu = &g;
                        break;
                    }
                }

                if (gpu == nullptr)
                {
                    // fall back to anything that can do graphics & presentation
                    
                    for (auto& g : m_GPUs)
                    {
                        if (g.HasGraphicsQueue() && g.IsDeviceSuitable()) { gpu = &g; break; }
                    }
                }
            }
        }
        
        if (gpu == nullptr) { throw std::runtime_error("none of the gpus are suitable"); }

        m_GPU = gpu;
        std::cout << "\nSelected GPU: " << (m_GPU->GetDeviceName()) << '\n';
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
        createInfo.enabledExtensionCount = static_cast<std::uint32_t>(m_DeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

        if (vkCreateDevice(m_GPU->GetPhysicalDevice(), &createInfo, NULL, &m_VkDevice) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device");
        }

        vkGetDeviceQueue(m_VkDevice, m_GPU->GetGraphicsQueueIndex(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_VkDevice, m_GPU->GetPresentQueueIndex(), 0, &m_PresentQueue);
    }
    
    void App::CreateSwapChain()
    {
        VkExtent2D extent;
        const auto& surfaceCapabilities = m_GPU->GetSurfaceCapabilities();

        if (surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
        {
            extent.width = surfaceCapabilities.currentExtent.width;
            extent.height = surfaceCapabilities.currentExtent.height;
        }
        else
        {
            int w, h;
            glfwGetFramebufferSize(m_Window, &w, &h);
            extent.width = std::clamp(static_cast<std::uint32_t>(w), surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);
            extent.height = std::clamp(static_cast<std::uint32_t>(h), surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);
        }

        std::uint32_t imageCount = surfaceCapabilities.minImageCount + 1;

        if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
        {
            imageCount = surfaceCapabilities.maxImageCount;
        }

        const auto& surfaceFormat = m_GPU->GetSurfaceFormat();

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_Surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        std::uint32_t queueFamilyIndices[] = {m_GPU->GetGraphicsQueueIndex(), m_GPU->GetPresentQueueIndex()};
        
        if (m_GPU->GetGraphicsQueueIndex() != m_GPU->GetPresentQueueIndex())
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
        createInfo.presentMode = m_GPU->GetPresentMode();
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_VkDevice, &createInfo, NULL, &m_SwapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("couldn't create swapchain");
        }
    }

    App::App() : m_Window(NULL), m_VkInst(VK_NULL_HANDLE), m_Surface(VK_NULL_HANDLE), m_VkDevice(VK_NULL_HANDLE), m_SwapChain(VK_NULL_HANDLE)
    {
        if (glfwInit() == GLFW_FALSE)
        {
            throw std::runtime_error("glfw failed to initialise");
        }

        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        m_Window = glfwCreateWindow(1280, 720, "Vulkan Test", NULL, NULL);

        if (m_Window == NULL)
        {
            throw std::runtime_error("couldn't create window");
        }

        if (volkInitialize() != VK_SUCCESS)
        {
            throw std::runtime_error("failed to initialise volk");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Vulkan Test";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        std::uint32_t glfwExtensionCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        if (glfwExtensions == NULL)
        {
            throw std::runtime_error("extensions not found");
        }

        std::vector<const char*> extensions;

        for (std::uint32_t i = 0; i < glfwExtensionCount; ++i)
        {
            extensions.push_back(glfwExtensions[i]);
        }

    #ifdef VK_TEST_DEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    #endif

        createInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();
    #ifdef VK_TEST_DEBUG
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = m_ValidationLayers;
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = DebugCallback;
        createInfo.pNext = &debugCreateInfo;
    #else
        createInfo.enabledLayerCount = 0;
    #endif

        if (vkCreateInstance(&createInfo, NULL, &m_VkInst) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance");
        }

        volkLoadInstance(m_VkInst);

    #ifdef VK_TEST_DEBUG
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_VkInst, "vkCreateDebugUtilsMessengerEXT");

        if (func == VK_NULL_HANDLE) { throw std::runtime_error("couldn't load debug messager create function"); }

        if (func(m_VkInst, &debugCreateInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger");
        }
    #endif

        if (glfwCreateWindowSurface(m_VkInst, m_Window, NULL, &m_Surface) != VK_SUCCESS)
        {
            throw std::runtime_error("couldn't create window surface");
        }

        std::uint32_t enumSize;
        vkEnumeratePhysicalDevices(m_VkInst, &enumSize, NULL);

        if (enumSize == 0) { throw std::runtime_error("no GPUs found"); }

        std::vector<VkPhysicalDevice> physicalDevices(enumSize);
        vkEnumeratePhysicalDevices(m_VkInst, &enumSize, physicalDevices.data());

        for (const auto& device : physicalDevices)
        {
            const GPU& gpu = m_GPUs.emplace_back(device, m_Surface);
            std::cout << "Found GPU: " << gpu << '\n';
        }

        CreateLogicalDevice();
        std::cout << "Logical device created.\n";
        CreateSwapChain();
        std::cout << "Swap chain created.\n";
        //glfwShowWindow(m_Window);
    }

    App::~App() noexcept
    {
        if (m_Window != NULL)
        {
            glfwDestroyWindow(m_Window);
        }

        if (m_SwapChain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_VkDevice, m_SwapChain, NULL);
        }
        
        if (m_VkDevice != VK_NULL_HANDLE)
        {
            vkDestroyDevice(m_VkDevice, NULL);
        }

        if (m_Surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_VkInst, m_Surface, NULL);
        }

    #ifdef VK_TEST_DEBUG
        if (m_DebugMessenger != VK_NULL_HANDLE)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInst, "vkDestroyDebugUtilsMessengerEXT");

            if (func != nullptr)
            {
                func(m_VkInst, m_DebugMessenger, NULL);
            }
        }
    #endif

        if (m_VkInst != VK_NULL_HANDLE)
        {
            vkDestroyInstance(m_VkInst, NULL);
        }

        glfwTerminate();
    }
}