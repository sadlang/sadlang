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
#include <optional>

namespace Sad {
namespace Compiler {
namespace SIR {

// ============================================================================
// buildFunctionCall - بناء استدعاء دالة
// ============================================================================
// مصدر التعريف / Source: sir_builder.h:464
// التوقيع / Signature: BuildResult buildFunctionCall(AST::FunctionCallNode* call);
//
// المعاملات / Parameters:
// - call: AST::FunctionCallNode* = Sad::AST::CallExpr* (sir_builder.h:68)
//
// CallExpr Members (expressions.h:276):
// - callee: ExprPtr (line 278)
// - arguments: ExprList (line 279)
//
// الدوال المضمنة / Builtin Functions (sir_types.h:178-187):
// - طول() / length: STRING_LEN للنصوص، ARRAY_LEN للمصفوفات
// - اطبع() / print: BUILTIN_PRINT
// - إلى_رقم() / to_int: STRING_TO_I64
// - إلى_نص() / to_string: I64_TO_STRING / F64_TO_STRING
// ============================================================================
BuildResult SIRBuilder::buildFunctionCall(AST::FunctionCallNode* call) {
    if (!call) {
        return BuildResult();
    }
    std::cerr << "[EXC-DBG] buildFunctionCall entered" << std::endl;
    
    // ========================================================================
    // (AR) الخطوة 1: استخراج اسم الدالة من callee
    // (EN) Step 1: Extract function name from callee
    // المصدر: expressions.h:290 - callee: ExprPtr
    // ========================================================================
    std::string funcName;
    
    // (AR) callee يمكن أن يكون VariableExpr (اسم دالة مباشر)
    // (EN) callee can be VariableExpr (direct function name)
    // المصدر: expressions.h:210-212 - VariableExpr::name
    if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr*>(call->callee.get())) {
        funcName = varExpr->name;
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: function name = '" << funcName << "'" << std::endl;
        #endif
    } else {
        // (AR) لا ندعم استدعاءات غير مباشرة حالياً
        // (EN) We don't support indirect calls currently
        errors_.push_back("Error: Only direct function calls are supported");
        return BuildResult();
    }
    
    // ========================================================================
    // (AR) التعامل مع استدعاء باني الأب: أساس(...) / الأساس(...) / super(...)
    // (EN) Handle super constructor call: أساس(...) / الأساس(...) / super(...)
    // ========================================================================
    if ((funcName == "\xD8\xA3\xD8\xB3\xD8\xA7\xD8\xB3" ||       // أساس
         funcName == "\xD8\xA7\xD9\x84\xD8\xA3\xD8\xB3\xD8\xA7\xD8\xB3" || // الأساس
         funcName == "\xD8\xA7\xD8\xB3\xD8\xA7\xD8\xB3" ||       // اساس
         funcName == "\xD8\xA7\xD9\x84\xD8\xA7\xD8\xB3\xD8\xA7\xD8\xB3" || // الاساس
         funcName == "super") && !currentClassName_.empty()) {
        // (AR) البحث عن الصنف الأب
        // (EN) Find parent class
        auto sirClass = module_->getClass(currentClassName_);
        if (sirClass && !sirClass->parentClass.empty()) {
            std::string parentCtorName = sirClass->parentClass + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // بناء
            
            // (AR) بناء المعاملات
            // (EN) Build arguments
            std::vector<SIROperand> superArgs;
            superArgs.push_back(SIROperand::Register("%self", SIRType::I64)); // self
            for (const auto& arg : call->arguments) {
                BuildResult argResult = buildExpression(arg.get());
                superArgs.push_back(SIROperand::Register(argResult.registerName, argResult.type));
            }
            
            // (AR) إصدار تعليمة CALL لباني الأب
            // (EN) Emit CALL instruction for parent constructor
            std::string superResultReg = newTempRegister();
            SIRInstruction callInst;
            callInst.opcode = SIROpcode::CALL;
            callInst.result = SIROperand::Register(superResultReg, SIRType::VOID);
            callInst.operands.push_back(SIROperand::Register(parentCtorName, SIRType::VOID));
            for (auto& op : superArgs) {
                callInst.operands.push_back(op);
            }
            if (currentBlock_) currentBlock_->addInstruction(callInst);
            
            return BuildResult(superResultReg, SIRType::VOID);
        }
        // (AR) لا يوجد صنف أب - نتجاهل
        // (EN) No parent class - ignore
        return BuildResult();
    }
    
