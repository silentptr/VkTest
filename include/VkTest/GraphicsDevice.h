#ifndef VKTEST_GRAPHICSDEVICE_H_
#define VKTEST_GRAPHICSDEVICE_H_

#include <cstdint>
#include <vector>
#include <set>
#include <stdexcept>
#include <iostream>

#include "VkTest/IncludeVolk.h"
#include "VkTest/AppSession.h"
#include "VkTest/Window.h"
#include "VkTest/GPU.h"

#include <GLFW/glfw3.h>

namespace VkTest
{
    class GraphicsDevice
    {
    private:
        const AppSession& m_AppSession;
        const Window& m_Window;

        std::vector<GPU> m_GPUs;
        GPU* m_GPU;

        VkDevice m_VkDevice;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;
    public:
        GraphicsDevice(const AppSession&, const Window&, const std::vector<const char*>&);
        ~GraphicsDevice() noexcept;

        VkDevice GetVkDevice() const noexcept { return m_VkDevice; }
        GPU* GetGPU() const noexcept { return m_GPU; }

        const VkQueue& GetVkGraphicsQueue() const noexcept { return m_GraphicsQueue; }
        const VkQueue& GetVkPresentQueue() const noexcept { return m_PresentQueue; }
    };
}

#endif