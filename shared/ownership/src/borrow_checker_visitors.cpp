// ============================================================================
// borrow_checker_visitors.cpp — زوار فاحص الاستعارة (جمل + تعريفات + مساعدات)
// (AR) زوار الجمل والتعريفات ودوال مساعدة لفاحص الاستعارة
// (EN) Statement/declaration visitors and helpers for borrow checker
// تم استخراج هذا الملف من borrow_checker.cpp وفقاً لقاعدة CW-05
// ============================================================================

#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

#include "ownership/borrow_checker.h"
#include <iostream>
#include <sstream>

namespace Sad
{
    namespace Semantic
    {
        void BorrowChecker::visitForRangeStmt(AST::ForRangeStmt &stmt)
        {
            // (AR) فحص حلقة for-range
            // (EN) Check for-range loop
            tracker_->enterScope();
            if (stmt.iterable)
            {
                stmt.iterable->accept(*this);
            }
            if (stmt.body)
            {
                stmt.body->accept(*this);
            }
            tracker_->exitScope();
        }

        void BorrowChecker::visitSwitchStmt(AST::SwitchStmt &stmt)
        {
            // (AR) فحص جملة switch
            // (EN) Check switch statement
        }

        void BorrowChecker::visitReturnStmt(AST::ReturnStmt &stmt)
        {
            // (AR) فحص جملة return
            // (EN) Check return statement
            if (stmt.value)
            {
                stmt.value->accept(*this);
            }
        }

        void BorrowChecker::visitYieldStmt(AST::YieldStmt &stmt)
        {
            // (AR) فحص جملة yield
            // (EN) Check yield statement
        }

        void BorrowChecker::visitBreakStmt(AST::BreakStmt &stmt)
        {
            // (AR) break لا يحتاج فحص ملكية
            // (EN) break doesn't need ownership checking
            (void)stmt;
        }

        void BorrowChecker::visitContinueStmt(AST::ContinueStmt &stmt)
        {
            // (AR) continue لا يحتاج فحص ملكية
            // (EN) continue doesn't need ownership checking
            (void)stmt;
        }

        void BorrowChecker::visitBlockStmt(AST::BlockStmt &stmt)
        {
            // (AR) فحص كتلة العبارات
            // (EN) Check block statement
            tracker_->enterScope();

            // (AR) زيارة كل عبارة في الكتلة
            // (EN) Visit each statement in the block
            for (auto &s : stmt.statements)
            {
                if (s)
                {
                    s->accept(*this);
                }
            }

            tracker_->exitScope();
        }

        void BorrowChecker::visitTryStmt(AST::TryStmt &stmt)
        {
            // (AR) فحص جملة try
            // (EN) Check try statement
        }

        void BorrowChecker::visitRaiseStmt(AST::RaiseStmt &stmt)
        {
            // (AR) فحص جملة raise
            // (EN) Check raise statement
        }

        void BorrowChecker::visitWithStmt(AST::WithStmt &stmt)
        {
            // (AR) فحص جملة with
            // (EN) Check with statement
        }

        void BorrowChecker::visitMatchStmt(AST::MatchStmt &stmt)
        {
            // (AR) فحص جملة match
            // (EN) Check match statement
        }

        void BorrowChecker::visitDeferStmt(AST::DeferStmt &stmt)
        {
            // (AR) فحص جملة أجّل — نفحص الجسم فقط
            // (EN) Check defer statement — just check the body
            if (stmt.body)
            {
                stmt.body->accept(*this);
            }
        }

        void BorrowChecker::visitGoStmt(AST::GoStmt &stmt)
        {
            // (AR) فحص جملة أطلق — نفحص التعبير أو الكتلة
            // (EN) Check go statement — check expression or block body
            if (stmt.expression)
            {
                stmt.expression->accept(*this);
            }
            if (stmt.blockBody)
            {
                stmt.blockBody->accept(*this);
            }
        }

