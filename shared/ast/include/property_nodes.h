// ======================================================================
// property_nodes.h - عقد الخصائص / Property Nodes
// ======================================================================
// الوصف بالعربية:
//   تعريف عقد AST للخصائص (Properties) مع الـ Getters والـ Setters
//   يستخدم في Phase 6.3 لدعم خصائص الكائنات
//
// English Description:
//   Definition of AST nodes for Properties with Getters and Setters
//   Used in Phase 6.3 to support object properties
// ======================================================================

#pragma once

#include "ast_node.h"
#include "ast_visitor.h"
#include "statements.h"
#include "declarations.h"
#include "value.h"
#include <memory>
#include <string>

namespace Sad {
namespace AST {

// Forward declarations
class ASTVisitor;
class BlockStmt;

// ======================================================================
// Getter Block - كتلة القراءة
// ======================================================================
/**
 * @class GetterBlock
 * @brief (AR) كتلة getter للخاصية - تُنفذ عند قراءة قيمة الخاصية
 * @brief (EN) Getter block for property - executes when property value is read
 * 
 * مثال:
 * احصل
 *     ارجع _الحقل
 * نهاية
 */
class GetterBlock {
public:
    std::unique_ptr<BlockStmt> body;  ///< (AR) جسم الـ getter / (EN) getter body
    
    /**
     * @brief (AR) منشئ مع جسم الكتلة
     * @brief (EN) Constructor with block body
     */
    explicit GetterBlock(std::unique_ptr<BlockStmt> blockBody)
        : body(std::move(blockBody)) {}
    
    /**
     * @brief (AR) تحويل إلى نص للطباعة
     * @brief (EN) Convert to string for printing
     */
    std::string toString() const {
        return "get { ... }";
    }
};

// ======================================================================
// Setter Block - كتلة الكتابة
// ======================================================================
/**
 * @class SetterBlock
 * @brief (AR) كتلة setter للخاصية - تُنفذ عند كتابة قيمة للخاصية
 * @brief (EN) Setter block for property - executes when property value is written
 * 
 * مثال:
 * عيّن(نص قيمة)
 *     _الحقل = قيمة
 * نهاية
 */
class SetterBlock {
public:
    std::string parameterName;         ///< (AR) اسم المعامل / (EN) parameter name
    Data::DataType parameterType;      ///< (AR) نوع المعامل / (EN) parameter type
    std::unique_ptr<BlockStmt> body;   ///< (AR) جسم الـ setter / (EN) setter body
    
    /**
     * @brief (AR) منشئ مع اسم المعامل والجسم
     * @brief (EN) Constructor with parameter name and body
     */
    SetterBlock(const std::string& paramName, Data::DataType paramType, 
                std::unique_ptr<BlockStmt> blockBody)
        : parameterName(paramName), parameterType(paramType), 
          body(std::move(blockBody)) {}
    
    /**
     * @brief (AR) تحويل إلى نص للطباعة
     * @brief (EN) Convert to string for printing
     */
    std::string toString() const {
        return "set(" + parameterName + ") { ... }";
    }
};

// ======================================================================
// Property Declaration - تصريح الخاصية
// ======================================================================
/**
 * @class PropertyDecl
 * @brief (AR) تصريح خاصية في الصنف
 * @brief (EN) Property declaration in a class
 * 
 * مثال كامل:
 * خاصية نص الاسم
 *     احصل
 *         ارجع _الاسم
 *     نهاية
 *     عيّن(نص قيمة)
 *         _الاسم = قيمة
 *     نهاية
 * نهاية
 */
class PropertyDecl : public Statement {
public:
    std::string name;                         ///< (AR) اسم الخاصية / (EN) property name
    Data::DataType type;                      ///< (AR) نوع الخاصية / (EN) property type
    std::unique_ptr<GetterBlock> getter;      ///< (AR) كتلة القراءة / (EN) getter block
    std::unique_ptr<SetterBlock> setter;      ///< (AR) كتلة الكتابة / (EN) setter block (optional)
    AccessModifier visibility;                ///< (AR) مستوى الوصول / (EN) access level
    bool isStatic;                            ///< (AR) هل خاصية ثابتة؟ / (EN) is static property?
    
    /**
     * @brief (AR) منشئ للخاصية
     * @brief (EN) Constructor for property
     * 
     * @param propName (std::string) — (AR) اسم الخاصية / (EN) property name
     * @param propType (Data::DataType) — (AR) نوع الخاصية / (EN) property type
     * @param getterBlock (std::unique_ptr<GetterBlock>) — (AR) كتلة القراءة / (EN) getter block
     * @param setterBlock (std::unique_ptr<SetterBlock>) — (AR) كتلة الكتابة / (EN) setter block (can be null)
     * @param vis (AccessModifier) — (AR) مستوى الوصول / (EN) access level
     * @param isStaticProp (bool) — (AR) هل ثابتة / (EN) is static
     */
    PropertyDecl(const std::string& propName, 
                 Data::DataType propType,
                 std::unique_ptr<GetterBlock> getterBlock,
                 std::unique_ptr<SetterBlock> setterBlock,
                 AccessModifier vis = AccessModifier::PUBLIC,
                 bool isStaticProp = false)
        : name(propName), type(propType), 
          getter(std::move(getterBlock)), setter(std::move(setterBlock)),
          visibility(vis), isStatic(isStaticProp) {}
    
    /**
     * @brief (AR) هل الخاصية للقراءة فقط؟ (بدون setter)
     * @brief (EN) Is the property read-only? (no setter)
     */
    bool isReadOnly() const {
        return getter && !setter;
    }
    
    /**
     * @brief (AR) هل الخاصية للكتابة فقط؟ (بدون getter)
     * @brief (EN) Is the property write-only? (no getter)
     */
    bool isWriteOnly() const {
        return !getter && setter;
    }
    
    /**
     * @brief (AR) قبول الزائر
     * @brief (EN) Accept visitor
     */
    void accept(ASTVisitor& visitor) override;
    
    /**
     * @brief (AR) تحويل إلى نص للطباعة
     * @brief (EN) Convert to string for printing
     */
    std::string toString() const override {
        std::string result;
        
        // Access modifier
        if (visibility == AccessModifier::PRIVATE) {
            result += "private ";
        } else if (visibility == AccessModifier::PROTECTED) {
            result += "protected ";
        } else {
            result += "public ";
        }
        
        // Static
        if (isStatic) {
            result += "static ";
        }
        
        // Property declaration
        result += "property " + name;
        
        // Read-only/Write-only indicator
        if (isReadOnly()) {
            result += " (read-only)";
        } else if (isWriteOnly()) {
            result += " (write-only)";
        }
        
        return result;
    }
};

} // namespace AST
} // namespace Sad
