// ============================================================================
// type_checker_expr_visitors.cpp - زوار التعبيرات المتقدمة
// ============================================================================
// (AR) زوار التعبيرات: IndexExpr, MemberExpr, ArrayExpr, MapExpr, NewExpr...
// (EN) Expression visitors: IndexExpr, MemberExpr, ArrayExpr, MapExpr, NewExpr...
// تم استخراج هذا الملف من type_checker.cpp وفقاً لقاعدة CW-05
// ============================================================================
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

#include "semantic/type_checker.h"
#include "token.h"
#include "class_nodes.h"

#include "sad_type_utils.h" // (AR) kindToArabic لرسالة حارس SEM045 / (EN) Arabic kind name for the SEM045 guard

#include <iostream>
#include <algorithm>
#include <cassert>

namespace Sad
{
    namespace Semantic
    {

        using namespace Sad::Types;
        using TT = Lexer::TokenType;

        void TypeChecker::visitIndexExpr(AST::IndexExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr objType = inferExprType(expr.object.get());
            Sad::Types::SadTypePtr idxType = inferExprType(expr.index.get());

            if (objType && objType->isArray())
            {
                // Array[int] -> element type
                if (strictMode_ && idxType && !idxType->isInteger() && !idxType->isUnknown())
                {
                    recordTypeError("", "integer", idxType->toString(), &expr,
                                    "Array index must be integer");
                }
                // (AR) استرجاع نوع العنصر من نوع المصفوفة / (EN) Retrieve element type from array type
                if (objType->getKind() == SadTypeKind::Array)
                {
                    auto *arrType = static_cast<const Sad::Types::SadArrayType *>(objType.get());
                    Sad::Types::SadTypePtr elemType = arrType->getElementType();
                    lastInferredType_ = elemType ? elemType : registry_.getUnknown();
                    return;
                }
            }
            // (AR) إذا كان كائن به عملية فهرسة / (EN) Object with subscript operation
            if (objType && (objType->getKind() == SadTypeKind::Map || objType->isString()))
            {
                lastInferredType_ = objType->isString() ? registry_.getString() : registry_.getAny();
                return;
            }
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitMemberExpr(AST::MemberExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr objType = inferExprType(expr.object.get());

            if (objType && (objType->getKind() == SadTypeKind::Class))
            {
                std::string className;
                if (auto *newExpr = dynamic_cast<AST::NewExpr *>(expr.object.get()))
                {
                    className = newExpr->className;
                }
            //      السجلَّ **لا يملؤه أحدٌ في المستودعِ كلِّه**: `registerStruct`
            //      لا مُنادِيَ له إلّا `StructBuilder::build()` — وهو نفسُه بلا
            //      مُنادٍ. أمرُ القياس (⇒ لا شيءَ خارجَ الملفِّ الميّت):
            //        grep -rn "registerStruct" shared compiler apps tools tests
            //      فكان الحارسُ **كاذبًا أبدًا** والكتلةُ داخلَه لا تُنفَّذُ قطُّ.
            //      فحُذِفَت هي والسجلُّ معًا — الميّتُ يُحذَفُ ولا يُنقَل.
            //      guard on it was permanently false and its body dead code.
            }
            // (AR) للنصوص: خصائص مثل الطول / (EN) For strings: properties like length
            if (objType && objType->isString())
            {
                if (expr.member == "الطول" || expr.member == "length")
                {
                    lastInferredType_ = registry_.getInteger();
                    return;
                }
            }
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitMemberAssignExpr(AST::MemberAssignExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr objType = inferExprType(expr.object.get());
            Sad::Types::SadTypePtr valType = inferExprType(expr.value.get());

            // ════════════════════════════════════════════════════════════════
            // (AR) SEM045 (عقد الغياب — حقول الأصناف): «فراغ» ناتجُ نداءٍ لا يعبر
            //      إلى حقلٍ مصنَّف — نظيرُ رفضِ «رقم س = فراغية()» القائم حرفًا.
            //      حسمُ الصنف: `هذا.` عبر currentCheckedClassName_ — وكائنُ
            //      المتغيّرِ لا اسمَ صنفٍ له في هذه الطبقة فيُترَك لحارسِ
            //      المفسّرِ زمنَ التشغيل (حدٌّ مُعلَن).
            //      الحقلُ المجرَّدُ خانةٌ ديناميّةٌ تقبل الفراغَ — لا رفضَ عليه.
            // (EN) SEM045 (absence contract — class fields): a call's Void must not
            //      cross into a TYPED field — literal sibling of the existing
            //      «رقم س = فراغية()» rejection. Class resolution: `this.` via
            //      currentCheckedClassName_; a variable
            //      object carries no class name at this layer and is left to the
            //      interpreter's runtime guard (declared limit). A bare field is a
            //      dynamic slot and accepts Void — never rejected.
            // ════════════════════════════════════════════════════════════════
            if (valType && valType->getKind() == SadTypeKind::Unit &&
                dynamic_cast<AST::CallExpr *>(expr.value.get()) != nullptr)
            {
                std::string voidClassName;
                if (auto *newExpr = dynamic_cast<AST::NewExpr *>(expr.object.get()))
                {
                    voidClassName = newExpr->className;
                }
                else if (dynamic_cast<AST::ThisExpr *>(expr.object.get()) != nullptr)
                {
                    // (AR) currentFunction_ داخل الطريقة اسمُ الطريقة لا الصنف —
                    //      الاسمُ الصادق currentCheckedClassName_ (مقيس).
                    // (EN) Inside a method currentFunction_ is the METHOD name;
                    //      the truthful source is currentCheckedClassName_.
                    voidClassName = currentCheckedClassName_;
                }
                if (!voidClassName.empty())
                {
                    auto classIt = classFieldKinds_.find(voidClassName);
                    if (classIt != classFieldKinds_.end())
                    {
                        auto fieldIt = classIt->second.find(expr.member);
                        if (fieldIt != classIt->second.end() &&
                            fieldIt->second != SadTypeKind::Unknown &&
                            fieldIt->second != SadTypeKind::Any &&
                            fieldIt->second != SadTypeKind::Unit &&
                            fieldIt->second != SadTypeKind::Null)
                        {
                            recordTypeError(expr.member,
                                            Sad::Types::kindToArabic(fieldIt->second),
                                            valType->toString(), &expr);
                            lastInferredType_ = valType;
                            return;
                        }
                    }
                }
            }

            // (AR) تحقق من توافق نوع العضو مع القيمة المُسندة / (EN) Check member type compatibility with assigned value
            if (strictMode_ && objType && (objType->getKind() == SadTypeKind::Class))
            {
                std::string className;
                if (auto *newExpr = dynamic_cast<AST::NewExpr *>(expr.object.get()))
                {
                    className = newExpr->className;
                }
            //      السجلَّ **لا يملؤه أحدٌ في المستودعِ كلِّه**: `registerStruct`
            //      لا مُنادِيَ له إلّا `StructBuilder::build()` — وهو نفسُه بلا
            //      مُنادٍ. أمرُ القياس (⇒ لا شيءَ خارجَ الملفِّ الميّت):
            //        grep -rn "registerStruct" shared compiler apps tools tests
            //      فكان الحارسُ **كاذبًا أبدًا** والكتلةُ داخلَه لا تُنفَّذُ قطُّ.
            //      فحُذِفَت هي والسجلُّ معًا — الميّتُ يُحذَفُ ولا يُنقَل.
            //      guard on it was permanently false and its body dead code.
            }
            lastInferredType_ = valType;
        }

        // ====================================================================
        // (AR) الصفُّ الفارغُ ‎()‎ ⇒ قيمةُ الوحدة «خالي»
        // ====================================================================
        //
        // (AR) 🔑 وهذا الزائرُ لم يكن موجودًا أصلًا: الفاحصُ يرِثُ زائرًا فارغًا
        //      من `ASTVisitor`، فيبقى `lastInferredType_` صفرًا، فيتراجعُ
        //      `inferExprType` إلى `expr->getTypeKind()` ويُرجِعُ `Unknown`.
        //      و`areTypesCompatible` تُرجِعُ **صادقًا** لكلِّ `Unknown` عمدًا
        //      («لم يُحدَّد بعد») — فكانَ `()` يوافقُ كلَّ نوعٍ في اللغة.
        //
        //      والأثرُ مقيسٌ (2026-09-07) ولم يكن انهيارًا بل **اختلاقَ قيمة**:
        //        `رقم س = ()`  ⇒ يُقبَل ويطبعُ **0**
        //        `نص س = ()`   ⇒ يُقبَل ويطبعُ **«لاشيء»**
        //        `إذا (س)`     ⇒ يُقبَل ويُقرَأُ كاذبًا
        //      أي أنّ خانةً مكتوبةَ النوعِ تحملُ ما ليس من نوعِها، ثمّ تُقرَأُ
        //      قيمةً معقولةَ المظهرِ لا أثرَ فيها للخطأ. ⚠️ وهو أخطرُ من الرفضِ
        //      الكاذبِ ومن الانهيارِ معًا: كلاهما يُرى، وهذا لا يُرى.
        //
        //      🔑 والذراعُ **للفارغِ وحدَه**. والصفُّ المملوءُ يبقى بلا استنتاجٍ
        //      كما كان: عقدُه مشروعٌ مستقلٌّ (مقيسٌ أنّ «رقم س = (1، 2)» يطبعُ
        //      عنوانًا في الذاكرة)، وإصلاحُه ههنا رقعةٌ أوسعُ من علّتِها — وقد
        //      كسرَتْ رقعةٌ كهذه المولِّدَ في هذا الملفِّ عينِه قبلَ يومَين.
        // (EN) This visitor did not exist: the checker inherited an empty one, so
        //      lastInferredType_ stayed null, inferExprType fell back to Unknown,
        //      and areTypesCompatible returns true for Unknown by design — so `()`
        //      matched EVERY type. Measured: «رقم س = ()» was accepted and printed
        //      0; «نص س = ()» printed «لاشيء». Not a crash but a FABRICATED VALUE,
        //      which is worse than either a false rejection or a crash: both are
        //      visible. The arm covers the EMPTY tuple only — a filled tuple's
        //      contract is a separate project, deliberately untouched.
        void TypeChecker::visitTupleExpr(AST::TupleExpr &expr)
        {
            currentResult_.totalExpressions++;

            if (expr.elements.empty())
            {
                lastInferredType_ = registry_.getVoid();
                return;
            }

            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 والصفُّ المملوءُ **يُصنَّفُ صفًّا** — وكان يُترَكُ بلا استنتاج
            //      (2026-09-07، الرقعةُ الأولى) بحجّةِ أنّ عقدَ الصفوفِ مشروعٌ
            //      مستقلٌّ لا يُدَسُّ في رقعةِ نوعِ الوحدة. **والحجّةُ كانت أوسعَ
            //      من موضعِها**: تركُه بلا استنتاجٍ لا يعني «لا عقدَ له» بل
            //      «يوافقُ **كلَّ** نوع» — إذ `Unknown` مقبولٌ مع كلِّ شيءٍ عمدًا.
            //      فبقيَ `خالي س = (1، 2)` مقبولًا يطبعُ `()` — أي أنّ صفًّا من
            //      عنصرَين **يتبخّرُ** في خانةِ وحدة.
            //
            //      🔑 وصنفُ الصفِّ وحدَه يكفي لهذا الحكم: `Unit` ليس `Tuple`
            //      فيُرَدُّ الإسناد. أمّا أنواعُ **عناصرِه** فتبقى غيرَ مُواصَفةٍ
            //      كما كانت — فالبذرةُ ههنا حكمُ صنفٍ لا حكمُ بنية، ولا دعوى
            //      بأنّ عقدَ الصفوفِ اكتمل.
            //
            //      ⚠️ وكسبٌ لم يكن مقصودًا وقِيسَ: `رقم س = (1، 2)` كان يُقبَلُ
            //      ويطبعُ **عنوانًا في الذاكرة** (مقيس: 2465587550624) — أي
            //      تسريبَ مؤشّرٍ في خانةٍ رقميّة. وهو من صنفِ «اختلاقِ القيمة»
            //      نفسِه، وأشدُّ منه: القيمةُ ليست صفرًا معقولًا بل عنوانًا.
            // (EN) A filled tuple is classified as Tuple. The first patch left it
            //      uninferred, arguing the tuple contract is a separate project —
            //      but "uninferred" does not mean "unjudged", it means "matches
            //      EVERY type", since Unknown is deliberately compatible with all.
            //      So `خالي س = (1، 2)` stayed accepted and printed `()`: a
            //      two-element tuple evaporating into a unit slot. Only the KIND is
            //      claimed here; element types stay unspecified as before.
            //      Unintended, measured gain: `رقم س = (1، 2)` used to be accepted
            //      and print a HEAP ADDRESS — a pointer leaking into a numeric slot.
            // ════════════════════════════════════════════════════════════════
            // (AR) 🔑 وعناصرُ الصفِّ **تُحمَل** لا تُلقى: كان يُردُّ
            //      `TupleType(TypeList{})` — صنفٌ صحيحٌ بعناصرَ فارغةٍ مهما كان
            //      عددُها، فيقرأُ كلُّ مستهلكٍ صفرَ عناصر. ولا مستهلكَ اليومَ في
            //      هذه الطبقة، فهو **فخُّ فهرسٍ لأوّلِ مَن يُكتَب** — والأنواعُ
            //      مستنتَجةٌ أصلًا في الحلقةِ نفسِها، فحملُها لا يكلّفُ مرورًا ثانيًا.
            // (EN) The tuple's element types are CARRIED, not discarded: it returned
            //      TupleType(TypeList{}) — a correct kind with empty elements however
            //      many there are — so every consumer reads zero. There is none today,
            //      making it an index trap for the first one written; the types are
            //      already inferred in this very loop.
            std::vector<Sad::Types::SadTypePtr> tupleElements;
            tupleElements.reserve(expr.elements.size());
            for (auto &elem : expr.elements)
            {
                auto elemType = inferExprType(elem.get());
                tupleElements.push_back(elemType ? elemType
                                                 : sadKindToTypePtr(Types::SadTypeKind::Unknown));
            }

            lastInferredType_ = registry_.makeTuple(tupleElements);
        }

        void TypeChecker::visitArrayExpr(AST::ArrayExpr &expr)
        {
            currentResult_.totalExpressions++;

            Sad::Types::SadTypePtr elementType = nullptr;
            for (auto &elem : expr.elements)
            {
                Sad::Types::SadTypePtr et = inferExprType(elem.get());
                if (!elementType)
                {
                    elementType = et;
                }
                else if (et && elementType && et->getKind() != elementType->getKind())
                {
                    // مصفوفة مختلطة / Mixed array
                    if (!et->isUnknown() && !elementType->isUnknown())
                    {
                        currentResult_.addWarning(
                            "Mixed array element types: " + elementType->toString() + " and " + et->toString());
                    }
                }
            }

            // (AR) إنشاء نوع المصفوفة الفعلي مع نوع العنصر
            // (EN) Create actual ArrayType with element type
            if (elementType)
            {
                // (AR) 🔑 وكان يُمرَّرُ ههنا **حجمٌ ثابتٌ** ثانيًا
                //      (`std::optional<size_t>`) يحملُه `ArrayType` في الهرمِ
                //      الثاني. وقِيسَ أنّ قارئَه `getFixedSize()` **بلا مُنادٍ
                //      واحدٍ في المستودعِ كلِّه**:
                //        grep -rn "getFixedSize" shared compiler tools apps ⇒ صفر
                //      فكان الحجمُ يُكتَبُ ولا يُقرَأ — حقلٌ لا عقدَ له.
                //      وهرمُ الأنواعِ الواحدُ لا يحملُه، فسقوطُه **مقيسُ الأثر**
                //      لا مسكوتٌ عنه. ⚠️ وهذا فرقُ تمثيلٍ يُبلَّغُ به المالك.
                // (EN) A fixed-size argument used to ride along here; its only
                //      reader, getFixedSize(), has zero callers tree-wide, so the
                //      value was written and never read. Reported, not decided.
                lastInferredType_ = registry_.makeArray(elementType);
            }
            else
            {
                lastInferredType_ = registry_.makeArray(registry_.getAny());
            }
        }

        void TypeChecker::visitMapExpr(AST::MapExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr keyType = nullptr;
            Sad::Types::SadTypePtr valType = nullptr;
            for (auto &pair : expr.pairs)
            {
                Sad::Types::SadTypePtr kt = inferExprType(pair.key.get());
                Sad::Types::SadTypePtr vt = inferExprType(pair.value.get());
                if (!keyType)
                    keyType = kt;
                if (!valType)
                    valType = vt;
            }
            // (AR) إنشاء نوع القاموس مع نوع المفتاح والقيمة
            lastInferredType_ = registry_.makeMap(
                keyType ? keyType : registry_.getAny(),
                valType ? valType : registry_.getAny());
        }

        void TypeChecker::visitWalrusExpr(AST::WalrusExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr valType = inferExprType(expr.value.get());
            declareVariable(expr.variable, valType);
            lastInferredType_ = valType;
        }

        void TypeChecker::visitLambdaExpr(AST::LambdaExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();

            // Register parameters and collect param types
            std::vector<Sad::Types::SadTypePtr> paramTypes;
            for (auto &param : expr.parameters)
            {
                Sad::Types::SadTypePtr paramType = sadKindToTypePtr(param.type);
                declareVariable(param.name, paramType);
                paramTypes.push_back(paramType);
            }

            // Infer body type
            Sad::Types::SadTypePtr bodyType = inferExprType(expr.body.get());

            exitScope();

            // (AR) إنشاء نوع الدالة مع المعاملات ونوع الإرجاع
            lastInferredType_ = registry_.makeFunction(
                std::move(paramTypes), bodyType ? bodyType : registry_.getVoid());
        }

        void TypeChecker::visitListComprehensionExpr(AST::ListComprehensionExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();
            Sad::Types::SadTypePtr iterType = inferExprType(expr.iterable.get());
            declareVariable(expr.variable, registry_.getUnknown());
            Sad::Types::SadTypePtr elemType = inferExprType(expr.element.get());
            if (expr.condition)
                inferExprType(expr.condition.get());
            exitScope();

            // (AR) نوع المصفوفة الناتجة من الاستيعاب
            lastInferredType_ = registry_.makeArray(elemType ? elemType : registry_.getAny());
        }

        void TypeChecker::visitDictComprehensionExpr(AST::DictComprehensionExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();
            inferExprType(expr.iterable.get());
            declareVariable(expr.variable, registry_.getUnknown());
            Sad::Types::SadTypePtr keyType = inferExprType(expr.key.get());
            Sad::Types::SadTypePtr valType = inferExprType(expr.value.get());
            if (expr.condition)
                inferExprType(expr.condition.get());
            exitScope();

            // (AR) نوع القاموس الناتج من الاستيعاب
            lastInferredType_ = registry_.makeMap(
                keyType ? keyType : registry_.getAny(),
                valType ? valType : registry_.getAny());
        }

        void TypeChecker::visitSetComprehensionExpr(AST::SetComprehensionExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();
            inferExprType(expr.iterable.get());
            declareVariable(expr.variable, registry_.getUnknown());
            Sad::Types::SadTypePtr setElemType = inferExprType(expr.expression.get());
            if (expr.condition)
                inferExprType(expr.condition.get());
            exitScope();

            // (AR) نوع المجموعة الناتجة من الاستيعاب
            lastInferredType_ = registry_.makeArray(setElemType ? setElemType : registry_.getAny());
        }

        void TypeChecker::visitGeneratorExpr(AST::GeneratorExpr &expr)
        {
            currentResult_.totalExpressions++;

            enterScope();
            inferExprType(expr.iterable.get());
            declareVariable(expr.variable, registry_.getUnknown());
            inferExprType(expr.element.get());
            if (expr.condition)
                inferExprType(expr.condition.get());
            exitScope();

            lastInferredType_ = registry_.getAny();
        }

        void TypeChecker::visitDecoratorExpr(AST::DecoratorExpr &expr)
        {
            currentResult_.totalExpressions++;
            for (auto &arg : expr.arguments)
            {
                if (arg)
                    inferExprType(arg.get());
            }
            lastInferredType_ = registry_.getUnknown();
        }

        // ============================================================================
        // OOP Expressions / تعابير الكائنات
        // ============================================================================

        void TypeChecker::visitNewExpr(AST::NewExpr &expr)
        {
            currentResult_.totalExpressions++;
            for (auto &arg : expr.arguments)
            {
                if (arg)
                    inferExprType(arg.get());
            }
            //      السجلَّ **لا يملؤه أحدٌ في المستودعِ كلِّه**: `registerStruct`
            //      لا مُنادِيَ له إلّا `StructBuilder::build()` — وهو نفسُه بلا
            //      مُنادٍ. أمرُ القياس (⇒ لا شيءَ خارجَ الملفِّ الميّت):
            //        grep -rn "registerStruct" shared compiler apps tools tests
            //      فكان الحارسُ **كاذبًا أبدًا** والكتلةُ داخلَه لا تُنفَّذُ قطُّ.
            //      فحُذِفَت هي والسجلُّ معًا — الميّتُ يُحذَفُ ولا يُنقَل.
            //      guard on it was permanently false and its body dead code.
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitMemberAccessExpr(AST::MemberAccessExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr objType = inferExprType(expr.object.get());

            if (objType && (objType->getKind() == SadTypeKind::Class))
            {
                // (AR) محاولة تحديد اسم الصنف / (EN) Try to identify class name
                std::string className;
                if (auto *newExpr = dynamic_cast<AST::NewExpr *>(expr.object.get()))
                {
                    className = newExpr->className;
                }
                else if (auto *thisExpr = dynamic_cast<AST::ThisExpr *>(expr.object.get()))
                {
                    (void)thisExpr;
                    className = currentFunction_;
                }
            //      السجلَّ **لا يملؤه أحدٌ في المستودعِ كلِّه**: `registerStruct`
            //      لا مُنادِيَ له إلّا `StructBuilder::build()` — وهو نفسُه بلا
            //      مُنادٍ. أمرُ القياس (⇒ لا شيءَ خارجَ الملفِّ الميّت):
            //        grep -rn "registerStruct" shared compiler apps tools tests
            //      فكان الحارسُ **كاذبًا أبدًا** والكتلةُ داخلَه لا تُنفَّذُ قطُّ.
            //      فحُذِفَت هي والسجلُّ معًا — الميّتُ يُحذَفُ ولا يُنقَل.
            //      guard on it was permanently false and its body dead code.
            }
            // (AR) خصائص النص / (EN) String properties
            if (objType && objType->isString())
            {
                if (expr.memberName == "الطول" || expr.memberName == "length")
                {
                    lastInferredType_ = registry_.getInteger();
                    return;
                }
            }
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitMethodCallExpr(AST::MethodCallExpr &expr)
        {
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr objType = inferExprType(expr.object.get());
            for (auto &arg : expr.arguments)
            {
                if (arg)
                    inferExprType(arg.get());
            }
            // (AR) استنتاج نوع الإرجاع من الطريقة / (EN) Infer return type from method
            // (AR) طرق النص تُرجع أنواعاً معروفة / (EN) String methods return known types
            if (objType && objType->isString())
            {
                const std::string &method = expr.methodName;
                if (method == "الطول" || method == "length")
                {
                    lastInferredType_ = registry_.getInteger();
                    return;
                }
                if (method == "يحتوي" || method == "contains" ||
                    method == "يبدأ_بـ" || method == "startsWith" ||
                    method == "ينتهي_بـ" || method == "endsWith")
                {
                    lastInferredType_ = registry_.getBoolean();
                    return;
                }
                if (method == "قطع" || method == "slice" ||
                    method == "استبدل" || method == "replace" ||
                    method == "حروف_كبيرة" || method == "toUpperCase" ||
                    method == "حروف_صغيرة" || method == "toLowerCase")
                {
                    lastInferredType_ = registry_.getString();
                    return;
                }
            }
            // (AR) طرق المصفوفة / (EN) Array methods
            if (objType && objType->isArray())
            {
                const std::string &method = expr.methodName;
                if (method == "الطول" || method == "length" || method == "حجم" || method == "size")
                {
                    lastInferredType_ = registry_.getInteger();
                    return;
                }
                if (method == "أضف" || method == "push" || method == "ادفع")
                {
                    lastInferredType_ = registry_.getVoid();
                    return;
                }
            }
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitThisExpr(AST::ThisExpr &expr)
        {
            currentResult_.totalExpressions++;
            // (AR) البحث عن نوع الصنف الحالي
            //      السجلَّ **لا يملؤه أحدٌ في المستودعِ كلِّه**: `registerStruct`
            //      لا مُنادِيَ له إلّا `StructBuilder::build()` — وهو نفسُه بلا
            //      مُنادٍ. أمرُ القياس (⇒ لا شيءَ خارجَ الملفِّ الميّت):
            //        grep -rn "registerStruct" shared compiler apps tools tests
            //      فكان الحارسُ **كاذبًا أبدًا** والكتلةُ داخلَه لا تُنفَّذُ قطُّ.
            //      فحُذِفَت هي والسجلُّ معًا — الميّتُ يُحذَفُ ولا يُنقَل.
            //      guard on it was permanently false and its body dead code.
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitSuperExpr(AST::SuperExpr &expr)
        {
            currentResult_.totalExpressions++;
            // (AR) super يشير للصنف الأب — يعتمد على السياق
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitBorrowExpr(AST::BorrowExpr &expr)
        {
            currentResult_.totalExpressions++;
            auto type = lookupVariable(expr.variableName);
            lastInferredType_ = type ? type : registry_.getUnknown();
        }

        void TypeChecker::visitInlineAsmExpr(AST::InlineAsmExpr &expr)
        {
            currentResult_.totalExpressions++;
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitRangeExpr(AST::RangeExpr &expr)
        {
            // (AR) تعبير النطاق 1..10 — ينتج مصفوفة أرقام
            // (EN) Range expression 1..10 — produces an array of integers
            currentResult_.totalExpressions++;
            if (expr.start)
                inferExprType(expr.start.get());
            if (expr.end)
                inferExprType(expr.end.get());
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitOptionalChainExpr(AST::OptionalChainExpr &expr)
        {
            // (AR) وصول آمن ?. — نوع النتيجة هو نوع العضو أو لاشيء
            // (EN) Optional chaining ?. — result is member type or null
            currentResult_.totalExpressions++;
            if (expr.object)
            {
                Sad::Types::SadTypePtr objType = inferExprType(expr.object.get());
                // (AR) البحث عن نوع العضو إذا كان الكائن بنية/صنف
                // (EN) Look up member type if object is a struct/class
                if (objType && objType->getKind() == SadTypeKind::Class)
                {
            //      السجلَّ **لا يملؤه أحدٌ في المستودعِ كلِّه**: `registerStruct`
            //      لا مُنادِيَ له إلّا `StructBuilder::build()` — وهو نفسُه بلا
            //      مُنادٍ. أمرُ القياس (⇒ لا شيءَ خارجَ الملفِّ الميّت):
            //        grep -rn "registerStruct" shared compiler apps tools tests
            //      فكان الحارسُ **كاذبًا أبدًا** والكتلةُ داخلَه لا تُنفَّذُ قطُّ.
            //      فحُذِفَت هي والسجلُّ معًا — الميّتُ يُحذَفُ ولا يُنقَل.
            //      guard on it was permanently false and its body dead code.
                }
            }
            // (AR) لا نعرف النوع — يمكن أن يكون لاشيء
            // (EN) Unknown type — could be null
            lastInferredType_ = registry_.getUnknown();
        }

        void TypeChecker::visitNullCoalesceExpr(AST::NullCoalesceExpr &expr)
        {
            // (AR) تجميع فارغ ?? — نوع النتيجة هو نوع الأيمن (البديل)
            // (EN) Null coalescing ?? — result type is the right (fallback) type
            currentResult_.totalExpressions++;
            Sad::Types::SadTypePtr leftType = nullptr;
            Sad::Types::SadTypePtr rightType = nullptr;

            if (expr.left)
            {
                leftType = inferExprType(expr.left.get());
            }
            if (expr.right)
            {
                rightType = inferExprType(expr.right.get());
            }

            // (AR) إذا كان النوعان معروفين ومتوافقين، استخدم الأيمن
            // (EN) If both types known and compatible, use right type
            if (rightType)
            {
                lastInferredType_ = rightType;
            }
            else if (leftType)
            {
                lastInferredType_ = leftType;
            }
            else
            {
                lastInferredType_ = registry_.getUnknown();
            }
        }

        // ============================================================================
        // (AR) [أ-م٢] بناء عضو تعداد بحمولة صريح — عدد(٥) أو تعداد.عدد(٥)
        // (EN) [A-M2] Explicit tagged-enum variant construction — عدد(5) or Enum.عدد(5)
        //
        // (AR) المحلّل النحويّ لا يُنتج EnumVariantExpr في أ-م١/أ-م٢ (يبقى «عدد(٥)»
        //      استدعاءً يُحسم في visitCallExpr)؛ هذا المُحقِّق دفاعيّ وجاهز لأ-م٣ حين
        //      يخفض المفسّر/المولّد الاستدعاء إلى هذه العقدة. يُعيد استعمال نفس فحص
        //      الحمولة تمامًا كي لا يتباعد المساران.
        // (EN) The parser does not emit EnumVariantExpr in A-M1/A-M2 («عدد(5)» stays a
        //      call resolved in visitCallExpr); this visitor is defensive and ready for
        //      A-M3 when the interpreter/backend lowers the call to this node. It reuses
        //      the exact same payload check so the two paths cannot diverge.
        // ============================================================================
        void TypeChecker::visitEnumVariantExpr(AST::EnumVariantExpr &expr)
        {
            currentResult_.totalExpressions++;

            for (auto &arg : expr.args)
            {
                if (arg)
                    inferExprType(arg.get());
            }

            const EnumVariantInfo *variant = nullptr;

            if (!expr.enumName.empty())
            {
                // (AR) مؤهَّل: ابحث في تعداده المُصرَّح مباشرةً.
                // (EN) Qualified: look up directly in its declared enum.
                auto enumIt = enumVariants_.find(expr.enumName);
                if (enumIt != enumVariants_.end())
                {
                    for (const auto &v : enumIt->second)
                    {
                        if (v.variantName == expr.variantName)
                        {
                            variant = &v;
                            break;
                        }
                    }
                }
            }
            else
            {
                std::string owningEnum;
                bool ambiguous = false;
                variant = lookupVariant(expr.variantName, owningEnum, ambiguous);
            }

            if (variant)
            {
                lastInferredType_ =
                    checkEnumConstruction(expr.variantName, *variant, expr.args, &expr);
            }
            else
            {
                lastInferredType_ = registry_.getUnknown();
            }
        }

        // ============================================================================
        // زيارة العبارات / Visit Statements
        // ============================================================================

    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
