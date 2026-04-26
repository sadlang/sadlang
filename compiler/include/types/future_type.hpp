#ifndef SAD_COMPILER_TYPES_FUTURE_TYPE_HPP
#define SAD_COMPILER_TYPES_FUTURE_TYPE_HPP

#include "type.h"
#include <memory>
#include <string>
#include <functional>

namespace Sad {
namespace TypeSystem {

// Future state
enum class FutureState {
    Pending,
    Ready,
    Error
};

// Future type for async programming
class FutureType : public Type {
public:
    FutureType(TypePtr valueType);
    virtual ~FutureType() = default;
    
    // Override Type methods
    std::string getArabicName() const override { return "مستقبل"; }
    std::string getEnglishName() const override { return "Future"; }
    std::string toString() const override;
    bool equals(const Type* other) const override;
    std::shared_ptr<Type> clone() const override;
    
    // Future-specific methods
    TypePtr getValueType() const;
    FutureState getState() const;
    void setState(FutureState newState);
    bool isConvertibleTo(const Type& target) const;
    bool isReady() const;
    bool isError() const;
    bool isPending() const;
    
private:
    TypePtr valueType_;
    FutureState state_ = FutureState::Pending;
};

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPES_FUTURE_TYPE_HPP
