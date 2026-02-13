/// تنفيذ معلومات التحويم

#include "lsp_hover.h"
#include <unordered_map>
#include <sstream>

namespace sad {
namespace lsp {

LspHover::LspHover(LspServer& server) : server_(server) {}

void LspHover::registerHandlers() {
    server_.registerHandler("textDocument/hover",
        [this](const std::string& p) { return handleHover(p); });
}

std::string LspHover::handleHover(const std::string& params) {
    // TODO: تحليل الموقع، إيجاد الرمز، إرجاع معلومات التحويم
    return "null";
}

/// معلومات التحويم للكلمات المفتاحية العربية
std::string LspHover::buildKeywordHover(const std::string& keyword) const {
    static const std::unordered_map<std::string, std::string> keyword_docs = {
        {"دالة", "# دالة\\n\\nتعريف دالة جديدة.\\n\\n```sad\\nدالة اسم(معامل: نوع) -> نوع_الإرجاع {\\n    // الجسم\\n}\\n```"},
        {"إذا", "# إذا\\n\\nتنفيذ مشروط.\\n\\n```sad\\nإذا الشرط {\\n    // ينفذ إذا كان الشرط صحيحاً\\n}\\n```"},
        {"طالما", "# طالما\\n\\nحلقة تكرار مشروطة.\\n\\n```sad\\nطالما الشرط {\\n    // يتكرر طالما الشرط صحيح\\n}\\n```"},
        {"لكل", "# لكل\\n\\nحلقة تكرار على مجموعة.\\n\\n```sad\\nلكل عنصر في المجموعة {\\n    // ينفذ لكل عنصر\\n}\\n```"},
        {"صنف", "# صنف\\n\\nتعريف صنف جديد.\\n\\n```sad\\nصنف اسم {\\n    عام:\\n    خاص:\\n}\\n```"},
        {"أعد", "# أعد\\n\\nإرجاع قيمة من دالة.\\n\\n```sad\\nأعد القيمة\\n```"},
        {"تطابق", "# تطابق\\n\\nمطابقة أنماط.\\n\\n```sad\\nتطابق القيمة {\\n    حالة نمط => نتيجة\\n    حالة _ => افتراضي\\n}\\n```"},
        {"حاول", "# حاول\\n\\nمعالجة الاستثناءات.\\n\\n```sad\\nحاول {\\n    // كود قد يفشل\\n} التقط خطأ {\\n    // معالجة الخطأ\\n}\\n```"},
    };
    
    auto it = keyword_docs.find(keyword);
    if (it == keyword_docs.end()) return "null";
    
    std::ostringstream result;
    result << "{\"contents\":{\"kind\":\"markdown\",\"value\":\""
           << it->second << "\"}}";
    return result.str();
}

/// معلومات التحويم للأنواع المدمجة
std::string LspHover::buildBuiltinTypeHover(const std::string& type_name) const {
    static const std::unordered_map<std::string, std::string> type_docs = {
        {"صحيح", "نوع عدد صحيح 64 بت"},
        {"عشري", "نوع عدد عشري مزدوج الدقة (64 بت)"},
        {"نص", "سلسلة نصية UTF-8"},
        {"منطقي", "قيمة منطقية (صحيح/خطأ)"},
        {"قائمة", "قائمة ديناميكية من العناصر"},
        {"خريطة", "خريطة ربط (مفتاح → قيمة)"},
        {"بايت", "مصفوفة بايتات"},
        {"فراغ", "لا شيء (دالة بدون إرجاع)"},
    };
    
    auto it = type_docs.find(type_name);
    if (it == type_docs.end()) return "null";
    
    std::ostringstream result;
    result << "{\"contents\":{\"kind\":\"markdown\",\"value\":\""
           << "**" << type_name << "** — " << it->second << "\"}}";
    return result.str();
}

} // namespace lsp
} // namespace sad
