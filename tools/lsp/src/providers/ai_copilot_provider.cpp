// بسم الله الرحمن الرحيم
/**
 * ══════════════════════════════════════════════════════════════════════════════
 * ملف: ai_copilot_provider.cpp
 * الوصف: تنفيذ نظام المساعد الذكي الثوري للغة ص
 * ══════════════════════════════════════════════════════════════════════════════
 */

#include "ai_copilot_provider.h"
#include <regex>
#include <sstream>
#include <algorithm>
#include <cmath>

namespace sad {
namespace lsp {
namespace ai {

// ══════════════════════════════════════════════════════════════════════════════
//  قوالب الأكواد الذكية
// ══════════════════════════════════════════════════════════════════════════════

namespace templates {

// قالب دالة
const char* FUNCTION_TEMPLATE = R"(
## ${توثيق}
دالة ${اسم}(${معلمات})
    ${جسم}
نهاية
)";

// قالب صنف
const char* CLASS_TEMPLATE = R"(
## صنف ${اسم}
## ${توثيق}
صنف ${اسم}
    ## خصائص الصنف
    ${خصائص}
    
    ## المنشئ
    دالة جديد(${معلمات_المنشئ})
        ${تهيئة}
    نهاية
    
    ${دوال}
نهاية
)";

// قالب حلقة
const char* LOOP_TEMPLATE = R"(
لكل ${عنصر} في ${قائمة}
    ${جسم}
نهاية
)";

// قالب شرط
const char* CONDITION_TEMPLATE = R"(
إذا ${شرط}
    ${إذا_صحيح}
وإلا
    ${إذا_خاطئ}
نهاية
)";

// قالب معالجة خطأ
const char* ERROR_HANDLING_TEMPLATE = R"(
حاول
    ${كود}
امسك ${نوع_الخطأ} كـ خطأ
    اطبع("خطأ: " + خطأ.رسالة)
    ${معالجة}
انتهى
)";

// قالب اختبار
const char* TEST_TEMPLATE = R"(
## اختبار: ${وصف}
دالة اختبار_${اسم}()
    ## الترتيب
    متغير ${متغير_الإدخال} = ${قيمة_الإدخال}
    
    ## التنفيذ
    متغير النتيجة = ${استدعاء}
    
    ## التحقق
    أكد(النتيجة == ${المتوقع}, "${رسالة_الفشل}")
نهاية
)";

} // namespace templates

// ══════════════════════════════════════════════════════════════════════════════
//  أنماط الكود الشائعة
// ══════════════════════════════════════════════════════════════════════════════

namespace patterns {

// أنماط الإكمال الذكي
struct CompletionPattern {
    std::string trigger;        // النص المحفز
    std::string pattern_name;   // اسم النمط
    std::string completion;     // الإكمال
    std::string description;    // وصف
};

const std::vector<CompletionPattern> SMART_PATTERNS = {
    // أنماط الدوال
    {"@دالة ", "function_definition", 
     "دالة ${1:اسم_الدالة}(${2:المعلمات})\n    ${3:// الكود}\nنهاية",
     "تعريف دالة جديدة"},
    
    {"@دالة_حساب", "calc_function",
     "## حساب ${1:الوصف}\nدالة ${2:اسم}(${3:أ}: رقم, ${4:ب}: رقم) -> رقم\n    إرجاع ${5:أ + ب}\nنهاية",
     "دالة حسابية"},
    
    // أنماط التحقق
    {"@تحقق ", "validation",
     "إذا ${1:الشرط}\n    ارفع خطأ(\"${2:رسالة الخطأ}\")\nنهاية",
     "التحقق من شرط"},
    
    {"@تحقق_فارغ", "null_check",
     "إذا ${1:المتغير} == عدم\n    إرجاع ${2:القيمة_الافتراضية}\nنهاية",
     "التحقق من القيمة الفارغة"},
    
    // أنماط الحلقات
    {"@حلقة_عداد", "counter_loop",
     "متغير ${1:ع} = 0\nطالما ${1:ع} < ${2:الحد}\n    ${3:// الكود}\n    ${1:ع} = ${1:ع} + 1\nنهاية",
     "حلقة بعداد"},
    
    {"@حلقة_قائمة", "list_loop",
     "لكل ${1:عنصر} في ${2:القائمة}\n    ${3:// معالجة العنصر}\nنهاية",
     "حلقة على قائمة"},
    
    // أنماط المعالجة
    {"@فلترة", "filter_pattern",
     "متغير ${1:النتيجة} = []\nلكل ${2:عنصر} في ${3:القائمة}\n    إذا ${4:الشرط}\n        ${1:النتيجة}.أضف(${2:عنصر})\n    نهاية\nنهاية",
     "فلترة قائمة"},
    
    {"@تحويل", "map_pattern",
     "متغير ${1:النتيجة} = []\nلكل ${2:عنصر} في ${3:القائمة}\n    ${1:النتيجة}.أضف(${4:تحويل(عنصر)})\nنهاية",
     "تحويل عناصر قائمة"},
    
    // أنماط الأصناف
    {"@صنف ", "class_definition",
     "## صنف ${1:الاسم}\n## ${2:الوصف}\nصنف ${1:الاسم}\n    خاصية ${3:اسم_الخاصية}: ${4:النوع}\n    \n    دالة جديد(${5:المعلمات})\n        ${6:// التهيئة}\n    نهاية\nنهاية",
     "تعريف صنف جديد"},
    
    {"@واجهة ", "interface_definition",
     "واجهة ${1:الاسم}\n    دالة ${2:اسم_الدالة}(${3:المعلمات}) -> ${4:النوع}\nنهاية",
     "تعريف واجهة"},
    
    // أنماط الملفات
    {"@قراءة_ملف", "file_read",
     "متغير ${1:محتوى} = \"\"\nحاول\n    ${1:محتوى} = اقرأ_ملف(\"${2:المسار}\")\nامسك خطأ_ملف كـ خ\n    اطبع(\"تعذر قراءة الملف: \" + خ.رسالة)\nانتهى",
     "قراءة ملف مع معالجة الأخطاء"},
    
    {"@كتابة_ملف", "file_write",
     "حاول\n    اكتب_ملف(\"${1:المسار}\", ${2:المحتوى})\nامسك خطأ_ملف كـ خ\n    اطبع(\"تعذر كتابة الملف: \" + خ.رسالة)\nانتهى",
     "كتابة ملف مع معالجة الأخطاء"},
};

} // namespace patterns

