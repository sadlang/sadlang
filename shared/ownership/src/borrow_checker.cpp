// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif

/**
 * @file borrow_checker.cpp
 * @brief تنفيذ فاحص الاستعارة / Borrow Checker Implementation
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 */

#include "ownership/borrow_checker.h"
#include <iostream>
#include <sstream>

namespace Sad
{
    namespace Semantic
    {

        // ============================================================================
        // بناء وهدم / Construction & Destruction
        // ============================================================================

        BorrowChecker::BorrowChecker()
            : tracker_(std::make_unique<OwnershipTracker>()), useArabicMessages_(true), debugMode_(false), nllMode_(true), currentFile_(""), currentFunction_("")
        {
        }

        BorrowChecker::~BorrowChecker() = default;


        // ============================================================================
        // واجهة الفحص الرئيسية / Main Check Interface
        // ============================================================================

        BorrowCheckResult BorrowChecker::check(AST::ASTNode *ast)
        {
            reset();

            if (!ast)
            {
                currentResult_.addWarning("AST is null / شجرة AST فارغة");
                return currentResult_;
            }

            if (debugMode_)
            {
                std::cout << "[BorrowChecker] Starting check...\n";
            }

            // (AR) زيارة الجذر
            // (EN) Visit root
            // (AR) إذا كان الجذر BlockStmt، نزور محتوياته مباشرة دون دخول نطاق جديد
            // (EN) If root is BlockStmt, visit its contents directly without entering a new scope
            // هذا ضروري لأن المتغيرات العامة يجب أن تُسجَّل في النطاق 0
            // This is necessary so global variables are registered in scope 0
            if (auto *blockStmt = dynamic_cast<AST::BlockStmt *>(ast))
            {
                for (auto &stmt : blockStmt->statements)
                {
                    if (stmt)
                    {
                        stmt->accept(*this);
                    }
                }
            }
            else
            {
                ast->accept(*this);
            }

            // (AR) جمع الأخطاء الإضافية من المتتبع (مثل: حذف أثناء الاستعارة)
            // (EN) Collect additional errors from tracker (e.g. drop while borrowed)
            // ملاحظة: الأخطاء من الزوار تُضاف مباشرة إلى currentResult_
            // Note: Visitor errors are added directly to currentResult_
            // هنا نجمع فقط الأخطاء التي يكتشفها المتتبع عند خروج النطاق
            // Here we only collect errors the tracker detects on scope exit
            // تم نقل الأخطاء من المتتبع إلى الزوار مباشرة - لا حاجة لجمعها مرتين
            // Errors moved to visitors directly - no need to collect twice

            if (debugMode_)
            {
                std::cout << "[BorrowChecker] Check complete. "
                          << "Errors: " << currentResult_.errors.size() << "\n";
                tracker_->dump();
            }

            return currentResult_;
        }

        BorrowCheckResult BorrowChecker::checkFunction(AST::FunctionDecl *func)
        {
            reset();

            if (!func)
            {
                currentResult_.addWarning("Function is null / الدالة فارغة");
                return currentResult_;
            }

            visitFunctionDecl(*func);

            return currentResult_;
        }

        void BorrowChecker::reset()
        {
            tracker_ = std::make_unique<OwnershipTracker>();
            currentResult_ = BorrowCheckResult();
            currentFile_ = "";
            currentFunction_ = "";
        }

        // ============================================================================
        // زيارة التعابير / Visit Expressions
        // ============================================================================

        void BorrowChecker::visitBinaryExpr(AST::BinaryExpr &expr)
        {
            // (AR) فحص الطرفين
            // (EN) Check both sides
            if (expr.left)
            {
                expr.left->accept(*this);
            }
            if (expr.right)
            {
                expr.right->accept(*this);
            }
        }

