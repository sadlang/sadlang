// compiler_driver_ui.cpp — Stub (UI Pipeline not available)
#include "compiler_driver.h"
#include <iostream>

namespace sad {
namespace driver {

bool CompilerDriver::run_ui_pipeline(const std::string& input_file,
                                      const std::string& ui_platform_str) {
    (void)input_file;
    (void)ui_platform_str;
    std::cerr << "[sadc] UI pipeline not available in this build." << std::endl;
    return false;
}

} // namespace driver
} // namespace sad
