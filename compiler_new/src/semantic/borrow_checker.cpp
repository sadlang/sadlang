// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file borrow_checker.cpp
 * @brief تنفيذ فاحص الاستعارة / Borrow Checker Implementation
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 */

#include "semantic/borrow_checker.h"
#include <iostream>
#include <sstream>

namespace Sad {
namespace Semantic {

// ============================================================================
// بناء وهدم / Construction & Destruction
// ============================================================================

BorrowChecker::BorrowChecker()
    : tracker_(std::make_unique<OwnershipTracker>())
    , useArabicMessages_(true)
    , debugMode_(false)
    , nllMode_(true)
    , currentFile_("")
    , currentFunction_("") {
    initializeCopyTypes();
}

BorrowChecker::~BorrowChecker() = default;

// ============================================================================
// تهيئة الأنواع القابلة للنسخ / Initialize Copy Types
// ============================================================================

void BorrowChecker::initializeCopyTypes() {
    // (AR) الأنواع البدائية قابلة للنسخ دائماً
    // (EN) Primitive types are always Copy
    
    // الأرقام الصحيحة / Integers
    copyTypes_.insert("ح8");    // i8
    copyTypes_.insert("ح16");   // i16
    copyTypes_.insert("ح32");   // i32
    copyTypes_.insert("ح64");   // i64
    copyTypes_.insert("ع8");    // u8
    copyTypes_.insert("ع16");   // u16
    copyTypes_.insert("ع32");   // u32
    copyTypes_.insert("ع64");   // u64
    copyTypes_.insert("رقم");   // default integer
    copyTypes_.insert("عدد");   // alias for integer
    
    // الأرقام العشرية / Floats
    copyTypes_.insert("عش32");  // f32
    copyTypes_.insert("عش64");  // f64
    copyTypes_.insert("عشري"); // default float
    
    // المنطقي / Boolean
    copyTypes_.insert("منطقي");
    copyTypes_.insert("bool");
    
    // الحرف / Character
    copyTypes_.insert("حرف");
    copyTypes_.insert("char");
    
    // المؤشرات الخام (unsafe) / Raw pointers
    copyTypes_.insert("*ثابت");
    copyTypes_.insert("*متغير");
    
    // English aliases
    copyTypes_.insert("i8");
    copyTypes_.insert("i16");
    copyTypes_.insert("i32");
    copyTypes_.insert("i64");
    copyTypes_.insert("u8");
    copyTypes_.insert("u16");
    copyTypes_.insert("u32");
    copyTypes_.insert("u64");
    copyTypes_.insert("f32");
    copyTypes_.insert("f64");
    copyTypes_.insert("int");
    copyTypes_.insert("float");
    copyTypes_.insert("double");
}

// ============================================================================
// واجهة الفحص الرئيسية / Main Check Interface
// ============================================================================

BorrowCheckResult BorrowChecker::check(AST::ASTNode* ast) {
    reset();
    
    if (!ast) {
        currentResult_.addWarning("AST is null / شجرة AST فارغة");
        return currentResult_;
    }
    
    if (debugMode_) {
        std::cout << "[BorrowChecker] Starting check...\n";
    }
    
    // (AR) زيارة الجذر
    // (EN) Visit root
    // (AR) إذا كان الجذر BlockStmt، نزور محتوياته مباشرة دون دخول نطاق جديد
    // (EN) If root is BlockStmt, visit its contents directly without entering a new scope
    // هذا ضروري لأن المتغيرات العامة يجب أن تُسجَّل في النطاق 0
    // This is necessary so global variables are registered in scope 0
    if (auto* blockStmt = dynamic_cast<AST::BlockStmt*>(ast)) {
        for (auto& stmt : blockStmt->statements) {
            if (stmt) {
                stmt->accept(*this);
            }
        }
    } else {
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
    
    if (debugMode_) {
        std::cout << "[BorrowChecker] Check complete. "
                  << "Errors: " << currentResult_.errors.size() << "\n";
        tracker_->dump();
    }
    
    return currentResult_;
}

BorrowCheckResult BorrowChecker::checkFunction(AST::FunctionDecl* func) {
    reset();
    
    if (!func) {
        currentResult_.addWarning("Function is null / الدالة فارغة");
        return currentResult_;
    }
    
    visitFunctionDecl(*func);
    
    return currentResult_;
}

void BorrowChecker::reset() {
    tracker_ = std::make_unique<OwnershipTracker>();
    currentResult_ = BorrowCheckResult();
    currentFile_ = "";
    currentFunction_ = "";
}

// ============================================================================
// زيارة التعابير / Visit Expressions
// ============================================================================

void BorrowChecker::visitBinaryExpr(AST::BinaryExpr& expr) {
    // (AR) فحص الطرفين
    // (EN) Check both sides
    if (expr.left) {
        expr.left->accept(*this);
    }
    if (expr.right) {
        expr.right->accept(*this);
    }
}

void BorrowChecker::visitUnaryExpr(AST::UnaryExpr& expr) {
    // (AR) التحقق من نوع العامل الأحادي
    // (EN) Check unary operator type

    if (!expr.operand) return;

    // (AR) معالجة عامل المرجع & (استعارة)
    // (EN) Handle reference operator & (borrow)
    if (expr.op == Lexer::TokenType::AMPERSAND) {
        // (AR) تحقق: هل المعامل متغير يمكن استعارته؟
        // (EN) Check: is operand a variable we can borrow?
        if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(expr.operand.get())) {
            std::string varName = varExpr->toString();
            std::string borrowerName = "&" + varName;
            
            // (AR) إنشاء استعارة ثابتة (قراءة فقط)
            // (EN) Create shared (immutable) borrow
            auto error = tracker_->createBorrow(
                varName, borrowerName,
                BorrowKind::Shared,
                getLocation(&expr));
            
            if (error) {
                currentResult_.addError(*error);
            }
            currentResult_.totalBorrows++;
        } else {
            // (AR) فحص المعامل عادياً
            // (EN) Check operand normally
            expr.operand->accept(*this);
        }
        return;
    }

    // (AR) معالجة عامل فك المرجع * (dereference)
    // (EN) Handle dereference operator *
    if (expr.op == Lexer::TokenType::OP_MULTIPLY) {
        // (AR) فحص أن المعامل مستعار بشكل صحيح
        // (EN) Check that operand is properly borrowed
        if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(expr.operand.get())) {
            std::string varName = varExpr->toString();
            // (AR) التحقق من أن المتغير صالح للاستخدام (ليس منقولاً)
            // (EN) Verify variable is valid (not moved)
            auto error = tracker_->useVariable(varName, getLocation(&expr));
            if (error) {
                currentResult_.addError(*error);
            }
        } else {
            expr.operand->accept(*this);
        }
        return;
    }

