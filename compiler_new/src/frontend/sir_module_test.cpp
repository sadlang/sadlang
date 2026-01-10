// ======================================================================
// sir_module_test.cpp - اختبار SIR Module
// ======================================================================

#include "frontend/sir_types.h"
#include "frontend/sir_module.h"
#include <string>
#include <sstream>

namespace Sad {
namespace Compiler {
namespace SIR {

std::string SIRParameter::toString() const {
    std::ostringstream oss;
    oss << sirTypeToString(type) << " %" << name;
    return oss.str();
}

}
}
}
