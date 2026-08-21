// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4819)
#endif

/**
 * @file type_checker.cpp
 * @brief تنفيذ فاحص الأنواع / Type Checker Implementation
 * @phase Phase 5: US3 - نظام الأنواع المتقدم
 */

#include "semantic/type_checker.h"
#include "token.h"
#include "class_nodes.h"
#include "pattern_nodes.h"    // (AR) [أ-م٢] ConstructorPattern / MatchStmt / CaseClause
#include "error_manager.h"    // (AR) [أ-م٢] بناء رسالة الكتالوج ثنائيّة اللغة
#include "types/composite_type_classes.h"
#include "types/enum_types.h"
#include "types/struct_types.h"

#include <iostream>
#include <algorithm>
#include <cassert>

namespace Sad
{
    namespace Semantic
    {

        using namespace TypeSystem;
        using TT = Lexer::TokenType;

        // ============================================================================
        // TypeCheckError
        // ============================================================================

        std::string TypeCheckError::toEnglishString() const
        {
            std::ostringstream oss;
            oss << "[Type Error] ";
            if (!variableName.empty())
                oss << "'" << variableName << "': ";
            if (!message.empty())
                oss << message;
            else
                oss << "expected '" << expectedType << "' but got '" << actualType << "'";
            oss << " (line " << line << ", col " << column << ")";
            return oss.str();
        }

        std::string TypeCheckError::toArabicString() const
        {
            std::ostringstream oss;
            oss << "[خطأ نوع] ";
            if (!variableName.empty())
                oss << "'" << variableName << "': ";
            if (!arabicMessage.empty())
                oss << arabicMessage;
            else
                oss << "متوقع '" << expectedType << "' لكن وُجد '" << actualType << "'";
            oss << " (سطر " << line << ", عمود " << column << ")";
            return oss.str();
        }

        // ============================================================================
        // TypeChecker - بناء / Construction
        // ============================================================================

        TypeChecker::TypeChecker()
            : useArabicMessages_(true), debugMode_(false), strictMode_(false), registry_(TypeRegistry::getInstance()), lastInferredType_(nullptr), expectedReturnType_(nullptr)
        {
            // إنشاء بيئة الأنواع العامة / Create global type environment
            currentEnv_ = std::make_shared<TypeEnvironment>();
            scopeStack_.push_back(currentEnv_);
            // (AR) [Phase 5c] هيّئ النطاق العام لأسماء أصناف المتغيرات.
            // (EN) [Phase 5c] Initialize global scope for variable class names.
            variableClassNames_.emplace_back();

            // تسجيل الدوال المدمجة / Register built-in functions
            currentEnv_->bind("اطبع", registry_.getVoidType());        // print
            currentEnv_->bind("اطبع_سطر", registry_.getVoidType());    // println
            currentEnv_->bind("ادخل", registry_.getStringType());      // input
            currentEnv_->bind("ادخل_رقم", registry_.getIntegerType()); // input_number
            currentEnv_->bind("طول", registry_.getIntegerType());      // length
            currentEnv_->bind("نوع", registry_.getStringType());       // typeof

            // English aliases
            currentEnv_->bind("print", registry_.getVoidType());
            currentEnv_->bind("println", registry_.getVoidType());
            currentEnv_->bind("input", registry_.getStringType());
            currentEnv_->bind("len", registry_.getIntegerType());
            currentEnv_->bind("type", registry_.getStringType());
        }

        // ============================================================================
        // واجهة الفحص الرئيسية / Main Check Interface
        // ============================================================================

        TypeCheckResult TypeChecker::check(AST::ASTNode *ast)
        {
            currentResult_ = TypeCheckResult();
            // (AR) تصفير سجلّ التعدادات بين نداءات الفحص كي لا تتراكم المعاملات مضاعَفةً
            //      (REPL/وحدات متعدّدة) فتختلق التباسًا كاذبًا (🟠-٥ من مراجعة أميليا).
            // (EN) Reset the enum registry between checks so variants don't accumulate
            //      (REPL/multi-module) and fabricate false ambiguity (Amelia 🟠-5).
            enumVariants_.clear();
            variantOwners_.clear();

            if (!ast)
            {
                currentResult_.addError({"", "", "", 0, 0,
                                         "Null AST provided to type checker",
                                         "تم تمرير شجرة AST فارغة لفاحص الأنواع"});
                return currentResult_;
            }

            try
            {
                ast->accept(*this);
            }
            catch (const std::exception &e)
            {
                currentResult_.addError({"", "", "", 0, 0,
                                         std::string("Internal type checker error: ") + e.what(),
                                         std::string("خطأ داخلي في فاحص الأنواع: ") + e.what()});
            }

            if (debugMode_)
            {
                if (useArabicMessages_)
                    std::cerr << currentResult_.getArabicSummary();
                else
                    std::cerr << currentResult_.getSummary();
            }

            return currentResult_;
        }

