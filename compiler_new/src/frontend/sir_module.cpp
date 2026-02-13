/*
 * ملف تنفيذ SIR Module - SIR Module Implementation File
 * 
 * الوصف: تنفيذ الفئات الأساسية لتمثيل البرنامج في SIR
 * Description: Implementation of core classes for SIR program representation
 *
 * يحتوي على:
 * Contains:
 * - SIRModule: تمثيل البرنامج الكامل / Complete program representation
 * - SIRFunction: تمثيل الدوال / Function representation
 * - SIRGlobalVariable: المتغيرات العامة / Global variables
 * - SIRClass: دعم OOP / OOP support
 */

#include "sir_module.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <set>

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// SIRParameter Implementation
// ============================================================================

std::string SIRParameter::toString() const {
    std::ostringstream oss;
    oss << sirTypeToString(type) << " %" << name;
    return oss.str();
}

// ============================================================================
// SIRFunction Implementation
// ============================================================================

SIRFunction::SIRFunction(const std::string& name, SIRType returnType)
    : name(name), returnType(returnType) {}

void SIRFunction::addParameter(const SIRParameter& param) {
    parameters.push_back(param);
}

void SIRFunction::addBasicBlock(std::shared_ptr<SIRBasicBlock> block) {
    if (!block) {
        throw std::runtime_error("Cannot add null basic block to function");
    }
    basicBlocks.push_back(block);
}

std::shared_ptr<SIRBasicBlock> SIRFunction::getBasicBlock(const std::string& label) {
    for (auto& block : basicBlocks) {
        if (block->name == label) {
            return block;
        }
    }
    return nullptr;
}

bool SIRFunction::validate() const {
    // التحقق من الاسم / Check name
    if (name.empty()) {
        return false;
    }
    
    // التحقق من وجود كتل أساسية على الأقل / Check at least one basic block
    if (basicBlocks.empty()) {
        return false;
    }
    
    // التحقق من أن كل كتلة منتهية بشكل صحيح / Check all blocks properly terminated
    for (const auto& block : basicBlocks) {
        if (block->getTerminator() == nullptr) {
            return false;
        }
    }
    
    // التحقق من عدم تكرار أسماء الكتل / Check no duplicate block labels
    std::set<std::string> labels;
    for (const auto& block : basicBlocks) {
        if (labels.count(block->name) > 0) {
            return false;
        }
        labels.insert(block->name);
    }
    
    return true;
}

std::string SIRFunction::toString() const {
    std::ostringstream oss;
    
    // Function signature
    oss << "define " << sirTypeToString(returnType) << " @" << name << "(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters[i].toString();
    }
    
    oss << ") {\n";
    
    // Basic blocks
    for (const auto& block : basicBlocks) {
        oss << block->name << ":\n";
        for (const auto& inst : block->instructions) {
            oss << "    " << inst.toString() << "\n";
        }
    }
    
    oss << "}\n";
    
    return oss.str();
}

// ============================================================================
// SIRGlobalVariable Implementation
// ============================================================================

SIRGlobalVariable::SIRGlobalVariable(const std::string& name, 
                                     SIRType type,
                                     const std::string& initialValue,
                                     bool isConstant)
    : name(name), type(type), initialValue(initialValue), isConstant(isConstant) {}

std::string SIRGlobalVariable::toString() const {
    std::ostringstream oss;
    
    oss << "$" << name << " = ";
    
    if (isConstant) {
        oss << "constant ";
    } else {
        oss << "global ";
    }
    
    oss << sirTypeToString(type);
    
    if (!initialValue.empty()) {
        oss << " " << initialValue;
    } else {
        oss << " zeroinitializer";
    }
    
    return oss.str();
}

// ============================================================================
// SIRClass Implementation
// ============================================================================

SIRClass::SIRClass(const std::string& name, const std::string& parentClass)
    : name(name), parentClass(parentClass) {}

void SIRClass::addField(const std::string& name, SIRType type) {
    fields_[name] = type;
    fieldOrder_.push_back(name);
}

void SIRClass::addMethod(std::shared_ptr<SIRFunction> method) {
    if (!method) {
        throw std::runtime_error("Cannot add null method to class");
    }
    methods_[method->name] = method;
}

const SIRType* SIRClass::getField(const std::string& name) const {
    auto it = fields_.find(name);
    if (it != fields_.end()) {
        return &(it->second);
    }
    return nullptr;
}

std::shared_ptr<SIRFunction> SIRClass::getMethod(const std::string& name) const {
    auto it = methods_.find(name);
    if (it != methods_.end()) {
        return it->second;
    }
    return nullptr;
}

std::string SIRClass::toString() const {
    std::ostringstream oss;
    
    oss << "class " << name;
    
    if (!parentClass.empty()) {
        oss << " extends " << parentClass;
    }
    
    oss << " {\n";
    
    // Fields
    oss << "    // Fields:\n";
    for (const auto& [fieldName, fieldType] : fields_) {
        oss << "    " << sirTypeToString(fieldType) << " " << fieldName << ";\n";
    }
    
    // Methods
    if (!methods_.empty()) {
        oss << "\n    // Methods:\n";
        for (const auto& [methodName, method] : methods_) {
            oss << "    " << method->toString() << "\n";
        }
    }
    
    oss << "}\n";
    
    return oss.str();
}