// ══════════════════════════════════════════════════════════════════════════════
//  المنشئ والمدمر
// ══════════════════════════════════════════════════════════════════════════════

AICopilotProvider::AICopilotProvider() {
    // تهيئة الأنماط الافتراضية
    for (const auto& p : patterns::SMART_PATTERNS) {
        CodePattern pattern;
        pattern.id = p.pattern_name;
        pattern.name = p.pattern_name;
        pattern.description = p.description;
        pattern.trigger_regex = "^" + std::regex_replace(p.trigger, std::regex("\\$"), "\\$");
        pattern.template_code = p.completion;
        pattern.usage_count = 0;
        pattern.success_rate = 0.8f;
        patterns_.push_back(pattern);
    }
}

AICopilotProvider::~AICopilotProvider() = default;

// ══════════════════════════════════════════════════════════════════════════════
//  بناء السياق
// ══════════════════════════════════════════════════════════════════════════════

AIContext AICopilotProvider::build_context(const std::string& document, const Position& pos) {
    AIContext context;
    
    // تقسيم المستند إلى أسطر
    std::vector<std::string> lines;
    std::istringstream stream(document);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    // السطر الحالي
    if (pos.line >= 0 && pos.line < static_cast<int>(lines.size())) {
        context.current_line = lines[pos.line];
        context.cursor_offset = pos.character;
        
        // النص المحفز (الكلمة الجزئية قبل المؤشر)
        int start = pos.character;
        while (start > 0 && (std::isalnum(context.current_line[start-1]) || 
               context.current_line[start-1] == '_' ||
               static_cast<unsigned char>(context.current_line[start-1]) > 127)) {
            start--;
        }
        context.trigger_text = context.current_line.substr(start, pos.character - start);
        context.is_partial_word = !context.trigger_text.empty();
    }
    
    // الكود السابق (آخر 50 سطر)
    std::ostringstream preceding;
    int start_line = std::max(0, pos.line - 50);
    for (int i = start_line; i < pos.line; i++) {
        preceding << lines[i] << "\n";
    }
    context.preceding_code = preceding.str();
    
    // الكود التالي (أول 10 أسطر)
    std::ostringstream following;
    int end_line = std::min(static_cast<int>(lines.size()), pos.line + 10);
    for (int i = pos.line + 1; i < end_line; i++) {
        following << lines[i] << "\n";
    }
    context.following_code = following.str();
    
    // تحديد نوع السياق
    context.type = ContextType::Global;
    
    // البحث عن السياق الحالي في الكود السابق
    std::regex func_regex(R"(دالة\s+(\w+))");
    std::regex class_regex(R"(صنف\s+(\w+))");
    std::regex loop_regex(R"((لكل|طالما|كرر)\s)");
    std::regex cond_regex(R"(إذا\s)");
    
    std::smatch match;
    std::string search_area = context.preceding_code + context.current_line;
    
    // تتبع المستوى (عمق التداخل)
    int depth = 0;
    for (const auto& l : lines) {
        if (l.find("دالة ") != std::string::npos || 
            l.find("صنف ") != std::string::npos ||
            l.find("إذا ") != std::string::npos ||
            l.find("طالما ") != std::string::npos) {
            depth++;
        }
        if (l.find("نهاية") != std::string::npos) {
            depth = std::max(0, depth - 1);
        }
    }
    
    // البحث عن آخر تعريف دالة
    std::string::const_iterator searchStart(search_area.cbegin());
    while (std::regex_search(searchStart, search_area.cend(), match, func_regex)) {
        context.current_function = match[1].str();
        context.type = ContextType::Function;
        searchStart = match.suffix().first;
    }
    
    // البحث عن آخر تعريف صنف
    searchStart = search_area.cbegin();
    while (std::regex_search(searchStart, search_area.cend(), match, class_regex)) {
        context.current_class = match[1].str();
        context.type = ContextType::Class;
        searchStart = match.suffix().first;
    }
    
    // استخراج المتغيرات المحلية
    std::regex var_regex(R"(متغير\s+(\w+))");
    searchStart = search_area.cbegin();
    while (std::regex_search(searchStart, search_area.cend(), match, var_regex)) {
        context.local_vars.push_back(match[1].str());
        searchStart = match.suffix().first;
    }
    
    // استخراج الثوابت
    std::regex const_regex(R"(ثابت\s+(\w+))");
    searchStart = search_area.cbegin();
    while (std::regex_search(searchStart, search_area.cend(), match, const_regex)) {
        context.local_vars.push_back(match[1].str());
        searchStart = match.suffix().first;
    }
    
    // استخراج الاستيرادات
    std::regex import_regex(R"(استورد\s+(\w+))");
    searchStart = document.cbegin();
    while (std::regex_search(searchStart, document.cend(), match, import_regex)) {
        context.imports.push_back(match[1].str());
        searchStart = match.suffix().first;
    }
    
    return context;
}

