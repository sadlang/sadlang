/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: layout.cpp
 * المسار: features/graphics/core/src/layout.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ محرك التخطيط (Layout Engine) — خوارزمية Flexbox المبسطة.
 *
 * خوارزمية التخطيط تعمل بمرحلتين:
 * ──────────────────────────────────
 * المرحلة 1 — القياس (Measure): من الأوراق إلى الجذر
 *   - كل عنصر ورقي يُحسب حجمه الطبيعي
 *   - كل حاوي يجمع أحجام أبنائه
 *
 * المرحلة 2 — التوزيع (Arrange): من الجذر إلى الأوراق
 *   - كل حاوي يوزع المساحة المتاحة على أبنائه
 *   - تُطبّق المحاذاة والحشوة
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/layout.h"
#include "sad_ui/ir.h"
#include "sad_ui/prop_keys.h" // (AR) SoT مفتاح الحشو (props::PADDING = «حشوة»)

#include <algorithm>
#include <numeric>

// حساب عدد أحرف UTF-8 الفعلية (ليس البايتات)
static size_t utf8CharCount(const std::string& text) {
    size_t count = 0;
    for (size_t i = 0; i < text.size(); ++count) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (c < 0x80) { i += 1; }
        else if ((c & 0xE0) == 0xC0) { i += 2; }
        else if ((c & 0xF0) == 0xE0) { i += 3; }
        else if ((c & 0xF8) == 0xF0) { i += 4; }
        else { i += 1; }
    }
    return count;
}

namespace sad {
namespace ui {

// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ LayoutConstraints
// ═══════════════════════════════════════════════════════════════════════════════

float LayoutConstraints::clampWidth(float w) const {
    return std::max(minWidth, std::min(w, maxWidth));
}

float LayoutConstraints::clampHeight(float h) const {
    return std::max(minHeight, std::min(h, maxHeight));
}

// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ LayoutEngine
// ═══════════════════════════════════════════════════════════════════════════════

LayoutEngine::LayoutEngine() = default;

std::shared_ptr<LayoutResult> LayoutEngine::layout(
    const std::shared_ptr<IRNode>& root,
    const LayoutConstraints& constraints
) {
    if (!root) return nullptr;

    // مرحلة واحدة مدمجة: قياس + توزيع
    return arrange(*root, constraints, 0.0f, 0.0f);
}

// ─── القياس ──────────────────────────────────

MeasuredSize LayoutEngine::measure(
    const IRNode& node,
    const LayoutConstraints& constraints
) {
    // تحقق من وجود عرض/ارتفاع ثابت
    const auto* widthProp  = node.findProperty("عرض");
    const auto* heightProp = node.findProperty("ارتفاع");

    LayoutConstraints adjusted = constraints;

    if (widthProp) {
        if (auto* v = std::get_if<double>(&widthProp->value)) {
            float w = static_cast<float>(*v);
            adjusted.minWidth = adjusted.maxWidth = w;
        } else if (auto* vi = std::get_if<int64_t>(&widthProp->value)) {
            float w = static_cast<float>(*vi);
            adjusted.minWidth = adjusted.maxWidth = w;
        }
    }
    if (heightProp) {
        if (auto* v = std::get_if<double>(&heightProp->value)) {
            float h = static_cast<float>(*v);
            adjusted.minHeight = adjusted.maxHeight = h;
        } else if (auto* vi = std::get_if<int64_t>(&heightProp->value)) {
            float h = static_cast<float>(*vi);
            adjusted.minHeight = adjusted.maxHeight = h;
        }
    }

    // حسب نوع العنصر
    switch (node.getType()) {
        case UINodeType::Column:
            return measureColumn(node, adjusted);

        case UINodeType::Row:
            return measureRow(node, adjusted);

        case UINodeType::Grid:
            return measureGrid(node, adjusted);

        case UINodeType::Stack:
            return measureStack(node, adjusted);

        case UINodeType::Wrap:
            return measureWrap(node, adjusted);

        case UINodeType::ScrollView:
        case UINodeType::LazyColumn:
        case UINodeType::List:
        case UINodeType::ListView:
            return measureScrollView(node, adjusted);

        default:
            // عنصر ورقي أو حاوي بسيط
            if (node.childCount() == 0) {
                return measureLeaf(node, adjusted);
            }
            // حاوي بأبناء — يتصرف كعمود افتراضياً
            return measureColumn(node, adjusted);
    }
}

MeasuredSize LayoutEngine::measureColumn(
    const IRNode& node,
    const LayoutConstraints& constraints
) {
    // عمود: الأبناء فوق بعض عمودياً
    // العرض = أكبر عرض من الأبناء
    // الارتفاع = مجموع ارتفاعات الأبناء + تباعد

    float totalHeight = 0.0f;
    float maxWidth = 0.0f;

    // التباعد بين الأبناء
    const auto* spacingProp = node.findProperty("تباعد");
    float spacing = 0.0f;
    if (spacingProp) {
        if (auto* v = std::get_if<double>(&spacingProp->value))
            spacing = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&spacingProp->value))
            spacing = static_cast<float>(*vi);
    }

