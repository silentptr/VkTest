#ifndef VKTEST_WINDOW_H_
#define VKTEST_WINDOW_H_

#include <stdexcept>
#include <iostream>

#include "VkTest/IncludeVolk.h"
#include "VkTest/AppSession.h"

#include <GLFW/glfw3.h>

namespace VkTest
{
    class Window
    {
    private:
        const AppSession& m_AppSession;
        GLFWwindow* m_Handle;
        VkSurfaceKHR m_Surface;

        void Cleanup() noexcept;
    public:
        Window(const AppSession&);
        ~Window() noexcept { Cleanup(); }

        void Show() noexcept { glfwShowWindow(m_Handle); }

        GLFWwindow* GetHandle() const noexcept { return m_Handle; }
        VkSurfaceKHR GetSurface() const noexcept { return m_Surface; }
    };
}

#endif