// ══════════════════════════════════════════════════════════════════════════════
//  تحليل النية
// ══════════════════════════════════════════════════════════════════════════════

IntentAnalysis AICopilotProvider::analyze_intent(const AIContext& context) {
    IntentAnalysis analysis;
    analysis.primary_intent = Intent::Unknown;
    analysis.confidence = 0.0f;
    
    const std::string& line = context.current_line;
    const std::string& trigger = context.trigger_text;
    
    // تحليل بناءً على الكلمات المفتاحية
    if (line.find("دالة") != std::string::npos || trigger == "دا" || trigger == "دال") {
        analysis.primary_intent = Intent::DefineFunction;
        analysis.confidence = 0.9f;
    }
    else if (line.find("صنف") != std::string::npos || trigger == "صن" || trigger == "صنف") {
        analysis.primary_intent = Intent::DefineClass;
        analysis.confidence = 0.9f;
    }
    else if (line.find("متغير") != std::string::npos || trigger == "متغ" || trigger == "متغير") {
        analysis.primary_intent = Intent::DefineVariable;
        analysis.confidence = 0.9f;
    }
    else if (line.find("إذا") != std::string::npos || trigger == "إذ" || trigger == "إذا") {
        analysis.primary_intent = Intent::WriteCondition;
        analysis.confidence = 0.9f;
    }
    else if (line.find("لكل") != std::string::npos || 
             line.find("طالما") != std::string::npos ||
             line.find("كرر") != std::string::npos) {
        analysis.primary_intent = Intent::WriteLoop;
        analysis.confidence = 0.9f;
    }
    else if (line.find("إرجاع") != std::string::npos || trigger == "إرج" || trigger == "إرجاع") {
        analysis.primary_intent = Intent::ReturnValue;
        analysis.confidence = 0.85f;
    }
    else if (line.find("##") != std::string::npos) {
        analysis.primary_intent = Intent::WriteDocumentation;
        analysis.confidence = 0.8f;
    }
    else if (line.find(".") != std::string::npos) {
        analysis.primary_intent = Intent::AccessMember;
        analysis.confidence = 0.7f;
    }
    else if (line.find("(") != std::string::npos && line.find(")") == std::string::npos) {
        analysis.primary_intent = Intent::CallFunction;
        analysis.confidence = 0.7f;
    }
    
    // نوايا ثانوية محتملة
    if (context.type == ContextType::Class) {
        analysis.secondary.push_back(Intent::CreateConstructor);
        analysis.secondary.push_back(Intent::OverrideMethod);
    }
    if (context.type == ContextType::Function) {
        analysis.secondary.push_back(Intent::AddValidation);
        analysis.secondary.push_back(Intent::HandleError);
    }
    
    return analysis;
}

// ══════════════════════════════════════════════════════════════════════════════
//  الحصول على اقتراحات
// ══════════════════════════════════════════════════════════════════════════════

std::vector<AISuggestion> AICopilotProvider::get_suggestions(
    const std::string& document,
    const Position& position,
    const std::string& trigger_character
) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<AISuggestion> suggestions;
    
    // بناء السياق
    AIContext context = build_context(document, position);
    
    // تحليل النية
    IntentAnalysis intent = analyze_intent(context);
    
    // البحث عن أنماط مطابقة
    for (const auto& pattern : patterns_) {
        // التحقق من تطابق النمط مع السياق
        if (context.trigger_text.find("@") == 0) {
            // نمط خاص يبدأ بـ @
            std::string pattern_trigger = "@" + pattern.name;
            if (pattern_trigger.find(context.trigger_text) == 0) {
                AISuggestion suggestion;
                suggestion.id = pattern.id;
                suggestion.type = SuggestionType::CodeBlock;
                suggestion.title = pattern.name;
                suggestion.description = pattern.description;
                suggestion.code = pattern.template_code;
                suggestion.confidence = 0.9f;
                suggestion.source = "pattern";
                suggestion.priority = 100;
                suggestions.push_back(suggestion);
            }
        }
    }
    
    // اقتراحات بناءً على النية
    if (intent.primary_intent != Intent::Unknown && intent.confidence > 0.5f) {
        AISuggestion intent_suggestion;
        intent_suggestion.id = "intent_" + std::to_string(static_cast<int>(intent.primary_intent));
        
        switch (intent.primary_intent) {
            case Intent::DefineFunction:
                intent_suggestion.type = SuggestionType::FullFunction;
                intent_suggestion.title = "تعريف دالة جديدة";
                intent_suggestion.code = "دالة ${1:اسم_الدالة}(${2:المعلمات})\n    ${3:// الكود}\nنهاية";
                break;
                
            case Intent::DefineClass:
                intent_suggestion.type = SuggestionType::FullFunction;
                intent_suggestion.title = "تعريف صنف جديد";
                intent_suggestion.code = "صنف ${1:اسم_الصنف}\n    ${2:// الخصائص والدوال}\nنهاية";
                break;
                
            case Intent::WriteCondition:
                intent_suggestion.type = SuggestionType::CodeBlock;
                intent_suggestion.title = "كتابة شرط";
                intent_suggestion.code = "إذا ${1:الشرط}\n    ${2:// إذا صحيح}\نهاية";
                break;
                
            case Intent::WriteLoop:
                intent_suggestion.type = SuggestionType::CodeBlock;
                intent_suggestion.title = "كتابة حلقة";
                intent_suggestion.code = "لكل ${1:عنصر} في ${2:القائمة}\n    ${3:// الكود}\نهاية";
                break;
                
            default:
                break;
        }
        
        if (!intent_suggestion.code.empty()) {
            intent_suggestion.confidence = intent.confidence;
            intent_suggestion.source = "intent";
            intent_suggestion.priority = 90;
            suggestions.push_back(intent_suggestion);
        }
    }
    
    // اقتراحات للمتغيرات المحلية
    for (const auto& var : context.local_vars) {
        if (var.find(context.trigger_text) == 0 || context.trigger_text.empty()) {
            AISuggestion var_suggestion;
            var_suggestion.id = "var_" + var;
            var_suggestion.type = SuggestionType::Completion;
            var_suggestion.title = var;
            var_suggestion.description = "متغير محلي";
            var_suggestion.code = var;
            var_suggestion.confidence = 0.8f;
            var_suggestion.source = "scope";
            var_suggestion.priority = 80;
            suggestions.push_back(var_suggestion);
        }
    }
    
    // فلترة وترتيب
    return filter_and_rank(suggestions, context);
}