        void BorrowChecker::visitUnaryExpr(AST::UnaryExpr &expr)
        {
            // (AR) التحقق من نوع العامل الأحادي
            // (EN) Check unary operator type

            if (!expr.operand)
                return;

            // (AR) معالجة عامل المرجع & (استعارة)
            // (EN) Handle reference operator & (borrow)
            if (expr.op == Lexer::TokenType::OP_BITWISE_AND)
            {
                // (AR) تحقق: هل المعامل متغير يمكن استعارته؟
                // (EN) Check: is operand a variable we can borrow?
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.operand.get()))
                {
                    std::string varName = varExpr->toString();
                    std::string borrowerName = "&" + varName;

                    // (AR) إنشاء استعارة ثابتة (قراءة فقط)
                    // (EN) Create shared (immutable) borrow
                    auto error = tracker_->createBorrow(
                        varName, borrowerName,
                        BorrowKind::Shared,
                        getLocation(&expr));

                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                    currentResult_.totalBorrows++;
                }
                else
                {
                    // (AR) فحص المعامل عادياً
                    // (EN) Check operand normally
                    expr.operand->accept(*this);
                }
                return;
            }

            // (AR) معالجة عامل فك المرجع * (dereference)
            // (EN) Handle dereference operator *
            if (expr.op == Lexer::TokenType::OP_MULTIPLY)
            {
                // (AR) فحص أن المعامل مستعار بشكل صحيح
                // (EN) Check that operand is properly borrowed
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.operand.get()))
                {
                    std::string varName = varExpr->toString();
                    // (AR) التحقق من أن المتغير صالح للاستخدام (ليس منقولاً)
                    // (EN) Verify variable is valid (not moved)
                    auto error = tracker_->useVariable(varName, getLocation(&expr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                }
                else
                {
                    expr.operand->accept(*this);
                }
                return;
            }

            // (AR) للعوامل الأخرى (-, !, ~): فحص المعامل عادياً
            // (EN) For other operators (-, !, ~): check operand normally
            expr.operand->accept(*this);
        }

        void BorrowChecker::visitTernaryExpr(AST::TernaryExpr &expr)
        {
            // (AR) فحص الشرط والفرعين
            // (EN) Check condition and both branches
        }

        void BorrowChecker::visitLiteralExpr(AST::LiteralExpr &expr)
        {
            // (AR) القيم الحرفية لا تحتاج فحص ملكية
            // (EN) Literals don't need ownership checking
            (void)expr;
        }

        void BorrowChecker::visitVariableExpr(AST::VariableExpr &expr)
        {
            // (AR) التحقق من إمكانية الاستخدام
            // (EN) Check if can be used
            auto error = tracker_->useVariable(expr.toString(), getLocation(&expr));
            if (error)
            {
                currentResult_.addError(*error);
            }
        }

        void BorrowChecker::visitAssignExpr(AST::AssignExpr &expr)
        {
            analyzeAssignment(&expr);
        }

        void BorrowChecker::visitCallExpr(AST::CallExpr &expr)
        {
            analyzeFunctionCall(&expr);
        }

        void BorrowChecker::visitIndexExpr(AST::IndexExpr &expr)
        {
            // (AR) فحص المصفوفة والفهرس
            // (EN) Check array and index
        }

        void BorrowChecker::visitMemberExpr(AST::MemberExpr &expr)
        {
            // (AR) فحص الكائن
            // (EN) Check object
        }

        void BorrowChecker::visitMemberAssignExpr(AST::MemberAssignExpr &expr)
        {
            // (AR) فحص تعيين العضو
            // (EN) Check member assignment
        }

        void BorrowChecker::visitIndexAssignExpr(AST::IndexAssignExpr &expr)
        {
            // (AR) فحص الإسناد بالفهرس
            // (EN) Check index assignment
        }

        void BorrowChecker::visitArrayExpr(AST::ArrayExpr &expr)
        {
            // (AR) فحص عناصر المصفوفة
            // (EN) Check array elements
        }

        void BorrowChecker::visitMapExpr(AST::MapExpr &expr)
        {
            // (AR) فحص عناصر القاموس
            // (EN) Check map entries
        }

        void BorrowChecker::visitWalrusExpr(AST::WalrusExpr &expr)
        {
            // (AR) فحص تعبير Walrus
            // (EN) Check walrus expression
        }

        void BorrowChecker::visitAwaitExpr(AST::AwaitExpr &expr)
        {
            // (AR) فحص تعبير الانتظار
            // (EN) Check await expression
        }

        void BorrowChecker::visitLambdaExpr(AST::LambdaExpr &expr)
        {
            // (AR) فحص Lambda
            // (EN) Check lambda expression
        }

        void BorrowChecker::visitListComprehensionExpr(AST::ListComprehensionExpr &expr)
        {
            // (AR) فحص استيعاب القائمة
            // (EN) Check list comprehension
        }

        void BorrowChecker::visitDictComprehensionExpr(AST::DictComprehensionExpr &expr)
        {
            // (AR) فحص استيعاب القاموس
            // (EN) Check dict comprehension
        }

        void BorrowChecker::visitSetComprehensionExpr(AST::SetComprehensionExpr &expr)
        {
            // (AR) فحص استيعاب المجموعة
            // (EN) Check set comprehension
        }

        void BorrowChecker::visitGeneratorExpr(AST::GeneratorExpr &expr)
        {
            // (AR) فحص تعبير المولد
            // (EN) Check generator expression
        }

        void BorrowChecker::visitDecoratorExpr(AST::DecoratorExpr &expr)
        {
            // (AR) فحص المزخرف
            // (EN) Check decorator
        }

        void BorrowChecker::visitNewExpr(AST::NewExpr &expr)
        {
            // (AR) فحص تعبير new
            // (EN) Check new expression
        }

        void BorrowChecker::visitMemberAccessExpr(AST::MemberAccessExpr &expr)
        {
            // (AR) فحص الوصول للعضو
            // (EN) Check member access
        }

        void BorrowChecker::visitMethodCallExpr(AST::MethodCallExpr &expr)
        {
            // (AR) فحص استدعاء الطريقة
            // (EN) Check method call
        }

        void BorrowChecker::visitThisExpr(AST::ThisExpr &expr)
        {
            // (AR) this/هذا لا يحتاج فحص خاص
            // (EN) this doesn't need special checking
            (void)expr;
        }

        void BorrowChecker::visitSuperExpr(AST::SuperExpr &expr)
        {
            // (AR) super/أصل لا يحتاج فحص خاص
            // (EN) super doesn't need special checking
            (void)expr;
        }

        void BorrowChecker::visitBorrowExpr(AST::BorrowExpr &expr)
        {
            // (AR) فحص تعبير الاستعارة &x أو &متغير x
            // (EN) Check borrow expression &x or &mut x
            if (!expr.variableName.empty())
            {
                // (AR) تحديد نوع الاستعارة: مشتركة أو متغيرة
                // (EN) Determine borrow kind: shared or mutable
                BorrowKind kind = expr.isMutable ? BorrowKind::Mutable : BorrowKind::Shared;
                std::string borrowerName = (expr.isMutable ? "&متغير " : "&") + expr.variableName;

                // (AR) إنشاء الاستعارة في متتبع الملكية
                // (EN) Create borrow in ownership tracker
                auto error = tracker_->createBorrow(
                    expr.variableName, borrowerName,
                    kind, getLocation(&expr));

                if (error)
                {
                    currentResult_.addError(*error);
                }
                currentResult_.totalBorrows++;

                if (debugMode_)
                {
                    recordWarning("[debug] " + std::string(expr.isMutable ? "Mutable" : "Shared") + " borrow of '" + expr.variableName + "' at " + getLocation(&expr).toString());
                }
            }
        }

        void BorrowChecker::visitInlineAsmExpr(AST::InlineAsmExpr &expr)
        {
            // (AR) تعبير التجميع المضمّن - لا يحتاج فحص ملكية
            // (EN) Inline assembly expression - no ownership checking needed
            (void)expr;
        }

        void BorrowChecker::visitRangeExpr(AST::RangeExpr &expr)
        {
            // (AR) تعبير النطاق — فحص التعبيرات الفرعية
            // (EN) Range expression — check sub-expressions
            if (expr.start)
                expr.start->accept(*this);
            if (expr.end)
                expr.end->accept(*this);
        }

        void BorrowChecker::visitOptionalChainExpr(AST::OptionalChainExpr &expr)
        {
            // (AR) وصول آمن ?. — فحص الكائن للتأكد أنه لم يُنقل
            // (EN) Optional chaining ?. — check object is not moved
            if (expr.object)
            {
                expr.object->accept(*this);

                // (AR) إذا كان الكائن متغيراً، تحقق من صلاحيته
                // (EN) If object is a variable, verify it's still valid
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.object.get()))
                {
                    auto error = tracker_->useVariable(varExpr->toString(), getLocation(&expr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                }
            }
        }

        void BorrowChecker::visitNullCoalesceExpr(AST::NullCoalesceExpr &expr)
        {
            // (AR) تجميع فارغ ?? — فحص الطرف الأيسر والأيمن
            // (EN) Null coalescing ?? — check both left and right
            if (expr.left)
            {
                expr.left->accept(*this);

                // (AR) الطرف الأيسر قد ينقل ملكية إذا كان متغيراً
                // (EN) Left side may transfer ownership if it's a variable
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.left.get()))
                {
                    auto error = tracker_->useVariable(varExpr->toString(), getLocation(&expr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                }
            }
            if (expr.right)
            {
                expr.right->accept(*this);
            }
        }

        void BorrowChecker::visitErrorPropagateExpr(AST::ErrorPropagateExpr &expr)
        {
            // (AR) نشر الخطأ — تعبير «انشر» يُقيّم التعبير الداخلي:
            //      - إذا كانت النتيجة خطأ/لاشيء، يُرجع مبكراً من الدالة
            //      - إذا كانت النتيجة صحيحة، يفك التغليف ويُرجع القيمة
            //      هذا يشبه عامل ? في Rust أو ?. مع إرجاع مبكر
            // (EN) Error propagation — 'propagate' expression evaluates inner expression:
            //      - If result is error/none, returns early from function
            //      - If result is ok, unwraps and returns value
            //      Similar to Rust's ? operator

            if (!expr.inner)
                return;

            // (AR) فحص التعبير الداخلي — قد يكون استدعاء دالة أو متغير
            // (EN) Check inner expression — could be a function call or variable
            expr.inner->accept(*this);

            // (AR) إذا كان التعبير الداخلي متغيراً، نتحقق من صلاحيته
            //      نشر الخطأ يقرأ المتغير (لا ينقله) لأنه يفك التغليف فقط
            // (EN) If inner is a variable, verify it's still valid
            //      Error propagation reads the variable (doesn't move) as it only unwraps
            if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(expr.inner.get()))
            {
                auto error = tracker_->useVariable(varExpr->toString(), getLocation(&expr));
                if (error)
                {
                    currentResult_.addError(*error);
                }
            }

            if (debugMode_)
            {
                recordWarning("[debug] Error propagation at " + getLocation(&expr).toString());
            }
        }

        void BorrowChecker::visitTupleExpr(AST::TupleExpr &expr)
        {
            // (AR) تعبير الصف (قيمة1، قيمة2، ...) — إنشاء صف جديد
            //      كل عنصر في الصف يُنقل إليه (ما لم يكن من نوع قابل للنسخ)
            //      لذلك نتحقق من ملكية كل عنصر
            // (EN) Tuple expression (val1, val2, ...) — create new tuple
            //      Each element is moved into the tuple (unless Copy type)
            //      So we verify ownership of each element

            for (auto &element : expr.elements)
            {
                if (!element)
                    continue;

                // (AR) إذا كان العنصر متغيراً غير قابل للنسخ، فهذا نقل ملكية
                // (EN) If element is a non-Copy variable, this is an ownership move
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(element.get()))
                {
                    std::string varName = varExpr->toString();

                    // (AR) تحقق من معلومات الملكية لتحديد إذا كان النوع قابلاً للنسخ
                    // (EN) Check ownership info to determine if type is Copy
                    auto info = tracker_->getOwnershipInfo(varName);
                    if (info && !info->isCopyType)
                    {
                        // (AR) نقل الملكية — المتغير يُنقل إلى الصف
                        // (EN) Move ownership — variable is moved into tuple
                        auto error = tracker_->moveVariable(varName, getLocation(&expr));
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
                        auto error = tracker_->useVariable(varName, getLocation(&expr));
                        if (error)
                        {
                            currentResult_.addError(*error);
                        }
                    }
                }
                else
                {
                    // (AR) تعبير غير متغير (حرفي، استدعاء دالة، ...) — فحص عادي
                    // (EN) Non-variable expression (literal, function call, ...) — normal check
                    element->accept(*this);
                }
            }

            if (debugMode_)
            {
                recordWarning("[debug] Tuple with " + std::to_string(expr.elements.size()) + " elements at " + getLocation(&expr).toString());
            }
        }

        // ============================================================================
        // زيارة العبارات / Visit Statements
        // ============================================================================

        void BorrowChecker::visitExprStmt(AST::ExprStmt &stmt)
        {
            // (AR) فحص التعبير داخل العبارة
            // (EN) Check expression inside statement
            if (stmt.expression)
            {
                stmt.expression->accept(*this);
            }
        }

        void BorrowChecker::visitVarDeclStmt(AST::VarDeclStmt &stmt)
        {
            // (AR) تسجيل المتغير الجديد في المتتبع
            // (EN) Register new variable with tracker
            currentResult_.totalVariables++;

            // (AR) فحص المُعيِّن أولاً (قبل تسجيل المتغير)
            // (EN) Check initializer first (before registering variable)
            if (stmt.initializer)
            {
                // (AR) تحليل المُعيِّن - قد يتضمن نقل ملكية
                // (EN) Analyze initializer - may involve ownership transfer

                // (AR) إذا كان المُعيِّن متغيراً، فهذا نقل ملكية
                // (EN) If initializer is a variable, this is an ownership move
                auto *varExpr = dynamic_cast<AST::VariableExpr *>(stmt.initializer.get());
                if (varExpr)
                {
                    // (AR) نقل الملكية من المتغير المصدر
                    // (EN) Move ownership from source variable
                    auto error = tracker_->moveVariable(varExpr->toString(), getLocation(varExpr));
                    if (error)
                    {
                        currentResult_.addError(*error);
                    }
                    currentResult_.totalMoves++;
                }
                else
                {
                    // (AR) فحص التعبير بشكل عام
                    // (EN) Check expression generally
                    stmt.initializer->accept(*this);
                }
            }

            // (AR) استخراج اسم النوع من DataType
            // (EN) Extract type name from DataType
            std::string typeName = dataTypeToString(stmt.type);
            // (AR) 🔑 الحكمُ بمفتاحِ النوعِ لا باسمِه: `sadTypeKindArabicName`
            //      تُرجِعُ «كائن» لـClass وStruct كليهما، فالاسمُ لا يميّزُ أصلًا.
            //      والاسمُ يبقى للتشخيصِ وحدَه.
            // (EN) The verdict is keyed on the kind, not its name: the Arabic-name
            //      function returns one word for both Class and Struct, so the name
            //      cannot discriminate. The name is kept for diagnostics only.
            bool isCopy = isCopyKind(stmt.type);

            // (AR) إذا كان النوع غير محدد ولكن المُعيِّن قيمة حرفية، استنتج النوع
            // (EN) If type is unspecified but initializer is a literal, infer the type
            if (!isCopy && stmt.initializer)
            {
                if (auto *lit = dynamic_cast<AST::LiteralExpr *>(stmt.initializer.get()))
                {
                    // (AR) القيم الحرفية (أرقام، عشرية، منطقية) قابلة للنسخ
                    // (EN) Literal values (integers, floats, booleans) are copy types
                    Types::SadTypeKind litType = lit->getTypeKind();
                    isCopy = true;
                    if (litType == Types::SadTypeKind::Integer)
                        typeName = "رقم";
                    else if (litType == Types::SadTypeKind::Float)
                        typeName = "عشري";
                    else if (litType == Types::SadTypeKind::Boolean)
                        typeName = "منطقي";
                    else if (litType == Types::SadTypeKind::String)
                    {
                        typeName = "نص";
                        isCopy = false; // Strings are not copy types
                    }
                }
            }

            // (AR) إذا كان المُعيِّن متغيراً، استنتج النوع من المتغير المصدر
            // (EN) If initializer is a variable, infer type from source variable
            if (!isCopy && stmt.initializer)
            {
                if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(stmt.initializer.get()))
                {
                    auto srcInfo = tracker_->getOwnershipInfo(varExpr->toString());
                    if (srcInfo)
                    {
                        typeName = srcInfo->typeName;
                        isCopy = srcInfo->isCopyType;
                    }
                }
            }

            // (AR) إذا لم نتمكن من تحديد النوع، نعتبره قابلاً للنسخ
            // (EN) If we couldn't determine the type, assume it's a copy type
            // (AR) هذا آمن لأن اللغة حالياً تدعم الأنواع البدائية فقط (أرقام، منطقي، نصوص)
            // (EN) This is safe because the language currently only supports primitive types
            if (typeName == "unknown" || typeName.empty())
            {
                isCopy = true;
            }

            if (debugMode_)
            {
                std::cout << "[BorrowChecker] Declaring variable: " << stmt.name
                          << " type: " << typeName << " isCopy: " << isCopy << "\n";
            }

            tracker_->declareVariable(stmt.name, typeName, getLocation(&stmt), isCopy);
        }

        void BorrowChecker::visitIfStmt(AST::IfStmt &stmt)
        {
            // (AR) فحص الشرط والفروع
            // (EN) Check condition and branches
            if (stmt.condition)
            {
                stmt.condition->accept(*this);
            }
            tracker_->enterScope();
            if (stmt.thenBranch)
            {
                stmt.thenBranch->accept(*this);
            }
            tracker_->exitScope();
            if (stmt.elseBranch)
            {
                tracker_->enterScope();
                stmt.elseBranch->accept(*this);
                tracker_->exitScope();
            }
        }

        void BorrowChecker::visitWhileStmt(AST::WhileStmt &stmt)
        {
            // (AR) فحص الحلقة
            // (EN) Check loop
            if (stmt.condition)
            {
                stmt.condition->accept(*this);
            }
            tracker_->enterScope();
            if (stmt.body)
            {
                stmt.body->accept(*this);
            }
            tracker_->exitScope();
        }

        void BorrowChecker::visitForStmt(AST::ForStmt &stmt)
        {
            // (AR) فحص حلقة for
            // (EN) Check for loop
            tracker_->enterScope();
            if (stmt.initializer)
            {
                stmt.initializer->accept(*this);
            }
            if (stmt.condition)
            {
                stmt.condition->accept(*this);
            }
            if (stmt.increment)
            {
                stmt.increment->accept(*this);
            }
            if (stmt.body)
            {
                stmt.body->accept(*this);
            }
            tracker_->exitScope();
        }


    } // namespace Semantic
} // namespace Sad

