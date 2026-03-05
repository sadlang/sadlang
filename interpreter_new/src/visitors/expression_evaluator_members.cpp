/**
 * @file expression_evaluator.cpp
 * @brief (AR) تنفيذ مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h"  // (AR) لضمان تعريف ASTNode الكامل عند استدعاء accept / (EN) Ensure full ASTNode definition for accept() calls
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h" // For AwaitExpr
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "exception.h"
#include "async_runtime.h"  // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>

namespace Sad {
namespace Interpreter {

using namespace Data;
using namespace AST;
using namespace Lexer;


// =========================================================================
// (AR) تقييم الوصول للعضو / (EN) Member Access Evaluation
// =========================================================================

void ExpressionEvaluator::visitMemberExpr(MemberExpr& node) {
    // تقييم الكائن
    node.object->accept(*this);
    Value objectValue = lastResult_;
    
    auto* classManager = Data::ClassManager::getInstance();
    
    // التحقق من الوصول الثابت: ClassName.staticField
    // Check for static access: ClassName.staticField
    if (objectValue.isString()) {
        std::string possibleClassName = objectValue.toString();
        ClassType* classType = classManager->getClass(possibleClassName);
        
        if (classType) {
            // هذا وصول ثابت: ClassName.staticField
            // This is static access: ClassName.staticField
            #ifdef DEBUG_OOP
            std::cout << "[OOP] الوصول لحقل ثابت: " << possibleClassName << "." << node.member << "\n";
#endif
            
            // البحث عن الحقل
            ClassField* field = classType->findField(node.member);
            if (!field) {
                std::string errMsg = "(AR) الحقل '" + node.member + "' غير موجود في الصنف '" + possibleClassName + "'. ";
                errMsg += "(EN) Field '" + node.member + "' not found in class '" + possibleClassName + "'.";
                throw RuntimeError(errMsg, node.position);
            }
            
            // التحقق من أن الحقل ثابت
            if (!field->isStatic) {
                std::string errMsg = "(AR) لا يمكن الوصول للحقل غير الثابت '" + node.member + "' من خلال اسم الصنف. ";
                errMsg += "(EN) Cannot access non-static field '" + node.member + "' through class name.";
                throw RuntimeError(errMsg, node.position);
            }
            
            // فحص الوصول
            checkMemberAccess(field->visibility, node.member, classType);
            
            // الحصول على قيمة الحقل الثابت
            Value* staticValue = classType->getStaticField(node.member);
            if (!staticValue) {
                std::string errMsg = "(AR) الحقل الثابت '" + node.member + "' غير مهيأ. ";
                errMsg += "(EN) Static field '" + node.member + "' not initialized.";
                throw RuntimeError(errMsg, node.position);
            }
            
            lastResult_ = *staticValue;
            #ifdef DEBUG_OOP
            std::cout << "[OOP] قيمة الحقل الثابت: " << lastResult_.toString() << "\n";
#endif
            return;
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) وصول عادي للكائن: كائن.حقل
    //      يدعم كلاً من نوع OBJECT الحقيقي ونوع MAP القديم
    //
    // (EN) Regular object access: object.field
    //      Supports both real OBJECT type and legacy MAP type
    // ═══════════════════════════════════════════════════════════════════
    
    std::string className;
    ClassType* classType = nullptr;
    Value::MapType fields;
    
    if (objectValue.isObject()) {
        // (AR) نوع OBJECT الحقيقي — الحصول على البيانات من ObjectInstance
        // (EN) Real OBJECT type — get data from ObjectInstance
        auto objPtr = objectValue.toObject();
        if (objPtr) {
            className = objPtr->getClassName();
            classType = classManager->getClass(className);
            fields = objPtr->fields;
            fields["__class__"] = Value(className);
        }
    } else if (objectValue.isMap()) {
        // (AR) نوع MAP — يدعم التعدادات والخرائط العامة والكائنات القديمة
        // (EN) MAP type — supports enums, generic maps, and legacy objects
        fields = objectValue.toMap();
        auto classNameIt = fields.find("__class__");
        if (classNameIt == fields.end()) {
            // (AR) خريطة بدون صنف — وصول مباشر للمفتاح (تعداد أو خريطة عامة)
            // (EN) Map without class — direct key access (enum or generic map)
            auto it = fields.find(node.member);
            if (it != fields.end()) {
                lastResult_ = it->second;
                return;
            }
            std::string errMsg = "(AR) المفتاح '" + node.member + "' غير موجود في الخريطة. ";
            errMsg += "(EN) Key '" + node.member + "' not found in map.";
            throw RuntimeError(errMsg, node.position);
        }
        className = classNameIt->second.toString();
        classType = classManager->getClass(className);
    } else {
        std::string errMsg = "(AR) لا يمكن الوصول لعضو من قيمة ليست كائن. ";
        errMsg += "(EN) Cannot access member of non-object value.";
        throw RuntimeError(errMsg, node.position);
    }
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.", node.position);
    }
    
    // البحث عن الحقل في السلسلة الهرمية
    ClassField* field = classType->findField(node.member);
    
    // البحث عن خاصية (Property) إذا لم يُوجد حقل
    ClassProperty* property = nullptr;
    if (!field) {
        property = classType->findProperty(node.member);
    }
    
    if (!field && !property) {
        // (AR) لم يُعثر على حقل أو خاصية معلنة — جرب قراءة حقل ديناميكي
        // (EN) No declared field or property — try reading dynamic field
        auto it = fields.find(node.member);
        if (it != fields.end()) {
            lastResult_ = it->second;
            return;
        }
        std::string errMsg = "(AR) الحقل أو الخاصية '" + node.member + "' غير موجود في الكائن. ";
        errMsg += "(EN) Field or property '" + node.member + "' not found in object.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // إذا كانت خاصية، نفذ الـ getter
    if (property) {
        #ifdef DEBUG_OOP
        std::cout << "[OOP] تنفيذ getter للخاصية: " << node.member << "\n";
#endif
        
        // فحص الوصول
        checkMemberAccess(property->visibility, node.member, classType);
        
        // التحقق من وجود getter
        if (!property->getterBody) {
            std::string errMsg = "(AR) الخاصية '" + node.member + "' للكتابة فقط (لا يوجد getter). ";
            errMsg += "(EN) Property '" + node.member + "' is write-only (no getter).";
            throw RuntimeError(errMsg, node.position);
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) تنفيذ getter body في نطاق جديد مع سياق الكائن
        //      نربط 'هذا' بالكائن ونضيف الحقول للنطاق
        //      حتى يستطيع الـ getter الوصول لبيانات الكائن
        //
        // (EN) Execute getter body in new scope with object context
        //      Bind 'هذا' (this) to object and add fields to scope
        //      so getter can access object data
        // ═══════════════════════════════════════════════════════════════
        variableManager_.enterScope(Data::ScopeType::FUNCTION, "get_" + node.member);
        
        // (AR) ربط 'هذا' بالكائن / (EN) Bind 'this' to object
        variableManager_.define("هذا", objectValue);
        variableManager_.define("this", objectValue);
        
        // (AR) ربط حقول الكائن كمتغيرات محلية / (EN) Bind object fields as local variables
        for (const auto& [fname, fval] : fields) {
            if (fname != "__class__") {
                variableManager_.define(fname, fval);
            }
        }
        
        try {
            // (AR) تنفيذ getter body عبر StatementExecutor (لأن الجسم هو BlockStmt)
            // (EN) Execute getter body via StatementExecutor (because body is BlockStmt)
            property->getterBody->accept(statementExecutor_);
            
            if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
                lastResult_ = statementExecutor_.getReturnValue();
                statementExecutor_.resetFlowControl();
            }
        } catch (...) {
            variableManager_.exitScope();
            throw;
        }
        
        variableManager_.exitScope();
        
        #ifdef DEBUG_OOP
        
        std::cout << "[OOP] قيمة الخاصية: " << lastResult_.toString() << "\n";
#endif
        return;
    }
    
    // معالجة الحقل العادي
    // فحص الوصول (Phase 6.1: Access Modifiers)
    checkMemberAccess(field->visibility, node.member, classType);
    
    // البحث عن قيمة الحقل
    auto it = fields.find(node.member);
    if (it == fields.end()) {
        std::string errMsg = "(AR) الحقل '" + node.member + "' غير موجود في الكائن. ";
        errMsg += "(EN) Field '" + node.member + "' not found in object.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // إرجاع قيمة الحقل
    lastResult_ = it->second;
    #ifdef DEBUG_OOP
    std::cout << "[OOP] قيمة الحقل: " << lastResult_.toString() << "\n";
#endif
}

// =========================================================================
// (AR) الوصول لعضو — عقدة MemberAccessExpr من class_nodes.h
// (EN) Member access — MemberAccessExpr node from class_nodes.h
// =========================================================================

void ExpressionEvaluator::visitMemberAccessExpr(MemberAccessExpr& node) {
    // (AR) تقييم الكائن / (EN) Evaluate the object
    node.object->accept(*this);
    Value objectValue = lastResult_;
    
    auto* classManager = Data::ClassManager::getInstance();
    
    // (AR) التحقق من الوصول الثابت: ClassName.staticField
    // (EN) Check for static access: ClassName.staticField
    if (objectValue.isString()) {
        std::string possibleClassName = objectValue.toString();
        ClassType* classType = classManager->getClass(possibleClassName);
        
        if (classType) {
            #ifdef DEBUG_OOP
            std::cout << "[OOP] الوصول لحقل ثابت: " << possibleClassName << "." << node.memberName << "\n";
            #endif
            
            ClassField* field = classType->findField(node.memberName);
            if (!field) {
                std::string errMsg = "(AR) الحقل '" + node.memberName + "' غير موجود في الصنف '" + possibleClassName + "'. ";
                errMsg += "(EN) Field '" + node.memberName + "' not found in class '" + possibleClassName + "'.";
                throw RuntimeError(errMsg, node.position);
            }
            
            if (!field->isStatic) {
                std::string errMsg = "(AR) لا يمكن الوصول للحقل غير الثابت '" + node.memberName + "' من خلال اسم الصنف. ";
                errMsg += "(EN) Cannot access non-static field '" + node.memberName + "' through class name.";
                throw RuntimeError(errMsg, node.position);
            }
            
            checkMemberAccess(field->visibility, node.memberName, classType);
            
            Value* staticValue = classType->getStaticField(node.memberName);
            if (!staticValue) {
                std::string errMsg = "(AR) الحقل الثابت '" + node.memberName + "' غير مهيأ. ";
                errMsg += "(EN) Static field '" + node.memberName + "' not initialized.";
                throw RuntimeError(errMsg, node.position);
            }
            
            lastResult_ = *staticValue;
            return;
        }
    }
    
    // (AR) وصول عادي للكائن / (EN) Regular object access
    std::string className;
    ClassType* classType = nullptr;
    Value::MapType fields;
    
    if (objectValue.isObject()) {
        auto objPtr = objectValue.toObject();
        if (objPtr) {
            className = objPtr->getClassName();
            classType = classManager->getClass(className);
            fields = objPtr->fields;
            fields["__class__"] = Value(className);
        }
    } else if (objectValue.isMap()) {
        fields = objectValue.toMap();
        auto classNameIt = fields.find("__class__");
        if (classNameIt == fields.end()) {
            auto it = fields.find(node.memberName);
            if (it != fields.end()) {
                lastResult_ = it->second;
                return;
            }
            std::string errMsg = "(AR) المفتاح '" + node.memberName + "' غير موجود في الخريطة. ";
            errMsg += "(EN) Key '" + node.memberName + "' not found in map.";
            throw RuntimeError(errMsg, node.position);
        }
        className = classNameIt->second.toString();
        classType = classManager->getClass(className);
    } else {
        std::string errMsg = "(AR) لا يمكن الوصول لعضو من قيمة ليست كائن. ";
        errMsg += "(EN) Cannot access member of non-object value.";
        throw RuntimeError(errMsg, node.position);
    }
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.", node.position);
    }
    
    // (AR) البحث عن الحقل في السلسلة الهرمية / (EN) Search field in hierarchy
    ClassField* field = classType->findField(node.memberName);
    
    // (AR) البحث عن خاصية (Property) / (EN) Look for property
    ClassProperty* property = nullptr;
    if (!field) {
        property = classType->findProperty(node.memberName);
    }
    
    if (!field && !property) {
        auto it = fields.find(node.memberName);
        if (it != fields.end()) {
            lastResult_ = it->second;
            return;
        }
        std::string errMsg = "(AR) الحقل أو الخاصية '" + node.memberName + "' غير موجود في الكائن. ";
        errMsg += "(EN) Field or property '" + node.memberName + "' not found in object.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // (AR) إذا كانت خاصية، نفذ الـ getter / (EN) If property, execute getter
    if (property) {
        checkMemberAccess(property->visibility, node.memberName, classType);
        
        if (!property->getterBody) {
            std::string errMsg = "(AR) الخاصية '" + node.memberName + "' للكتابة فقط (لا يوجد getter). ";
            errMsg += "(EN) Property '" + node.memberName + "' is write-only (no getter).";
            throw RuntimeError(errMsg, node.position);
        }
        
        variableManager_.enterScope(Data::ScopeType::FUNCTION, "get_" + node.memberName);
        variableManager_.define("هذا", objectValue);
        variableManager_.define("this", objectValue);
        
        for (const auto& [fname, fval] : fields) {
            if (fname != "__class__") {
                variableManager_.define(fname, fval);
            }
        }
        
        try {
            property->getterBody->accept(statementExecutor_);
            
            if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
                lastResult_ = statementExecutor_.getReturnValue();
                statementExecutor_.resetFlowControl();
            }
        } catch (...) {
            variableManager_.exitScope();
            throw;
        }
        
        variableManager_.exitScope();
        return;
    }
    
    // (AR) معالجة الحقل العادي / (EN) Handle regular field
    checkMemberAccess(field->visibility, node.memberName, classType);
    
    
    auto it = fields.find(node.memberName);
    if (it == fields.end()) {
        std::string errMsg = "(AR) الحقل '" + node.memberName + "' غير موجود في الكائن. ";
        errMsg += "(EN) Field '" + node.memberName + "' not found in object.";
        throw RuntimeError(errMsg, node.position);
    }
    
    lastResult_ = it->second;
}

// =========================================================================
// (AR) كتابة القيمة رجوعاً عبر سلسلة التعبيرات المتداخلة
// (EN) Recursive writeback for deep nested assignment chains
// a.b.c = val → read a.b, set c in it, write back a.b, then a
// arr[0][1] = val → read arr[0], set [1] in it, write back arr[0], then arr
// =========================================================================
void ExpressionEvaluator::writeBackChain(AST::Expression* expr, const Data::Value& value) {
    if (!expr) return;
    
    // (AR) الحالة الأساسية: الوصول لمتغير جذري ← نعيّنه مباشرة
    // (EN) Base case: root variable → assign directly
    if (auto* varExpr = dynamic_cast<VariableExpr*>(expr)) {
        variableManager_.assign(varExpr->name, value);
        return;
    }
    
    // (AR) الحالة 2: parent.member ← نقرأ الأصل، نحدّث الحقل، ثم نكتب رجوعاً
    // (EN) Case 2: parent.member → read parent, update field, write back recursively
    if (auto* memberExpr = dynamic_cast<MemberExpr*>(expr)) {
        memberExpr->object->accept(*this);
        Value parentValue = lastResult_;
        
        if (parentValue.isMap()) {
            // (AR) تعديل مباشر عبر shared_ptr — بدون نسخ!
            // (EN) Direct mutation via shared_ptr — no copying!
            parentValue.toMapMut()[memberExpr->member] = value;
        } else if (parentValue.isObject()) {
            auto parentObj = parentValue.toObject();
            if (parentObj) {
                parentObj->fields[memberExpr->member] = value;
                // (AR) الكائنات بالمرجع — التعديل ينعكس تلقائياً
                // (EN) Objects are ref-counted — changes propagate automatically
            }
        }
        return;
    }
    
    // (AR) الحالة 3: container[index] ← تعديل مباشر عبر shared_ptr
    // (EN) Case 3: container[index] → direct mutation via shared_ptr (O(1) per level)
    if (auto* indexExpr = dynamic_cast<IndexExpr*>(expr)) {
        indexExpr->object->accept(*this);
        Value containerValue = lastResult_;
        
        indexExpr->index->accept(*this);
        Value idxValue = lastResult_;
        
        if (containerValue.isArray()) {
            // (AR) تعديل مباشر — المصفوفة مخزّنة كـ shared_ptr
            // (EN) Direct mutation — array is stored as shared_ptr
            auto& arr = containerValue.toArrayMut();
            int idx = idxValue.toInt();
            if (idx < 0) idx = static_cast<int>(arr.size()) + idx;
            if (idx >= 0 && idx < static_cast<int>(arr.size())) {
                arr[idx] = value;
            }
        } else if (containerValue.isMap()) {
            // (AR) تعديل مباشر على الخريطة
            containerValue.toMapMut()[idxValue.toString()] = value;
        }
        return;
    }
}

// =========================================================================
// (AR) تعيين قيمة لعضو / (EN) Member Assignment
// =========================================================================

void ExpressionEvaluator::visitMemberAssignExpr(MemberAssignExpr& node) {
    // ═══════════════════════════════════════════════════════════════════════
    // (AR) تعيين قيمة لحقل في كائن: كائن.حقل = قيمة
    //      يدعم كلاً من نوع OBJECT الحقيقي ونوع MAP القديم
    //      
    //      مع OBJECT الحقيقي: التعيين يكون بالمرجع — أي تغيير ينعكس
    //      على جميع المتغيرات التي تشير لنفس الكائن
    //
    // (EN) Assign value to field in object: object.field = value
    //      Supports both real OBJECT type and legacy MAP type
    //      
    //      With real OBJECT: assignment is by reference — any change
    //      reflects on all variables pointing to the same object
    // ═══════════════════════════════════════════════════════════════════════
    
    // (AR) تقييم الكائن
    // (EN) Evaluate the object
    node.object->accept(*this);
    Value objectValue = lastResult_;
    
    // (AR) تقييم القيمة الجديدة
    // (EN) Evaluate the new value
    node.value->accept(*this);
    Value newValue = lastResult_;
    
    std::string className;
    ClassType* classType = nullptr;
    Value::MapType fields;
    bool isRealObject = false;
    std::shared_ptr<ObjectInstance> objPtr = nullptr;
    
    if (objectValue.isObject()) {
        // (AR) نوع OBJECT الحقيقي
        // (EN) Real OBJECT type
        isRealObject = true;
        objPtr = objectValue.toObject();
        if (objPtr) {
            className = objPtr->getClassName();
            fields = objPtr->fields;
        }
    } else if (objectValue.isMap()) {
        // (AR) نوع MAP القديم
        // (EN) Legacy MAP type
        fields = objectValue.toMap();
        auto classNameIt = fields.find("__class__");
        if (classNameIt == fields.end()) {
            // ═══════════════════════════════════════════════════════════════
            // (AR) خريطة عادية بدون صنف — وصول مباشر للمفتاح
            //      مثال: خ.مفتاح = قيمة  حيث خ = {"مفتاح": ...}
            // (EN) Plain map without class — direct key access
            //      Example: m.key = val  where m = {"key": ...}
            // ═══════════════════════════════════════════════════════════════
            fields[node.member] = newValue;
            Value modifiedObject(fields);
            writeBackChain(node.object.get(), modifiedObject);
            lastResult_ = newValue;
            return;
        }
        className = classNameIt->second.toString();
    } else if (objectValue.isString()) {
        // ═══════════════════════════════════════════════════════════════
        // (AR) وصول ثابت/ساكن: ClassName.staticField = value
        //      مثال: موظف.عدد_الموظفين = 5 أو موظف.عدد_الموظفين += 1
        // (EN) Static access: ClassName.staticField = value
        // ═══════════════════════════════════════════════════════════════
        std::string possibleClassName = objectValue.toString();
        auto* cm = Data::ClassManager::getInstance();
        ClassType* staticClassType = cm->getClass(possibleClassName);
        if (staticClassType) {
            ClassField* field = staticClassType->findField(node.member);
            if (field && field->isStatic) {
                staticClassType->setStaticField(node.member, newValue);
                lastResult_ = newValue;
                return;
            }
        }
        std::string errMsg = "(AR) لا يمكن تعيين قيمة لعضو من قيمة ليست كائن. ";
        errMsg += "(EN) Cannot assign to member of non-object value.";
        throw RuntimeError(errMsg, node.position);
    } else {
        std::string errMsg = "(AR) لا يمكن تعيين قيمة لعضو من قيمة ليست كائن. ";
        errMsg += "(EN) Cannot assign to member of non-object value.";
        throw RuntimeError(errMsg, node.position);
    }
    
    // (AR) الحصول على ClassType
    // (EN) Get ClassType
    auto* classManager = Data::ClassManager::getInstance();
    classType = classManager->getClass(className);
    
    if (!classType) {
        throw RuntimeError("(AR) الصنف غير موجود. (EN) Class not found.", node.position);
    }
    
    // (AR) البحث عن الحقل
    // (EN) Find the field
    ClassField* field = classType->findField(node.member);
    
    // (AR) البحث عن خاصية (Property) إذا لم يُوجد حقل
    // (EN) Search for property if field not found
    ClassProperty* property = nullptr;
    if (!field) {
        property = classType->findProperty(node.member);
    }
    
    if (!field && !property) {
        // (AR) لم يُعثر على حقل أو خاصية معلنة — إنشاء حقل ديناميكي
        //      هذا يسمح بـ هذا.حقل = قيمة في الباني بدون إعلان مسبق
        // (EN) No declared field or property found — create dynamic field
        //      This allows this.field = value in constructor without prior declaration
        objPtr->fields[node.member] = newValue;
        // (AR) مزامنة النطاق / (EN) Sync scope
        try {
            variableManager_.assign(node.member, newValue);
        } catch (const std::exception&) {
            // (AR) متوقع: المتغير قد لا يكون معرّفاً في النطاق عند إنشاء حقل ديناميكي
            // (EN) Expected: variable may not be defined in scope when creating dynamic field
        }
        lastResult_ = newValue;
        return;
    }
    
    // (AR) إذا كانت خاصية، نفذ الـ setter
    // (EN) If it's a property, execute the setter
    if (property) {
        checkMemberAccess(property->visibility, node.member, classType);
        
        if (!property->setterBody) {
            std::string errMsg = "(AR) الخاصية '" + node.member + "' للقراءة فقط (لا يوجد setter). ";
            errMsg += "(EN) Property '" + node.member + "' is read-only (no setter).";
            throw RuntimeError(errMsg, node.position);
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) تنفيذ setter body في نطاق جديد مع سياق الكائن
        //      نربط 'هذا' بالكائن، ونربط 'قيمة'/'value' بالقيمة الجديدة
        //      وكذلك نربط الحقول للوصول المباشر
        //
        // (EN) Execute setter body in new scope with object context
        //      Bind 'هذا' (this) to object, 'قيمة'/'value' to new value
        //      and bind fields for direct access
        // ═══════════════════════════════════════════════════════════════
        variableManager_.enterScope(Data::ScopeType::FUNCTION, "set_" + node.member);
        
        // (AR) ربط 'هذا' و 'قيمة' / (EN) Bind 'this' and 'value'
        variableManager_.define("هذا", objectValue);
        variableManager_.define("this", objectValue);
        variableManager_.define("قيمة", newValue);
        variableManager_.define("value", newValue);
        
        // (AR) ربط حقول الكائن / (EN) Bind object fields
        if (isRealObject && objPtr) {
            for (const auto& [fname, fval] : objPtr->fields) {
                variableManager_.define(fname, fval);
            }
        } else {
            auto fields = objectValue.toMap();
            for (const auto& [fname, fval] : fields) {
                if (fname != "__class__") {
                    variableManager_.define(fname, fval);
                }
            }
        }
        
        try {
            // (AR) تنفيذ setter body عبر StatementExecutor (لأن الجسم هو BlockStmt)
            // (EN) Execute setter body via StatementExecutor (because body is BlockStmt)
            property->setterBody->accept(statementExecutor_);
            
            if (statementExecutor_.getFlowControl() == FlowControl::RETURN) {
                statementExecutor_.resetFlowControl();
            }
            
            // (AR) كتابة الحقول المحدثة إلى الكائن
            // (EN) Write back updated fields to object
            if (isRealObject && objPtr) {
                for (auto& [fname, fval] : objPtr->fields) {
                    if (variableManager_.exists(fname)) {
                        Value updated = variableManager_.get(fname);
                        objPtr->fields[fname] = updated;
                    }
                    // (AR) الحقل غير موجود في النطاق — متوقع عند كتابة الحقول
                    // (EN) Field not in scope — expected during field writeback
                }
            }
        } catch (...) {
            variableManager_.exitScope();
            throw;
        }
        
        variableManager_.exitScope();
        lastResult_ = newValue;
        return;
    }
    
    // (AR) معالجة الحقل العادي
    // (EN) Handle regular field
    checkMemberAccess(field->visibility, node.member, classType);
    
    if (isRealObject && objPtr) {
        // ═══════════════════════════════════════════════════════════════
        // (AR) OBJECT حقيقي: نحدّث ObjectInstance مباشرة بالمرجع
        //      لا نحتاج إعادة تعيين المتغير لأن shared_ptr يعمل بالمرجع
        //
        // (EN) Real OBJECT: update ObjectInstance directly by reference
        //      No need to reassign the variable since shared_ptr works by ref
        //
        // (AR) مزامنة متغير النطاق: عند تعيين هذا.حقل = قيمة داخل دالة،
        //      يجب تحديث متغير النطاق أيضاً لأن مرحلة الكتابة الراجعة
        //      في visitMethodCallExpr تقرأ من متغيرات النطاق
        //
        // (EN) Sync scope variable: when assigning this.field = value inside 
        //      a method, also update scope variable because the writeback 
        //      phase in visitMethodCallExpr reads from scope variables
        // ═══════════════════════════════════════════════════════════════
        objPtr->fields[node.member] = newValue;
        // (AR) مزامنة النطاق / (EN) Sync scope
        try {
            variableManager_.assign(node.member, newValue);
        } catch (const std::exception&) {
            // (AR) الحقل قد لا يكون في النطاق — مقبول عند الوصول المباشر للحقل
            // (EN) Field might not be in scope — acceptable for direct field access
        } catch (...) {
            // (AR) خطأ غير متوقع — تحذير بدلاً من ابتلاع صامت
            // (EN) Unexpected error — warn instead of silent swallow
            std::cerr << "(AR) تحذير: خطأ غير متوقع عند مزامنة الحقل '"
                      << node.member << "' / "
                      << "(EN) Warning: unexpected error syncing field '"
                      << node.member << "'" << std::endl;
        }
        lastResult_ = newValue;
    } else {
        // (AR) MAP القديم: نحتاج إعادة بناء وتعيين — ندعم أي عمق
        // (EN) Legacy MAP: need to rebuild and reassign — supports any depth
        fields[node.member] = newValue;
        Value modifiedObject(fields);
        
        // (AR) كتابة رجوعية تكرارية تدعم أي مستوى تداخل
        // (EN) Recursive writeback supports any nesting depth
        writeBackChain(node.object.get(), modifiedObject);
        lastResult_ = newValue;
    }
}

// =========================================================================
// (AR) تقييم الإسناد بالفهرس / (EN) Index Assignment Evaluation
// م[0] = 5 ، قاموس["مفتاح"] = قيمة
// =========================================================================
void ExpressionEvaluator::visitIndexAssignExpr(IndexAssignExpr& node) {
    // ==========================================
    // الخطوة 1: تقييم الكائن (المصفوفة أو القاموس)
    // ==========================================
    node.object->accept(*this);
    Value objectValue = lastResult_;

    // ==========================================
    // الخطوة 2: تقييم الفهرس
    // ==========================================
    node.index->accept(*this);
    Value indexValue = lastResult_;

    // ==========================================
    // الخطوة 3: تقييم القيمة الجديدة
    // ==========================================
    node.value->accept(*this);
    Value newValue = lastResult_;

    // ==========================================
    // الخطوة 4: التعامل مع المصفوفات
    // ==========================================
    if (objectValue.isArray()) {
        if (!indexValue.isNumeric()) {
            throw RuntimeError(
                "(AR) فهرس المصفوفة يجب أن يكون رقماً. "
                "(EN) Array index must be a number.",
                node.position);
        }

        auto arr = objectValue.toArray();
        int idx = indexValue.isInteger() ? indexValue.toInt() : static_cast<int>(indexValue.toDouble());
        int size = static_cast<int>(arr.size());

        // دعم الفهارس السالبة: -1 = آخر عنصر، -2 = ما قبل الأخير...
        if (idx < 0) {
            idx = size + idx;
        }

        if (idx < 0 || idx >= size) {
            throw RuntimeError(
                "(AR) فهرس المصفوفة خارج النطاق: " + std::to_string(idx) +
                " (الحجم: " + std::to_string(size) + "). "
                "(EN) Array index out of range: " + std::to_string(idx) +
                " (size: " + std::to_string(size) + ").",
                node.position);
        }

        arr[idx] = newValue;

        // (AR) كتابة رجوعية تكرارية تدعم أي مستوى تداخل
        // (EN) Recursive writeback supports any nesting depth
        writeBackChain(node.object.get(), Value(arr));

        lastResult_ = newValue;
        return;
    }

    // ==========================================
    // الخطوة 5: التعامل مع القواميس (MAP)
    // ==========================================
    if (objectValue.isMap()) {
        std::string key = indexValue.toString();
        auto map = objectValue.toMap();
        map[key] = newValue;

        // (AR) كتابة رجوعية تكرارية تدعم أي مستوى تداخل
        // (EN) Recursive writeback supports any nesting depth
        writeBackChain(node.object.get(), Value(map));

        lastResult_ = newValue;
        return;
    }

    throw RuntimeError(
        "(AR) لا يمكن الإسناد بالفهرس إلا للمصفوفات والقواميس. "
        "(EN) Index assignment only supported on arrays and maps.",
        node.position);
}

// =========================================================================
// (AR) تقييم تعبير Walrus / (EN) Walrus Expression Evaluation
// =========================================================================

/**
 * @brief (AR) تقييم عامل Walrus - التعيين داخل التعبير
 *        (EN) Evaluate walrus operator - assignment within expression
 * 
 * @param node (AR) عقدة تعبير Walrus / (EN) Walrus expression node
 * 
 * @details
 *   (AR) عامل Walrus (:=) يسمح بالتعيين داخل التعبير ويُرجع القيمة المُعيّنة
 *   (EN) Walrus operator (:=) allows assignment within expression and returns assigned value
 * 
 *   Examples / أمثلة:
 *   - if (n := len(items)) > 10: print(n)
 *   - while (line := file.read()): process(line)
 */
