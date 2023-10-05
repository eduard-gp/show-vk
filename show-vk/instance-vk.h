#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.h>

namespace shw {
    void printInstanceVersion();
    void printTable(const std::vector<std::string>& header, const std::vector<std::vector<std::string>>& rows);
    void parseInstanceOption(const std::string& option,
        std::unordered_map<std::string, bool>& infoOptions,
        std::unordered_map<std::string, std::vector<std::string>>& supportOptions);
    bool isSupportOption(const std::string& arg, const std::string& supportOption);
    void insertSupportOptionValues(const std::string& arg,
        const std::string& supportOption,
        std::unordered_map<std::string, std::vector<std::string>>& instanceSupportOptions);
    void executeInstanceOptions(const std::unordered_map<std::string, bool>& infoOptions,
        const std::unordered_map<std::string, std::vector<std::string>>& supportOptions);
    // instance extensions
    std::vector<VkExtensionProperties> getInstanceExtensions();
    void printTable(const std::vector<std::string>& header, const std::vector<VkExtensionProperties>& extensions);
    void printInstanceExtensions();
    void printInstanceExtensionsSupport(const std::vector<std::string>& extensions);
    // instance layers
    std::vector<VkLayerProperties> getInstanceLayers();
    void printTable(const std::vector<std::string>& header, const std::vector<VkLayerProperties>& layers);
    void printInstanceLayers();
    void printInstanceLayersSupport(const std::vector<std::string>& layers);
    
    extern const std::string instanceAllOption;
    extern const std::string instanceVersionOption;
    extern const std::string instanceShowExtensionsOption;
    extern const std::string instanceShowLayersOption;
    extern const std::string instanceExtensionsSupportOption;
    extern const std::string instanceLayersSupportOption;
    extern const std::string instanceKeyValueOptionDelim;
    extern const std::string instanceValuesOptionDelim;
}