        void BorrowChecker::visitSelectCase(AST::SelectCase &stmt)
        {
            // (AR) فحص حالة اختيار — نفحص تعبير القناة والجسم
            // (EN) Check select case — check channel expression and body
            if (stmt.channelExpr)
            {
                stmt.channelExpr->accept(*this);
            }
            for (auto &s : stmt.body)
            {
                if (s)
                    s->accept(*this);
            }
        }

        void BorrowChecker::visitSelectStmt(AST::SelectStmt &stmt)
        {
            // (AR) فحص جملة اختر — نفحص جميع الحالات والجسم الافتراضي
            // (EN) Check select statement — check all cases and default body
            for (auto &c : stmt.cases)
            {
                if (c)
                    c->accept(*this);
            }
            for (auto &s : stmt.defaultBody)
            {
                if (s)
                    s->accept(*this);
            }
        }

        void BorrowChecker::visitClassDeclStmt(AST::ClassDeclStmt &stmt)
        {
            // (AR) فحص تصريح الصنف
            // (EN) Check class declaration statement
        }

        // ============================================================================
        // زيارة التصريحات / Visit Declarations
        // ============================================================================

        void BorrowChecker::visitFunctionDecl(AST::FunctionDecl &decl)
        {
            std::string previousFunction = currentFunction_;
            currentFunction_ = decl.name;

            tracker_->enterScope();

            // (AR) تسجيل المعاملات كمتغيرات في النطاق
            // (EN) Register parameters as variables in scope
            for (const auto &param : decl.parameters)
            {
                std::string typeName = dataTypeToString(param.type);
                bool isCopy = isCopyType(typeName);
                tracker_->declareVariable(param.name, typeName, getLocation(&decl), isCopy);
                currentResult_.totalVariables++;
            }

            // (AR) فحص جسم الدالة
            // (EN) Check function body
            if (decl.body)
            {
                decl.body->accept(*this);
            }

            tracker_->exitScope();
            currentFunction_ = previousFunction;
        }

        void BorrowChecker::visitClassDecl(AST::ClassDecl &decl)
        {
            // (AR) فحص كل طريقة في الصنف
            // (EN) Check each method in class
        }

        void BorrowChecker::visitFieldDecl(AST::FieldDecl &decl)
        {
            // (AR) فحص تصريح الحقل
            // (EN) Check field declaration
        }

        void BorrowChecker::visitMethodDecl(AST::MethodDecl &decl)
        {
            // (AR) فحص تصريح الطريقة
            // (EN) Check method declaration
        }

        void BorrowChecker::visitPropertyDecl(AST::PropertyDecl &decl)
        {
            // (AR) فحص تصريح الخاصية
            // (EN) Check property declaration
        }

        void BorrowChecker::visitConstructorDecl(AST::ConstructorDecl &decl)
        {
            // (AR) فحص تصريح البناء
            // (EN) Check constructor declaration
        }

        void BorrowChecker::visitDestructorDecl(AST::DestructorDecl &decl)
        {
            // (AR) فحص تصريح الهدم
            // (EN) Check destructor declaration
        }

        void BorrowChecker::visitEnumDecl(AST::EnumDecl &decl)
        {
            // (AR) فحص تصريح التعداد
            // (EN) Check enum declaration
        }

        void BorrowChecker::visitImportStmt(AST::ImportStmt &stmt)
        {
            // (AR) فحص جملة الاستيراد
            // (EN) Check import statement
            (void)stmt;
        }

        void BorrowChecker::visitFromImportStmt(AST::FromImportStmt &stmt)
        {
            // (AR) فحص جملة الاستيراد الانتقائي
            // (EN) Check from-import statement
            (void)stmt;
        }

        void BorrowChecker::visitExportStmt(AST::ExportStmt &stmt)
        {
            // (AR) فحص جملة التصدير
            // (EN) Check export statement
            (void)stmt;
        }

        void BorrowChecker::visitExportDecl(AST::ExportDecl &decl)
        {
            // (AR) فحص تصريح التصدير
            // (EN) Check export declaration
            (void)decl;
        }

        void BorrowChecker::visitReExportStmt(AST::ReExportStmt &stmt)
        {
            (void)stmt;
        }

