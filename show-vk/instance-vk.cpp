#include "instance-vk.h"
#include "error-vk.h"

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <regex>
#include <unordered_set>

void shw::printInstanceVersion() {
    std::uint32_t version{};
    VkResult result{ vkEnumerateInstanceVersion(&version) };
    if (result == VK_SUCCESS) {
        std::cout << "Vulkan Instance Version: " << VK_API_VERSION_VARIANT(version)
            << '.' << VK_API_VERSION_MAJOR(version)
            << '.' << VK_API_VERSION_MINOR(version)
            << '.' << VK_API_VERSION_PATCH(version) << '\n';
    }
    else {
        printError("vkEnumerateInstanceversion() failed", result);
    }
}

void shw::printTable(const std::vector<std::string>& header, const std::vector<std::vector<std::string>>& rows) {
    if (rows.size() > 0 && header.size() != rows[0].size()) {
        throw std::runtime_error{ "Different number of columns in header and rows!" };
    }
    std::vector<std::size_t> offsets(header.size());
    std::transform(header.cbegin(), header.cend(),
        offsets.begin(),
        [](const std::string& columnName) { return columnName.size(); });
    for (const auto& row : rows) {
        for (std::size_t i{}, length{ row.size() }; i < length; ++i) {
            offsets[i] = std::max(offsets[i], row[i].size());
        }
    }
    constexpr std::size_t offsetDelim{ 5 };
    auto printRow{ [&](const std::vector<std::string>& row) {
        for (std::size_t i{}, length{row.size() - 1}; i < length; ++i) {
            std::cout << std::setw(offsets[i]) << row[i] << std::setw(offsetDelim) << ' ';
        }
        std::cout << row.back() << '\n';
    } };
    printRow(header);
    for (const auto& row : rows) {
        printRow(row);
    }
}

bool shw::isSupportOption(const std::string& arg, const std::string& supportOption) {
    return (arg.find(supportOption) == 0) && (arg.size() > supportOption.size())
        && (arg[supportOption.size()] == '=');
}

void shw::insertSupportOptionValues(const std::string& arg,
        const std::string& supportOption,
        std::unordered_map<std::string, std::vector<std::string>>& supportOptions) {
    std::regex regex{instanceValuesOptionDelim};
    std::copy(std::sregex_token_iterator{arg.cbegin() + supportOption.size() + instanceKeyValueOptionDelim.size(),
                                        arg.cend(),
                                        regex, -1},
            std::sregex_token_iterator{},
            std::back_inserter(supportOptions[supportOption]));
 }

void shw::parseInstanceOption(const std::string& arg,
        std::unordered_map<std::string, bool>& infoOptions,
        std::unordered_map<std::string, std::vector<std::string>>& supportOptions) {
    if (arg == instanceAllOption) {
        infoOptions[instanceAllOption] = true;
    } else if (arg == instanceVersionOption) {
        infoOptions[instanceVersionOption] = true;
    } else if (arg == instanceShowExtensionsOption) {
        infoOptions[instanceShowExtensionsOption] = true;
    } else if (arg == instanceShowLayersOption) {
        infoOptions[instanceShowLayersOption] = true;
    } else if (isSupportOption(arg, instanceExtensionsSupportOption)) {
        insertSupportOptionValues(arg, instanceExtensionsSupportOption, supportOptions);
    } else if (isSupportOption(arg, instanceLayersSupportOption)) {
        insertSupportOptionValues(arg, instanceLayersSupportOption, supportOptions);
    }
}