void ExpressionEvaluator::visitWalrusExpr(WalrusExpr& node) {
    // (AR) تقييم القيمة المراد تعيينها / (EN) Evaluate the value to assign
    node.value->accept(*this);
    Value assignedValue = lastResult_;
    
    // (AR) محاولة تعيين القيمة للمتغير — أو تعريفه إن لم يوجد
    // (EN) Try to assign value to variable — or define it if not found
    // (AR) تحسين أداء: بحث واحد بدلاً من اثنين
    // (EN) Performance: single lookup instead of two
    variableManager_.defineOrAssign(node.variable, assignedValue);
    
    // (AR) إرجاع القيمة المُعيّنة (هذا هو سلوك Walrus)
    // (EN) Return the assigned value (this is walrus behavior)
    lastResult_ = assignedValue;
}

// =========================================================================
// (AR) تقييم دالة Lambda / (EN) Lambda Function Evaluation
// =========================================================================

void ExpressionEvaluator::visitLambdaExpr(LambdaExpr& node) {
    // (AR) إنشاء اسم فريد للدالة Lambda / (EN) Generate unique name for lambda function
    static std::atomic<int> lambdaCounter{0};
    std::string lambdaName = "__lambda_" + std::to_string(lambdaCounter.fetch_add(1));
    
    // (AR) تحويل المعاملات من AST Parameter إلى FunctionParameter
    // (EN) Convert parameters from AST Parameter to FunctionParameter
    auto dataTypeToString = [](DataType type) -> std::string {
        switch (type) {
            case DataType::INTEGER: return "integer";
            case DataType::FLOAT: return "float";
            case DataType::STRING: return "string";
            case DataType::BOOLEAN: return "boolean";
            case DataType::NONE: return "none";
            case DataType::ARRAY: return "array";
            case DataType::MAP: return "map";
            case DataType::FUNCTION: return "function";
            case DataType::OBJECT: return "object";
            default: return "unknown";
        }
    };
    
    std::vector<Data::FunctionParameter> params;
    for (const auto& param : node.parameters) {
        params.push_back(Data::FunctionParameter(
            param.name,
            dataTypeToString(param.type)
        ));
    }
    
    // (AR) تحويل جسم Lambda إلى ASTNode — upcast آمن
    // (EN) Convert lambda body to ASTNode — safe upcast
    // (AR) إذا كان جسم كتلي (دالة مجهولة)، استخدمه. وإلا استخدم التعبير.
    // (EN) If block body (anonymous function), use it. Otherwise use expression body.
    std::shared_ptr<AST::ASTNode> bodyNode;
    if (node.blockBody) {
        bodyNode = std::shared_ptr<AST::ASTNode>(
            static_cast<AST::ASTNode*>(node.blockBody.get()),
            [](AST::ASTNode*) {}  // Empty deleter - AST owns the memory
        );
    } else {
        bodyNode = std::shared_ptr<AST::ASTNode>(
            static_cast<AST::ASTNode*>(node.body.get()),
            [](AST::ASTNode*) {}  // Empty deleter - AST owns the memory
        );
    }
    
    // (AR) تسجيل Lambda كدالة في FunctionManager
    // (EN) Register lambda as function in FunctionManager
    functionManager_.defineFunction(lambdaName, params, bodyNode);
    
    // (AR) التقاط المتغيرات من النطاق الحالي للإغلاقات
    // (EN) Capture variables from current scope for closures
    {
        auto func = functionManager_.getFunction(lambdaName, params.size());
        if (func) {
            std::unordered_map<std::string, Data::Value> captures;
            auto varNames = variableManager_.getVariableNames();
            for (const auto& vname : varNames) {
                // (AR) لا نلتقط اسم اللامدا نفسها ولا المعاملات
                // (EN) Don't capture the lambda name itself or parameter names
                bool isParam = false;
                for (const auto& p : params) {
                    if (p.name == vname) { isParam = true; break; }
                }
                if (!isParam && vname != lambdaName) {
                    try {
                        captures[vname] = variableManager_.get(vname);
                    } catch (const std::exception&) {
                        // (AR) متوقع: بعض المتغيرات قد لا تكون متاحة للالتقاط
                        // (EN) Expected: some variables may not be accessible for capture
                    }
                }
            }
            func->setCaptures(captures);
        }
    }
    
    // (AR) إرجاع اسم الدالة كقيمة String
    // (EN) Return function name as String value
    // Note: في المستقبل، يمكن إضافة نوع FUNCTION إلى Value
    // Note: In future, we can add FUNCTION type to Value
    lastResult_ = Value(lambdaName);
}