// ══════════════════════════════════════════════════════════════════════════════
//  الإكمال متعدد الأسطر
// ══════════════════════════════════════════════════════════════════════════════

std::optional<std::string> AICopilotProvider::get_multiline_completion(const AIContext& context) {
    // تحليل السياق لتحديد الإكمال متعدد الأسطر
    
    // إذا كان السطر الحالي يحتوي على تعريف دالة غير مكتمل
    if (context.current_line.find("دالة ") != std::string::npos &&
        context.current_line.find("نهاية") == std::string::npos) {
        
        // استخراج اسم الدالة
        std::regex func_regex(R"(دالة\s+(\w+)\s*\()");
        std::smatch match;
        if (std::regex_search(context.current_line, match, func_regex)) {
            std::string func_name = match[1].str();
            
            // توليد جسم الدالة
            std::ostringstream body;
            body << "\n    ## TODO: تنفيذ " << func_name << "\n";
            body << "    إرجاع عدم\n";
            body << "نهاية";
            
            return body.str();
        }
    }
    
    // إذا كان السطر يحتوي على شرط غير مكتمل
    if (context.current_line.find("إذا ") != std::string::npos &&
        context.following_code.find("نهاية") == std::string::npos) {
        return "\n    ## TODO: الكود هنا\nنهاية";
    }
    
    // إذا كان السطر يحتوي على حلقة غير مكتملة
    if ((context.current_line.find("لكل ") != std::string::npos ||
         context.current_line.find("طالما ") != std::string::npos) &&
        context.following_code.find("نهاية") == std::string::npos) {
        return "\n    ## TODO: الكود هنا\nنهاية";
    }
    
    return std::nullopt;
}

// ══════════════════════════════════════════════════════════════════════════════
//  توليد الدوال
// ══════════════════════════════════════════════════════════════════════════════

std::string AICopilotProvider::generate_function_from_description(
    const std::string& description,
    const AIContext& context
) {
    std::ostringstream code;
    
    // تحليل الوصف لاستخراج المعلومات
    std::string func_name = "دالة_جديدة";
    std::string params = "";
    std::string return_type = "عدم";
    
    // محاولة استخراج الاسم من الوصف
    if (description.find("حساب") != std::string::npos ||
        description.find("احسب") != std::string::npos) {
        func_name = "احسب";
        return_type = "رقم";
    }
    else if (description.find("بحث") != std::string::npos ||
             description.find("ابحث") != std::string::npos) {
        func_name = "ابحث";
        return_type = "منطقي";
    }
    else if (description.find("تحويل") != std::string::npos ||
             description.find("حوّل") != std::string::npos) {
        func_name = "حوّل";
    }
    
    // استخراج المعلمات من الوصف
    if (description.find("قائمة") != std::string::npos) {
        params = "القائمة: قائمة";
    }
    if (description.find("نص") != std::string::npos) {
        if (!params.empty()) params += ", ";
        params += "النص: نص";
    }
    if (description.find("رقم") != std::string::npos) {
        if (!params.empty()) params += ", ";
        params += "الرقم: رقم";
    }
    
    // بناء الدالة
    code << "## " << description << "\n";
    code << "دالة " << func_name << "(" << params << ")";
    if (return_type != "عدم") {
        code << " -> " << return_type;
    }
    code << "\n";
    code << "    ## TODO: التنفيذ\n";
    code << "    إرجاع ";
    if (return_type == "رقم") code << "0";
    else if (return_type == "نص") code << "\"\"";
    else if (return_type == "منطقي") code << "خطأ";
    else if (return_type == "قائمة") code << "[]";
    else code << "عدم";
    code << "\n";
    code << "نهاية\n";
    
    return code.str();
}

// ══════════════════════════════════════════════════════════════════════════════
//  توليد الأصناف
// ══════════════════════════════════════════════════════════════════════════════

