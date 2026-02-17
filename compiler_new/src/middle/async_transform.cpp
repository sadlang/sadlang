/**
 * @file async_transform.cpp
 * @brief (AR) تحويل الدوال غير المتزامنة إلى آلات حالة
 *        (EN) Async Function Transformation to State Machines
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║    ████████╗██████╗  █████╗ ███╗   ██╗███████╗███████╗ ██████╗ ██████╗ ███╗   ███╗║
 * ║    ╚══██╔══╝██╔══██╗██╔══██╗████╗  ██║██╔════╝██╔════╝██╔═══██╗██╔══██╗████╗ ████║║
 * ║       ██║   ██████╔╝███████║██╔██╗ ██║███████╗█████╗  ██║   ██║██████╔╝██╔████╔██║║
 * ║       ██║   ██╔══██╗██╔══██║██║╚██╗██║╚════██║██╔══╝  ██║   ██║██╔══██╗██║╚██╔╝██║║
 * ║       ██║   ██║  ██║██║  ██║██║ ╚████║███████║██║     ╚██████╔╝██║  ██║██║ ╚═╝ ██║║
 * ║       ╚═╝   ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚═╝      ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝║
 * ║                                                                               ║
 * ║              تحويل async إلى آلات حالة                                        ║
 * ║              Async to State Machine Transformation                            ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يُحوّل الدوال غير المتزامنة إلى آلات حالة:
 * 
 *      ┌─────────────────────────────────────────────────────────────────────────┐
 *      │                     التحويل من async إلى آلة حالة                        │
 *      ├─────────────────────────────────────────────────────────────────────────┤
 *      │                                                                         │
 *      │    // الكود الأصلي                                                       │
 *      │    غير_متزامن دالة جلب() -> عدد {                                        │
 *      │        دع أ = انتظر جلب_أ()؛                                             │
 *      │        دع ب = انتظر جلب_ب()؛                                             │
 *      │        أ + ب                                                            │
 *      │    }                                                                    │
 *      │                                                                         │
 *      │    // الكود المُحوَّل (داخلياً)                                          │
 *      │    هيكل آلة_جلب {                                                       │
 *      │        حالة: حالة_جلب،                                                  │
 *      │        أ: اختياري<عدد>،                                                  │
 *      │        ب: اختياري<عدد>،                                                  │
 *      │        مستقبل_أ: اختياري<مستقبل<عدد>>،                                   │
 *      │        مستقبل_ب: اختياري<مستقبل<عدد>>،                                   │
 *      │    }                                                                    │
 *      │                                                                         │
 *      │    تعداد حالة_جلب {                                                      │
 *      │        بداية،                                                           │
 *      │        انتظار_أ،                                                         │
 *      │        انتظار_ب،                                                         │
 *      │        منتهية،                                                          │
 *      │    }                                                                    │
 *      │                                                                         │
 *      └─────────────────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#include "async_transform.hpp"
// (AR) كل الأنواع المطلوبة (ast::*, types::*, errors::*) معرّفة في async_transform.hpp
// (EN) All required types (ast::*, types::*, errors::*) are defined in async_transform.hpp

#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

namespace sad::middle {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) حالة آلة async
//                    (EN) Async State Machine State
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) نوع حالة آلة async
 */
enum class AsyncStateKind {
    START,          // (AR) حالة البداية
    AWAIT_POINT,    // (AR) نقطة انتظار
    YIELD_POINT,    // (AR) نقطة إنتاج (للمولدات)
    BRANCH,         // (AR) تفرع (if/match)
    LOOP,           // (AR) حلقة
    END             // (AR) حالة النهاية
};

/**
 * (AR) حالة في آلة async
 */
struct AsyncState {
    int id;                             // (AR) معرف الحالة
    std::string name;                   // (AR) اسم الحالة للتصحيح
    AsyncStateKind kind;                // (AR) نوع الحالة
    std::vector<ast::ASTNode*> code;    // (AR) الكود المُنفَّذ في هذه الحالة
    int next_state;                     // (AR) الحالة التالية (-1 للنهاية)
    std::optional<int> alt_state;       // (AR) حالة بديلة (للتفرعات)
    
    AsyncState()
        : id(-1)
        , kind(AsyncStateKind::START)
        , next_state(-1) {}
};

/**
 * (AR) متغير مُلتقط
 */
