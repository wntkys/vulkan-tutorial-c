CFLAGS = -Ofast -s -m64 -std=c11
OUTPUT = -o bin/vulkan-test.exe

INPUT = \
	main.c \
	src/*.c

INCLUDE = \
	-Iinclude \
	-IC:/Sdks/VulkanSDK_1.2.198.1/Include
LDFLAGS = \
	-Llib \
	-DGLFW_DLL -lglfw3dll \
	-LC:/Sdks/VulkanSDK_1.2.198.1/Lib -lvulkan-1 #\
	#-LC:/Program\ Files/TDM-GCC/x86_64-w64-mingw32/lib -lpthread

all: main.c
	gcc \
    $(CFLAGS) \
    $(OUTPUT) \
    $(INCLUDE) \
    $(INPUT) \
    $(LDFLAGS)

run:
	./bin/vulkan-test.exe