    // قراءة الحشو (padding)
    const auto* paddingProp = node.findProperty(props::PADDING); // SoT: «حشوة»
    float padding = 0.0f;
    if (paddingProp) {
        if (auto* v = std::get_if<double>(&paddingProp->value))
            padding = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&paddingProp->value))
            padding = static_cast<float>(*vi);
    }

    // قراءة الهامش (margin)
    const auto* marginProp = node.findProperty("هامش");
    float margin = 0.0f;
    if (marginProp) {
        if (auto* v = std::get_if<double>(&marginProp->value))
            margin = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&marginProp->value))
            margin = static_cast<float>(*vi);
    }

    for (size_t i = 0; i < node.childCount(); ++i) {
        const auto& child = node.getChildren()[i];
        auto childSize = measure(*child, constraints);
        maxWidth = std::max(maxWidth, childSize.width);
        totalHeight += childSize.height;
        if (i > 0) totalHeight += spacing;
    }

    // إضافة الحشو والهامش
    totalHeight += padding * 2;
    maxWidth += padding * 2;

    return {
        constraints.clampWidth(maxWidth + margin * 2),
        constraints.clampHeight(totalHeight + margin * 2)
    };
}

MeasuredSize LayoutEngine::measureRow(
    const IRNode& node,
    const LayoutConstraints& constraints
) {
    // صف: الأبناء جنب بعض أفقياً
    // العرض = مجموع عروض الأبناء + تباعد
    // الارتفاع = أكبر ارتفاع من الأبناء

    float totalWidth = 0.0f;
    float maxHeight = 0.0f;

    const auto* spacingProp = node.findProperty("تباعد");
    float spacing = 0.0f;
    if (spacingProp) {
        if (auto* v = std::get_if<double>(&spacingProp->value))
            spacing = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&spacingProp->value))
            spacing = static_cast<float>(*vi);
    }

    // قراءة الحشو (padding)
    const auto* paddingProp = node.findProperty(props::PADDING); // SoT: «حشوة»
    float padding = 0.0f;
    if (paddingProp) {
        if (auto* v = std::get_if<double>(&paddingProp->value))
            padding = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&paddingProp->value))
            padding = static_cast<float>(*vi);
    }

    // قراءة الهامش (margin)
    const auto* marginProp = node.findProperty("هامش");
    float margin = 0.0f;
    if (marginProp) {
        if (auto* v = std::get_if<double>(&marginProp->value))
            margin = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&marginProp->value))
            margin = static_cast<float>(*vi);
    }

    for (size_t i = 0; i < node.childCount(); ++i) {
        const auto& child = node.getChildren()[i];
        auto childSize = measure(*child, constraints);
        totalWidth += childSize.width;
        maxHeight = std::max(maxHeight, childSize.height);
        if (i > 0) totalWidth += spacing;
    }

    // إضافة الحشو والهامش
    totalWidth += padding * 2;
    maxHeight += padding * 2;

    return {
        constraints.clampWidth(totalWidth + margin * 2),
        constraints.clampHeight(maxHeight + margin * 2)
    };
}

