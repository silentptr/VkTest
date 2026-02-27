#include "VkTest/AppSession.h"

namespace VkTest
{
#ifdef VK_TEST_DEBUG
    const char* AppSession::m_ValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };

    VKAPI_ATTR VkBool32 VKAPI_CALL AppSession::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        {
            std::cerr << "Validation layer message: " << pCallbackData->pMessage << "\n\n";
        }
        
        return VK_FALSE;
    }
#endif

    AppSession::AppSession() : m_VkInst(VK_NULL_HANDLE)
    {
        if (glfwInit() == GLFW_FALSE)
        {
            throw std::runtime_error("glfw failed to initialise");
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

        if (glfwExtensions == nullptr)
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

        if (vkCreateInstance(&createInfo, nullptr, &m_VkInst) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance");
        }

        volkLoadInstance(m_VkInst);

    #ifdef VK_TEST_DEBUG
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_VkInst, "vkCreateDebugUtilsMessengerEXT");

        if (func == VK_NULL_HANDLE) { throw std::runtime_error("couldn't load debug messager create function"); }

        if (func(m_VkInst, &debugCreateInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
        {
            Cleanup();
            throw std::runtime_error("failed to set up debug messenger");
        }
    #endif

    #ifdef VK_TEST_DEBUG
        std::cout << "Instance created with debug messaging.\n";
    #else
        std::cout << "Instance created.\n";
    #endif
    }

    void AppSession::Cleanup() noexcept
    {
    #ifdef VK_TEST_DEBUG
        if (m_DebugMessenger != VK_NULL_HANDLE)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInst, "vkDestroyDebugUtilsMessengerEXT");

            if (func != nullptr)
            {
                func(m_VkInst, m_DebugMessenger, nullptr);
            }
        }
    #endif

        if (m_VkInst != VK_NULL_HANDLE) { vkDestroyInstance(m_VkInst, nullptr); }

        glfwTerminate();
    }
}