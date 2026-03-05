/**
 * @file function_manager.cpp
 * @brief (AR) تنفيذ مدير الدوال
 * @brief (EN) Function Manager implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "function_manager.h"
#include "expressions.h"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace Sad {
namespace Data {

// ============================================================================
// FunctionDefinition Implementation
// ============================================================================

FunctionDefinition::FunctionDefinition(const std::string& name,
                                     const std::vector<FunctionParameter>& params,
                                     std::shared_ptr<AST::ASTNode> body)
    : name_(name), type_(FunctionType::USER_DEFINED), parameters_(params), 
      body_(body), declaration_(body), functionDecl_(nullptr),
      nativeImplementation_(nullptr), returnType_("auto") {
    // (AR) نحفظ body كـ declaration للوصول للـ Parameters
    // (EN) Save body as declaration to access Parameters
}

FunctionDefinition::FunctionDefinition(const std::string& name,
                                     const std::vector<FunctionParameter>& params,
                                     std::function<std::shared_ptr<Data::Value>(const std::vector<std::shared_ptr<Data::Value>>&)> nativeImpl)
    : name_(name), type_(FunctionType::BUILT_IN), parameters_(params),
      body_(nullptr), declaration_(nullptr), functionDecl_(nullptr),
      nativeImplementation_(nativeImpl), returnType_("auto") {
}

size_t FunctionDefinition::getRequiredParameterCount() const {
    // (AR) حساب عدد المعاملات الإلزامية (بدون قيم افتراضية)
    // (EN) Calculate required parameters (without default values)
    size_t count = 0;
    for (const auto& param : parameters_) {
        if (!param.hasDefaultValue) {
            count++;
        }
    }
    return count;
}

bool FunctionDefinition::matchesParameterCount(size_t argCount) const {
    // (AR) التحقق من تطابق عدد المعاملات
    // (EN) Check parameter count matching
    
    size_t required = getRequiredParameterCount();
    size_t total = parameters_.size();
    
    // (AR) يجب أن يكون عدد المعاملات بين الإلزامية والكلية
    // (EN) Argument count must be between required and total
    return argCount >= required && argCount <= total;
}

bool FunctionDefinition::acceptsArgumentCount(size_t argCount) const {
    // (AR) التحقق من قبول عدد معاملات (مع القيم الافتراضية)
    // (EN) Check if function accepts argument count (with defaults)
    
    size_t required = getRequiredParameterCount();
    size_t total = parameters_.size();
    
    // (AR) نقبل إذا كان العدد بين الإلزامية والكلية
    // (EN) Accept if count is between required and total
    return argCount >= required && argCount <= total;
}

void FunctionDefinition::setFunctionDecl(std::shared_ptr<AST::ASTNode> decl) {
    // (AR) حفظ FunctionDecl للوصول لـ Parameters مع defaultValue
    // (EN) Save FunctionDecl to access Parameters with defaultValue
    functionDecl_ = decl;
}

std::string FunctionDefinition::getSignature() const {
    // (AR) إنشاء توقيع الدالة: functionName(param1, param2, ...)
    // (EN) Create function signature: functionName(param1, param2, ...)
    
    std::ostringstream oss;
    oss << name_ << "(";
    
    for (size_t i = 0; i < parameters_.size(); ++i) {
        if (i > 0) oss << ", ";
        
        oss << parameters_[i].name;
        if (!parameters_[i].type.empty()) {
            oss << ": " << parameters_[i].type;
        }
        if (parameters_[i].hasDefaultValue) {
            oss << " = " << parameters_[i].defaultValue;
        }
    }
    
    oss << ")";
    if (returnType_ != "auto") {
        oss << " -> " << returnType_;
    }
    
    return oss.str();
}

std::string FunctionDefinition::debugString() const {
    // (AR) معلومات تفصيلية للتصحيح
    // (EN) Detailed debug information
    
    std::ostringstream oss;
    oss << "Function: " << getSignature() << "\n";
    oss << "  Type: ";
    switch (type_) {
        case FunctionType::USER_DEFINED:
            oss << "USER_DEFINED";
            break;
        case FunctionType::BUILT_IN:
            oss << "BUILT_IN";
            break;
        case FunctionType::LAMBDA:
            oss << "LAMBDA";
            break;
    }
    oss << "\n";
    oss << "  Parameters: " << parameters_.size() << " total, "
        << getRequiredParameterCount() << " required\n";
    oss << "  Has body: " << (hasBody() ? "yes" : "no") << "\n";
    
    return oss.str();
}

// ============================================================================
// FunctionManager Implementation
// ============================================================================

FunctionManager::FunctionManager() {
    // (AR) المُنشئ - لا حاجة لتهيئة خاصة
    // (EN) Constructor - no special initialization needed
}

FunctionManager::~FunctionManager() {
    // (AR) المُدمر - التنظيف التلقائي
    // (EN) Destructor - automatic cleanup
    clear();
}

void FunctionManager::defineFunction(const std::string& name,
                                    const std::vector<FunctionParameter>& params,
                                    std::shared_ptr<AST::ASTNode> body) {
    // (AR) تعريف دالة معرفة من المستخدم
    // (EN) Define user-defined function
    
    // (AR) التحقق من صحة الاسم
    // (EN) Validate name
    if (name.empty()) {
        throwError("لا يمكن تعريف دالة بدون اسم", 
                  "Cannot define function without name");
    }
    
    // (AR) السماح بإعادة تعريف الدوال — ضروري للإغلاقات والدوال المتداخلة
    // (EN) Allow re-definition — essential for closures and nested functions
    size_t paramCount = params.size();
    auto it = functions_.find(name);
    if (it != functions_.end()) {
        // (AR) إزالة التعريف السابق بنفس التوقيع (مضمنة أو مستخدمة)
        // (EN) Remove previous definition with same signature (built-in or user-defined)
        it->second.erase(
            std::remove_if(it->second.begin(), it->second.end(),
                          [paramCount](const auto& func) {
                              return func->getParameterCount() == paramCount;
                          }),
            it->second.end()
        );
    }
    
    // (AR) إنشاء تعريف الدالة
    // (EN) Create function definition
    auto funcDef = std::make_shared<FunctionDefinition>(name, params, body);
    
    // (AR) إضافة الدالة للخريطة
    // (EN) Add function to map
    functions_[name].push_back(funcDef);
}

void FunctionManager::defineFunction(const std::string& name,
                                    const std::vector<FunctionParameter>& params,
                                    std::shared_ptr<AST::ASTNode> body,
                                    std::shared_ptr<AST::ASTNode> decl) {
    // (AR) تعريف دالة معرفة من المستخدم مع FunctionDecl
    // (EN) Define user-defined function with FunctionDecl
    
    // (AR) التحقق من صحة الاسم
    // (EN) Validate name
    if (name.empty()) {
        throwError("لا يمكن تعريف دالة بدون اسم", 
                  "Cannot define function without name");
    }
    
    // (AR) السماح بإعادة تعريف الدوال — ضروري للإغلاقات والدوال المتداخلة
    // (EN) Allow re-definition — essential for closures and nested functions
    size_t paramCount = params.size();
    auto it = functions_.find(name);
    if (it != functions_.end()) {
        // (AR) إزالة التعريف السابق بنفس التوقيع (مضمنة أو مستخدمة)
        // (EN) Remove previous definition with same signature (built-in or user-defined)
        it->second.erase(
            std::remove_if(it->second.begin(), it->second.end(),
                          [paramCount](const auto& func) {
                              return func->getParameterCount() == paramCount;
                          }),
            it->second.end()
        );
    }
    
    // (AR) إنشاء تعريف الدالة
    // (EN) Create function definition
    auto funcDef = std::make_shared<FunctionDefinition>(name, params, body);
    
    // (AR) حفظ FunctionDecl للوصول لـ default parameters
    // (EN) Save FunctionDecl for default parameters access
    funcDef->setFunctionDecl(decl);
    
    // (AR) إضافة الدالة للخريطة
    // (EN) Add function to map
    functions_[name].push_back(funcDef);
}

void FunctionManager::redefineFunction(const std::string& name,
                                      const std::vector<FunctionParameter>& params,
                                      std::shared_ptr<AST::ASTNode> body) {
    // (AR) إعادة تعريف دالة موجودة — يُستخدم للمزخرفات
    // (EN) Redefine existing function — used for decorators
    size_t paramCount = params.size();
    
    auto it = functions_.find(name);
    if (it != functions_.end()) {
        // (AR) إزالة التعريف القديم بنفس عدد المعاملات
        // (EN) Remove old definition with same parameter count
        it->second.erase(
            std::remove_if(it->second.begin(), it->second.end(),
                          [paramCount](const auto& func) {
                              return func->getParameterCount() == paramCount;
                          }),
            it->second.end()
        );
    }
    
    // (AR) إنشاء التعريف الجديد
    // (EN) Create new definition
    auto funcDef = std::make_shared<FunctionDefinition>(name, params, body);
    functions_[name].push_back(funcDef);
}

void FunctionManager::defineBuiltInFunction(const std::string& name,
                                           const std::vector<FunctionParameter>& params,
                                           std::function<std::shared_ptr<Data::Value>(const std::vector<std::shared_ptr<Data::Value>>&)> impl) {
    // (AR) تعريف دالة مضمنة
    // (EN) Define built-in function
    
    if (name.empty()) {
        throwError("لا يمكن تعريف دالة مضمنة بدون اسم", 
                  "Cannot define built-in function without name");
    }
    
    // (AR) إنشاء تعريف الدالة المضمنة
    // (EN) Create built-in function definition
    auto funcDef = std::make_shared<FunctionDefinition>(name, params, impl);
    
    // (AR) إضافة الدالة
    // (EN) Add function
    functions_[name].push_back(funcDef);
}

size_t FunctionManager::removeFunction(const std::string& name, int paramCount) {
    // (AR) حذف دالة أو مجموعة دوال
    // (EN) Remove function or group of functions
    
    auto it = functions_.find(name);
    if (it == functions_.end()) {
        return 0; // (AR) لا توجد دوال بهذا الاسم / (EN) No functions with this name
    }
    
    if (paramCount == -1) {
        // (AR) حذف جميع الدوال بهذا الاسم
        // (EN) Remove all functions with this name
        size_t count = it->second.size();
        functions_.erase(it);
        return count;
    } else {
        // (AR) حذف الدالة بعدد معاملات محدد
        // (EN) Remove function with specific parameter count
        auto& overloads = it->second;
        auto funcIt = std::remove_if(overloads.begin(), overloads.end(),
            [paramCount](const std::shared_ptr<FunctionDefinition>& func) {
                return func->getParameterCount() == static_cast<size_t>(paramCount);
            });
        
        if (funcIt != overloads.end()) {
            overloads.erase(funcIt, overloads.end());
            
            // (AR) إذا لم يبق أي دالة، احذف المفتاح
            // (EN) If no functions left, remove key
            if (overloads.empty()) {
                functions_.erase(it);
            }
            return 1;
        }
        return 0;
    }
}

std::shared_ptr<FunctionDefinition> FunctionManager::getFunction(
    const std::string& name, size_t argCount) const {
    // (AR) البحث عن دالة بالاسم وعدد المعاملات
    // (EN) Find function by name and argument count
    
    auto it = functions_.find(name);
    if (it == functions_.end()) {
        return nullptr; // (AR) لا توجد دالة بهذا الاسم / (EN) No function with this name
    }
    
    // (AR) البحث عن أفضل تطابق
    // (EN) Find best match
    const auto& overloads = it->second;
    for (const auto& func : overloads) {
        if (func->matchesParameterCount(argCount)) {
            return func;
        }
    }
    
    return nullptr; // (AR) لا يوجد تطابق / (EN) No match
}

bool FunctionManager::hasFunction(const std::string& name, int argCount) const {
    // (AR) التحقق من وجود دالة
    // (EN) Check function existence
    
    auto it = functions_.find(name);
    if (it == functions_.end()) {
        return false;
    }
    
    if (argCount == -1) {
        // (AR) البحث عن أي دالة بهذا الاسم
        // (EN) Look for any function with this name
        return !it->second.empty();
    } else {
        // (AR) البحث عن دالة بعدد معاملات محدد
        // (EN) Look for function with specific parameter count
        return getFunction(name, argCount) != nullptr;
    }
}

std::vector<std::shared_ptr<FunctionDefinition>> FunctionManager::getFunctionOverloads(
    const std::string& name) const {
    // (AR) الحصول على جميع الدوال بنفس الاسم
    // (EN) Get all functions with same name
    
    auto it = functions_.find(name);
    if (it == functions_.end()) {
        return {}; // (AR) قائمة فارغة / (EN) Empty list
    }
    
    return it->second;
}

std::vector<std::string> FunctionManager::getFunctionNames() const {
    // (AR) الحصول على قائمة بأسماء جميع الدوال
    // (EN) Get list of all function names
    
    std::vector<std::string> names;
    names.reserve(functions_.size());
    
    for (const auto& pair : functions_) {
        names.push_back(pair.first);
    }
    
    // (AR) ترتيب أبجدي
    // (EN) Sort alphabetically
    std::sort(names.begin(), names.end());
    
    return names;
}

size_t FunctionManager::getFunctionCount() const {
    // (AR) حساب عدد جميع الدوال (بما فيها Overloads)
    // (EN) Count all functions (including overloads)
    
    size_t count = 0;
    for (const auto& pair : functions_) {
        count += pair.second.size();
    }
    return count;
}

void FunctionManager::clear() {
    // (AR) مسح جميع الدوال
    // (EN) Clear all functions
    functions_.clear();
}

void FunctionManager::printAllFunctions() const {
    // (AR) طباعة جميع الدوال (للتصحيح)
    // (EN) Print all functions (for debugging)
    
    std::cout << "╔════════════════════════════════════════════════════╗\n";
    std::cout << "║  FUNCTION MANAGER - ALL FUNCTIONS                 ║\n";
    std::cout << "║  (AR) مدير الدوال - جميع الدوال                 ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
    
    if (functions_.empty()) {
        std::cout << "(AR) لا توجد دوال معرفة\n";
        std::cout << "(EN) No functions defined\n";
        return;
    }
    
    std::cout << "Total functions: " << getFunctionCount() 
              << " (" << functions_.size() << " unique names)\n\n";
    
    for (const auto& pair : functions_) {
        std::cout << "Function name: '" << pair.first << "' "
                  << "(" << pair.second.size() << " overload(s))\n";
        
        for (const auto& func : pair.second) {
            std::cout << "  " << func->getSignature() << "\n";
        }
        std::cout << "\n";
    }
}

std::string FunctionManager::getFunctionInfo(const std::string& name, 
                                            size_t argCount) const {
    // (AR) الحصول على معلومات عن دالة
    // (EN) Get information about function
    
    auto func = getFunction(name, argCount);
    if (!func) {
        std::ostringstream oss;
        oss << "(AR) دالة '" << name << "' بعدد معاملات " << argCount 
            << " غير موجودة\n";
        oss << "(EN) Function '" << name << "' with " << argCount 
            << " parameters not found";
        return oss.str();
    }
    
    return func->debugString();
}

std::string FunctionManager::debugString() const {
    // (AR) نص تصحيح شامل
    // (EN) Comprehensive debug string
    
    std::ostringstream oss;
    oss << "FunctionManager Debug Info:\n";
    oss << "  Total functions: " << getFunctionCount() << "\n";
    oss << "  Unique names: " << functions_.size() << "\n";
    
    if (!functions_.empty()) {
        oss << "\nFunction List:\n";
        for (const auto& pair : functions_) {
            oss << "  - " << pair.first << " (" << pair.second.size() 
                << " overload(s))\n";
            for (const auto& func : pair.second) {
                oss << "    * " << func->getSignature() << "\n";
            }
        }
    }
    
    return oss.str();
}

void FunctionManager::throwError(const std::string& messageAr, 
                                 const std::string& messageEn) const {
    // (AR) رمي خطأ ثنائي اللغة
    // (EN) Throw bilingual error
    
    std::ostringstream oss;
    oss << "(AR) خطأ في مدير الدوال: " << messageAr << "\n";
    oss << "(EN) Function Manager Error: " << messageEn;
    throw std::runtime_error(oss.str());
}

// ============================================================================
// (AR) تسجيل الدوال المضمنة
// (EN) Register Built-in Functions
// ============================================================================

void FunctionManager::registerBuiltinFunction(
    const std::string& name,
    const std::function<std::shared_ptr<Data::Value>(const std::vector<std::shared_ptr<Data::Value>>&)>& func) {
    
    // (AR) إنشاء دالة مضمنة جديدة مع دعم معاملات متعددة
    // (EN) Create new built-in function with support for multiple arguments
    
    // (AR) حذف الدالة إذا كانت موجودة بالفعل
    // (EN) Remove function if it already exists
    removeFunction(name);
    
    // (AR) إنشاء تعريف دالة مضمنة جديدة - نستخدم func مباشرة بدون wrapper
    // (EN) Create new built-in function definition - use func directly without wrapper
    std::vector<FunctionParameter> params;
    auto funcDef = std::make_shared<FunctionDefinition>(name, params, func);
    
    // (AR) تخزين الدالة في الخريطة
    // (EN) Store function in map
    functions_[name].push_back(funcDef);
}

} // namespace Data
} // namespace Sad