    // ========================================================================
    // (AR) الخطوة 2: بناء المعاملات (arguments)
    // (EN) Step 2: Build arguments
    // المصدر: expressions.h:291 - arguments: ExprList = vector<ExprPtr>
    // ========================================================================
    std::vector<SIROperand> argOperands;
    std::vector<BuildResult> argResults;
    
    for (const auto& arg : call->arguments) {
        // (AR) بناء كل معامل باستخدام buildExpression (sir_builder.h:440)
        // (EN) Build each argument using buildExpression
        BuildResult argResult = buildExpression(arg.get());
        
        if (argResult.registerName.empty()) {
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildFunctionCall: failed to build argument" << std::endl;
            #endif
            errors_.push_back("Error: Failed to build function argument");
            return BuildResult();
        }
        
        argResults.push_back(argResult);
        
        // (AR) إنشاء SIROperand للمعامل (sir_types.h:355-362 - Register() أو Constant*)
        // (EN) Create SIROperand for argument
        SIROperand argOp;
        
        // (AR) إذا كان المعامل ثابتاً، استخدم Constant* factory methods
        // (EN) If argument is constant, use Constant* factory methods
        if (argResult.isConstant) {
            switch (argResult.type) {
                case SIRType::STRING:
                    // استخدام ConstantString (sir_types.h:353-361)
                    argOp = SIROperand::ConstantString(argResult.constantValue);
                    #ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: added STRING constant='" 
                              << argResult.constantValue << "'" << std::endl;
                    #endif
                    break;
                case SIRType::I64: {
                    int64_t intVal = std::stoll(argResult.constantValue);
                    argOp = SIROperand::ConstantI64(intVal);
                    #ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: added I64 constant=" 
                              << intVal << std::endl;
                    #endif
                    break;
                }
                case SIRType::F64: {
                    double floatVal = std::stod(argResult.constantValue);
                    argOp = SIROperand::ConstantF64(floatVal);
                    #ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: added F64 constant=" 
                              << floatVal << std::endl;
                    #endif
                    break;
                }
                case SIRType::BOOL: {
                    bool boolVal = (argResult.constantValue == "true");
                    argOp = SIROperand::ConstantBool(boolVal);
                    #ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: added BOOL constant=" 
                              << boolVal << std::endl;
                    #endif
                    break;
                }
                default:
                    // للأنواع الأخرى، استخدم Register كافتراضي
                    argOp = SIROperand::Register(argResult.registerName, argResult.type);
                    #ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: added register='" 
                              << argResult.registerName << "', type=" 
                              << static_cast<int>(argResult.type) << std::endl;
                    #endif
                    break;
            }
        } else {
            // (AR) للمتغيرات، استخدم Register
            // (EN) For variables, use Register
            argOp = SIROperand::Register(argResult.registerName, argResult.type);
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildFunctionCall: added register='" 
                      << argResult.registerName << "', type=" 
                      << static_cast<int>(argResult.type) << std::endl;
            #endif
        }
        
        argOperands.push_back(argOp);
    }
    
    // ========================================================================
    // (AR) الخطوة 2.5: التحقق من الدوال المضمنة
    // (EN) Step 2.5: Check for builtin functions
    // ========================================================================
    
    // ========================================================================
    // (AR) إصلاح: تحقق أولاً إذا كانت الدالة معرّفة من المستخدم
    // (EN) Fix: First check if function is user-defined before checking builtins
    // هذا يمنع الدوال المضمنة من التداخل مع دوال المستخدم التي تحمل نفس الاسم
    // This prevents builtins from shadowing user-defined functions with same name
    // ========================================================================
    bool isUserDefinedFunction = (functionTable_.find(funcName) != functionTable_.end());
    std::cerr << "[EXC-DBG] buildFunctionCall: funcName='" << funcName 
              << "' isUserDefined=" << isUserDefinedFunction << std::endl;
    
    // (AR) دالة طول() - STRING_LEN للنصوص، ARRAY_LEN للمصفوفات
    // (EN) length() function - STRING_LEN for strings, ARRAY_LEN for arrays
    
    // ========================================================================
    // (AR) ״§„״×״­‚‚ …† ״§„״¯ˆ״§„ ״§„…״¯…״¬״© (…‚״³…״© ״¥„‰ …„†)
    // (EN) Check builtin functions (split into two files)
    // ========================================================================
    auto builtinResult = buildBuiltinCallCore(funcName, isUserDefinedFunction, argResults, argOperands);
    if (builtinResult.has_value()) {
        std::cerr << "[EXC-DBG] buildFunctionCall: handled by BuiltinCore! type=" 
                  << static_cast<int>(builtinResult.value().type) << std::endl;
        return builtinResult.value();
    }
    builtinResult = buildBuiltinCallSystem(funcName, isUserDefinedFunction, argResults, argOperands);
    if (builtinResult.has_value()) {
        std::cerr << "[EXC-DBG] buildFunctionCall: handled by BuiltinSystem! type="
                  << static_cast<int>(builtinResult.value().type) << std::endl;
        return builtinResult.value();
    }