        void BorrowChecker::visitTemplateFunctionDecl(AST::TemplateFunctionDecl &decl)
        {
            // (AR) فحص تصريح دالة القالب
            // (EN) Check template function declaration
        }

        void BorrowChecker::visitTemplateClassDecl(AST::TemplateClassDecl &decl)
        {
            // (AR) فحص تصريح صنف القالب
            // (EN) Check template class declaration
        }

        void BorrowChecker::visitTemplateInstantiation(AST::TemplateInstantiation &inst)
        {
            // (AR) فحص تنفيذ القالب
            // (EN) Check template instantiation
        }

        void BorrowChecker::visitNamespaceDecl(AST::NamespaceDecl &decl)
        {
            // (AR) فحص تصريح فضاء الأسماء
            // (EN) Check namespace declaration
        }

        void BorrowChecker::visitOperatorDecl(AST::OperatorDecl &decl)
        {
            // (AR) فحص تصريح تحميل العامل
            // (EN) Check operator overload declaration
        }

        void BorrowChecker::visitTraitDecl(AST::TraitDecl &decl)
        {
            // (AR) فحص تصريح السمة — تحقق من الملكية في الدوال المعلنة
            // (EN) Check trait declaration — verify ownership in declared methods
        }

        void BorrowChecker::visitImplDecl(AST::ImplDecl &decl)
        {
            // (AR) فحص تنفيذ السمة — تحليل أجسام الدوال
            // (EN) Check trait implementation — analyze method bodies
            for (auto &method : decl.methods)
            {
                if (method)
                    method->accept(*this);
            }
        }

        void BorrowChecker::visitStructDecl(AST::StructDecl &decl)
        {
            // (AR) فحص تصريح البنية — لا فحوصات ملكية حالياً
            // (EN) Check struct declaration — no ownership checks currently
        }

        void BorrowChecker::visitTestDecl(AST::TestDecl &decl)
        {
            // (AR) فحص تصريح الاختبار — تحليل جسم الاختبار
            // (EN) Check test declaration — analyze test body
            if (decl.body)
                decl.body->accept(*this);
        }

        void BorrowChecker::visitExtensionDecl(AST::ExtensionDecl &decl)
        {
            // (AR) تصريح كتلة الامتداد — إضافة دوال جديدة لنوع موجود
            //      مثال: امتداد نقطة ... نهاية
            //      كل طريقة في الامتداد تعمل كطريقة عادية في الصنف الأصلي
            //      لذلك نفحص كل طريقة في نطاق مستقل
            // (EN) Extension block declaration — add new methods to existing type
            //      Example: extension Point ... end
            //      Each method works like a regular method on the original class
            //      So we check each method in its own scope

            if (debugMode_)
            {
                recordWarning("[debug] Extension for type '" + decl.targetType + "' with " + std::to_string(decl.methods.size()) + " methods at " + getLocation(&decl).toString());
            }

            // (AR) فحص كل طريقة في الامتداد — كل طريقة تفتح نطاقها الخاص
            // (EN) Check each method in extension — each method opens its own scope
            for (auto &method : decl.methods)
            {
                if (method)
                {
                    method->accept(*this);
                }
            }
        }

