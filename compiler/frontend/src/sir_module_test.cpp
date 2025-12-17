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
