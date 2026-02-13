#ifndef SAD_COMPILER_TYPES_COMPREHENSION_TYPES_HPP
#define SAD_COMPILER_TYPES_COMPREHENSION_TYPES_HPP

#include "type.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace TypeSystem {

// Comprehension operation type
enum class ComprehensionOp {
    Map,
    Filter,
    FlatMap,
    Zip
};

// List/Set/Dict comprehension type
class ComprehensionType : public Type {
public:
    ComprehensionType(TypePtr elementType, ComprehensionOp op);
    virtual ~ComprehensionType() = default;
    
    // Override Type methods
    std::string getArabicName() const override { return "تعبير توليدي"; }
    std::string getEnglishName() const override { return "Comprehension"; }
    std::string toString() const override;
    bool equals(const Type* other) const override;
    std::shared_ptr<Type> clone() const override;
    
    // Comprehension-specific methods
    TypePtr getElementType() const;
    ComprehensionOp getOperation() const;
    bool isConvertibleTo(const Type& target) const;
    
private:
    TypePtr elementType_;
    ComprehensionOp operation_;
};

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPES_COMPREHENSION_TYPES_HPP
