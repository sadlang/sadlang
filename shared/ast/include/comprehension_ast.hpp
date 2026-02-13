/**
 * @file comprehension_ast.hpp
 * @brief (AR) عقد AST لتعبيرات الفهم
 *        (EN) AST Nodes for Comprehension Expressions
 * 
 * @details
 * ╔═══════════════════════════════════════════════════════════════════════════════╗
 * ║                                                                               ║
 * ║     █████╗ ███████╗████████╗                                                  ║
 * ║    ██╔══██╗██╔════╝╚══██╔══╝                                                  ║
 * ║    ███████║███████╗   ██║                                                     ║
 * ║    ██╔══██║╚════██║   ██║                                                     ║
 * ║    ██║  ██║███████║   ██║                                                     ║
 * ║    ╚═╝  ╚═╝╚══════╝   ╚═╝                                                     ║
 * ║                                                                               ║
 * ║              عقد الشجرة التركيبية المجردة للفهم                               ║
 * ║              Abstract Syntax Tree Nodes for Comprehensions                    ║
 * ║                                                                               ║
 * ╚═══════════════════════════════════════════════════════════════════════════════╝
 * 
 * (AR) هذا الملف يعرّف عقد AST لتعبيرات الفهم:
 *      - فهم القوائم
 *      - فهم القواميس
 *      - فهم المجموعات
 *      - تعبيرات المولدات
 *      - الأنابيب (Pipelines)
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2024-01-24
 * @version 1.0.0
 */

#ifndef SAD_AST_COMPREHENSION_HPP
#define SAD_AST_COMPREHENSION_HPP

#include "ast.hpp"
#include <memory>
#include <vector>
#include <string>
#include <optional>

namespace sad::ast {

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) أنواع عقد الفهم
//                    (EN) Comprehension Node Kinds
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) نوع تعبير الفهم
 */
enum class ComprehensionType {
    LIST,           // (AR) [تعبير لكل ...]
    DICT,           // (AR) {مفتاح: قيمة لكل ...}
    SET,            // (AR) {|تعبير لكل ...|}
    GENERATOR       // (AR) (تعبير لكل ...)
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقدة الحلقة في الفهم
//                    (EN) Comprehension Loop Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) عقدة حلقة واحدة في تعبير الفهم
 * 
 * تمثل: لكل متغير في متكرر
 * أو:   لكل (أ، ب) في أزواج (نمط تفكيك)
 */
struct ComprehensionLoopNode : public ASTNode {
    // ════════════════════════════════════════════════════════════════════════
    // (AR) الحقول
    // ════════════════════════════════════════════════════════════════════════
    
    /**
     * (AR) اسم المتغير البسيط
     * 
     * مثال: س في "لكل س في قائمة"
     */
    std::string variable;
    
    /**
     * (AR) نمط التفكيك (بديل عن المتغير البسيط)
     * 
     * مثال: (مفتاح، قيمة) في "لكل (مفتاح، قيمة) في قاموس"
     */
    std::unique_ptr<PatternNode> pattern;
    
    /**
     * (AR) التعبير القابل للتكرار
     * 
     * مثال: مدى(10) أو قائمة أو قاموس.عناصر()
     */
    std::unique_ptr<ExpressionNode> iterable;
    
    /**
     * (AR) هل هذه حلقة غير متزامنة؟
     * 
     * مثال: غير_متزامن لكل عنصر في تدفق
     */
    bool is_async = false;
    
    /**
     * (AR) موقع المصدر
     */
    SourceLocation location;
    
    // ════════════════════════════════════════════════════════════════════════
    // (AR) الدوال
    // ════════════════════════════════════════════════════════════════════════
    
    NodeKind getKind() const override {
        return NodeKind::ComprehensionLoop;
    }
    
