/// تنفيذ معلومات التحويم

#include "lsp_hover.h"
#include "lsp_sync.h"
#include <unordered_map>
#include <unordered_set>
#include <sstream>

namespace sad {
namespace lsp {

LspHover::LspHover(LspServer& server) : server_(server) {}

void LspHover::registerHandlers() {
    server_.registerHandler("textDocument/hover",
        [this](const std::string& p) { return handleHover(p); });
}

/// (AR) مساعد: استخراج قيمة عددية من JSON بعد مفتاح
/// (EN) Helper: extract integer value from JSON after a key
static int extractInt(const std::string& json, const std::string& key) {
    size_t pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos) return -1;
    size_t colon = json.find(':', pos);
    if (colon == std::string::npos) return -1;
    return std::atoi(json.c_str() + colon + 1);
}

/// (AR) مساعد: استخراج نص من JSON بعد مفتاح
/// (EN) Helper: extract string value from JSON after a key
static std::string extractString(const std::string& json, const std::string& key) {
    size_t pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos) return "";
    size_t colon = json.find(':', pos);
    if (colon == std::string::npos) return "";
    size_t q1 = json.find('"', colon + 1);
    size_t q2 = json.find('"', q1 + 1);
    if (q1 == std::string::npos || q2 == std::string::npos) return "";
    return json.substr(q1 + 1, q2 - q1 - 1);
}

/// (AR) مساعد: هل الحرف جزء من معرّف عربي/لاتيني
/// (EN) Helper: is character part of Arabic/Latin identifier
static bool isIdentChar(unsigned char c) {
    // ASCII letters, digits, underscore
    if (std::isalnum(c) || c == '_') return true;
    // UTF-8 multi-byte (Arabic and other scripts)
    if (c >= 0x80) return true;
    return false;
}

/// (AR) استخراج الكلمة عند موقع محدد في سطر
/// (EN) Extract word at given position in a line
static std::string getWordAt(const std::string& line, int character) {
    if (character < 0 || character >= static_cast<int>(line.size())) return "";
    
    // (AR) البحث عن بداية الكلمة
    // (EN) Find word start
    int start = character;
    while (start > 0 && isIdentChar(static_cast<unsigned char>(line[start - 1]))) {
        start--;
    }
    
    // (AR) البحث عن نهاية الكلمة
    // (EN) Find word end
    int end = character;
    while (end < static_cast<int>(line.size()) && isIdentChar(static_cast<unsigned char>(line[end]))) {
        end++;
    }
    
    if (start == end) return "";
    return line.substr(start, end - start);
}

std::string LspHover::handleHover(const std::string& params) {
    // (AR) استخراج URI والموقع من params
    // (EN) Extract URI and position from params
    std::string uri = extractString(params, "uri");
    
    // (AR) استخراج السطر والعمود من "position"
    // (EN) Extract line and character from "position"
    size_t pos_section = params.find("\"position\"");
    int line = -1, character = -1;
    if (pos_section != std::string::npos) {
        line = extractInt(params.substr(pos_section), "line");
        character = extractInt(params.substr(pos_section), "character");
    }
    
    if (uri.empty() || line < 0 || character < 0) return "null";
    
    // (AR) الحصول على المستند المفتوح
    // (EN) Get the open document
    auto* sync = server_.getSync();
    if (!sync) return "null";
    
    auto* doc = sync->getDocument(uri);
    if (!doc || line >= doc->lineCount()) return "null";
    
    // (AR) استخراج الكلمة عند الموقع
    // (EN) Extract word at position
    const std::string& lineText = doc->getLine(line);
    std::string word = getWordAt(lineText, character);
    
    if (word.empty()) return "null";
    
    // (AR) التحقق أولاً من الكلمات المفتاحية
    // (EN) Check keywords first
    std::string result = buildKeywordHover(word);
    if (result != "null") return result;
    
    // (AR) التحقق من الأنواع المدمجة
    // (EN) Check built-in types
    result = buildBuiltinTypeHover(word);
    if (result != "null") return result;
    
    // (AR) التحقق من كلمات مفتاحية إضافية
    // (EN) Check additional keywords
    static const std::unordered_map<std::string, std::string> extra_keywords = {
        {"متغير", "**متغير** — تعريف متغير قابل للتغيير"},
        {"ثابت", "**ثابت** — تعريف قيمة ثابتة لا تتغير"},
        {"ارجع", "**ارجع** — إرجاع قيمة من دالة\\n\\n```sad\\nارجع القيمة\\n```"},
        {"اطبع", "**اطبع** — طباعة نص بدون سطر جديد"},
        {"اطبع_سطر", "**اطبع_سطر** — طباعة نص مع سطر جديد"},
        {"بينما", "**بينما** — حلقة تكرار مشروطة\\n\\n```sad\\nبينما (شرط)\\n   // كود\\nنهاية\\n```"},
        {"نهاية", "**نهاية** — نهاية كتلة كود (دالة، شرط، حلقة، صنف)"},
        {"استورد", "**استورد** — استيراد وحدة خارجية"},
        {"تعداد", "**تعداد** — تعريف نوع تعداد\\n\\n```sad\\nتعداد الأيام\\n   السبت\\n   الأحد\\nنهاية\\n```"},
        {"بنية", "**بنية** — تعريف بنية بيانات\\n\\n```sad\\nبنية نقطة\\n   س\\n   ع\\nنهاية\\n```"},
        {"غير_متزامن", "**غير_متزامن** — تعريف دالة غير متزامنة (async)"},
        {"انتظر", "**انتظر** — انتظار نتيجة عملية غير متزامنة (await)"},
        {"جديد", "**جديد** — إنشاء كائن جديد من صنف"},
        {"هذا", "**هذا** — مرجع للكائن الحالي (this)"},
        {"باني", "**باني** — تعريف دالة البناء في صنف"},
        {"في", "**في** — عامل عضوية أو جزء من حلقة لكل"},
        {"اخرج", "**اخرج** — الخروج من حلقة (break)"},
        {"تابع", "**تابع** — المتابعة في الحلقة التالية (continue)"},
        {"حالة", "**حالة** — تعريف حالة في مطابقة الأنماط"},
        {"افتراضي", "**افتراضي** — الحالة الافتراضية في مطابقة الأنماط"},
        {"صحيح", "**صحيح** — قيمة منطقية: true"},
        {"خطأ", "**خطأ** — قيمة منطقية: false"},
        {"عدم", "**عدم** — قيمة فارغة (null/nil)"},
    };
    
    auto extra_it = extra_keywords.find(word);
    if (extra_it != extra_keywords.end()) {
        std::ostringstream oss;
        oss << "{\"contents\":{\"kind\":\"markdown\",\"value\":\""
            << extra_it->second << "\"}}";
        return oss.str();
    }
    
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