    // ========================================================================
    // (AR) الخطوة 3: البحث عن الدالة والحصول على نوع الإرجاع
    // (EN) Step 3: Look up function and get return type
    // المصدر: sir_builder.h:719 - functionTable_
    // المصدر: sir_builder.h:162-175 - FunctionInfo struct
    // ========================================================================
    SIRType returnType = SIRType::VOID;  // (AR) افتراضياً void
    
    auto it = functionTable_.find(funcName);
    if (it != functionTable_.end()) {
        // (AR) الدالة موجودة - استخدم نوع الإرجاع (sir_builder.h:165)
        // (EN) Function found - use return type
        returnType = it->second.returnType;
        std::cerr << "[EXC-DBG] buildFunctionCall: found '" << funcName 
                  << "' retType=" << static_cast<int>(returnType) << std::endl;
        
        // ================================================================
        // (AR) استنتاج أنواع المعاملات من موقع الاستدعاء
        //      إذا كان المعامل I64 (افتراضي/غير محدد) والوسيط STRING/F64/BOOL
        //      نحدّث نوع المعامل ليطابق نوع الوسيط الفعلي
        // (EN) Infer parameter types from call-site arguments
        //      If param is I64 (default/unknown) and arg is STRING/F64/BOOL,
        //      update param type to match actual arg type
        // ================================================================
        auto& funcInfo = it->second;
        for (size_t i = 0; i < argResults.size() && i < funcInfo.parameters.size(); i++) {
            SIRType argType = argResults[i].type;
            SIRType& paramType = funcInfo.parameters[i].type;
            
            if (paramType == SIRType::I64 && argType == SIRType::STRING) {
                // (AR) تحديث إلى STRING — المعامل يستقبل نصوصاً
                // (EN) Update to STRING — parameter receives strings
                paramType = SIRType::STRING;
                if (funcInfo.sirFunction && i < funcInfo.sirFunction->parameters.size()) {
                    funcInfo.sirFunction->parameters[i].type = SIRType::STRING;
                }
            } else if (paramType == SIRType::STRING && argType == SIRType::I64) {
                // (AR) تعارض: المعامل سبق تحديثه إلى STRING لكن يُستدعى بـ I64
                //      نعيده إلى I64 (النوع الشامل الافتراضي)
                // (EN) Conflict: param was updated to STRING but called with I64
                //      Revert to I64 (default catch-all type)
                paramType = SIRType::I64;
                if (funcInfo.sirFunction && i < funcInfo.sirFunction->parameters.size()) {
                    funcInfo.sirFunction->parameters[i].type = SIRType::I64;
                }
            }
        }
        
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: function found, returnType=" 
                  << static_cast<int>(returnType) << std::endl;
        #endif
    } else {
        // (AR) الدالة غير موجودة - تحقق إذا كانت قالباً
        // (EN) Function not found - check if it's a template
        if (templateFunctions_.find(funcName) != templateFunctions_.end()) {
            std::cout << "[Template] Found template function: " << funcName << std::endl;
            
            // (AR) استنتاج الأنواع من المعاملات
            // (EN) Infer types from arguments
            std::vector<SIRType> inferredTypes;
            for (const auto& argResult : argResults) {
                inferredTypes.push_back(argResult.type);
            }
            
            // (AR) إنشاء نسخة من القالب مع الأنواع المستنتجة
            // (EN) Instantiate template with inferred types
            std::string instanceName = instantiateTemplate(funcName, inferredTypes);
            
            if (instanceName.empty()) {
                std::cerr << "[Template Error] Failed to instantiate template: " << funcName << std::endl;
                errors_.push_back("Error: Failed to instantiate template '" + funcName + "'");
                return BuildResult();
            }
            
            // (AR) استخدم اسم النسخة المُنشأة
            // (EN) Use the instantiated function name
            funcName = instanceName;
            
            // (AR) ابحث عن نوع الإرجاع للنسخة المُنشأة
            // (EN) Look up return type for instantiated function
            auto instIt = functionTable_.find(funcName);
            if (instIt != functionTable_.end()) {
                returnType = instIt->second.returnType;
            } else {
                std::cerr << "[Template Error] Instantiated function not in function table: " << funcName << std::endl;
            }
            
            std::cout << "[Template] Using instantiated function: " << funcName << std::endl;
        } else {
            // (AR) قد تكون دالة مدمجة غير مسجلة بعد
            // (EN) May be a builtin function not registered yet
            #ifndef NDEBUG
            std::cout << "[DEBUG] buildFunctionCall: function '" << funcName 
                      << "' not found (no template), assuming VOID return" << std::endl;
            #endif
        }
    }
    