MeasuredSize LayoutEngine::measureLeaf(
    const IRNode& node,
    const LayoutConstraints& constraints
) {
    // عنصر ورقي (نص، صورة، فاصل...)
    // الأحجام الافتراضية تعتمد على النوع

    float defaultWidth = 0.0f;
    float defaultHeight = 0.0f;

    switch (node.getType()) {
        case UINodeType::Text: {
            // النص: حجم تقريبي بناءً على طول النص
            const auto* textProp = node.findProperty("text");
            if (!textProp) textProp = node.findProperty("محتوى");
            if (!textProp) textProp = node.findProperty("نص");
            if (textProp) {
                if (auto* text = std::get_if<std::string>(&textProp->value)) {
                    // تقدير تقريبي: كل حرف UTF-8 ≈ 10dp عرض
                    // نستخدم عدد الأحرف الفعلي وليس البايتات
                    defaultWidth = static_cast<float>(utf8CharCount(*text)) * 10.0f;
                    defaultHeight = 20.0f;
                    // حجم الخط يؤثر على الارتفاع
                    const auto* sizeProp = node.findProperty("حجم_خط");
                    if (!sizeProp) sizeProp = node.findProperty("حجم_الخط");
                    if (!sizeProp) sizeProp = node.findProperty("حجم");
                    if (sizeProp) {
                        if (auto* v = std::get_if<double>(&sizeProp->value))
                            defaultHeight = static_cast<float>(*v) * 1.5f;
                        else if (auto* vi = std::get_if<int64_t>(&sizeProp->value))
                            defaultHeight = static_cast<float>(*vi) * 1.5f;
                    }
                }
            }
            if (defaultWidth == 0.0f) defaultWidth = 50.0f;
            if (defaultHeight == 0.0f) defaultHeight = 20.0f;
            break;
        }

        case UINodeType::Button:
            defaultWidth = 120.0f;
            defaultHeight = 48.0f;
            break;

        case UINodeType::Image:
            defaultWidth = 100.0f;
            defaultHeight = 100.0f;
            break;

        case UINodeType::Spacer:
            defaultWidth = 0.0f;
            defaultHeight = 8.0f;
            break;

        case UINodeType::Divider:
            defaultWidth = constraints.maxWidth;
            defaultHeight = 1.0f;
            break;

        case UINodeType::ProgressBar:
            defaultWidth = constraints.maxWidth > 0 ? constraints.maxWidth : 200.0f;
            defaultHeight = 24.0f;
            break;

        default:
            defaultWidth = 50.0f;
            defaultHeight = 50.0f;
            break;
    }

    return {
        constraints.clampWidth(defaultWidth),
        constraints.clampHeight(defaultHeight)
    };
}

// ─── قياس الشبكة (Grid) ──────────────────────────────────

