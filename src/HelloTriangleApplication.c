#include "HelloTriangleApplication.h"

GLFWwindow *HTA_window;
AppProperties HTA_props = { 400, 400, "App Window", "Hello Triangle", "Test Engine" };
VkInstance HTA_vkInstance;
VkPhysicalDevice HTA_vkPhysicalDevice = VK_NULL_HANDLE;
VkDevice HTA_vkDevice;
VkQueue HTA_graphicsQueue;

void HTA_Run( void ) {
    debug_entry( "HTA_Run" );

    HTA_InitWindow();

    VkResult result = HTA_InitVulkan();
    if ( result != VK_SUCCESS ) {
        printf( "Vulkan Error code: %d\n", result );
    } else {
        HTA_MainLoop();
    }

    HTA_Cleanup();
}

void HTA_InitWindow() {
    debug_entry( "HTA_InitWindow" );

    glfwInit();

    glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
    glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );

    HTA_window = glfwCreateWindow( HTA_props.width, HTA_props.height, HTA_props.title, NULL, NULL );
}

void HTA_MainLoop() {
    debug_entry( "HTA_MainLoop" );

    while ( !glfwWindowShouldClose( HTA_window ) ) {
        glfwPollEvents();
    }
}

void HTA_Cleanup() {
    debug_entry( "HTA_Cleanup" );

    vkDestroyDevice( HTA_vkDevice, NULL );
    vkDestroyInstance( HTA_vkInstance, NULL );
    glfwDestroyWindow( HTA_window );
    glfwTerminate();
}

VkResult HTA_InitVulkan() {
    debug_entry( "HTA_InitVulkan" );

    VkResult result = HTA_CreateVulkanInstance();
    if ( result != VK_SUCCESS ) {
        puts( "Error: failed to create vulkan instance!" );
        return result;
    }

    result = HTA_PickPhysicalDevice();
    if ( result != VK_SUCCESS ) {
        puts( "Error: failed to pick physical device!" );
        return result;
    }

    result = HTA_CreateLogicalDevice();
    if ( result != VK_SUCCESS ) {
        puts( "Error: failed to create logical device!" );
        return result;
    }

    return result;
}

VkResult HTA_CreateVulkanInstance() {
    debug_entry( "HTA_CreateVulkanInstance" );

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = NULL,
        .pApplicationName = HTA_props.applicationName,
        .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
        .pEngineName = HTA_props.engineName,
        .engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
        .apiVersion = VK_API_VERSION_1_0
    };

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = NULL,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL
    };

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    puts( "glfwExtensions:" );
    for ( int i = 0; i < glfwExtensionCount; i++ )
        printf( "\t\"%s\"\n", glfwExtensions[ i ] );

    uint32_t vkExtCount = 0;
    vkEnumerateInstanceExtensionProperties( NULL, &vkExtCount, NULL );
    VkExtensionProperties vkExtensions[ vkExtCount ];
    vkEnumerateInstanceExtensionProperties( NULL, &vkExtCount, vkExtensions );
    puts( "vkInstanceExtensions:" );
    for ( int i = 0; i < vkExtCount; i++ ) {
        printf( "\t\"%s\" (v%u.0)\n", 
            vkExtensions[ i ].extensionName, 
            vkExtensions[ i ].specVersion 
        );
    }

    return vkCreateInstance( &createInfo, NULL, &HTA_vkInstance );
}

VkResult HTA_PickPhysicalDevice() {
    debug_entry( "HTA_PickPhysicalDevice" );

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices( HTA_vkInstance, &deviceCount, NULL );
    if ( deviceCount == 0 ) {
        puts( "Error: No Graphical Devices was found!" );
        return VK_ERROR_DEVICE_LOST;
    }
    VkPhysicalDevice devices[ deviceCount ];
    vkEnumeratePhysicalDevices( HTA_vkInstance, &deviceCount, devices );
    puts( "devices:" );

    bool isNotSupported = true;
    for ( int i = 0; i < deviceCount; i++ ) {
        bool isSupported = HTA_IsDeviceSuitable( devices[ i ] );
        if ( isSupported ) {
            HTA_vkPhysicalDevice = devices[ i ];
            isNotSupported = false;
        }
    }

    if ( isNotSupported ) {
        puts( "Error: no supported Graphical Devices was found!" );
        return VK_ERROR_DEVICE_LOST;
    }

    return VK_SUCCESS;
}

