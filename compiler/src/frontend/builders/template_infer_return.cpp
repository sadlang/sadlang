// ============================================================================
// template_infer_return.cpp - inferReturnType and collectFreeVars helpers
// ============================================================================
#include <string>
#include <string_view> // (AR) std::string_view في builtinReturnsToSIRKind
#include "sir_builder.h"
#include "builders/template_builder.h"
#include "builtin_registry.h" // (AR) §9: نوع إرجاع المدمجات من السجلّ المشترك (returns)
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <limits>
#include <functional>
#include <set>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            // (AR) تحويل نوع إرجاع المدمجة من مصدر الحقيقة (حقل `returns` في
            //      language-truth/builtins/*.yaml، يظهر في BuiltinMeta::returnType) إلى
            //      SadTypeKind للمترجم. مركزيٌّ وقابل للتوسّع: هنا فقط تعيش أسماء أنواع
            //      SoT النصّيّة، فإذا أُضيف `returns: عدد/نص/…` لمدمجاتٍ أخرى يُضاف تعيينُه
            //      هنا وحده. اسمٌ فارغ/غير معروف ⇒ Unknown (يُترَك للاستنتاج، لا ينهار).
            //      ملاحظة: لا تزال المطابقة نصّيّة لأنّ SoT نصّيّ (yaml)؛ لكنّها ممركزة
            //      وموثَّقة بدل نثرها. الحلّ الأمتن (مخطَّط SoT يفرض أسماء الأنواع) شريحة أكبر.
            namespace
            {
                // (AR) الرابط النصّيّ الوحيد لقيمة `returns: كائن` من مصدر الحقيقة —
                //      ثابت مسمّى مملوك للمترجم (لا يوسّع SoT اللغة). أيّ استهلاك آخر
                //      يشير إليه بدل تكرار السلسلة. «كائن» = UTF-8: D9 83 D8 A7 D8 A6 D9 86.
                // (EN) The single named binding for the SoT `returns: كائن` value —
                //      compiler-owned constant (does not expand the language SoT).
                constexpr std::string_view SOT_RETURNS_WIDGET_HANDLE = "\xd9\x83\xd8\xa7\xd8\xa6\xd9\x86";
            } // namespace

            SadTypeKind TemplateBuilder::builtinReturnsToSIRKind(std::string_view soTReturns)
            {
                if (soTReturns == SOT_RETURNS_WIDGET_HANDLE) // كائن ⇒ مقبض عنصر
                    return SadTypeKind::Pointer;
                return SadTypeKind::Unknown;
            }

            SadTypeKind TemplateBuilder::inferReturnTypeFromBody(const Sad::AST::Statement *body,
                                                            const Sad::AST::FunctionDecl *funcDecl)
            {
                if (!body)
                    return SadTypeKind::Void;

                if (!b_.hasReturnWithValue(body))
                {
                    return SadTypeKind::Void;
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) خريطة أنواع المتغيرات المحلية — تُملأ تسلسلياً أثناء المسح
                // (EN) Local variable type map — populated sequentially during scan
                // ═══════════════════════════════════════════════════════════════════
                std::unordered_map<std::string, SadTypeKind> localVarTypes;

                // (AR) الخطوة 1: تعبئة أنواع معاملات الدالة من b_.functionTable_ (المرحلة 1.7)
                // (EN) Step 1: Populate function parameter types from b_.functionTable_ (Phase 1.7)
                if (funcDecl)
                {
                    auto ftIt = b_.functionTable_.find(funcDecl->name);
                    if (ftIt != b_.functionTable_.end())
                    {
                        for (size_t i = 0; i < funcDecl->parameters.size() &&
                                           i < ftIt->second.parameters.size();
                             i++)
                        {
                            SadTypeKind paramType = ftIt->second.parameters[i].type;
                            localVarTypes[funcDecl->parameters[i].name] = paramType;
                        }
                    }
                    else
                    {
                        // (AR) إذا لم تكن في الجدول، نستخدم أنواع AST
                        // (EN) If not in table, use AST types
                        for (const auto &param : funcDecl->parameters)
                        {
                            SadTypeKind paramType = b_.astTypeToSIRType(param.type);
                            localVarTypes[param.name] = paramType;
                        }
                    }
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) الخطوة 2: دالة استنتاج نوع تعبير مع دعم المتغيرات المحلية
                // (EN) Step 2: Expression type inference with local variable support
                // ═══════════════════════════════════════════════════════════════════
                std::function<SadTypeKind(const Sad::AST::Expression *)> inferExprType;
                inferExprType = [&](const Sad::AST::Expression *expr) -> SadTypeKind
                {
                    if (!expr)
                        return SadTypeKind::Integer;

                    // (AR) حرفي — نستنتج من نوع الرمز
                    // (EN) Literal — infer from token type
                    if (auto lit = dynamic_cast<const Sad::AST::LiteralExpr *>(expr))
                    {
                        auto tt = lit->token.getType();
                        if (tt == Sad::Lexer::TokenType::NUMBER_DOUBLE)
                            return SadTypeKind::Float;
                        if (tt == Sad::Lexer::TokenType::STRING_LITERAL)
                            return SadTypeKind::String;
                        if (tt == Sad::Lexer::TokenType::LITERAL_TRUE ||
                            tt == Sad::Lexer::TokenType::LITERAL_FALSE)
                            return SadTypeKind::Boolean;
                        if (tt == Sad::Lexer::TokenType::LITERAL_NULL)
                            return SadTypeKind::Integer;
                        return SadTypeKind::Integer;
                    }

                    // (AR) متغير — نبحث في المتغيرات المحلية المُتتبَّعة
                    // (EN) Variable — look up in tracked local variable types
                    if (auto var = dynamic_cast<const Sad::AST::VariableExpr *>(expr))
                    {
                        auto it = localVarTypes.find(var->name);
                        if (it != localVarTypes.end())
                            return it->second;
                        return SadTypeKind::Integer;
                    }

                    // (AR) مصفوفة حرفية
                    // (EN) Array literal
                    if (dynamic_cast<const Sad::AST::ArrayExpr *>(expr))
                        return SadTypeKind::Array;

                    // (AR) تعبير ثنائي — نشر النوع النصي/العشري
                    // (EN) Binary expression — propagate string/float types
                    if (auto bin = dynamic_cast<const Sad::AST::BinaryExpr *>(expr))
                    {
                        SadTypeKind left = inferExprType(bin->left.get());
                        SadTypeKind right = inferExprType(bin->right.get());
                        // (AR) عمليات المقارنة تُرجع BOOL دائماً — يجب فحصها قبل نشر النوع النصي/العشري
                        // (EN) Comparison operations always return BOOL — must be checked BEFORE string/float propagation
                        switch (bin->op)
                        {
                        case Sad::Lexer::TokenType::OP_EQUAL:
                        case Sad::Lexer::TokenType::OP_NOT_EQUAL:
                        case Sad::Lexer::TokenType::OP_LESS:
                        case Sad::Lexer::TokenType::OP_GREATER:
                        case Sad::Lexer::TokenType::OP_LESS_EQUAL:
                        case Sad::Lexer::TokenType::OP_GREATER_EQUAL:
                        case Sad::Lexer::TokenType::OP_AND:
                        case Sad::Lexer::TokenType::OP_OR:
                            return SadTypeKind::Boolean;
                        default:
                            break;
                        }
                        // (AR) القسمة `/` تُنتج عشري دائماً (حسب مواصفات اللغة)
                        // (EN) Division `/` always produces float (per language spec)
                        if (bin->op == Sad::Lexer::TokenType::OP_DIVIDE)
                            return SadTypeKind::Float;
                        // (AR) إذا كان الطرفان نصاً أو أحدهما، النتيجة نص (جمع نصوص)
                        if (left == SadTypeKind::String || right == SadTypeKind::String)
                            return SadTypeKind::String;
                        if (left == SadTypeKind::Float || right == SadTypeKind::Float)
                            return SadTypeKind::Float;
                        return left;
                    }

                    // (AR) تعبير أحادي
                    // (EN) Unary expression
                    if (auto unary = dynamic_cast<const Sad::AST::UnaryExpr *>(expr))
                    {
                        if (unary->op == Sad::Lexer::TokenType::OP_NOT)
                            return SadTypeKind::Boolean;
                        return inferExprType(unary->operand.get());
                    }

                    // (AR) استدعاء دالة: نحاول استنتاج نوع الإرجاع من b_.functionTable_
                    //      مع دعم استدعاءات الأعضاء (obj.method()) حتى داخل اللامدا.
                    // (EN) Function call: infer return type from b_.functionTable_,
                    //      with support for member calls (obj.method()) even inside lambdas.
                    // (AR) §9: نداء طريقة (MethodCallExpr) — معدّل واجهة انسيابيّ على عنصرٍ
                    //      يُستنتَج Pointer (مثل عمود().ابن(x) أو س.لون("أحمر")) يُعيد المقبض
                    //      نفسه ⇒ Pointer (نظير call_method_dispatch.cpp "RETURN THE WIDGET").
                    //      شرطُ «الكائن==Pointer» هو نظير حارس className.empty() في الخافض:
                    //      المقبض الواجهيّ لا صنفَ مستخدمٍ له فيُعامَل كمعدّل.
                    //      ⚠ قيدٌ معروف (نادر): مثيلُ صنفٍ مستخدمٍ يُستنتَج كائنُه Pointer
                    //        وله طريقةٌ مسجَّلة بنوع إرجاعٍ آخر (س.مساحة()⇒Float) قد يُستنتَج
                    //        هنا Pointer خطأً؛ الإصلاح الكامل يحتاج اشتقاق className للكائن
                    //        والبحثَ في جدول الطرق (غير متاحٍ في هذا المسار). مقبولٌ لأنّ
                    //        دوال UI العائدة بمقابض هي الغالب الساحق في هذا الاستنتاج.
                    if (auto mcall = dynamic_cast<const Sad::AST::MethodCallExpr *>(expr))
                    {
                        if (inferExprType(mcall->object.get()) == SadTypeKind::Pointer)
                            return SadTypeKind::Pointer;
                    }

                    if (auto call = dynamic_cast<const Sad::AST::CallExpr *>(expr))
                    {
                        if (auto varExpr = dynamic_cast<const Sad::AST::VariableExpr *>(call->callee.get()))
                        {
                            auto it = b_.functionTable_.find(varExpr->name);
                            if (it != b_.functionTable_.end())
                            {
                                return it->second.returnType;
                            }
                            // (AR) §9 الجذر2: مدمجة معروفة (ليست دالة مستخدم) ⇒ نوع إرجاعها
                            //      من السجلّ المشترك (حقل returns في language-truth). «كائن»
                            //      = مقبض عنصر واجهة ⇒ Pointer (نظير نوع()=«كائن»). هكذا
                            //      دالةٌ تُرجِع عمود()/زر() تُستنتَج Pointer لا Integer.
                            if (const auto *meta = Sad::Builtins::findBuiltinMeta(varExpr->name))
                            {
                                SadTypeKind k = builtinReturnsToSIRKind(meta->returnType);
                                if (k != SadTypeKind::Unknown)
                                    return k;
                            }
                        }

                        if (auto memberCallee = dynamic_cast<const Sad::AST::MemberExpr *>(call->callee.get()))
                        {
                            std::string className;

                            if (dynamic_cast<const Sad::AST::ThisExpr *>(memberCallee->object.get()))
                            {
                                className = b_.currentClassName_;
                            }
                            else if (auto varObj = dynamic_cast<const Sad::AST::VariableExpr *>(memberCallee->object.get()))
                            {
                                auto ciIt = b_.classInstanceTypes_.find(varObj->name);
                                if (ciIt != b_.classInstanceTypes_.end())
                                {
                                    className = ciIt->second;
                                }
                            }

                            if (!className.empty())
                            {
                                std::string searchClass = className;
                                while (!searchClass.empty())
                                {
                                    std::string fullMethodName = searchClass + "." + memberCallee->member;
                                    auto fit = b_.functionTable_.find(fullMethodName);
                                    if (fit != b_.functionTable_.end())
                                    {
                                        return fit->second.returnType;
                                    }

                                    if (!b_.module_)
                                    {
                                        break;
                                    }
                                    auto sirClass = b_.module_->getClass(searchClass);
                                    if (!sirClass || sirClass->parentClass.empty())
                                    {
                                        break;
                                    }
                                    searchClass = sirClass->parentClass;
                                }
                            }

                            // (AR) §9 (احتياط): سلاسل المعدّلات الفعليّة تُبنى MethodCallExpr
                            //      (يعالجها الفرع أعلاه)، لا CallExpr بـcallee=MemberExpr. هذا
                            //      الفرع يغطّي مسار CallExpr+MemberExpr النادر إن وُجد: معدّل
                            //      واجهة على عنصرٍ (كائنُه Pointer) يُعيد المقبض ⇒ Pointer،
                            //      مطابقةً لشرط التوليد (call_method_dispatch.cpp:523 "RETURN
                            //      THE WIDGET"). يبقى متّسقًا مع التوليد إن التُقط يومًا.
                            if (className.empty() &&
                                inferExprType(memberCallee->object.get()) == SadTypeKind::Pointer)
                            {
                                return SadTypeKind::Pointer;
                            }

                            // (AR) احتياط: إذا كان نوع الكائن غير معروف، نبحث عن أي دالة باسم "*.member"
                            //      وعند توحّد نوع الإرجاع نستخدمه بدلاً من الافتراضي Integer.
                            // (EN) Fallback: if object type is unknown, look for "*.member" methods and
                            //      use the return type when all matches agree.
                            bool foundAny = false;
                            bool conflict = false;
                            SadTypeKind unifiedType = SadTypeKind::Integer;
                            std::string suffix = "." + memberCallee->member;
                            for (const auto &entry : b_.functionTable_)
                            {
                                const auto &name = entry.first;
                                if (name.size() >= suffix.size() &&
                                    name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
                                {
                                    if (!foundAny)
                                    {
                                        unifiedType = entry.second.returnType;
                                        foundAny = true;
                                    }
                                    else if (unifiedType != entry.second.returnType)
                                    {
                                        conflict = true;
                                        break;
                                    }
                                }
                            }

                            if (foundAny && !conflict)
                            {
                                return unifiedType;
                            }
                        }

                        return SadTypeKind::Integer;
                    }

                    // (AR) تعبير ثلاثي
                    // (EN) Ternary expression
                    if (auto ternary = dynamic_cast<const Sad::AST::TernaryExpr *>(expr))
                    {
                        SadTypeKind trueType = inferExprType(ternary->trueExpr.get());
                        SadTypeKind falseType = inferExprType(ternary->falseExpr.get());
                        if (trueType == SadTypeKind::String || falseType == SadTypeKind::String)
                            return SadTypeKind::String;
                        if (trueType == SadTypeKind::Float || falseType == SadTypeKind::Float)
                            return SadTypeKind::Float;
                        return trueType;
                    }

                    // (AR) إسناد — نوع القيمة المُسندة
                    // (EN) Assignment — type of assigned value
                    if (auto assign = dynamic_cast<const Sad::AST::AssignExpr *>(expr))
                    {
                        return inferExprType(assign->value.get());
                    }

                    // (AR) وصول لحقل عبر هذا.حقل — نبحث عن نوع الحقل في b_.module_->getClass
                    // (EN) Member access via this.field — look up field type in b_.module_->getClass
                    // (AR) ملاحظة: AST يحتوي على نوعين: MemberExpr (expressions.h) و MemberAccessExpr (class_nodes.h)
                    // (EN) Note: AST has two types: MemberExpr (expressions.h) and MemberAccessExpr (class_nodes.h)
                    if (auto memberExpr = dynamic_cast<const Sad::AST::MemberExpr *>(expr))
                    {
                        if (dynamic_cast<const Sad::AST::ThisExpr *>(memberExpr->object.get()))
                        {
                            if (!b_.currentClassName_.empty() && b_.module_)
                            {
                                auto sirClass = b_.module_->getClass(b_.currentClassName_);
                                if (sirClass)
                                {
                                    auto fieldIt = sirClass->fields_.find(memberExpr->member);
                                    if (fieldIt != sirClass->fields_.end())
                                    {
                                        SadTypeKind ft = fieldIt->second;
                                        if (ft == SadTypeKind::String || ft == SadTypeKind::Array ||
                                            ft == SadTypeKind::Float || ft == SadTypeKind::Boolean)
                                            return ft;

                                        // ═══════════════════════════════════════════════════
                                        // (AR) إصلاح: إذا كان الحقل Pointer (مجهول النوع)،
                                        //      نبحث عبر paramToFieldMap_ عن المعامل المرتبط
                                        //      ثم نتحقق من نوعه في b_.functionTable_ (باني الصنف)
                                        //      Phase 1.7 حدّث نوع المعامل، وPhase 2 حفظه
                                        //      بدون هذا: ارجع هذا.حقل يُستنتج كـ Integer
                                        // (EN) Fix: If field is Pointer (unknown), infer from
                                        //      constructor param via paramToFieldMap_ lookup
                                        //      Phase 1.7 updated param type, Phase 2 preserved it
                                        // ═══════════════════════════════════════════════════
                                        if (ft == SadTypeKind::Pointer)
                                        {
                                            for (const auto &[paramName, fieldName] : sirClass->paramToFieldMap_)
                                            {
                                                if (fieldName == memberExpr->member)
                                                {
                                                    std::string ctorName = b_.currentClassName_ + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                                                    auto ctorIt = b_.functionTable_.find(ctorName);
                                                    if (ctorIt != b_.functionTable_.end())
                                                    {
                                                        for (const auto &param : ctorIt->second.parameters)
                                                        {
                                                            if (param.name == paramName)
                                                            {
                                                                if (param.type == SadTypeKind::String ||
                                                                    param.type == SadTypeKind::Array ||
                                                                    param.type == SadTypeKind::Float ||
                                                                    param.type == SadTypeKind::Boolean)
                                                                    return param.type;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (auto varObj = dynamic_cast<const Sad::AST::VariableExpr *>(memberExpr->object.get()))
                        {
                            auto ciIt = b_.classInstanceTypes_.find(varObj->name);
                            if (ciIt != b_.classInstanceTypes_.end() && b_.module_)
                            {
                                auto sirClass = b_.module_->getClass(ciIt->second);
                                if (sirClass)
                                {
                                    auto fieldIt = sirClass->fields_.find(memberExpr->member);
                                    if (fieldIt != sirClass->fields_.end())
                                    {
                                        SadTypeKind ft = fieldIt->second;
                                        if (ft == SadTypeKind::String || ft == SadTypeKind::Array ||
                                            ft == SadTypeKind::Float || ft == SadTypeKind::Boolean)
                                            return ft;

                                        // ═══════════════════════════════════════════════════
                                        // (AR) إصلاح: إذا كان الحقل Pointer (مجهول النوع)،
                                        //      نبحث عبر paramToFieldMap_ عن المعامل المرتبط
                                        //      ثم نتحقق من نوعه في b_.functionTable_ (باني الصنف)
                                        //      بدون هذا: ارجع هذا.حقل يُستنتج كـ Integer
                                        //      بدلاً من String عندما الحقل بلا مُهيئ
                                        // (EN) Fix: If field is Pointer (unknown type), try to
                                        //      infer from constructor param via paramToFieldMap_
                                        //      Without this: return this.field infers as Integer
                                        //      instead of String when field has no initializer
                                        // ═══════════════════════════════════════════════════
                                        if (ft == SadTypeKind::Pointer)
                                        {
                                            for (const auto &[paramName, fieldName] : sirClass->paramToFieldMap_)
                                            {
                                                if (fieldName == memberExpr->member)
                                                {
                                                    std::string ctorName = b_.currentClassName_ + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                                                    auto ctorIt = b_.functionTable_.find(ctorName);
                                                    if (ctorIt != b_.functionTable_.end())
                                                    {
                                                        for (const auto &param : ctorIt->second.parameters)
                                                        {
                                                            if (param.name == paramName)
                                                            {
                                                                if (param.type == SadTypeKind::String ||
                                                                    param.type == SadTypeKind::Array ||
                                                                    param.type == SadTypeKind::Float ||
                                                                    param.type == SadTypeKind::Boolean)
                                                                    return param.type;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                        // (EN) If field is Pointer (unknown), try to infer from constructor
                                        //      Find param linked to this field via paramToFieldMap_
                                        //      then check its type in b_.functionTable_ (updated in Phase 1.7)
                                        if (ft == SadTypeKind::Pointer)
                                        {
                                            // (AR) بحث عكسي: أي معامل يُقابل هذا الحقل؟
                                            // (EN) Reverse lookup: which param maps to this field?
                                            for (const auto &[paramName, fieldName] : sirClass->paramToFieldMap_)
                                            {
                                                if (fieldName == memberExpr->member)
                                                {
                                                    // (AR) وجدنا المعامل — الآن نبحث عن نوعه في الباني
                                                    // (EN) Found param — now look up its type in constructor
                                                    std::string ctorName = ciIt->second + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1"; // .باني
                                                    auto ctorIt = b_.functionTable_.find(ctorName);
                                                    if (ctorIt != b_.functionTable_.end())
                                                    {
                                                        // (AR) params[0] = self, ابحث عن المعامل بالاسم
                                                        for (const auto &param : ctorIt->second.parameters)
                                                        {
                                                            if (param.name == paramName)
                                                            {
                                                                if (param.type == SadTypeKind::String ||
                                                                    param.type == SadTypeKind::Array ||
                                                                    param.type == SadTypeKind::Float ||
                                                                    param.type == SadTypeKind::Boolean)
                                                                    return param.type;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        return SadTypeKind::Integer;
                    }
                    if (auto memberAccessExpr = dynamic_cast<const Sad::AST::MemberAccessExpr *>(expr))
                    {
                        if (dynamic_cast<const Sad::AST::ThisExpr *>(memberAccessExpr->object.get()))
                        {
                            if (!b_.currentClassName_.empty() && b_.module_)
                            {
                                auto sirClass = b_.module_->getClass(b_.currentClassName_);
                                if (sirClass)
                                {
                                    auto fieldIt = sirClass->fields_.find(memberAccessExpr->memberName);
                                    if (fieldIt != sirClass->fields_.end())
                                    {
                                        SadTypeKind ft = fieldIt->second;
                                        if (ft == SadTypeKind::String || ft == SadTypeKind::Array ||
                                            ft == SadTypeKind::Float || ft == SadTypeKind::Boolean)
                                            return ft;

                                        // (EN) Same fix as MemberExpr: Pointer → paramToFieldMap_ lookup
                                        if (ft == SadTypeKind::Pointer)
                                        {
                                            for (const auto &[paramName, fieldName] : sirClass->paramToFieldMap_)
                                            {
                                                if (fieldName == memberAccessExpr->memberName)
                                                {
                                                    std::string ctorName = b_.currentClassName_ + "." + "\xD8\xA8\xD9\x86\xD8\xA7\xD8\xA1";
                                                    auto ctorIt = b_.functionTable_.find(ctorName);
                                                    if (ctorIt != b_.functionTable_.end())
                                                    {
                                                        for (const auto &param : ctorIt->second.parameters)
                                                        {
                                                            if (param.name == paramName)
                                                            {
                                                                if (param.type == SadTypeKind::String ||
                                                                    param.type == SadTypeKind::Array ||
                                                                    param.type == SadTypeKind::Float ||
                                                                    param.type == SadTypeKind::Boolean)
                                                                    return param.type;
                                                                break;
                                                            }
                                                        }
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (auto varObj = dynamic_cast<const Sad::AST::VariableExpr *>(memberAccessExpr->object.get()))
                        {
                            auto ciIt = b_.classInstanceTypes_.find(varObj->name);
                            if (ciIt != b_.classInstanceTypes_.end() && b_.module_)
                            {
                                auto sirClass = b_.module_->getClass(ciIt->second);
                                if (sirClass)
                                {
                                    auto fieldIt = sirClass->fields_.find(memberAccessExpr->memberName);
                                    if (fieldIt != sirClass->fields_.end())
                                    {
                                        SadTypeKind ft = fieldIt->second;
                                        if (ft == SadTypeKind::String || ft == SadTypeKind::Array ||
                                            ft == SadTypeKind::Float || ft == SadTypeKind::Boolean)
                                            return ft;
                                    }
                                }
                            }
                        }
                        return SadTypeKind::Integer;
                    }

                    // (AR) إنشاء كائن جديد
                    // (EN) New object creation
                    if (dynamic_cast<const Sad::AST::NewExpr *>(expr))
                        return SadTypeKind::Struct;
                    // (AR) تعبير صف (Tuple)
                    // (EN) Tuple expression
                    if (dynamic_cast<const Sad::AST::TupleExpr *>(expr))
                        return SadTypeKind::Tuple;
                    // ================================================================
                    // (AR) [Fix #52] تعبير لامدا — يُرجع دائماً نوع Function
                    //      بدون هذا الفحص، inferReturnTypeFromBody يُعيد Integer
                    //      لدالة مثل: دالة صانع() ارجع لامدا()...نهاية نهاية
                    //      مما يمنع تتبع returnLambdaName في b_.functionTable_
                    // (EN) [Fix #52] Lambda expression — always returns Function type
                    //      Without this check, inferReturnTypeFromBody returns Integer
                    //      for functions like: function maker() return lambda()...end end
                    //      which prevents returnLambdaName tracking in b_.functionTable_
                    // ================================================================
                    if (dynamic_cast<const Sad::AST::LambdaExpr *>(expr))
                        return SadTypeKind::Function;
                    // (AR) فحص DataType من التعبير نفسه (إذا توفر)
                    // (EN) Check DataType from expression itself (if available)
                    auto dtype = expr->getTypeKind();
                    if (dtype == Sad::Types::SadTypeKind::Float)
                        return SadTypeKind::Float;
                    if (dtype == Sad::Types::SadTypeKind::Boolean)
                        return SadTypeKind::Boolean;
                    if (dtype == Sad::Types::SadTypeKind::String)
                        return SadTypeKind::String;
                    if (dtype == Sad::Types::SadTypeKind::Array)
                        return SadTypeKind::Array;
                    if (dtype == Sad::Types::SadTypeKind::Class)
                        return SadTypeKind::Struct;
                    if (dtype == Sad::Types::SadTypeKind::Map)
                        return SadTypeKind::Map;

                    return SadTypeKind::Integer;
                };

                // ═══════════════════════════════════════════════════════════════════
                // (AR) الخطوة 3: مسح تسلسلي لجسم الدالة لتعبئة أنواع المتغيرات المحلية
                // (EN) Step 3: Sequential scan of function body to populate local variable types
                // ═══════════════════════════════════════════════════════════════════
                std::function<void(const Sad::AST::Statement *)> populateVarTypes;
                populateVarTypes = [&](const Sad::AST::Statement *stmt)
                {
                    if (!stmt)
                        return;

                    // (AR) إعلان متغير — نستنتج نوع المُهيِّئ ونسجّله
                    // (EN) Variable declaration — infer initializer type and register
                    if (auto varDecl = dynamic_cast<const Sad::AST::VarDeclStmt *>(stmt))
                    {
                        if (varDecl->initializer)
                        {
                            SadTypeKind varType = inferExprType(varDecl->initializer.get());
                            localVarTypes[varDecl->name] = varType;
                        }
                        else
                        {
                            // (AR) متغير بدون مُهيِّئ — ننظر إلى نوع AST
                            SadTypeKind astType = b_.astTypeToSIRType(varDecl->type);
                            localVarTypes[varDecl->name] = astType;
                        }
                        return;
                    }

                    // (AR) كتلة جمل — مسح تسلسلي للحفاظ على ترتيب الإعلانات
                    // (EN) Block statement — sequential scan to preserve declaration order
                    if (auto block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                    {
                        for (const auto &s : block->statements)
                        {
                            populateVarTypes(s.get());
                        }
                        return;
                    }

                    // (AR) تعبير إسناد — تحديث نوع المتغير
                    // (EN) Assignment expression — update variable type
                    if (auto exprStmt = dynamic_cast<const Sad::AST::ExprStmt *>(stmt))
                    {
                        if (auto assign = dynamic_cast<const Sad::AST::AssignExpr *>(exprStmt->expression.get()))
                        {
                            SadTypeKind newType = inferExprType(assign->value.get());
                            localVarTypes[assign->name] = newType;
                        }
                        return;
                    }

                    // (AR) فروع شرطية — مسح كلا الفرعين
                    // (EN) Conditional branches — scan both branches
                    if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                    {
                        populateVarTypes(ifStmt->thenBranch.get());
                        if (ifStmt->elseBranch)
                            populateVarTypes(ifStmt->elseBranch.get());
                        return;
                    }

                    // (AR) حلقات
                    // (EN) Loops
                    if (auto whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                    {
                        populateVarTypes(whileStmt->body.get());
                        return;
                    }
                    if (auto forRange = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                    {
                        populateVarTypes(forRange->body.get());
                        return;
                    }
                    // (AR) جملة match — مسح أجسام الحالات
                    // (EN) Match statement — scan case bodies
                    if (auto matchStmt = dynamic_cast<const Sad::AST::MatchStmt *>(stmt))
                    {
                        for (const auto &caseClause : matchStmt->cases)
                        {
                            // (AR) ISSUE-076/082/084 (ب″): سجّل متغيرات ربط نمط حالة ADT بنوعها
                            //      المُستنتَج (من adtEnumTable_ إن سُجِّل من موقع إنشاء، وإلّا Any).
                            //      حاسمٌ لاستنتاج نوع إرجاع دالةٍ تُرجع رابطًا (`ارجع س`): حمولةٌ
                            //      مجهولة النوع سكونيًّا (إحالة أماميّة/تعارُض) ⇒ Any ⇒ يُبقيها
                            //      الإرجاع قيمةً موسومة يفكّها نص()/اطبع زمنَ التشغيل بدل طمسها
                            //      صحيحًا/عشريًّا (ISSUE-084: إرجاع حمولة عشريّة عبر حدود الدوال).
                            // (EN) ISSUE-076/082/084 (ب″): register ADT variant-pattern binding
                            //      variables with their inferred type (from adtEnumTable_ if
                            //      registered at a construction site, else Any). Crucial for
                            //      inferring the return type of a function returning a binding
                            //      (`return s`): a statically-unknown payload (forward-ref/conflict)
                            //      ⇒ Any ⇒ the return keeps it a tagged value that نص()/print decode
                            //      at runtime instead of erasing it to int/float (ISSUE-084: returning
                            //      a float payload across a function boundary).
                            if (auto *enumPat = dynamic_cast<const Sad::AST::EnumVariantPattern *>(caseClause.pattern.get()))
                            {
                                const ADTEnumInfo *adt = nullptr;
                                auto adtIt = b_.adtEnumTable_.find(enumPat->enumName);
                                if (adtIt != b_.adtEnumTable_.end())
                                    adt = &adtIt->second;
                                for (size_t fi = 0; fi < enumPat->fieldPatterns.size(); ++fi)
                                {
                                    if (auto *vp = dynamic_cast<const Sad::AST::VariablePattern *>(enumPat->fieldPatterns[fi].get()))
                                    {
                                        SadTypeKind ft = SadTypeKind::Any;
                                        if (adt)
                                        {
                                            for (const auto &v : adt->variants)
                                            {
                                                if (v.name == enumPat->variantName)
                                                {
                                                    SadTypeKind reg = v.fieldTypeAt(fi);
                                                    if (reg != SadTypeKind::Unknown)
                                                        ft = reg;
                                                    break;
                                                }
                                            }
                                        }
                                        localVarTypes[vp->name] = ft;
                                    }
                                }
                            }
                            for (const auto &bodyStmt : caseClause.body)
                            {
                                populateVarTypes(bodyStmt.get());
                            }
                        }
                        return;
                    }
                    // (AR) جملة حالة/switch — مسح أجسام الفروع والافتراضيّ (شقيق collectReturnTypes)
                    // (EN) Switch statement — scan case bodies + default (sibling of collectReturnTypes)
                    if (auto switchStmt = dynamic_cast<const Sad::AST::SwitchStmt *>(stmt))
                    {
                        for (const auto &caseBranch : switchStmt->cases)
                            populateVarTypes(caseBranch.body.get());
                        if (switchStmt->defaultCase)
                            populateVarTypes(switchStmt->defaultCase.get());
                        return;
                    }
                    // (AR) جملة try-catch — مسح كتل المحاولة والالتقاط
                    // (EN) Try-catch statement — scan try and catch blocks
                    if (auto tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                    {
                        populateVarTypes(tryStmt->tryBlock.get());
                        for (const auto &catchClause : tryStmt->catchClauses)
                        {
                            populateVarTypes(catchClause.body.get());
                        }
                        if (tryStmt->finallyBlock)
                            populateVarTypes(tryStmt->finallyBlock.get());
                        return;
                    }
                };

                populateVarTypes(body);

                // ═══════════════════════════════════════════════════════════════════
                // (AR) الخطوة 4: جمع أنواع جميع عبارات الإرجاع وتوحيدها
                // (EN) Step 4: Collect types from ALL return statements and unify them
                // ═══════════════════════════════════════════════════════════════════
                std::function<void(const Sad::AST::Statement *, std::vector<SadTypeKind> &)> collectReturnTypes;
                collectReturnTypes = [&](const Sad::AST::Statement *stmt, std::vector<SadTypeKind> &types)
                {
                    if (!stmt)
                        return;
                    if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt *>(stmt))
                    {
                        if (ret->value)
                        {
                            types.push_back(inferExprType(ret->value.get()));
                        }
                        return;
                    }
                    if (auto block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                    {
                        for (auto &s : block->statements)
                            collectReturnTypes(s.get(), types);
                    }
                    if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                    {
                        collectReturnTypes(ifStmt->thenBranch.get(), types);
                        if (ifStmt->elseBranch)
                            collectReturnTypes(ifStmt->elseBranch.get(), types);
                    }
                    if (auto whileStmt = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                    {
                        collectReturnTypes(whileStmt->body.get(), types);
                    }
                    if (auto forRange = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                    {
                        collectReturnTypes(forRange->body.get(), types);
                    }
                    // (AR) جملة match — نبحث عن return في جميع أجسام الحالات
                    // (EN) Match statement — search for return in all case bodies
                    if (auto matchStmt = dynamic_cast<const Sad::AST::MatchStmt *>(stmt))
                    {
                        for (const auto &caseClause : matchStmt->cases)
                        {
                            for (const auto &bodyStmt : caseClause.body)
                            {
                                collectReturnTypes(bodyStmt.get(), types);
                            }
                        }
                    }
                    // (AR) جملة حالة/switch — نجمع أنواع الإرجاع من كلّ فرع والافتراضيّ (ISSUE-055)
                    // (EN) Switch statement — collect return types from all cases + default (ISSUE-055)
                    if (auto switchStmt = dynamic_cast<const Sad::AST::SwitchStmt *>(stmt))
                    {
                        for (const auto &caseBranch : switchStmt->cases)
                        {
                            collectReturnTypes(caseBranch.body.get(), types);
                        }
                        if (switchStmt->defaultCase)
                            collectReturnTypes(switchStmt->defaultCase.get(), types);
                    }
                    // (AR) جملة try-catch — نبحث في كتل المحاولة والالتقاط
                    // (EN) Try-catch statement — search in try and catch blocks
                    if (auto tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                    {
                        collectReturnTypes(tryStmt->tryBlock.get(), types);
                        for (const auto &catchClause : tryStmt->catchClauses)
                        {
                            collectReturnTypes(catchClause.body.get(), types);
                        }
                        if (tryStmt->finallyBlock)
                            collectReturnTypes(tryStmt->finallyBlock.get(), types);
                    }
                };

                std::vector<SadTypeKind> returnTypes;
                collectReturnTypes(body, returnTypes);

                if (returnTypes.empty())
                    return SadTypeKind::Integer;

                // (AR) توحيد الأنواع: STRING يسيطر، I64+F64→F64
                // (EN) Unify types: STRING dominates, I64+F64→F64
                SadTypeKind unified = returnTypes[0];
                for (size_t i = 1; i < returnTypes.size(); ++i)
                {
                    if (unified == returnTypes[i])
                        continue;
                    // (AR) ISSUE-076/084 (ب″): Any يسيطر — أيّ فرع إرجاعٍ ديناميّ (حمولة ADT
                    //      مجهولة النوع) يجعل نوع الإرجاع Any، فتعبر القيمة موسومةً ويفكّها
                    //      المستهلك زمنَ التشغيل بدل طمسها إلى صحيح/عشريّ في فرعٍ آخر.
                    // (EN) ISSUE-076/084 (ب″): Any dominates — any dynamic return arm (a
                    //      statically-unknown ADT payload) makes the return type Any, so the value
                    //      crosses tagged and the consumer decodes it at runtime instead of being
                    //      erased to int/float by another arm.
                    if (unified == SadTypeKind::Any || returnTypes[i] == SadTypeKind::Any)
                    {
                        unified = SadTypeKind::Any;
                    }
                    else if ((unified == SadTypeKind::Integer && returnTypes[i] == SadTypeKind::Float) ||
                             (unified == SadTypeKind::Float && returnTypes[i] == SadTypeKind::Integer))
                    {
                        unified = SadTypeKind::Float;
                    }
                    else if (unified == SadTypeKind::String || returnTypes[i] == SadTypeKind::String)
                    {
                        unified = SadTypeKind::String;
                    }
                }

                return unified;
            }

            // ============================================================================
            // b_.enterScope - دخول نطاق جديد
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:587
            // التوقيع / Signature: void b_.enterScope();
            //
            // المتغيرات المستخدمة / Used variables:
            // - b_.currentScopeLevel_: sir_builder.h:599 (int)
            // - scopes_: sir_builder.h:630 (std::vector<std::vector<VariableInfo>>)
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
