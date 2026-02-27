#ifndef VKTEST_APPSESSION_H_
#define VKTEST_APPSESSION_H_

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "VkTest/IncludeVolk.h"

#include <GLFW/glfw3.h>

namespace VkTest
{
    class AppSession
    {
    private:
        VkInstance m_VkInst;
    #ifdef VK_TEST_DEBUG
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        static const char* m_ValidationLayers[];
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
    #endif

        void Cleanup() noexcept;
    public:
        AppSession();
        ~AppSession() noexcept { Cleanup(); }

        VkInstance GetVkInst() const noexcept { return m_VkInst; }
    };
}

#endif