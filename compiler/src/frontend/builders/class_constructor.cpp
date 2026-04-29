// ============================================================================
// sir_builder_classes_ctor.cpp
// ============================================================================
// (AR) معالجة بواني الأصناف — مستخرج من sir_builder_classes.cpp (CW-05)
//      تحليل الباني، استنتاج أنواع الحقول، وراثة super args، بناء الجسم
// (EN) Class constructor processing — extracted from sir_builder_classes.cpp
//      Constructor analysis, field type inference, super args, body building
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
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            void ClassBuilder::buildClassConstructor(
                AST::ClassDeclNode *classDecl,
                std::shared_ptr<SIRClass> sirClass,
                Sad::AST::ConstructorDecl *ctorDecl)
            {
                // (AR) حساب الصنف الأب — مطلوب لمعالجة super() في الباني
                // (EN) Compute parent class — needed for super() handling in constructor
                std::string parentClass = "";
                if (!classDecl->superclasses.empty())
                {
                    parentClass = classDecl->superclasses[0];
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildClass: found constructor" << std::endl;
#endif

                std::string fullCtorName = classDecl->name + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // بناء
                auto sirCtor = std::make_shared<SIRFunction>(fullCtorName, SadTypeKind::Void);
                sirCtor->addParameter(SIRParameter(kSelfParamName, SadTypeKind::Integer));

                for (const auto &param : ctorDecl->parameters)
                {
                    SadTypeKind paramType = b_.astTypeToSIRType(param.type);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildClass: constructor param '" << param.name
                              << "' AST type=" << static_cast<int>(param.type)
                              << " -> SIR type=" << static_cast<int>(paramType) << std::endl;
#endif
                    sirCtor->addParameter(SIRParameter(param.name, paramType));
                }

                sirClass->addMethod(sirCtor);

                // ═══════════════════════════════════════════════════════════════
                // (AR) استنتاج أنواع الحقول: مسح جسم الباني لبناء خريطة معامل→حقل
                // (EN) Field type inference: scan constructor body for param→field mapping
                // ═══════════════════════════════════════════════════════════════
                if (ctorDecl->body)
                {
                    auto *blockStmt = dynamic_cast<AST::BlockStmt *>(ctorDecl->body.get());
                    if (blockStmt)
                    {
                        for (const auto &stmt : blockStmt->statements)
                        {
                            auto *exprStmt = dynamic_cast<AST::ExprStmt *>(stmt.get());
                            if (exprStmt && exprStmt->expression)
                            {
                                auto *memberAssign = dynamic_cast<AST::MemberAssignExpr *>(exprStmt->expression.get());
                                if (memberAssign && dynamic_cast<AST::ThisExpr *>(memberAssign->object.get()))
                                {
                                    // (AR) هذا.حقل = تعبير — تتبع العلاقة وإضافة الحقل ضمنياً
                                    // (EN) this.field = expr — track relationship AND add implicit field
                                    const std::string &fieldName = memberAssign->member;

                                    auto *varExpr = dynamic_cast<AST::VariableExpr *>(memberAssign->value.get());
                                    if (varExpr)
                                    {
                                        // (AR) هذا.حقل = معامل → تسجيل في الخريطة
                                        // (EN) this.field = param → record mapping
                                        sirClass->paramToFieldMap_[varExpr->name] = fieldName;

                                        // (AR) إضافة الحقل ضمنياً إذا لم يكن موجوداً (حقول من الباني)
                                        // (EN) Add implicit field if not present (constructor-defined fields)
                                        if (sirClass->fields_.find(fieldName) == sirClass->fields_.end())
                                        {
                                            SadTypeKind fieldType = SadTypeKind::Pointer;
                                            for (const auto &param : ctorDecl->parameters)
                                            {
                                                if (param.name == varExpr->name)
                                                {
                                                    if (param.type != Data::DataType::UNKNOWN &&
                                                        param.type != Data::DataType::OBJECT)
                                                    {
                                                        fieldType = b_.astTypeToSIRType(param.type);
                                                    }
                                                    break;
                                                }
                                            }
                                            sirClass->addField(fieldName, fieldType);
                                        }
                                    }
                                    // (AR) هذا.حقل = قيمة_حرفية → استنتاج النوع وإضافة الحقل
                                    // (EN) this.field = literal → infer type and add field
                                    auto *literal = dynamic_cast<AST::LiteralExpr *>(memberAssign->value.get());
                                    if (literal)
                                    {
                                        SadTypeKind litType = SadTypeKind::Pointer;
                                        auto tokenType = literal->token.getType();
                                        if (tokenType == Sad::Lexer::TokenType::NUMBER_INTEGER)
                                        {
                                            litType = SadTypeKind::Integer;
                                        }
                                        else if (tokenType == Sad::Lexer::TokenType::NUMBER_DOUBLE)
                                        {
                                            litType = SadTypeKind::Float;
                                        }
                                        else if (tokenType == Sad::Lexer::TokenType::STRING_LITERAL)
                                        {
                                            litType = SadTypeKind::String;
                                        }
                                        else if (tokenType == Sad::Lexer::TokenType::LITERAL_TRUE ||
                                                 tokenType == Sad::Lexer::TokenType::LITERAL_FALSE)
                                        {
                                            litType = SadTypeKind::Boolean;
                                        }
                                        if (sirClass->fields_.find(fieldName) == sirClass->fields_.end())
                                        {
                                            sirClass->addField(fieldName, litType);
                                        }
                                        else
                                        {
                                            sirClass->fields_[fieldName] = litType;
                                        }
                                    }
                                    // (AR) هذا.حقل = تعبير_آخر (استدعاء دالة، عملية، إلخ)
                                    // (EN) this.field = other expr (call, operation, etc.)
                                    if (!varExpr && !literal)
                                    {
                                        SadTypeKind exprType = SadTypeKind::Pointer;
                                        // (AR) هذا.حقل = [عناصر] → مصفوفة
                                        // (EN) this.field = [elements] → array
                                        if (dynamic_cast<Sad::AST::ArrayExpr *>(memberAssign->value.get()))
                                        {
                                            exprType = SadTypeKind::Array;
                                        }
                                        // (AR) هذا.حقل = تعبير_ثنائي (عملية حسابية/منطقية) → استنتاج النوع
                                        //      مثال: هذا.المساحة = العرض * الطول → I64
                                        // (EN) this.field = binary expr (arithmetic/logic) → infer type
                                        //      e.g.: this.area = width * height → I64
                                        else if (auto *binExpr = dynamic_cast<Sad::AST::BinaryExpr *>(memberAssign->value.get()))
                                        {
                                            auto dataType = binExpr->getDataType();
                                            if (dataType != Data::DataType::UNKNOWN &&
                                                dataType != Data::DataType::OBJECT)
                                            {
                                                exprType = b_.astTypeToSIRType(dataType);
                                            }
                                        }
                                        // (AR) هذا.حقل = تعبير_أحادي → استنتاج النوع
                                        // (EN) this.field = unary expr → infer type
                                        else if (auto *unaryExpr = dynamic_cast<Sad::AST::UnaryExpr *>(memberAssign->value.get()))
                                        {
                                            auto dataType = unaryExpr->getDataType();
                                            if (dataType != Data::DataType::UNKNOWN &&
                                                dataType != Data::DataType::OBJECT)
                                            {
                                                exprType = b_.astTypeToSIRType(dataType);
                                            }
                                        }
                                        if (sirClass->fields_.find(fieldName) == sirClass->fields_.end())
                                        {
                                            sirClass->addField(fieldName, exprType);
                                        }
                                        else
                                        {
                                            // (AR) تحديث نوع الحقل إذا كان موجوداً
                                            sirClass->fields_[fieldName] = exprType;
                                        }
                                        // (AR) تسجيل حقل المصفوفة لتهيئة SadArray في emitAlloca
                                        // (EN) Register array field for SadArray initialization in emitAlloca
                                        if (exprType == SadTypeKind::Array)
                                        {
                                            sirClass->markFieldAsArray(fieldName);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                // (AR) بناء جسم الباني
                // (EN) Build constructor body
                if (ctorDecl->body)
                {
                    auto prevFunction = b_.currentFunction_;
                    auto prevBlock = b_.currentBlock_;
                    auto prevClassName = b_.currentClassName_;

                    b_.currentFunction_ = sirCtor;
                    b_.currentClassName_ = classDecl->name;

                    b_.enterScope();

                    // (AR) تسجيل معامل self
                    // (EN) Register self parameter
                    {
                        VariableInfo selfInfo;
                        selfInfo.name = kSelfParamName;
                        selfInfo.type = SadTypeKind::Integer;
                        selfInfo.registerName = kSelfRegisterName;
                        selfInfo.isGlobal = false;
                        selfInfo.isMutable = false;
                        selfInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_constructor_size");
                        b_.addVariable(selfInfo);
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
                        thisInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_constructor_size");
                        b_.addVariable(thisInfo);
                    }

                    // (AR) تسجيل المعاملات + حقول الصنف كمتغيرات محلية
                    // (EN) Register parameters + class fields as local variables

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) إصلاح حرج: بناء مجموعة أسماء المعاملات لمنع تضارب الأسماء
                    //      عندما يكون اسم الحقل مطابقاً لاسم المعامل (مثل هذا.الاسم = الاسم)
                    //      يجب أن يأخذ المعامل الأولوية حتى يقرأ b_.buildExpression من المعامل
                    //      وليس من الحقل (غير المُهيّأ) عند تعيين هذا.حقل = معامل
                    // (EN) Critical fix: build parameter name set to prevent name collision
                    //      When field name matches param name (e.g., this.name = name)
                    //      parameter must take priority so b_.buildExpression reads from param
                    //      not from the (uninitialized) field when assigning this.field = param
                    // ═══════════════════════════════════════════════════════════════
                    std::unordered_set<std::string> ctorParamNames;
                    for (const auto &param : ctorDecl->parameters)
                    {
                        ctorParamNames.insert(param.name);
                        VariableInfo paramInfo;
                        paramInfo.name = param.name;
                        paramInfo.type = b_.astTypeToSIRType(param.type);
                        paramInfo.registerName = "%" + param.name;
                        paramInfo.isGlobal = false;
                        paramInfo.isMutable = false;
                        paramInfo.isParameter = true;
                        paramInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_constructor_size");
                        b_.addVariable(paramInfo);
                    }

                    // (AR) تسجيل حقول الصنف كمتغيرات محلية للوصول المباشر
                    //      تخطي الحقول التي تتطابق أسماؤها مع معاملات الباني
                    //      هذه الحقول يُوصَل إليها عبر هذا.حقل (member access) وليس مباشرة
                    // (EN) Register class fields as local variables for direct access
                    //      Skip fields whose names match constructor parameters
                    //      These fields are accessed via this.field (member access) not directly
                    for (const auto &field : sirClass->fields_)
                    {
                        if (ctorParamNames.count(field.first) > 0)
                        {
                            continue; // (AR) تخطي — المعامل يأخذ الأولوية
                        }
                        VariableInfo fieldInfo;
                        fieldInfo.name = field.first;
                        fieldInfo.type = field.second;
                        fieldInfo.registerName = "%" + field.first;
                        fieldInfo.isGlobal = false;
                        fieldInfo.isMutable = true;
                        fieldInfo.scopeLevel = Sad::Security::SafeArithmetic::assertSafeCast<int>(b_.scopeStack_.size(), "class_constructor_size");
                        b_.addVariable(fieldInfo);
                    }

                    auto entryBlock = b_.createBasicBlock(kEntryBlockName);
                    sirCtor->addBasicBlock(entryBlock);
                    b_.currentBlock_ = entryBlock;

                    // (AR) Alloca لكل حقل — تخطي الحقول المتطابقة مع المعاملات
                    //      الحقول المتطابقة تُعيَّن عبر STORE member (هذا.حقل = معامل)
                    //      الذي يُنشئ GEP خاصاً به، فلا حاجة لـ ALLOC منفصل
                    // (EN) Alloca for each field — skip fields matching parameter names
                    //      Matching fields are set via member STORE (this.field = param)
                    //      which creates its own GEP, so no separate ALLOC is needed
                    for (const auto &field : sirClass->fields_)
                    {
                        if (ctorParamNames.count(field.first) > 0)
                        {
                            continue; // (AR) تخطي — يُعيَّن عبر هذا.حقل = معامل
                        }
                        SIRInstruction allocInst;
                        allocInst.opcode = SIROpcode::ALLOC;
                        allocInst.result = SIROperand::Register("%" + field.first, field.second);
                        b_.currentBlock_->addInstruction(allocInst);
                    }

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) استدعاء باني الأب إذا كانت هناك superArgs
                    // (EN) Call parent constructor if superArgs exist
                    // ═══════════════════════════════════════════════════════════════
                    if (!ctorDecl->superArgs.empty() && !parentClass.empty())
                    {
                        // (AR) بناء معاملات استدعاء باني الأب
                        // (EN) Build parent constructor call arguments
                        std::string parentCtorName = parentClass + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // بناء

                        std::vector<SIROperand> superArgOperands;
                        superArgOperands.push_back(SIROperand::Register("%self", SadTypeKind::Integer)); // self

                        // ═══════════════════════════════════════════════════════════════
                        // (AR) بناء خريطة وسائط الأساس → معاملات الباني
                        // (EN) Build super arg mapping: classify each as param ref or constant
                        // ═══════════════════════════════════════════════════════════════
                        for (int superIdx = 0; superIdx < Sad::Security::SafeArithmetic::assertSafeCast<int>(ctorDecl->superArgs.size(), "class_constructor_size"); superIdx++)
                        {
                            auto &arg = ctorDecl->superArgs[superIdx];

                            // (AR) تصنيف وسيط الأساس: متغير (مرجع لمعامل) أو ثابت
                            // (EN) Classify super arg: variable (param ref) or constant
                            auto *varExpr = dynamic_cast<AST::VariableExpr *>(arg.get());
                            if (varExpr)
                            {
                                sirClass->superParamMapping_[superIdx] = varExpr->name;
                            }
                            auto *literal = dynamic_cast<AST::LiteralExpr *>(arg.get());
                            if (literal)
                            {
                                auto tokenType = literal->token.getType();
                                if (tokenType == Sad::Lexer::TokenType::NUMBER_INTEGER)
                                {
                                    sirClass->superConstantMapping_[superIdx] = {SadTypeKind::Integer, literal->token.getValue()};
                                }
                                else if (tokenType == Sad::Lexer::TokenType::NUMBER_DOUBLE)
                                {
                                    sirClass->superConstantMapping_[superIdx] = {SadTypeKind::Float, literal->token.getValue()};
                                }
                                else if (tokenType == Sad::Lexer::TokenType::STRING_LITERAL)
                                {
                                    sirClass->superConstantMapping_[superIdx] = {SadTypeKind::String, literal->token.getValue()};
                                }
                            }

                            BuildResult argResult = b_.buildExpression(arg.get());
                            // (AR) تحويل النتيجة الثابتة إلى SIROperand::Constant
                            // (EN) Convert constant result to SIROperand::Constant
                            if (argResult.isConstant)
                            {
                                if (argResult.type == SadTypeKind::Integer)
                                {
                                    int64_t val = 0;
                                    try
                                    {
                                        val = std::stoll(argResult.constantValue);
                                    }
                                    catch (const std::exception &)
                                    {
#ifndef NDEBUG
                                        std::cerr << "[SIR] تحذير: فشل تحويل '" << argResult.constantValue << "' إلى I64\n";
#endif
                                    }
                                    superArgOperands.push_back(SIROperand::ConstantI64(val));
                                }
                                else if (argResult.type == SadTypeKind::Float)
                                {
                                    double val = 0.0;
                                    try
                                    {
                                        val = std::stod(argResult.constantValue);
                                    }
                                    catch (const std::exception &)
                                    {
#ifndef NDEBUG
                                        std::cerr << "[SIR] تحذير: فشل تحويل '" << argResult.constantValue << "' إلى F64\n";
#endif
                                    }
                                    superArgOperands.push_back(SIROperand::ConstantF64(val));
                                }
                                else if (argResult.type == SadTypeKind::String)
                                {
                                    superArgOperands.push_back(SIROperand::ConstantString(argResult.constantValue));
                                }
                                else if (argResult.type == SadTypeKind::Boolean)
                                {
                                    superArgOperands.push_back(SIROperand::ConstantBool(argResult.constantValue == "true"));
                                }
                                else
                                {
                                    superArgOperands.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                                }
                            }
                            else
                            {
                                superArgOperands.push_back(SIROperand::Register(argResult.registerName, argResult.type));
                            }
                        }

                        // ═══════════════════════════════════════════════════════════════
                        // (AR) نشر ثوابت وسائط الأساس إلى حقول الأب
                        // (EN) Propagate constant super args to parent field types
                        // ═══════════════════════════════════════════════════════════════
                        if (!sirClass->superConstantMapping_.empty())
                        {
                            auto parentSirClass = b_.module_->getClass(parentClass);
                            if (parentSirClass)
                            {
                                // (AR) الحصول على معاملات باني الأب
                                // (EN) Get parent constructor params
                                std::string parentCtorMethodName = parentClass + ".\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1";
                                auto parentCtor = parentSirClass->getMethod(parentCtorMethodName);
                                if (parentCtor)
                                {
                                    const auto &parentParams = parentCtor->getParameters();
                                    for (auto &[superIdx, typeAndVal] : sirClass->superConstantMapping_)
                                    {
                                        int parentParamIdx = superIdx + 1; // +1 (skip self)
                                        if (parentParamIdx < Sad::Security::SafeArithmetic::assertSafeCast<int>(parentParams.size(), "class_constructor_size"))
                                        {
                                            const std::string &parentParamName = parentParams[parentParamIdx].name;
                                            auto pFieldIt = parentSirClass->paramToFieldMap_.find(parentParamName);
                                            if (pFieldIt != parentSirClass->paramToFieldMap_.end())
                                            {
                                                const std::string &parentFieldName = pFieldIt->second;
                                                auto currentFieldType = parentSirClass->fields_.find(parentFieldName);
                                                if (currentFieldType != parentSirClass->fields_.end() &&
                                                    currentFieldType->second == SadTypeKind::Pointer)
                                                {
                                                    SadTypeKind newType = typeAndVal.first;
                                                    if (newType == SadTypeKind::String)
                                                        newType = SadTypeKind::Pointer; // string is ptr
                                                    parentSirClass->fields_[parentFieldName] = newType;
                                                    // (AR) تحديث أيضاً في الصنف الابن (الحقول الموروثة)
                                                    // (EN) Also update in child class (inherited fields)
                                                    if (sirClass->fields_.count(parentFieldName))
                                                    {
                                                        sirClass->fields_[parentFieldName] = newType;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // (AR) إصدار تعليمة CALL لباني الأب
                        // (EN) Emit CALL instruction for parent constructor
                        SIRInstruction callInst;
                        callInst.opcode = SIROpcode::CALL;
                        callInst.result = SIROperand::Register(b_.newTempRegister(), SadTypeKind::Void);
                        callInst.operands.push_back(SIROperand::Register(parentCtorName, SadTypeKind::Void));
                        for (auto &op : superArgOperands)
                        {
                            callInst.operands.push_back(op);
                        }
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(callInst);
                    }

                    b_.buildStatement(ctorDecl->body.get());

                    // (AR) إضافة RET_VOID
                    // (EN) Add RET_VOID
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
                            retInst.opcode = SIROpcode::RET_VOID;
                            b_.currentBlock_->addInstruction(retInst);
                        }
                    }

                    b_.exitScope();

                    b_.module_->addFunction(sirCtor);

                    // ═══════════════════════════════════════════════════════════
                    // (AR) تسجيل في جدول الدوال مع الحفاظ على أنواع Phase 1.7
                    //      Phase 1.35 يسجّل الباني مبكراً ثم Phase 1.7 يُحدّث
                    //      أنواع المعاملات من call-sites. إذا كتبنا فوقه نفقد
                    //      التحديثات. الحل: دمج الأنواع المُستنتجة من Phase 1.7
                    // (EN) Register in function table while preserving Phase 1.7 types
                    //      Phase 1.35 pre-registers ctor, Phase 1.7 updates param
                    //      types from call-sites. Overwriting loses those updates.
                    //      Solution: merge inferred types from Phase 1.7
                    // ═══════════════════════════════════════════════════════════
                    FunctionInfo ctorInfo;
                    ctorInfo.name = fullCtorName;
                    ctorInfo.returnType = SadTypeKind::Void;
                    ctorInfo.parameters = sirCtor->getParameters();
                    ctorInfo.sirFunction = sirCtor;

                    // (AR) دمج أنواع المعاملات من Phase 1.7 إذا كانت أدق
                    // (EN) Merge param types from Phase 1.7 if they're more specific
                    auto prevIt = b_.functionTable_.find(fullCtorName);
                    if (prevIt != b_.functionTable_.end())
                    {
                        const auto &prevParams = prevIt->second.parameters;
                        for (size_t pi = 0; pi < ctorInfo.parameters.size() && pi < prevParams.size(); pi++)
                        {
                            // (AR) إذا كان Phase 1.7 قد حدّث النوع من Integer إلى نوع أدق
                            // (EN) If Phase 1.7 updated the type from Integer to a more specific type
                            if (ctorInfo.parameters[pi].type == SadTypeKind::Integer &&
                                prevParams[pi].type != SadTypeKind::Integer &&
                                prevParams[pi].type != SadTypeKind::Void &&
                                prevParams[pi].type != SadTypeKind::Pointer)
                            {
                                ctorInfo.parameters[pi].type = prevParams[pi].type;
                                // (AR) أيضاً نحدّث المعاملات في دالة SIR نفسها
                                // (EN) Also update the params in the SIR function itself
                                if (pi < sirCtor->getParameters().size())
                                {
                                    sirCtor->getMutableParameters()[pi].type = prevParams[pi].type;
                                }
                            }
                        }
                    }

                    b_.functionTable_[fullCtorName] = ctorInfo;

                    b_.currentFunction_ = prevFunction;
                    b_.currentBlock_ = prevBlock;
                    b_.currentClassName_ = prevClassName;
                }
            }
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad