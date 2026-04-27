// ============================================================================
// sir_builder_calls_args.cpp — بناء وسائط استدعاء الدوال
// ============================================================================
// المؤلف / Author: Sad Compiler Team
// التاريخ / Date: January 2026
// الغرض / Purpose:
//   (AR) مُستخرج من sir_builder_calls.cpp وفق قاعدة CW-05 (≤800 سطر)
//        يحتوي على منطق بناء قائمة الوسائط لاستدعاء الدوال العادية:
//        1. buildCallArgumentsList — بناء SIROperand لكل وسيط
//        2. fillDefaultCallArguments — تعبئة القيم الافتراضية الناقصة
//
//   (EN) Extracted from sir_builder_calls.cpp per CW-05 (≤800 lines)
//        Contains argument building logic for regular function calls:
//        1. buildCallArgumentsList — build SIROperand for each argument
//        2. fillDefaultCallArguments — fill in missing default values
//
// الملفات المرتبطة / Related files:
//   - sir_builder_calls.cpp          — الدالة الرئيسية buildFunctionCall
//   - sir_builder_calls_macro.cpp    — معالجة الماكرو
//   - sir_builder_calls_special.cpp  — callable object + super constructor
//   - compiler/include/frontend/sir_builder.h — التصريحات
// ============================================================================

