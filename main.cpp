#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <algorithm>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 800;

#ifdef NDEBUG
    const bool ENABLE_VALIDATION_LAYERS = false;
#else
    const bool ENABLE_VALIDATION_LAYERS = true;
#endif

const uint32_t DEVICE_EXTENSION_COUNT = 1;
const char* DEVICE_EXTENSIONS[DEVICE_EXTENSION_COUNT] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices{
    uint32_t graphics = UINT32_MAX;
    uint32_t present = UINT32_MAX;
};

VkInstance createVkInstance(){
    VkInstance instance{};

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Moebius";
    appInfo.applicationVersion = VK_MAKE_VERSION(0,1,0);//Update
    appInfo.pEngineName = "Moebius";
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    if(ENABLE_VALIDATION_LAYERS){
        char* validationLayers[1] = {"VK_LAYER_KHRONOS_validation"};//Update
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = validationLayers;
    }
    else{
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        printf("Vulkan instance creation failed!");
        exit(EXIT_FAILURE);
    }

    return instance;
}

VkPhysicalDevice pickVkPhysicalDevice(VkInstance instance){
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if(deviceCount == 0){
        printf("No Vulkan compatible devices found!\n");
        exit(EXIT_FAILURE);
    }
    deviceCount = 1;//Pick the first one
    vkEnumeratePhysicalDevices(instance, &deviceCount, &physicalDevice);
    return physicalDevice;
}

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface){
    QueueFamilyIndices indices{};
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    VkQueueFamilyProperties* queueFamilies = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties)*queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies);

    for(int i = 0; i < queueFamilyCount; i++){
        if(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
            indices.graphics = i;
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
            if(!presentSupport){
                printf("Present Queue not the same as the Graphics Queue!");
                exit(EXIT_FAILURE);
            }
            else{
                indices.present = indices.graphics;
            }
            break;
        }
    }

    if(indices.graphics == UINT32_MAX){
        printf("No Graphics Queue Family found!\n");
        exit(EXIT_FAILURE);
    }

    return indices;
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, QueueFamilyIndices *indices){
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices->graphics;
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    VkPhysicalDeviceFeatures deviceFeatures{};
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = DEVICE_EXTENSION_COUNT;
    createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS;

    VkDevice device;
    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS){
        printf("Failed to create Logical Device!\n");
        exit(EXIT_FAILURE);
    }

    return device;
}

struct SwapChainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupport querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface){
    SwapChainSupport support;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &support.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
    if(formatCount > 0){
        support.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, support.formats.data());
    }
    else{
        printf("No Surface Formats!\n");
        exit(EXIT_FAILURE);
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
    if(presentModeCount > 0){
        support.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, support.presentModes.data());
    }
    else{
        printf("No Surface Present Modes!\n");
        exit(EXIT_FAILURE);
    }

    return support;
}

VkSurfaceFormatKHR selectSurfaceFormat(SwapChainSupport *support){
    for(const VkSurfaceFormatKHR& availableFormat: support->formats){
        if(availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR){
            return availableFormat;
        }
    }

    printf("No suitable Surface Format found!\n");
    exit(EXIT_FAILURE);
}

VkPresentModeKHR selectPresentMode(SwapChainSupport *support){
    for(const VkPresentModeKHR& presentMode : support->presentModes){
        if(presentMode == VK_PRESENT_MODE_MAILBOX_KHR){
            return presentMode;
        }
    }

    printf("Mailbox Present Mode not available!\n");
    exit(EXIT_FAILURE);
}

VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR *capabilities, GLFWwindow *window){
    if(capabilities->currentExtent.width != UINT32_MAX){
        return capabilities->currentExtent;
    }
    else{
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        VkExtent2D actualExtent = {(uint32_t)width, (uint32_t)height};
        actualExtent.width = std::clamp(actualExtent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

        return actualExtent;
    }
}

VkSwapchainKHR createSwapChain(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, GLFWwindow *window, VkSurfaceCapabilitiesKHR *capabilities, VkSurfaceFormatKHR surfaceFormat, VkPresentModeKHR presentMode){
    VkExtent2D extent = selectSwapExtent(capabilities, window);

    uint32_t imageCount = capabilities->minImageCount + 1;
    if(capabilities->maxImageCount > 0 && imageCount > capabilities->maxImageCount){
        imageCount = capabilities->maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    //Assume Graphics and Present Queue Family are shared
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = nullptr; // Optional
    createInfo.preTransform = capabilities->currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkSwapchainKHR swapchain;
    if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain) != VK_SUCCESS){
        printf("Failed to create SwapChain!\n");
        exit(EXIT_FAILURE);
    }
    return swapchain;
}

VkImageView* createImageViews(VkDevice device, VkImage swapchainImages[], uint32_t swapchainImageCount, VkSurfaceFormatKHR surfaceFormat){
    VkImageView* imageViews = (VkImageView*)malloc(sizeof(VkImageView)*swapchainImageCount);

    for(int i = 0; i < swapchainImageCount; i++){
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapchainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = surfaceFormat.format;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if(vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]) != VK_SUCCESS){
            printf("Could create Image View %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    return imageViews;
}

int main(int, char**) {
    printf("Hello World!\n");

    if(!glfwInit()){
        printf("GLFW Init failed!\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Moebius", nullptr, nullptr);
    if(!window){
        printf("GLFW window creation failed!\n");
        exit(EXIT_FAILURE);
    }

    VkInstance instance = createVkInstance();
    VkSurfaceKHR surface;
    if(glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS){
        printf("Failed to create a Window Surface!");
        exit(EXIT_FAILURE);
    }
    VkPhysicalDevice physicalDevice = pickVkPhysicalDevice(instance);
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice, surface);
    VkDevice device = createLogicalDevice(physicalDevice, &queueFamilyIndices);
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, queueFamilyIndices.graphics, 0, &graphicsQueue);
    VkQueue presentQueue;
    vkGetDeviceQueue(device, queueFamilyIndices.present, 0, &presentQueue);
    SwapChainSupport swapChainSupport = querySwapChainSupport(physicalDevice, surface);
    VkSurfaceFormatKHR surfaceFormat = selectSurfaceFormat(&swapChainSupport);
    VkPresentModeKHR presentMode = selectPresentMode(&swapChainSupport);
    VkSwapchainKHR swapchain = createSwapChain(device, physicalDevice, surface, window, &(swapChainSupport.capabilities), surfaceFormat, presentMode);

    VkImage* swapchainImages;
    uint32_t swapchainImageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr);
    swapchainImages = (VkImage*)malloc(sizeof(VkImage)*swapchainImageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages);

    VkImageView* imageViews = createImageViews(device, swapchainImages, swapchainImageCount, surfaceFormat);

    printf("Successfully initialised Vulkan.\n");

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    free(swapchainImages);
    free(imageViews);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwTerminate();

    return 0;
}