void shw::executeInstanceOptions(const std::unordered_map<std::string, bool>& infoOptions,
    const std::unordered_map<std::string, std::vector<std::string>>& supportOptions) {
    if (auto it{infoOptions.find(instanceAllOption)};
            it != infoOptions.cend() && it->second) {
        shw::printInstanceVersion();
        shw::printInstanceExtensions();
        shw::printInstanceLayers();
    }
    else {
        if (auto it{infoOptions.find(instanceVersionOption)};
                it != infoOptions.cend() && it->second) {
            shw::printInstanceVersion();
        }
        if (auto it{infoOptions.find(instanceShowExtensionsOption)};
                it != infoOptions.cend() && it->second) {
            shw::printInstanceExtensions();
        }
        if (auto it{ infoOptions.find(instanceShowLayersOption)};
                it != infoOptions.cend() && it->second) {
            shw::printInstanceLayers();
        }
    }
    if (auto it{supportOptions.find(instanceExtensionsSupportOption)};
            it != supportOptions.cend()) {
        shw::printInstanceExtensionsSupport(it->second);
    }
    if (auto it{supportOptions.find(instanceLayersSupportOption)};
            it != supportOptions.cend()) {
        shw::printInstanceLayersSupport(it->second);
    }
}

void shw::printTable(const std::vector<std::string>& header, const std::vector<VkExtensionProperties>& extensions) {
    constexpr std::size_t numExtensionsProperties{ 2 };
    if (header.size() != numExtensionsProperties) {
        throw std::runtime_error{ "Invalid number of columns in header! VkExtensionProperties has 2 data members." };
    }
    const std::size_t offsetWidth{ 5 };
    std::size_t nameWidth{ header[0].size() };
    std::size_t versionWidth{ header[1].size() };
    for (const auto& extension : extensions) {
        nameWidth = std::max(nameWidth, std::strlen(extension.extensionName));
        versionWidth = std::max(versionWidth, std::to_string(extension.specVersion).size());
    }
    auto printRow{ [=](const auto& name, const auto& specVersion) {
        std::cout << '\t' << std::left << std::setw(nameWidth) << name << std::setw(offsetWidth) << " "
            << std::right << std::setw(versionWidth) << specVersion << '\n';
    } };
    printRow(header[0], header[1]);
    for (const auto& extension : extensions) {
        printRow(extension.extensionName, extension.specVersion);
    }
}

std::vector<VkExtensionProperties> shw::getInstanceExtensions() {
    std::uint32_t count{};
    VkResult result{ vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr) };
    if (result == VK_SUCCESS) {
        std::vector<VkExtensionProperties> extensions(count);
        result = vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
        if (result == VK_SUCCESS || result == VK_INCOMPLETE) {
            return extensions;
        }
        else {
            throw std::runtime_error{ getError("vkEnumerateInstanceExtensionsProperties() failed", result) };
        }
    }
    else {
        throw std::runtime_error{ getError("vkEnumerateInstanceExtensionsProperties() failed", result) };
    }
}

void shw::printInstanceExtensions() {
    std::vector<VkExtensionProperties> extensions{ getInstanceExtensions() };
    const std::vector<std::string> header{ "Name", "Spec Version" };
    std::cout << "Instance extensions:\n";
    printTable(header, extensions);
}

void shw::printInstanceExtensionsSupport(const std::vector<std::string>& extensions) {
    auto availableExtensions{ getInstanceExtensions() };
    std::unordered_set<std::string> avExts;
    avExts.reserve(availableExtensions.size());
    std::transform(availableExtensions.cbegin(), availableExtensions.cend(),
        std::inserter(avExts, avExts.begin()),
        [](const auto& extension) { return extension.extensionName;  });
    std::vector<std::vector<std::string>> rows;
    rows.reserve(extensions.size());
    for (const auto& ext : extensions) {
        std::string isAvailable{ avExts.find(ext) != avExts.end() ? "YES" : "NO" };
        rows.push_back({ ext, isAvailable });
    }
    std::cout << "Instance extensions supported:\n";
    const std::vector<std::string> header{ "Name", "Supported" };
    printTable(header, rows);
}

