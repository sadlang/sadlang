// ============================================================================
// template_infer_return.cpp - inferReturnType and collectFreeVars helpers
// ============================================================================
#include <string>
#include <string_view> // (AR) std::string_view في builtinReturnsToSIRKind
#include "sir_builder.h"
#include "builders/template_builder.h"
#include "builders/type_method_return.h" // (AR) نوع عائد طرق الأنواع — مرجع واحد
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
#include <unordered_set> // (AR) مزورات صعود الوراثة — قطع الدورات الفاسدة

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

                // (AR) أسماءُ الأنواع القانونيّةُ من حقل `word:` في language-truth/types.yaml
                //      (لا من typeof_ar، ولا من SimpleTypeNode::stringToKind: تلك تعيّن
                //      «كائن» ⇒ Class بينما المترجمُ يريده Pointer، وتُصدِر «قاموس» للخريطة
                //      خلافًا لـSoT). لذا يبقى الجدولُ مملوكًا للمترجم هنا وحدَه.
                // (EN) Canonical type words from `word:` in language-truth/types.yaml — NOT
                //      typeof_ar and NOT SimpleTypeNode::stringToKind (which maps «كائن» to
                //      Class where the compiler needs Pointer, and emits «قاموس» for Map
                //      against SoT). The table stays compiler-owned here.
                constexpr std::string_view SOT_RETURNS_INTEGER = "\xd8\xb1\xd9\x82\xd9\x85";                     // رقم
                constexpr std::string_view SOT_RETURNS_FLOAT = "\xd8\xb9\xd8\xb4\xd8\xb1\xd9\x8a";               // عشري
                constexpr std::string_view SOT_RETURNS_STRING = "\xd9\x86\xd8\xb5";                             // نص
                constexpr std::string_view SOT_RETURNS_BOOLEAN = "\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd9\x8a";     // منطقي

                // (AR) قياسيٌّ **غيرُ نصّيّ**: قيمةٌ تسعُ حمولةَ %SadDyn بلا كومةٍ ولا مؤشّر.
                //      النصُّ مستثنًى عمدًا لأنّه الطرفُ الآخرُ في موازنةِ توحيدِ المُرجَع؛
                //      والمصفوفةُ/الخريطةُ/الكائنُ خارجَها لأنّ حمولتَها مؤشّرٌ مُدارٌ لا يزال
                //      يفقدُ وسمَه في خانةِ المتغيّر (انظر ز.٤٢ في هذا الملفّ).
                // (EN) A NON-string scalar: fits a %SadDyn payload with no heap pointer. String
                //      is deliberately excluded (it is the other side of the unification), and
                //      array/map/object are excluded because their managed-pointer payload still
                //      loses its tag in a variable slot (see ز.٤٢ in this file).
                inline bool isScalarKind(SadTypeKind kind)
                {
                    return kind == SadTypeKind::Integer || kind == SadTypeKind::Float ||
                           kind == SadTypeKind::Boolean || kind == SadTypeKind::Byte ||
                           kind == SadTypeKind::UInt64;
                }
            } // namespace

            SadTypeKind TemplateBuilder::builtinReturnsToSIRKind(std::string_view soTReturns)
            {
                if (soTReturns == SOT_RETURNS_WIDGET_HANDLE) // كائن ⇒ مقبض عنصر
                    return SadTypeKind::Pointer;
                if (soTReturns == SOT_RETURNS_STRING)
                    return SadTypeKind::String;
                if (soTReturns == SOT_RETURNS_INTEGER)
                    return SadTypeKind::Integer;
                if (soTReturns == SOT_RETURNS_FLOAT)
                    return SadTypeKind::Float;
                if (soTReturns == SOT_RETURNS_BOOLEAN)
                    return SadTypeKind::Boolean;
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

                // (AR) نوعُ **عنصرِ** كلِّ حاويةٍ محلّيّة — مستقلٌّ عن نوعِ الحاويةِ نفسِها.
                //      بدونه لا يملك استنتاجُ الإرجاعِ جوابًا عن `ز[0]` فيسقط إلى
                //      الافتراضِ «رقم». راجِعْ ذراعَ IndexExpr أدناه.
                // (EN) The ELEMENT type of each local container — distinct from the
                //      container's own type. Without it return inference has no answer for
                //      `pair[0]` and falls through to the Integer default. See the IndexExpr
                //      arm below.
                std::unordered_map<std::string, SadTypeKind> localElementTypes;

                // (AR) صنفُ كلِّ مثيلٍ محلّيٍّ («متغير م = محيّي()») — تقرؤه ذراعُ
                //      MethodCallExpr لتستنتج نوعَ إرجاعِ «م.طريقة()» من جدولِ الطرائق؛
                //      بدونه سقطت طريقةٌ نصّيّةٌ إلى «رقم» فطُبع مؤشّرٌ (قِيس: بذرة 110).
                // (EN) The class of each local instance («متغير م = محيّي()») — read by
                //      the MethodCallExpr arm to resolve «م.method()» return types from
                //      the method table; without it a string method fell to Integer and
                //      printed a raw pointer (measured: seed 110).
                std::unordered_map<std::string, std::string> localClassNames;

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

                        // (AR) ليس محلّيًّا ⇒ قد يكون ثابتَ وحدةٍ أو متغيّرًا عامًّا. كان
                        //      المسارُ يفترضه «رقمًا»، فدالّةٌ تُرجع «بادئة + النص» حيث
                        //      «بادئة» ثابتُ وحدةٍ نصّيٌّ تُستنتَج رقمًا ⇒ يُطبع مؤشّرُ
                        //      النصّ عددًا. نسأل جدولَ المتغيّرات ثمّ عوالمَ الوحدة.
                        // (EN) Not local ⇒ may be a module constant or a global. This used
                        //      to default to Integer, so a function returning
                        //      "prefix + text" (prefix being a string module constant) was
                        //      inferred Integer and printed the string pointer as a number.
                        if (VariableInfo *known = b_.lookupVariable(var->name))
                        {
                            if (known->type != SadTypeKind::Void &&
                                known->type != SadTypeKind::Unknown)
                                return known->type;
                        }
                        if (b_.module_)
                        {
                            if (auto g = b_.module_->getGlobalVariable(var->name))
                            {
                                if (g->type != SadTypeKind::Void &&
                                    g->type != SadTypeKind::Unknown)
                                    return g->type;
                            }
                        }
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
                        // (AR) المعينُ الواحد لنوعِ ناتجِ `/` — انظر التعليقَ عند
                        //      `divisionResultKind` في template_builder.h.
                        // (EN) Single source for the kind of `/` — see divisionResultKind.
                        if (bin->op == Sad::Lexer::TokenType::OP_DIVIDE)
                            return divisionResultKind(left, right);
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
                        SadTypeKind objectKind = inferExprType(mcall->object.get());
                        if (objectKind == SadTypeKind::Pointer)
                            return SadTypeKind::Pointer;

                        // (AR) طريقةُ نوعٍ مدمجة (نصّ/مصفوفة/خريطة): نوعُ العائد من المرجع
                        //      الواحد في type_method_return.cpp. بدونه كان العائدُ يسقط إلى
                        //      الافتراضيّ «رقم»، فتُطوى المقارنةُ `نص == نص` إلى «خطأ» ثابتة
                        //      (اختصارُ الخلط الصارم في expression_binary_op.cpp) — فشلَ بناءٍ
                        //      داخلَ شرطِ «إذا»، ونتيجةً خاطئةً صامتةً خارجَه.
                        // (EN) Built-in type method: take the return type from the single
                        //      reference. Without it the result defaulted to Integer, making the
                        //      strict mixed-type shortcut fold a string==string comparison to a
                        //      constant false.
                        TypeMethodReturn tmr = typeMethodReturnKind(objectKind, mcall->methodName);
                        if (tmr.known)
                            return tmr.kind;

                        // (AR) طريقةُ صنفِ مستخدمٍ — كانت الذراعُ تقف هنا فتسقط إلى «رقم»
                        //      الافتراضيِّ حتى لطريقةٍ نصّيّةٍ توقيعُها في الجدول (المرحلة 1.3)،
                        //      فدالّةٌ حرّةٌ تُرجع «مصنع.اصنع()» طُبع ناتجُها مؤشّرًا رقمًا
                        //      (قِيس: عيب e2 من المراجعة العدائية). الاسمُ يُشتقُّ من «هذا»
                        //      أو سجلِّ المثيلاتِ أو اسمِ الصنفِ ذاتِه (نداءٌ ساكن)، ثم
                        //      يُصعَدُ في سلسلةِ الوراثةِ — نظيرُ ذراعِ CallExpr+MemberExpr
                        //      أدناه حرفًا بحرف.
                        // (EN) User-class method — this arm used to stop here and fall to
                        //      the Integer default even when the signature is in the table
                        //      (Phase 1.3), so a free function returning «مصنع.اصنع()»
                        //      printed its result as a raw pointer number (measured: defect
                        //      e2, adversarial review). Derive the class from «هذا», the
                        //      instance registry, or the name itself (static call), then
                        //      walk the inheritance chain — the exact peer of the
                        //      CallExpr+MemberExpr arm below.
                        std::string mcClassName;
                        if (dynamic_cast<const Sad::AST::ThisExpr *>(mcall->object.get()))
                        {
                            mcClassName = b_.currentClassName_;
                        }
                        else if (auto varObj = dynamic_cast<const Sad::AST::VariableExpr *>(mcall->object.get()))
                        {
                            auto lcIt = localClassNames.find(varObj->name);
                            auto ciIt = b_.classInstanceTypes_.find(varObj->name);
                            if (lcIt != localClassNames.end())
                            {
                                mcClassName = lcIt->second;
                            }
                            else if (ciIt != b_.classInstanceTypes_.end())
                            {
                                mcClassName = ciIt->second;
                            }
                            else if (b_.functionTable_.count(varObj->name + "." + mcall->methodName) > 0)
                            {
                                mcClassName = varObj->name;
                            }
                        }
                        // (AR) مجموعةُ زيارةٍ تقطع دورةَ وراثةٍ فاسدةٍ («أ يرث ب يرث أ»)
                        //      — عطبُ المصدرِ لا يجوز أن يصيرَ تعليقَ مصرِّفٍ (نظير
                        //      findMethodInHierarchy — رصد مراجعة الجودة).
                        // (EN) A visited set breaks an invalid inheritance cycle — a
                        //      source defect must never hang the compiler (peer of
                        //      findMethodInHierarchy — quality-review finding).
                        std::unordered_set<std::string> visitedOwners;
                        while (!mcClassName.empty() && visitedOwners.insert(mcClassName).second)
                        {
                            auto fit = b_.functionTable_.find(mcClassName + "." + mcall->methodName);
                            if (fit != b_.functionTable_.end())
                            {
                                return fit->second.returnType;
                            }
                            if (!b_.module_)
                            {
                                break;
                            }
                            auto ownerClass = b_.module_->getClass(mcClassName);
                            if (!ownerClass || ownerClass->parentClass.empty())
                            {
                                break;
                            }
                            mcClassName = ownerClass->parentClass;
                        }
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
                                // (AR) مزورات تقطع دورة الوراثة الفاسدة — الفخ نفسه
                                //      المسدود في ذراع MethodCallExpr أعلاه.
                                // (EN) Visited set breaks an invalid inheritance cycle —
                                //      same trap sealed in the MethodCallExpr arm above.
                                std::unordered_set<std::string> visitedSearch;
                                while (!searchClass.empty() && visitedSearch.insert(searchClass).second)
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
                                                    std::string ctorName = constructorNameFor(b_.currentClassName_);
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
                                                    std::string ctorName = constructorNameFor(b_.currentClassName_);
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
                                                    std::string ctorName = constructorNameFor(ciIt->second);
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
                                                    std::string ctorName = constructorNameFor(b_.currentClassName_);
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
                    // ================================================================
                    // (AR) وصولُ فهرسٍ — كان بلا ذراعٍ أصلًا، فيسقط إلى الافتراضِ «رقم».
                    //
                    //      وثمنُه ليس نوعًا مغلوطًا في جدولٍ فحسب: دالّةٌ تُرجع `الزوج[0]`
                    //      من مصفوفةٍ مختلطة تُصدَّر فعلًا بـ%SadDyn (وهو الصواب)، بينما
                    //      يُسجَّل نوعُها «رقم». فمتى قُورنت قيمتُها بنصّ اشتغل اختصارُ
                    //      «نصّ في مواجهة عددٍ ⇒ ثابتٌ خطأ» في بناءِ العملياتِ الثنائيّة
                    //      **قبلَ إصدارِ أيِّ تعليمةِ مقارنة**، فلا يبقى في IR أثرٌ يُقاس:
                    //      يُطبَع «خطأ» من ثابتٍ لا من حكم.
                    //      مقيسًا: `حلل(ولد(س)) == س` في مكتبةِ جيسون كانت «خطأ» مصرَّفةً
                    //      و«صحيح» مفسَّرةً — والدورةُ المغلقةُ هي خاصّيّةُ المكتبةِ الحاكمة.
                    //
                    //      والجوابُ هنا يتبع التمثيلَ لا يخمّنه: نوعُ العنصرِ متى عُرِف،
                    //      و«أي» متى لم يُعرَف — و«أي» هي بعينها ما يُصدِره الباني لعنصرٍ
                    //      مختلطٍ، فيتطابق الجدولُ مع الكود بدل أن يتناقضا.
                    // (EN) Index access — had no arm at all, so it fell to the Integer default.
                    //
                    //      The cost is not merely a wrong type in a table: a function returning
                    //      `pair[0]` from a heterogeneous array really is emitted as %SadDyn
                    //      (correctly), while its type is recorded as Integer. So when its value
                    //      is compared against a string, the "string vs number ⇒ constant false"
                    //      short-circuit in the binary-op builder fires **before any comparison
                    //      instruction is emitted**, leaving no trace in the IR to measure:
                    //      «false» is printed from a constant, not from a judgement.
                    //      Measured: `parse(generate(s)) == s` in the JSON library was false
                    //      compiled and true interpreted — and that round trip is the library's
                    //      governing property.
                    //
                    //      The answer here follows the representation rather than guessing it:
                    //      the element type when it is known, and Any when it is not — and Any is
                    //      exactly what the builder emits for a heterogeneous element, so the
                    //      table agrees with the code instead of contradicting it.
                    // ================================================================
                    if (auto idx = dynamic_cast<const Sad::AST::IndexExpr *>(expr))
                    {
                        // (AR) الدلالةُ في المعينِ الواحدِ `bracketReadResultKind`؛ هذا
                        //      المسارُ وحدَه يملك سجلَّ أنواعِ العناصرِ المحلّيَّ فيمرّره.
                        // (EN) Semantics live in the single source bracketReadResultKind;
                        //      only this path owns the local element-type registry.
                        SadTypeKind knownElem = SadTypeKind::Void;
                        if (auto obj = dynamic_cast<const Sad::AST::VariableExpr *>(idx->object.get()))
                        {
                            auto elemIt = localElementTypes.find(obj->name);
                            if (elemIt != localElementTypes.end())
                                knownElem = elemIt->second;
                        }
                        return TemplateBuilder::bracketReadResultKind(
                            inferExprType(idx->object.get()), knownElem);
                    }

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

                            // (AR) مثيلُ صنفٍ محلّيٌّ — بالنداء «صنف(…)» أو بـ«جديد»:
                            //      سجّل صنفَه لذراعِ MethodCallExpr (الأصدافُ 1.25 تجعل
                            //      getClass يرى حتى الصنفَ المعرَّفَ لاحقًا).
                            // (EN) Local class instance — via call «Class(…)» or «new»:
                            //      record its class for the MethodCallExpr arm (the 1.25
                            //      shells make getClass see even later-declared classes).
                            if (auto ctorCall = dynamic_cast<const Sad::AST::CallExpr *>(
                                    varDecl->initializer.get()))
                            {
                                if (auto ctorName = dynamic_cast<const Sad::AST::VariableExpr *>(
                                        ctorCall->callee.get()))
                                {
                                    if (b_.module_ && b_.module_->getClass(ctorName->name))
                                        localClassNames[varDecl->name] = ctorName->name;
                                }
                            }
                            else if (auto newE = dynamic_cast<const Sad::AST::NewExpr *>(
                                         varDecl->initializer.get()))
                            {
                                localClassNames[varDecl->name] = newE->className;
                            }

                            // (AR) تسجيلُ نوعِ العنصرِ للحاوياتِ المحلّيّة:
                            //        · حرفيّةُ مصفوفةٍ ⇒ نوعُ عناصرِها إن تجانست، و«أي» إن اختلفت.
                            //        · نداءُ دالّةٍ ⇒ `returnElementType` المسجَّلُ لها إن وُجد.
                            //      وهذا هو ما تقرؤه ذراعُ IndexExpr أعلاه.
                            // (EN) Record the element type of local containers:
                            //        - array literal ⇒ its elements' type if homogeneous, Any otherwise.
                            //        - function call ⇒ its recorded `returnElementType`, if any.
                            //      This is what the IndexExpr arm above reads.
                            if (auto arrLit =
                                    dynamic_cast<const Sad::AST::ArrayExpr *>(varDecl->initializer.get()))
                            {
                                SadTypeKind elemType = SadTypeKind::Void;
                                bool homogeneous = true;
                                for (const auto &el : arrLit->elements)
                                {
                                    SadTypeKind t = inferExprType(el.get());
                                    if (elemType == SadTypeKind::Void)
                                        elemType = t;
                                    else if (t != elemType)
                                        homogeneous = false;
                                }
                                if (!arrLit->elements.empty())
                                    localElementTypes[varDecl->name] =
                                        homogeneous ? elemType : SadTypeKind::Any;
                            }
                            else if (auto callExpr = dynamic_cast<const Sad::AST::CallExpr *>(
                                         varDecl->initializer.get()))
                            {
                                if (auto callee = dynamic_cast<const Sad::AST::VariableExpr *>(
                                        callExpr->callee.get()))
                                {
                                    auto fnIt = b_.functionTable_.find(callee->name);
                                    if (fnIt != b_.functionTable_.end() &&
                                        fnIt->second.returnElementType != SadTypeKind::Void &&
                                        fnIt->second.returnElementType != SadTypeKind::Unknown)
                                    {
                                        localElementTypes[varDecl->name] =
                                            fnIt->second.returnElementType;
                                    }
                                }
                            }
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
                //
                //      ⚠️ ز.٤٢ — عيبٌ **قائمٌ معروفٌ** هنا، وتراجعٌ موثَّقٌ عن إصلاحه:
                //      الجدولُ يعرف ثلاثَ حالاتٍ فقط، وأيُّ زوجٍ خارجَها يسقط من كلِّ
                //      الفروعِ فيبقى **نوعُ أوّلِ `ارجع`** — كذبةٌ صامتة. فـ
                //      `إذا (…) ارجع "نص" نهاية ارجع [١، ٢]` تُقرأ نصًّا في الذراعين،
                //      ويخالف `نوع()` المفسّرَ.
                //
                //      جُرِّب توحيدُ المتباينِ إلى `Any` (الجوابُ الصادق، ونظيرُ المسارِ
                //      القائم ISSUE-076/084) و**أُعيد**: الأماميّةُ ليست جاهزةً لاستهلاكِ
                //      `Any` في الفهرسة. المُصغَّرُ المُثبِت:
                //
                //          دالة ابحث(س)
                //              إذا (س == 1)  ارجع [1، 2]  نهاية
                //              ارجع لاشيء
                //          نهاية
                //          … متغير ن = ابحث(1)؛ اطبع(ن[0])
                //
                //      المفسّر يطبع `1`؛ ومع التوحيدِ إلى Any يخرج المصرَّفُ **صامتًا
                //      برمزِ ٠ بلا طباعةٍ ولا خطأ**. والسببُ أنّ `لاشيء` تُستنتَج
                //      `Integer` (أعلاه)، فيصير الزوجُ Array+Integer ⇒ Any ⇒ تصل القيمةُ
                //      إلى `normalizeArrayPtr` وليس فيها حالةُ `%SadDyn` (بخلاف شقيقتِها
                //      `normalizeMapPtr`) ⇒ `StructGEP` على بنيةٍ لا مؤشّر.
                //
                //      شرطا القبولِ لأيِّ محاولةٍ لاحقة (كلاهما مطلوب):
                //        ١) `normalizeArrayPtr` تحرس الوسمَ وتفكّ الحمولةَ مؤشّرًا، نظيرَ
                //           `normalizeMapPtr` حرفيًّا.
                //        ٢) توزيعُ الفهرسةِ والطرقِ في الأماميّة يعي `Any` فيؤجّله إلى
                //           الحارسِ الزمنيّ بدل رفضِه ساكنًا.
                //      وهما نفسُ الشرطين اللذين أوقفا رفعَ ABI في `sir_builder_helpers.cpp`.
                //
                //      ── تحديثٌ مقيسٌ (لا مُستنتَجٌ من قراءةِ كود) ──
                //      • الشرطُ ١ **مستوفًى**: حالةُ `%SadDyn` أُضيفت إلى `normalizeArrayPtr`
                //        (`array_ops.cpp`، فرعُ `isSadDyn` بحارسِ الوسمِ و`emitDynNotArrayFailure`).
                //        النصُّ أعلاه كان بائتًا حتّى صُحِّح هنا.
                //      • الشرطُ ٢ **مستوفًى في الفهرسةِ نفسِها**: `expression_index.cpp` يمرّرُ
                //        الفهرسَ العدديَّ على `Any` إلى ARRAY_GET بدل رفضِه ساكنًا.
                //      • **العائقُ الباقي انتقلَ إلى موضعٍ ثالثٍ لم يكن مذكورًا**: خانةُ
                //        المتغيّرِ تفقدُ الوسم. مقيسٌ بالمُصغَّرِ نفسِه:
                //            `اطبع(ابحث(1)[0])`         ⇒ ١ (تكافؤٌ تامٌّ مع المفسّر)
                //            `متغير ن = ابحث(1)؛ ن[0]`  ⇒ **٠** (المفسّر: ١)
                //        فالفهرسةُ سليمةٌ والتخزينُ هو الكاسر. ولذلك اقتصرَ التوحيدُ أدناه على
                //        القياسيِّ غيرِ النصّيّ: حمولتُه لا مؤشّرَ فيها فلا تمرُّ بهذا العائق.
                // (EN) Unify types: STRING dominates, I64+F64→F64.
                //      ⚠️ ز.٤٢: a known live defect here plus a documented revert. Unifying
                //      disjoint pairs to `Any` is the honest answer but the frontend cannot
                //      consume it: `normalizeArrayPtr` has no %SadDyn case (unlike its
                //      sibling normalizeMapPtr), so `[1,2] | لاشيء` then `n[0]` exits
                //      silently with code 0. Two acceptance conditions, both required, are
                //      spelled out above — the same two that stopped the ABI promotion.
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
                    // (AR) نصٌّ مقابلَ قياسيٍّ (صحيح/عشريّ/منطقيّ) ⇒ `Any` لا `String`.
                    //      «النصُّ يسيطر» كانت **كذبةً تُنفَّذ**: القيمةُ القياسيّةُ تعبرُ
                    //      حينئذٍ في موضعِ مؤشّرِ نصٍّ، فتُفكُّ عنوانًا. المقيسُ قبلَ الإصلاح:
                    //        نص+صحيح  ⇒ يُترجَمُ ثمّ SIGSEGV (فشلٌ **صامت**)
                    //        نص+عشريّ ⇒ verifyModule يرفض ⇒ «علّةُ مترجمٍ داخليّة»
                    //        نص+منطقيّ⇒ verifyModule يرفض كذلك
                    //      والمفسّرُ يُنفّذُ الثلاثةَ سليمةً، فالتباعُدُ كان في المصرّفِ وحدَه.
                    //      ومع `Any` تعبرُ القيمةُ موسومةً ويفكّها المستهلكُ زمنَ التشغيلِ
                    //      كالمفسّرِ حرفًا بحرف — مُتحقَّقٌ منه في ستّةِ مستهلكين: الطباعةُ
                    //      و`نوع()` والحسابُ ووصلُ النصِّ والمقارنةُ والتمريرُ إلى دالّة.
                    // (EN) String vs a scalar ⇒ `Any`, not `String`. "String dominates" was an
                    //      executed lie: the scalar then crossed in a char* slot and was
                    //      dereferenced (SIGSEGV for int; an invalid module for float/bool),
                    //      while the interpreter ran all three correctly. Tagged crossing
                    //      restores parity — verified across six consumers.
                    else if ((unified == SadTypeKind::String && isScalarKind(returnTypes[i])) ||
                             (returnTypes[i] == SadTypeKind::String && isScalarKind(unified)))
                    {
                        unified = SadTypeKind::Any;
                    }
                    else if (unified == SadTypeKind::String || returnTypes[i] == SadTypeKind::String)
                    {
                        // (AR) نصٌّ مقابلَ **غيرِ قياسيٍّ** (مصفوفة/خريطة/كائن): يبقى على حالِه.
                        //      قِستُ توحيدَه إلى `Any` فأنتجَ **جوابًا خاطئًا صامتًا** لا إصلاحًا
                        //      — انظر ز.٤٢ أعلاه: `متغير ن = ابحث(1)` ثمّ `ن[0]` يطبعُ صفرًا،
                        //      بينما `ابحث(1)[0]` مباشرةً يطبعُ الصوابَ. الخطأُ الصامتُ أسوأُ
                        //      من العطبِ المُعلَن، فلا يُبدَّلُ أحدُهما بالآخرِ قبلَ سدِّ خانةِ
                        //      المتغيّر.
                        // (EN) String vs a NON-scalar (array/map/object) stays as it was:
                        //      unifying it to `Any` produced a silently WRONG answer rather than
                        //      a fix (see ز.٤٢ above). A wrong answer is worse than a declared
                        //      defect, so this waits for the variable-slot fix.
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
