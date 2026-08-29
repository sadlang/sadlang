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
#include "error_manager.h" // (AR) buildBilingualMessage من كتالوج الأخطاء (مصدر الحقيقة)
#include "error_catalog.h" // (AR) RenderContext (حاملُ placeholders)
#include "error_codes.h"   // (AR) ErrorCode::INT_COMPILER_NULL_IR
#include <string>
#include <cstdio>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include "sad_debug_log.h"
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
                    const std::size_t errorsBefore = b_.errors_.size();
                    BuildResult argResult = b_.buildExpression(arg.get());

                    // (AR) فحص الفشل: إذا كان فارغاً وليس ثابتاً
                    // (EN) Check failure: if empty and not a constant
                    if (argResult.registerName.empty() && !argResult.isConstant)
                    {
#ifndef NDEBUG
                        SAD_DEBUG_LOG_LINE("[DEBUG] buildCallArgumentsList: failed to build argument");
#endif
                        // (AR) 🔑 إن كان بناءُ الوسيطِ قد سجّل تشخيصَه فالتشخيصُ
                        //      قائمٌ يسمّي العلّةَ باسمِها (رتبةٌ خاطئةٌ، نوعٌ لا
                        //      يلائم…). وإضافةُ رسالةٍ عامّةٍ فوقَه تُغرِق الحقيقيَّ
                        //      بضجيجٍ لا يدلُّ على شيء — والمستعمِلُ يقرأ الأخيرَ.
                        //      فلا نتكلّمُ إلّا حين لا يكونُ أحدٌ قد تكلّم.
                        // (EN) Only speak when nothing else did: a generic message
                        //      stacked on a precise one buries the precise one.
                        if (b_.errors_.size() == errorsBefore)
                        {
                            // (AR) فشلٌ صامتٌ في بناءِ وسيطٍ خللٌ في المصرّفِ نفسِه
                            //      (ICE) لا علّةٌ في برنامجِ المستعمِل — ومن الكتالوج
                            //      لا سلسلةً خامّةً بلا رمزٍ ولا ترجمة. وحمولةُ
                            //      {detail} رتبةُ الوسيطِ عددًا: بيانٌ لا نثر.
                            Sad::Errors::RenderContext ctx;
                            ctx.placeholders = {
                                {"detail", std::to_string(outArgResults.size() + 1)}};
                            b_.errors_.push_back(
                                Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                                    Sad::Errors::ErrorCode::INT_COMPILER_NULL_IR, ctx));
                        }
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
                            SAD_DEBUG_LOG_LINE("[DEBUG] buildCallArgumentsList: STRING constant='"
                                      << argResult.constantValue << "'");
#endif
                            break;

                        case SadTypeKind::Integer:
                        {
                            int64_t intVal = std::stoll(argResult.constantValue);
                            argOp = SIROperand::ConstantI64(intVal);
#ifndef NDEBUG
                            SAD_DEBUG_LOG_LINE("[DEBUG] buildCallArgumentsList: I64 constant="
                                      << intVal);
#endif
                            break;
                        }

                        case SadTypeKind::Float:
                        {
                            double floatVal = std::stod(argResult.constantValue);
                            argOp = SIROperand::ConstantF64(floatVal);
#ifndef NDEBUG
                            SAD_DEBUG_LOG_LINE("[DEBUG] buildCallArgumentsList: F64 constant="
                                      << floatVal);
#endif
                            break;
                        }

                        case SadTypeKind::Boolean:
                        {
                            bool boolVal = (argResult.constantValue == "true");
                            argOp = SIROperand::ConstantBool(boolVal);
#ifndef NDEBUG
                            SAD_DEBUG_LOG_LINE("[DEBUG] buildCallArgumentsList: BOOL constant="
                                      << boolVal);
#endif
                            break;
                        }

                        default:
                            // (AR) أنواع أخرى — استخدم Register كافتراضي
                            // (EN) Other types — fall back to Register
                            argOp = SIROperand::Register(argResult.registerName, argResult.type);
#ifndef NDEBUG
                            SAD_DEBUG_LOG_LINE("[DEBUG] buildCallArgumentsList: register='"
                                      << argResult.registerName << "', type="
                                      << static_cast<int>(argResult.type));
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
                        SAD_DEBUG_LOG_LINE("[DEBUG] buildCallArgumentsList: register='"
                                  << argResult.registerName << "', type="
                                  << static_cast<int>(argResult.type));
#endif
                    }

                    // ================================================================
                    // (AR) 🔑 إثباتُ الحضورِ النصّيِّ البنيويِّ — هنا حصرًا، حيث يجتمع
                    //      تعبيرُ AST وجدولُ الرموز: وسيطٌ هو قراءةُ متغيّرٍ/معاملٍ
                    //      **مُصرَّحٍ** «نص» (declaredSurfaceType == String — والعدميُّ
                    //      نوعُه السطحيُّ Optional فلا يجتاز)، أو حرفيٌّ نصّيٌّ ثابت.
                    //      والعامُّ مُستثنًى عمدًا: خانتُه تبدأ صفرًا قبل مُهيِّئها،
                    //      فالحارسُ عليه ما زال يصطاد قراءةً سابقةً للتهيئة.
                    //      (انظر عقدَ الحقلِ في sir_types.h — الافتراضُ الغافرُ false.)
                    // (EN) 🔑 The single proof site — where the AST argument and the
                    //      symbol table meet: a read of a local/parameter EXPLICITLY
                    //      declared «نص» (nullable strings surface as Optional and do
                    //      not pass), or a constant string literal. Globals are
                    //      deliberately excluded: their slot is zero before the
                    //      initializer runs, so the guard still catches a pre-init
                    //      read. (Field contract in sir_types.h — forgiving default.)
                    // ================================================================
                    if (argResult.isConstant && argResult.type == SadTypeKind::String)
                    {
                        argOp.structurallyPresentString = true;
                    }
                    else if (auto *varExpr =
                                 dynamic_cast<const Sad::AST::VariableExpr *>(arg.get()))
                    {
                        auto *varInfo = b_.lookupVariable(varExpr->name);
                        if (varInfo && !varInfo->isGlobal &&
                            varInfo->declaredSurfaceType == SadTypeKind::String)
                            argOp.structurallyPresentString = true;
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