std::string AICopilotProvider::generate_class_from_description(const std::string& description) {
    std::ostringstream code;
    
    std::string class_name = "صنف_جديد";
    
    // محاولة استخراج الاسم من الوصف
    std::regex name_regex(R"(صنف\s+(\w+))");
    std::smatch match;
    if (std::regex_search(description, match, name_regex)) {
        class_name = match[1].str();
    }
    
    code << "## " << description << "\n";
    code << "صنف " << class_name << "\n";
    code << "    ## الخصائص\n";
    code << "    خاصية الاسم: نص\n";
    code << "    \n";
    code << "    ## المنشئ\n";
    code << "    دالة جديد(الاسم: نص)\n";
    code << "        هذا.الاسم = الاسم\n";
    code << "    نهاية\n";
    code << "    \n";
    code << "    ## دالة للعرض\n";
    code << "    دالة عرض() -> نص\n";
    code << "        إرجاع هذا.الاسم\n";
    code << "    نهاية\n";
    code << "نهاية\n";
    
    return code.str();
}

// ══════════════════════════════════════════════════════════════════════════════
//  توليد الاختبارات
// ══════════════════════════════════════════════════════════════════════════════

std::string AICopilotProvider::generate_tests(const std::string& function_code) {
    std::ostringstream tests;
    
    // استخراج اسم الدالة
    std::regex func_regex(R"(دالة\s+(\w+)\s*\()");
    std::smatch match;
    std::string func_name = "الدالة";
    
    if (std::regex_search(function_code, match, func_regex)) {
        func_name = match[1].str();
    }
    
    tests << "## ═══════════════════════════════════════════════════════════════════════════\n";
    tests << "## اختبارات " << func_name << "\n";
    tests << "## ═══════════════════════════════════════════════════════════════════════════\n";
    tests << "\n";
    tests << "## اختبار الحالة الأساسية\n";
    tests << "دالة اختبار_" << func_name << "_أساسي()\n";
    tests << "    ## الترتيب\n";
    tests << "    متغير المدخل = عدم  ## TODO: قيمة الإدخال\n";
    tests << "    \n";
    tests << "    ## التنفيذ\n";
    tests << "    متغير النتيجة = " << func_name << "(المدخل)\n";
    tests << "    \n";
    tests << "    ## التحقق\n";
    tests << "    أكد(النتيجة != عدم, \"يجب أن تُرجع الدالة قيمة\")\n";
    tests << "نهاية\n";
    tests << "\n";
    tests << "## اختبار الحافة\n";
    tests << "دالة اختبار_" << func_name << "_حافة()\n";
    tests << "    ## اختبار مع قيم حدية\n";
    tests << "    متغير النتيجة = " << func_name << "(عدم)\n";
    tests << "    أكد(النتيجة != عدم || صحيح, \"يجب معالجة القيم الفارغة\")\n";
    tests << "نهاية\n";
    
    return tests.str();
}

// ══════════════════════════════════════════════════════════════════════════════
//  اقتراح الإصلاحات
// ══════════════════════════════════════════════════════════════════════════════

std::vector<AISuggestion> AICopilotProvider::suggest_fixes(
    const Diagnostic& diagnostic,
    const std::string& document
) {
    std::vector<AISuggestion> fixes;
    
    const std::string& msg = diagnostic.message;
    
    // خطأ: متغير غير معرف
    if (msg.find("غير معرف") != std::string::npos || 
        msg.find("undefined") != std::string::npos) {
        
        // استخراج اسم المتغير
        std::regex var_regex(R"('(\w+)')");
        std::smatch match;
        std::string var_name;
        
        if (std::regex_search(msg, match, var_regex)) {
            var_name = match[1].str();
        }
        
        if (!var_name.empty()) {
            // اقتراح 1: تعريف المتغير
            AISuggestion define_fix;
            define_fix.id = "fix_define_" + var_name;
            define_fix.type = SuggestionType::Fix;
            define_fix.title = "تعريف المتغير '" + var_name + "'";
            define_fix.code = "متغير " + var_name + " = عدم  ## TODO: قيمة\n";
            define_fix.confidence = 0.8f;
            define_fix.source = "fix";
            fixes.push_back(define_fix);
            
            // اقتراح 2: هل تقصد متغير آخر؟
            // (في تطبيق حقيقي، نبحث عن أقرب متغير)
        }
    }
    
    // خطأ: نقص نهاية
    if (msg.find("نهاية") != std::string::npos ||
        msg.find("متوقع") != std::string::npos) {
        
        AISuggestion end_fix;
        end_fix.id = "fix_add_end";
        end_fix.type = SuggestionType::Fix;
        end_fix.title = "إضافة 'نهاية' المفقودة";
        end_fix.code = "نهاية\n";
        end_fix.confidence = 0.9f;
        end_fix.source = "fix";
        fixes.push_back(end_fix);
    }
    
    // خطأ: نوع غير صحيح
    if (msg.find("نوع") != std::string::npos) {
        AISuggestion type_fix;
        type_fix.id = "fix_type";
        type_fix.type = SuggestionType::Fix;
        type_fix.title = "تحويل النوع";
        type_fix.description = "استخدام دالة تحويل";
        type_fix.confidence = 0.7f;
        type_fix.source = "fix";
        fixes.push_back(type_fix);
    }
    
    return fixes;
}

// ══════════════════════════════════════════════════════════════════════════════
//  شرح الكود
// ══════════════════════════════════════════════════════════════════════════════

