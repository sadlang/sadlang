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

            // ============================================================================
            // HELPER FUNCTIONS - الدوال المساعدة
            // ============================================================================

            // ============================================================================
            // createBasicBlock - إنشاء كتلة أساسية جديدة
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:501
            // التوقيع / Signature: std::shared_ptr<SIRBasicBlock> createBasicBlock(const std::string& name);
            //
            // الإرجاع / Returns:
            // - std::shared_ptr<SIRBasicBlock>: sir_module.h:145 (SIRBasicBlock class)
            //
            // SIRBasicBlock Constructor (sir_module.h:145):
            // - SIRBasicBlock(const std::string& name): line 168
            // ============================================================================
            std::shared_ptr<SIRBasicBlock> SIRBuilder::createBasicBlock(const std::string &name)
            {
                // (AR) إنشاء كتلة جديدة (sir_module.h:168 - SIRBasicBlock constructor)
                // (EN) Create new block
                return std::make_shared<SIRBasicBlock>(name);
            }

            // ============================================================================
            // newTempRegister - إنشاء سجل مؤقت جديد
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:511
            // التوقيع / Signature: std::string newTempRegister();
            //
            // المتغيرات المستخدمة / Used variables:
            // - registerCounter_: sir_builder.h:600 (int)
            //
            // الإرجاع / Returns:
            // - std::string: اسم السجل بصيغة %0, %1, %2, ...
            // ============================================================================
            std::string SIRBuilder::newTempRegister()
            {
                // (AR) إنشاء سجل بصيغة %N حيث N هو nextTempRegister_
                // (EN) Create register in format %N where N is nextTempRegister_
                return "%" + std::to_string(nextTempRegister_++);
            }

            // ============================================================================
            // newLabel - إنشاء تسمية جديدة
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:520
            // التوقيع / Signature: std::string newLabel(const std::string& prefix);
            //
            // المعاملات / Parameters:
            // - prefix: std::string = البادئة (L, if, loop, etc)
            //
            // المتغيرات المستخدمة / Used variables:
            // - labelCounter_: sir_builder.h:601 (int)
            //
            // الإرجاع / Returns:
            // - std::string: اسم التسمية بصيغة prefix_N
            // ============================================================================
            std::string SIRBuilder::newLabel(const std::string &prefix)
            {
                // (AR) إنشاء تسمية بصيغة prefix_N
                // (EN) Create label in format prefix_N
                return prefix + "_" + std::to_string(nextLabel_++);
            }

            // ============================================================================
            // astTypeToSIRType — جسر النوع المُعلَن (AST) إلى نوع SIR
            // ============================================================================
            // (AR) **دالّةٌ كلّيّة**: لكلّ قيمةٍ من SadTypeKind فرعٌ صريح، ولا فرعَ
            //      افتراضيَّ يُخمّن. كان الفرعُ الافتراضيُّ يُرجِع Integer مع تحذيرٍ
            //      على الخرج القياسيّ لأربعين نوعًا من اثنين وخمسين — فيُترجَم برنامجٌ
            //      صحيحٌ إلى شيفرةٍ خاطئةٍ بدل أن يُرفَض. مثالُه المقيس: «خريطة» عابرةً
            //      حدَّ النداء كانت تُخمَّن عددًا، فتسلك فهرسةُ `م["م"]` **مسارَ النصّ**
            //      (اقتطاعَ سلسلة) فتُصدَر `add i64، ptr` وتسقط الوحدةُ كلُّها في
            //      verifyModule. الخرائطُ المحلّيّة نجت لأنّ نوعها يُستنتَج من المُهيّئ
            //      لا من تعليقٍ مُعلَن.
            //
            //      ولأنّ SIR::SadTypeKind ليس تعدادًا آخر بل **هو نفسه**
            //      Sad::Types::SadTypeKind (sir_types.h: `using`)، فالتحويلُ هويّةٌ
            //      في الأصل، وكلُّ خروجٍ عنها **خفضُ تمثيلٍ مقصود** يُذكَر لا يُخمَّن.
            //
            //      وحارسُ التكرار: static_assert على SAD_TYPE_KIND_COUNT المُولَّد من
            //      types.yaml — فإضافةُ نوعٍ جديدٍ إلى مصدر الحقيقة تكسر البناءَ هنا
            //      وتُلزِم قرارًا صريحًا، بدل أن تسقط صامتةً كما سقطت «خريطة».
            //
            // (EN) **Total function**: every SadTypeKind value has an explicit arm and
            //      there is no guessing default. The old default returned Integer (with
            //      a warning on stderr) for 40 of 52 kinds, so a valid program compiled
            //      to wrong code instead of being rejected. Measured instance: a «خريطة»
            //      crossing a call boundary was guessed as a number, so `m["k"]` took the
            //      STRING indexing path and emitted `add i64, ptr`, failing verifyModule.
            //      Local maps survived because their type is inferred from the initialiser.
            //
            //      Since SIR::SadTypeKind IS Sad::Types::SadTypeKind (a `using` alias in
            //      sir_types.h), this conversion is identity by nature; every departure
            //      is a deliberate representation lowering, stated rather than guessed.
            //
            //      Recurrence guard: static_assert on the generated SAD_TYPE_KIND_COUNT —
            //      adding a kind to types.yaml breaks the build here and forces a decision.
            // ============================================================================
            SadTypeKind SIRBuilder::astTypeToSIRType(const Sad::Types::SadTypeKind &type)
            {
                static_assert(Sad::Types::SAD_TYPE_KIND_COUNT == 52,
                              "(AR) تغيّر عددُ أنواع types.yaml — راجع كلَّ فرعٍ أدناه وقرّر "
                              "تمثيلَ النوع الجديد صراحةً. (EN) types.yaml kind count changed — "
                              "revisit the arms below and decide the new kind's representation.");

                switch (type)
                {
                // ─── هويّة: أنواعٌ لها تمثيلٌ أوّليٌّ في SIR وفي مُخطِّط أنواع LLVM ───
                // ─── Identity: kinds with a first-class SIR / LLVM lowering ───
                case Types::SadTypeKind::Void:
                    return SadTypeKind::Void;
                case Types::SadTypeKind::Integer:
                    return SadTypeKind::Integer;
                case Types::SadTypeKind::Float:
                    return SadTypeKind::Float;
                case Types::SadTypeKind::Boolean:
                    return SadTypeKind::Boolean;
                case Types::SadTypeKind::String:
                    return SadTypeKind::String;
                case Types::SadTypeKind::Byte:
                    return SadTypeKind::Byte;
                case Types::SadTypeKind::UInt64:
                    return SadTypeKind::UInt64;
                case Types::SadTypeKind::Array:
                    return SadTypeKind::Array;
                case Types::SadTypeKind::Function:
                    return SadTypeKind::Function;
                // (AR) خريطة: مقبض الخريطة `ptr` في الخلفيّة (map_ops يُصدِر البنيةَ
                //      مباشرةً ويعيد مؤشّرًا)، ومُخطِّطُ الأنواع يُرجِع مؤشّرًا كذلك —
                //      فالهويّةُ هنا متّسقةٌ مع التمثيل الفعليّ، مقيسةً لا مفترضة.
                // (EN) Map: the backend emits the map struct inline and yields a `ptr`,
                //      and the type mapper lowers Map to a pointer — identity here matches
                //      the actual representation (measured, not assumed).
                case Types::SadTypeKind::Map:
                    return SadTypeKind::Map;

                // ─── توسيعٌ عدديٌّ مقصود: لا تمثيلَ أضيقَ في الخلفيّة بعدُ ───
                // ─── Deliberate numeric widening: no narrower lowering exists yet ───
                case Types::SadTypeKind::Int8:
                case Types::SadTypeKind::Int16:
                case Types::SadTypeKind::Int32:
                case Types::SadTypeKind::Int64:
                case Types::SadTypeKind::UInt8:
                case Types::SadTypeKind::UInt16:
                case Types::SadTypeKind::UInt32:
                case Types::SadTypeKind::Char:
                    return SadTypeKind::Integer;
                case Types::SadTypeKind::Float32:
                case Types::SadTypeKind::Float64:
                    return SadTypeKind::Float;

                // ─── مقبضٌ معتِم (i64): أنواعٌ تُمرَّر قيمةً/مؤشّرًا بلا بنيةٍ في SIR ───
                // (AR) هذه الأفرعُ تحفظ السلوكَ القائمَ **صراحةً** بعد إزالة الفرع
                //      الافتراضيّ. رفعُ أيٍّ منها إلى الهويّة تغييرُ ABI يلزمه قياسٌ
                //      مستقلٌّ على المحرّكين، فلا يُدسّ ضمن هذا الإصلاح.
                // (EN) Opaque handle (i64): kinds passed as a value/pointer with no SIR
                //      structure. These arms preserve existing behaviour EXPLICITLY now
                //      that the default is gone. Promoting any of them to identity is an
                //      ABI change requiring its own dual-engine measurement — not smuggled
                //      into this fix.
                case Types::SadTypeKind::Tuple:
                case Types::SadTypeKind::Slice:
                case Types::SadTypeKind::Class:
                case Types::SadTypeKind::Struct:
                case Types::SadTypeKind::Enum:
                case Types::SadTypeKind::Trait:
                case Types::SadTypeKind::Closure:
                case Types::SadTypeKind::Union:
                case Types::SadTypeKind::Intersection:
                case Types::SadTypeKind::Optional:
                case Types::SadTypeKind::Result:
                case Types::SadTypeKind::Generic:
                case Types::SadTypeKind::TypeParameter:
                case Types::SadTypeKind::TypeAlias:
                case Types::SadTypeKind::Pointer:
                case Types::SadTypeKind::Reference:
                case Types::SadTypeKind::MutableRef:
                case Types::SadTypeKind::Error:
                case Types::SadTypeKind::Future:
                case Types::SadTypeKind::Generator:
                case Types::SadTypeKind::Comprehension:
                case Types::SadTypeKind::Color:
                case Types::SadTypeKind::Widget:
                case Types::SadTypeKind::Window:
                case Types::SadTypeKind::Event:
                case Types::SadTypeKind::Vector:
                case Types::SadTypeKind::Null:
                case Types::SadTypeKind::Point:
                case Types::SadTypeKind::Rect:
                case Types::SadTypeKind::Never:
                // (AR) مجهول: نائبٌ يستبدله استنتاجُ الأنواع لاحقًا.
                // (EN) Unknown: placeholder overwritten by type inference.
                case Types::SadTypeKind::Unknown:
                    return SadTypeKind::Integer;

                // (AR) ⚠️ **حالةٌ مستقلّةٌ عمدًا، لا في سلسلةِ السقوطِ أعلاه.** كانت
                //      `case Any:` آخِرَ لصيقةٍ في تلك السلسلة، فمحاولةُ الرفعِ بكتابةِ
                //      `return Any;` تحتها **تشقّ السلسلةَ**: كلُّ الثلاثين لصيقةً قبلها
                //      (Class · Struct · Enum · TypeParameter · Pointer …) تصير هي أيضًا
                //      Any. وهذا بالضبط ما أفسد قياسَ الرفعِ الرابع: التسعةُ التي عُدَّت
                //      «تباعُداتِ أي» كانت أثرَ رفعِ **الأصنافِ والبِنى ومعاملاتِ النوع**
                //      معها. فالحالةُ هنا منفصلةٌ بنيويًّا كي لا يتكرّر الخطأُ صامتًا.
                // (EN) Deliberately a STANDALONE case, not part of the fallthrough chain
                //      above. `case Any:` used to be that chain's last label, so writing
                //      `return Any;` under it silently split the chain and lifted thirty
                //      other kinds (Class, Struct, Enum, TypeParameter, Pointer, …) too —
                //      which is exactly what invalidated the fourth lift measurement.
                // (AR) **«أي» المُعلَن يُخفَّض إلى الهويّة (%SadDyn) — ISSUE-076 مُغلَق.**
                //      كان يُخفَّض i64 فتضيع قيمتُه عند الحدّ: `أي س = خليط[١]` يطبع
                //      **عنوانَ مكدّسٍ يختلف بالمعماريّة** (x86 ١٤٠٧٣٥٣٧٠٤٤٧٣٥٢ ·
                //      arm ٧٠٣٦٨٧٥٢٥٦٧٦٣٢) بدل `2.5`. جُرّب الرفعُ ثلاثَ مرّاتٍ وأُعيد،
                //      ثمّ قِيس رابعةً على ٢٨٥٧ اختبارَ مطابقةٍ: **صفرُ تباعُد**،
                //      وctest ١٤٩/١٥٠ (الأحمرُ سابقٌ)، و١١٠+٣٢ تنفيذًا أصليًّا خضراء.
                //      الشرطُ الأخيرُ (افتراقُ موقعَي فكِّ التعليب) أُغلق بتوحيدِ الجدولِ
                //      في `coerceToParamType`، وحدُّ C يُخفَّض i64 صراحةً في functions2_ops.
                // (EN) Declared `أي` lowers to identity (%SadDyn) — ISSUE-076 closed.
                case Types::SadTypeKind::Any:
                    return SadTypeKind::Any;
                }

                // (AR) لا يُبلَغ إلّا بقيمةٍ خارج التعداد (تحويلٌ فاسد) — نُفشِل بصوتٍ
                //      عالٍ لا نُخمّن تمثيلًا. (EN) Reachable only for an out-of-enum
                //      value (corrupt cast) — fail loudly instead of guessing.
                throw std::logic_error(
                    "astTypeToSIRType: SadTypeKind خارج التعداد / out of enum range: " +
                    std::to_string(static_cast<int>(type)));
            }

            // ============================================================================
            // astTypeToSadType — تحويل DataType إلى SadTypePtr (النظام الموحد)
            // ============================================================================
            // (AR) يُرجع SadTypePtr مباشرة بدلاً من SadTypeKind
            //      يستخدم SadType::fromDataType للتحويل المباشر
            //      هذا هو الطريق المثالي — يتجنب فقدان معلومات النوع
            // (EN) Returns SadTypePtr directly instead of SadTypeKind
            //      Uses SadType::fromDataType for direct conversion
            //      This is the ideal path — avoids type information loss
            // ============================================================================
            Sad::Types::SadTypePtr SIRBuilder::astTypeToSadType(const Sad::Types::SadTypeKind &type)
            {
                return Sad::Types::SadType::fromValueType(type);
            }

            // ============================================================================
            // hasReturnWithValue - فحص إذا كانت الجملة تحتوي return مع قيمة
            // ============================================================================
            // التوقيع / Signature: bool hasReturnWithValue(const Sad::AST::Statement* stmt);
            //
            // الوصف / Description:
            // تفحص الجملة بشكل تعاودي للبحث عن جمل return تحتوي قيمة.
            // Recursively checks statement for return statements with values.
            // ============================================================================
            bool SIRBuilder::hasReturnWithValue(const Sad::AST::Statement *stmt)
            {
                if (!stmt)
                    return false;

                // (AR) جملة return مباشرة
                // (EN) Direct return statement
                if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt *>(stmt))
                {
                    return ret->value != nullptr; // true if return has a value
                }

                // (AR) كتلة من الجمل
                // (EN) Block of statements
                if (auto block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                {
                    for (const auto &s : block->statements)
                    {
                        if (hasReturnWithValue(s.get()))
                            return true;
                    }
                    return false;
                }

                // (AR) جملة if
                // (EN) If statement
                if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                {
                    if (hasReturnWithValue(ifStmt->thenBranch.get()))
                        return true;
                    if (ifStmt->elseBranch && hasReturnWithValue(ifStmt->elseBranch.get()))
                        return true;
                    return false;
                }

                // (AR) حلقة while
                // (EN) While loop
                if (auto whileLoop = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                {
                    return hasReturnWithValue(whileLoop->body.get());
                }

                // (AR) حلقة for
                // (EN) For loop
                if (auto forLoop = dynamic_cast<const Sad::AST::ForStmt *>(stmt))
                {
                    return hasReturnWithValue(forLoop->body.get());
                }

                // (AR) حلقة for-range
                // (EN) For-range loop
                if (auto forRange = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                {
                    return hasReturnWithValue(forRange->body.get());
                }

                // (AR) جملة match — نبحث في أجسام جميع الحالات
                // (EN) Match statement — search in all case bodies
                if (auto matchStmt = dynamic_cast<const Sad::AST::MatchStmt *>(stmt))
                {
                    for (const auto &caseClause : matchStmt->cases)
                    {
                        for (const auto &bodyStmt : caseClause.body)
                        {
                            if (hasReturnWithValue(bodyStmt.get()))
                                return true;
                        }
                    }
                    return false;
                }

                // (AR) جملة حالة/switch — نبحث في أجسام جميع الفروع والافتراضيّ (ISSUE-055)
                //      دون هذا يُستنتَج نوع الدالة void فيُقرأ الإرجاع 0 في المُنادي.
                // (EN) Switch statement — search all case bodies + default (ISSUE-055).
                //      Without this the function is inferred void and its return reads 0.
                if (auto switchStmt = dynamic_cast<const Sad::AST::SwitchStmt *>(stmt))
                {
                    for (const auto &caseBranch : switchStmt->cases)
                    {
                        if (hasReturnWithValue(caseBranch.body.get()))
                            return true;
                    }
                    if (switchStmt->defaultCase && hasReturnWithValue(switchStmt->defaultCase.get()))
                        return true;
                    return false;
                }

                // (AR) جملة try-catch — نبحث في كتل المحاولة والالتقاط
                // (EN) Try-catch statement — search in try and catch blocks
                if (auto tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                {
                    if (hasReturnWithValue(tryStmt->tryBlock.get()))
                        return true;
                    for (const auto &catchClause : tryStmt->catchClauses)
                    {
                        if (hasReturnWithValue(catchClause.body.get()))
                            return true;
                    }
                    if (tryStmt->finallyBlock && hasReturnWithValue(tryStmt->finallyBlock.get()))
                        return true;
                    return false;
                }

                // (AR) جمل أخرى لا تحتوي return
                // (EN) Other statements don't contain return
                return false;
            }

            // ============================================================================
            // inferReturnTypeFromBody - استنتاج نوع الإرجاع من جسم الدالة
            // ============================================================================
            // التوقيع / Signature: SadTypeKind inferReturnTypeFromBody(const Sad::AST::Statement* body);
            //
            // الوصف / Description:
            // تفحص جسم الدالة لاستنتاج نوع الإرجاع:
            // - إذا لم يكن هناك return مع قيمة، تُرجع VOID
            // - إذا وُجد return مع قيمة، تفحص نوع التعبير
            // ============================================================================

            // Helper: get the return expression from the body
            static const Sad::AST::Expression *findFirstReturnExpr(const Sad::AST::Statement *stmt)
            {
                if (!stmt)
                    return nullptr;
                if (auto ret = dynamic_cast<const Sad::AST::ReturnStmt *>(stmt))
                {
                    return ret->value ? ret->value.get() : nullptr;
                }
                if (auto block = dynamic_cast<const Sad::AST::BlockStmt *>(stmt))
                {
                    for (const auto &s : block->statements)
                    {
                        auto *e = findFirstReturnExpr(s.get());
                        if (e)
                            return e;
                    }
                }
                if (auto ifStmt = dynamic_cast<const Sad::AST::IfStmt *>(stmt))
                {
                    auto *e = findFirstReturnExpr(ifStmt->thenBranch.get());
                    if (e)
                        return e;
                    if (ifStmt->elseBranch)
                        return findFirstReturnExpr(ifStmt->elseBranch.get());
                }
                if (auto w = dynamic_cast<const Sad::AST::WhileStmt *>(stmt))
                {
                    return findFirstReturnExpr(w->body.get());
                }
                if (auto f = dynamic_cast<const Sad::AST::ForStmt *>(stmt))
                {
                    return findFirstReturnExpr(f->body.get());
                }
                if (auto fr = dynamic_cast<const Sad::AST::ForRangeStmt *>(stmt))
                {
                    return findFirstReturnExpr(fr->body.get());
                }
                // (AR) جملة match — نبحث عن أول return في أي حالة
                // (EN) Match statement — find first return in any case
                if (auto matchStmt = dynamic_cast<const Sad::AST::MatchStmt *>(stmt))
                {
                    for (const auto &caseClause : matchStmt->cases)
                    {
                        for (const auto &bodyStmt : caseClause.body)
                        {
                            auto *e = findFirstReturnExpr(bodyStmt.get());
                            if (e)
                                return e;
                        }
                    }
                }
                // (AR) جملة حالة/switch — أوّل return في أيّ فرع أو الافتراضيّ (ISSUE-055)
                // (EN) Switch statement — first return in any case body or default (ISSUE-055)
                if (auto switchStmt = dynamic_cast<const Sad::AST::SwitchStmt *>(stmt))
                {
                    for (const auto &caseBranch : switchStmt->cases)
                    {
                        auto *e = findFirstReturnExpr(caseBranch.body.get());
                        if (e)
                            return e;
                    }
                    if (switchStmt->defaultCase)
                    {
                        auto *e = findFirstReturnExpr(switchStmt->defaultCase.get());
                        if (e)
                            return e;
                    }
                }
                // (AR) جملة try-catch — نبحث في كتل المحاولة والالتقاط
                // (EN) Try-catch statement — search in try and catch blocks
                if (auto tryStmt = dynamic_cast<const Sad::AST::TryStmt *>(stmt))
                {
                    auto *e = findFirstReturnExpr(tryStmt->tryBlock.get());
                    if (e)
                        return e;
                    for (const auto &catchClause : tryStmt->catchClauses)
                    {
                        e = findFirstReturnExpr(catchClause.body.get());
                        if (e)
                            return e;
                    }
                    if (tryStmt->finallyBlock)
                    {
                        e = findFirstReturnExpr(tryStmt->finallyBlock.get());
                        if (e)
                            return e;
                    }
                }
                return nullptr;
            }

            // ================================================================
            // (AR) نوعُ خانةِ التصريحِ — سلطةُ محوِ `Optional` الواحدة
            // (EN) Declaration storage kind — the single Optional-erasure authority
            // ================================================================
            SadTypeKind SIRBuilder::resolveDeclaredStorageKind(
                const Sad::Types::SadTypeKind &declaredKind,
                const Sad::Types::SadType *declaredSadType,
                SadTypeKind fallbackKind)
            {
                if (declaredKind != Types::SadTypeKind::Optional || declaredSadType == nullptr)
                {
                    return fallbackKind;
                }

                const auto *optional =
                    dynamic_cast<const Sad::Types::SadOptionalType *>(declaredSadType);
                if (optional == nullptr || !optional->getInnerType())
                {
                    return fallbackKind;
                }

                // (AR) العقد (أ): النوعُ الداخليُّ إن كانت خانتُه تحفظ العدمَ، و`Any`
                //      (‏%SadDyn = وسمٌ خارجَ النطاق) إن كانت لا تحفظُه.
                // (EN) Contract (a): the inner kind when its slot preserves null, `Any`
                //      (%SadDyn = out-of-band tag) when it does not.
                return sirNullableStorageKind(
                    astTypeToSIRType(optional->getInnerType()->getKind()));
            }

            // ================================================================
            // (AR) بابُ الخانةِ الديناميّةِ الواحد (ISSUE-138 + SEM045): خانةٌ بلا
            //      نوعٍ — بلا تهيئةٍ، أو بمُهيِّئٍ استُنتج «فراغًا»
            // (EN) The single dynamic-slot door (ISSUE-138 + SEM045): a typeless
            //      slot — with no initializer, or with a Void-inferred one
            // ================================================================
            SadTypeKind SIRBuilder::resolveBareSlotStorageKind(
                const Sad::Types::SadTypeKind &declaredKind,
                bool hasInitializer,
                SadTypeKind resolvedKind)
            {
                // (AR) الشرطان لازمان معًا: `Unknown` **مع** مُهيِّئٍ يستبدله الاستنتاجُ
                //      فعلًا فلا يُمَسّ، و`Unknown` بلا مُهيِّئٍ لا يستبدله شيء.
                // (EN) Both clauses are required: Unknown WITH an initializer really is
                //      overwritten by inference, so it is left alone.
                if (declaredKind == Types::SadTypeKind::Unknown && !hasInitializer)
                {
                    return SadTypeKind::Any;
                }
                // ════════════════════════════════════════════════════════════════
                // (AR) SEM045 (دَين الخانة المجرَّدة): مُهيِّئٌ استُنتج «فراغًا» — نداءُ
                //      دالّةٍ لا تُرجِع قيمةً — لا يصلح نوعَ خانةٍ ساكنًا: خانةُ Void
                //      عامّةً تُفجّر LLVM («null constant»)، ومحلّيّةً تُقرأ صفرًا كاذبًا.
                //      والمفسّرُ (المقيس) يجعل الخانةَ ديناميّةً تحمل الفراغَ:
                //      «نوع()» = فراغ، الطباعةُ «لاشيء»، و`== لاشيء` خطأ — وهي بعينِها
                //      دلالةُ خانةِ ISSUE-138 الديناميّة، فتُوسَم Any (%SadDyn).
                // (EN) SEM045 (bare-slot debt): an initializer inferred Void — a call to a
                //      value-less function — cannot type a slot statically: a Void GLOBAL
                //      trips LLVM's "null constant" UNREACHABLE and a Void LOCAL reads back
                //      a lying zero. The measured interpreter makes the slot dynamic,
                //      holding Void (نوع()=فراغ, prints «لاشيء», ==null false) — exactly
                //      the ISSUE-138 dynamic-slot contract, so it becomes Any (%SadDyn).
                if (declaredKind == Types::SadTypeKind::Unknown &&
                    resolvedKind == SadTypeKind::Void)
                {
                    return SadTypeKind::Any;
                }
                return resolvedKind;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