// =========================================================================
// (AR) تقييم استيعاب قائمي / (EN) List Comprehension Evaluation
// =========================================================================

void ExpressionEvaluator::visitListComprehensionExpr(ListComprehensionExpr& node) {
    // (AR) تقييم iterable / (EN) Evaluate iterable
    node.iterable->accept(*this);
    Value iterableValue = lastResult_;
    
    if (!iterableValue.isArray()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "الاستيعاب القائمي يتطلب مصفوفة",
            "List comprehension requires an array"
        );
        lastResult_ = Value(std::vector<Value>());
        return;
    }
    
    // (AR) إنشاء مصفوفة النتيجة / (EN) Create result array
    std::vector<Value> result;
    
    // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
    variableManager_.enterScope(Data::ScopeType::BLOCK, "list_comprehension");
    
    // (AR) المرور على كل عنصر / (EN) Iterate over each element
    for (size_t i = 0; i < iterableValue.size(); ++i) {
        Value item = iterableValue[i];
        
        // (AR) تحديث متغير الحلقة / (EN) Update loop variable
        if (i == 0) {
            // أول iteration - نعرّف المتغير
            variableManager_.define(node.variable, item);
        } else {
            // iterations تالية - نحدّث باستخدام assign
            variableManager_.assign(node.variable, item);
        }
        
        // (AR) التحقق من الشرط إن وُجد / (EN) Check condition if exists
        bool includeItem = true;
        if (node.condition) {
            node.condition->accept(*this);
            includeItem = lastResult_.toBool();
        }
        
        // (AR) تقييم التعبير وإضافة النتيجة / (EN) Evaluate element expression and add result
        if (includeItem) {
            node.element->accept(*this);
            result.push_back(lastResult_);
        }
    }
    
    // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
    variableManager_.exitScope();
    
    // (AR) إرجاع المصفوفة الناتجة / (EN) Return result array
    lastResult_ = Value(result);
}