void shw::printTable(const std::vector<std::string>& header, const std::vector<VkLayerProperties>& layers) {
    constexpr std::size_t numLayerProperties{ 4 };
    if (header.size() != numLayerProperties) {
        throw std::runtime_error{ "Invalid number of columns in header! VkLayerProperties has 4 data members." };
    }
    const std::size_t offsetWidth{ 5 };
    std::size_t nameWidth{ header[0].size() };
    std::size_t specVersionWidth{ header[1].size() };
    std::size_t impVersionWidth{ header[2].size() };
    std::size_t descriptionWidth{ header[3].size() };
    for (const auto& layer : layers) {
        nameWidth = std::max(nameWidth, std::strlen(layer.layerName));
        specVersionWidth = std::max(specVersionWidth, std::to_string(layer.specVersion).size());
        impVersionWidth = std::max(impVersionWidth, std::to_string(layer.implementationVersion).size());
        descriptionWidth = std::max(descriptionWidth, std::strlen(layer.description));
    }
    auto printRow = [=](const auto& name, const auto& specVersion, const auto& impVersion, const auto& description) {
        std::cout << '\t' << std::left << std::setw(nameWidth) << name << std::setw(offsetWidth) << " "
            << std::right << std::setw(specVersionWidth) << specVersion << std::setw(offsetWidth) << " "
            << std::setw(impVersionWidth) << impVersion << std::setw(offsetWidth) << " "
            << std::left << std::setw(descriptionWidth) << description << '\n';
    };
    printRow(header[0], header[1], header[2], header[3]);
    for (const auto& layer : layers) {
        printRow(layer.layerName, layer.specVersion, layer.implementationVersion, layer.description);
    }
}

std::vector<VkLayerProperties> shw::getInstanceLayers() {
    std::uint32_t count{};
    VkResult result{ vkEnumerateInstanceLayerProperties(&count, nullptr) };
    if (result == VK_SUCCESS) {
        std::vector<VkLayerProperties> layers(count);
        result = vkEnumerateInstanceLayerProperties(&count, layers.data());
        if (result == VK_SUCCESS || result == VK_INCOMPLETE) {
            return layers;
        }
        else {
            throw std::runtime_error{ getError("vkEnumerateInstanceLayerProperties() failed", result) };
        }
    }
    else {
        throw std::runtime_error{ getError("vkEnumerateInstanceLayerProperties() failed", result) };
    }
}

void shw::printInstanceLayers() {
    std::vector<VkLayerProperties> layers{getInstanceLayers()};
    const std::vector<std::string> header{"Name", "Spec Version", "Implementatnion Version", "Description"};
    std::cout << "Instance layers:\n";
    printTable(header, layers);
}

void shw::printInstanceLayersSupport(const std::vector<std::string>& layers) {
    auto availableLayers{getInstanceLayers()};
    std::unordered_set<std::string> avLayers;
    avLayers.reserve(availableLayers.size());
    std::transform(availableLayers.cbegin(), availableLayers.cend(),
        std::inserter(avLayers, avLayers.begin()),
        [](const auto& layer) { return layer.layerName;  });
    std::vector<std::vector<std::string>> rows;
    rows.reserve(layers.size());
    for (const auto& layer : layers) {
        std::string isAvailable{ avLayers.find(layer) != avLayers.end() ? "YES" : "NO" };
        rows.push_back({ layer, isAvailable });
    }
    std::cout << "Instance layers supported:\n";
    const std::vector<std::string> header{"Name", "Supported"};
    printTable(header, rows);
}

const std::string shw::instanceAllOption{"--instance-all"};
const std::string shw::instanceVersionOption{"--instance-version"};
const std::string shw::instanceShowExtensionsOption{"--instance-extensions"};
const std::string shw::instanceShowLayersOption{"--instance-layers"};
const std::string shw::instanceExtensionsSupportOption{"--instance-support-extensions"};
const std::string shw::instanceLayersSupportOption{"--instance-support-layers"};
const std::string shw::instanceKeyValueOptionDelim{"="};
const std::string shw::instanceValuesOptionDelim{","};
