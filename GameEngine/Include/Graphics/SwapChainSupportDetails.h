#pragma once
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

namespace SekhmetEngine
{
    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
}