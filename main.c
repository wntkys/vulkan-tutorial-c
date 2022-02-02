#include "src/HelloTriangleApplication.h"



int main() {
    AppProperties props = { 400, 400, "App Window", "Hello Triangle", "Test Engine" };
    HTA_Run(props);

    return 0;
}
