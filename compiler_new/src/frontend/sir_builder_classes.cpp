// ============================================================================
// sir_builder.cpp - بناء SIR من AST / SIR Builder from AST
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 5, 2026
// الإصدار / Version: 2.0 (إعادة كتابة كاملة من الصفر)
//
// ملاحظة هامة / Important Note:
// هذا الملف مُعاد كتابته بالكامل من الصفر بما يتوافق 100% مع:
// This file is completely rewritten from scratch to comply 100% with:
// - STRICT_CODING_RULES.md
// - sir_builder.h
// - sir_types.h
// - sir_instruction.h
// - sir_module.h
// - AST headers (ast_node.h, expressions.h, statements.h, declarations.h)
// ============================================================================

#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// buildClass - بناء صنف
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:364
// التوقيع / Signature: void buildClass(AST::ClassDeclNode* classDecl);
//
// المعاملات / Parameters:
// - classDecl: AST::ClassDeclNode* = Sad::AST::ClassDecl* (sir_builder.h:60)
//
// ClassDecl Members (declarations.h:122-156):
// - name: std::string (line 125)
// - superclasses: std::vector<std::string> (line 126)
// - members: StmtList (line 127)
// - typeParameters: std::vector<TypeParameter> (line 128)
//
// الدوال المستدعاة / Called functions:
// - std::make_shared<SIRClass>: sir_module.h:409
// - module_->addClass: sir_module.h:608
// ============================================================================
void SIRBuilder::buildClass(AST::ClassDeclNode* classDecl) {
    if (!classDecl) {
        return;
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildClass: processing class '" << classDecl->name << "'" << std::endl;
    #endif
    
    // (AR) تحديد الصنف الأب (إن وجد)
    // (EN) Determine parent class (if any)
    std::string parentClass = "";
    if (!classDecl->superclasses.empty()) {
        parentClass = classDecl->superclasses[0];  // دعم وراثة واحدة حالياً
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildClass: parent class = '" << parentClass << "'" << std::endl;
        #endif
    }
    
    // (AR) إنشاء صنف SIR (SIRClass constructor: sir_module.h:409)
    // (EN) Create SIR class
    auto sirClass = std::make_shared<SIRClass>(classDecl->name, parentClass);
    
    // (AR) تعيين علامة المجرد / (EN) Set abstract flag
    sirClass->isAbstract = classDecl->isAbstract;
    
    // (AR) نسخ حقول الصنف الأب إلى الصنف الابن (دعم الوراثة)
    // (EN) Copy parent class fields into child class (inheritance support)
    if (!parentClass.empty()) {
        auto parentSirClass = module_->getClass(parentClass);
        if (parentSirClass) {
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildClass: inheriting " << parentSirClass->fields_.size() 
                      << " fields from parent '" << parentClass << "'" << std::endl;
            #endif
            // (AR) إضافة حقول الأب بالترتيب أولاً
            // (EN) Add parent fields in order first
            for (const auto& parentFieldName : parentSirClass->fieldOrder_) {
                auto fieldIt = parentSirClass->fields_.find(parentFieldName);
                if (fieldIt != parentSirClass->fields_.end()) {
                    sirClass->addField(parentFieldName, fieldIt->second);
                }
            }
        } else {
            std::cerr << "[WARNING] buildClass: parent class '" << parentClass 
                      << "' not found in module (must be declared before child class)" << std::endl;
        }
    }
    
    // (AR) معالجة أعضاء الصنف (members)
    // (EN) Process class members
    for (const auto& member : classDecl->members) {
        // (AR) التحقق من نوع العضو: حقل أو دالة
        // (EN) Check member type: field or method
        
        // (AR) الحقول (FieldDecl - declarations.h:180)
        // (EN) Fields
        if (auto fieldDecl = dynamic_cast<AST::FieldDecl*>(member.get())) {
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildClass: found field '" << fieldDecl->name << "'" << std::endl;
            #endif
            
            // (AR) تحويل النوع وإضافة الحقل
            // (EN) Convert type and add field
            SIRType fieldType = astTypeToSIRType(fieldDecl->type);
            sirClass->addField(fieldDecl->name, fieldType);
        }
        
        // (AR) الباني (ConstructorDecl - declarations.h:268)
        // (EN) Constructor
        else if (auto ctorDecl = dynamic_cast<Sad::AST::ConstructorDecl*>(member.get())) {
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildClass: found constructor" << std::endl;
            #endif
            
            std::string fullCtorName = classDecl->name + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // بناء
            auto sirCtor = std::make_shared<SIRFunction>(fullCtorName, SIRType::VOID);
            sirCtor->addParameter(SIRParameter("self", SIRType::I64));
            
            for (const auto& param : ctorDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                sirCtor->addParameter(SIRParameter(param.name, paramType));
            }
            
            sirClass->addMethod(sirCtor);
            
            // (AR) بناء جسم الباني
            // (EN) Build constructor body
            if (ctorDecl->body) {
                auto prevFunction = currentFunction_;
                auto prevBlock = currentBlock_;
                auto prevClassName = currentClassName_;
                
                currentFunction_ = sirCtor;
                currentClassName_ = classDecl->name;
                
                enterScope();
                
                // (AR) تسجيل معامل self
                // (EN) Register self parameter
                {
                    VariableInfo selfInfo;
                    selfInfo.name = "self";
                    selfInfo.type = SIRType::I64;
                    selfInfo.registerName = "%self";
                    selfInfo.isGlobal = false;
                    selfInfo.isMutable = false;
                    selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(selfInfo);
                }
                
                // (AR) تسجيل "هذا" كمرادف لـ self
                // (EN) Register "هذا" (this) as alias for self
                {
                    VariableInfo thisInfo;
                    thisInfo.name = "\xD9\x87\xD8\xB0\xD8\xA7"; // هذا
                    thisInfo.type = SIRType::I64;
                    thisInfo.registerName = "%self";
                    thisInfo.isGlobal = false;
                    thisInfo.isMutable = false;
                    thisInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(thisInfo);
                }
                
                // (AR) تسجيل المعاملات + حقول الصنف كمتغيرات محلية
                // (EN) Register parameters + class fields as local variables
                for (const auto& param : ctorDecl->parameters) {
                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = astTypeToSIRType(param.type);
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    paramInfo.isMutable = false;
                    paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(paramInfo);
                }
                
                // (AR) تسجيل حقول الصنف كمتغيرات محلية للوصول المباشر
                // (EN) Register class fields as local variables for direct access
                for (const auto& field : sirClass->fields_) {
                    VariableInfo fieldInfo;
                    fieldInfo.name = field.first;
                    fieldInfo.type = field.second;
                    fieldInfo.registerName = "%" + field.first;
                    fieldInfo.isGlobal = false;
                    fieldInfo.isMutable = true;
                    fieldInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(fieldInfo);
                }
                
                auto entryBlock = createBasicBlock("entry");
                sirCtor->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;
                
                // (AR) Alloca لكل حقل
                // (EN) Alloca for each field
                for (const auto& field : sirClass->fields_) {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register("%" + field.first, field.second);
                    currentBlock_->addInstruction(allocInst);
                }
                
                // ═══════════════════════════════════════════════════════════════
                // (AR) استدعاء باني الأب إذا كانت هناك superArgs
                // (EN) Call parent constructor if superArgs exist
                // ═══════════════════════════════════════════════════════════════
                if (!ctorDecl->superArgs.empty() && !parentClass.empty()) {
                    // (AR) بناء معاملات استدعاء باني الأب
                    // (EN) Build parent constructor call arguments
                    std::string parentCtorName = parentClass + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // بناء
                    
                    std::vector<SIROperand> superArgOperands;
                    superArgOperands.push_back(SIROperand::Register("%self", SIRType::I64)); // self
                    
                    for (auto& arg : ctorDecl->superArgs) {
                        BuildResult argResult = buildExpression(arg.get());
                        superArgOperands.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                    }
                    
                    // (AR) إصدار تعليمة CALL لباني الأب
                    // (EN) Emit CALL instruction for parent constructor
                    SIRInstruction callInst;
                    callInst.opcode = SIROpcode::CALL;
                    callInst.result = SIROperand::Register(newTempRegister(), SIRType::VOID);
                    callInst.operands.push_back(SIROperand::Register(parentCtorName, SIRType::VOID));
                    for (auto& op : superArgOperands) {
                        callInst.operands.push_back(op);
                    }
                    if (currentBlock_) currentBlock_->addInstruction(callInst);
                }
                
                buildStatement(ctorDecl->body.get());
                
                // (AR) إضافة RET_VOID
                // (EN) Add RET_VOID
                if (currentBlock_) {
                    bool hasTerminator = false;
                    if (!currentBlock_->instructions.empty()) {
                        auto lastOp = currentBlock_->instructions.back().opcode;
                        hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                    }
                    if (!hasTerminator) {
                        SIRInstruction retInst;
                        retInst.opcode = SIROpcode::RET_VOID;
                        currentBlock_->addInstruction(retInst);
                    }
                }
                
                exitScope();
                
                module_->addFunction(sirCtor);
                
                // (AR) تسجيل في جدول الدوال
                // (EN) Register in function table
                FunctionInfo ctorInfo;
                ctorInfo.name = fullCtorName;
                ctorInfo.returnType = SIRType::VOID;
                ctorInfo.parameters = sirCtor->getParameters();
                ctorInfo.sirFunction = sirCtor;
                functionTable_[fullCtorName] = ctorInfo;
                
                currentFunction_ = prevFunction;
                currentBlock_ = prevBlock;
                currentClassName_ = prevClassName;
            }
        }
        
        // (AR) الدوال (MethodDecl - declarations.h:222)
        // (EN) Methods
        else if (auto methodDecl = dynamic_cast<AST::MethodDecl*>(member.get())) {
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildClass: found method '" << methodDecl->name << "'" << std::endl;
            #endif
            
            // (AR) تخطي الدوال المجردة — لا تملك جسماً يُبنى
            // (EN) Skip abstract methods — they have no body to build
            if (methodDecl->isAbstract) {
                #ifndef NDEBUG
                std::cout << "[DEBUG] buildClass: skipping abstract method '" << methodDecl->name << "'" << std::endl;
                #endif
                continue;
            }
            
            // (AR) استنتاج نوع الإرجاع إذا لم يُحدد
            // (EN) Infer return type if not specified
            SIRType returnType;
            if (methodDecl->returnType == Data::DataType::UNKNOWN ||
                methodDecl->returnType == Data::DataType::NONE) {
                returnType = inferReturnTypeFromBody(methodDecl->body.get());
            } else {
                returnType = astTypeToSIRType(methodDecl->returnType);
            }
            std::string fullMethodName = classDecl->name + "." + methodDecl->name;
            auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);
            
            sirMethod->addParameter(SIRParameter("self", SIRType::I64));
            
            for (const auto& param : methodDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                sirMethod->addParameter(SIRParameter(param.name, paramType));
            }
            
            sirClass->addMethod(sirMethod);
            
            // (AR) بناء جسم الطريقة
            // (EN) Build method body
            if (methodDecl->body) {
                auto prevFunction = currentFunction_;
                auto prevBlock = currentBlock_;
                auto prevClassName = currentClassName_;
                
                currentFunction_ = sirMethod;
                currentClassName_ = classDecl->name;
                
                enterScope();
                
                // (AR) تسجيل self + معاملات + حقول الصنف
                // (EN) Register self + params + class fields
                {
                    VariableInfo selfInfo;
                    selfInfo.name = "self";
                    selfInfo.type = SIRType::I64;
                    selfInfo.registerName = "%self";
                    selfInfo.isGlobal = false;
                    selfInfo.isMutable = false;
                    selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(selfInfo);
                }
                
                // (AR) تسجيل "هذا" كمرادف لـ self
                // (EN) Register "هذا" (this) as alias for self
                {
                    VariableInfo thisInfo;
                    thisInfo.name = "\xD9\x87\xD8\xB0\xD8\xA7"; // هذا
                    thisInfo.type = SIRType::I64;
                    thisInfo.registerName = "%self";
                    thisInfo.isGlobal = false;
                    thisInfo.isMutable = false;
                    thisInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(thisInfo);
                }
                
                for (const auto& param : methodDecl->parameters) {
                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = astTypeToSIRType(param.type);
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    paramInfo.isMutable = false;
                    paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(paramInfo);
                }
                
                for (const auto& field : sirClass->fields_) {
                    VariableInfo fieldInfo;
                    fieldInfo.name = field.first;
                    fieldInfo.type = field.second;
                    fieldInfo.registerName = "%" + field.first;
                    fieldInfo.isGlobal = false;
                    fieldInfo.isMutable = true;
                    fieldInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(fieldInfo);
                }
                
                auto entryBlock = createBasicBlock("entry");
                sirMethod->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;
                
                // (AR) Alloca لكل حقل
                // (EN) Alloca for each field
                for (const auto& field : sirClass->fields_) {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register("%" + field.first, field.second);
                    currentBlock_->addInstruction(allocInst);
                }
                
                buildStatement(methodDecl->body.get());
                
                // (AR) إضافة terminator
                // (EN) Add terminator
                if (currentBlock_) {
                    bool hasTerminator = false;
                    if (!currentBlock_->instructions.empty()) {
                        auto lastOp = currentBlock_->instructions.back().opcode;
                        hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                    }
                    if (!hasTerminator) {
                        SIRInstruction retInst;
                        if (returnType == SIRType::VOID) {
                            retInst.opcode = SIROpcode::RET_VOID;
                        } else {
                            retInst.opcode = SIROpcode::RET;
                            if (returnType == SIRType::STRING) {
                                retInst.operands.push_back(SIROperand::ConstantString(""));
                            } else {
                                retInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                        }
                        currentBlock_->addInstruction(retInst);
                    }
                }
                
                exitScope();
                
                module_->addFunction(sirMethod);
                
                FunctionInfo methodInfo;
                methodInfo.name = fullMethodName;
                methodInfo.returnType = returnType;
                methodInfo.parameters = sirMethod->getParameters();
                methodInfo.sirFunction = sirMethod;
                functionTable_[fullMethodName] = methodInfo;
                
                currentFunction_ = prevFunction;
                currentBlock_ = prevBlock;
                currentClassName_ = prevClassName;
            }
        }
        
        // (AR) الدالة العادية داخل الصنف (FunctionDecl)
        // (EN) Regular function inside class (FunctionDecl)
        else if (auto funcDecl = dynamic_cast<AST::FunctionDecl*>(member.get())) {
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildClass: found function as method '" << funcDecl->name << "'" << std::endl;
            #endif
            
            // (AR) استنتاج نوع الإرجاع إذا لم يُحدد
            // (EN) Infer return type if not specified
            SIRType returnType;
            if (funcDecl->returnType == Data::DataType::UNKNOWN ||
                funcDecl->returnType == Data::DataType::NONE) {
                returnType = inferReturnTypeFromBody(funcDecl->body.get());
            } else {
                returnType = astTypeToSIRType(funcDecl->returnType);
            }
            std::string fullMethodName = classDecl->name + "." + funcDecl->name;
            auto sirMethod = std::make_shared<SIRFunction>(fullMethodName, returnType);
            
            sirMethod->addParameter(SIRParameter("self", SIRType::I64));
            
            for (const auto& param : funcDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                sirMethod->addParameter(SIRParameter(param.name, paramType));
            }
            
            sirClass->addMethod(sirMethod);
            
            // (AR) بناء جسم الدالة (مثل الطريقة تماماً)
            // (EN) Build function body (same as method)
            if (funcDecl->body) {
                auto prevFunction = currentFunction_;
                auto prevBlock = currentBlock_;
                auto prevClassName = currentClassName_;
                
                currentFunction_ = sirMethod;
                currentClassName_ = classDecl->name;
                
                enterScope();
                
                {
                    VariableInfo selfInfo;
                    selfInfo.name = "self";
                    selfInfo.type = SIRType::I64;
                    selfInfo.registerName = "%self";
                    selfInfo.isGlobal = false;
                    selfInfo.isMutable = false;
                    selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(selfInfo);
                }
                
                // (AR) تسجيل "هذا" كمرادف لـ self
                // (EN) Register "هذا" (this) as alias for self
                {
                    VariableInfo thisInfo;
                    thisInfo.name = "\xD9\x87\xD8\xB0\xD8\xA7"; // هذا
                    thisInfo.type = SIRType::I64;
                    thisInfo.registerName = "%self";
                    thisInfo.isGlobal = false;
                    thisInfo.isMutable = false;
                    thisInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(thisInfo);
                }
                
                for (const auto& param : funcDecl->parameters) {
                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = astTypeToSIRType(param.type);
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    paramInfo.isMutable = false;
                    paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(paramInfo);
                }
                
                for (const auto& field : sirClass->fields_) {
                    VariableInfo fieldInfo;
                    fieldInfo.name = field.first;
                    fieldInfo.type = field.second;
                    fieldInfo.registerName = "%" + field.first;
                    fieldInfo.isGlobal = false;
                    fieldInfo.isMutable = true;
                    fieldInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(fieldInfo);
                }
                
                auto entryBlock = createBasicBlock("entry");
                sirMethod->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;
                
                for (const auto& field : sirClass->fields_) {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register("%" + field.first, field.second);
                    currentBlock_->addInstruction(allocInst);
                }
                
                buildStatement(funcDecl->body.get());
                
                if (currentBlock_) {
                    bool hasTerminator = false;
                    if (!currentBlock_->instructions.empty()) {
                        auto lastOp = currentBlock_->instructions.back().opcode;
                        hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                    }
                    if (!hasTerminator) {
                        SIRInstruction retInst;
                        if (returnType == SIRType::VOID) {
                            retInst.opcode = SIROpcode::RET_VOID;
                        } else {
                            retInst.opcode = SIROpcode::RET;
                            if (returnType == SIRType::STRING) {
                                retInst.operands.push_back(SIROperand::ConstantString(""));
                            } else {
                                retInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                        }
                        currentBlock_->addInstruction(retInst);
                    }
                }
                
                exitScope();
                
                module_->addFunction(sirMethod);
                
                FunctionInfo methodInfo;
                methodInfo.name = fullMethodName;
                methodInfo.returnType = returnType;
                methodInfo.parameters = sirMethod->getParameters();
                methodInfo.sirFunction = sirMethod;
                functionTable_[fullMethodName] = methodInfo;
                
                currentFunction_ = prevFunction;
                currentBlock_ = prevBlock;
                currentClassName_ = prevClassName;
            }
        }
        
        // (AR) تحميل المعاملات الزائد (OperatorDecl - declarations.h:632)
        // (EN) Operator overloading
        else if (auto operatorDecl = dynamic_cast<Sad::AST::OperatorDecl*>(member.get())) {
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildClass: found operator overload '" << operatorDecl->operatorSymbol << "'" << std::endl;
            #endif
            
            // (AR) تحويل رمز العامل إلى اسم آمن للدالة
            // (EN) Convert operator symbol to safe function name
            std::string opSafeName;
            if (operatorDecl->operatorSymbol == "+") opSafeName = "__op_add__";
            else if (operatorDecl->operatorSymbol == "-") opSafeName = "__op_sub__";
            else if (operatorDecl->operatorSymbol == "*") opSafeName = "__op_mul__";
            else if (operatorDecl->operatorSymbol == "/") opSafeName = "__op_div__";
            else if (operatorDecl->operatorSymbol == "%") opSafeName = "__op_mod__";
            else if (operatorDecl->operatorSymbol == "**") opSafeName = "__op_pow__";
            else if (operatorDecl->operatorSymbol == "==") opSafeName = "__op_eq__";
            else if (operatorDecl->operatorSymbol == "!=") opSafeName = "__op_ne__";
            else if (operatorDecl->operatorSymbol == "<") opSafeName = "__op_lt__";
            else if (operatorDecl->operatorSymbol == "<=") opSafeName = "__op_le__";
            else if (operatorDecl->operatorSymbol == ">") opSafeName = "__op_gt__";
            else if (operatorDecl->operatorSymbol == ">=") opSafeName = "__op_ge__";
            else opSafeName = "__op_" + operatorDecl->operatorSymbol + "__";
            
            // (AR) استنتاج نوع الإرجاع إذا لم يُحدد
            // (EN) Infer return type if not specified
            SIRType returnType;
            if (operatorDecl->returnType == Data::DataType::UNKNOWN ||
                operatorDecl->returnType == Data::DataType::NONE) {
                returnType = inferReturnTypeFromBody(operatorDecl->body.get());
            } else {
                returnType = astTypeToSIRType(operatorDecl->returnType);
            }
            std::string fullOpName = classDecl->name + "." + opSafeName;
            auto sirOpFunc = std::make_shared<SIRFunction>(fullOpName, returnType);
            
            // (AR) المعامل الأول: self (الكائن الحالي)
            // (EN) First param: self (current object)
            sirOpFunc->addParameter(SIRParameter("self", SIRType::I64));
            
            // (AR) المعامل الثاني: المعامل الآخر (other)
            // (EN) Second param: other operand
            for (const auto& param : operatorDecl->parameters) {
                SIRType paramType = astTypeToSIRType(param.type);
                sirOpFunc->addParameter(SIRParameter(param.name, paramType));
            }
            
            sirClass->addMethod(sirOpFunc);
            
            // (AR) بناء جسم العامل
            // (EN) Build operator body
            if (operatorDecl->body) {
                auto prevFunction = currentFunction_;
                auto prevBlock = currentBlock_;
                auto prevClassName = currentClassName_;
                
                currentFunction_ = sirOpFunc;
                currentClassName_ = classDecl->name;
                
                enterScope();
                
                {
                    VariableInfo selfInfo;
                    selfInfo.name = "self";
                    selfInfo.type = SIRType::I64;
                    selfInfo.registerName = "%self";
                    selfInfo.isGlobal = false;
                    selfInfo.isMutable = false;
                    selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(selfInfo);
                }
                
                // (AR) تسجيل "هذا" كمرادف لـ self
                // (EN) Register "هذا" (this) as alias for self
                {
                    VariableInfo thisInfo;
                    thisInfo.name = "\xD9\x87\xD8\xB0\xD8\xA7"; // هذا
                    thisInfo.type = SIRType::I64;
                    thisInfo.registerName = "%self";
                    thisInfo.isGlobal = false;
                    thisInfo.isMutable = false;
                    thisInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(thisInfo);
                }
                
                for (const auto& param : operatorDecl->parameters) {
                    VariableInfo paramInfo;
                    paramInfo.name = param.name;
                    paramInfo.type = astTypeToSIRType(param.type);
                    paramInfo.registerName = "%" + param.name;
                    paramInfo.isGlobal = false;
                    paramInfo.isMutable = false;
                    paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(paramInfo);
                }
                
                for (const auto& field : sirClass->fields_) {
                    VariableInfo fieldInfo;
                    fieldInfo.name = field.first;
                    fieldInfo.type = field.second;
                    fieldInfo.registerName = "%" + field.first;
                    fieldInfo.isGlobal = false;
                    fieldInfo.isMutable = true;
                    fieldInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                    addVariable(fieldInfo);
                }
                
                auto entryBlock = createBasicBlock("entry");
                sirOpFunc->addBasicBlock(entryBlock);
                currentBlock_ = entryBlock;
                
                for (const auto& field : sirClass->fields_) {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register("%" + field.first, field.second);
                    currentBlock_->addInstruction(allocInst);
                }
                
                buildStatement(operatorDecl->body.get());
                
                if (currentBlock_) {
                    bool hasTerminator = false;
                    if (!currentBlock_->instructions.empty()) {
                        auto lastOp = currentBlock_->instructions.back().opcode;
                        hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                    }
                    if (!hasTerminator) {
                        SIRInstruction retInst;
                        if (returnType == SIRType::VOID) {
                            retInst.opcode = SIROpcode::RET_VOID;
                        } else {
                            retInst.opcode = SIROpcode::RET;
                            if (returnType == SIRType::STRING) {
                                retInst.operands.push_back(SIROperand::ConstantString(""));
                            } else {
                                retInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                        }
                        currentBlock_->addInstruction(retInst);
                    }
                }
                
                exitScope();
                
                module_->addFunction(sirOpFunc);
                
                FunctionInfo opInfo;
                opInfo.name = fullOpName;
                opInfo.returnType = returnType;
                opInfo.parameters = sirOpFunc->getParameters();
                opInfo.sirFunction = sirOpFunc;
                functionTable_[fullOpName] = opInfo;
                
                currentFunction_ = prevFunction;
                currentBlock_ = prevBlock;
                currentClassName_ = prevClassName;
            }
        }
    }
    
    // (AR) إضافة الصنف للوحدة (sir_module.h:608 - addClass)
    // (EN) Add class to module
    module_->addClass(sirClass);
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildClass: class '" << classDecl->name << "' added with " 
              << sirClass->fields_.size() << " fields and "
              << sirClass->methods_.size() << " methods" << std::endl;
    #endif
}

// ============================================================================
// (AR) بناء سمة/واجهة / (EN) Build Trait/Interface
// ============================================================================
// السمة هي عقد موصفة — لا تولّد كوداً مباشراً، بل تُسجَّل للتحقق والتوثيق
// Traits are descriptive nodes — no direct code generated, registered for validation
// ============================================================================
void SIRBuilder::buildTrait(AST::TraitDecl* traitDecl) {
    if (!traitDecl) return;
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildTrait: registering trait '" << traitDecl->name 
              << "' with " << traitDecl->methods.size() << " methods" << std::endl;
    #endif
    
    // (AR) السمات لا تولّد كوداً — فقط تُسجَّل كبيانات وصفية
    // (EN) Traits don't generate code — they're registered as metadata
    // (AR) يمكن تخزينها في وحدة SIR إذا احتجنا للتحقق لاحقاً
    // (EN) Could be stored in SIR module for later validation if needed
}

// ============================================================================
// (AR) بناء كتلة تنفيذ سمة / (EN) Build Impl Block
// ============================================================================
// كتلة التنفيذ تضيف دوال إلى صنف موجود
// Impl block adds methods to an existing class
// ============================================================================
void SIRBuilder::buildImpl(AST::ImplDecl* implDecl) {
    if (!implDecl) return;
    
    std::string className = implDecl->targetType;
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildImpl: implementing";
    #endif
    if (!implDecl->traitName.empty()) {
        std::cout << " trait '" << implDecl->traitName << "'";
    }
    std::cout << " for class '" << className << "'" << std::endl;
    
    // (AR) البحث عن الصنف المستهدف في وحدة SIR
    // (EN) Find target class in SIR module
    auto sirClass = module_->getClass(className);
    if (!sirClass) {
        std::cerr << "[WARNING] buildImpl: class '" << className 
                  << "' not found in module" << std::endl;
        return;
    }
    
    // (AR) معالجة الدوال في كتلة التنفيذ
    // (EN) Process methods in impl block
    for (const auto& method : implDecl->methods) {
        if (!method) continue;
        
        auto funcDecl = dynamic_cast<AST::FunctionDecl*>(method.get());
        if (!funcDecl) continue;
        
        std::string fullMethodName = className + "." + funcDecl->name;
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildImpl: adding method '" << fullMethodName << "'" << std::endl;
        #endif
        
        // (AR) تحويل نوع الإرجاع
        // (EN) Convert return type
        SIR::SIRType retType = astTypeToSIRType(funcDecl->returnType);
        auto sirMethod = std::make_shared<SIR::SIRFunction>(fullMethodName, retType);
        
        // (AR) إضافة معامل self ضمنياً
        // (EN) Add implicit self parameter
        sirMethod->addParameter(SIR::SIRParameter("self", SIR::SIRType::I64));
        
        // (AR) إضافة المعاملات
        // (EN) Add parameters
        for (const auto& param : funcDecl->parameters) {
            SIR::SIRType paramType = astTypeToSIRType(param.type);
            sirMethod->addParameter(SIR::SIRParameter(param.name, paramType));
        }
        
        // (AR) إضافة الدالة للصنف
        // (EN) Add method to class
        sirClass->addMethod(sirMethod);
        
        // (AR) بناء جسم الدالة
        // (EN) Build method body
        if (funcDecl->body) {
            auto prevFunction = currentFunction_;
            auto prevBlock = currentBlock_;
            auto prevClassName = currentClassName_;
            
            currentFunction_ = sirMethod;
            currentClassName_ = className;
            
            enterScope();
            
            // (AR) تسجيل معامل self
            // (EN) Register self parameter
            {
                VariableInfo selfInfo;
                selfInfo.name = "self";
                selfInfo.type = SIR::SIRType::I64;
                selfInfo.registerName = "%self";
                selfInfo.isGlobal = false;
                selfInfo.isMutable = false;
                selfInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                addVariable(selfInfo);
            }
            
            // (AR) تسجيل المعاملات
            // (EN) Register parameters
            for (const auto& param : funcDecl->parameters) {
                VariableInfo paramInfo;
                paramInfo.name = param.name;
                paramInfo.type = astTypeToSIRType(param.type);
                paramInfo.registerName = "%" + param.name;
                paramInfo.isGlobal = false;
                paramInfo.isMutable = true;
                paramInfo.scopeLevel = static_cast<int>(scopeStack_.size());
                addVariable(paramInfo);
            }
            
            // (AR) بناء جسم الدالة
            // (EN) Build function body
            if (auto blockStmt = dynamic_cast<Sad::AST::BlockStmt*>(funcDecl->body.get())) {
                for (const auto& bodyStmt : blockStmt->statements) {
                    if (bodyStmt) {
                        buildStatement(bodyStmt.get());
                    }
                }
            } else {
                buildStatement(funcDecl->body.get());
            }
            
            exitScope();
            
            currentFunction_ = prevFunction;
            currentBlock_ = prevBlock;
            currentClassName_ = prevClassName;
        }
        
        // (AR) إضافة الدالة للوحدة
        // (EN) Add function to module
        module_->addFunction(sirMethod);
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildImpl: finished impl for '" << className << "'" << std::endl;
    #endif
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
