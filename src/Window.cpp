#include "VkTest/Window.h"

namespace VkTest
{
    Window::Window(const AppSession& sesh) : m_AppSession(sesh), m_Handle(nullptr), m_Surface(VK_NULL_HANDLE)
    {
        glfwDefaultWindowHints();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        m_Handle = glfwCreateWindow(1280, 720, "Vulkan Test", nullptr, nullptr);

        if (m_Handle == nullptr)
        {
            throw std::runtime_error("couldn't create window");
        }

        if (glfwCreateWindowSurface(m_AppSession.GetVkInst(), m_Handle, nullptr, &m_Surface) != VK_SUCCESS)
        {
            Cleanup();
            throw std::runtime_error("couldn't create window surface");
        }

        std::cout << "Window & window surface created.\n";
    }

    void Window::Cleanup() noexcept
    {
        if (m_Surface != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_AppSession.GetVkInst(), m_Surface, NULL);
        }

        if (m_Handle != nullptr)
        {
            glfwDestroyWindow(m_Handle);
        }
    }
}