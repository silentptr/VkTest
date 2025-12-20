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
    void App::CreateLogicalDevice()
    {
        GPU* gpu = nullptr;

        if (m_GPUs.size() == 1 && m_GPUs[0].HasGraphicsQueue())
        {
            gpu = &m_GPUs[0];
        }
        else
        {
            for (auto& g : m_GPUs)
            {
                if (g.HasGraphicsQueue() && g.HasPresentQueue())
                {
                    gpu = &g;

                    if (g.IsDiscrete())
                    {
                        break;
                    }
                }
            }
        }
        
        if (gpu == nullptr) { throw std::runtime_error("none of the gpus can do graphics AND presentation"); }

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
        createInfo.enabledExtensionCount = 0;

        if (vkCreateDevice(m_GPU->GetPhysicalDevice(), &createInfo, NULL, &m_VkDevice) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device");
        }

        vkGetDeviceQueue(m_VkDevice, m_GPU->GetGraphicsQueueIndex(), 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_VkDevice, m_GPU->GetPresentQueueIndex(), 0, &m_PresentQueue);
    }

    App::App() : m_Window(NULL), m_VkInst(VK_NULL_HANDLE), m_Surface(VK_NULL_HANDLE), m_VkDevice(VK_NULL_HANDLE)
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
        //glfwShowWindow(m_Window);
    }

    App::~App() noexcept
    {
        if (m_Window != NULL)
        {
            glfwDestroyWindow(m_Window);
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