std::string AICopilotProvider::explain_code(const std::string& code, int detail_level) {
    std::ostringstream explanation;
    
    explanation << "## شرح الكود\n\n";
    
    // تحليل بسيط للكود
    if (code.find("دالة ") != std::string::npos) {
        std::regex func_regex(R"(دالة\s+(\w+)\s*\(([^)]*)\))");
        std::smatch match;
        if (std::regex_search(code, match, func_regex)) {
            explanation << "**نوع الكود:** تعريف دالة\n\n";
            explanation << "**اسم الدالة:** `" << match[1].str() << "`\n\n";
            if (!match[2].str().empty()) {
                explanation << "**المعلمات:** `" << match[2].str() << "`\n\n";
            }
        }
    }
    else if (code.find("صنف ") != std::string::npos) {
        explanation << "**نوع الكود:** تعريف صنف\n\n";
    }
    else if (code.find("إذا ") != std::string::npos) {
        explanation << "**نوع الكود:** جملة شرطية\n\n";
    }
    else if (code.find("لكل ") != std::string::npos || 
             code.find("طالما ") != std::string::npos) {
        explanation << "**نوع الكود:** حلقة تكرار\n\n";
    }
    
    if (detail_level >= 2) {
        explanation << "### الخطوات:\n";
        explanation << "1. تحليل المدخلات\n";
        explanation << "2. تنفيذ المنطق الأساسي\n";
        explanation << "3. إرجاع النتيجة\n";
    }
    
    if (detail_level >= 3) {
        explanation << "\n### ملاحظات:\n";
        explanation << "- تأكد من معالجة الحالات الحدية\n";
        explanation << "- أضف التوثيق للوضوح\n";
    }
    
    return explanation.str();
}

// ══════════════════════════════════════════════════════════════════════════════
//  توليد التوثيق
// ══════════════════════════════════════════════════════════════════════════════

std::string AICopilotProvider::generate_documentation(const std::string& function_code) {
    std::ostringstream doc;
    
    // استخراج معلومات الدالة
    std::regex func_regex(R"(دالة\s+(\w+)\s*\(([^)]*)\)(?:\s*->\s*(\w+))?)");
    std::smatch match;
    
    if (std::regex_search(function_code, match, func_regex)) {
        std::string func_name = match[1].str();
        std::string params = match[2].str();
        std::string return_type = match.size() > 3 ? match[3].str() : "";
        
        doc << "## ═══════════════════════════════════════════════════════════════════════════\n";
        doc << "## @دالة " << func_name << "\n";
        doc << "## @الوصف TODO: أضف وصفاً للدالة\n";
        doc << "## ═══════════════════════════════════════════════════════════════════════════\n";
        
        // تحليل المعلمات
        if (!params.empty()) {
            doc << "##\n";
            doc << "## @المعلمات:\n";
            
            std::regex param_regex(R"((\w+)\s*:\s*(\w+))");
            std::string::const_iterator searchStart(params.cbegin());
            while (std::regex_search(searchStart, params.cend(), match, param_regex)) {
                doc << "##   - " << match[1].str() << " (" << match[2].str() << "): TODO: وصف المعلمة\n";
                searchStart = match.suffix().first;
            }
        }
        
        if (!return_type.empty()) {
            doc << "##\n";
            doc << "## @الإرجاع: " << return_type << " - TODO: وصف القيمة المُرجعة\n";
        }
        
        doc << "##\n";
        doc << "## @مثال:\n";
        doc << "##   متغير النتيجة = " << func_name << "(...)\n";
        doc << "## ═══════════════════════════════════════════════════════════════════════════\n";
    }
    
    return doc.str();
}

// ══════════════════════════════════════════════════════════════════════════════
//  التحويل بين اللغات
// ══════════════════════════════════════════════════════════════════════════════

std::string AICopilotProvider::translate_to_sad(
    const std::string& code,
    const std::string& source_lang
) {
    std::string result = code;
    
    if (source_lang == "python" || source_lang == "بايثون") {
        // تحويل من بايثون
        result = std::regex_replace(result, std::regex("def "), "دالة ");
        result = std::regex_replace(result, std::regex("class "), "صنف ");
        result = std::regex_replace(result, std::regex("if "), "إذا ");
        result = std::regex_replace(result, std::regex("else:"), "وإلا");
        result = std::regex_replace(result, std::regex("elif "), "وإلا_إذا ");
        result = std::regex_replace(result, std::regex("for "), "لكل ");
        result = std::regex_replace(result, std::regex("while "), "طالما ");
        result = std::regex_replace(result, std::regex("return "), "إرجاع ");
        result = std::regex_replace(result, std::regex("print\\("), "اطبع(");
        result = std::regex_replace(result, std::regex("True"), "صحيح");
        result = std::regex_replace(result, std::regex("False"), "خطأ");
        result = std::regex_replace(result, std::regex("None"), "عدم");
        result = std::regex_replace(result, std::regex("self"), "هذا");
        result = std::regex_replace(result, std::regex(":$"), "\n    ## TODO: الكود\nنهاية");
    }
    else if (source_lang == "javascript" || source_lang == "جافاسكريبت") {
        // تحويل من جافاسكريبت
        result = std::regex_replace(result, std::regex("function "), "دالة ");
        result = std::regex_replace(result, std::regex("class "), "صنف ");
        result = std::regex_replace(result, std::regex("if \\("), "إذا (");
        result = std::regex_replace(result, std::regex("else \\{"), "وإلا");
        result = std::regex_replace(result, std::regex("for \\("), "لكل ");
        result = std::regex_replace(result, std::regex("while \\("), "طالما (");
        result = std::regex_replace(result, std::regex("return "), "إرجاع ");
        result = std::regex_replace(result, std::regex("console\\.log\\("), "اطبع(");
        result = std::regex_replace(result, std::regex("true"), "صحيح");
        result = std::regex_replace(result, std::regex("false"), "خطأ");
        result = std::regex_replace(result, std::regex("null"), "عدم");
        result = std::regex_replace(result, std::regex("this"), "هذا");
        result = std::regex_replace(result, std::regex("\\{"), "");
        result = std::regex_replace(result, std::regex("\\}"), "نهاية");
        result = std::regex_replace(result, std::regex("let |const |var "), "متغير ");
    }
    
    return result;
}