struct CapturedVariable {
    std::string name;                   // (AR) اسم المتغير
    std::unique_ptr<types::Type> type;  // (AR) نوع المتغير
    bool is_mutable;                    // (AR) هل قابل للتغيير؟
    int defined_in_state;               // (AR) الحالة التي عُرِّف فيها
    int last_used_state;                // (AR) آخر حالة استُخدم فيها
};

/**
 * (AR) نقطة انتظار
 */
struct AwaitPoint {
    int state_id;                       // (AR) معرف الحالة
    std::string future_var;             // (AR) اسم متغير المستقبل
    std::string result_var;             // (AR) اسم متغير النتيجة
    std::unique_ptr<types::Type> result_type;  // (AR) نوع النتيجة
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) آلة حالة async
//                    (EN) Async State Machine
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) آلة حالة async
 */
class AsyncStateMachine {
public:
    std::string name;                                   // (AR) اسم الآلة
    std::vector<AsyncState> states;                     // (AR) الحالات
    std::vector<CapturedVariable> captured_vars;        // (AR) المتغيرات المُلتقطة
    std::vector<AwaitPoint> await_points;               // (AR) نقاط الانتظار
    std::unique_ptr<types::Type> output_type;           // (AR) نوع الناتج
    bool is_generator;                                  // (AR) هل هذا مولد؟
    
    AsyncStateMachine() : is_generator(false) {
        // (AR) إضافة حالة البداية
        AsyncState start;
        start.id = 0;
        start.name = "بداية";
        start.kind = AsyncStateKind::START;
        states.push_back(std::move(start));
    }
    
    /**
     * (AR) إضافة حالة جديدة
     */
    int addState(AsyncStateKind kind, const std::string& name = "") {
        AsyncState state;
        state.id = static_cast<int>(states.size());
        state.kind = kind;
        state.name = name.empty() ? "حالة_" + std::to_string(state.id) : name;
        states.push_back(std::move(state));
        return state.id;
    }
    
    /**
     * (AR) إضافة نقطة انتظار
     */
    int addAwaitPoint(
        const std::string& future_var,
        const std::string& result_var,
        std::unique_ptr<types::Type> result_type
    ) {
        int state_id = addState(AsyncStateKind::AWAIT_POINT, "انتظار_" + result_var);
        
        AwaitPoint point;
        point.state_id = state_id;
        point.future_var = future_var;
        point.result_var = result_var;
        point.result_type = std::move(result_type);
        await_points.push_back(std::move(point));
        
        return state_id;
    }
    
    /**
     * (AR) إضافة متغير مُلتقط
     */
    void addCapturedVariable(
        const std::string& name,
        std::unique_ptr<types::Type> type,
        bool is_mutable,
        int defined_in_state
    ) {
        CapturedVariable var;
        var.name = name;
        var.type = std::move(type);
        var.is_mutable = is_mutable;
        var.defined_in_state = defined_in_state;
        var.last_used_state = defined_in_state;
        captured_vars.push_back(std::move(var));
    }
    
    /**
     * (AR) تحديث آخر استخدام لمتغير
     */
    void updateVariableUsage(const std::string& name, int state_id) {
        for (auto& var : captured_vars) {
            if (var.name == name) {
                var.last_used_state = std::max(var.last_used_state, state_id);
                break;
            }
        }
    }
    
