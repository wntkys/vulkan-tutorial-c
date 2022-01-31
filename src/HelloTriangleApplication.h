#ifndef __HELLO_TRIANGLE_APPLICATION__
#define __HELLO_TRIANGLE_APPLICATION__

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdio.h>

#ifndef bool
    #include <stdbool.h>
#endif

#define debug_entry(x) puts(":::: "x" ::::")
#define debug_method(x) puts("\t>>>> "x)

typedef struct {
    int width;
    int height;
    const char *title;
    const char *applicationName;
    const char *engineName;
} AppWindowProperties;

typedef struct {
    bool isSet;
    uint32_t value;
} FamilyIndex;

typedef struct {
    bool error;
    FamilyIndex graphicsFamily;
} QueueFamilyIndices;

// public
void HTA_Run( void );

// private
void HTA_InitWindow( void );
void HTA_MainLoop( void );
void HTA_Cleanup( void );

void HTA_ClearFeatures( VkPhysicalDeviceFeatures* );

bool HTA_IsDeviceSuitable( VkPhysicalDevice );

VkResult HTA_InitVulkan( void );
VkResult HTA_CreateVulkanInstance( void );
VkResult HTA_PickPhysicalDevice( void );
VkResult HTA_CreateLogicalDevice( void );

QueueFamilyIndices HTA_FindQueueFamilies( VkPhysicalDevice );

#endif