        void TypeChecker::printSummary() const
        {
            if (useArabicMessages_)
            {
                std::cerr << currentResult_.getArabicSummary();
            }
            else
            {
                std::cerr << currentResult_.getSummary();
            }

            // طباعة الأخطاء / Print errors
            for (const auto &err : currentResult_.errors)
            {
                if (useArabicMessages_)
                {
                    std::cerr << "  ❌ خطأ نوع";
                    if (!err.variableName.empty())
                        std::cerr << " في '" << err.variableName << "'";
                    std::cerr << ": " << err.arabicMessage;
                    if (err.line > 0)
                        std::cerr << " (سطر " << err.line << ")";
                    std::cerr << "\n";
                }
                else
                {
                    std::cerr << "  ❌ Type error";
                    if (!err.variableName.empty())
                        std::cerr << " in '" << err.variableName << "'";
                    std::cerr << ": " << err.message;
                    if (err.line > 0)
                        std::cerr << " (line " << err.line << ")";
                    std::cerr << "\n";
                }
            }

            // طباعة التحذيرات / Print warnings
            for (const auto &warn : currentResult_.warnings)
            {
                std::cerr << "  ⚠️ " << warn << "\n";
            }
        }

        void TypeChecker::reset()
        {
            currentResult_ = TypeCheckResult();
            lastInferredType_ = nullptr;
            expectedReturnType_ = nullptr;
            currentFunction_.clear();
            scopeStack_.clear();
            currentEnv_ = std::make_shared<TypeEnvironment>();
            scopeStack_.push_back(currentEnv_);
            // (AR) [Phase 5c] إعادة تهيئة سجل أسماء أصناف المتغيرات.
            // (EN) [Phase 5c] Reset variable class names registry.
            variableClassNames_.clear();
            variableClassNames_.emplace_back();
        }

        // ============================================================================
        // (AR) [S-TS-P2] تحويل SadTypeKind إلى TypePtr — المحور SadTypeKind
        // (EN) [S-TS-P2] Convert SadTypeKind to TypePtr — SadTypeKind-centric
        //
        // (AR) قراءات الـAST (DataType) تُجسَّر عبر Types::fromDataType عند نقطة الاستدعاء.
        //      هذا الجسر يُحذف في S-TS-P2.5a عند ترحيل حقول الـAST إلى SadTypeKind.
        // ============================================================================

        TypePtr TypeChecker::sadKindToTypePtr(Types::SadTypeKind kind) const
        {
            using K = Types::SadTypeKind;
            switch (kind)
            {
            case K::Integer:
                return registry_.getIntegerType();
            case K::Float:
                return registry_.getFloatType();
            case K::String:
                return registry_.getStringType();
            case K::Boolean:
                return registry_.getBooleanType();
            case K::Void:
            case K::Null: // (AR) عدم — لا تمييز دلالي بعد (انظر S-TS-P9)؛ يُعامل كفراغ حاليًّا
                return registry_.getVoidType();
            case K::Array:
                return std::make_shared<ArrayType>(registry_.getAnyType()); // عنصر المصفوفة يُحدد لاحقاً
            case K::Map:
                return std::make_shared<DictionaryType>(registry_.getStringType(), registry_.getAnyType()); // مفتاح/قيمة يُحددان لاحقاً
            case K::Function:
                return std::make_shared<FunctionType>(TypeList{}, registry_.getVoidType()); // توقيع الدالة يُحدد لاحقاً
            case K::Class:
                return registry_.getAnyType(); // يُحدد من StructRegistry عند الوصول
            case K::Enum:
                return registry_.getAnyType(); // يُحدد من EnumRegistry عند الوصول
            case K::Tuple:
                return std::make_shared<TupleType>(TypeList{}); // عناصر الصف تُحدد لاحقاً
            case K::Byte:
                return registry_.getIntegerType(); // byte -> int
            case K::Error:
                return registry_.getUnknownType();
            // (AR) [S-TS-P4] أنواع متقدّمة على مستوى الـkind (دون نوع داخلي هنا) → Any
            //      حتى لا يُطلِق المدقّق تحذيرات إسناد كاذبة لـ`رقم?` ونحوه. النوع الداخلي
            //      الغنيّ يُحمَل عبر sadType في العقد (تمثيل أغنى مخطّط لاحقًا).
            case K::Optional:
            case K::Result:
            case K::Future:
            case K::Generator:
                return registry_.getAnyType();
            // (AR) 🔑 `أي` نوعٌ صريحٌ لا غيابُ نوع. كان يسقط في `default` فيُرَدَّ
            //      `Unknown`، فيُهمِله `visitVarDeclStmt` («المُصرَّحُ مجهولٌ ⇒ خُذ
            //      المُستنتَج») ويُقيَّد المتغيّرُ بنوعِ مُهيِّئه. والمقيس: `أي س = 5`
            //      ثمّ `س = "نص"` تحت `--أنواع-صارمة` تُخفِق برسالة «متوقّع 'رقم'» —
            //      **حرفًا بحرفِ** رسالةِ `متغير س = 5` ورسالةِ `رقم س = 5`. أي أنّ
            //      الفاحصَ لم يكن يرى فرقًا بين الثلاثة: كانت `أي` عندَه **تعليقًا لا
            //      نوعًا**، في الوضعِ الوحيدِ الذي يُفترَض أن تحمل فيه وزنًا دلاليًّا.
            //      ⚠️ و`areTypesCompatible` تُجيز `Any` مع كلِّ شيءٍ منذ البداية —
            //      فالبابُ كان مفتوحًا والطريقُ إليه وحدَه مقطوعًا. ولذلك لا يُقاس
            //      هذا العطبُ بقراءةِ سلطةِ التوافق: هي سليمةٌ، والعلّةُ قبلَها بخطوة.
            // (EN) `أي` is an explicit type, not the absence of one. It fell into
            //      `default` and returned Unknown, so visitVarDeclStmt overrode it with
            //      the initializer's type. Measured: `أي س = 5` then `س = "نص"` under
            //      --strict-types failed with the SAME message as `متغير` and as `رقم` —
            //      the checker saw no difference between the three. areTypesCompatible
            //      has always accepted Any; only the road to it was cut, which is why
            //      reading the compatibility authority does not expose this defect.
            case K::Any:
                return registry_.getAnyType();
            case K::Unknown:
            default:
                return registry_.getUnknownType();
            }
        }