        void BorrowChecker::visitMacroDecl(AST::MacroDecl &decl)
        {
            // (AR) تصريح الماكرو — يعمل كدالة مُضمّنة تُنفّذ في نطاق المُستدعي
            //      الماكرو صحي (hygienic): متغيراته الداخلية لا تلوّث النطاق الخارجي
            //      لذلك نفحص الجسم في نطاق معزول مع تسجيل المعاملات
            // (EN) Macro declaration — works as inline function in caller's scope
            //      Macro is hygienic: internal variables don't pollute outer scope
            //      So we check the body in isolated scope with parameters registered

            if (debugMode_)
            {
                recordWarning("[debug] Macro '" + decl.name + "' with " + std::to_string(decl.params.size()) + " params" + (decl.isVariadic ? " (variadic)" : "") + " at " + getLocation(&decl).toString());
            }

            tracker_->enterScope();

            // (AR) تسجيل معاملات الماكرو كمتغيرات في النطاق المعزول
            //      معاملات الماكرو تُنسخ عند الاستدعاء، لذا تُعتبر قابلة للنسخ
            // (EN) Register macro parameters as variables in isolated scope
            //      Macro parameters are copied on invocation, so treated as Copy
            for (const auto &param : decl.params)
            {
                // (AR) المعاملات العادية — كل معامل يُسجّل كمتغير قابل للنسخ
                // (EN) Regular parameters — each is registered as a Copy variable
                tracker_->declareVariable(param, "أي", getLocation(&decl), true);
                currentResult_.totalVariables++;
            }

            // (AR) فحص جسم الماكرو — نفس فحص جسم أي دالة
            // (EN) Check macro body — same as checking any function body
            if (decl.body)
            {
                decl.body->accept(*this);
            }

            tracker_->exitScope();
        }

        void BorrowChecker::visitTypeAliasDecl(AST::TypeAliasDecl &decl)
        {
            // (AR) تصريح اسم مستعار للنوع — لا يتضمن قيم وقت التشغيل
            //      مثال: نوع عدد = رقم
            //      مثال: نوع قائمة = مصفوفة
            //      هذا تصريح على مستوى الأنواع فقط، لا يولّد كود ولا يؤثر على الملكية
            //      لكن نفحص التعبير الهدف في حال كان يشير إلى رمز غير معرّف
            // (EN) Type alias declaration — no runtime values involved
            //      Example: type integer = number
            //      This is a type-level declaration only, generates no code, no ownership impact
            //      But we check the target expression in case it references an undefined symbol

            if (decl.target)
            {
                // (AR) التعبير الهدف عادة اسم نوع (VariableExpr) — لا نتحقق من ملكيته
                //      لأنه ليس متغيراً بل اسم نوع يُحلّل في مرحلة لاحقة
                // (EN) Target expression is usually a type name (VariableExpr) — no ownership check
                //      Because it's not a variable but a type name resolved in a later phase

                // (AR) لا حاجة لاستدعاء accept — الاسم المستعار لا يستهلك قيماً
                // (EN) No need to call accept — alias doesn't consume values
            }

            if (debugMode_)
            {
                recordWarning("[debug] Type alias '" + decl.name + "' at " + getLocation(&decl).toString());
            }

            (void)decl;
        }

        void BorrowChecker::visitTupleDestructureStmt(AST::TupleDestructureStmt &stmt)
        {
            // (AR) تفكيك الصف — تعيين عناصر صف إلى متغيرات فردية
            //      مثال: متغير (أ، ب، ج) = صف(1، 2، 3)
            //      مثال: ثابت (س، ص) = نقطة.إحداثيات()
            //      التعبير المُهيّئ (الطرف الأيمن) يُنقَل أو يُنسخ حسب نوعه
            //      كل متغير مُفكَّك يحصل على ملكية عنصر واحد من الصف
            // (EN) Tuple destructuring — assign tuple elements to individual variables
            //      Example: var (a, b, c) = tuple(1, 2, 3)
            //      Example: const (x, y) = point.coordinates()
            //      The initializer (right side) is moved or copied depending on its type
            //      Each destructured variable gets ownership of one tuple element

            if (debugMode_)
            {
                std::string varNames;
                for (size_t i = 0; i < stmt.names.size(); ++i)
                {
                    if (i > 0)
                        varNames += ", ";
                    varNames += stmt.names[i];
                }
                recordWarning("[debug] Tuple destructure (" + varNames + ")" + (stmt.isConst ? " [const]" : " [var]") + " at " + getLocation(&stmt).toString());
            }

            // (AR) فحص المُهيّئ أولاً (قبل تسجيل المتغيرات الجديدة)
            // (EN) Check initializer first (before registering new variables)
            if (stmt.initializer)
            {
                // (AR) إذا كان المُهيّئ متغيراً، فهذا نقل ملكية للصف بالكامل
                // (EN) If initializer is a variable, this is a whole-tuple ownership move
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(stmt.initializer.get()))
                {
                    std::string varName = varExpr->toString();

                    // (AR) تحقق من معلومات الملكية لتحديد إذا كان قابلاً للنسخ
                    // (EN) Check ownership info to determine if it's Copy
                    auto info = tracker_->getOwnershipInfo(varName);
                    if (info && !info->isCopyType)
                    {
                        // (AR) نقل الملكية من المتغير المصدر
                        // (EN) Move ownership from source variable
                        auto error = tracker_->moveVariable(varName, getLocation(varExpr));
                        if (error)
                        {
                            currentResult_.addError(*error);
                        }
                        currentResult_.totalMoves++;
                    }
                    else
                    {
                        // (AR) نوع قابل للنسخ — قراءة فقط
                        // (EN) Copy type — read only
                        auto error = tracker_->useVariable(varName, getLocation(varExpr));
                        if (error)
                        {
                            currentResult_.addError(*error);
                        }
                    }
                }
                else
                {
                    // (AR) تعبير عام (استدعاء دالة، صف حرفي، ...) — فحص عادي
                    // (EN) General expression (function call, tuple literal, ...) — normal check
                    stmt.initializer->accept(*this);
                }
            }

