#pragma once

#include <string>
#include <vulkan/vulkan.h>

namespace shw {
    std::string resultToStr(VkResult result);
    std::string getError(const std::string& message, VkResult result);
    void printError(const std::string& message, VkResult result);
}
