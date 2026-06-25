#ifndef VKTEST_APP_H_
#define VKTEST_APP_H_

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <set>
#include <fstream>
#include <string>
#include <array>

#include "VkTest/IncludeVolk.h"
#include "VkTest/AppSession.h"
#include "VkTest/Window.h"
#include "VkTest/GPU.h"
#include "VkTest/GraphicsDevice.h"
#include "VkTest/SwapChain.h"
#include "VkTest/Pipeline.h"
#include "VkTest/Framebuffer.h"
#include "VkTest/CommandBuffer.h"

namespace VkTest
{
    class App
    {
    private:
        static const std::vector<const char*> m_DeviceExtensions;

        AppSession m_AppSession;
        Window m_Window;
        GraphicsDevice m_GraphicsDevice;
        SwapChain m_SwapChain;
        Pipeline m_Pipeline;
        Framebuffer m_Framebuffer;

        static constexpr int m_InFlightCount = 2;
        int m_InFlightIndex = 0;

        std::array<VkSemaphore, m_InFlightCount> m_ImgAvailSemaphores;
        std::vector<VkSemaphore> m_RenderDoneSemaphores;
        std::array<VkFence, m_InFlightCount> m_InFlightFences;
        std::vector<CommandBuffer> m_CmdBuffers;

        void Loop();
        void Draw();
    public:
        App();
        ~App() noexcept;
    };
}

#endif