VkResult HTA_CreateLogicalDevice() {
    debug_entry( "HTA_CreateLogicalDevice" );

    QueueFamilyIndices indices = HTA_FindQueueFamilies( HTA_vkPhysicalDevice );
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = NULL,
        .queueFamilyIndex = indices.graphicsFamily.value,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    VkPhysicalDeviceFeatures deviceFeatures;
    HTA_ClearFeatures( &deviceFeatures );
    
    VkDeviceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = NULL,
        .pEnabledFeatures = &deviceFeatures,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueCreateInfo,
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = NULL,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = NULL
    };

    VkResult result = vkCreateDevice( HTA_vkPhysicalDevice, &createInfo, NULL, &HTA_vkDevice );
    if ( result != VK_SUCCESS ) return result;

    vkGetDeviceQueue( HTA_vkDevice, indices.graphicsFamily.value, 0, &HTA_graphicsQueue );

    return VK_SUCCESS;
}

void HTA_ClearFeatures( VkPhysicalDeviceFeatures *pFeatures ) {
    int length = sizeof( VkPhysicalDeviceFeatures ) / sizeof( VkBool32 );
    VkBool32 *p = ( VkBool32* )pFeatures;
    for ( int i = 0; i < length; i++, p++ ) *p = 0;
}

void HTA_GetDriverVersion( char *output_str, uint32_t vendor_id, uint32_t driver_version ) {
    // NVIDIA version conventions
    if ( vendor_id == 0x10DE ) {
        sprintf( output_str, "%d.%d.%d.%d",
            ( driver_version >> 22 ) & 0x03FF,
            ( driver_version >> 14 ) & 0x00FF,
            ( driver_version >> 6  ) & 0x00FF,
            ( driver_version >> 0  ) & 0x003F
		);
        return;
    }

    // Intel version conventions
    if ( vendor_id == 0x8086 ) {
        sprintf( output_str, "%d.%d",
            ( driver_version >> 14 ),
            ( driver_version >> 0  ) & 0x3FFF
        );
        return;
    }

    // Vulkan version conventions
    sprintf( output_str, "%d.%d.%d",
        ( driver_version >> 22 ),
        ( driver_version >> 12 ) & 0x03FF,
        ( driver_version >> 0  ) & 0x0FFF
    );
}

bool HTA_IsDeviceSuitable( VkPhysicalDevice device ) {
    debug_method( "HTA_IsDeviceSuitable" );

    bool isSupported = false;

    QueueFamilyIndices indices = HTA_FindQueueFamilies( device );

    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties( device, &deviceProperties );
    vkGetPhysicalDeviceFeatures( device, &deviceFeatures );

    isSupported = (
        deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
        deviceFeatures.geometryShader &&
        !indices.error &&
        indices.graphicsFamily.isSet
    );

    char driverVersion[ 64 ];
    HTA_GetDriverVersion( driverVersion, deviceProperties.vendorID, deviceProperties.driverVersion );

    printf( 
        "%s\n"
        "\t%s (Device ID: %u) (Driver version: %s)\n"
        "\tDiscrete: %s\n"
        "\tGeometry Shader support: %s\n",
        isSupported ? "[ OK ]" : "[ ERROR ]",
        deviceProperties.deviceName,
        deviceProperties.deviceID,
        driverVersion,
        deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "Yes" : "No",
        deviceFeatures.geometryShader ? "Yes" : "No"
    );

    return isSupported;
}

QueueFamilyIndices HTA_FindQueueFamilies( VkPhysicalDevice device ) {
    debug_method( "HTA_FindQueueFamilies" );

    QueueFamilyIndices indices = {
        .error = true,
        .graphicsFamily = { 
            .isSet = false,
            .value = 0
        }
    };

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, NULL );
    VkQueueFamilyProperties queueFamilyProperties[ queueFamilyCount ];
    vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilyProperties );

    for ( int i = 0; i < queueFamilyCount; i++ ) {
        if ( !queueFamilyProperties[ i ].queueFlags & VK_QUEUE_GRAPHICS_BIT ) continue;

        indices.error = false;
        indices.graphicsFamily.isSet = true;
        indices.graphicsFamily.value = i;
    }

    return indices;
}