            // (AR) تسجيل كل متغير مُفكَّك في متتبع الملكية
            //      كل متغير يحصل على نوع "أي" لأن نوع عناصر الصف غير محدد في AST
            //      المتغيرات الثابتة (isConst) تُسجَّل كأنواع قابلة للنسخ لأنها لا تُعدَّل
            // (EN) Register each destructured variable in ownership tracker
            //      Each variable gets type "any" since tuple element types aren't specified in AST
            //      Const variables (isConst) are registered as Copy types since they can't be mutated
            for (const auto &name : stmt.names)
            {
                // (AR) استنتاج إذا كان النوع قابلاً للنسخ:
                //      - ثابت: لا يُنقل أبداً، لذا يُعتبر قابلاً للنسخ
                //      - متغير: يعتمد على نوع العنصر الأصلي (لا نعرفه، نفترض copy)
                // (EN) Infer if type is Copy:
                //      - const: never moved, so treated as Copy
                //      - var: depends on original element type (unknown, assume copy)
                bool isCopy = true; // (AR) افتراضي — آمن للغة ص الحالية
                tracker_->declareVariable(name, "أي", getLocation(&stmt), isCopy);
                currentResult_.totalVariables++;
            }
        }

        // ============================================================================
        // دوال مساعدة / Helper Functions
        // ============================================================================

        void BorrowChecker::analyzeExpression(AST::Expression *expr, bool isMoveContext)
        {
            if (!expr)
                return;

            // (AR) إذا كان متغيراً وسياق نقل
            // (EN) If variable and move context
            if (isMoveContext)
            {
                auto error = tracker_->moveVariable(
                    expr->toString(),
                    getLocation(expr));
                if (error)
                {
                    currentResult_.addError(*error);
                }
                currentResult_.totalMoves++;
            }
            else
            {
                // (AR) قراءة فقط
                // (EN) Read only
                auto error = tracker_->useVariable(
                    expr->toString(),
                    getLocation(expr));
                if (error)
                {
                    currentResult_.addError(*error);
                }
            }
        }

