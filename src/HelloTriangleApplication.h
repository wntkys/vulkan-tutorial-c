#ifndef __HELLO_TRIANGLE_APPLICATION__
#define __HELLO_TRIANGLE_APPLICATION__

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
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
} AppProperties;

typedef struct {
    bool isSet;
    uint32_t value;
} FamilyIndex;

typedef struct {
    bool error;
    FamilyIndex graphicsFamily;
} QueueFamilyIndices;

// public
void HTA_Run( AppProperties );

// private
void HTA_InitWindow( void );
void HTA_MainLoop( void );
void HTA_Cleanup( void );


VkResult HTA_InitVulkan( void );
VkResult HTA_CreateVulkanInstance( void );
VkResult HTA_PickPhysicalDevice( void );
VkResult HTA_CreateLogicalDevice( void );
VkResult HTA_CreateSurface( void );

void HTA_ClearFeatures( VkPhysicalDeviceFeatures* );
void HTA_GetDriverVersion( char*, uint32_t, uint32_t );
bool HTA_IsDeviceSuitable( VkPhysicalDevice );
QueueFamilyIndices HTA_FindQueueFamilies( VkPhysicalDevice );

#endif