std::string AICopilotProvider::translate_from_sad(
    const std::string& code,
    const std::string& target_lang
) {
    std::string result = code;
    
    if (target_lang == "python" || target_lang == "بايثون") {
        result = std::regex_replace(result, std::regex("دالة "), "def ");
        result = std::regex_replace(result, std::regex("صنف "), "class ");
        result = std::regex_replace(result, std::regex("إذا "), "if ");
        result = std::regex_replace(result, std::regex("وإلا"), "else:");
        result = std::regex_replace(result, std::regex("وإلا_إذا "), "elif ");
        result = std::regex_replace(result, std::regex("لكل "), "for ");
        result = std::regex_replace(result, std::regex("طالما "), "while ");
        result = std::regex_replace(result, std::regex("إرجاع "), "return ");
        result = std::regex_replace(result, std::regex("اطبع\\("), "print(");
        result = std::regex_replace(result, std::regex("صحيح"), "True");
        result = std::regex_replace(result, std::regex("خطأ"), "False");
        result = std::regex_replace(result, std::regex("عدم"), "None");
        result = std::regex_replace(result, std::regex("هذا"), "self");
        result = std::regex_replace(result, std::regex("نهاية"), "");
        result = std::regex_replace(result, std::regex("متغير "), "");
    }
    
    return result;
}

// ══════════════════════════════════════════════════════════════════════════════
//  التحسينات
// ══════════════════════════════════════════════════════════════════════════════

std::vector<AISuggestion> AICopilotProvider::suggest_performance_improvements(
    const std::string& code
) {
    std::vector<AISuggestion> improvements;
    
    // البحث عن حلقات متداخلة (O(n²))
    size_t nested_loop = 0;
    int loop_depth = 0;
    for (size_t i = 0; i < code.size(); i++) {
        if (code.substr(i, 3) == "لكل" || code.substr(i, 5) == "طالما") {
            loop_depth++;
            if (loop_depth > 1) {
                nested_loop = i;
            }
        }
        if (code.substr(i, 5) == "نهاية") {
            loop_depth--;
        }
    }
    
    if (nested_loop > 0) {
        AISuggestion suggestion;
        suggestion.id = "perf_nested_loop";
        suggestion.type = SuggestionType::Refactor;
        suggestion.title = "تحسين الحلقات المتداخلة";
        suggestion.description = "الحلقات المتداخلة قد تؤدي إلى أداء O(n²). فكر في استخدام خريطة أو فهرس.";
        suggestion.confidence = 0.7f;
        improvements.push_back(suggestion);
    }
    
    // البحث عن استخدام + للنصوص في حلقة
    if (code.find("لكل") != std::string::npos && 
        code.find("+ \"") != std::string::npos) {
        AISuggestion suggestion;
        suggestion.id = "perf_string_concat";
        suggestion.type = SuggestionType::Refactor;
        suggestion.title = "تحسين دمج النصوص";
        suggestion.description = "دمج النصوص في حلقة بطيء. استخدم قائمة ثم انضم().";
        suggestion.confidence = 0.8f;
        improvements.push_back(suggestion);
    }
    
    return improvements;
}

std::vector<AISuggestion> AICopilotProvider::suggest_refactoring(
    const std::string& code,
    const std::string& refactor_type
) {
    std::vector<AISuggestion> suggestions;
    
    // دالة طويلة جداً
    int line_count = std::count(code.begin(), code.end(), '\n');
    if (line_count > 50) {
        AISuggestion suggestion;
        suggestion.id = "refactor_long_function";
        suggestion.type = SuggestionType::Refactor;
        suggestion.title = "تقسيم الدالة الطويلة";
        suggestion.description = "الدالة طويلة جداً (" + std::to_string(line_count) + 
                                " سطر). فكر في تقسيمها لدوال أصغر.";
        suggestion.confidence = 0.9f;
        suggestions.push_back(suggestion);
    }
    
    // كود مكرر (نمط بسيط)
    // (في تطبيق حقيقي، سنستخدم خوارزمية أكثر تقدماً)
    
    // معلمات كثيرة
    std::regex params_regex(R"(دالة\s+\w+\s*\(([^)]+)\))");
    std::smatch match;
    if (std::regex_search(code, match, params_regex)) {
        std::string params = match[1].str();
        int param_count = std::count(params.begin(), params.end(), ',') + 1;
        if (param_count > 5) {
            AISuggestion suggestion;
            suggestion.id = "refactor_many_params";
            suggestion.type = SuggestionType::Refactor;
            suggestion.title = "تقليل عدد المعلمات";
            suggestion.description = "الدالة لديها " + std::to_string(param_count) + 
                                    " معلمات. فكر في استخدام كائن إعدادات.";
            suggestion.confidence = 0.8f;
            suggestions.push_back(suggestion);
        }
    }
    
    return suggestions;
}