MeasuredSize LayoutEngine::measureGrid(
    const IRNode& node,
    const LayoutConstraints& constraints
) {
    // شبكة: الأبناء يوزعون في صفوف وأعمدة
    const auto* colsProp = node.findProperty("أعمدة");
    if (!colsProp) colsProp = node.findProperty("columns");
    int cols = 2; // افتراضي
    if (colsProp) {
        if (auto* v = std::get_if<int64_t>(&colsProp->value)) cols = static_cast<int>(*v);
        else if (auto* v = std::get_if<double>(&colsProp->value)) cols = static_cast<int>(*v);
    }
    if (cols < 1) cols = 1;

    const auto* spacingProp = node.findProperty("تباعد");
    float spacing = 8.0f;
    if (spacingProp) {
        if (auto* v = std::get_if<double>(&spacingProp->value)) spacing = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&spacingProp->value)) spacing = static_cast<float>(*vi);
    }

    const auto* paddingProp = node.findProperty(props::PADDING); // SoT: «حشوة»
    float padding = 0.0f;
    if (paddingProp) {
        if (auto* v = std::get_if<double>(&paddingProp->value)) padding = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&paddingProp->value)) padding = static_cast<float>(*vi);
    }

    float cellW = (constraints.maxWidth - padding * 2 - spacing * (cols - 1)) / cols;
    LayoutConstraints cellConstraints = constraints;
    cellConstraints.maxWidth = std::max(cellW, 0.0f);

    int rows = (static_cast<int>(node.childCount()) + cols - 1) / cols;
    float maxRowH = 0.0f;
    float totalH = padding * 2;

    for (int r = 0; r < rows; ++r) {
        float rowH = 0.0f;
        for (int c = 0; c < cols; ++c) {
            size_t idx = r * cols + c;
            if (idx >= node.childCount()) break;
            auto childSize = measure(*node.getChildren()[idx], cellConstraints);
            rowH = std::max(rowH, childSize.height);
        }
        totalH += rowH;
        if (r > 0) totalH += spacing;
    }

    return {
        constraints.clampWidth(constraints.maxWidth),
        constraints.clampHeight(totalH)
    };
}

// ─── قياس المكدس (Stack) ──────────────────────────────────

MeasuredSize LayoutEngine::measureStack(
    const IRNode& node,
    const LayoutConstraints& constraints
) {
    // مكدس: الأبناء فوق بعض (متراكبين) — الحجم = أكبر ابن
    float maxW = 0.0f, maxH = 0.0f;
    for (const auto& child : node.getChildren()) {
        auto childSize = measure(*child, constraints);
        maxW = std::max(maxW, childSize.width);
        maxH = std::max(maxH, childSize.height);
    }
    return {
        constraints.clampWidth(maxW),
        constraints.clampHeight(maxH)
    };
}

// ─── قياس الالتفاف (Wrap) ──────────────────────────────────

MeasuredSize LayoutEngine::measureWrap(
    const IRNode& node,
    const LayoutConstraints& constraints
) {
    // التفاف: الأبناء يلتفون للسطر التالي عند نفاد المساحة الأفقية
    const auto* spacingProp = node.findProperty("تباعد");
    float spacing = 8.0f;
    if (spacingProp) {
        if (auto* v = std::get_if<double>(&spacingProp->value)) spacing = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&spacingProp->value)) spacing = static_cast<float>(*vi);
    }

    float lineX = 0.0f, lineH = 0.0f, totalH = 0.0f;
    float maxW = constraints.maxWidth;

    for (const auto& child : node.getChildren()) {
        auto childSize = measure(*child, constraints);
        if (lineX + childSize.width > maxW && lineX > 0) {
            // سطر جديد
            totalH += lineH + spacing;
            lineX = 0.0f;
            lineH = 0.0f;
        }
        lineX += childSize.width + spacing;
        lineH = std::max(lineH, childSize.height);
    }
    totalH += lineH; // السطر الأخير

    return {
        constraints.clampWidth(maxW),
        constraints.clampHeight(totalH)
    };
}

// ─── قياس عنصر قابل للتمرير (ScrollView) ──────────────────────────────────