    // (AR) للعوامل الأخرى (-, !, ~): فحص المعامل عادياً
    // (EN) For other operators (-, !, ~): check operand normally
    expr.operand->accept(*this);
}

void BorrowChecker::visitTernaryExpr(AST::TernaryExpr& expr) {
    // (AR) فحص الشرط والفرعين
    // (EN) Check condition and both branches
}

void BorrowChecker::visitLiteralExpr(AST::LiteralExpr& expr) {
    // (AR) القيم الحرفية لا تحتاج فحص ملكية
    // (EN) Literals don't need ownership checking
    (void)expr;
}

void BorrowChecker::visitVariableExpr(AST::VariableExpr& expr) {
    // (AR) التحقق من إمكانية الاستخدام
    // (EN) Check if can be used
    auto error = tracker_->useVariable(expr.toString(), getLocation(&expr));
    if (error) {
        currentResult_.addError(*error);
    }
}

void BorrowChecker::visitAssignExpr(AST::AssignExpr& expr) {
    analyzeAssignment(&expr);
}

void BorrowChecker::visitCallExpr(AST::CallExpr& expr) {
    analyzeFunctionCall(&expr);
}

void BorrowChecker::visitIndexExpr(AST::IndexExpr& expr) {
    // (AR) فحص المصفوفة والفهرس
    // (EN) Check array and index
}

void BorrowChecker::visitMemberExpr(AST::MemberExpr& expr) {
    // (AR) فحص الكائن
    // (EN) Check object
}

void BorrowChecker::visitMemberAssignExpr(AST::MemberAssignExpr& expr) {
    // (AR) فحص تعيين العضو
    // (EN) Check member assignment
}

void BorrowChecker::visitIndexAssignExpr(AST::IndexAssignExpr& expr) {
    // (AR) فحص الإسناد بالفهرس
    // (EN) Check index assignment
}

void BorrowChecker::visitArrayExpr(AST::ArrayExpr& expr) {
    // (AR) فحص عناصر المصفوفة
    // (EN) Check array elements
}

void BorrowChecker::visitMapExpr(AST::MapExpr& expr) {
    // (AR) فحص عناصر القاموس
    // (EN) Check map entries
}

void BorrowChecker::visitWalrusExpr(AST::WalrusExpr& expr) {
    // (AR) فحص تعبير Walrus
    // (EN) Check walrus expression
}

void BorrowChecker::visitAwaitExpr(AST::AwaitExpr& expr) {
    // (AR) فحص تعبير الانتظار
    // (EN) Check await expression
}

void BorrowChecker::visitLambdaExpr(AST::LambdaExpr& expr) {
    // (AR) فحص Lambda
    // (EN) Check lambda expression
}

void BorrowChecker::visitListComprehensionExpr(AST::ListComprehensionExpr& expr) {
    // (AR) فحص استيعاب القائمة
    // (EN) Check list comprehension
}

void BorrowChecker::visitDictComprehensionExpr(AST::DictComprehensionExpr& expr) {
    // (AR) فحص استيعاب القاموس
    // (EN) Check dict comprehension
}

void BorrowChecker::visitSetComprehensionExpr(AST::SetComprehensionExpr& expr) {
    // (AR) فحص استيعاب المجموعة
    // (EN) Check set comprehension
}

void BorrowChecker::visitGeneratorExpr(AST::GeneratorExpr& expr) {
    // (AR) فحص تعبير المولد
    // (EN) Check generator expression
}

void BorrowChecker::visitDecoratorExpr(AST::DecoratorExpr& expr) {
    // (AR) فحص المزخرف
    // (EN) Check decorator
}

void BorrowChecker::visitNewExpr(AST::NewExpr& expr) {
    // (AR) فحص تعبير new
    // (EN) Check new expression
}

void BorrowChecker::visitMemberAccessExpr(AST::MemberAccessExpr& expr) {
    // (AR) فحص الوصول للعضو
    // (EN) Check member access
}

void BorrowChecker::visitMethodCallExpr(AST::MethodCallExpr& expr) {
    // (AR) فحص استدعاء الطريقة
    // (EN) Check method call
}

void BorrowChecker::visitThisExpr(AST::ThisExpr& expr) {
    // (AR) this/هذا لا يحتاج فحص خاص
    // (EN) this doesn't need special checking
    (void)expr;
}

void BorrowChecker::visitSuperExpr(AST::SuperExpr& expr) {
    // (AR) super/أصل لا يحتاج فحص خاص
    // (EN) super doesn't need special checking
    (void)expr;
}

void BorrowChecker::visitBorrowExpr(AST::BorrowExpr& expr) {
    // (AR) فحص تعبير الاستعارة &x أو &متغير x
    // (EN) Check borrow expression &x or &mut x
    if (!expr.variableName.empty()) {
        // (AR) تحديد نوع الاستعارة: مشتركة أو متغيرة
        // (EN) Determine borrow kind: shared or mutable
        BorrowKind kind = expr.isMutable ? BorrowKind::Mutable : BorrowKind::Shared;
        std::string borrowerName = (expr.isMutable ? "&متغير " : "&") + expr.variableName;
        
        // (AR) إنشاء الاستعارة في متتبع الملكية
        // (EN) Create borrow in ownership tracker
        auto error = tracker_->createBorrow(
            expr.variableName, borrowerName,
            kind, getLocation(&expr));
        
        if (error) {
            currentResult_.addError(*error);
        }
        currentResult_.totalBorrows++;

        if (debugMode_) {
            recordWarning("[debug] " + std::string(expr.isMutable ? "Mutable" : "Shared")
                + " borrow of '" + expr.variableName + "' at " + getLocation(&expr).toString());
        }
    }
}

void BorrowChecker::visitInlineAsmExpr(AST::InlineAsmExpr& expr) {
    // (AR) تعبير التجميع المضمّن - لا يحتاج فحص ملكية
    // (EN) Inline assembly expression - no ownership checking needed
    (void)expr;
}

void BorrowChecker::visitRangeExpr(AST::RangeExpr& expr) {
    // (AR) تعبير النطاق — فحص التعبيرات الفرعية
    // (EN) Range expression — check sub-expressions
    if (expr.start) expr.start->accept(*this);
    if (expr.end) expr.end->accept(*this);
}

// ============================================================================
// زيارة العبارات / Visit Statements
// ============================================================================

void BorrowChecker::visitExprStmt(AST::ExprStmt& stmt) {
    // (AR) فحص التعبير داخل العبارة
    // (EN) Check expression inside statement
    if (stmt.expression) {
        stmt.expression->accept(*this);
    }
}

void BorrowChecker::visitVarDeclStmt(AST::VarDeclStmt& stmt) {
    // (AR) تسجيل المتغير الجديد في المتتبع
    // (EN) Register new variable with tracker
    currentResult_.totalVariables++;
    
    // (AR) فحص المُعيِّن أولاً (قبل تسجيل المتغير)
    // (EN) Check initializer first (before registering variable)
    if (stmt.initializer) {
        // (AR) تحليل المُعيِّن - قد يتضمن نقل ملكية
        // (EN) Analyze initializer - may involve ownership transfer
        
        // (AR) إذا كان المُعيِّن متغيراً، فهذا نقل ملكية
        // (EN) If initializer is a variable, this is an ownership move
        auto* varExpr = dynamic_cast<AST::VariableExpr*>(stmt.initializer.get());
        if (varExpr) {
            // (AR) نقل الملكية من المتغير المصدر
            // (EN) Move ownership from source variable
            auto error = tracker_->moveVariable(varExpr->toString(), getLocation(varExpr));
            if (error) {
                currentResult_.addError(*error);
            }
            currentResult_.totalMoves++;
        } else {
            // (AR) فحص التعبير بشكل عام
            // (EN) Check expression generally
            stmt.initializer->accept(*this);
        }
    }
    
    // (AR) استخراج اسم النوع من DataType
    // (EN) Extract type name from DataType
    std::string typeName = dataTypeToString(stmt.type);
    bool isCopy = isCopyType(typeName);
    
    // (AR) إذا كان النوع غير محدد ولكن المُعيِّن قيمة حرفية، استنتج النوع
    // (EN) If type is unspecified but initializer is a literal, infer the type
    if (!isCopy && stmt.initializer) {
        if (auto* lit = dynamic_cast<AST::LiteralExpr*>(stmt.initializer.get())) {
            // (AR) القيم الحرفية (أرقام، عشرية، منطقية) قابلة للنسخ
            // (EN) Literal values (integers, floats, booleans) are copy types
            Data::DataType litType = lit->getType();
            isCopy = true;
            if (litType == Data::DataType::INTEGER) typeName = "رقم";
            else if (litType == Data::DataType::FLOAT) typeName = "عشري";
            else if (litType == Data::DataType::BOOLEAN) typeName = "منطقي";
            else if (litType == Data::DataType::STRING) {
                typeName = "نص";
                isCopy = false;  // Strings are not copy types
            }
        }
    }
    
    // (AR) إذا كان المُعيِّن متغيراً، استنتج النوع من المتغير المصدر
    // (EN) If initializer is a variable, infer type from source variable
    if (!isCopy && stmt.initializer) {
        if (auto* varExpr = dynamic_cast<AST::VariableExpr*>(stmt.initializer.get())) {
            auto srcInfo = tracker_->getOwnershipInfo(varExpr->toString());
            if (srcInfo) {
                typeName = srcInfo->typeName;
                isCopy = srcInfo->isCopyType;
            }
        }
    }
    
    // (AR) إذا لم نتمكن من تحديد النوع، نعتبره قابلاً للنسخ
    // (EN) If we couldn't determine the type, assume it's a copy type
    // (AR) هذا آمن لأن اللغة حالياً تدعم الأنواع البدائية فقط (أرقام، منطقي، نصوص)
    // (EN) This is safe because the language currently only supports primitive types
    if (typeName == "unknown" || typeName.empty()) {
        isCopy = true;
    }
    
    if (debugMode_) {
        std::cout << "[BorrowChecker] Declaring variable: " << stmt.name 
                  << " type: " << typeName << " isCopy: " << isCopy << "\n";
    }
    
    tracker_->declareVariable(stmt.name, typeName, getLocation(&stmt), isCopy);
}

void BorrowChecker::visitIfStmt(AST::IfStmt& stmt) {
    // (AR) فحص الشرط والفروع
    // (EN) Check condition and branches
    if (stmt.condition) {
        stmt.condition->accept(*this);
    }
    tracker_->enterScope();
    if (stmt.thenBranch) {
        stmt.thenBranch->accept(*this);
    }
    tracker_->exitScope();
    if (stmt.elseBranch) {
        tracker_->enterScope();
        stmt.elseBranch->accept(*this);
        tracker_->exitScope();
    }
}

void BorrowChecker::visitWhileStmt(AST::WhileStmt& stmt) {
    // (AR) فحص الحلقة
    // (EN) Check loop
    if (stmt.condition) {
        stmt.condition->accept(*this);
    }
    tracker_->enterScope();
    if (stmt.body) {
        stmt.body->accept(*this);
    }
    tracker_->exitScope();
}

void BorrowChecker::visitForStmt(AST::ForStmt& stmt) {
    // (AR) فحص حلقة for
    // (EN) Check for loop
    tracker_->enterScope();
    if (stmt.initializer) {
        stmt.initializer->accept(*this);
    }
    if (stmt.condition) {
        stmt.condition->accept(*this);
    }
    if (stmt.increment) {
        stmt.increment->accept(*this);
    }
    if (stmt.body) {
        stmt.body->accept(*this);
    }
    tracker_->exitScope();
}

void BorrowChecker::visitForRangeStmt(AST::ForRangeStmt& stmt) {
    // (AR) فحص حلقة for-range
    // (EN) Check for-range loop
    tracker_->enterScope();
    if (stmt.iterable) {
        stmt.iterable->accept(*this);
    }
    if (stmt.body) {
        stmt.body->accept(*this);
    }
    tracker_->exitScope();
}

void BorrowChecker::visitSwitchStmt(AST::SwitchStmt& stmt) {
    // (AR) فحص جملة switch
    // (EN) Check switch statement
}

void BorrowChecker::visitReturnStmt(AST::ReturnStmt& stmt) {
    // (AR) فحص جملة return
    // (EN) Check return statement
    if (stmt.value) {
        stmt.value->accept(*this);
    }
}

void BorrowChecker::visitYieldStmt(AST::YieldStmt& stmt) {
    // (AR) فحص جملة yield
    // (EN) Check yield statement
}

void BorrowChecker::visitBreakStmt(AST::BreakStmt& stmt) {
    // (AR) break لا يحتاج فحص ملكية
    // (EN) break doesn't need ownership checking
    (void)stmt;
}

void BorrowChecker::visitContinueStmt(AST::ContinueStmt& stmt) {
    // (AR) continue لا يحتاج فحص ملكية
    // (EN) continue doesn't need ownership checking
    (void)stmt;
}

void BorrowChecker::visitBlockStmt(AST::BlockStmt& stmt) {
    // (AR) فحص كتلة العبارات
    // (EN) Check block statement
    tracker_->enterScope();
    
    // (AR) زيارة كل عبارة في الكتلة
    // (EN) Visit each statement in the block
    for (auto& s : stmt.statements) {
        if (s) {
            s->accept(*this);
        }
    }
    
    tracker_->exitScope();
}

void BorrowChecker::visitTryStmt(AST::TryStmt& stmt) {
    // (AR) فحص جملة try
    // (EN) Check try statement
}

void BorrowChecker::visitRaiseStmt(AST::RaiseStmt& stmt) {
    // (AR) فحص جملة raise
    // (EN) Check raise statement
}

void BorrowChecker::visitWithStmt(AST::WithStmt& stmt) {
    // (AR) فحص جملة with
    // (EN) Check with statement
}

void BorrowChecker::visitMatchStmt(AST::MatchStmt& stmt) {
    // (AR) فحص جملة match
    // (EN) Check match statement
}

void BorrowChecker::visitClassDeclStmt(AST::ClassDeclStmt& stmt) {
    // (AR) فحص تصريح الصنف
    // (EN) Check class declaration statement
}

// ============================================================================
// زيارة التصريحات / Visit Declarations
// ============================================================================

void BorrowChecker::visitFunctionDecl(AST::FunctionDecl& decl) {
    std::string previousFunction = currentFunction_;
    currentFunction_ = decl.name;
    
    tracker_->enterScope();
    
    // (AR) تسجيل المعاملات كمتغيرات في النطاق
    // (EN) Register parameters as variables in scope
    for (const auto& param : decl.parameters) {
        std::string typeName = dataTypeToString(param.type);
        bool isCopy = isCopyType(typeName);
        tracker_->declareVariable(param.name, typeName, getLocation(&decl), isCopy);
        currentResult_.totalVariables++;
    }
    
    // (AR) فحص جسم الدالة
    // (EN) Check function body
    if (decl.body) {
        decl.body->accept(*this);
    }
    
    tracker_->exitScope();
    currentFunction_ = previousFunction;
}

void BorrowChecker::visitClassDecl(AST::ClassDecl& decl) {
    // (AR) فحص كل طريقة في الصنف
    // (EN) Check each method in class
}

void BorrowChecker::visitFieldDecl(AST::FieldDecl& decl) {
    // (AR) فحص تصريح الحقل
    // (EN) Check field declaration
}

void BorrowChecker::visitMethodDecl(AST::MethodDecl& decl) {
    // (AR) فحص تصريح الطريقة
    // (EN) Check method declaration
}

void BorrowChecker::visitPropertyDecl(AST::PropertyDecl& decl) {
    // (AR) فحص تصريح الخاصية
    // (EN) Check property declaration
}

void BorrowChecker::visitConstructorDecl(AST::ConstructorDecl& decl) {
    // (AR) فحص تصريح البناء
    // (EN) Check constructor declaration
}

void BorrowChecker::visitDestructorDecl(AST::DestructorDecl& decl) {
    // (AR) فحص تصريح الهدم
    // (EN) Check destructor declaration
}

void BorrowChecker::visitEnumDecl(AST::EnumDecl& decl) {
    // (AR) فحص تصريح التعداد
    // (EN) Check enum declaration
}

void BorrowChecker::visitImportStmt(AST::ImportStmt& stmt) {
    // (AR) فحص جملة الاستيراد
    // (EN) Check import statement
    (void)stmt;
}

void BorrowChecker::visitFromImportStmt(AST::FromImportStmt& stmt) {
    // (AR) فحص جملة الاستيراد الانتقائي
    // (EN) Check from-import statement
    (void)stmt;
}

void BorrowChecker::visitExportStmt(AST::ExportStmt& stmt) {
    // (AR) فحص جملة التصدير
    // (EN) Check export statement
    (void)stmt;
}

void BorrowChecker::visitExportDecl(AST::ExportDecl& decl) {
    // (AR) فحص تصريح التصدير
    // (EN) Check export declaration
    (void)decl;
}

void BorrowChecker::visitTemplateFunctionDecl(AST::TemplateFunctionDecl& decl) {
    // (AR) فحص تصريح دالة القالب
    // (EN) Check template function declaration
}

void BorrowChecker::visitTemplateClassDecl(AST::TemplateClassDecl& decl) {
    // (AR) فحص تصريح صنف القالب
    // (EN) Check template class declaration
}

void BorrowChecker::visitTemplateInstantiation(AST::TemplateInstantiation& inst) {
    // (AR) فحص تنفيذ القالب
    // (EN) Check template instantiation
}

void BorrowChecker::visitNamespaceDecl(AST::NamespaceDecl& decl) {
    // (AR) فحص تصريح فضاء الأسماء
    // (EN) Check namespace declaration
}

void BorrowChecker::visitOperatorDecl(AST::OperatorDecl& decl) {
    // (AR) فحص تصريح تحميل العامل
    // (EN) Check operator overload declaration
}

void BorrowChecker::visitTraitDecl(AST::TraitDecl& decl) {
    // (AR) فحص تصريح السمة — تحقق من الملكية في الدوال المعلنة
    // (EN) Check trait declaration — verify ownership in declared methods
}

void BorrowChecker::visitImplDecl(AST::ImplDecl& decl) {
    // (AR) فحص تنفيذ السمة — تحليل أجسام الدوال
    // (EN) Check trait implementation — analyze method bodies
    for (auto& method : decl.methods) {
        if (method) method->accept(*this);
    }
}

void BorrowChecker::visitStructDecl(AST::StructDecl& decl) {
    // (AR) فحص تصريح البنية — لا فحوصات ملكية حالياً
    // (EN) Check struct declaration — no ownership checks currently
}

void BorrowChecker::visitTestDecl(AST::TestDecl& decl) {
    // (AR) فحص تصريح الاختبار — تحليل جسم الاختبار
    // (EN) Check test declaration — analyze test body
    if (decl.body) decl.body->accept(*this);
}

// ============================================================================
// دوال مساعدة / Helper Functions
// ============================================================================

void BorrowChecker::analyzeExpression(AST::Expression* expr, bool isMoveContext) {
    if (!expr) return;
    
    // (AR) إذا كان متغيراً وسياق نقل
    // (EN) If variable and move context
    if (isMoveContext) {
        auto error = tracker_->moveVariable(
            expr->toString(),
            getLocation(expr)
        );
        if (error) {
            currentResult_.addError(*error);
        }
        currentResult_.totalMoves++;
    } else {
        // (AR) قراءة فقط
        // (EN) Read only
        auto error = tracker_->useVariable(
            expr->toString(),
            getLocation(expr)
        );
        if (error) {
            currentResult_.addError(*error);
        }
    }
}

void BorrowChecker::analyzeAssignment(AST::AssignExpr* assign) {
    if (!assign) return;
    
    // (AR) فحص الطرف الأيمن أولاً
    // (EN) Check right side first
    if (assign->value) {
        // (AR) إذا كان المصدر متغيراً، فهذا نقل ملكية
        // (EN) If source is a variable, this is an ownership move
        auto* varExpr = dynamic_cast<AST::VariableExpr*>(assign->value.get());
        if (varExpr) {
            auto error = tracker_->moveVariable(varExpr->toString(), getLocation(varExpr));
            if (error) {
                currentResult_.addError(*error);
            }
            currentResult_.totalMoves++;
        } else {
            assign->value->accept(*this);
        }
    }
    
    // (AR) فحص الطرف الأيسر (التعديل)
    // (EN) Check left side (mutation)
    auto error = tracker_->mutateVariable(
        assign->name,
        getLocation(assign)
    );
    if (error) {
        currentResult_.addError(*error);
    }
}

void BorrowChecker::analyzeFunctionCall(AST::CallExpr* call) {
    if (!call) return;
    
    // (AR) فحص المستدعى
    // (EN) Check callee
    if (call->callee) {
        call->callee->accept(*this);
    }
    
    // (AR) فحص كل معامل - كل معامل يُقرأ (وقد يُنقل)
    // (EN) Check each argument - each is read (and may be moved)
    for (auto& arg : call->arguments) {
        if (arg) {
            arg->accept(*this);
        }
    }
}

bool BorrowChecker::isCopyType(const std::string& typeName) const {
    // (AR) التحقق من القائمة
    // (EN) Check the list
    if (copyTypes_.find(typeName) != copyTypes_.end()) {
        return true;
    }
    
    // (AR) المراجع ليست قابلة للنسخ (تُستنسخ المراجع نفسها)
    // (EN) References are Copy (the references themselves are copied)
    if (!typeName.empty() && typeName.front() == '&') {
        return true;
    }
    if (typeName.size() >= 4 && typeName.substr(0, 4) == "مرجع") {
        return true;
    }
    
    return false;
}

std::string BorrowChecker::dataTypeToString(Data::DataType type) const {
    // (AR) تحويل نوع البيانات إلى نص لمطابقة أنواع النسخ
    // (EN) Convert DataType enum to string for copy type matching
    switch (type) {
        case Data::DataType::INTEGER:   return "رقم";
        case Data::DataType::FLOAT:     return "عشري";
        case Data::DataType::BOOLEAN:   return "منطقي";
        case Data::DataType::STRING:    return "نص";
        case Data::DataType::BYTE:      return "بايت";
        case Data::DataType::ARRAY:     return "مصفوفة";
        case Data::DataType::MAP:       return "قاموس";
        case Data::DataType::TUPLE:     return "ثنائي";
        case Data::DataType::FUNCTION:  return "دالة";
        case Data::DataType::NONE:      return "لاشيء";
        case Data::DataType::ENUM:      return "تعداد";
        case Data::DataType::ERROR:     return "خطأ";
        default:                        return "unknown";
    }
}

SourceLocation BorrowChecker::getLocation(AST::ASTNode* node) const {
    if (!node) {
        return SourceLocation();
    }
    
    // TODO: Get actual location from node position
    return SourceLocation(currentFile_, 
                          node->position.line, 
                          node->position.column);
}

void BorrowChecker::recordError(const OwnershipError& error) {
    currentResult_.addError(error);
    
    if (debugMode_) {
        if (useArabicMessages_) {
            std::cerr << error.toArabicString();
        } else {
            std::cerr << error.toEnglishString();
        }
    }
}

void BorrowChecker::recordWarning(const std::string& warning) {
    currentResult_.addWarning(warning);
    
    if (debugMode_) {
        std::cerr << "[تحذير/Warning] " << warning << "\n";
    }
}

} // namespace Semantic
} // namespace Sad
