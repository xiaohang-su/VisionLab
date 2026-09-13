#include <iostream>
#include "runtime/runtime.h"
#include "core/core_types.h"

int main() {
    std::cout << "VisionLab V0.2" << std::endl;

    visionlab::runtime::Runtime runtime;

    if (!runtime.initialize()) {
        std::cerr << "Failed to initialize runtime" << std::endl;
        return 1;
    }
    std::cout << "Runtime initialized." << std::endl;

    if (!runtime.start()) {
        std::cerr << "Failed to start runtime" << std::endl;
        return 1;
    }
    std::cout << "Runtime started." << std::endl;

    if (!runtime.stop()) {
        std::cerr << "Failed to stop runtime" << std::endl;
        return 1;
    }
    std::cout << "Runtime stopped." << std::endl;

    return 0;
}
