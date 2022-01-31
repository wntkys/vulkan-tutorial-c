CFLAGS = -g -m64 -O2 -std=c11
OUTPUT = -o bin/vulkan-test.exe

INPUT = \
	main.c \
	src/*.c

INCLUDE = \
	-Iinclude \
	-Ic:/VulkanSDK/1.2.198.1/Include

LDFLAGS = \
	-Llib \
	-DGLFW_DLL -lglfw3dll \
	-LC:/VulkanSDK/1.2.198.1/Lib -lvulkan-1 \
	-LC:/MinGW/lib -lpthread

all: main.c
	gcc \
    $(CFLAGS) \
    $(OUTPUT) \
    $(INCLUDE) \
    $(INPUT) \
    $(LDFLAGS)

run:
	./bin/vulkan-test.exe