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

#include <string>
#include "sir_builder.h"
#include "builders/expression_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // (AR) تم نقل buildLiteral, buildVariableAccess, buildShortCircuitLogical, buildUnaryOp
            //      إلى sir_builder_expressions.cpp (CW-05)
            // (EN) buildLiteral, buildVariableAccess, buildShortCircuitLogical, buildUnaryOp
            //      moved to sir_builder_expressions.cpp (CW-05)
            // ============================================================================
            BuildResult ExpressionBuilder::buildBinaryOp(AST::BinaryOpNode *binOp)
            {
                if (!binOp)
                {
                    return BuildResult();
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: بدء بناء عملية ثنائية" << std::endl;
#endif

                // ================================================================
                // (AR) التقييم الكسول (Short-circuit) للعوامل المنطقية و/أو (&&/||)
                //      لا يتم تقييم الطرف الأيمن إذا كان الطرف الأيسر كافياً:
                //      - «و» (&&): إذا كان الأيسر false → النتيجة false
                //      - «أو» (||): إذا كان الأيسر true → النتيجة true
                //      هذا ضروري لمنع الآثار الجانبية غير المرغوبة.
                //      يُبنى عبر كتل SIR (basic blocks) مع تفرع مشروط.
                // (EN) Short-circuit evaluation for logical AND/OR (&&/||)
                //      Right operand is not evaluated if left operand is sufficient:
                //      - AND (&&): if left is false → result is false
                //      - OR (||): if left is true → result is true
                //      Built using SIR basic blocks with conditional branching.
                // ================================================================
                if (binOp->op == Lexer::TokenType::OP_AND ||
                    binOp->op == Lexer::TokenType::OP_OR)
                {
                    return buildShortCircuitLogical(binOp);
                }

                // (AR) بناء المعامل الأيسر (expressions.h:42 - left: ExprPtr)
                // (EN) Build left operand
                auto leftResult = buildExpression(binOp->left.get());
#ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: leftResult.registerName='" << leftResult.registerName
                          << "', type=" << static_cast<int>(leftResult.type)
                          << ", isConstant=" << leftResult.isConstant << std::endl;
#endif

                // (AR) بناء المعامل الأيمن (expressions.h:44 - right: ExprPtr)
                // (EN) Build right operand
                auto rightResult = buildExpression(binOp->right.get());
#ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: rightResult.registerName='" << rightResult.registerName
                          << "', type=" << static_cast<int>(rightResult.type)
                          << ", isConstant=" << rightResult.isConstant << std::endl;
#endif

                // ================================================================
                // (AR) تحميل المعاملات الزائد: إذا كان المعامل الأيسر كائن، استدعاء دالة العامل
                // (EN) Operator overloading: if left operand is an object, call operator function
                // ================================================================
                std::string leftClassName = leftResult.className;
                if (leftClassName.empty() && !leftResult.registerName.empty())
                {
                    auto it = b_.classInstanceTypes_.find(leftResult.registerName);
                    if (it != b_.classInstanceTypes_.end())
                    {
                        leftClassName = it->second;
                    }
                }

                if (!leftClassName.empty())
                {
                    // (AR) تحويل رمز العامل إلى اسم دالة
                    // (EN) Convert operator symbol to function name
                    std::string opSafeName;
                    switch (binOp->op)
                    {
                    case Lexer::TokenType::OP_PLUS:
                        opSafeName = "__op_add__";
                        break;
                    case Lexer::TokenType::OP_MINUS:
                        opSafeName = "__op_sub__";
                        break;
                    case Lexer::TokenType::OP_MULTIPLY:
                        opSafeName = "__op_mul__";
                        break;
                    case Lexer::TokenType::OP_DIVIDE:
                        opSafeName = "__op_div__";
                        break;
                    case Lexer::TokenType::OP_FLOOR_DIVIDE:
                        opSafeName = "__op_floordiv__";
                        break;
                    case Lexer::TokenType::OP_MODULO:
                        opSafeName = "__op_mod__";
                        break;
                    case Lexer::TokenType::OP_POWER:
                        opSafeName = "__op_pow__";
                        break;
                    case Lexer::TokenType::OP_EQUAL:
                        opSafeName = "__op_eq__";
                        break;
                    case Lexer::TokenType::OP_NOT_EQUAL:
                        opSafeName = "__op_ne__";
                        break;
                    case Lexer::TokenType::OP_LESS:
                        opSafeName = "__op_lt__";
                        break;
                    case Lexer::TokenType::OP_LESS_EQUAL:
                        opSafeName = "__op_le__";
                        break;
                    case Lexer::TokenType::OP_GREATER:
                        opSafeName = "__op_gt__";
                        break;
                    case Lexer::TokenType::OP_GREATER_EQUAL:
                        opSafeName = "__op_ge__";
                        break;
                    // (AR) العوامل البتية والإزاحة
                    // (EN) Bitwise and shift operators
                    case Lexer::TokenType::OP_XOR:
                        opSafeName = "__op_xor__";
                        break;
                    case Lexer::TokenType::OP_BITWISE_AND:
                        opSafeName = "__op_band__";
                        break;
                    case Lexer::TokenType::OP_BITWISE_OR:
                        opSafeName = "__op_bor__";
                        break;
                    case Lexer::TokenType::OP_SHIFT_LEFT:
                        opSafeName = "__op_shl__";
                        break;
                    case Lexer::TokenType::OP_SHIFT_RIGHT:
                        opSafeName = "__op_shr__";
                        break;
                    default:
                        // (AR) عامل غير مدعوم للتحميل الزائد — لن يُبحّث عن operator overload
                        // (EN) Unsupported operator for overloading — no overload lookup
                        break;
                    }

                    if (!opSafeName.empty())
                    {
                        // (AR) البحث عن دالة العامل في الصنف أو الأصناف الأب
                        // (EN) Look for operator function in the class or parent classes
                        std::string searchClass = leftClassName;
                        std::string fullOpName;
                        bool found = false;
                        while (!searchClass.empty())
                        {
                            fullOpName = searchClass + "." + opSafeName;
                            auto funcIt = b_.functionTable_.find(fullOpName);
                            if (funcIt != b_.functionTable_.end())
                            {
                                found = true;
                                break;
                            }
                            // (AR) البحث في الأب
                            // (EN) Search in parent
                            auto parentClass = b_.module_->getClass(searchClass);
                            if (parentClass && !parentClass->parentClass.empty())
                            {
                                searchClass = parentClass->parentClass;
                            }
                            else
                            {
                                break;
                            }
                        }

                        if (found)
                        {
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildBinaryOp: dispatching to operator overload '"
                                      << fullOpName << "'" << std::endl;
#endif

                            std::string resultReg = b_.newTempRegister();
                            auto &opInfo = b_.functionTable_[fullOpName];
                            SadTypeKind returnType = opInfo.returnType;

                            if (b_.currentBlock_)
                            {
                                SIRInstruction callInst;
                                callInst.opcode = SIROpcode::OBJECT_CALL;
                                callInst.result = SIROperand::Register(resultReg, returnType);
                                callInst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                                callInst.operands.push_back(SIROperand::ConstantString(opSafeName));
                                // (AR) إضافة المعامل الأيمن
                                // (EN) Add right operand
                                if (rightResult.isConstant && !rightResult.constantValue.empty())
                                {
                                    switch (rightResult.type)
                                    {
                                    case SadTypeKind::Integer:
                                        callInst.operands.push_back(SIROperand::ConstantI64(std::stoll(rightResult.constantValue)));
                                        break;
                                    case SadTypeKind::Float:
                                        callInst.operands.push_back(SIROperand::ConstantF64(std::stod(rightResult.constantValue)));
                                        break;
                                    case SadTypeKind::String:
                                        callInst.operands.push_back(SIROperand::ConstantString(rightResult.constantValue));
                                        break;
                                    default:
                                        callInst.operands.push_back(SIROperand::Register(rightResult.registerName, rightResult.type));
                                        break;
                                    }
                                }
                                else
                                {
                                    callInst.operands.push_back(SIROperand::Register(rightResult.registerName, rightResult.type));
                                }
                                b_.currentBlock_->addInstruction(callInst);
                            }

                            BuildResult result(resultReg, returnType);
                            // (AR) ننشر className فقط إذا كان نوع الإرجاع يمكن أن يمثل كائناً (I64/STRUCT)
                            //      النتائج BOOL/STRING/F64 ليست كائنات — ننع نشر className لتجنب
                            //      dispatch خاطئ لـ __op_tobool__/__op_tostring__ على قيم أولية
                            // (EN) Only propagate className if return type can represent an object (I64/STRUCT)
                            //      BOOL/STRING/F64 results are NOT objects — prevent className propagation
                            //      to avoid false __op_tobool__/__op_tostring__ dispatch on primitive values
                            if (returnType == SadTypeKind::Integer || returnType == SadTypeKind::Struct)
                            {
                                result.className = leftClassName;
                            }
                            return result;
                        }
                    }
                }

                // (AR) إنشاء سجل للنتيجة (sir_builder.h:511 - b_.newTempRegister)
                // (EN) Create result register
                std::string resultReg = b_.newTempRegister();

                // ================================================================
                // (AR) تحويل تلقائي: نص + كائن → نص + __op_tostring__(كائن)
                //      عند جمع نص مع كائن (+ فقط)، نستدعي __op_tostring__ تلقائياً
                //      يتوافق مع: المفسر يستدعي toString() ضمنياً عند الجمع مع نص
                // (EN) Auto-convert: string + object → string + __op_tostring__(object)
                //      When concatenating string with object (+ only), auto-call __op_tostring__
                //      Matches: interpreter implicitly calls toString() when concatenating with string
                // ================================================================
                if (binOp->op == Lexer::TokenType::OP_PLUS)
                {
                    // (AR) تحقق: يسار نص + يمين كائن
                    // (EN) Check: left is string + right is object
                    std::string rightClassName = rightResult.className;
                    if (rightClassName.empty() && !rightResult.registerName.empty())
                    {
                        auto it = b_.classInstanceTypes_.find(rightResult.registerName);
                        if (it != b_.classInstanceTypes_.end())
                            rightClassName = it->second;
                    }
                    if ((leftResult.type == SadTypeKind::String || (leftResult.isConstant && leftResult.type == SadTypeKind::String)) && !rightClassName.empty())
                    {
                        // (AR) بحث في سلسلة الوراثة عن __op_tostring__
                        // (EN) Search inheritance chain for __op_tostring__
                        std::string searchClass = rightClassName;
                        std::string tostrName;
                        bool foundToStr = false;
                        while (!searchClass.empty())
                        {
                            tostrName = searchClass + ".__op_tostring__";
                            if (b_.functionTable_.find(tostrName) != b_.functionTable_.end())
                            {
                                foundToStr = true;
                                break;
                            }
                            auto classInfo = b_.module_->getClass(searchClass);
                            if (classInfo && !classInfo->parentClass.empty())
                                searchClass = classInfo->parentClass;
                            else
                                break;
                        }
                        if (foundToStr)
                        {
                            std::string strReg = b_.newTempRegister();
                            SIRInstruction callInst;
                            callInst.opcode = SIROpcode::OBJECT_CALL;
                            callInst.result = SIROperand::Register(strReg, SadTypeKind::String);
                            callInst.operands.push_back(SIROperand::Register(rightResult.registerName, rightResult.type));
                            callInst.operands.push_back(SIROperand::ConstantString("__op_tostring__"));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(callInst);
                            // (AR) استبدال المعامل الأيمن بالنتيجة النصية
                            rightResult = BuildResult(strReg, SadTypeKind::String);
                        }
                    }
                    // (AR) تحقق: يسار كائن + يمين نص
                    // (EN) Check: left is object + right is string
                    if (!leftClassName.empty() &&
                        (rightResult.type == SadTypeKind::String || (rightResult.isConstant && rightResult.type == SadTypeKind::String)))
                    {
                        // (AR) بحث في سلسلة الوراثة عن __op_tostring__
                        // (EN) Search inheritance chain for __op_tostring__
                        std::string searchClassL = leftClassName;
                        std::string tostrName;
                        bool foundToStrL = false;
                        while (!searchClassL.empty())
                        {
                            tostrName = searchClassL + ".__op_tostring__";
                            if (b_.functionTable_.find(tostrName) != b_.functionTable_.end())
                            {
                                foundToStrL = true;
                                break;
                            }
                            auto classInfo = b_.module_->getClass(searchClassL);
                            if (classInfo && !classInfo->parentClass.empty())
                                searchClassL = classInfo->parentClass;
                            else
                                break;
                        }
                        if (foundToStrL)
                        {
                            std::string strReg = b_.newTempRegister();
                            SIRInstruction callInst;
                            callInst.opcode = SIROpcode::OBJECT_CALL;
                            callInst.result = SIROperand::Register(strReg, SadTypeKind::String);
                            callInst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                            callInst.operands.push_back(SIROperand::ConstantString("__op_tostring__"));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(callInst);
                            leftResult = BuildResult(strReg, SadTypeKind::String);
                        }
                    }
                }

                // (AR) تحديد نوع النتيجة - إذا كان أحد المعاملين عشري، النتيجة عشرية
                // (EN) Determine result type - if either operand is float, result is float
                // (AR) === إصلاح شامل لاكتشاف دمج النصوص ===
                // القاعدة الجديدة:
                //   1. أحد المعاملين نص حرفي (isConstant + STRING) → دمج نصوص دائماً
                //   2. أحد المعاملين متغير نصي حقيقي (STRING + !isParameter) + الآخر أي نوع → دمج نصوص
                //   3. كلا المعاملين STRING وليس أي منهما parameter مُستنتج → دمج نصوص
                //   4. أحد المعاملين parameter بنوع STRING مُستنتج + الآخر رقمي → حساب مؤشرات (ADD_I64)
                // هذا يحل:
                //   - `متغير_نص + 5` → STRING_CONCAT (كان ينتج ADD_I64 خطأ)
                //   - `نتيجة_دالة_نصية() + رقم` → STRING_CONCAT
                //   - `param(STRING مُستنتج) + رقم` → ADD_I64 (حساب مؤشرات صحيح)
                // (EN) === Comprehensive string concat detection fix ===
                // New rules:
                //   1. One operand is a string literal (isConstant + STRING) → always string concat
                //   2. One operand is a real string variable (STRING + !isParameter) + other any type → string concat
                //   3. Both STRING and neither is an inferred parameter → string concat
                //   4. One parameter with inferred STRING type + numeric → pointer arithmetic (ADD_I64)
                // This fixes:
                //   - `string_var + 5` → STRING_CONCAT (was producing ADD_I64 incorrectly)
                //   - `string_func_result() + number` → STRING_CONCAT
                //   - `param(inferred STRING) + number` → ADD_I64 (correct pointer arithmetic)
                SadTypeKind resultType = leftResult.type;
                bool isStringOp = false;

                // (AR) معامل نصي حرفي حقيقي = ثابت + نوع STRING
                // (EN) Real string literal = constant + STRING type
                bool leftIsStringLiteral = (leftResult.type == SadTypeKind::String && leftResult.isConstant);
                bool rightIsStringLiteral = (rightResult.type == SadTypeKind::String && rightResult.isConstant);

                // (AR) معامل STRING حقيقي غير مُستنتج: متغير نصي أو نتيجة دالة (ليس parameter)
                // (EN) Real non-inferred STRING operand: string variable or function result (not parameter)
                bool leftIsRealString = (leftResult.type == SadTypeKind::String && !leftResult.isParameter);
                bool rightIsRealString = (rightResult.type == SadTypeKind::String && !rightResult.isParameter);

                // (AR) === دعم المصفوفات: STRING + ARRAY → دمج نصوص (تحويل المصفوفة لنص) ===
                // (EN) === Array support: STRING + ARRAY → string concat (convert array to string) ===
                bool leftIsArray = (leftResult.type == SadTypeKind::Array);
                bool rightIsArray = (rightResult.type == SadTypeKind::Array);

                if (leftIsStringLiteral || rightIsStringLiteral)
                {
                    // (AR) أحد المعاملين نص حرفي فعلي — دمج نصوص مؤكد
                    // (EN) One operand is a real string literal — definitely string concat
                    isStringOp = true;
                }
                else if ((leftIsRealString && rightIsArray) || (rightIsRealString && leftIsArray) || (leftIsStringLiteral && rightIsArray) || (rightIsStringLiteral && leftIsArray))
                {
                    // (AR) نص + مصفوفة → دمج نصوص (المصفوفة ستُحوَّل لنص في LLVM codegen)
                    // (EN) string + array → string concat (array will be converted to string in LLVM codegen)
                    isStringOp = true;
                }
                else if (leftIsRealString || rightIsRealString)
                {
                    // (AR) أحد المعاملين متغير/نتيجة نصية حقيقية (ليس parameter مُستنتج) — دمج نصوص
                    //      هذا يشمل: متغيرات نصية + أرقام، نتائج دوال نصية + أي نوع
                    // (EN) One operand is a real string variable/result (not inferred parameter) — string concat
                    //      This covers: string variables + numbers, string function results + any type
                    isStringOp = true;
                }
                else if (leftResult.type == SadTypeKind::String && rightResult.type == SadTypeKind::String)
                {
                    // (AR) كلا المعاملين STRING وكلاهما parameter → حساب مؤشرات (لا دمج)
                    //      هذا يحدث فقط عندما parameter نوعه مُستنتج كـ STRING لكنه فعلياً pointer
                    // (EN) Both parameters with STRING type → pointer arithmetic (no concat)
                    //      This only happens when parameter type is inferred as STRING but is actually a pointer
                    isStringOp = false;
                }
                // (AR) الحالة المتبقية: أحد المعاملين parameter(STRING) والآخر رقمي → لا دمج نصوص (حساب مؤشرات)
                // (EN) Remaining case: one parameter(STRING) + numeric → no string concat (pointer arithmetic)

                if (isStringOp)
                {
                    resultType = SadTypeKind::String;
                }
                else if (leftResult.type == SadTypeKind::Float || rightResult.type == SadTypeKind::Float)
                {
                    resultType = SadTypeKind::Float;
                }
                // (AR) ISSUE-076/084 (ب″): معاملٌ ديناميّ (Any = حمولة ADT مجهولة النوع سكونيًّا،
                //      إحالة أماميّة/تعارُض) ولا عشريّ/نصّ صريح ⇒ نتيجةٌ ديناميّة (Any). فيُصدِر
                //      الخلفُ عمليّةً ثنائيّة ديناميّة تفحص وسم المعاملَين زمنَ التشغيل (عشريّ
                //      مُعلَّب أم صحيح) وتُنتج نتيجةً موسومة يفكّها نص()/اطبع — كالمفسّر تمامًا.
                //      المقارنات تُثبَّت Boolean لاحقًا؛ //،%،<<،>>،|،&،^ تُثبِّت Integer؛ **
                //      تُعالَج أدناه. لا يمسّ هذا العمليّات الساكنة (لا انحدار في البوّابة).
                // (EN) ISSUE-076/084 (ب″): a dynamic operand (Any = a statically-unknown ADT
                //      payload, forward-ref/conflict) with no explicit float/string ⇒ a dynamic
                //      (Any) result. The backend emits a dynamic binary op that inspects both
                //      operands' tags at runtime (boxed float vs int) and produces a tagged result
                //      that نص()/print decode — exactly like the interpreter. Comparisons are
                //      forced Boolean later; //,%,<<,>>,|,&,^ force Integer; ** is handled below.
                //      Does not touch static operands (no gate regression).
                else if (leftResult.type == SadTypeKind::Any || rightResult.type == SadTypeKind::Any)
                {
                    resultType = SadTypeKind::Any;
                }

                // (AR) ====== إصلاح BF-04 (OE-039): مقارنة صارمة بين أنواع مختلفة ======
                //      المفسر يُرجع `خطأ` عند `42 == "42"` (بمختلف الأنواع → غير متساويين).
                //      السلوك السابق في المترجم: كان يستدعي STRING_CMP على i64+ptr فيُنتج
                //      LLVM IR مكسور (مخرج فارغ).
                //      الحل الجذري: إذا كانت العملية == أو != وكان أحد المعاملين نصاً
                //      والآخر غير نصي (رقم، عشري، منطقي) → النتيجة ثابتة (false لـ ==، true لـ !=)
                //      دون إنشاء أي تعليمة SIR (لا STRING_CMP ولا EQ).
                // (EN) BF-04 fix (OE-039): Strict-type comparison short-circuit.
                //      Interpreter returns `false` for `42 == "42"` (different types → not equal).
                //      Previous compiler behavior: emit STRING_CMP on i64+ptr → broken LLVM IR.
                //      Root fix: when == or != with one string + one non-string operand,
                //      short-circuit to a constant boolean without emitting any SIR instruction.
                if ((binOp->op == Lexer::TokenType::OP_EQUAL ||
                     binOp->op == Lexer::TokenType::OP_NOT_EQUAL))
                {
                    bool leftIsStr = (leftResult.type == SadTypeKind::String);
                    bool rightIsStr = (rightResult.type == SadTypeKind::String);
                    bool leftIsNumeric = (leftResult.type == SadTypeKind::Integer ||
                                          leftResult.type == SadTypeKind::Float ||
                                          leftResult.type == SadTypeKind::Boolean);
                    bool rightIsNumeric = (rightResult.type == SadTypeKind::Integer ||
                                           rightResult.type == SadTypeKind::Float ||
                                           rightResult.type == SadTypeKind::Boolean);

                    // (AR) خلط نص بنوع رقمي/منطقي (وليس parameter مُستنتَج)
                    // (EN) Mixing string with numeric/boolean (not an inferred parameter)
                    bool mixedTypes =
                        (leftIsStr && !leftResult.isParameter && rightIsNumeric) ||
                        (rightIsStr && !rightResult.isParameter && leftIsNumeric);

                    if (mixedTypes)
                    {
                        // (AR) النتيجة ثابتة: == بين أنواع مختلفة = false، != = true
                        bool comparisonResult = (binOp->op == Lexer::TokenType::OP_NOT_EQUAL);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildBinaryOp: مقارنة صارمة بين أنواع مختلفة → "
                                  << (comparisonResult ? "true" : "false") << " (ثابت)" << std::endl;
#endif
                        return BuildResult(comparisonResult ? "true" : "false",
                                           SadTypeKind::Boolean, true);
                    }
                }

                // (AR) تحديد رمز العملية (SIROpcode) بناءً على TokenType (token.h:205-229)
                // (EN) Determine SIROpcode based on TokenType
                SIROpcode opcode;
                bool isComparison = false; // (AR) عمليات المقارنة تُرجع BOOL

                // (AR) العملية من expressions.h:43 - op: Lexer::TokenType
                // (EN) Operation from expressions.h:43
                switch (binOp->op)
                {
                // ========== العمليات الحسابية (token.h:205-210) ==========
                case Lexer::TokenType::OP_PLUS:
                    // (AR) إذا كانت العملية على نصوص: STRING_CONCAT (sir_types.h:182)
                    // (EN) If operation on strings: STRING_CONCAT
                    if (isStringOp)
                    {
                        opcode = SIROpcode::STRING_CONCAT;
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildBinaryOp: عملية دمج نصوص (+)" << std::endl;
#endif
                    }
                    else if (leftIsArray && rightIsArray)
                    {
                        // (AR) مصفوفة + مصفوفة → دمج مصفوفات (ARRAY_CONCAT)
                        // (EN) array + array → array concatenation (ARRAY_CONCAT)
                        opcode = SIROpcode::ARRAY_CONCAT;
                        resultType = SadTypeKind::Array;
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildBinaryOp: عملية دمج مصفوفات (+)" << std::endl;
#endif
                    }
                    else
                    {
                        // (AR) جمع: ADD_I64 للأعداد الصحيحة، ADD_F64 للعشرية
                        opcode = (resultType == SadTypeKind::Float) ? SIROpcode::ADD_F64 : SIROpcode::ADD_I64;
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildBinaryOp: عملية جمع (+)" << std::endl;
#endif
                    }
                    break;

                case Lexer::TokenType::OP_MINUS:
                    // (AR) طرح: SUB_I64 للأعداد الصحيحة، SUB_F64 للعشرية
                    opcode = (resultType == SadTypeKind::Float) ? SIROpcode::SUB_F64 : SIROpcode::SUB_I64;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية طرح (-)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_MULTIPLY:
                    // (AR) ضرب: MUL_I64 للأعداد الصحيحة، MUL_F64 للعشرية
                    opcode = (resultType == SadTypeKind::Float) ? SIROpcode::MUL_F64 : SIROpcode::MUL_I64;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية ضرب (*)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_DIVIDE:
                    // (AR) ISSUE-063: دلالة المفسّر (المرجع) للقسمة `/`:
                    //      صحيح/صحيح ⇒ صحيح عند انعدام الباقي (6/3=2) وعشريّ عند وجوده
                    //      (7/2=3.5) — أي أنّ نوع النتيجة يتقرّر زمنَ التشغيل ⇒ نتيجةٌ
                    //      ديناميّة (Any) يفكّها الخلف (dynBinOp يفحص الباقي ويَسِم النتيجة).
                    //      معاملٌ عشريّ صريح ⇒ عشريّ ساكن (fdiv) كالمفسّر (useDouble).
                    // (EN) ISSUE-063: interpreter (reference) semantics for `/`:
                    //      int/int ⇒ int when the remainder is zero (6/3=2), float otherwise
                    //      (7/2=3.5) — the result kind is runtime-dependent ⇒ a dynamic (Any)
                    //      result decoded by the backend (dynBinOp checks the remainder and
                    //      tags the result). An explicit float operand ⇒ static Float (fdiv),
                    //      matching the interpreter's useDouble path.
                    opcode = SIROpcode::DIV_F64;
                    if (resultType != SadTypeKind::Float)
                        resultType = SadTypeKind::Any;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية قسمة (/) → "
                              << (resultType == SadTypeKind::Float ? "F64" : "ديناميكية (باقي زمن التشغيل)")
                              << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_FLOOR_DIVIDE:
                    // (AR) قسمة صحيحة أرضية: I64 عادةً؛ لكن معامِلٌ ديناميّ (Any) ⇒ نتيجةٌ
                    //      ديناميّة (يفكّها الخلف زمنَ التشغيل) بدل تصليب Integer الذي يطمس وسم
                    //      الحمولة (ISSUE-076/084). (EN) Floor division: I64 normally; but a dynamic
                    //      (Any) operand ⇒ dynamic result (backend decodes at runtime) instead of
                    //      pinning Integer which erases the payload tag (ISSUE-076/084).
                    // (AR) Amelia (ISSUE-063): معاملٌ عشريّ ⇒ نتيجةٌ عشريّة floor(fdiv)
                    //      كالمفسّر (7.5//2=3.0، −7.5//2=−4.0) — مرآةُ إصلاح `%`؛ التصليبُ
                    //      Integer كان يقتطع (3) ويناقض الطيَّ الواعي بالنوع (3.0).
                    // (EN) Amelia (ISSUE-063): a float operand ⇒ Float result floor(fdiv)
                    //      like the interpreter (7.5//2=3.0, -7.5//2=-4.0) — mirror of the `%`
                    //      fix; pinning Integer truncated (3), contradicting the type-aware
                    //      folding (3.0).
                    // (AR) دلالة المفسّر (المرجع) للقسمة الأرضيّة `//` على صحيحين: صحيحٌ
                    //      عادةً، لكنّ INT64_MIN // -1 يفيض i64 فيرقّيه المفسّر إلى عشريّ
                    //      (9223372036854775808.0) — أي أنّ نوع النتيجة يتقرّر زمنَ التشغيل
                    //      ⇒ نتيجةٌ ديناميّة (Any) يفكّها الخلف (dynBinOp يَسِمها عند الفيض)،
                    //      نظيرَ `/` تمامًا. كان تصليبُ Integer يُسقِط التنفيذ (sdiv فيض ⇒ #DE).
                    //      معاملٌ عشريّ ⇒ عشريّ ساكن floor(fdiv) كالمفسّر (7.5//2=3.0).
                    // (EN) Interpreter (reference) `//` semantics on two ints: usually int, but
                    //      INT64_MIN // -1 overflows i64 ⇒ the interpreter promotes to float
                    //      (9223372036854775808.0) — the result kind is runtime-dependent ⇒ a
                    //      dynamic (Any) result decoded by the backend (dynBinOp tags it on
                    //      overflow), exactly mirroring `/`. Pinning Integer crashed (sdiv
                    //      overflow ⇒ #DE). A float operand ⇒ static Float floor(fdiv) (7.5//2=3.0).
                    opcode = SIROpcode::FLOOR_DIV_I64;
                    if (resultType != SadTypeKind::Float)
                        resultType = SadTypeKind::Any;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية قسمة صحيحة (//)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_MODULO:
                    // (AR) باقي القسمة: MOD_I64. معامِلٌ ديناميّ (Any) ⇒ نتيجةٌ ديناميّة (يفكّها
                    //      الخلف) بدل تصليب Integer الذي يطمس وسم الحمولة (ISSUE-076/084).
                    //      ISSUE-063: معاملٌ عشريّ ⇒ نتيجةٌ عشريّة (fmod كالمفسّر: 7.5%2=1.5)
                    //      بدل تصليب Integer الذي كان يقتطع فيعطي 1 (srem عبر التطبيع).
                    // (EN) Modulo: MOD_I64. A dynamic (Any) operand ⇒ dynamic result (backend
                    //      decodes) instead of pinning Integer which erases the tag (ISSUE-076/084).
                    //      ISSUE-063: a float operand ⇒ Float result (fmod like the interpreter:
                    //      7.5%2=1.5) instead of pinning Integer (srem-via-truncation gave 1).
                    opcode = SIROpcode::MOD_I64;
                    if (resultType != SadTypeKind::Any && resultType != SadTypeKind::Float)
                        resultType = SadTypeKind::Integer;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية باقي القسمة (%)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_POWER:
                    // (AR) الأس: BUILTIN_POW (sir_types.h:223)
                    // (AR) إصلاح BF-04 (OE-006/OE-029): الحفاظ على نوع المعاملين.
                    //      إذا كان كلا المعاملين عددين صحيحين فالنتيجة صحيحة (مطابقة للمفسر)؛
                    //      إذا كان أحدهما عشرياً فالنتيجة عشرية. السلوك السابق كان يُجبر
                    //      Float دائماً مما يجعل `2 ** 3` يُخرج 8.0 بدل 8.
                    // (EN) Power operator: preserve operand types. If both ints → int result
                    //      (matches interpreter); if any float → float. Previous behavior forced
                    //      Float unconditionally, breaking parity with the interpreter.
                    opcode = SIROpcode::BUILTIN_POW;
                    // (AR) لا نُعدّل resultType — تركها كما حُدِّدت من المعاملين أعلاه
                    // (EN) Do NOT override resultType — keep what was inferred from operands above
                    // (AR) ISSUE-076/084 (ب″): BUILTIN_POW لا يعالج نتيجة Any الديناميّة ⇒ ثبِّتها
                    //      عشريّة (نظير معاملٍ عشريّ) بدل تسريب Any إلى مسارٍ لا يفكّ وسمها.
                    // (EN) ISSUE-076/084 (ب″): BUILTIN_POW has no dynamic-Any result path ⇒ pin it
                    //      to Float (as if a float operand) rather than leaking Any to a path that
                    //      would not decode the tag.
                    if (resultType == SadTypeKind::Any)
                        resultType = SadTypeKind::Float;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية الأس (**) — resultType="
                              << (resultType == SadTypeKind::Float ? "Float" : "Integer") << std::endl;
#endif
                    break;

                // ========== عمليات المقارنة (token.h:219-224) ==========
                case Lexer::TokenType::OP_EQUAL:
                    // (AR) يساوي: EQ للأرقام، STRING_CMP للنصوص (sir_types.h:125, 183)
                    // (EN) Equal: EQ for numbers, STRING_CMP for strings
                    if (isStringOp)
                    {
                        opcode = SIROpcode::STRING_CMP;
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildBinaryOp: عملية مقارنة نصوص (==)" << std::endl;
#endif
                    }
                    else
                    {
                        opcode = SIROpcode::EQ;
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildBinaryOp: عملية يساوي (==)" << std::endl;
#endif
                    }
                    isComparison = true;
                    break;

                case Lexer::TokenType::OP_NOT_EQUAL:
                    // (AR) لا يساوي: NE للأرقام، STRING_CMP مع NOT للنصوص
                    // (EN) Not equal: NE for numbers, STRING_CMP with NOT for strings
                    if (isStringOp)
                    {
                        // (AR) سيتم معالجة النفي لاحقاً
                        opcode = SIROpcode::STRING_CMP;
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildBinaryOp: عملية عدم تساوي نصوص (!=)" << std::endl;
#endif
                    }
                    else
                    {
                        opcode = SIROpcode::NE;
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildBinaryOp: عملية لا يساوي (!=)" << std::endl;
#endif
                    }
                    isComparison = true;
                    break;

                case Lexer::TokenType::OP_LESS:
                    // (AR) أصغر من: LT (sir_types.h:127)
                    opcode = SIROpcode::LT;
                    isComparison = true;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية أصغر من (<)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_LESS_EQUAL:
                    // (AR) أصغر أو يساوي: LE (sir_types.h:128)
                    opcode = SIROpcode::LE;
                    isComparison = true;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية أصغر أو يساوي (<=)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_GREATER:
                    // (AR) أكبر من: GT (sir_types.h:129)
                    opcode = SIROpcode::GT;
                    isComparison = true;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية أكبر من (>)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_GREATER_EQUAL:
                    // (AR) أكبر أو يساوي: GE (sir_types.h:130)
                    opcode = SIROpcode::GE;
                    isComparison = true;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية أكبر أو يساوي (>=)" << std::endl;
#endif
                    break;

                // ========== العمليات المنطقية (token.h:227-228) ==========
                case Lexer::TokenType::OP_AND:
                    // (AR) AND المنطقي: AND (sir_types.h:118)
                    opcode = SIROpcode::AND;
                    isComparison = true; // (AR) النتيجة منطقية
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية AND (&&)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_OR:
                    // (AR) OR المنطقي: OR (sir_types.h:119)
                    opcode = SIROpcode::OR;
                    isComparison = true; // (AR) النتيجة منطقية
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية OR (||)" << std::endl;
#endif
                    break;

                // ========== العمليات البتية (token.h) ==========
                case Lexer::TokenType::OP_XOR:
                    // (AR) XOR بتّي: Xor (sir_opcodes.h)
                    opcode = SIROpcode::XOR;
                    resultType = SadTypeKind::Integer;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية XOR بتي (^)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_BITWISE_AND:
                    // (AR) AND بتّي: AND
                    opcode = SIROpcode::AND;
                    resultType = SadTypeKind::Integer;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية AND بتي (&)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_BITWISE_OR:
                    // (AR) OR بتّي: OR
                    opcode = SIROpcode::OR;
                    resultType = SadTypeKind::Integer;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية OR بتي (|)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_SHIFT_LEFT:
                    // (AR) إزاحة يسار: Shl
                    opcode = SIROpcode::SHL;
                    resultType = SadTypeKind::Integer;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية إزاحة يسار (<<)" << std::endl;
#endif
                    break;

                case Lexer::TokenType::OP_SHIFT_RIGHT:
                    // (AR) إزاحة يمين: Shr
                    opcode = SIROpcode::SHR;
                    resultType = SadTypeKind::Integer;
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية إزاحة يمين (>>)" << std::endl;
#endif
                    break;

                // ================================================================
                // (AR) عامل العضوية "في" (IN operator) — دعم شامل
                //      يدعم ثلاثة أنواع على الجانب الأيمن:
                //      1. مصفوفة: حلقة بحث خطي inline (5 كتل SIR)
                //      2. نص:     strstr() عبر BUILTIN_STRING_CONTAINS
                //      3. خريطة:  __sad_map_has() للبحث في المفاتيح
                //      يُرجع دائماً قيمة منطقية (Boolean/i64: 1=true, 0=false)
                // (EN) Membership operator "في" (IN) — comprehensive support
                //      Supports 3 RHS types: Array (inline loop), String (strstr),
                //      Map (__sad_map_has). Always returns Boolean.
                // ================================================================
                case Lexer::TokenType::KEYWORD_IN:
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عامل العضوية (في) — نوع الأيمن: "
                              << static_cast<int>(rightResult.type) << std::endl;
#endif
                    // ─────────────────────────────────────────────────────
                    // (AR) الحالة 1: نص في نص → BUILTIN_STRING_CONTAINS
                    //      "عالم" في "مرحبا بالعالم" → true
                    //      يستخدم strstr() الموجودة في LLVM codegen
                    // (EN) Case 1: String IN String → BUILTIN_STRING_CONTAINS
                    // ─────────────────────────────────────────────────────
                    if (rightResult.type == SadTypeKind::String)
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] في: نص في نص → BUILTIN_STRING_CONTAINS" << std::endl;
#endif
                        std::string containsReg = b_.newTempRegister();
                        SIRInstruction inst(SIROpcode::BUILTIN_STRING_CONTAINS);
                        inst.result = SIROperand::Register(containsReg, SadTypeKind::Boolean);
                        // (AR) المعامل 0 = النص الأصلي (haystack)، المعامل 1 = النص المبحوث عنه (needle)
                        inst.operands.push_back(SIROperand::Register(rightResult.registerName, SadTypeKind::String));
                        inst.operands.push_back(SIROperand::Register(leftResult.registerName, SadTypeKind::String));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(inst);
                        return BuildResult(containsReg, SadTypeKind::Boolean);
                    }

                    // ─────────────────────────────────────────────────────
                    // (AR) الحالة 2: مفتاح في خريطة → CALL __sad_map_has
                    //      "لون" في خريطة → true إذا وُجد المفتاح
                    //      يستخدم __sad_map_has() الموجودة في LLVM codegen
                    // (EN) Case 2: Key IN Map → CALL __sad_map_has
                    // ─────────────────────────────────────────────────────
                    if (rightResult.type == SadTypeKind::Map)
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] في: مفتاح في خريطة → CALL __sad_map_has" << std::endl;
#endif
                        std::string hasReg = b_.newTempRegister();
                        SIRInstruction inst(SIROpcode::CALL);
                        inst.result = SIROperand::Register(hasReg, SadTypeKind::Boolean);
                        inst.operands.push_back(SIROperand::Label("__sad_map_has"));
                        // (AR) المعامل 0 = الخريطة، المعامل 1 = المفتاح (نص)
                        inst.operands.push_back(SIROperand::Register(rightResult.registerName, SadTypeKind::Map));
                        inst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(inst);
                        return BuildResult(hasReg, SadTypeKind::Boolean);
                    }

                    // ─────────────────────────────────────────────────────
                    // (AR) الحالة 3: قيمة في مصفوفة → حلقة بحث خطي inline
                    //      "أحمر" في ["أحمر"، "أخضر"] → true
                    //      يُنشئ 5 كتل: cond, body, found, inc, exit
                    //      يدعم مقارنة نصوص (STRING_CMP) وأرقام (EQ)
                    // (EN) Case 3: Value IN Array → inline linear search loop
                    // ─────────────────────────────────────────────────────
                    // (AR) المعامل الأيسر = القيمة المبحوث عنها، الأيمن = المصفوفة
                    // (EN) Left = value to search, Right = array

                    // (AR) إنشاء متغير نتيجة منطقي (مبدئياً false)
                    std::string inResultReg = b_.newTempRegister();
                    {
                        SIRInstruction allocResult(SIROpcode::ALLOC);
                        allocResult.result = SIROperand::Register(inResultReg, SadTypeKind::Boolean);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocResult);
                    }
                    {
                        SIRInstruction storeInit(SIROpcode::STORE);
                        storeInit.operands.push_back(SIROperand::ConstantBool(false));
                        storeInit.operands.push_back(SIROperand::Register(inResultReg, SadTypeKind::Boolean));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeInit);
                    }

                    // (AR) إنشاء عداد الفهرس
                    std::string inIdxReg = b_.newTempRegister();
                    {
                        SIRInstruction allocIdx(SIROpcode::ALLOC);
                        allocIdx.result = SIROperand::Register(inIdxReg, SadTypeKind::Integer);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(allocIdx);
                    }
                    {
                        SIRInstruction storeZero(SIROpcode::STORE);
                        storeZero.operands.push_back(SIROperand::ConstantI64(0));
                        storeZero.operands.push_back(SIROperand::Register(inIdxReg, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeZero);
                    }

                    // (AR) حساب طول المصفوفة
                    std::string inLenReg = b_.newTempRegister();
                    {
                        SIRInstruction lenInst(SIROpcode::ARRAY_LEN);
                        lenInst.result = SIROperand::Register(inLenReg, SadTypeKind::Integer);
                        SIROperand arrOp = SIROperand::Register(rightResult.registerName, rightResult.type);
                        lenInst.operands.push_back(arrOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(lenInst);
                    }

                    // (AR) إنشاء الكتل الأساسية للحلقة
                    std::string condLabel = b_.newLabel("in_cond");
                    std::string bodyLabel = b_.newLabel("in_body");
                    std::string foundLabel = b_.newLabel("in_found");
                    std::string incLabel = b_.newLabel("in_inc");
                    std::string exitLabel = b_.newLabel("in_exit");

                    auto condBlock = b_.createBasicBlock(condLabel);
                    auto bodyBlock = b_.createBasicBlock(bodyLabel);
                    auto foundBlock = b_.createBasicBlock(foundLabel);
                    auto incBlock = b_.createBasicBlock(incLabel);
                    auto exitBlock = b_.createBasicBlock(exitLabel);

                    // (AR) قفز إلى كتلة الشرط
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(
                            SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) كتلة الشرط: index < length
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(condBlock);
                    b_.currentBlock_ = condBlock;

                    std::string loadedIdx = b_.newTempRegister();
                    {
                        SIRInstruction loadIdx(SIROpcode::LOAD);
                        loadIdx.result = SIROperand::Register(loadedIdx, SadTypeKind::Integer);
                        loadIdx.operands.push_back(SIROperand::Register(inIdxReg, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(loadIdx);
                    }

                    std::string inCmpReg = b_.newTempRegister();
                    {
                        SIRInstruction cmpInst(SIROpcode::LT);
                        cmpInst.result = SIROperand::Register(inCmpReg, SadTypeKind::Boolean);
                        cmpInst.operands.push_back(SIROperand::Register(loadedIdx, SadTypeKind::Integer));
                        cmpInst.operands.push_back(SIROperand::Register(inLenReg, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(cmpInst);
                    }

                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(
                            SIRInstruction::BranchCond(
                                SIROperand::Register(inCmpReg, SadTypeKind::Boolean),
                                SIROperand::Label(bodyLabel),
                                SIROperand::Label(exitLabel)));

                    // (AR) كتلة الجسم: elem = array[index]; if elem == value → found
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(bodyBlock);
                    b_.currentBlock_ = bodyBlock;

                    // (AR) تحديد نوع العنصر من نوع عناصر المصفوفة أو من نوع المعامل الأيسر
                    SadTypeKind inElemType = (rightResult.elementType != SadTypeKind::Void)
                                                 ? rightResult.elementType
                                                 : leftResult.type;

                    std::string inElemReg = b_.newTempRegister();
                    {
                        SIRInstruction getInst(SIROpcode::ARRAY_GET);
                        getInst.result = SIROperand::Register(inElemReg, inElemType);
                        getInst.operands.push_back(SIROperand::Register(rightResult.registerName, rightResult.type));
                        getInst.operands.push_back(SIROperand::Register(loadedIdx, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(getInst);
                    }

                    // (AR) مقارنة: elem == value — نصوص تستخدم STRING_CMP، أرقام تستخدم EQ
                    std::string inEqReg = b_.newTempRegister();
                    if (inElemType == SadTypeKind::String)
                    {
                        SIRInstruction cmpStr(SIROpcode::STRING_CMP);
                        cmpStr.result = SIROperand::Register(inEqReg, SadTypeKind::Boolean);
                        cmpStr.operands.push_back(SIROperand::Register(inElemReg, SadTypeKind::String));
                        cmpStr.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(cmpStr);
                    }
                    else
                    {
                        SIRInstruction eqInst(SIROpcode::EQ);
                        eqInst.result = SIROperand::Register(inEqReg, SadTypeKind::Boolean);
                        eqInst.operands.push_back(SIROperand::Register(inElemReg, inElemType));
                        eqInst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(eqInst);
                    }

                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(
                            SIRInstruction::BranchCond(
                                SIROperand::Register(inEqReg, SadTypeKind::Boolean),
                                SIROperand::Label(foundLabel),
                                SIROperand::Label(incLabel)));

                    // (AR) كتلة الوجود: result = true → exit
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(foundBlock);
                    b_.currentBlock_ = foundBlock;
                    {
                        SIRInstruction storeTrue(SIROpcode::STORE);
                        storeTrue.operands.push_back(SIROperand::ConstantBool(true));
                        storeTrue.operands.push_back(SIROperand::Register(inResultReg, SadTypeKind::Boolean));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeTrue);
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(
                            SIRInstruction::Branch(SIROperand::Label(exitLabel)));

                    // (AR) كتلة الزيادة: index++ → cond
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(incBlock);
                    b_.currentBlock_ = incBlock;
                    {
                        std::string incReg = b_.newTempRegister();
                        SIRInstruction addInst(SIROpcode::ADD_I64);
                        addInst.result = SIROperand::Register(incReg, SadTypeKind::Integer);
                        addInst.operands.push_back(SIROperand::Register(loadedIdx, SadTypeKind::Integer));
                        addInst.operands.push_back(SIROperand::ConstantI64(1));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(addInst);

                        SIRInstruction storeInc(SIROpcode::STORE);
                        storeInc.operands.push_back(SIROperand::Register(incReg, SadTypeKind::Integer));
                        storeInc.operands.push_back(SIROperand::Register(inIdxReg, SadTypeKind::Integer));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(storeInc);
                    }
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(
                            SIRInstruction::Branch(SIROperand::Label(condLabel)));

                    // (AR) كتلة الخروج: تحميل النتيجة النهائية
                    if (b_.currentFunction_)
                        b_.currentFunction_->addBasicBlock(exitBlock);
                    b_.currentBlock_ = exitBlock;

                    std::string inLoadResult = b_.newTempRegister();
                    {
                        SIRInstruction loadRes(SIROpcode::LOAD);
                        loadRes.result = SIROperand::Register(inLoadResult, SadTypeKind::Boolean);
                        loadRes.operands.push_back(SIROperand::Register(inResultReg, SadTypeKind::Boolean));
                        if (b_.currentBlock_)
                            b_.currentBlock_->instructions.push_back(loadRes);
                    }

                    return BuildResult(inLoadResult, SadTypeKind::Boolean);
                }

                default:
// (AR) عملية غير مدعومة
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: عملية غير مدعومة: "
                              << static_cast<int>(binOp->op) << std::endl;
#endif
                    b_.errors_.push_back("عملية ثنائية غير مدعومة / Unsupported binary operation");
                    return BuildResult(resultReg, resultType);
                }

                // (AR) نوع النتيجة للمقارنات هو BOOL
                // (EN) Result type for comparisons is BOOL
                if (isComparison)
                {
                    resultType = SadTypeKind::Boolean;
                }

                // (AR) إنشاء معاملات SIR (sir_types.h:306-312 - SIROperand::Register)
                // (EN) Create SIR operands
                SIROperand leftOp, rightOp, resultOp;

                // (AR) المعامل الأيسر
                if (leftResult.isConstant)
                {
                    // (AR) قيمة ثابتة - استخدم ConstantI64/ConstantF64/ConstantString/ConstantBool
                    if (leftResult.type == SadTypeKind::String)
                    {
                        leftOp = SIROperand::ConstantString(leftResult.constantValue);
                    }
                    else if (leftResult.type == SadTypeKind::Float)
                    {
                        leftOp = SIROperand::ConstantF64(std::stod(leftResult.constantValue));
                    }
                    else if (leftResult.type == SadTypeKind::Boolean)
                    {
                        leftOp = SIROperand::ConstantBool(leftResult.constantValue == "true");
                    }
                    else if (leftResult.type == SadTypeKind::Pointer)
                    {
                        // (AR) لاشيء/null → عدد صحيح بقيمة 0
                        leftOp = SIROperand::ConstantI64(0);
                        leftOp.dataType = SadTypeKind::Integer;
                    }
                    else
                    {
                        leftOp = SIROperand::ConstantI64(std::stoll(leftResult.constantValue));
                    }
                }
                else
                {
                    // (AR) سجل - استخدم Register
                    leftOp = SIROperand::Register(leftResult.registerName, leftResult.type);
                }

                // (AR) المعامل الأيمن
                if (rightResult.isConstant)
                {
                    if (rightResult.type == SadTypeKind::String)
                    {
                        rightOp = SIROperand::ConstantString(rightResult.constantValue);
                    }
                    else if (rightResult.type == SadTypeKind::Float)
                    {
                        rightOp = SIROperand::ConstantF64(std::stod(rightResult.constantValue));
                    }
                    else if (rightResult.type == SadTypeKind::Boolean)
                    {
                        rightOp = SIROperand::ConstantBool(rightResult.constantValue == "true");
                    }
                    else if (rightResult.type == SadTypeKind::Pointer)
                    {
                        // (AR) لاشيء/null → عدد صحيح بقيمة 0
                        rightOp = SIROperand::ConstantI64(0);
                        rightOp.dataType = SadTypeKind::Integer;
                    }
                    else
                    {
                        rightOp = SIROperand::ConstantI64(std::stoll(rightResult.constantValue));
                    }
                }
                else
                {
                    rightOp = SIROperand::Register(rightResult.registerName, rightResult.type);
                }

                // (AR) سجل النتيجة
                resultOp = SIROperand::Register(resultReg, resultType);

                // (AR) تحويل i64 إلى f64 إذا كانت العملية عشرية ولكن أحد المعاملين صحيح
                // (EN) Convert i64 to f64 if operation is float but one operand is integer
                if (resultType == SadTypeKind::Float)
                {
                    if (leftOp.dataType == SadTypeKind::Integer && !leftResult.isConstant)
                    {
                        std::string convReg = b_.newTempRegister();
                        SIRInstruction convInst;
                        convInst.opcode = SIROpcode::I64_TO_F64;
                        convInst.result = SIROperand::Register(convReg, SadTypeKind::Float);
                        convInst.operands.push_back(leftOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(convInst);
                        leftOp = SIROperand::Register(convReg, SadTypeKind::Float);
                    }
                    else if (leftResult.isConstant && leftResult.type == SadTypeKind::Integer)
                    {
                        double val = std::stod(leftResult.constantValue);
                        leftOp = SIROperand::ConstantF64(val);
                    }
                    if (rightOp.dataType == SadTypeKind::Integer && !rightResult.isConstant)
                    {
                        std::string convReg = b_.newTempRegister();
                        SIRInstruction convInst;
                        convInst.opcode = SIROpcode::I64_TO_F64;
                        convInst.result = SIROperand::Register(convReg, SadTypeKind::Float);
                        convInst.operands.push_back(rightOp);
                        if (b_.currentBlock_)
                            b_.currentBlock_->addInstruction(convInst);
                        rightOp = SIROperand::Register(convReg, SadTypeKind::Float);
                    }
                    else if (rightResult.isConstant && rightResult.type == SadTypeKind::Integer)
                    {
                        double val = std::stod(rightResult.constantValue);
                        rightOp = SIROperand::ConstantF64(val);
                    }
                }

                // (AR) إنشاء تعليمة SIR (sir_instruction.h:100-107 - SIRInstruction::Binary)
                // (EN) Create SIR instruction
                SIRInstruction inst = SIRInstruction::Binary(opcode, resultOp, leftOp, rightOp);

                // (AR) إضافة التعليمة للكتلة الحالية
                // (EN) Add instruction to current block
                if (b_.currentBlock_)
                {
                    b_.currentBlock_->addInstruction(inst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: تمت إضافة التعليمة للكتلة الحالية" << std::endl;
#endif
                }
                else
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildBinaryOp: تحذير - لا توجد كتلة حالية!" << std::endl;
#endif
                }

                // (AR) إصلاح: عند مقارنة نصوص بـ != يجب نفي نتيجة STRING_CMP
                //      STRING_CMP تُرجع true إذا متساويين، لذا != تحتاج NOT
                // (EN) Fix: For string != comparison, negate STRING_CMP result
                //      STRING_CMP returns true if equal, so != needs NOT
                if (opcode == SIROpcode::STRING_CMP && binOp->op == Lexer::TokenType::OP_NOT_EQUAL)
                {
                    std::string negReg = b_.newTempRegister();
                    SIRInstruction notInst;
                    notInst.opcode = SIROpcode::NOT;
                    notInst.result = SIROperand::Register(negReg, SadTypeKind::Boolean);
                    notInst.operands.push_back(SIROperand::Register(resultReg, SadTypeKind::Boolean));
                    if (b_.currentBlock_)
                        b_.currentBlock_->addInstruction(notInst);
                    resultReg = negReg;
                    resultType = SadTypeKind::Boolean;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildBinaryOp: النتيجة في سجل " << resultReg
                          << " بنوع " << static_cast<int>(resultType) << std::endl;
#endif

                return BuildResult(resultReg, resultType);
            }

            // ============================================================================
            // buildUnaryOp - بناء عملية أحادية
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:456
            // التوقيع / Signature: BuildResult buildUnaryOp(AST::UnaryOpNode* unaryOp);
            //
            // المعاملات / Parameters:
            // - unaryOp: AST::UnaryOpNode* = Sad::AST::UnaryExpr* (sir_builder.h:67)
            //
            // UnaryExpr Members (expressions.h:78-81):
            // - op: Lexer::TokenType (line 80)
            // - operand: ExprPtr (line 81)
            //
            // TokenType للعمليات الأحادية (token.h):
            // - OP_MINUS (206): السالب
            // - OP_NOT (229): النفي المنطقي
            //
            // SIROpcode (sir_types.h):
            // - NEG (115): السالب
            // - NOT (121): النفي
            //
            // SIRInstruction::Unary (sir_instruction.h:114-120):
            // - الاستخدام: SIRInstruction::Unary(opcode, result, operand)
            // ============================================================================
        } // namespace SIR
    } // namespace Compiler
} // namespace Sad