// =========================================================================
// (AR) تقييم استيعاب قاموسي / (EN) Dict Comprehension Evaluation
// =========================================================================

void ExpressionEvaluator::visitDictComprehensionExpr(DictComprehensionExpr& node) {
    // (AR) تقييم iterable / (EN) Evaluate iterable
    node.iterable->accept(*this);
    Value iterableValue = lastResult_;
    
    if (!iterableValue.isArray()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "الاستيعاب القاموسي يتطلب مصفوفة",
            "Dictionary comprehension requires an array"
        );
        lastResult_ = Value(std::unordered_map<std::string, Value>());
        return;
    }
    
    // (AR) إنشاء قاموس النتيجة / (EN) Create result dictionary
    std::unordered_map<std::string, Value> result;
    
    // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
    variableManager_.enterScope(Data::ScopeType::BLOCK, "dict_comprehension");
    
    // (AR) المرور على كل عنصر / (EN) Iterate over each element
    for (size_t i = 0; i < iterableValue.size(); ++i) {
        Value item = iterableValue[i];
        
        // (AR) تحديث متغير الحلقة / (EN) Update loop variable
        if (i == 0) {
            // أول iteration - نعرّف المتغير
            variableManager_.define(node.variable, item);
        } else {
            // iterations تالية - نحدّث باستخدام assign
            variableManager_.assign(node.variable, item);
        }
        
        // (AR) التحقق من الشرط إن وُجد / (EN) Check condition if exists
        bool includeItem = true;
        if (node.condition) {
            node.condition->accept(*this);
            includeItem = lastResult_.toBool();
        }
        
        // (AR) تقييم Key و Value وإضافة النتيجة / (EN) Evaluate key & value expressions and add result
        if (includeItem) {
            // تقييم تعبير المفتاح / Evaluate key expression
            node.key->accept(*this);
            Value keyValue = lastResult_;
            
            // تقييم تعبير القيمة / Evaluate value expression
            node.value->accept(*this);
            Value valueValue = lastResult_;
            
            // تحويل المفتاح إلى نص / Convert key to string
            std::string keyStr = keyValue.toString();
            
            // إضافة إلى القاموس / Add to dictionary
            result[keyStr] = valueValue;
        }
    }
    
    // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
    variableManager_.exitScope();
    
    // (AR) إرجاع القاموس الناتج / (EN) Return result dictionary
    lastResult_ = Value(result);
}