// ============================================================================
// SIRModule Implementation
// ============================================================================

// (AR) منشئ الوحدة / (EN) Module constructor
// مصدر التعريف / Source: sir_module.h:78
SIRModule::SIRModule(const std::string& moduleName)
    : name(moduleName) {
}

// (AR) الحصول على جميع الدوال / (EN) Get all functions
// مصدر التعريف / Source: sir_module.h:102
const std::vector<std::shared_ptr<SIRFunction>>& SIRModule::getFunctions() const {
    return functions_;
}

void SIRModule::addFunction(std::shared_ptr<SIRFunction> func) {
    if (!func) {
        throw std::runtime_error("Cannot add null function to module");
    }
    functions_.push_back(func);
}

void SIRModule::addGlobalVariable(std::shared_ptr<SIRGlobalVariable> globalVar) {
    if (!globalVar) {
        throw std::runtime_error("Cannot add null global variable to module");
    }
    globalVariables_.push_back(globalVar);
}

void SIRModule::addClass(std::shared_ptr<SIRClass> cls) {
    if (!cls) {
        throw std::runtime_error("Cannot add null class to module");
    }
    classes_.push_back(cls);
}

std::shared_ptr<SIRFunction> SIRModule::getFunction(const std::string& name) const {
    for (const auto& func : functions_) {
        if (func->name == name) {
            return func;
        }
    }
    return nullptr;
}

std::shared_ptr<SIRGlobalVariable> SIRModule::getGlobalVariable(const std::string& name) const {
    for (const auto& globalVar : globalVariables_) {
        if (globalVar->name == name) {
            return globalVar;
        }
    }
    return nullptr;
}

std::shared_ptr<SIRClass> SIRModule::getClass(const std::string& name) const {
    for (const auto& cls : classes_) {
        if (cls->getName() == name) {
            return cls;
        }
    }
    return nullptr;
}

std::string SIRModule::addStringConstant(const std::string& value) {
    // التحقق من وجود الثابت / Check if constant already exists
    for (const auto& [name, existingValue] : stringConstantMap_) {
        if (existingValue == value) {
            return name;
        }
    }
    
    // إنشاء اسم جديد / Create new name
    std::string name = ".str" + std::to_string(stringConstantMap_.size());
    stringConstantMap_[name] = value;
    
    return name;
}

bool SIRModule::validate() const {
    // التحقق من عدم تكرار أسماء الدوال / Check no duplicate function names
    std::set<std::string> funcNames;
    for (const auto& func : functions_) {
        if (funcNames.count(func->name) > 0) {
            return false;
        }
        funcNames.insert(func->name);
        
        // التحقق من صحة الدالة / Validate function
        if (!func->validate()) {
            return false;
        }
    }
    
    // التحقق من عدم تكرار أسماء المتغيرات العامة / Check no duplicate global names
    std::set<std::string> globalNames;
    for (const auto& globalVar : globalVariables_) {
        if (globalNames.count(globalVar->name) > 0) {
            return false;
        }
        globalNames.insert(globalVar->name);
    }
    
    // التحقق من عدم تكرار أسماء الأصناف / Check no duplicate class names
    std::set<std::string> classNames;
    for (const auto& cls : classes_) {
        if (classNames.count(cls->getName()) > 0) {
            return false;
        }
        classNames.insert(cls->getName());
    }
    
    // التحقق من وجود دالة main / Check for main function
    bool hasMain = false;
    for (const auto& func : functions_) {
        if (func->name == "main") {
            hasMain = true;
            break;
        }
    }
    
    if (!hasMain) {
        return false;
    }
    
    return true;
}

std::string SIRModule::toString() const {
    std::ostringstream oss;
    
    oss << "; SIR Module\n";
    oss << "; Generated by S Language Compiler\n\n";
    
    // String constants
    if (!stringConstantMap_.empty()) {
        oss << "; String Constants:\n";
        for (const auto& [name, value] : stringConstantMap_) {
            oss << name << " = \"" << value << "\"\n";
        }
        oss << "\n";
    }
    
    // Global variables
    if (!globalVariables_.empty()) {
        oss << "; Global Variables:\n";
        for (const auto& globalVar : globalVariables_) {
            oss << globalVar->toString() << "\n";
        }
        oss << "\n";
    }
    
    // Classes
    if (!classes_.empty()) {
        oss << "; Classes:\n";
        for (const auto& cls : classes_) {
            oss << cls->toString() << "\n";
        }
        oss << "\n";
    }
    
    // Functions
    if (!functions_.empty()) {
        oss << "; Functions:\n";
        for (const auto& func : functions_) {
            oss << func->toString() << "\n";
        }
    }
    
    return oss.str();
}

/**
 * طباعة الوحدة للـ console
 * Print module to console
 */
void SIRModule::print() const {
    std::cout << toString() << std::endl;
}

/**
 * الحصول على إجمالي عدد التعليمات
 * Get total instruction count
 */
size_t SIRModule::getTotalInstructions() const {
    size_t count = 0;
    for (const auto& func : functions_) {
        for (const auto& bb : func->getBasicBlocks()) {
            // Source: SIRBasicBlock::instructions is PUBLIC member at sir_instruction.h:356
            count += bb->instructions.size();
        }
    }
    return count;
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