        // (AR) جسر حدود الـAST (S-TS-P2): DataType→SadTypeKind→TypePtr. يُحذف في S-TS-P2.5a.
        // (EN) AST-boundary bridge (S-TS-P2): DataType→SadTypeKind→TypePtr. Removed in S-TS-P2.5a.
        TypePtr TypeChecker::dataTypeToTypePtr(Types::SadTypeKind dt) const
        {
            return sadKindToTypePtr(dt);
        }

        // ============================================================================
        // التحقق من توافق الأنواع / Check Type Compatibility
        // ============================================================================

        bool TypeChecker::areTypesCompatible(TypePtr expected, TypePtr actual) const
        {
            if (!expected || !actual)
                return true; // null types = no checking

            // أي نوع = Any يتوافق مع الكل / Any type is compatible with everything
            if (expected->isAny() || actual->isAny())
                return true;

            // Unknown = لم يُحدد بعد / Unknown = not yet determined
            if (expected->isUnknown() || actual->isUnknown())
                return true;

            // نفس النوع بالضبط / Exact same type
            if (expected->getKind() == actual->getKind())
                return true;

            // التحقق من التوافق الرقمي / Numeric compatibility
            if (expected->isNumeric() && actual->isNumeric())
                return true;

            // (AR) المصفوفات متوافقة مع الأعداد (التمثيل الداخلي i64 pointer)
            //      ومتوافقة مع أي نوع مصفوفة آخر (بغض النظر عن حجم العناصر)
            // (EN) Arrays are compatible with integers (internal i64 pointer representation)
            //      and with any other array type (regardless of element count)
            if (expected->isArray() || actual->isArray())
                return true;

            // Never يتوافق مع أي نوع / Never is subtype of everything
            if (actual->isNever())
                return true;

            // استخدم isAssignableTo / Use isAssignableTo
            return actual->isAssignableTo(expected.get());
        }

        // ============================================================================
        // استنتاج نوع تعبير / Infer Expression Type
        // ============================================================================

        TypePtr TypeChecker::inferExprType(AST::Expression *expr)
        {
            if (!expr)
                return registry_.getUnknownType();

            lastInferredType_ = nullptr;
            expr->accept(*this);

            TypePtr result = lastInferredType_;
            if (!result)
            {
                // (AR) Fallback: نوع الـAST (DataType) مُجسَّرًا إلى SadTypeKind — S-TS-P2
                //      (الجسر يُحذف في S-TS-P2.5a عند ترحيل حقول الـAST).
                // (EN) Fallback: AST DataType bridged to SadTypeKind — S-TS-P2.
                result = sadKindToTypePtr(expr->getTypeKind());
            }

            currentResult_.totalInferred++;
            return result;
        }

        // ============================================================================
        // إدارة النطاقات / Scope Management
        // ============================================================================

        void TypeChecker::enterScope()
        {
            // إنشاء بيئة فرعية مع تمرير الأب / Create child env with parent
            // ملاحظة: لا نستخدم createChild() لأنها لا تمرر الأب بشكل صحيح
            auto childEnv = std::make_shared<TypeEnvironment>(currentEnv_);
            scopeStack_.push_back(childEnv);
            currentEnv_ = childEnv;
            // (AR) [Phase 5c] احفظ نطاق متوازٍ لأسماء أصناف المتغيرات.
            // (EN) [Phase 5c] Push parallel scope for variable class names.
            variableClassNames_.emplace_back();
        }

        void TypeChecker::exitScope()
        {
            if (scopeStack_.size() > 1)
            {
                scopeStack_.pop_back();
                currentEnv_ = scopeStack_.back();
            }
            // (AR) [Phase 5c] أخرج النطاق المتوازي.
            // (EN) [Phase 5c] Pop the parallel scope.
            if (!variableClassNames_.empty())
                variableClassNames_.pop_back();
        }

        void TypeChecker::declareVariable(const std::string &name, TypePtr type)
        {
            currentEnv_->bind(name, type);
        }

        TypePtr TypeChecker::lookupVariable(const std::string &name) const
        {
            auto type = currentEnv_->lookup(name);
            return type ? type : nullptr;
        }

        // ============================================================================
        // مساعدات / Helpers
        // ============================================================================

        std::pair<size_t, size_t> TypeChecker::getLocation(AST::ASTNode *node) const
        {
            if (!node)
                return {0, 0};
            auto &pos = node->position;
            return {pos.line, pos.column};
        }

