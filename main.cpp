//#include "helloTriangle2.hpp"
#include "HelloTriangle.hpp"

int main() {
    HelloTriangleApplication app;
    try {
        app.Run();
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}