MeasuredSize LayoutEngine::measureScrollView(
    const IRNode& node,
    const LayoutConstraints& constraints
) {
    // ScrollView: الحاوية تحترم القيود، لكن الأبناء يُقاسون بارتفاع غير محدود
    LayoutConstraints unbounded = constraints;
    unbounded.maxHeight = 1e6f;
    unbounded.minHeight = 0.0f;

    float totalHeight = 0.0f;
    float maxWidth = 0.0f;

    const auto* spacingProp = node.findProperty("تباعد");
    float spacing = 0.0f;
    if (spacingProp) {
        if (auto* v = std::get_if<double>(&spacingProp->value))
            spacing = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&spacingProp->value))
            spacing = static_cast<float>(*vi);
    }

    const auto* paddingProp = node.findProperty(props::PADDING); // SoT: «حشوة»
    float padding = 0.0f;
    if (paddingProp) {
        if (auto* v = std::get_if<double>(&paddingProp->value))
            padding = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&paddingProp->value))
            padding = static_cast<float>(*vi);
    }

    for (size_t i = 0; i < node.childCount(); ++i) {
        const auto& child = node.getChildren()[i];
        auto childSize = measure(*child, unbounded);
        maxWidth = std::max(maxWidth, childSize.width);
        totalHeight += childSize.height;
        if (i > 0) totalHeight += spacing;
    }

    totalHeight += padding * 2;
    maxWidth += padding * 2;

    // الحاوية تُقيّد بالقيود الأصلية (حجم الشاشة)
    // لكن ارتفاع المحتوى الحقيقي يُحفظ في LayoutResult.contentHeight
    return {
        constraints.clampWidth(maxWidth),
        constraints.clampHeight(totalHeight)
    };
}

// ─── التوزيع ──────────────────────────────────

