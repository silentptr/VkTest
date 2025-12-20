#include "VkTest/GPU.h"

namespace VkTest
{
    std::ostream& operator<<(std::ostream& os, const GPU& gpu)
    {
        // return os << gpu.m_DeviceProperties.deviceName << " (driver version: " <<
        // std::to_string(VK_API_VERSION_MAJOR(gpu.m_DeviceProperties.driverVersion)) << '.' <<
        // std::to_string(VK_API_VERSION_MINOR(gpu.m_DeviceProperties.driverVersion)) << '.' <<
        // std::to_string(VK_API_VERSION_PATCH(gpu.m_DeviceProperties.driverVersion)) << " variant " <<
        // std::to_string(VK_API_VERSION_VARIANT(gpu.m_DeviceProperties.driverVersion)) << ") (api version: " <<
        // std::to_string(VK_API_VERSION_MAJOR(gpu.m_DeviceProperties.apiVersion)) << '.' <<
        // std::to_string(VK_API_VERSION_MINOR(gpu.m_DeviceProperties.apiVersion)) << '.' <<
        // std::to_string(VK_API_VERSION_PATCH(gpu.m_DeviceProperties.apiVersion)) << " variant " <<
        // std::to_string(VK_API_VERSION_VARIANT(gpu.m_DeviceProperties.apiVersion)) << ')';

        os << gpu.m_DeviceProperties.deviceName << " (type: ";

        switch (gpu.m_DeviceProperties.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            os << "integrated";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            os << "discrete";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            os << "virtual";
            break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            os << "cpu";
            break;
        default:
            os << "unknown";
            break;
        }

        os << ")\n\ndriver version: " << 
        std::to_string(VK_API_VERSION_MAJOR(gpu.m_DeviceProperties.driverVersion)) << '.' <<
        std::to_string(VK_API_VERSION_MINOR(gpu.m_DeviceProperties.driverVersion)) << '.' <<
        std::to_string(VK_API_VERSION_PATCH(gpu.m_DeviceProperties.driverVersion)) << " variant " <<
        std::to_string(VK_API_VERSION_VARIANT(gpu.m_DeviceProperties.driverVersion)) << "\napi version: " <<
        std::to_string(VK_API_VERSION_MAJOR(gpu.m_DeviceProperties.apiVersion)) << '.' <<
        std::to_string(VK_API_VERSION_MINOR(gpu.m_DeviceProperties.apiVersion)) << '.' <<
        std::to_string(VK_API_VERSION_PATCH(gpu.m_DeviceProperties.apiVersion)) << " variant " <<
        std::to_string(VK_API_VERSION_VARIANT(gpu.m_DeviceProperties.apiVersion)) << '\n';
        return os;
    }
}