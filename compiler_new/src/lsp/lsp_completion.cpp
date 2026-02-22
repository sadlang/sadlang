/// تنفيذ الإكمال التلقائي

#include "lsp_completion.h"
#include <sstream>
#include <algorithm>

namespace sad {
namespace lsp {

/// تحويل عنصر إكمال إلى JSON
std::string CompletionItem::toJson() const {
    std::ostringstream json;
    json << "{\"label\":\"" << label << "\"";
    json << ",\"kind\":" << static_cast<int>(kind);
    
    if (!detail.empty()) {
        json << ",\"detail\":\"" << detail << "\"";
    }
    if (!documentation.empty()) {
        json << ",\"documentation\":{\"kind\":\"markdown\",\"value\":\"" 
             << documentation << "\"}";
    }
    if (!insert_text.empty()) {
        json << ",\"insertText\":\"" << insert_text << "\"";
        if (is_snippet) {
            json << ",\"insertTextFormat\":2";
        }
    }
    if (!sort_text.empty()) {
        json << ",\"sortText\":\"" << sort_text << "\"";
    }
    if (!filter_text.empty()) {
        json << ",\"filterText\":\"" << filter_text << "\"";
    }
    if (deprecated) {
        json << ",\"deprecated\":true,\"tags\":[1]";
    }
    
    json << "}";
    return json.str();
}

LspCompletion::LspCompletion(LspServer& server) : server_(server) {
    initKeywords();
    initSnippets();
}

/// تسجيل المعالجات
void LspCompletion::registerHandlers() {
    server_.registerHandler("textDocument/completion",
        [this](const std::string& params) { return handleCompletion(params); });
    
    server_.registerHandler("completionItem/resolve",
        [this](const std::string& params) { return handleCompletionResolve(params); });
}

/// معالجة طلب الإكمال
std::string LspCompletion::handleCompletion(const std::string& params) {
    // TODO: تحليل الموقع والسياق من params
    
    std::ostringstream result;
    result << "{\"isIncomplete\":false,\"items\":[";
    
    bool first = true;
    
    // إضافة الكلمات المفتاحية
    for (const auto& item : keyword_items_) {
        if (!first) result << ",";
        result << item.toJson();
        first = false;
    }
    
    // إضافة القصاصات
    for (const auto& item : snippet_items_) {
        if (!first) result << ",";
        result << item.toJson();
        first = false;
    }
    
    result << "]}";
    return result.str();
}

/// معالجة طلب حل عنصر إكمال
std::string LspCompletion::handleCompletionResolve(const std::string& params) {
    // إرجاع العنصر كما هو مع وثائق إضافية
    return params;
}

/// تهيئة الكلمات المفتاحية العربية
void LspCompletion::initKeywords() {
    auto kw = [](const std::string& label, const std::string& detail) {
        CompletionItem item;
        item.label = label;
        item.kind = CompletionItemKind::Keyword;
        item.detail = detail;
        item.sort_text = "0_" + label;
        return item;
    };
    
    keyword_items_ = {
        kw("دالة", "تعريف دالة"),
        kw("إذا", "شرط"),
        kw("وإلا", "فرع بديل"),
        kw("طالما", "حلقة تكرار"),
        kw("لكل", "حلقة تكرار على مجموعة"),
        kw("أعد", "إرجاع قيمة"),
        kw("متغير", "تعريف متغير"),
        kw("ثابت", "تعريف ثابت"),
        kw("صنف", "تعريف صنف"),
        kw("واجهة", "تعريف واجهة"),
        kw("بنية", "تعريف بنية"),
        kw("تعداد", "تعريف تعداد"),
        kw("استورد", "استيراد وحدة"),
        kw("من", "استيراد جزئي"),
        kw("صحيح", "قيمة منطقية"),
        kw("خطأ", "قيمة منطقية"),
        kw("فارغ", "قيمة فارغة"),
        kw("عام", "محدد وصول عام"),
        kw("خاص", "محدد وصول خاص"),
        kw("محمي", "محدد وصول محمي"),
        kw("ساكن", "عضو ثابت"),
        kw("حاول", "معالجة استثناء"),
        kw("التقط", "التقاط استثناء"),
        kw("أخيراً", "تنفيذ دائم"),
        kw("ارمِ", "رمي استثناء"),
        kw("نوع", "تعريف اسم بديل لنوع"),
        kw("مزامن", "دالة غير متزامنة"),
        kw("انتظر", "انتظار نتيجة غير متزامنة"),
        kw("اكسر", "الخروج من حلقة"),
        kw("تابع", "المتابعة في حلقة"),
        kw("تطابق", "مطابقة أنماط"),
        kw("حالة", "حالة في تطابق"),
        kw("ذاتي", "المرجع الذاتي"),
    };
}

/// تهيئة القصاصات
void LspCompletion::initSnippets() {
    auto snip = [](const std::string& label, const std::string& detail,
                   const std::string& body) {
        CompletionItem item;
        item.label = label;
        item.kind = CompletionItemKind::Snippet;
        item.detail = detail;
        item.insert_text = body;
        item.is_snippet = true;
        item.sort_text = "1_" + label;
        return item;
    };
    
    snippet_items_ = {
        snip("دالة_جديدة", "قالب دالة",
             "دالة ${1:اسم_الدالة}(${2:المعاملات}) -> ${3:نوع_الإرجاع} {\n\t${0}\n}"),
        snip("إذا_وإلا", "قالب شرط",
             "إذا ${1:الشرط} {\n\t${2}\n} وإلا {\n\t${0}\n}"),
        snip("لكل_في", "قالب حلقة",
             "لكل ${1:عنصر} في ${2:المجموعة} {\n\t${0}\n}"),
        snip("طالما_شرط", "قالب حلقة",
             "طالما ${1:الشرط} {\n\t${0}\n}"),
        snip("صنف_جديد", "قالب صنف",
             "صنف ${1:اسم_الصنف} {\n\tعام:\n\t\tدالة ${1}() {\n\t\t\t${0}\n\t\t}\n}"),
        snip("حاول_التقط", "قالب معالجة أخطاء",
             "حاول {\n\t${1}\n} التقط ${2:خطأ} {\n\t${0}\n}"),
        snip("تطابق_أنماط", "قالب تطابق",
             "تطابق ${1:القيمة} {\n\tحالة ${2:نمط1} => ${3}\n\tحالة _ => ${0}\n}"),
        snip("بنية_جديدة", "قالب بنية",
             "بنية ${1:اسم_البنية} {\n\t${2:حقل}: ${3:نوع}\n}"),
        snip("استيراد", "قالب استيراد",
             "استورد \"${1:المسار}\""),
        snip("اختبار", "قالب اختبار",
             "اختبار \"${1:وصف_الاختبار}\" {\n\t// التحضير\n\t${2}\n\t// التأكد\n\tأكد(${0})\n}"),
    };
}

} // namespace lsp
} // namespace sad