// =========================================================================
// (AR) تقييم استيعاب مجموعة / (EN) Set Comprehension Evaluation
// =========================================================================

void ExpressionEvaluator::visitSetComprehensionExpr(SetComprehensionExpr& node) {
    // (AR) تقييم iterable / (EN) Evaluate iterable
    node.iterable->accept(*this);
    Value iterableValue = lastResult_;
    
    if (!iterableValue.isArray()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation("<input>", 0, 0),
            "الاستيعاب المجموعة يتطلب مصفوفة",
            "Set comprehension requires an array"
        );
        lastResult_ = Value(std::vector<Value>());
        return;
    }
    
    // (AR) إنشاء set النتيجة (نستخدم vector مع فحص التكرار) / (EN) Create result set (using vector with uniqueness check)
    std::vector<Value> result;
    
    // (AR) إنشاء نطاق جديد للـ comprehension / (EN) Create new scope for comprehension
    variableManager_.enterScope(Data::ScopeType::BLOCK, "set_comprehension");
    
    // (AR) المرور على كل عنصر / (EN) Iterate over each element
    for (size_t i = 0; i < iterableValue.size(); ++i) {
        Value item = iterableValue[i];
        
        // (AR) تحديث متغير الحلقة / (EN) Update loop variable
        if (i == 0) {
            // أول iteration - نعرّف المتغير
            variableManager_.define(node.variable, item);
        } else {
            // iterations تالية - نحدّث باستخدام assign
            variableManager_.assign(node.variable, item);
        }
        
        // (AR) التحقق من الشرط إن وُجد / (EN) Check condition if exists
        bool includeItem = true;
        if (node.condition) {
            node.condition->accept(*this);
            includeItem = lastResult_.toBool();
        }
        
        // (AR) تقييم Expression وإضافة النتيجة (بدون تكرار) / (EN) Evaluate expression and add result (without duplicates)
        if (includeItem) {
            node.expression->accept(*this);
            Value itemValue = lastResult_;
            
            // (AR) فحص إذا كان العنصر موجود مسبقاً / (EN) Check if item already exists
            bool exists = false;
            for (const auto& existingItem : result) {
                // Simple equality check - compare toString() representations
                // (AR) فحص بسيط للمساواة - مقارنة التمثيلات النصية
                if (existingItem.toString() == itemValue.toString()) {
                    exists = true;
                    break;
                }
            }
            
            // (AR) إضافة العنصر إذا لم يكن موجوداً / (EN) Add item if not exists
            if (!exists) {
                result.push_back(itemValue);
            }
        }
    }
    
    // (AR) الخروج من نطاق الـ comprehension / (EN) Exit comprehension scope
    variableManager_.exitScope();
    
    // (AR) إرجاع المجموعة الناتجة (كمصفوفة) / (EN) Return result set (as array)
    lastResult_ = Value(result);
}

