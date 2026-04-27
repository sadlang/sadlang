// ============================================================================
// sir_builder_classes_operators.cpp
// ============================================================================
// (AR) تحميل العوامل الزائد — مستخرج من sir_builder_classes.cpp (CW-05)
//      تحويل رموز العوامل، بناء جسم العامل، تسجيل في b_.functionTable_
// (EN) Operator overloading — extracted from sir_builder_classes.cpp
//      Operator symbol conversion, body building, b_.functionTable_ registration
// ============================================================================

#include "sir_builder.h"
#include "builders/class_builder.h"
#include <string>
#include <iostream>
#include <functional>
#include <unordered_set>
#include "sir_builder.h"
#include "module_nodes.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            void ClassBuilder::buildClassOperator(
                AST::ClassDeclNode *classDecl,
                std::shared_ptr<SIRClass> sirClass,
                Sad::AST::OperatorDecl *operatorDecl)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildClass: found operator overload '" << operatorDecl->operatorSymbol << "'" << std::endl;
#endif

                // (AR) تحويل رمز العامل إلى اسم آمن للدالة
                //      يشمل جميع العوامل الـ 21 المدعومة في المفسر:
                //      - 13 عامل ثنائي (حسابية + مقارنة)
                //      - 3 عوامل خاصة: فهرسة []، إسناد بالفهرس []=، استدعاء ()
                //      - 3 عوامل أحادية: سالب u-، موجب u+، نفي !
                //      - 3 عوامل تحويل: نص، منطقي، رقم
                // (EN) Convert operator symbol to safe function name
                //      Includes all 21 operators supported in the interpreter:
                //      - 13 binary operators (arithmetic + comparison)
                //      - 3 special operators: index [], index assign []=, call ()
                //      - 3 unary operators: negate u-, positive u+, not !
                //      - 3 conversion operators: نص (toString), منطقي (toBool), رقم (toNumber)
                std::string opSafeName;
                // (AR) العوامل الحسابية الثنائية
                // (EN) Binary arithmetic operators
                if (operatorDecl->operatorSymbol == "+")
                    opSafeName = "__op_add__";
                else if (operatorDecl->operatorSymbol == "-")
                    opSafeName = "__op_sub__";
                else if (operatorDecl->operatorSymbol == "*")
                    opSafeName = "__op_mul__";
                else if (operatorDecl->operatorSymbol == "/")
                    opSafeName = "__op_div__";
                else if (operatorDecl->operatorSymbol == "//")
                    opSafeName = "__op_floordiv__";
                else if (operatorDecl->operatorSymbol == "%")
                    opSafeName = "__op_mod__";
                else if (operatorDecl->operatorSymbol == "**")
                    opSafeName = "__op_pow__";
                // (AR) عوامل المقارنة
                // (EN) Comparison operators
                else if (operatorDecl->operatorSymbol == "==")
                    opSafeName = "__op_eq__";
                else if (operatorDecl->operatorSymbol == "!=")
                    opSafeName = "__op_ne__";
                else if (operatorDecl->operatorSymbol == "<")
                    opSafeName = "__op_lt__";
                else if (operatorDecl->operatorSymbol == "<=")
                    opSafeName = "__op_le__";
                else if (operatorDecl->operatorSymbol == ">")
                    opSafeName = "__op_gt__";
                else if (operatorDecl->operatorSymbol == ">=")
                    opSafeName = "__op_ge__";
                // (AR) العوامل البتية والإزاحة
                // (EN) Bitwise and shift operators
                else if (operatorDecl->operatorSymbol == "^")
                    opSafeName = "__op_xor__";
                else if (operatorDecl->operatorSymbol == "&")
                    opSafeName = "__op_band__";
                else if (operatorDecl->operatorSymbol == "|")
                    opSafeName = "__op_bor__";
                else if (operatorDecl->operatorSymbol == "<<")
                    opSafeName = "__op_shl__";
                else if (operatorDecl->operatorSymbol == ">>")
                    opSafeName = "__op_shr__";
                // (AR) العوامل الخاصة: فهرسة، إسناد بالفهرس، استدعاء
                // (EN) Special operators: index, index-assign, call
                else if (operatorDecl->operatorSymbol == "[]")
                    opSafeName = "__op_index__";
                else if (operatorDecl->operatorSymbol == "[]=")
                    opSafeName = "__op_index_set__";
                else if (operatorDecl->operatorSymbol == "()")
                    opSafeName = "__op_call__";
                // (AR) العوامل الأحادية (parser يحول - بدون معاملات إلى u-)
                // (EN) Unary operators (parser converts - with 0 params to u-)
                else if (operatorDecl->operatorSymbol == "u-")
                    opSafeName = "__op_neg__";
                else if (operatorDecl->operatorSymbol == "u+")
                    opSafeName = "__op_pos__";
                else if (operatorDecl->operatorSymbol == "!")
                    opSafeName = "__op_not__";
                // (AR) عوامل التحويل: نص() → __op_tostring__, منطقي() → __op_tobool__, رقم() → __op_tonumber__
                //      هذه العوامل تُستدعى ضمنياً عند طباعة الكائن أو استخدامه في شرط أو تحويل رقمي
                // (EN) Conversion operators: نص() → __op_tostring__, منطقي() → __op_tobool__, رقم() → __op_tonumber__
                //      These are called implicitly when printing, using in conditions, or numeric conversion
                else if (operatorDecl->operatorSymbol == "\xD9\x86\xD8\xB5")
                    opSafeName = "__op_tostring__"; // نص
                else if (operatorDecl->operatorSymbol == "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A")
                    opSafeName = "__op_tobool__"; // منطقي
                else if (operatorDecl->operatorSymbol == "\xD8\xB1\xD9\x82\xD9\x85")
                    opSafeName = "__op_tonumber__"; // رقم
                // (AR) عامل غير معروف — استخدم الاسم كما هو مع بادئة __op_
                // (EN) Unknown operator — use name as-is with __op_ prefix
                else
                    opSafeName = "__op_" + operatorDecl->operatorSymbol + "__";

                // (AR) استنتاج نوع الإرجاع إذا لم يُحدد
                // (EN) Infer return type if not specified
                SadTypeKind returnType;
                if (operatorDecl->returnType == Data::DataType::UNKNOWN ||
                    operatorDecl->returnType == Data::DataType::NONE)
                {
                    auto savedClassName = b_.currentClassName_;
                    b_.currentClassName_ = classDecl->name;
                    returnType = b_.inferReturnTypeFromBody(operatorDecl->body.get());
                    b_.currentClassName_ = savedClassName;
                }
                else
                {
                    returnType = b_.astTypeToSIRType(operatorDecl->returnType);
                }
                std::string fullOpName = classDecl->name + "." + opSafeName;
                auto sirOpFunc = std::make_shared<SIRFunction>(fullOpName, returnType);

                // (AR) المعامل الأول: self (الكائن الحالي)
                // (EN) First param: self (current object)
                sirOpFunc->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                // (AR) المعامل الثاني: المعامل الآخر (other)
                // (EN) Second param: other operand
                for (const auto &param : operatorDecl->parameters)
                {
                    SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                    // (AR) معاملات العوامل بدون نوع صريح → I64 افتراضياً (الكائنات تُمرر كـ i64 pointer)
                    // (EN) Operator params without explicit type → default to I64 (objects passed as i64 pointer)
                    if (paramType == SadTypeKind::Void)
                    {
                        paramType = SadTypeKind::Integer;
                    }
                    sirOpFunc->addParameter(SIRParameter(param.name, paramType));
                }

                sirClass->addMethod(sirOpFunc);

                // (AR) بناء جسم العامل
                // (EN) Build operator body
                if (operatorDecl->body)
                {
                    auto prevFunction = b_.currentFunction_;
                    auto prevBlock = b_.currentBlock_;
                    auto prevClassName = b_.currentClassName_;

                    b_.currentFunction_ = sirOpFunc;
                    b_.currentClassName_ = classDecl->name;

                    b_.enterScope();

                    {
                        VariableInfo selfInfo;
                        selfInfo.name = kSelfParamName;
                        selfInfo.type = SadTypeKind::Integer;
                        selfInfo.registerName = kSelfRegisterName;
                        selfInfo.isGlobal = false;
                        selfInfo.isMutable = false;
                        selfInfo.scopeLevel = static_cast<int>(b_.scopeStack_.size());
                        b_.addVariable(selfInfo);
                        // (AR) self هو كائن من نوع الصنف الحالي
                        // (EN) self is an object of the current class type
                        b_.classInstanceTypes_["self"] = classDecl->name;
                        b_.classInstanceTypes_[kSelfRegisterName] = classDecl->name;
                    }

                    // (AR) تسجيل "هذا" كمرادف لـ self
                    // (EN) Register "هذا" (this) as alias for self
                    {
                        VariableInfo thisInfo;
                        thisInfo.name = kThisAliasName; // هذا
                        thisInfo.type = SadTypeKind::Integer;
                        thisInfo.registerName = kSelfRegisterName;
                        thisInfo.isGlobal = false;
                        thisInfo.isMutable = false;
                        thisInfo.scopeLevel = static_cast<int>(b_.scopeStack_.size());
                        b_.addVariable(thisInfo);
                        // (AR) هذا أيضاً يشير للصنف الحالي
                        // (EN) this also points to the current class
                        b_.classInstanceTypes_[kThisAliasName] = classDecl->name;
                    }

                    for (const auto &param : operatorDecl->parameters)
                    {
                        VariableInfo paramInfo;
                        paramInfo.name = param.name;
                        paramInfo.type = b_.astTypeToSIRType(param.type);
                        // (AR) معاملات العوامل بدون نوع صريح → I64
                        // (EN) Operator params without explicit type → I64
                        if (paramInfo.type == SadTypeKind::Void)
                        {
                            paramInfo.type = SadTypeKind::Integer;
                        }
                        paramInfo.registerName = "%" + param.name;
                        paramInfo.isGlobal = false;
                        paramInfo.isMutable = true;
                        paramInfo.isParameter = true;
                        paramInfo.scopeLevel = static_cast<int>(b_.scopeStack_.size());
                        b_.addVariable(paramInfo);

                        // (AR) إذا كان المعامل بدون نوع صريح (UNKNOWN/NONE)، نفترض أنه كائن من نفس الصنف
                        //      هذا يسمح بالوصول لحقول المعامل (مثل آخر.س) داخل جسم العامل
                        // (EN) If param has no explicit type (UNKNOWN/NONE), assume it's an object of the same class
                        //      This allows member access (e.g. other.x) inside operator body
                        if (param.type == Data::DataType::UNKNOWN || param.type == Data::DataType::NONE ||
                            param.type == Data::DataType::OBJECT)
                        {
                            b_.classInstanceTypes_[param.name] = classDecl->name;
                            b_.classInstanceTypes_["%" + param.name] = classDecl->name;
                        }
                        else if (!param.typeName.empty())
                        {
                            // (AR) إذا حُدد اسم النوع صراحة (مثل متجه آخر)
                            // (EN) If type name was explicitly specified (e.g. متجه other)
                            b_.classInstanceTypes_[param.name] = param.typeName;
                            b_.classInstanceTypes_["%" + param.name] = param.typeName;
                        }
                    }

                    for (const auto &field : sirClass->fields_)
                    {
                        VariableInfo fieldInfo;
                        fieldInfo.name = field.first;
                        fieldInfo.type = field.second;
                        fieldInfo.registerName = "%" + field.first;
                        fieldInfo.isGlobal = false;
                        fieldInfo.isMutable = true;
                        fieldInfo.scopeLevel = static_cast<int>(b_.scopeStack_.size());
                        b_.addVariable(fieldInfo);
                    }

                    auto entryBlock = b_.createBasicBlock(kEntryBlockName);
                    sirOpFunc->addBasicBlock(entryBlock);
                    b_.currentBlock_ = entryBlock;

                    for (const auto &field : sirClass->fields_)
                    {
                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::ALLOC;
                        allocInst.result = SIROperand::Register("%" + field.first, field.second);
                        b_.currentBlock_->addInstruction(allocInst);
                    }

                    b_.buildStatement(operatorDecl->body.get());

                    if (b_.currentBlock_)
                    {
                        bool hasTerminator = false;
                        if (!b_.currentBlock_->instructions.empty())
                        {
                            auto lastOp = b_.currentBlock_->instructions.back().opcode;
                            hasTerminator = (lastOp == SIROpcode::RET || lastOp == SIROpcode::RET_VOID);
                        }
                        if (!hasTerminator)
                        {
                            SIRInstruction retInst;
                            if (returnType == SadTypeKind::Void)
                            {
                                retInst.opcode = SIROpcode::RET_VOID;
                            }
                            else
                            {
                                retInst.opcode = SIROpcode::RET;
                                if (returnType == SadTypeKind::String)
                                {
                                    retInst.operands.push_back(SIROperand::ConstantString(""));
                                }
                                else
                                {
                                    retInst.operands.push_back(SIROperand::ConstantI64(0));
                                }
                            }
                            b_.currentBlock_->addInstruction(retInst);
                        }
                    }

                    b_.exitScope();

                    b_.module_->addFunction(sirOpFunc);

                    FunctionInfo opInfo;
                    opInfo.name = fullOpName;
                    opInfo.returnType = returnType;
                    opInfo.parameters = sirOpFunc->getParameters();
                    opInfo.sirFunction = sirOpFunc;
                    b_.functionTable_[fullOpName] = opInfo;

                    b_.currentFunction_ = prevFunction;
                    b_.currentBlock_ = prevBlock;
                    b_.currentClassName_ = prevClassName;
                }
            }
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad