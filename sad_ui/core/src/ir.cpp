/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: ir.cpp
 * المسار: sad_ui/core/src/ir.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ عقد التمثيل الوسيط (IRNode) ووظائفها.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/ir.h"
#include "sad_ui/types.h"

#include <sstream>

namespace sad
{
    namespace ui
    {

        // ═══════════════════════════════════════════════════════════════════════════════
        // تنفيذ IRNode
        // ═══════════════════════════════════════════════════════════════════════════════

        IRNode::IRNode(UINodeType type)
            : type_(type)
        {
        }

        std::shared_ptr<IRNode> IRNode::create(UINodeType type)
        {
            return std::make_shared<IRNode>(type);
        }

        const IRProperty *IRNode::findProperty(const std::string &key) const
        {
            for (const auto &prop : properties_)
            {
                if (prop.key == key)
                {
                    return &prop;
                }
            }
            return nullptr;
        }

        std::string IRNode::dump(int indent) const
        {
            std::ostringstream ss;
            std::string prefix(indent * 2, ' ');

            // نوع العقدة
            ss << prefix << "IRNode(" << nodeTypeToArabicName(type_) << ")";

            // معرّف
            if (!id_.empty())
            {
                ss << " #" << id_;
            }

            // تلميح العرض
            switch (hint_)
            {
            case RenderingHint::Native:
                ss << " [أصلي]";
                break;
            case RenderingHint::Canvas:
                ss << " [رسم]";
                break;
            case RenderingHint::Hybrid:
                ss << " [هجين]";
                break;
            default:
                break;
            }

            ss << "\n";

            // الخصائص
            for (const auto &prop : properties_)
            {
                ss << prefix << "  " << prop.key << " = ";
                // طباعة القيمة حسب نوعها
                std::visit([&ss](auto &&val)
                           {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                ss << val;
            } else if constexpr (std::is_same_v<T, double>) {
                ss << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                ss << (val ? "صحيح" : "خطأ");
            } else if constexpr (std::is_same_v<T, std::string>) {
                ss << "\"" << val << "\"";
            } }, prop.value);
                ss << "\n";
            }

            // الأحداث
            for (const auto &evt : events_)
            {
                ss << prefix << "  @" << evt.getEventName() << " → " << evt.expression << "\n";
            }

            // إشارات الحالة
            for (const auto &ref : stateRefs_)
            {
                ss << prefix << "  $" << ref.stateName << " → " << ref.propertyBinding << "\n";
            }

            // الأبناء
            for (const auto &child : children_)
            {
                ss << child->dump(indent + 1);
            }

            return ss.str();
        }

    } // namespace ui
} // namespace sad