        void TypeChecker::recordTypeError(const std::string &varName,
                                          const std::string &expected,
                                          const std::string &actual,
                                          AST::ASTNode *node,
                                          const std::string &msg)
        {
            auto [line, col] = getLocation(node);

            TypeCheckError err;
            err.variableName = varName;
            err.expectedType = expected;
            err.actualType = actual;
            err.line = line;
            err.column = col;

            if (msg.empty())
            {
                err.message = "Type mismatch: expected '" + expected + "' but got '" + actual + "'";
                err.arabicMessage = "عدم تطابق الأنواع: متوقع '" + expected + "' لكن وُجد '" + actual + "'";
            }
            else
            {
                err.message = msg;
                err.arabicMessage = msg;
            }

            currentResult_.addError(err);
        }

        // ============================================================================
        // (AR) [أ-م٢] مساعدات التعداد بحمولة (ADT) / (EN) [A-M2] Tagged-enum helpers
        // ============================================================================

        const TypeChecker::EnumVariantInfo *
        TypeChecker::lookupVariant(const std::string &variantName,
                                   std::string &owningEnumOut,
                                   bool &ambiguousOut) const
        {
            ambiguousOut = false;
            owningEnumOut.clear();

            auto ownersIt = variantOwners_.find(variantName);
            if (ownersIt == variantOwners_.end() || ownersIt->second.empty())
            {
                return nullptr;
            }
            // (AR) معامل مشترك بين أكثر من تعداد ⇒ غامض (نأخذ الأوّل لكن نُعلِم المُستدعي).
            // (EN) Variant shared by more than one enum ⇒ ambiguous (take first, flag caller).
            ambiguousOut = ownersIt->second.size() > 1;
            const std::string &enumName = ownersIt->second.front();
            owningEnumOut = enumName;

            auto enumIt = enumVariants_.find(enumName);
            if (enumIt == enumVariants_.end())
            {
                return nullptr;
            }
            for (const auto &v : enumIt->second)
            {
                if (v.variantName == variantName)
                {
                    return &v;
                }
            }
            return nullptr;
        }

        void TypeChecker::reportCatalogError(
            Errors::ErrorCode code,
            const std::map<std::string, std::string> &placeholders,
            AST::ASTNode *node)
        {
            auto [line, col] = getLocation(node);

            // (AR) رمز الخطأ من كتالوج مصدر الحقيقة حصرًا؛ الرسالة تُبنى من القالب.
            // (EN) Error code from the SoT catalog exclusively; message built from template.
            Errors::RenderContext ctx;
            ctx.location = Errors::SourceLocation("<input>", line, col);
            ctx.placeholders = placeholders;

            const std::string rendered =
                Errors::ErrorManager::getInstance().buildBilingualMessage(code, ctx);

            TypeCheckError err;
            err.line = line;
            err.column = col;
            err.message = rendered;
            err.arabicMessage = rendered;
            currentResult_.addError(err);
        }

        TypePtr TypeChecker::checkEnumConstruction(
            const std::string &variantName,
            const EnumVariantInfo &info,
            const std::vector<AST::ExprPtr> &args,
            AST::ASTNode *node)
        {
            // (AR) فحص عدد الحمولة — نعيد استعمال رمز الكتالوج القائم SEM_WRONG_ARG_COUNT.
            // (EN) Payload arity check — reuse the existing SEM_WRONG_ARG_COUNT catalog code.
            const size_t expected = info.fieldTypes.size();
            const size_t found = args.size();
            if (found != expected)
            {
                reportCatalogError(
                    Errors::ErrorCode::SEM_WRONG_ARG_COUNT,
                    {{"name", variantName},
                     {"expected", std::to_string(expected)},
                     {"found", std::to_string(found)}},
                    node);
            }
            else
            {
                // (AR) فحص أنواع الحمولة عبر fieldTypes — للوسائط المصرَّح نوعها بنوعٍ
                //      مدمجٍ معروف فقط (رقم/عشري/نص/منطقي). الحقول غير المُصنَّفة ("")
                //      أو أنواع الأصناف تُتجاوز في أ-م٢ (استدلالها غير موثوق بعد).
                // (EN) Payload type check via fieldTypes — only for args whose declared
                //      field type is a known built-in (رقم/عشري/نص/منطقي). Untyped ("")
                //      or class-typed fields are skipped in A-M2 (inference not reliable yet).
                for (size_t i = 0; i < expected; ++i)
                {
                    const std::string &ftName = info.fieldTypes[i];
                    if (ftName.empty() || !args[i])
                        continue;

                    // (AR) خرائط اسم النوع المدمج → SadTypeKind (تجاوز غير المعروف).
                    // (EN) Map built-in type name → SadTypeKind (skip if unknown).
                    using K = Types::SadTypeKind;
                    K expKind;
                    if (ftName == "رقم")
                        expKind = K::Integer;
                    else if (ftName == "عشري")
                        expKind = K::Float;
                    else if (ftName == "نص")
                        expKind = K::String;
                    else if (ftName == "منطقي")
                        expKind = K::Boolean;
                    else
                        continue; // (AR) نوع صنف/غير مدمج / (EN) class/non-built-in type

                    TypePtr argT = inferExprType(args[i].get());
                    if (!argT || argT->isUnknown() || argT->getKind() == SadTypeKind::Any)
                        continue; // (AR) نوع الوسيط غير معروف / (EN) unknown arg type

                    TypePtr expT = sadKindToTypePtr(expKind);
                    if (expT && !areTypesCompatible(expT, argT))
                    {
                        reportCatalogError(
                            Errors::ErrorCode::SEM_TYPE_MISMATCH,
                            {{"expected", expT->toString()},
                             {"found", argT->toString()}},
                            node);
                    }
                }
            }

            // (AR) نوع القيمة المُنشأة: نستعمل Class مبدئيًّا (لا EnumKind بعد) — نظير
            //      visitEnumDecl. codegen الاتّحاد الموسوم مؤجَّل لـأ-م٤.
            // (EN) Constructed value's type: Class placeholder (no EnumKind yet) — mirrors
            //      visitEnumDecl. Tagged-union codegen deferred to A-M4.
            return registry_.internPrimitiveType(SadTypeKind::Class);
        }

