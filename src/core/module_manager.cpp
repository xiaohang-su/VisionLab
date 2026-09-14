#include "module_manager.h"

#include "../logging/logger.h"

#include <string>

namespace visionlab::core {

ModuleManager::ModuleManager(logging::Logger* logger)
    : logger_(logger)
{
}

void ModuleManager::add(Module* module) {
    if (module != nullptr) {
        modules_.push_back(module);
    }
}

bool ModuleManager::initialize_all() {
    for (Module* module : modules_) {
        if (!module->initialize()) {
            if (logger_ != nullptr) {
                logger_->error(
                    "Module '" + std::string(module->name()) + "' failed to initialize."
                );
            }
            return false;
        }
    }
    return true;
}

bool ModuleManager::start_all() {
    for (Module* module : modules_) {
        if (!module->start()) {
            if (logger_ != nullptr) {
                logger_->error(
                    "Module '" + std::string(module->name()) + "' failed to start."
                );
            }
            return false;
        }
    }
    return true;
}

bool ModuleManager::stop_all() {
    bool all_success = true;
    
    // Stop in reverse order (LIFO)
    for (int i = static_cast<int>(modules_.size()) - 1; i >= 0; --i) {
        if (!modules_[i]->stop()) {
            if (logger_ != nullptr) {
                logger_->error(
                    "Module '" + std::string(modules_[i]->name()) + "' failed to stop."
                );
            }
            all_success = false;
        }
    }
    
    return all_success;
}

}
