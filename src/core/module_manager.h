#pragma once

#include "module.h"
#include <vector>

namespace visionlab::core {

/**
 * @class ModuleManager
 * @brief Manages the lifecycle of modules.
 * 
 * Coordinates module initialization, startup, and shutdown.
 * Stores non-owning pointers to Module instances.
 */
class ModuleManager {
public:
    ModuleManager() = default;
    ~ModuleManager() = default;

    /**
     * @brief Add a module to the manager.
     * @param module Non-owning pointer to a Module instance.
     *               If nullptr is passed, it is ignored.
     * 
     * Modules are stored in the order they are added.
     */
    void add(Module* module);

    /**
     * @brief Initialize all modules in order.
     * @return true if all modules initialize successfully, false otherwise.
     * 
     * Initialization occurs in the order modules were added.
     * If any module fails, returns false immediately.
     */
    bool initialize_all();

    /**
     * @brief Start all modules in order.
     * @return true if all modules start successfully, false otherwise.
     * 
     * Startup occurs in the order modules were added.
     * If any module fails, returns false immediately.
     */
    bool start_all();

    /**
     * @brief Stop all modules in reverse order.
     * @return true if all modules stop successfully, false otherwise.
     * 
     * Shutdown occurs in reverse order (LIFO) for proper cleanup.
     * Attempts to stop all modules even if some fail.
     */
    bool stop_all();

private:
    std::vector<Module*> modules_;
};

}