std::shared_ptr<LayoutResult> LayoutEngine::arrange(
    const IRNode& node,
    const LayoutConstraints& constraints,
    float offsetX, float offsetY
) {
    auto result = std::make_shared<LayoutResult>();

    // قياس الحجم المطلوب
    auto measured = measure(node, constraints);
    result->rect.x = offsetX;
    result->rect.y = offsetY;
    result->rect.width = measured.width;
    result->rect.height = measured.height;

    // توزيع الأبناء
    if (node.childCount() == 0) return result;

    const auto* spacingProp = node.findProperty("تباعد");
    float spacing = 0.0f;
    if (spacingProp) {
        if (auto* v = std::get_if<double>(&spacingProp->value))
            spacing = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&spacingProp->value))
            spacing = static_cast<float>(*vi);
    }

    // قراءة الحشو (padding) — إزاحة داخلية للأبناء
    const auto* paddingProp = node.findProperty(props::PADDING); // SoT: «حشوة»
    float padding = 0.0f;
    if (paddingProp) {
        if (auto* v = std::get_if<double>(&paddingProp->value))
            padding = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&paddingProp->value))
            padding = static_cast<float>(*vi);
    }

    // قراءة الهامش (margin) — إزاحة خارجية
    const auto* marginProp = node.findProperty("هامش");
    float margin = 0.0f;
    if (marginProp) {
        if (auto* v = std::get_if<double>(&marginProp->value))
            margin = static_cast<float>(*v);
        else if (auto* vi = std::get_if<int64_t>(&marginProp->value))
            margin = static_cast<float>(*vi);
    }

    // تعديل القيود المتاحة للأبناء بعد خصم الحشو
    LayoutConstraints childConstraints = constraints;
    if (padding > 0) {
        childConstraints.maxWidth = std::max(0.0f, childConstraints.maxWidth - padding * 2);
        childConstraints.maxHeight = std::max(0.0f, childConstraints.maxHeight - padding * 2);
    }

    if (node.getType() == UINodeType::ScrollView ||
        node.getType() == UINodeType::LazyColumn ||
        node.getType() == UINodeType::List) {
        // ─── توزيع عنصر قابل للتمرير (ScrollView) ───
        // الأبناء يُوزعون كعمود لكن بارتفاع غير محدود
        LayoutConstraints scrollChildConstraints = childConstraints;
        scrollChildConstraints.maxHeight = 1e6f;
        scrollChildConstraints.minHeight = 0.0f;

        float childY = padding;
        for (size_t i = 0; i < node.childCount(); ++i) {
            const auto& child = node.getChildren()[i];
            auto childResult = arrange(*child, scrollChildConstraints,
                                        offsetX + padding, offsetY + childY);
            childY += childResult->rect.height;
            if (i < node.childCount() - 1) childY += spacing;
            result->children.push_back(childResult);
        }
        childY += padding;
        // حفظ ارتفاع المحتوى الكامل (قد يتجاوز ارتفاع الحاوية)
        result->contentHeight = childY;
    } else if (node.getType() == UINodeType::Grid) {
        // ─── توزيع شبكي (Grid) ───
        const auto* colsProp = node.findProperty("أعمدة");
        if (!colsProp) colsProp = node.findProperty("columns");
        int cols = 2;
        if (colsProp) {
            if (auto* v = std::get_if<int64_t>(&colsProp->value)) cols = static_cast<int>(*v);
            else if (auto* v = std::get_if<double>(&colsProp->value)) cols = static_cast<int>(*v);
        }
        if (cols < 1) cols = 1;

        float cellW = (childConstraints.maxWidth - spacing * (cols - 1)) / cols;
        LayoutConstraints cellConstraints2 = childConstraints;
        cellConstraints2.maxWidth = std::max(cellW, 0.0f);

        int rows = (static_cast<int>(node.childCount()) + cols - 1) / cols;
        float gridY = padding;

        for (int r = 0; r < rows; ++r) {
            float rowH = 0.0f;
            // أولاً: قياس ارتفاع الصف
            for (int c = 0; c < cols; ++c) {
                size_t idx = r * cols + c;
                if (idx >= node.childCount()) break;
                auto childSize = measure(*node.getChildren()[idx], cellConstraints2);
                rowH = std::max(rowH, childSize.height);
            }
            // ثانياً: توزيع الخلايا
            for (int c = 0; c < cols; ++c) {
                size_t idx = r * cols + c;
                if (idx >= node.childCount()) break;
                float cellX = padding + c * (cellW + spacing);
                auto childResult = arrange(*node.getChildren()[idx], cellConstraints2,
                                            offsetX + cellX, offsetY + gridY);
                result->children.push_back(childResult);
            }
            gridY += rowH + spacing;
        }
    } else if (node.getType() == UINodeType::Stack) {
        // ─── توزيع مكدس (Stack) — كل الأبناء في نفس الموقع ───
        for (const auto& child : node.getChildren()) {
            auto childResult = arrange(*child, childConstraints,
                                        offsetX + padding, offsetY + padding);
            result->children.push_back(childResult);
        }
    } else if (node.getType() == UINodeType::Wrap) {
        // ─── توزيع التفاف (Wrap) ───
        float lineX = padding, lineY = padding, lineH = 0.0f;
        float maxWrapW = childConstraints.maxWidth;

        for (const auto& child : node.getChildren()) {
            auto childSize = measure(*child, childConstraints);
            if (lineX + childSize.width > maxWrapW + padding && lineX > padding) {
                lineY += lineH + spacing;
                lineX = padding;
                lineH = 0.0f;
            }
            auto childResult = arrange(*child, childConstraints,
                                        offsetX + lineX, offsetY + lineY);
            lineX += childResult->rect.width + spacing;
            lineH = std::max(lineH, childResult->rect.height);
            result->children.push_back(childResult);
        }
    } else if (node.getType() == UINodeType::Column ||
        (node.getType() != UINodeType::Row && node.childCount() > 0)) {
        // ─── توزيع عمودي (Column) مع دعم الوزن (flex weight) ───
        // المرحلة 1: قياس الأبناء العاديين وحساب الأوزان
        float fixedTotal = 0.0f;
        float totalWeight = 0.0f;
        struct ChildInfo {
            size_t index;
            float height;
            float weight;
        };
        std::vector<ChildInfo> infos;

        for (size_t i = 0; i < node.childCount(); ++i) {
            const auto& child = node.getChildren()[i];
            float weight = 0.0f;
            const auto* weightProp = child->findProperty("وزن");
            if (!weightProp) weightProp = child->findProperty("flex");
            if (weightProp) {
                if (auto* v = std::get_if<double>(&weightProp->value)) weight = static_cast<float>(*v);
                else if (auto* vi = std::get_if<int64_t>(&weightProp->value)) weight = static_cast<float>(*vi);
            }
            auto childSize = measure(*child, childConstraints);
            if (weight > 0) {
                totalWeight += weight;
                infos.push_back({i, 0.0f, weight});
            } else {
                fixedTotal += childSize.height;
                infos.push_back({i, childSize.height, 0.0f});
            }
            if (i > 0) fixedTotal += spacing;
        }

        // المرحلة 2: توزيع المساحة المتبقية حسب الأوزان
        float availableH = std::max(0.0f, childConstraints.maxHeight - fixedTotal - padding * 2);
        for (auto& info : infos) {
            if (info.weight > 0 && totalWeight > 0) {
                info.height = availableH * (info.weight / totalWeight);
            }
        }

        // المرحلة 3: ترتيب الأبناء
        float childY = padding;
        for (const auto& info : infos) {
            const auto& child = node.getChildren()[info.index];
            LayoutConstraints cc = childConstraints;
            if (info.weight > 0) {
                cc.maxHeight = info.height;
                cc.minHeight = info.height;
            }
            auto childResult = arrange(*child, cc, offsetX + padding, offsetY + childY);
            if (info.weight > 0) {
                childResult->rect.height = info.height;
            }
            childY += childResult->rect.height + spacing;
            result->children.push_back(childResult);
        }
    } else if (node.getType() == UINodeType::Row) {
        // ─── توزيع أفقي (Row) مع دعم الوزن (flex weight) ───
        // المرحلة 1: قياس الأبناء العاديين وحساب الأوزان
        float fixedTotal = 0.0f;
        float totalWeight = 0.0f;
        struct RowChildInfo {
            size_t index;
            float width;
            float weight;
        };
        std::vector<RowChildInfo> rowInfos;

        for (size_t i = 0; i < node.childCount(); ++i) {
            const auto& child = node.getChildren()[i];
            float weight = 0.0f;
            const auto* weightProp = child->findProperty("وزن");
            if (!weightProp) weightProp = child->findProperty("flex");
            if (weightProp) {
                if (auto* v = std::get_if<double>(&weightProp->value)) weight = static_cast<float>(*v);
                else if (auto* vi = std::get_if<int64_t>(&weightProp->value)) weight = static_cast<float>(*vi);
            }
            auto childSize = measure(*child, childConstraints);
            if (weight > 0) {
                totalWeight += weight;
                rowInfos.push_back({i, 0.0f, weight});
            } else {
                fixedTotal += childSize.width;
                rowInfos.push_back({i, childSize.width, 0.0f});
            }
            if (i > 0) fixedTotal += spacing;
        }

        float availableW = std::max(0.0f, childConstraints.maxWidth - fixedTotal - padding * 2);
        for (auto& info : rowInfos) {
            if (info.weight > 0 && totalWeight > 0) {
                info.width = availableW * (info.weight / totalWeight);
            }
        }

        if (direction_ == LayoutDirection::RTL) {
            float childX = measured.width - padding;
            for (const auto& info : rowInfos) {
                const auto& child = node.getChildren()[info.index];
                LayoutConstraints cc = childConstraints;
                if (info.weight > 0) { cc.maxWidth = info.width; cc.minWidth = info.width; }
                auto childSz = measure(*child, cc);
                float w = info.weight > 0 ? info.width : childSz.width;
                childX -= w;
                auto childResult = arrange(*child, cc, offsetX + childX, offsetY + padding);
                if (info.weight > 0) childResult->rect.width = info.width;
                childX -= spacing;
                result->children.push_back(childResult);
            }
        } else {
            float childX = padding;
            for (const auto& info : rowInfos) {
                const auto& child = node.getChildren()[info.index];
                LayoutConstraints cc = childConstraints;
                if (info.weight > 0) { cc.maxWidth = info.width; cc.minWidth = info.width; }
                auto childResult = arrange(*child, cc, offsetX + childX, offsetY + padding);
                if (info.weight > 0) childResult->rect.width = info.width;
                childX += childResult->rect.width + spacing;
                result->children.push_back(childResult);
            }
        }
    }

    return result;
}

} // namespace ui
} // namespace sad