// =========================================================================
// (AR) فحص الوصول للأعضاء / (EN) Member Access Check
// =========================================================================

void ExpressionEvaluator::checkMemberAccess(
    AST::Visibility visibility, 
    const std::string& memberName,
    Data::ClassType* targetClass) 
{
    // PUBLIC: دائماً متاح
    if (visibility == AST::Visibility::PUBLIC) {
        return;
    }
    
    // التحقق من السياق: هل نحن داخل method من الصنف نفسه أو صنف مشتق؟
    // Check context: are we inside a method from the same class or derived class?
    bool insideMethod = variableManager_.exists("هذا") || variableManager_.exists("this");
    
    if (insideMethod) {
        // (AR) نحن داخل طريقة — نتحقق من الصنف الحالي
        //      يدعم نوع OBJECT الحقيقي ونوع MAP القديم
        // (EN) We're inside a method — check current class
        //      Supports real OBJECT type and legacy MAP type
        Value thisValue = variableManager_.exists("هذا") ? 
                         variableManager_.get("هذا") : 
                         variableManager_.get("this");
        
        std::string currentClassName;
        if (thisValue.isObject()) {
            currentClassName = thisValue.getClassName();
        } else if (thisValue.isMap()) {
            auto fields = thisValue.toMap();
            auto classNameIt = fields.find("__class__");
            if (classNameIt != fields.end()) {
                currentClassName = classNameIt->second.toString();
            }
        }
        
        if (!currentClassName.empty()) {
            auto* classManager = Data::ClassManager::getInstance();
            Data::ClassType* currentClass = classManager->getClass(currentClassName);
            
            if (currentClass) {
                    // (AR) التحقق إذا كان الصنف الحالي هو نفسه أو مشتق من targetClass
                    // (EN) Check if current class is same or derived from targetClass
                    Data::ClassType* temp = currentClass;
                    while (temp) {
                        if (temp == targetClass || temp->name == targetClass->name) {
                            // PRIVATE: متاح فقط في نفس الصنف
                            if (visibility == AST::Visibility::PRIVATE && temp == targetClass) {
                                return; // Same class - allow access
                            }
                            // PROTECTED: متاح في نفس الصنف أو الأصناف المشتقة
                            if (visibility == AST::Visibility::PROTECTED) {
                                return; // Same class or derived - allow access
                            }
                        }
                        temp = temp->getBaseClass();
                    }
                }
        }
    }
    
    // الوصول مرفوض
    if (visibility == AST::Visibility::PRIVATE) {
        std::string errMsg = "(AR) لا يمكن الوصول للعضو الخاص '" + memberName + 
                           "' من خارج الصنف '" + targetClass->name + "'. ";
        errMsg += "(EN) Cannot access private member '" + memberName + 
                 "' from outside class '" + targetClass->name + "'.";
        throw RuntimeError(errMsg, Lexer::Position());
    }
    
    if (visibility == AST::Visibility::PROTECTED) {
        std::string errMsg = "(AR) لا يمكن الوصول للعضو المحمي '" + memberName + 
                           "' من خارج الصنف '" + targetClass->name + "' أو الأصناف المشتقة. ";
        errMsg += "(EN) Cannot access protected member '" + memberName + 
                 "' from outside class '" + targetClass->name + "' or derived classes.";
        throw RuntimeError(errMsg, Lexer::Position());
    }
}