        // ============================================================================
        // زيارة التعابير / Visit Expressions
        // ============================================================================

        void TypeChecker::visitLiteralExpr(AST::LiteralExpr &expr)
        {
            currentResult_.totalExpressions++;

            switch (expr.token.getType())
            {
            case TT::NUMBER_INTEGER:
                lastInferredType_ = registry_.getIntegerType();
                break;
            case TT::NUMBER_DOUBLE:
                lastInferredType_ = registry_.getFloatType();
                break;
            case TT::STRING_LITERAL:
                lastInferredType_ = registry_.getStringType();
                break;
            case TT::LITERAL_TRUE:
            case TT::LITERAL_FALSE:
                lastInferredType_ = registry_.getBooleanType();
                break;
            case TT::LITERAL_NULL:
                // (AR) لاشيء = nullable — متوافق مع أي نوع لاحق عند إعادة الإسناد
                // (EN) null literal = nullable — compatible with any type on reassignment
                lastInferredType_ = registry_.getUnknownType();
                break;
            default:
                lastInferredType_ = registry_.getUnknownType();
                break;
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] Literal: " << (lastInferredType_ ? lastInferredType_->toString() : "null") << "\n";
            }
        }

        void TypeChecker::visitVariableExpr(AST::VariableExpr &expr)
        {
            currentResult_.totalExpressions++;

            auto type = lookupVariable(expr.name);
            if (type)
            {
                lastInferredType_ = type;
            }
            else
            {
                // متغير غير معرّف — ليس خطأ أنواع بل خطأ دلالي
                // Undeclared variable — not a type error, semantic error
                lastInferredType_ = registry_.getUnknownType();

                if (strictMode_)
                {
                    recordTypeError(expr.name, "", "", &expr,
                                    "Undeclared variable '" + expr.name + "'");
                }
            }

            if (debugMode_)
            {
                std::cerr << "  [TC] Variable '" << expr.name << "': "
                          << (lastInferredType_ ? lastInferredType_->toString() : "unknown") << "\n";
            }
        }

