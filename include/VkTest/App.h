#ifndef VKTEST_APP_H_
#define VKTEST_APP_H_

#include <cstdint>
#include <stdexcept>
#include <vector>
#include <set>
#include <fstream>
#include <string>

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
        CommandBuffer m_CommandBuffer;

        VkSemaphore m_ImgAvailSemaphore;
        VkSemaphore m_RenderDoneSemaphore;
        VkFence m_InFlightFence;

        void Loop();
        void Draw();
    public:
        App();
        ~App() noexcept;
    };
}

#endif