// =========================================================================
// (AR) تقييم تعبير Await / (EN) Await Expression Evaluation
// =========================================================================

void ExpressionEvaluator::visitAwaitExpr(AwaitExpr& node) {
    // (AR) تقييم التعبير المُنتظَر / (EN) Evaluate awaited expression
    node.expression->accept(*this);
    
    // (AR) التحقق إذا كانت النتيجة Future خريطة (map-based Future)
    // (EN) Check if result is a map-based Future
    if (lastResult_.isMap()) {
        auto mapVal = lastResult_.toMap();
        auto futureIt = mapVal.find("__future__");
        if (futureIt != mapVal.end()) {
            // (AR) هذا Future مبني على خريطة
            // (EN) This is a map-based Future
            
            // (AR) التحقق إذا كان مرفوضاً
            // (EN) Check if rejected
            auto rejIt = mapVal.find("__rejected__");
            if (rejIt != mapVal.end() && rejIt->second.isBoolean() && rejIt->second.toBool()) {
                auto errIt = mapVal.find("__error__");
                std::string errMsg = (errIt != mapVal.end()) ? errIt->second.toString() : "Unknown async error";
                throw RuntimeError(
                    "(AR) خطأ في المهمة غير المتزامنة: " + errMsg +
                    " / (EN) Error in async task: " + errMsg,
                    node.position);
            }
            
            // (AR) إرجاع القيمة المحلولة
            // (EN) Return resolved value
            auto valIt = mapVal.find("__value__");
            if (valIt != mapVal.end()) {
                lastResult_ = valIt->second;
                return;
            }
        }
    }
    
    // (AR) التحقق إذا كانت النتيجة Future ككائن حقيقي (Object-based Future)
    // (EN) Check if result is an Object-based Future
    if (lastResult_.isObject()) {
        auto objPtr = lastResult_.toObject();
        if (objPtr) {
            auto futureIt = objPtr->fields.find("__future__");
            if (futureIt != objPtr->fields.end()) {
                auto valIt = objPtr->fields.find("__value__");
                if (valIt != objPtr->fields.end()) {
                    lastResult_ = valIt->second;
                    return;
                }
            }
        }
    }
    
    // (AR) إذا لم يكن Future، نرجع القيمة كما هي (متوافق مع الإصدارات السابقة)
    // (EN) If not a Future, return value as-is (backward compatible)
}

// =========================================================================
// (AR) تقييم تنفيذ القوالب / (EN) Template Instantiation Evaluation
// =========================================================================

