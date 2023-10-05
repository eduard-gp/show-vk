#include "show-vk.h"

#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <regex>
#include <string>
#include <vector>

#include "show-vk.h"

void shw::parseArgs(const std::vector<std::string>& args) {
    std::unordered_map<std::string, bool> instanceInfoOptions{
        {instanceAllOption, false},
        {instanceVersionOption, false},
        {instanceShowExtensionsOption, false},
        {instanceShowLayersOption, false}
    };
    std::unordered_map<std::string, std::vector<std::string>> instanceSupportOptions{};

    for (const auto& arg : args) {
        parseInstanceOption(arg, instanceInfoOptions, instanceSupportOptions);
    }
    executeInstanceOptions(instanceInfoOptions, instanceSupportOptions);
}

int main(int argc, char* argv[]) {
	std::cout << "Hello CMake!\n";
    std::vector<std::string> args;
    args.reserve(argc);
    std::copy(argv, argv + argc, std::back_inserter(args));
    shw::parseArgs(args);

    // .\show-vk.exe --instance-support-extensions=VK_KHR_surface,VK_KHR_external_fence_capabilities,VK_dummy  --instance-support-layers=VK_LAYER_NV_optimus,VK_LAYER_LUNARG_api_dump,VK_dummy 
}
