#include "module_manager.h"

namespace visionlab::core {

void ModuleManager::add(Module* module) {
    if (module != nullptr) {
        modules_.push_back(module);
    }
}

bool ModuleManager::initialize_all() {
    for (Module* module : modules_) {
        if (!module->initialize()) {
            return false;
        }
    }
    return true;
}

bool ModuleManager::start_all() {
    for (Module* module : modules_) {
        if (!module->start()) {
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
            all_success = false;
        }
    }
    
    return all_success;
}

}