    // ========================================================================
    // (AR) الخطوة 4: إنشاء سجل النتيجة وتعليمة الاستدعاء
    // (EN) Step 4: Create result register and call instruction
    // المصدر: sir_instruction.h:231-238 - SIRInstruction::Call()
    // المصدر: sir_types.h:390-395 - SIROperand::Function()
    // ========================================================================
    std::string resultReg = newTempRegister();
    
    // (AR) إنشاء معامل الدالة (sir_types.h:390)
    // (EN) Create function operand
    SIROperand funcOp = SIROperand::Function(funcName);
    
    // (AR) إنشاء معامل النتيجة (sir_types.h:355)
    // (EN) Create result operand
    SIROperand resultOp = SIROperand::Register(resultReg, returnType);
    
    // (AR) إنشاء تعليمة الاستدعاء (sir_instruction.h:231)
    // (EN) Create call instruction
    // SIRInstruction::Call(result, function, args)
    SIRInstruction callInst = SIRInstruction::Call(resultOp, funcOp, argOperands);
    
    // ========================================================================
    // (AR) الخطوة 5: إضافة التعليمة إلى الكتلة الحالية
    // (EN) Step 5: Add instruction to current block
    // المصدر: sir_builder.h:712 - currentBlock_
    // ========================================================================
    if (currentBlock_) {
        currentBlock_->instructions.push_back(callInst);
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: added CALL instruction to block" << std::endl;
        #endif
    } else {
        #ifndef NDEBUG
        std::cout << "[DEBUG] buildFunctionCall: WARNING - no current block!" << std::endl;
        #endif
    }
    