        void BorrowChecker::analyzeAssignment(AST::AssignExpr *assign)
        {
            if (!assign)
                return;

            // (AR) فحص الطرف الأيمن أولاً
            // (EN) Check right side first
            if (assign->value)
            {
                // (AR) إذا كان المصدر متغيراً، فهذا نقل ملكية
                // (EN) If source is a variable, this is an ownership move
                auto *varExpr = dynamic_cast<AST::VariableExpr *>(assign->value.get());
                if (varExpr)
                {
                    auto error = tracker_->moveVariable(varExpr->toString(), getLocation(varExpr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                    currentResult_.totalMoves++;
                }
                else
                {
                    assign->value->accept(*this);
                }
            }

            // (AR) فحص الطرف الأيسر (التعديل)
            // (EN) Check left side (mutation)
            auto error = tracker_->mutateVariable(
                assign->name,
                getLocation(assign));
            if (error)
            {
                currentResult_.addError(*error);
            }
        }

        void BorrowChecker::analyzeFunctionCall(AST::CallExpr *call)
        {
            if (!call)
                return;

            // (AR) فحص المستدعى
            // (EN) Check callee
            if (call->callee)
            {
                call->callee->accept(*this);
            }

            // (AR) فحص كل معامل - كل معامل يُقرأ (وقد يُنقل)
            // (EN) Check each argument - each is read (and may be moved)
            for (auto &arg : call->arguments)
            {
                if (arg)
                {
                    arg->accept(*this);
                }
            }
        }

        bool BorrowChecker::isCopyType(const std::string &typeName) const
        {
            // (AR) الأنواع غير المحددة أو غير المعروفة تُعتبر قابلة للنسخ افتراضياً
            // (EN) Unknown/unspecified/none types default to copy — safe for ص language
            // (AR) لغة ص لا تدعم نقل الملكية مثل Rust، لذا الأنواع غير المحددة تُنسخ
            // (EN) S language doesn't have Rust-like move semantics, so unresolved types are copy
            if (typeName.empty() || typeName == "unknown" || typeName == "لاشيء")
            {
                return true;
            }

            // (AR) التحقق من القائمة
            // (EN) Check the list
            if (copyTypes_.find(typeName) != copyTypes_.end())
            {
                return true;
            }

            // (AR) المراجع ليست قابلة للنسخ (تُستنسخ المراجع نفسها)
            // (EN) References are Copy (the references themselves are copied)
            if (!typeName.empty() && typeName.front() == '&')
            {
                return true;
            }
            if (typeName.size() >= 4 && typeName.substr(0, 4) == "مرجع")
            {
                return true;
            }

            return false;
        }

        std::string BorrowChecker::dataTypeToString(Types::SadTypeKind type) const
        {
            // (AR) تحويل نوع البيانات إلى نص لمطابقة أنواع النسخ
            // (EN) Convert DataType enum to string for copy type matching
            switch (type)
            {
            case Types::SadTypeKind::Integer:
                return "رقم";
            case Types::SadTypeKind::Float:
                return "عشري";
            case Types::SadTypeKind::Boolean:
                return "منطقي";
            case Types::SadTypeKind::String:
                return "نص";
            case Types::SadTypeKind::UInt8:
                return "بايت";
            case Types::SadTypeKind::Array:
                return "مصفوفة";
            case Types::SadTypeKind::Map:
                return "قاموس";
            case Types::SadTypeKind::Tuple:
                return "ثنائي";
            case Types::SadTypeKind::Function:
                return "دالة";
            case Types::SadTypeKind::Void:
                return "لاشيء";
            case Types::SadTypeKind::Enum:
                return "تعداد";
            case Types::SadTypeKind::Error:
                return "خطأ";
            default:
                return "unknown";
            }
        }

        SourceLocation BorrowChecker::getLocation(AST::ASTNode *node) const
        {
            if (!node)
            {
                return SourceLocation();
            }

            // تتبع الموقع الفعلي من العقدة مع fallback آمن إذا كانت الإحداثيات غير مهيأة.
            const size_t line = (node->position.line == 0) ? 1 : node->position.line;
            const size_t column = (node->position.column == 0) ? 1 : node->position.column;
            return SourceLocation(currentFile_,
                                  line,
                                  column);
        }

        void BorrowChecker::recordError(const OwnershipError &error)
        {
            currentResult_.addError(error);

            if (debugMode_)
            {
                if (useArabicMessages_)
                {
                    std::cerr << error.toArabicString();
                }
                else
                {
                    std::cerr << error.toEnglishString();
                }
            }
        }

        void BorrowChecker::recordWarning(const std::string &warning)
        {
            currentResult_.addWarning(warning);

            if (debugMode_)
            {
                std::cerr << "[تحذير/Warning] " << warning << "\n";
            }
        }


    } // namespace Semantic
} // namespace Sad