// ══════════════════════════════════════════════════════════════════════════════
//  التعلم
// ══════════════════════════════════════════════════════════════════════════════

void AICopilotProvider::record_acceptance(const std::string& suggestion_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    usage_stats_[suggestion_id]++;
    
    // تحديث معدل النجاح للنمط
    for (auto& pattern : patterns_) {
        if (pattern.id == suggestion_id) {
            pattern.usage_count++;
            pattern.success_rate = (pattern.success_rate * 0.9f) + 0.1f;
            break;
        }
    }
}

void AICopilotProvider::record_rejection(const std::string& suggestion_id) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (auto& pattern : patterns_) {
        if (pattern.id == suggestion_id) {
            pattern.success_rate = pattern.success_rate * 0.95f;
            break;
        }
    }
}

void AICopilotProvider::learn_pattern(const std::string& code, const std::string& context) {
    if (!learning_enabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // إنشاء نمط جديد من الكود
    CodePattern new_pattern;
    new_pattern.id = "learned_" + std::to_string(patterns_.size());
    new_pattern.name = "نمط متعلم";
    new_pattern.template_code = code;
    new_pattern.usage_count = 1;
    new_pattern.success_rate = 0.5f;
    
    patterns_.push_back(new_pattern);
}

// ══════════════════════════════════════════════════════════════════════════════
//  أدوات داخلية
// ══════════════════════════════════════════════════════════════════════════════

std::vector<AISuggestion> AICopilotProvider::filter_and_rank(
    std::vector<AISuggestion>& suggestions,
    const AIContext& context
) {
    // فلترة بالحد الأدنى للثقة
    suggestions.erase(
        std::remove_if(suggestions.begin(), suggestions.end(),
            [this](const AISuggestion& s) { return s.confidence < min_confidence_; }),
        suggestions.end()
    );
    
    // ترتيب بالأولوية والثقة
    std::sort(suggestions.begin(), suggestions.end(),
        [](const AISuggestion& a, const AISuggestion& b) {
            if (a.priority != b.priority) return a.priority > b.priority;
            return a.confidence > b.confidence;
        }
    );
    
    // تقليص للحد الأقصى
    if (static_cast<int>(suggestions.size()) > max_suggestions_) {
        suggestions.resize(max_suggestions_);
    }
    
    return suggestions;
}

float AICopilotProvider::evaluate_suggestion(
    const AISuggestion& suggestion,
    const AIContext& context
) {
    float score = suggestion.confidence;
    
    // زيادة للاقتراحات المتوافقة مع السياق
    if (suggestion.type == SuggestionType::Completion && context.is_partial_word) {
        score *= 1.2f;
    }
    
    // زيادة للأنماط الشائعة
    auto it = usage_stats_.find(suggestion.id);
    if (it != usage_stats_.end() && it->second > 10) {
        score *= 1.1f;
    }
    
    return std::min(1.0f, score);
}

// ══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ══════════════════════════════════════════════════════════════════════════════

std::string context_type_to_string(ContextType type) {
    switch (type) {
        case ContextType::Function: return "دالة";
        case ContextType::Class: return "صنف";
        case ContextType::Loop: return "حلقة";
        case ContextType::Condition: return "شرط";
        case ContextType::Global: return "عام";
        case ContextType::Import: return "استيراد";
        case ContextType::Comment: return "تعليق";
        case ContextType::String: return "نص";
        default: return "غير معروف";
    }
}

std::string suggestion_type_to_string(SuggestionType type) {
    switch (type) {
        case SuggestionType::Completion: return "إكمال";
        case SuggestionType::FullFunction: return "دالة كاملة";
        case SuggestionType::CodeBlock: return "كتلة كود";
        case SuggestionType::Fix: return "إصلاح";
        case SuggestionType::Refactor: return "إعادة هيكلة";
        case SuggestionType::Documentation: return "توثيق";
        case SuggestionType::Example: return "مثال";
        case SuggestionType::Translation: return "تحويل";
        default: return "غير معروف";
    }
}

std::string intent_to_string(Intent intent) {
    switch (intent) {
        case Intent::DefineVariable: return "تعريف متغير";
        case Intent::DefineFunction: return "تعريف دالة";
        case Intent::DefineClass: return "تعريف صنف";
        case Intent::CallFunction: return "استدعاء دالة";
        case Intent::AccessMember: return "وصول لعضو";
        case Intent::WriteCondition: return "كتابة شرط";
        case Intent::WriteLoop: return "كتابة حلقة";
        case Intent::HandleError: return "معالجة خطأ";
        case Intent::ReturnValue: return "إرجاع قيمة";
        case Intent::ImplementInterface: return "تنفيذ واجهة";
        case Intent::OverrideMethod: return "تجاوز دالة";
        case Intent::CreateConstructor: return "إنشاء منشئ";
        case Intent::AddValidation: return "إضافة تحقق";
        case Intent::AddLogging: return "إضافة سجل";
        case Intent::WriteComment: return "كتابة تعليق";
        case Intent::WriteDocumentation: return "كتابة توثيق";
        default: return "غير معروف";
    }
}

} // namespace ai
} // namespace lsp
} // namespace sad