    std::string toString() const override {
        std::string result = is_async ? "AsyncForLoop(" : "ForLoop(";
        if (!variable.empty()) {
            result += "var=" + variable;
        } else if (pattern) {
            result += "pattern=" + pattern->toString();
        }
        result += ")";
        return result;
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto copy = std::make_unique<ComprehensionLoopNode>();
        copy->variable = variable;
        if (pattern) copy->pattern = std::unique_ptr<PatternNode>(
            static_cast<PatternNode*>(pattern->clone().release()));
        if (iterable) copy->iterable = std::unique_ptr<ExpressionNode>(
            static_cast<ExpressionNode*>(iterable->clone().release()));
        copy->is_async = is_async;
        copy->location = location;
        return copy;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقدة شرط التصفية
//                    (EN) Comprehension Filter Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) عقدة شرط التصفية في تعبير الفهم
 * 
 * تمثل: إذا شرط
 */
struct ComprehensionFilterNode : public ASTNode {
    /**
     * (AR) تعبير الشرط
     */
    std::unique_ptr<ExpressionNode> condition;
    
    /**
     * (AR) موقع المصدر
     */
    SourceLocation location;
    
    NodeKind getKind() const override {
        return NodeKind::ComprehensionFilter;
    }
    
    std::string toString() const override {
        return "Filter(" + (condition ? condition->toString() : "null") + ")";
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto copy = std::make_unique<ComprehensionFilterNode>();
        if (condition) copy->condition = std::unique_ptr<ExpressionNode>(
            static_cast<ExpressionNode*>(condition->clone().release()));
        copy->location = location;
        return copy;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقدة فهم القائمة
//                    (EN) List Comprehension Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) عقدة تعبير فهم القائمة
 * 
 * الصيغة: [تعبير لكل متغير في متكرر [إذا شرط]...]
 * 
 * مثال: [س² لكل س في مدى(10) إذا س % 2 == 0]
 */
struct ListComprehensionNode : public ExpressionNode {
    /**
     * (AR) التعبير الذي يُنتج العنصر
     */
    std::unique_ptr<ExpressionNode> element;
    
    /**
     * (AR) قائمة الحلقات (قد تكون متداخلة)
     */
    std::vector<std::unique_ptr<ComprehensionLoopNode>> loops;
    
    /**
     * (AR) قائمة شروط التصفية
     */
    std::vector<std::unique_ptr<ComprehensionFilterNode>> filters;
    
    /**
     * (AR) هل يحتوي على await؟
     */
    bool has_await = false;
    
    /**
     * (AR) موقع المصدر
     */
    SourceLocation location;
    
    NodeKind getKind() const override {
        return NodeKind::ListComprehension;
    }
    
    std::string toString() const override {
        return "ListComp(loops=" + std::to_string(loops.size()) +
               ", filters=" + std::to_string(filters.size()) + ")";
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto copy = std::make_unique<ListComprehensionNode>();
        if (element) copy->element = std::unique_ptr<ExpressionNode>(
            static_cast<ExpressionNode*>(element->clone().release()));
        for (const auto& loop : loops) {
            copy->loops.push_back(std::unique_ptr<ComprehensionLoopNode>(
                static_cast<ComprehensionLoopNode*>(loop->clone().release())));
        }
        for (const auto& filter : filters) {
            copy->filters.push_back(std::unique_ptr<ComprehensionFilterNode>(
                static_cast<ComprehensionFilterNode*>(filter->clone().release())));
        }
        copy->has_await = has_await;
        copy->location = location;
        return copy;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقدة فهم القاموس
//                    (EN) Dict Comprehension Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) عقدة تعبير فهم القاموس
 * 
 * الصيغة: {مفتاح: قيمة لكل متغير في متكرر [إذا شرط]...}
 * 
 * مثال: {س: س² لكل س في مدى(5)}
 */
struct DictComprehensionNode : public ExpressionNode {
    /**
     * (AR) تعبير المفتاح
     */
    std::unique_ptr<ExpressionNode> key;
    
    /**
     * (AR) تعبير القيمة
     */
    std::unique_ptr<ExpressionNode> value;
    
    /**
     * (AR) قائمة الحلقات
     */
    std::vector<std::unique_ptr<ComprehensionLoopNode>> loops;
    
    /**
     * (AR) قائمة شروط التصفية
     */
    std::vector<std::unique_ptr<ComprehensionFilterNode>> filters;
    
    /**
     * (AR) هل المفتاح محسوب؟ [تعبير]: قيمة
     */
    bool computed_key = false;
    
    /**
     * (AR) هل يحتوي على await؟
     */
    bool has_await = false;
    
    /**
     * (AR) موقع المصدر
     */
    SourceLocation location;
    
    NodeKind getKind() const override {
        return NodeKind::DictComprehension;
    }
    
    std::string toString() const override {
        return "DictComp(loops=" + std::to_string(loops.size()) +
               ", computed=" + (computed_key ? "yes" : "no") + ")";
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto copy = std::make_unique<DictComprehensionNode>();
        if (key) copy->key = std::unique_ptr<ExpressionNode>(
            static_cast<ExpressionNode*>(key->clone().release()));
        if (value) copy->value = std::unique_ptr<ExpressionNode>(
            static_cast<ExpressionNode*>(value->clone().release()));
        for (const auto& loop : loops) {
            copy->loops.push_back(std::unique_ptr<ComprehensionLoopNode>(
                static_cast<ComprehensionLoopNode*>(loop->clone().release())));
        }
        for (const auto& filter : filters) {
            copy->filters.push_back(std::unique_ptr<ComprehensionFilterNode>(
                static_cast<ComprehensionFilterNode*>(filter->clone().release())));
        }
        copy->computed_key = computed_key;
        copy->has_await = has_await;
        copy->location = location;
        return copy;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقدة فهم المجموعة
//                    (EN) Set Comprehension Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) عقدة تعبير فهم المجموعة
 * 
 * الصيغة: {|تعبير لكل متغير في متكرر [إذا شرط]...|}
 * 
 * مثال: {|س² لكل س في قائمة|}
 */
struct SetComprehensionNode : public ExpressionNode {
    /**
     * (AR) التعبير الذي يُنتج العنصر
     */
    std::unique_ptr<ExpressionNode> element;
    
    /**
     * (AR) قائمة الحلقات
     */
    std::vector<std::unique_ptr<ComprehensionLoopNode>> loops;
    
    /**
     * (AR) قائمة شروط التصفية
     */
    std::vector<std::unique_ptr<ComprehensionFilterNode>> filters;
    
    /**
     * (AR) هل يحتوي على await؟
     */
    bool has_await = false;
    
    /**
     * (AR) موقع المصدر
     */
    SourceLocation location;
    
    NodeKind getKind() const override {
        return NodeKind::SetComprehension;
    }
    
    std::string toString() const override {
        return "SetComp(loops=" + std::to_string(loops.size()) + ")";
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto copy = std::make_unique<SetComprehensionNode>();
        if (element) copy->element = std::unique_ptr<ExpressionNode>(
            static_cast<ExpressionNode*>(element->clone().release()));
        for (const auto& loop : loops) {
            copy->loops.push_back(std::unique_ptr<ComprehensionLoopNode>(
                static_cast<ComprehensionLoopNode*>(loop->clone().release())));
        }
        for (const auto& filter : filters) {
            copy->filters.push_back(std::unique_ptr<ComprehensionFilterNode>(
                static_cast<ComprehensionFilterNode*>(filter->clone().release())));
        }
        copy->has_await = has_await;
        copy->location = location;
        return copy;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقدة تعبير المولد
//                    (EN) Generator Expression Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) عقدة تعبير المولد (كسول)
 * 
 * الصيغة: (تعبير لكل متغير في متكرر [إذا شرط]...)
 * 
 * مثال: (س² لكل س في مدى(1000000))
 * 
 * الفرق عن فهم القائمة: لا ينشئ قائمة في الذاكرة
 * بل ينتج العناصر عند الطلب (كسول/lazy)
 */
struct GeneratorExpressionNode : public ExpressionNode {
    /**
     * (AR) التعبير الذي يُنتج العنصر
     */
    std::unique_ptr<ExpressionNode> element;
    
    /**
     * (AR) قائمة الحلقات
     */
    std::vector<std::unique_ptr<ComprehensionLoopNode>> loops;
    
    /**
     * (AR) قائمة شروط التصفية
     */
    std::vector<std::unique_ptr<ComprehensionFilterNode>> filters;
    
    /**
     * (AR) هل يحتوي على await؟ (يصبح تدفق)
     */
    bool has_await = false;
    
    /**
     * (AR) موقع المصدر
     */
    SourceLocation location;
    
    NodeKind getKind() const override {
        return NodeKind::GeneratorExpression;
    }
    
    std::string toString() const override {
        return "GenExpr(loops=" + std::to_string(loops.size()) +
               ", async=" + (has_await ? "yes" : "no") + ")";
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto copy = std::make_unique<GeneratorExpressionNode>();
        if (element) copy->element = std::unique_ptr<ExpressionNode>(
            static_cast<ExpressionNode*>(element->clone().release()));
        for (const auto& loop : loops) {
            copy->loops.push_back(std::unique_ptr<ComprehensionLoopNode>(
                static_cast<ComprehensionLoopNode*>(loop->clone().release())));
        }
        for (const auto& filter : filters) {
            copy->filters.push_back(std::unique_ptr<ComprehensionFilterNode>(
                static_cast<ComprehensionFilterNode*>(filter->clone().release())));
        }
        copy->has_await = has_await;
        copy->location = location;
        return copy;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقدة عامل الأنبوب
//                    (EN) Pipeline Operator Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) عقدة عامل الأنبوب (|>)
 * 
 * الصيغة: تعبير |> دالة
 * 
 * مثال: قائمة |> تصفية(زوجي) |> تحويل(مربع) |> مجموع()
 * 
 * يكافئ: مجموع(تحويل(تصفية(قائمة، زوجي)، مربع))
 */
struct PipelineNode : public ExpressionNode {
    /**
     * (AR) التعبير المصدر (الجانب الأيسر)
     */
    std::unique_ptr<ExpressionNode> source;
    
    /**
     * (AR) سلسلة العمليات في الأنبوب
     */
    std::vector<std::unique_ptr<ExpressionNode>> operations;
    
    /**
     * (AR) هل الأنبوب غير متزامن؟
     */
    bool is_async = false;
    
    /**
     * (AR) موقع المصدر
     */
    SourceLocation location;
    
    NodeKind getKind() const override {
        return NodeKind::Pipeline;
    }
    
    std::string toString() const override {
        return "Pipeline(ops=" + std::to_string(operations.size()) + ")";
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto copy = std::make_unique<PipelineNode>();
        if (source) copy->source = std::unique_ptr<ExpressionNode>(
            static_cast<ExpressionNode*>(source->clone().release()));
        for (const auto& op : operations) {
            copy->operations.push_back(std::unique_ptr<ExpressionNode>(
                static_cast<ExpressionNode*>(op->clone().release())));
        }
        copy->is_async = is_async;
        copy->location = location;
        return copy;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    (AR) عقدة التدفق (Stream)
//                    (EN) Stream Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * (AR) عقدة تدفق البيانات
 * 
 * تمثل تدفق بيانات غير متزامن يمكن معالجته عبر الأنبوب
 */
struct StreamNode : public ExpressionNode {
    /**
     * (AR) مصدر التدفق
     */
    std::unique_ptr<ExpressionNode> source;
    
    /**
     * (AR) عمليات التحويل على التدفق
     */
    std::vector<std::unique_ptr<ExpressionNode>> transforms;
    
    /**
     * (AR) موقع المصدر
     */
    SourceLocation location;
    
    NodeKind getKind() const override {
        return NodeKind::Stream;
    }
    
    std::string toString() const override {
        return "Stream(transforms=" + std::to_string(transforms.size()) + ")";
    }
    
    std::unique_ptr<ASTNode> clone() const override {
        auto copy = std::make_unique<StreamNode>();
        if (source) copy->source = std::unique_ptr<ExpressionNode>(
            static_cast<ExpressionNode*>(source->clone().release()));
        for (const auto& t : transforms) {
            copy->transforms.push_back(std::unique_ptr<ExpressionNode>(
                static_cast<ExpressionNode*>(t->clone().release())));
        }
        copy->location = location;
        return copy;
    }
};

} // namespace sad::ast

#endif // SAD_AST_COMPREHENSION_HPP