    /**
     * (AR) إنهاء الآلة
     */
    void finalize() {
        // (AR) إضافة حالة النهاية
        addState(AsyncStateKind::END, "نهاية");
        
        // (AR) ربط الحالات
        for (size_t i = 0; i < states.size() - 1; ++i) {
            if (states[i].next_state == -1) {
                states[i].next_state = static_cast<int>(i + 1);
            }
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مُحوِّل async
//                    (EN) Async Transformer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) مُحوِّل الدوال غير المتزامنة
 */
class AsyncTransformer {
private:
    errors::ErrorReporter& error_reporter_;
    std::unique_ptr<AsyncStateMachine> current_machine_;
    int current_state_id_;
    std::unordered_set<std::string> defined_vars_;
    
public:
    explicit AsyncTransformer(errors::ErrorReporter& reporter)
        : error_reporter_(reporter)
        , current_state_id_(0) {}
    
    /**
     * (AR) تحويل دالة async إلى آلة حالة
     */
    std::unique_ptr<AsyncStateMachine> transform(
        const ast::AsyncFunctionNode& func
    ) {
        current_machine_ = std::make_unique<AsyncStateMachine>();
        current_machine_->name = func.name + "_آلة";
        current_machine_->is_generator = (func.func_kind == ast::AsyncFunctionKind::ASYNC_GENERATOR);
        
        if (func.return_type) {
            current_machine_->output_type = extractOutputType(func.return_type.get());
        }
        
        // (AR) تحويل المعاملات إلى متغيرات مُلتقطة
        for (const auto& param : func.parameters) {
            current_machine_->addCapturedVariable(
                param.name,
                param.type ? param.type->clone() : nullptr,
                false,  // (AR) المعاملات ليست قابلة للتغيير افتراضياً
                0       // (AR) معرفة في حالة البداية
            );
            defined_vars_.insert(param.name);
        }
        
        // (AR) تحويل جسم الدالة
        if (func.body) {
            transformBlock(*func.body);
        }
        
        current_machine_->finalize();
        return std::move(current_machine_);
    }
    
private:
    /**
     * (AR) تحويل كتلة كود
     */
    void transformBlock(const ast::BlockNode& block) {
        for (const auto& stmt : block.statements) {
            transformStatement(stmt.get());
        }
    }
    
    /**
     * (AR) تحويل عبارة
     */
    void transformStatement(const ast::ASTNode* stmt) {
        if (!stmt) return;
        
        switch (stmt->getKind()) {
            case ast::NodeKind::LetDecl:
                transformLetDecl(static_cast<const ast::LetDeclNode*>(stmt));
                break;
                
            case ast::NodeKind::ExprStmt:
                transformExprStmt(static_cast<const ast::ExprStmtNode*>(stmt));
                break;
                
            case ast::NodeKind::If:
                transformIf(static_cast<const ast::IfNode*>(stmt));
                break;
                
            case ast::NodeKind::While:
                transformWhile(static_cast<const ast::WhileNode*>(stmt));
                break;
                
            case ast::NodeKind::For:
                transformFor(static_cast<const ast::ForNode*>(stmt));
                break;
                
            case ast::NodeKind::Return:
                transformReturn(static_cast<const ast::ReturnNode*>(stmt));
                break;
                
            default:
                // (AR) إضافة العبارة للحالة الحالية مباشرة
                addToCurrentState(const_cast<ast::ASTNode*>(stmt));
                break;
        }
    }
    
    /**
     * (AR) تحويل تعريف متغير
     */
    void transformLetDecl(const ast::LetDeclNode* decl) {
        if (!decl) return;
        
        // (AR) تحقق من وجود await في التعبير
        if (decl->initializer && containsAwait(decl->initializer.get())) {
            // (AR) استخراج await وإنشاء نقطة انتظار
            transformAwaitInLet(decl);
        } else {
            // (AR) إضافة التعريف للحالة الحالية
            current_machine_->addCapturedVariable(
                decl->name,
                decl->type ? decl->type->clone() : nullptr,
                decl->is_mutable,
                current_state_id_
            );
            defined_vars_.insert(decl->name);
            addToCurrentState(const_cast<ast::LetDeclNode*>(decl));
        }
    }
    
    /**
     * (AR) تحويل تعريف يحتوي await
     */
    void transformAwaitInLet(const ast::LetDeclNode* decl) {
        // (AR) إنشاء نقطة انتظار
        std::string future_var = "_مستقبل_" + decl->name;
        
        int await_state = current_machine_->addAwaitPoint(
            future_var,
            decl->name,
            decl->type ? decl->type->clone() : nullptr
        );
        
        // (AR) ربط الحالة الحالية بنقطة الانتظار
        current_machine_->states[current_state_id_].next_state = await_state;
        
        // (AR) إضافة المتغير كمُلتقط
        current_machine_->addCapturedVariable(
            decl->name,
            decl->type ? decl->type->clone() : nullptr,
            decl->is_mutable,
            await_state
        );
        defined_vars_.insert(decl->name);
        
        // (AR) الانتقال للحالة التالية
        current_state_id_ = await_state + 1;
    }
    
    /**
     * (AR) تحويل عبارة تعبير
     */
    void transformExprStmt(const ast::ExprStmtNode* stmt) {
        if (!stmt || !stmt->expression) return;
        
        if (containsAwait(stmt->expression.get())) {
            // (AR) إنشاء نقطة انتظار
            std::string temp_var = "_نتيجة_" + std::to_string(current_state_id_);
            int await_state = current_machine_->addAwaitPoint(
                "_مستقبل_" + std::to_string(current_state_id_),
                temp_var,
                nullptr
            );
            current_machine_->states[current_state_id_].next_state = await_state;
            current_state_id_ = await_state + 1;
        } else {
            addToCurrentState(const_cast<ast::ExprStmtNode*>(stmt));
        }
    }
    
    /**
     * (AR) تحويل عبارة if
     */
    void transformIf(const ast::IfNode* if_node) {
        if (!if_node) return;
        
        // (AR) إنشاء حالة للتفرع
        int branch_state = current_machine_->addState(
            AsyncStateKind::BRANCH,
            "تفرع_" + std::to_string(current_state_id_)
        );
        
        current_machine_->states[current_state_id_].next_state = branch_state;
        
        // (AR) حفظ الحالة الحالية
        int saved_state = current_state_id_;
        
        // (AR) تحويل الفرع الصحيح
        current_state_id_ = current_machine_->addState(AsyncStateKind::START, "فرع_صحيح");
        current_machine_->states[branch_state].next_state = current_state_id_;
        
        if (if_node->then_block) {
            transformBlock(*if_node->then_block);
        }
        int then_end = current_state_id_;
        
        // (AR) تحويل الفرع البديل
        if (if_node->else_block) {
            current_state_id_ = current_machine_->addState(AsyncStateKind::START, "فرع_خاطئ");
            current_machine_->states[branch_state].alt_state = current_state_id_;
            transformBlock(*if_node->else_block);
        }
        int else_end = current_state_id_;
        
        // (AR) إنشاء حالة الاندماج
        int merge_state = current_machine_->addState(AsyncStateKind::START, "اندماج");
        current_machine_->states[then_end].next_state = merge_state;
        current_machine_->states[else_end].next_state = merge_state;
        
        current_state_id_ = merge_state;
    }
    
    /**
     * (AR) تحويل حلقة while
     */
    void transformWhile(const ast::WhileNode* while_node) {
        if (!while_node) return;
        
        int loop_state = current_machine_->addState(
            AsyncStateKind::LOOP,
            "حلقة_" + std::to_string(current_state_id_)
        );
        
        current_machine_->states[current_state_id_].next_state = loop_state;
        current_state_id_ = loop_state;
        
        // (AR) تحويل جسم الحلقة
        if (while_node->body) {
            transformBlock(*while_node->body);
        }
        
        // (AR) العودة لبداية الحلقة
        current_machine_->states[current_state_id_].next_state = loop_state;
        
        // (AR) إنشاء حالة الخروج
        int exit_state = current_machine_->addState(AsyncStateKind::START, "خروج_حلقة");
        current_machine_->states[loop_state].alt_state = exit_state;
        current_state_id_ = exit_state;
    }
    
    /**
     * (AR) تحويل حلقة for
     */
    void transformFor(const ast::ForNode* for_node) {
        // (AR) تحويل مشابه لـ while مع إضافة متغير التكرار
        if (!for_node) return;
        
        // (AR) إضافة متغير التكرار
        current_machine_->addCapturedVariable(
            for_node->variable,
            nullptr,
            true,
            current_state_id_
        );
        defined_vars_.insert(for_node->variable);
        
        int loop_state = current_machine_->addState(
            AsyncStateKind::LOOP,
            "حلقة_for_" + std::to_string(current_state_id_)
        );
        
        current_machine_->states[current_state_id_].next_state = loop_state;
        current_state_id_ = loop_state;
        
        if (for_node->body) {
            transformBlock(*for_node->body);
        }
        
        current_machine_->states[current_state_id_].next_state = loop_state;
        
        int exit_state = current_machine_->addState(AsyncStateKind::START, "خروج_for");
        current_machine_->states[loop_state].alt_state = exit_state;
        current_state_id_ = exit_state;
    }
    
    /**
     * (AR) تحويل عبارة return
     */
    void transformReturn(const ast::ReturnNode* ret) {
        if (!ret) return;
        
        if (ret->value && containsAwait(ret->value.get())) {
            // (AR) await في return
            std::string temp_var = "_نتيجة_نهائية";
            int await_state = current_machine_->addAwaitPoint(
                "_مستقبل_نهائي",
                temp_var,
                current_machine_->output_type ? current_machine_->output_type->clone() : nullptr
            );
            current_machine_->states[current_state_id_].next_state = await_state;
            current_state_id_ = await_state;
        }
        
        // (AR) الانتقال لحالة النهاية
        addToCurrentState(const_cast<ast::ReturnNode*>(ret));
        current_machine_->states[current_state_id_].next_state = -1; // النهاية
    }
    
    /**
     * (AR) هل التعبير يحتوي await؟
     * (AR) تقبل أي عقدة ASTNode لتسهيل التحقق العام
     */
    bool containsAwait(const ast::ASTNode* expr) const {
        if (!expr) return false;
        
        if (expr->getKind() == ast::NodeKind::AwaitExpr) {
            return true;
        }
        
        // (AR) فحص التعبيرات الفرعية (يحتاج تنفيذ كامل)
        return false;
    }
    
    /**
     * (AR) إضافة عقدة للحالة الحالية
     */
    void addToCurrentState(ast::ASTNode* node) {
        if (current_state_id_ < static_cast<int>(current_machine_->states.size())) {
            current_machine_->states[current_state_id_].code.push_back(node);
        }
    }
    
    /**
     * (AR) استخراج نوع الناتج من نوع المستقبل
     */
    std::unique_ptr<types::Type> extractOutputType(const ast::TypeNode* type) {
        // (AR) إذا كان مستقبل<T>، استخرج T
        // التنفيذ يعتمد على بنية TypeNode
        if (type) {
            return type->clone();
        }
        return nullptr;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) مولد كود آلة الحالة
//                    (EN) State Machine Code Generator
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) مولد كود آلة الحالة
 */
class StateMachineCodeGenerator {
private:
    const AsyncStateMachine& machine_;
    
public:
    explicit StateMachineCodeGenerator(const AsyncStateMachine& machine)
        : machine_(machine) {}
    
    /**
     * (AR) توليد هيكل آلة الحالة
     */
    std::unique_ptr<ast::StructNode> generateStateStruct() {
        auto struct_node = std::make_unique<ast::StructNode>();
        struct_node->name = machine_.name;
        
        // (AR) حقل الحالة
        ast::StructField state_field;
        state_field.name = "حالة";
        state_field.type = std::make_unique<ast::IdentifierTypeNode>(machine_.name + "_حالة");
        struct_node->fields.push_back(std::move(state_field));
        
        // (AR) حقول المتغيرات المُلتقطة
        for (const auto& var : machine_.captured_vars) {
            ast::StructField field;
            field.name = var.name;
            if (var.type) {
                // (AR) تغليف بـ اختياري لأن القيمة قد لا تكون موجودة في كل الحالات
                auto opt_type = std::make_unique<ast::GenericTypeNode>();
                opt_type->name = "اختياري";
                opt_type->type_arguments.push_back(var.type->clone());
                field.type = std::move(opt_type);
            }
            struct_node->fields.push_back(std::move(field));
        }
        
        // (AR) حقول المستقبلات المُنتظرة
        for (const auto& await : machine_.await_points) {
            ast::StructField field;
            field.name = await.future_var;
            auto opt_type = std::make_unique<ast::GenericTypeNode>();
            opt_type->name = "اختياري";
            // (AR) نوع المستقبل
            auto future_type = std::make_unique<ast::GenericTypeNode>();
            future_type->name = "مستقبل";
            if (await.result_type) {
                future_type->type_arguments.push_back(await.result_type->clone());
            }
            opt_type->type_arguments.push_back(std::move(future_type));
            field.type = std::move(opt_type);
            struct_node->fields.push_back(std::move(field));
        }
        
        return struct_node;
    }
    
    /**
     * (AR) توليد تعداد الحالات
     */
    std::unique_ptr<ast::EnumNode> generateStateEnum() {
        auto enum_node = std::make_unique<ast::EnumNode>();
        enum_node->name = machine_.name + "_حالة";
        
        for (const auto& state : machine_.states) {
            ast::EnumVariant variant;
            variant.name = state.name;
            enum_node->variants.push_back(std::move(variant));
        }
        
        return enum_node;
    }
    
    /**
     * (AR) توليد تطبيق سمة مستقبل
     */
    std::unique_ptr<ast::ImplNode> generateFutureImpl() {
        auto impl = std::make_unique<ast::ImplNode>();
        impl->trait_name = "مستقبل";
        impl->type_name = machine_.name;
        
        // (AR) النوع المرتبط: ناتج
        ast::AssociatedTypeDecl output_decl;
        output_decl.name = "ناتج";
        if (machine_.output_type) {
            output_decl.type = machine_.output_type->clone();
        }
        impl->associated_types.push_back(std::move(output_decl));
        
        // (AR) دالة استطلع
        impl->methods.push_back(generatePollMethod());
        
        return impl;
    }
    
    /**
     * (AR) توليد دالة استطلع
     */
    std::unique_ptr<ast::FunctionNode> generatePollMethod() {
        auto func = std::make_unique<ast::FunctionNode>();
        func->name = "استطلع";
        
        // (AR) المعاملات: &mut self, cx: &mut Context
        ast::Parameter self_param;
        self_param.name = "ذاتي";
        self_param.is_mut_ref = true;
        func->parameters.push_back(std::move(self_param));
        
        ast::Parameter cx_param;
        cx_param.name = "سياق";
        cx_param.type = std::make_unique<ast::RefTypeNode>(
            std::make_unique<ast::IdentifierTypeNode>("سياق"),
            true
        );
        func->parameters.push_back(std::move(cx_param));
        
        // (AR) نوع الإرجاع: Poll<Self::Output>
        auto return_type = std::make_unique<ast::GenericTypeNode>();
        return_type->name = "حالة_استطلاع";
        if (machine_.output_type) {
            return_type->type_arguments.push_back(machine_.output_type->clone());
        }
        func->return_type = std::move(return_type);
        
        // (AR) جسم الدالة: match على الحالة
        func->body = generatePollBody();
        
        return func;
    }
    
    /**
     * (AR) توليد جسم دالة استطلع
     */
    std::unique_ptr<ast::BlockNode> generatePollBody() {
        auto body = std::make_unique<ast::BlockNode>();
        
        // (AR) match ذاتي.حالة { ... }
        auto match_expr = std::make_unique<ast::MatchNode>();
        match_expr->scrutinee = std::make_unique<ast::FieldAccessNode>(
            std::make_unique<ast::IdentifierNode>("ذاتي"),
            "حالة"
        );
        
        // (AR) إضافة ذراع لكل حالة
        for (const auto& state : machine_.states) {
            ast::MatchArm arm;
            arm.pattern = std::make_unique<ast::EnumPatternNode>(
                machine_.name + "_حالة",
                state.name
            );
            arm.body = generateStateBody(state);
            match_expr->arms.push_back(std::move(arm));
        }
        
        auto match_stmt = std::make_unique<ast::ExprStmtNode>();
        match_stmt->expression = std::move(match_expr);
        body->statements.push_back(std::move(match_stmt));
        
        return body;
    }
    
    /**
     * (AR) توليد كود حالة معينة
     */
    std::unique_ptr<ast::BlockNode> generateStateBody(const AsyncState& state) {
        auto body = std::make_unique<ast::BlockNode>();
        
        switch (state.kind) {
            case AsyncStateKind::START:
            case AsyncStateKind::BRANCH:
            case AsyncStateKind::LOOP:
                // (AR) تنفيذ الكود والانتقال للحالة التالية
                for (auto* code : state.code) {
                    // (AR) نسخ الكود (يحتاج تنفيذ clone)
                }
                generateStateTransition(body.get(), state.next_state);
                break;
                
            case AsyncStateKind::AWAIT_POINT:
                // (AR) استطلاع المستقبل
                generateAwaitPoll(body.get(), state);
                break;
                
            case AsyncStateKind::END:
                // (AR) إرجاع Ready
                generateReadyReturn(body.get());
                break;
                
            default:
                break;
        }
        
        return body;
    }
    
    /**
     * (AR) توليد انتقال الحالة
     */
    void generateStateTransition(ast::BlockNode* body, int next_state) {
        if (next_state >= 0 && next_state < static_cast<int>(machine_.states.size())) {
            // ذاتي.حالة = حالة_جديدة;
            auto assign = std::make_unique<ast::AssignNode>();
            assign->target = std::make_unique<ast::FieldAccessNode>(
                std::make_unique<ast::IdentifierNode>("ذاتي"),
                "حالة"
            );
            assign->value = std::make_unique<ast::EnumVariantNode>(
                machine_.name + "_حالة",
                machine_.states[next_state].name
            );
            body->statements.push_back(std::make_unique<ast::ExprStmtNode>(std::move(assign)));
            
            // (AR) استدعاء استطلع مرة أخرى
            auto self_poll = std::make_unique<ast::CallNode>();
            self_poll->callee = std::make_unique<ast::MethodCallNode>(
                std::make_unique<ast::IdentifierNode>("ذاتي"),
                "استطلع"
            );
            self_poll->arguments.push_back(std::make_unique<ast::IdentifierNode>("سياق"));
            
            auto return_stmt = std::make_unique<ast::ReturnNode>();
            return_stmt->value = std::move(self_poll);
            body->statements.push_back(std::move(return_stmt));
        }
    }
    
    /**
     * (AR) توليد استطلاع await
     */
    void generateAwaitPoll(ast::BlockNode* body, const AsyncState& state) {
        // (AR) البحث عن نقطة الانتظار المناسبة
        for (const auto& await : machine_.await_points) {
            if (await.state_id == state.id) {
                // match pin!(self.future).poll(cx) { ... }
                auto future_poll = std::make_unique<ast::MatchNode>();
                
                auto poll_call = std::make_unique<ast::MethodCallNode>(
                    std::make_unique<ast::MacroCallNode>(
                        "pin",
                        std::make_unique<ast::FieldAccessNode>(
                            std::make_unique<ast::IdentifierNode>("ذاتي"),
                            await.future_var
                        )
                    ),
                    "استطلع"
                );
                poll_call->arguments.push_back(std::make_unique<ast::IdentifierNode>("سياق"));
                future_poll->scrutinee = std::move(poll_call);
                
                // Ready(val) => ...
                ast::MatchArm ready_arm;
                {
                    // (AR) بناء أنماط الربط بدون initializer_list (unique_ptr لا يُنسخ)
                    std::vector<std::unique_ptr<ast::PatternNode>> patterns;
                    patterns.push_back(std::make_unique<ast::BindingPatternNode>(await.result_var));
                    ready_arm.pattern = std::make_unique<ast::EnumPatternNode>(
                        "حالة_استطلاع",
                        "جاهز",
                        std::move(patterns)
                    );
                }
                ready_arm.body = std::make_unique<ast::BlockNode>();
                // (AR) حفظ النتيجة والانتقال للحالة التالية
                generateStateTransition(
                    static_cast<ast::BlockNode*>(ready_arm.body.get()),
                    state.next_state
                );
                future_poll->arms.push_back(std::move(ready_arm));
                
                // Pending => return Pending
                ast::MatchArm pending_arm;
                pending_arm.pattern = std::make_unique<ast::EnumPatternNode>(
                    "حالة_استطلاع",
                    "معلق"
                );
                auto return_pending = std::make_unique<ast::ReturnNode>();
                return_pending->value = std::make_unique<ast::EnumVariantNode>(
                    "حالة_استطلاع",
                    "معلق"
                );
                pending_arm.body = std::move(return_pending);
                future_poll->arms.push_back(std::move(pending_arm));
                
                body->statements.push_back(
                    std::make_unique<ast::ExprStmtNode>(std::move(future_poll))
                );
                break;
            }
        }
    }
    
    /**
     * (AR) توليد إرجاع Ready
     */
    void generateReadyReturn(ast::BlockNode* body) {
        auto return_stmt = std::make_unique<ast::ReturnNode>();
        return_stmt->value = std::make_unique<ast::EnumVariantNode>(
            "حالة_استطلاع",
            "جاهز"
            // (AR) القيمة النهائية تُضاف هنا
        );
        body->statements.push_back(std::move(return_stmt));
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) نقاط الدخول العامة
//                    (EN) Public Entry Points
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) تحويل دالة async
 */
std::unique_ptr<AsyncStateMachine> transformAsyncFunction(
    const ast::AsyncFunctionNode& func,
    errors::ErrorReporter& reporter
) {
    AsyncTransformer transformer(reporter);
    return transformer.transform(func);
}

/**
 * (AR) توليد كود آلة الحالة
 */
void generateStateMachineCode(
    const AsyncStateMachine& machine,
    ast::ModuleNode& output_module
) {
    StateMachineCodeGenerator generator(machine);
    
    // (AR) إضافة الهيكل والتعداد والتطبيق
    output_module.items.push_back(generator.generateStateEnum());
    output_module.items.push_back(generator.generateStateStruct());
    output_module.items.push_back(generator.generateFutureImpl());
}

} // namespace sad::middle