        void TypeChecker::visitBinaryExpr(AST::BinaryExpr &expr)
        {
            currentResult_.totalExpressions++;

            TypePtr leftType = inferExprType(expr.left.get());
            TypePtr rightType = inferExprType(expr.right.get());

            // (AR) SEM045 (D8): «فراغ» ناتجَ **نداءٍ** طرفًا في عملية **حسابية**
            //      يُرفض هنا لا في الإسناد وحده — `رقم س = لا_شيء() + 1` كانت تمرّ
            //      صامتةً (الجمعُ يُسوّي الفراغَ Unknown فيطابق الخانة — قِيس في
            //      المراجعة العدائية) بينما المفسّر يرفضها وقت التشغيل. الرفضُ
            //      مقصورٌ على `CallExpr`: وصولُ العضو (`هذا.س`) قد يُستنتَج «فراغًا»
            //      أثريًّا فأطلق الصياغةَ الأولى كذبًا على بذرة تحميل العوامل (قِيس).
            //      والمقارناتُ خارجُ الرفض (عقد `فراغ == لاشيء` ⇒ «خطأ»، البذرة 100).
            // (EN) SEM045 (D8): a CALL result typed Void as an ARITHMETIC operand is
            //      rejected here — `رقم س = لا_شيء() + 1` compiled silently
            //      (measured). Restricted to CallExpr: member access may infer Void
            //      as an artifact (measured false positive on the operator-overload
            //      seed). Comparisons stay out (seed 100 pins `فراغ == لاشيء`).
            {
                const bool arithmeticOp =
                    expr.op == TT::OP_PLUS || expr.op == TT::OP_MINUS ||
                    expr.op == TT::OP_MULTIPLY || expr.op == TT::OP_DIVIDE ||
                    expr.op == TT::OP_FLOOR_DIVIDE || expr.op == TT::OP_MODULO;
                const bool voidOperand =
                    (leftType && leftType->getKind() == Types::SadTypeKind::Void &&
                     dynamic_cast<AST::CallExpr *>(expr.left.get()) != nullptr) ||
                    (rightType && rightType->getKind() == Types::SadTypeKind::Void &&
                     dynamic_cast<AST::CallExpr *>(expr.right.get()) != nullptr);
                if (arithmeticOp && voidOperand)
                {
                    // (AR) الرسالة الافتراضية «متوقع … وُجد …» (الوسيط الخامس يطغى
                    //      على العربية — قِيس، فتُرك فارغًا).
                    recordTypeError("", useArabicMessages_ ? "قيمة" : "a value",
                                    leftType && leftType->getKind() == Types::SadTypeKind::Void
                                        ? leftType->toString()
                                        : rightType->toString(),
                                    &expr);
                    lastInferredType_ = registry_.getUnknownType();
                    return;
                }
            }

            switch (expr.op)
            {
            // عمليات حسابية / Arithmetic
            case TT::OP_PLUS:
                // يمكن أن تكون جمع أرقام أو دمج نصوص
                if (leftType && rightType)
                {
                    if (leftType->isString() || rightType->isString())
                    {
                        lastInferredType_ = registry_.getStringType();
                    }
                    else if (leftType->isNumeric() && rightType->isNumeric())
                    {
                        // float يسود / float dominates
                        if (leftType->isFloat() || rightType->isFloat())
                        {
                            lastInferredType_ = registry_.getFloatType();
                        }
                        else
                        {
                            lastInferredType_ = registry_.getIntegerType();
                        }
                    }
                    else
                    {
                        lastInferredType_ = registry_.getUnknownType();
                        if (strictMode_)
                        {
                            recordTypeError("",
                                            "numeric or string",
                                            leftType->toString() + " + " + rightType->toString(),
                                            &expr, "Cannot add incompatible types");
                        }
                    }
                }
                else
                {
                    lastInferredType_ = registry_.getUnknownType();
                }
                break;

            case TT::OP_MINUS:
            case TT::OP_MULTIPLY:
            case TT::OP_DIVIDE:
            case TT::OP_FLOOR_DIVIDE:
            case TT::OP_MODULO:
                // عمليات حسابية تتطلب أرقام / Arithmetic requires numbers
                if (leftType && rightType)
                {
                    if (leftType->isNumeric() && rightType->isNumeric())
                    {
                        if (expr.op == TT::OP_FLOOR_DIVIDE)
                        {
                            // (AR) القسمة الصحيحة // دائماً تنتج عدد صحيح
                            lastInferredType_ = registry_.getIntegerType();
                        }
                        else if (leftType->isFloat() || rightType->isFloat() || expr.op == TT::OP_DIVIDE)
                        {
                            lastInferredType_ = registry_.getFloatType();
                        }
                        else
                        {
                            lastInferredType_ = registry_.getIntegerType();
                        }
                    }
                    else
                    {
                        lastInferredType_ = registry_.getUnknownType();
                        if (strictMode_)
                        {
                            recordTypeError("",
                                            "numeric",
                                            leftType->toString() + " op " + rightType->toString(),
                                            &expr, "Arithmetic operation requires numeric operands");
                        }
                    }
                }
                else
                {
                    lastInferredType_ = registry_.getUnknownType();
                }
                break;

            // عمليات المقارنة / Comparison
            case TT::OP_EQUAL:
            case TT::OP_NOT_EQUAL:
            case TT::OP_LESS:
            case TT::OP_LESS_EQUAL:
            case TT::OP_GREATER:
            case TT::OP_GREATER_EQUAL:
                lastInferredType_ = registry_.getBooleanType();
                break;

            // عمليات منطقية / Logical
            case TT::OP_AND:
            case TT::OP_OR:
                lastInferredType_ = registry_.getBooleanType();
                break;

            default:
                lastInferredType_ = registry_.getUnknownType();
                break;
            }
        }

        void TypeChecker::visitUnaryExpr(AST::UnaryExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr operandType = inferExprType(expr.operand.get());

            switch (expr.op)
            {
            case TT::OP_MINUS:
                if (operandType && operandType->isNumeric())
                {
                    lastInferredType_ = operandType;
                }
                else
                {
                    lastInferredType_ = registry_.getUnknownType();
                    if (strictMode_)
                    {
                        recordTypeError("", "numeric",
                                        operandType ? operandType->toString() : "unknown",
                                        &expr, "Unary minus requires numeric operand");
                    }
                }
                break;

            case TT::OP_NOT:
                lastInferredType_ = registry_.getBooleanType();
                break;

            default:
                lastInferredType_ = operandType ? operandType : registry_.getUnknownType();
                break;
            }
        }

        void TypeChecker::visitTernaryExpr(AST::TernaryExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr condType = inferExprType(expr.condition.get());
            TypePtr trueType = inferExprType(expr.trueExpr.get());
            TypePtr falseType = inferExprType(expr.falseExpr.get());

            // الشرط يجب أن يكون منطقي / Condition should be boolean
            if (strictMode_ && condType && !condType->isBoolean() && !condType->isUnknown())
            {
                recordTypeError("", "boolean", condType->toString(), &expr,
                                "Ternary condition should be boolean");
            }

            // إذا تطابق الفرعين / If branches match
            if (trueType && falseType && trueType->getKind() == falseType->getKind())
            {
                lastInferredType_ = trueType;
            }
            else
            {
                // Union type would be ideal, fall back to broader type
                lastInferredType_ = trueType ? trueType : registry_.getUnknownType();
            }
        }