    // ========================================================================
    // (AR) الخطوة 6: إذا كانت الدالة مولّد، أضف GENERATOR_CONSUME لجمع القيم
    // (EN) Step 6: If callee is a generator, add GENERATOR_CONSUME to collect values
    // ========================================================================
    if (it != functionTable_.end() && it->second.isGenerator) {
        std::cerr << "[GEN] Emitting GENERATOR_CONSUME for generator '" << funcName << "'" << std::endl;
        
        // (AR) CALL أعاد المقبض (PTR) — الآن نستهلكه
        // (EN) CALL returned the handle (PTR) — now consume it
        std::string consumeReg = newTempRegister();
        SIRInstruction consumeInst;
        consumeInst.opcode = SIROpcode::GENERATOR_CONSUME;
        consumeInst.result = SIROperand::Register(consumeReg, SIRType::I64);
        consumeInst.operands.push_back(SIROperand::Register(resultReg, SIRType::PTR));
        consumeInst.comment = "consume generator yields";
        
        if (currentBlock_) {
            currentBlock_->addInstruction(consumeInst);
        }
        
        return BuildResult(consumeReg, SIRType::I64);
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildFunctionCall: returning result reg='" << resultReg 
              << "', type=" << static_cast<int>(returnType) << std::endl;
    #endif
    
    return BuildResult(resultReg, returnType);
}

// ============================================================================
// buildNewObject - بناء تعبير إنشاء كائن جديد
// ============================================================================
// مصدر التعريف / Source: class_nodes.h:164
// التوقيع / Signature: BuildResult buildNewObject(AST::NewExpr* newExpr);
//
// المعاملات / Parameters:
// - newExpr: AST::NewExpr* (class_nodes.h:164)
//
// NewExpr Members:
// - className: std::string (line 167)
// - arguments: std::vector<std::unique_ptr<Expr>> (line 168)
// ============================================================================
BuildResult SIRBuilder::buildNewObject(AST::NewExpr* newExpr) {
    if (!newExpr) {
        return BuildResult();
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildNewObject: creating object of class '" 
              << newExpr->className << "'" << std::endl;
    #endif
    
    // (AR) الخطوة 1: البحث عن الصنف في الوحدة
    // (EN) Step 1: Find class in module
    auto sirClass = module_->getClass(newExpr->className);
    if (!sirClass) {
        errors_.push_back("Class not found: " + newExpr->className);
        return BuildResult();
    }
    
    // (AR) الخطوة 2: تخصيص ذاكرة للكائن
    // (EN) Step 2: Allocate memory for object
    std::string objReg = newTempRegister();
    
    if (currentBlock_) {
        SIRInstruction allocInst;
        allocInst.opcode = SIROpcode::ALLOC;
        allocInst.result = SIROperand::Register(objReg, SIRType::I64);
        // (AR) إضافة اسم الصنف كـ metadata
        // (EN) Add class name as metadata
        allocInst.operands.push_back(SIROperand::ConstantString(newExpr->className));
        currentBlock_->addInstruction(allocInst);
    }
    
    // (AR) الخطوة 3: استدعاء دالة البناء (constructor) إن وجدت
    // (EN) Step 3: Call constructor if exists
    std::string constructorName = newExpr->className + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // بناء
    auto constructor = sirClass->getMethod(constructorName);
    
    if (constructor || !newExpr->arguments.empty()) {
        // (AR) بناء معاملات البناء
        // (EN) Build constructor arguments
        std::vector<SIROperand> args;
        
        // (AR) المعامل الأول هو الكائن نفسه (self)
        // (EN) First argument is the object itself (self)
        args.push_back(SIROperand::Register(objReg, SIRType::I64));
        
        // (AR) باقي المعاملات
        // (EN) Rest of arguments
        for (const auto& arg : newExpr->arguments) {
            auto argResult = buildExpression(arg.get());
            if (argResult.isConstant && !argResult.constantValue.empty()) {
                // تحويل الثابت حسب النوع
                switch (argResult.type) {
                    case SIRType::I64:
                        args.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                        break;
                    case SIRType::F64:
                        args.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                        break;
                    case SIRType::STRING:
                        args.push_back(SIROperand::ConstantString(argResult.constantValue));
                        break;
                    default:
                        args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                        break;
                }
            } else {
                args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
            }
        }
        
        // (AR) إنشاء تعليمة استدعاء البناء
        // (EN) Create constructor call instruction
        if (currentBlock_) {
            SIRInstruction callInst;
            callInst.opcode = SIROpcode::CALL;
            callInst.result = SIROperand::Register(newTempRegister(), SIRType::VOID);
            callInst.operands.push_back(SIROperand::Function(constructorName));
            for (const auto& arg : args) {
                callInst.operands.push_back(arg);
            }
            currentBlock_->addInstruction(callInst);
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) استنتاج أنواع الحقول من أنواع الوسائط الفعلية
        // (EN) Infer field types from actual argument types at call site
        // ═══════════════════════════════════════════════════════════════
        if (constructor) {
            const auto& params = constructor->getParameters();
            // params[0] = self, params[1..N] = user params
            // args[0] = self, args[1..N] = user args
            
            // (AR) بناء خريطة: اسم_المعامل → نوع_الوسيط
            // (EN) Build map: paramName → argType
            std::unordered_map<std::string, SIRType> paramTypes;
            for (size_t i = 1; i < params.size() && i < args.size(); i++) {
                paramTypes[params[i].name] = args[i].dataType;
            }
            
            // (AR) تحديث حقول الصنف الحالي
            // (EN) Update current class fields
            if (!sirClass->paramToFieldMap_.empty()) {
                for (auto& [paramName, argType] : paramTypes) {
                    auto fieldIt = sirClass->paramToFieldMap_.find(paramName);
                    if (fieldIt != sirClass->paramToFieldMap_.end()) {
                        const std::string& fieldName = fieldIt->second;
                        auto currentType = sirClass->fields_.find(fieldName);
                        if (currentType != sirClass->fields_.end() && 
                            currentType->second == SIRType::PTR &&
                            argType != SIRType::PTR && argType != SIRType::VOID) {
                            sirClass->fields_[fieldName] = argType;
                            #ifndef NDEBUG
                            std::cout << "[DEBUG] buildNewObject: inferred field '" << fieldName
                                      << "' type=" << static_cast<int>(argType) 
                                      << " from arg '" << paramName << "'" << std::endl;
                            #endif
                        }
                    }
                }
            }
            
            // ═══════════════════════════════════════════════════════════════
            // (AR) نشر أنواع الوسائط عبر سلسلة الوراثة (super constructor chain)
            // (EN) Propagate arg types through inheritance chain (super ctor chain)
            // ═══════════════════════════════════════════════════════════════
            auto currentClass = sirClass;
            auto currentParamTypes = paramTypes;
            
            while (currentClass && !currentClass->parentClass.empty() && 
                   !currentClass->superParamMapping_.empty()) {
                auto parentSirClass = module_->getClass(currentClass->parentClass);
                if (!parentSirClass) break;
                
                // (AR) الحصول على معاملات باني الأب
                // (EN) Get parent constructor params
                std::string parentCtorName = currentClass->parentClass + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1";
                auto parentCtor = parentSirClass->getMethod(parentCtorName);
                if (!parentCtor) break;
                
                const auto& parentParams = parentCtor->getParameters();
                
                // (AR) بناء خريطة أنواع معاملات الأب من superParamMapping
                // (EN) Build parent param types from superParamMapping
                std::unordered_map<std::string, SIRType> parentParamTypes;
                for (auto& [superIdx, childParamName] : currentClass->superParamMapping_) {
                    int parentIdx = superIdx + 1; // +1 (skip self)
                    if (parentIdx < static_cast<int>(parentParams.size())) {
                        auto it = currentParamTypes.find(childParamName);
                        if (it != currentParamTypes.end()) {
                            parentParamTypes[parentParams[parentIdx].name] = it->second;
                        }
                    }
                }
                
                // (AR) تحديث حقول الأب باستخدام paramToFieldMap
                // (EN) Update parent fields using paramToFieldMap
                for (auto& [parentParamName, inferredType] : parentParamTypes) {
                    auto fieldIt = parentSirClass->paramToFieldMap_.find(parentParamName);
                    if (fieldIt != parentSirClass->paramToFieldMap_.end()) {
                        const std::string& fieldName = fieldIt->second;
                        auto currentFldType = parentSirClass->fields_.find(fieldName);
                        if (currentFldType != parentSirClass->fields_.end() &&
                            currentFldType->second == SIRType::PTR &&
                            inferredType != SIRType::PTR && inferredType != SIRType::VOID) {
                            parentSirClass->fields_[fieldName] = inferredType;
                            // (AR) تحديث أيضاً في الأبناء (الحقول الموروثة)
                            // (EN) Also update in children (inherited fields)
                            if (sirClass->fields_.count(fieldName)) {
                                sirClass->fields_[fieldName] = inferredType;
                            }
                            if (currentClass->fields_.count(fieldName)) {
                                currentClass->fields_[fieldName] = inferredType;
                            }
                            #ifndef NDEBUG
                            std::cout << "[DEBUG] buildNewObject: propagated field '" << fieldName
                                      << "' type=" << static_cast<int>(inferredType)
                                      << " to parent '" << parentSirClass->name << "'" << std::endl;
                            #endif
                        }
                    }
                }
                
                // (AR) انتقل للأب التالي
                // (EN) Move to next parent
                currentClass = parentSirClass;
                currentParamTypes = parentParamTypes;
            }
        }
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildNewObject: object created in register '" << objReg << "'" << std::endl;
    #endif
    
    // (AR) تتبع نوع الكائن لدعم تحميل المعاملات الزائد
    // (EN) Track object type for operator overloading support
    classInstanceTypes_[objReg] = newExpr->className;
    
    // (AR) إرجاع مؤشر للكائن مع اسم الصنف — نوع STRUCT وليس I64
    // (EN) Return pointer to object with class name — STRUCT type not I64
    BuildResult result(objReg, SIRType::STRUCT);
    result.className = newExpr->className;
    return result;
}

// ============================================================================
// buildMemberAccess - بناء تعبير الوصول لعضو في كائن
// ============================================================================
// مصدر التعريف / Source: class_nodes.h:206
// التوقيع / Signature: BuildResult buildMemberAccess(AST::MemberAccessExpr* memberExpr);
//
// المعاملات / Parameters:
// - memberExpr: AST::MemberAccessExpr* (class_nodes.h:206)
//
// MemberAccessExpr Members:
// - object: std::unique_ptr<Expr> (line 209)
// - memberName: std::string (line 210)
// ============================================================================
BuildResult SIRBuilder::buildMemberAccess(AST::MemberAccessExpr* memberExpr) {
    if (!memberExpr) {
        return BuildResult();
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildMemberAccess: accessing member '" 
              << memberExpr->memberName << "'" << std::endl;
    #endif
    
    // (AR) الخطوة 1: بناء تعبير الكائن
    // (EN) Step 1: Build object expression
    auto objResult = buildExpression(memberExpr->object.get());
    
    // (AR) الخطوة 2: إنشاء تعليمة الوصول للعضو
    // (EN) Step 2: Create member access instruction
    std::string resultReg = newTempRegister();
    
    // (AR) محاولة استنتاج نوع العضو من جدول الأصناف
    // (EN) Try to infer member type from class table
    SIRType memberType = SIRType::I64;  // (AR) افتراضي
    if (!objResult.className.empty()) {
        auto classIt = classTable_.find(objResult.className);
        if (classIt != classTable_.end()) {
            auto& sirClass = classIt->second;
            // (AR) البحث عن نوع الحقل في الصنف (fields_ هي unordered_map<string, SIRType>)
            auto fieldIt = sirClass->fields_.find(memberExpr->memberName);
            if (fieldIt != sirClass->fields_.end()) {
                memberType = fieldIt->second;
            }
        }
    }
    
    if (currentBlock_) {
        SIRInstruction loadInst;
        loadInst.opcode = SIROpcode::LOAD;
        loadInst.result = SIROperand::Register(resultReg, memberType);
        
        // (AR) المعامل الأول: الكائن
        // (EN) First operand: object
        loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
        
        // (AR) المعامل الثاني: اسم العضو (كـ offset أو اسم)
        // (EN) Second operand: member name (as offset or name)
        loadInst.operands.push_back(SIROperand::ConstantString(memberExpr->memberName));
        
        currentBlock_->addInstruction(loadInst);
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildMemberAccess: result in register '" << resultReg 
              << "' with type " << sirTypeToString(memberType) << std::endl;
    #endif
    
    BuildResult result(resultReg, memberType);
    result.className = objResult.className;
    return result;
}

// ============================================================================
// buildMethodCall - بناء استدعاء طريقة على كائن
// ============================================================================
// مصدر التعريف / Source: class_nodes.h:245
// التوقيع / Signature: BuildResult buildMethodCall(AST::MethodCallExpr* methodCallExpr);
//
// MethodCallExpr Members:
// - object: std::unique_ptr<Expr> (line 247)
// - methodName: std::string (line 248)
// - arguments: std::vector<std::unique_ptr<Expr>> (line 249)
// ============================================================================
BuildResult SIRBuilder::buildMethodCall(AST::MethodCallExpr* methodCallExpr) {
    if (!methodCallExpr) {
        return BuildResult();
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildMethodCall: calling method '" 
              << methodCallExpr->methodName << "'" << std::endl;
    #endif
    
    // (AR) الخطوة 1: بناء تعبير الكائن
    // (EN) Step 1: Build object expression
    auto objResult = buildExpression(methodCallExpr->object.get());
    
    // (AR) الخطوة 2: تحديد اسم الصنف من المتغير
    // (EN) Step 2: Determine class name from variable
    std::string className;
    
    // (AR) نحاول إيجاد اسم الصنف من VariableExpr
    // (EN) Try to find class name from VariableExpr
    if (auto varExpr = dynamic_cast<Sad::AST::VariableExpr*>(methodCallExpr->object.get())) {
        // (AR) البحث عن معلومات المتغير في classInstanceTypes_
        // (EN) Look up variable info in classInstanceTypes_
        if (classInstanceTypes_.find(varExpr->name) != classInstanceTypes_.end()) {
            className = classInstanceTypes_[varExpr->name];
        }
    }
    
    // (AR) إذا 'هذا' (this), استخدم currentClassName_
    // (EN) If 'this', use currentClassName_
    if (auto thisExpr = dynamic_cast<Sad::AST::ThisExpr*>(methodCallExpr->object.get())) {
        className = currentClassName_;
    }
    
    // (AR) إذا لم نجد اسم الصنف، نحاول من الصنف الحالي
    // (EN) If class name not found, try current class
    if (className.empty() && !currentClassName_.empty()) {
        className = currentClassName_;
    }
    
    // (AR) البحث عن الطريقة في جدول الدوال
    // (EN) Look up method in function table
    std::string fullMethodName = className + "." + methodCallExpr->methodName;
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildMethodCall: full method name = '" << fullMethodName << "'" << std::endl;
    #endif
    
    // (AR) تحديد نوع الإرجاع
    // (EN) Determine return type
    SIRType returnType = SIRType::VOID;
    if (functionTable_.find(fullMethodName) != functionTable_.end()) {
        returnType = functionTable_[fullMethodName].returnType;
    }
    
    // (AR) الخطوة 3: بناء المعاملات
    // (EN) Step 3: Build arguments
    std::vector<SIROperand> args;
    
    // (AR) المعامل الأول: self (مؤشر الكائن)
    // (EN) First argument: self (object pointer)
    args.push_back(SIROperand::Register(objResult.registerName, objResult.type));
    
    // (AR) باقي المعاملات
    // (EN) Rest of arguments
    for (const auto& arg : methodCallExpr->arguments) {
        auto argResult = buildExpression(arg.get());
        if (argResult.isConstant && !argResult.constantValue.empty()) {
            switch (argResult.type) {
                case SIRType::I64:
                    args.push_back(SIROperand::ConstantI64(std::stoll(argResult.constantValue)));
                    break;
                case SIRType::F64:
                    args.push_back(SIROperand::ConstantF64(std::stod(argResult.constantValue)));
                    break;
                case SIRType::STRING:
                    args.push_back(SIROperand::ConstantString(argResult.constantValue));
                    break;
                default:
                    args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                    break;
            }
        } else {
            args.push_back(SIROperand::Register(argResult.registerName, argResult.type));
        }
    }
    
    // ========================================================================
    // (AR) الخطوة 3.5: فحص الطرق المضمنة للمصفوفات
    // (EN) Step 3.5: Check for builtin array methods
    // ========================================================================
    std::string methodName = methodCallExpr->methodName;
    
    // (AR) أضف / push - إضافة عنصر للمصفوفة
    // (EN) append / push - add element to array
    if (methodName == "أضف" || methodName == "اضف" || 
        methodName == "push" || methodName == "append") {
        std::string resultReg = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_APPEND);
        inst.result = SIROperand::Register(resultReg, SIRType::VOID);
        // (AR) المعامل الأول: المصفوفة، الثاني: العنصر المُضاف
        // (EN) First operand: array, Second: element to add
        inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
        if (args.size() > 1) {
            inst.operands.push_back(args[1]); // (AR) العنصر (args[0] هو self)
        }
        if (currentBlock_) {
            currentBlock_->instructions.push_back(inst);
        }
        return BuildResult(resultReg, SIRType::VOID);
    }
    
    // (AR) حجم / size - الحصول على حجم المصفوفة
    // (EN) size / length - get array size
    if (methodName == "حجم" || methodName == "طول" ||
        methodName == "size" || methodName == "length" || methodName == "len") {
        std::string resultReg = newTempRegister();
        SIRInstruction inst(SIROpcode::ARRAY_LEN);
        inst.result = SIROperand::Register(resultReg, SIRType::I64);
        inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) أزل / pop - إزالة آخر عنصر
    // (EN) pop / remove - remove last element
    if (methodName == "أزل" || methodName == "ازل" ||
        methodName == "pop" || methodName == "remove_last") {
        std::string resultReg = newTempRegister();
        SIRInstruction inst(SIROpcode::BUILTIN_ARRAY_REMOVE);
        inst.result = SIROperand::Register(resultReg, SIRType::I64); // (AR) العنصر المُزال
        inst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
        if (currentBlock_) currentBlock_->instructions.push_back(inst);
        return BuildResult(resultReg, SIRType::I64);
    }
    
    // (AR) فارغة / empty - التحقق إذا كانت المصفوفة فارغة
    // (EN) empty / is_empty - check if array is empty
    // (AR) نستخدم ARRAY_LEN ونقارن بـ 0
    // (EN) Use ARRAY_LEN and compare with 0
    if (methodName == "فارغة" || methodName == "فارغ" ||
        methodName == "empty" || methodName == "is_empty") {
        // (AR) الخطوة 1: الحصول على الحجم
        // (EN) Step 1: Get size
        std::string sizeReg = newTempRegister();
        SIRInstruction sizeInst(SIROpcode::ARRAY_LEN);
        sizeInst.result = SIROperand::Register(sizeReg, SIRType::I64);
        sizeInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
        if (currentBlock_) currentBlock_->instructions.push_back(sizeInst);
        
        // (AR) الخطوة 2: مقارنة size == 0
        // (EN) Step 2: Compare size == 0
        std::string resultReg = newTempRegister();
        SIRInstruction cmpInst(SIROpcode::EQ);
        cmpInst.result = SIROperand::Register(resultReg, SIRType::BOOL);
        cmpInst.operands.push_back(SIROperand::Register(sizeReg, SIRType::I64));
        cmpInst.operands.push_back(SIROperand::ConstantI64(0));
        if (currentBlock_) currentBlock_->instructions.push_back(cmpInst);
        
        return BuildResult(resultReg, SIRType::BOOL);
    }
    
    // (AR) الخطوة 4: إنشاء تعليمة CALL
    // (EN) Step 4: Create CALL instruction
    std::string resultReg = newTempRegister();
    
    if (currentBlock_) {
        SIRInstruction callInst;
        callInst.opcode = SIROpcode::CALL;
        callInst.result = SIROperand::Register(resultReg, returnType);
        callInst.operands.push_back(SIROperand::Function(fullMethodName));
        for (const auto& arg : args) {
            callInst.operands.push_back(arg);
        }
        currentBlock_->addInstruction(callInst);
    }
    
    #ifndef NDEBUG
    std::cout << "[DEBUG] buildMethodCall: result in register '" << resultReg << "'" << std::endl;
    #endif
    
    return BuildResult(resultReg, returnType);
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
