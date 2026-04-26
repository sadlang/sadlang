#ifndef SAD_COMPILER_TYPES_GENERATOR_TYPE_HPP
#define SAD_COMPILER_TYPES_GENERATOR_TYPE_HPP

#include "type.h"
#include <memory>
#include <string>

namespace Sad {
namespace TypeSystem {

// Generator state
enum class GeneratorState {
    Created,
    Running,
    Suspended,
    Completed
};

// Generator type for lazy sequences
class GeneratorType : public Type {
public:
    GeneratorType(TypePtr yieldType);
    virtual ~GeneratorType() = default;
    
    // Override Type methods
    std::string getArabicName() const override { return "مولد"; }
    std::string getEnglishName() const override { return "Generator"; }
    std::string toString() const override;
    bool equals(const Type* other) const override;
    std::shared_ptr<Type> clone() const override;
    
    // Generator-specific methods
    TypePtr getYieldType() const;
    GeneratorState getState() const;
    void setState(GeneratorState newState);
    
private:
    TypePtr yieldType_;
    GeneratorState state_ = GeneratorState::Created;
};

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPES_GENERATOR_TYPE_HPP