        void TypeChecker::visitAssignExpr(AST::AssignExpr &expr)
        {
            currentResult_.totalExpressions++;
            TypePtr valueType = inferExprType(expr.value.get());

            auto varType = lookupVariable(expr.name);
            if (varType && valueType)
            {
                // ================================================================
                // (AR) [Fix #53] لغة ص ديناميكية — إعادة الإسناد عبر الأنواع مسموحة:
                //      متغير س = 42      →  نوع: رقم
                //      س = "نص الآن"      →  نوع: نص (مسموح — ديناميكية الأنواع)
                //      في الوضع الصارم (strictMode_) فقط نُبلّغ عن عدم التطابق.
                //      بدون هذا: المترجم يرفض إعادة الإسناد ولا يُنشئ ملف تنفيذي.
                // (EN) [Fix #53] Sad is dynamically typed — cross-type reassignment is allowed:
                //      Only report mismatch in strict mode.
                //      Without this: compiler rejects reassignment and fails to compile.
                // ================================================================
                if (strictMode_ && !areTypesCompatible(varType, valueType))
                {
                    recordTypeError(expr.name,
                                    varType->toString(), valueType->toString(),
                                    &expr);
                }

                // (AR) تحديث نوع المتغير في بيئة الأنواع ليعكس النوع الجديد
                // (EN) Update variable type in type environment to reflect new type
                // ================================================================
                // (AR) ⚠️ ولا يضيقُ `أي` بأوّلِ إسناد: التتبّعُ التدفّقيُّ صوابٌ لخانةٍ
                //      استُنتِج نوعُها، وخطأٌ لخانةٍ **صرّح كاتبُها** أنّها ديناميّة.
                //      ولولا هذا القيدُ لصحَّ `أي س = 5` ثمّ `س = "نص"` وأخفق الثالثُ
                //      `س = 7` — فيصير قبولُ `أي` رهنَ **عددِ** الإسناداتِ لا معناها،
                //      وهو أسوأُ من الرفضِ الصريح لأنّه يُقرأ عشوائيًّا.
                //      والفحصُ على النوعِ المربوطِ لا على تصريحٍ محفوظ، فالفاحصُ لا
                //      يحتفظ بالمُصرَّح؛ وأثرُه لا يزيد على تعطيلِ تضييقٍ، فلا يُنشئ
                //      إخفاقًا جديدًا في أيِّ حال.
                // (EN) An `أي` slot must not be narrowed by its first assignment: flow
                //      tracking is right for an inferred slot and wrong for one the
                //      author declared dynamic. Otherwise the 2nd assignment passes and
                //      the 3rd fails — acceptance by assignment COUNT, which reads as
                //      random. The test is on the bound type (the checker keeps no record
                //      of the declaration); it can only disable a narrowing, never create
                //      a new failure.
                // ================================================================
                if (valueType && !valueType->isUnknown() && !(varType && varType->isAny()))
                {
                    currentEnv_->bind(expr.name, valueType);
                }
            }

            lastInferredType_ = valueType;
        }