#include "sir_builder.h"
#include "builders/call_builder.h"
#include <string>
#include <cstdio>
#include "sir_builder.h"
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
            // buildCallArgumentsList — بناء قائمة الوسائط لاستدعاء دالة
            // ============================================================================
            // (AR) يُقيّم كل وسيط باستخدام b_.buildExpression ثم يحوّله إلى SIROperand
            //      مع الأخذ بعين الاعتبار الثوابت (Integer, Float, String, Boolean)
            //      مقابل السجلات (registers)
            //
            // (EN) Evaluates each argument using b_.buildExpression then converts to SIROperand
            //      Handles constants (Integer, Float, String, Boolean)
            //      vs registers correctly
            //
            // الإرجاع / Return:
            //   true = نجح بناء جميع الوسائط
            //   false = فشل بناء أحد الوسائط (تم تسجيل الخطأ في b_.errors_)
            // ============================================================================
            bool CallBuilder::buildCallArgumentsList(
                AST::FunctionCallNode *call,
                std::vector<SIROperand> &outArgOperands,
                std::vector<BuildResult> &outArgResults)
            {
                for (const auto &arg : call->arguments)
                {
                    // (AR) بناء كل وسيط باستخدام b_.buildExpression
                    // (EN) Build each argument using b_.buildExpression
                    BuildResult argResult = b_.buildExpression(arg.get());

                    // (AR) فحص الفشل: إذا كان فارغاً وليس ثابتاً
                    // (EN) Check failure: if empty and not a constant
                    if (argResult.registerName.empty() && !argResult.isConstant)
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildCallArgumentsList: failed to build argument" << std::endl;
#endif
                        b_.errors_.push_back("Error: Failed to build function argument");
                        return false;
                    }

                    outArgResults.push_back(argResult);

                    // ================================================================
                    // (AR) إنشاء SIROperand للوسيط
                    //      إذا كان ثابتاً → استخدم Constant* factory methods
                    //      إذا كان سجلاً → استخدم Register
                    // (EN) Create SIROperand for argument
                    //      If constant → use Constant* factory methods
                    //      If register → use Register
                    // ================================================================
                    SIROperand argOp;

                    if (argResult.isConstant)
                    {
                        switch (argResult.type)
                        {
                        case SadTypeKind::String:
                            argOp = SIROperand::ConstantString(argResult.constantValue);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildCallArgumentsList: STRING constant='"
                                      << argResult.constantValue << "'" << std::endl;
#endif
                            break;

                        case SadTypeKind::Integer:
                        {
                            int64_t intVal = std::stoll(argResult.constantValue);
                            argOp = SIROperand::ConstantI64(intVal);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildCallArgumentsList: I64 constant="
                                      << intVal << std::endl;
#endif
                            break;
                        }

                        case SadTypeKind::Float:
                        {
                            double floatVal = std::stod(argResult.constantValue);
                            argOp = SIROperand::ConstantF64(floatVal);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildCallArgumentsList: F64 constant="
                                      << floatVal << std::endl;
#endif
                            break;
                        }

                        case SadTypeKind::Boolean:
                        {
                            bool boolVal = (argResult.constantValue == "true");
                            argOp = SIROperand::ConstantBool(boolVal);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildCallArgumentsList: BOOL constant="
                                      << boolVal << std::endl;
#endif
                            break;
                        }

                        default:
                            // (AR) أنواع أخرى — استخدم Register كافتراضي
                            // (EN) Other types — fall back to Register
                            argOp = SIROperand::Register(argResult.registerName, argResult.type);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildCallArgumentsList: register='"
                                      << argResult.registerName << "', type="
                                      << static_cast<int>(argResult.type) << std::endl;
#endif
                            break;
                        }
                    }
                    else
                    {
                        // (AR) متغير أو تعبير — استخدم Register
                        // (EN) Variable or expression — use Register
                        argOp = SIROperand::Register(argResult.registerName, argResult.type);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildCallArgumentsList: register='"
                                  << argResult.registerName << "', type="
                                  << static_cast<int>(argResult.type) << std::endl;
#endif
                    }

                    outArgOperands.push_back(argOp);
                }

                return true;
            }

            // ============================================================================
            // fillDefaultCallArguments — تعبئة القيم الافتراضية للمعاملات الناقصة
            // ============================================================================
            // (AR) إذا كان عدد الوسائط الممررة أقل من عدد المعاملات في الدالة،
            //      يبحث عن القيم الافتراضية في AST ويبنيها كوسائط إضافية.
            //      هذا يدعم أسلوب: دالة حساب(أ، ب = 0) → حساب(5)
            //
            // (EN) If fewer arguments passed than function parameters,
            //      looks up default values in AST and builds them as additional arguments.
            //      Supports: function calc(a, b = 0) → calc(5)
            // ============================================================================
            void CallBuilder::fillDefaultCallArguments(
                AST::FunctionCallNode *call,
                const std::string &funcName,
                std::vector<SIROperand> &argOperands,
                std::vector<BuildResult> &argResults)
            {
                // (AR) البحث عن تعريف الدالة في جدول الدوال
                // (EN) Look up function definition in function table
                auto ftIt = b_.functionTable_.find(funcName);
                if (ftIt == b_.functionTable_.end() || !ftIt->second.astDecl)
                    return;

                auto *astDecl = ftIt->second.astDecl;
                size_t numParams = astDecl->parameters.size();
                size_t numArgs = argOperands.size();

                // (AR) إذا كان عدد الوسائط كافياً → لا شيء يُفعل
                // (EN) If argument count is sufficient → nothing to do
                if (numArgs >= numParams)
                    return;

                // (AR) بناء القيم الافتراضية للمعاملات الناقصة
                // (EN) Build default values for missing parameters
                for (size_t i = numArgs; i < numParams; i++)
                {
                    if (!astDecl->parameters[i].defaultValue)
                        continue;

                    // (AR) بناء التعبير الافتراضي من AST
                    // (EN) Build default value expression from AST
                    auto defResult = b_.buildExpression(astDecl->parameters[i].defaultValue.get());
                    argResults.push_back(defResult);

                    SIROperand defOp;
                    if (defResult.isConstant)
                    {
                        switch (defResult.type)
                        {
                        case SadTypeKind::String:
                            defOp = SIROperand::ConstantString(defResult.constantValue);
                            break;
                        case SadTypeKind::Integer:
                            defOp = SIROperand::ConstantI64(std::stoll(defResult.constantValue));
                            break;
                        case SadTypeKind::Float:
                            defOp = SIROperand::ConstantF64(std::stod(defResult.constantValue));
                            break;
                        case SadTypeKind::Boolean:
                            defOp = SIROperand::ConstantBool(defResult.constantValue == "true");
                            break;
                        default:
                            defOp = SIROperand::Register(defResult.registerName, defResult.type);
                            break;
                        }
                    }
                    else
                    {
                        defOp = SIROperand::Register(defResult.registerName, defResult.type);
                    }
                    argOperands.push_back(defOp);
                }
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