void ExpressionEvaluator::visitTemplateInstantiation(TemplateInstantiation& node) {
    // (AR) إنشاء نسخة من القالب بالأنواع المحددة
    // (EN) Instantiate template with specified types
    
    #ifdef DEBUG
    std::cout << "[Template] تنفيذ قالب: " << node.templateName << "<";
    for (size_t i = 0; i < node.typeArguments.size(); i++) {
        if (i > 0) std::cout << ", ";
        std::cout << static_cast<int>(node.typeArguments[i]);
    }
    std::cout << ">" << std::endl;
    #endif
    
    // (AR) إنشاء مفتاح القالب
    // (EN) Create template key
    std::string templateKey = "__template_" + node.templateName;
    
    // (AR) البحث عن القالب في FunctionManager
    // (EN) Look for template in FunctionManager
    auto templateFunc = functionManager_.getFunction(templateKey, 0);
    
    if (!templateFunc) {
        // (AR) القالب غير موجود
        // (EN) Template not found
        throw Interpreter::RuntimeError(
            "(AR) القالب '" + node.templateName + "' غير معرّف. (EN) Template '" + node.templateName + "' is not defined.",
            node.position
        );
    }
    
    // (AR) للتنفيذ الحالي: نحفظ معلومات القالب في قيمة خاصة
    // (EN) For current implementation: Store template info in special value
    // سنستخدم هذا عند استدعاء الدالة
    // We'll use this when calling the function
    
    // (AR) إنشاء اسم دالة مؤقت للنسخة المُنشأة
    // (EN) Create temporary function name for instantiated version
    std::string instantiatedName = node.templateName;
    
    // (AR) حفظ اسم القالب في lastResult_
    // (EN) Store template name in lastResult_
    lastResult_ = Value(instantiatedName);
}

// =========================================================================
// (AR) زوّار التعابير المفقودة / (EN) Missing expression visitors
// =========================================================================

void ExpressionEvaluator::visitGeneratorExpr(GeneratorExpr& node) {
    // (AR) تنفيذ تعبير المولّد كمصفوفة كسولة (ينتج كل العناصر مباشرةً)
    // (EN) Execute generator expression as lazy array (produces all elements eagerly)
    // مثال: (س * 2 لكل س في [1, 2, 3] إذا س > 1)
    // Example: (x * 2 for x in [1, 2, 3] if x > 1)
    
    // (AR) تقييم التعبير القابل للتكرار
    // (EN) Evaluate the iterable expression
    node.iterable->accept(*this);
    Value iterableValue = lastResult_;
    
    std::vector<Data::Value> results;
    
    if (iterableValue.isArray()) {
        // (AR) التكرار على المصفوفة
        // (EN) Iterate over array
        auto arr = iterableValue.toArray();
        
        // (AR) ندخل نطاقاً جديداً للمولّد
        // (EN) Enter new scope for generator
        variableManager_.enterScope(Data::ScopeType::BLOCK, "__generator__");
        
        for (const auto& item : arr) {
            // (AR) تعريف أو تحديث متغير الحلقة — بحث واحد
            // (EN) Define or update loop variable — single lookup
            variableManager_.defineOrAssign(node.variable, item);
            
            // (AR) التحقق من الشرط (إن وُجد)
            // (EN) Check condition (if exists)
            if (node.condition) {
                node.condition->accept(*this);
                Value condValue = lastResult_;
                if (!condValue.toBool()) {
                    continue;  // (AR) تخطي العنصر / (EN) Skip element
                }
            }
            
            // (AR) تقييم تعبير العنصر
            // (EN) Evaluate element expression
            node.element->accept(*this);
            results.push_back(lastResult_);
        }
        
        variableManager_.exitScope();
    } else if (iterableValue.isMap()) {
        // (AR) التكرار على خريطة (مفاتيح)
        // (EN) Iterate over map (keys)
        auto mapVal = iterableValue.toMap();
        
        variableManager_.enterScope(Data::ScopeType::BLOCK, "__generator__");
        
        for (const auto& [key, val] : mapVal) {
            // (AR) تحسين أداء: بحث واحد / (EN) Performance: single lookup
            variableManager_.defineOrAssign(node.variable, Data::Value(key));
            
            if (node.condition) {
                node.condition->accept(*this);
                if (!lastResult_.toBool()) continue;
            }
            
            node.element->accept(*this);
            results.push_back(lastResult_);
        }
        
        variableManager_.exitScope();
    } else {
        throw RuntimeError(
            "(AR) تعبير المولّد يتطلب قيمة قابلة للتكرار (مصفوفة أو خريطة) / "
            "(EN) Generator expression requires an iterable value (array or map)",
            node.position);
    }
    
    // (AR) إرجاع المصفوفة الناتجة
    // (EN) Return the resulting array
    lastResult_ = Data::Value(results);
}

void ExpressionEvaluator::visitDecoratorExpr(DecoratorExpr& node) {
    // (AR) تقييم المزخرف كتعبير — يُرجع دالة المزخرف كقيمة قابلة للاستدعاء
    // (EN) Evaluate decorator as expression — returns decorator function as callable value
    auto func = functionManager_.getFunction(node.name, 1);
    if (!func) {
        // (AR) محاولة بدون عدد معاملات محدد / (EN) Try without specific param count
        auto overloads = functionManager_.getFunctionOverloads(node.name);
        if (!overloads.empty()) {
            func = overloads[0];
        }
    }
    if (func) {
        // (AR) المزخرف دالة — نرجعها كاسم الدالة (نص)
        // (EN) Decorator is a function — return it as function name string
        lastResult_ = Value(node.name);
    } else {
        // (AR) محاولة البحث عن المزخرف كمتغير (مثلاً: lambda)
        // (EN) Try looking up decorator as variable (e.g. lambda)
        try {
            Value decoratorVal = variableManager_.get(node.name);
            lastResult_ = decoratorVal;
        } catch (...) {
            // (AR) إذا لم نجد الدالة أو المتغير، نرجع الاسم كنص (سلوك احتياطي)
            // (EN) If not found as function or variable, fall back to name string
            lastResult_ = Value(node.name);
        }
    }
}

void ExpressionEvaluator::visitInlineAsmExpr(InlineAsmExpr& node) {
    // (AR) تعبير التجميع المضمن غير مدعوم في المفسّر — متاح في المترجم فقط
    // (EN) Inline assembly is not supported in interpreter — available in compiler only
    throw Interpreter::RuntimeError(
        "(AR) التجميع المضمن (inline asm) غير مدعوم في المفسّر. استخدم المترجم (sadc) / "
        "(EN) Inline assembly is not supported in interpreter. Use the compiler (sadc)",
        node.position
    );
}

/**
 * @brief (AR) تقييم تعبير المدى — ينشئ مصفوفة من بداية..نهاية (شامل الطرفين)
 * @brief (EN) Evaluate range expression — creates array from start..end (inclusive both ends)
 */
void ExpressionEvaluator::visitRangeExpr(AST::RangeExpr& node) {
    node.start->accept(*this);
    int startVal = lastResult_.toInt();
    
    node.end->accept(*this);
    int endVal = lastResult_.toInt();
    
    Value::ArrayType arr;
    if (startVal <= endVal) {
        arr.reserve(endVal - startVal + 1);
        for (int i = startVal; i <= endVal; ++i) {
            arr.push_back(Value(i));
        }
    } else {
        arr.reserve(startVal - endVal + 1);
        for (int i = startVal; i >= endVal; --i) {
            arr.push_back(Value(i));
        }
    }
    
    lastResult_ = Value(std::move(arr));
}

} // namespace Interpreter
} // namespace Sad