        void TypeChecker::visitCallExpr(AST::CallExpr &expr)
        {
            currentResult_.totalExpressions++;

            // تحقق من أنواع المعاملات / Check argument types
            for (auto &arg : expr.arguments)
            {
                if (arg)
                    inferExprType(arg.get());
            }

            // ============================================================
            // (AR) [أ-م٢] حسم هويّة بناء معامل تعداد بحمولة: «عدد(٥)» يُحلَّل نحويًّا
            //      كاستدعاء دالّة، لكن إن كان اسم المُستدعى معامل تعداد معروفًا فهو
            //      بناء قيمة موسومة (ADT) لا استدعاء دالّة غير معرّفة. نفحص الحمولة
            //      (عددًا/أنواعًا) ونثبّت النوع المُستنتَج ونعود مبكّرًا (لا نعامله دالّة).
            //      بناء القيمة الفعليّة (الاتّحاد الموسوم) في المفسّر/codegen (أ-م٣/أ-م٤).
            // (EN) [A-M2] Resolve tagged-enum variant construction: «عدد(5)» parses as a
            //      function call, but if the callee name is a known enum variant it is a
            //      tagged (ADT) value construction, not an undefined function call. We check
            //      the payload (arity/types), set the inferred type, and return early.
            //      Actual value building (tagged union) is in interpreter/codegen (A-M3/A-M4).
            // ============================================================
            if (auto *calleeVar = dynamic_cast<AST::VariableExpr *>(expr.callee.get()))
            {
                // (AR) المُعامِل يخسر التنازع أمام دالّة/رمزٍ مُصرَّحٍ صراحةً (🔴-٢ من مراجعة أميليا):
                //      لا نختطف نداءً إلى دالّة مستخدم اسمُها يطابق اسم معامل تعداد.
                // (EN) A variant loses to an explicitly declared function/symbol (Amelia 🔴-2):
                //      do not hijack a call to a user function whose name matches a variant.
                TypeSystem::TypePtr declared = lookupVariable(calleeVar->name);
                const bool isDeclaredCallable =
                    declared && std::dynamic_pointer_cast<TypeSystem::FunctionType>(declared) != nullptr;
                if (!isDeclaredCallable)
                {
                    std::string owningEnum;
                    bool ambiguous = false;
                    const EnumVariantInfo *variant =
                        lookupVariant(calleeVar->name, owningEnum, ambiguous);
                    if (variant)
                    {
                        lastInferredType_ = checkEnumConstruction(
                            calleeVar->name, *variant, expr.arguments, &expr);
                        return;
                    }
                }
            }

            // ============================================================
            // (AR) [Phase 5b] استدلال نوعي للقوالب: إن كان الاستدعاء بدون
            //      <...> لكنه يستهدف اسم قالب معروف، حاول استنتاج
            //      وسائط الأنواع من أنواع الوسائط المُمرَّرة وفرض القيود.
            // (EN) [Phase 5b] Template type-arg inference: if the call has
            //      no <...> but targets a known template name, try to
            //      infer type args from passed argument types and enforce
            //      the corresponding constraints.
            // ============================================================
            if (auto *idExpr = dynamic_cast<AST::VariableExpr *>(expr.callee.get()))
            {
                auto tIt = userTemplates_.find(idExpr->name);
                if (tIt != userTemplates_.end())
                {
                    const auto &tmpl = tIt->second;

                    // (AR) اجمع أسماء أنواع الوسائط بشكل آمن.
                    // (EN) Collect argument type-name strings safely.
                    std::vector<std::string> argTypeNames;
                    argTypeNames.reserve(expr.arguments.size());
                    for (auto &a : expr.arguments)
                        argTypeNames.push_back(extractTypeNameFromExpr(a.get()));

                    std::vector<std::string> inferred;
                    if (inferTemplateTypeArguments(tmpl, argTypeNames, inferred))
                    {
                        // (AR) فرض القيود على كل معامل نوعي مُستدلّ.
                        // (EN) Enforce constraints on each inferred type param.
                        std::vector<const ParamInfo *> typeParams;
                        for (const auto &p : tmpl.typeParameters)
                        {
                            if (!p.isConst)
                                typeParams.push_back(&p);
                        }
                        size_t n = std::min(typeParams.size(), inferred.size());
                        for (size_t i = 0; i < n; ++i)
                        {
                            enforceParamConstraints(*typeParams[i],
                                                    inferred[i],
                                                    expr.position);
                        }
                        // (AR) فرض جملة حيث (where) أيضاً.
                        // (EN) Enforce where clause too.
                        for (const auto &item : tmpl.whereItems)
                        {
                            int idx = -1;
                            for (size_t k = 0; k < typeParams.size(); ++k)
                            {
                                if (typeParams[k]->name == item.typeName)
                                {
                                    idx = static_cast<int>(k);
                                    break;
                                }
                            }
                            if (idx < 0 || idx >= static_cast<int>(inferred.size()))
                                continue;
                            ParamInfo synthetic;
                            synthetic.name = item.typeName;
                            synthetic.isConst = false;
                            synthetic.constraints = item.constraints;
                            enforceParamConstraints(synthetic,
                                                    inferred[idx],
                                                    expr.position);
                        }

                        if (debugMode_)
                        {
                            std::cerr << "  [TC][5b] Inferred template '"
                                      << idExpr->name << "' type args:";
                            for (const auto &t : inferred)
                                std::cerr << " " << t;
                            std::cerr << "\n";
                        }
                    }
                }
            }

            // استنتج نوع الإرجاع من المُستدعى / Infer return type from callee
            TypePtr calleeType = inferExprType(expr.callee.get());

            // إذا كان CalleeType function type، استخرج نوع الإرجاع
            if (calleeType && calleeType->getKind() == SadTypeKind::Function)
            {
                auto *fnType = static_cast<FunctionType *>(calleeType.get());

                // (AR) [Zone 2] فحص أنواع الوسائط مقابل أنواع المعاملات المُصرَّحة.
                //      نُقارن عدد الوسائط وأنواعها. نتجاهل المعاملات ذات النوع UNKNOWN/ANY.
                // (EN) [Zone 2] Check argument types against declared parameter types.
                //      Compare argument count and types. Skip UNKNOWN/ANY params.
                const auto &paramTypes = fnType->getParamTypes();
                size_t nArgs = expr.arguments.size();
                size_t nParams = paramTypes.size();
                for (size_t i = 0; i < nArgs && i < nParams; ++i)
                {
                    if (!expr.arguments[i])
                        continue;
                    TypePtr paramT = paramTypes[i];
                    TypePtr argT = inferExprType(expr.arguments[i].get());
                    // (AR) تجاهل المعاملات غير المعروفة الأنواع أو ANY
                    // (EN) Skip unknown/any parameter types
                    if (!paramT || paramT->isUnknown() || paramT->getKind() == SadTypeKind::Any)
                        continue;
                    // (AR) تجاهل الوسائط غير المعروفة الأنواع
                    // (EN) Skip unknown argument types
                    if (!argT || argT->isUnknown() || argT->getKind() == SadTypeKind::Any)
                        continue;
                    if (!areTypesCompatible(paramT, argT))
                    {
                        std::string funcName;
                        if (auto *idExpr = dynamic_cast<AST::VariableExpr *>(expr.callee.get()))
                            funcName = idExpr->name;
                        recordTypeError(funcName,
                                        paramT->toString(), argT->toString(),
                                        &expr,
                                        "Argument type mismatch in call to '" + funcName +
                                            "': parameter " + std::to_string(i + 1));
                    }
                }

                TypePtr retType = fnType->getReturnType();
                // (AR) إذا لم يُحدَّد نوع الإرجاع، نفترض رقم (مثل int في C)
                // (EN) If return type is unspecified, default to integer (like C's implicit int)
                lastInferredType_ = retType ? retType : registry_.getIntegerType();
            }
            else
            {
                lastInferredType_ = calleeType ? calleeType : registry_.getUnknownType();
            }
        }

    } // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
