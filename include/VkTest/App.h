#ifndef VKTEST_APP_H_
#define VKTEST_APP_H_

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <set>
#include <fstream>
#include <string>

#include "VkTest/IncludeVolk.h"
#include "VkTest/GPU.h"

#include <GLFW/glfw3.h>

namespace VkTest
{
    class App
    {
    private:
    #ifdef VK_TEST_DEBUG
        static const char* m_ValidationLayers[];

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT*, void*);
    #endif

        static const std::vector<const char*> m_DeviceExtensions;

        GLFWwindow* m_Window;

        VkInstance m_VkInst;
    #ifdef VK_TEST_DEBUG
        VkDebugUtilsMessengerEXT m_DebugMessenger;
    #endif
        VkSurfaceKHR m_Surface;

        std::vector<GPU> m_GPUs;
        GPU* m_GPU;

        VkDevice m_VkDevice;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
        VkSwapchainKHR m_SwapChain;
        VkExtent2D m_SwapChainExtent;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;

        VkRenderPass m_RenderPass;
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;
        std::vector<VkFramebuffer> m_Framebuffers;

        VkCommandPool m_CommandPool;
        VkCommandBuffer m_CommandBuffer;

        void CreateLogicalDevice();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateRenderPass();
        void CreateGraphicsPipeline();
        void CreateFramebuffers();
        void CreateCommandPool();
        void CreateCommandBuffer();
    public:
        App();
        ~App() noexcept;
    };
}

#endif