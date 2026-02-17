/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * بسم الله الرحمن الرحيم
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @file refactoring_provider.cpp
 * @brief تنفيذ مساعد إعادة الهيكلة الذكي للغة ص
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                        تنفيذ نظام إعادة الهيكلة الثوري
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على التنفيذ الكامل لجميع عمليات إعادة الهيكلة.
 * كل عملية مصممة لتكون آمنة وذكية.
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "refactoring_provider.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <cctype>

namespace sad {
namespace lsp {

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ TextEdit
// ═══════════════════════════════════════════════════════════════════════════════

std::string TextEdit::تطبيق(const std::string& الكود) const {
    auto lines = std::vector<std::string>();
    std::istringstream stream(الكود);
    std::string line;
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    // حساب موقع البداية والنهاية
    std::string result;
    
    for (int i = 0; i < النطاق.البداية.سطر && i < static_cast<int>(lines.size()); ++i) {
        result += lines[i] + "\n";
    }
    
    // إضافة الجزء قبل البداية في السطر
    if (النطاق.البداية.سطر < static_cast<int>(lines.size())) {
        result += lines[النطاق.البداية.سطر].substr(0, النطاق.البداية.عمود);
    }
    
    // إضافة النص الجديد
    result += النص_الجديد;
    
    // إضافة الجزء بعد النهاية في السطر
    if (النطاق.النهاية.سطر < static_cast<int>(lines.size())) {
        if (النطاق.النهاية.عمود < static_cast<int>(lines[النطاق.النهاية.سطر].length())) {
            result += lines[النطاق.النهاية.سطر].substr(النطاق.النهاية.عمود);
        }
    }
    result += "\n";
    
    // إضافة بقية الأسطر
    for (int i = النطاق.النهاية.سطر + 1; i < static_cast<int>(lines.size()); ++i) {
        result += lines[i] + "\n";
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ FileEdit
// ═══════════════════════════════════════════════════════════════════════════════

std::string FileEdit::تطبيق(const std::string& المحتوى) const {
    std::string result = المحتوى;
    
    // ترتيب التعديلات من النهاية للبداية لتجنب تعارض المواقع
    auto sorted_edits = التعديلات;
    std::sort(sorted_edits.begin(), sorted_edits.end(),
              [](const TextEdit& a, const TextEdit& b) {
                  return b.النطاق.البداية < a.النطاق.البداية;
              });
    
    for (const auto& edit : sorted_edits) {
        result = edit.تطبيق(result);
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ RefactoringResult
// ═══════════════════════════════════════════════════════════════════════════════

std::string RefactoringResult::معاينة() const {
    std::ostringstream ss;
    
    ss << "═══════════════════════════════════════════════════════════════\n";
    ss << "                     معاينة إعادة الهيكلة\n";
    ss << "═══════════════════════════════════════════════════════════════\n\n";
    
    ss << "الحالة: ";
    switch (الحالة) {
        case RefactoringStatus::جاهز: ss << "✅ جاهز للتنفيذ"; break;
        case RefactoringStatus::تحذير: ss << "⚠️ يمكن التنفيذ مع تحذيرات"; break;
        case RefactoringStatus::خطأ: ss << "❌ غير ممكن"; break;
        case RefactoringStatus::قيد_التنفيذ: ss << "🔄 قيد التنفيذ"; break;
    }
    ss << "\n\n";
    
    if (!الرسالة.empty()) {
        ss << "📋 " << الرسالة << "\n\n";
    }
    
    if (!التحذيرات.empty()) {
        ss << "⚠️ تحذيرات:\n";
        for (const auto& w : التحذيرات) {
            ss << "   • " << w << "\n";
        }
        ss << "\n";
    }
    
    if (!الأخطاء.empty()) {
        ss << "❌ أخطاء:\n";
        for (const auto& e : الأخطاء) {
            ss << "   • " << e << "\n";
        }
        ss << "\n";
    }
    
    ss << "📝 التعديلات (" << التعديلات.size() << " ملف):\n";
    ss << "───────────────────────────────────────────────────────────────\n";
    
    for (const auto& file_edit : التعديلات) {
        ss << "\n📄 " << file_edit.مسار_الملف << " (" 
           << file_edit.التعديلات.size() << " تعديل)\n";
        
        for (const auto& edit : file_edit.التعديلات) {
            ss << "   " << edit.النطاق.إلى_نص() << "\n";
            if (!edit.الوصف.empty()) {
                ss << "   └─ " << edit.الوصف << "\n";
            }
        }
    }
    
    return ss.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              المُنشئ
// ═══════════════════════════════════════════════════════════════════════════════

RefactoringProvider::RefactoringProvider() {
    // تهيئة الكلمات المحجوزة في لغة ص
    الكلمات_المحجوزة_ = {
        "متغير", "ثابت", "دالة", "أرجع", "إرجاع",
        "إذا", "وإلا", "طالما", "لكل", "في",
        "اختر", "حالة", "افتراضي",
        "صحيح", "خطأ", "عدم",
        "اطبع", "اقرأ", "ادخال",
        "توقف", "استمر", "كرر",
        "نوع", "صنف", "هذا",
        "و", "أو", "ليس"
    };
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              إعادة التسمية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief إعادة تسمية رمز في الكود
 * 
 * هذه العملية تبحث عن الرمز في الموقع المحدد، تجد تعريفه
 * وجميع استخداماته، ثم تغير الاسم في كل مكان.
 */
RefactoringResult RefactoringProvider::إعادة_تسمية(
    const std::string& الكود,
    const CodeLocation& الموقع,
    const std::string& الاسم_الجديد,
    const RenameOptions& خيارات) {
    
    RefactoringResult result;
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 1: التحقق من صلاحية الاسم الجديد
    // ─────────────────────────────────────────────────────────────────────────
    
    if (الاسم_الجديد.empty()) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("الاسم الجديد فارغ");
        return result;
    }
    
    if (!معرف_صالح_(الاسم_الجديد)) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("الاسم الجديد '" + الاسم_الجديد + "' غير صالح كمعرف");
        return result;
    }
    
    if (الكلمات_المحجوزة_.count(الاسم_الجديد) > 0) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("الاسم '" + الاسم_الجديد + "' كلمة محجوزة في اللغة");
        return result;
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 2: إيجاد الرمز الأصلي
    // ─────────────────────────────────────────────────────────────────────────
    
    auto symbol_opt = إيجاد_الرمز_(الكود, الموقع);
    if (!symbol_opt) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("لم يُعثر على رمز في الموقع المحدد");
        return result;
    }
    
    const auto& symbol = *symbol_opt;
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 3: التحقق من التعارضات
    // ─────────────────────────────────────────────────────────────────────────
    
    if (خيارات.تحقق_من_التعارضات) {
        // فحص ما إذا كان الاسم الجديد موجوداً بالفعل
        auto existing = إيجاد_المراجع_(الكود, الاسم_الجديد);
        if (!existing.empty()) {
            result.الحالة = RefactoringStatus::تحذير;
            result.التحذيرات.push_back(
                "يوجد رمز آخر بالاسم '" + الاسم_الجديد + "' - قد يحدث تعارض");
        }
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 4: إيجاد جميع المراجع
    // ─────────────────────────────────────────────────────────────────────────
    
    auto references = إيجاد_المراجع_(الكود, symbol.الاسم);
    
    if (references.empty()) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("لم يُعثر على استخدامات للرمز");
        return result;
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 5: إنشاء التعديلات
    // ─────────────────────────────────────────────────────────────────────────
    
    FileEdit file_edit;
    file_edit.مسار_الملف = symbol.الملف.empty() ? "<current>" : symbol.الملف;
    
    // ترتيب المراجع من النهاية للبداية
    std::sort(references.begin(), references.end(),
              [](const CodeRange& a, const CodeRange& b) {
                  return b.البداية < a.البداية;
              });
    
    for (const auto& ref : references) {
        TextEdit edit;
        edit.النطاق = ref;
        edit.النص_الجديد = الاسم_الجديد;
        edit.الوصف = "تغيير '" + symbol.الاسم + "' إلى '" + الاسم_الجديد + "'";
        file_edit.التعديلات.push_back(edit);
    }
    
    result.التعديلات.push_back(file_edit);
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 6: إنشاء الرسالة النهائية
    // ─────────────────────────────────────────────────────────────────────────
    
    if (result.الحالة != RefactoringStatus::خطأ && 
        result.الحالة != RefactoringStatus::تحذير) {
        result.الحالة = RefactoringStatus::جاهز;
    }
    
    result.الرسالة = "سيتم تغيير '" + symbol.الاسم + "' إلى '" + الاسم_الجديد + 
                    "' في " + std::to_string(references.size()) + " موقع";
    
    return result;
}

std::vector<CodeRange> RefactoringProvider::معاينة_إعادة_التسمية(
    const std::string& الكود,
    const CodeLocation& الموقع) {
    
    auto symbol_opt = إيجاد_الرمز_(الكود, الموقع);
    if (!symbol_opt) {
        return {};
    }
    
    return إيجاد_المراجع_(الكود, symbol_opt->الاسم);
}

std::string RefactoringProvider::التحقق_من_اسم(
    const std::string& الكود,
    const CodeLocation& الموقع,
    const std::string& الاسم_الجديد) {
    
    if (الاسم_الجديد.empty()) {
        return "الاسم لا يمكن أن يكون فارغاً";
    }
    
    if (!معرف_صالح_(الاسم_الجديد)) {
        return "الاسم يحتوي على أحرف غير مسموحة";
    }
    
    if (الكلمات_المحجوزة_.count(الاسم_الجديد) > 0) {
        return "هذا الاسم كلمة محجوزة في اللغة";
    }
    
    // التحقق من عدم وجود تعارض
    auto existing = إيجاد_المراجع_(الكود, الاسم_الجديد);
    if (!existing.empty()) {
        return "يوجد رمز آخر بهذا الاسم في النطاق الحالي";
    }
    
    return ""; // لا خطأ
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              استخراج الدوال
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief استخراج جزء من الكود إلى دالة منفصلة
 * 
 * هذه العملية معقدة وتتطلب:
 * 1. تحليل المتغيرات الداخلة (المُستخدمة من خارج النطاق)
 * 2. تحليل المتغيرات الخارجة (المُعرَّفة داخل النطاق وتُستخدم خارجه)
 * 3. إنشاء توقيع الدالة
 * 4. استبدال الكود الأصلي باستدعاء الدالة
 */
RefactoringResult RefactoringProvider::استخراج_دالة(
    const std::string& الكود,
    const CodeRange& النطاق,
    const ExtractFunctionOptions& خيارات) {
    
    RefactoringResult result;
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 1: التحقق من صلاحية النطاق
    // ─────────────────────────────────────────────────────────────────────────
    
    if (النطاق.فارغ()) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("النطاق المحدد فارغ");
        return result;
    }
    
    // استخراج الكود المحدد
    std::string selected_code = استخراج_نص_(الكود, النطاق);
    
    if (selected_code.empty()) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("لم يُعثر على كود في النطاق المحدد");
        return result;
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 2: تحليل المتغيرات
    // ─────────────────────────────────────────────────────────────────────────
    
    std::set<std::string> المُستخدمة;
    std::set<std::string> المُعرَّفة;
    تحليل_المتغيرات_(الكود, النطاق, المُستخدمة, المُعرَّفة);
    
    // المتغيرات الداخلة = المُستخدمة - المُعرَّفة (داخل النطاق)
    std::vector<std::string> params;
    for (const auto& v : المُستخدمة) {
        if (المُعرَّفة.count(v) == 0) {
            params.push_back(v);
        }
    }
    
    // المتغيرات الخارجة = المُعرَّفة التي تُستخدم بعد النطاق
    // (تبسيط: نفترض كلها قد تُستخدم)
    std::vector<std::string> returns;
    for (const auto& v : المُعرَّفة) {
        returns.push_back(v);
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 3: تحديد اسم الدالة
    // ─────────────────────────────────────────────────────────────────────────
    
    std::string اسم_الدالة = خيارات.اسم_الدالة;
    if (اسم_الدالة.empty()) {
        اسم_الدالة = اقتراح_اسم_دالة(selected_code);
    }
    
    if (!معرف_صالح_(اسم_الدالة)) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("اسم الدالة '" + اسم_الدالة + "' غير صالح");
        return result;
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 4: بناء الدالة الجديدة
    // ─────────────────────────────────────────────────────────────────────────
    
    std::ostringstream func;
    
    // التوثيق
    if (خيارات.إنشاء_توثيق) {
        func << "/**\n";
        func << " * @brief دالة مُستخرجة\n";
        if (!params.empty()) {
            for (const auto& p : params) {
                func << " * @param " << p << " [وصف]\n";
            }
        }
        if (!returns.empty()) {
            func << " * @return [وصف القيمة المُرجعة]\n";
        }
        func << " */\n";
    }
    
    // توقيع الدالة
    func << "دالة " << اسم_الدالة << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) func << "، ";
        func << params[i];
    }
    func << ") {\n";
    
    // جسم الدالة
    auto lines = تقسيم_أسطر_(selected_code);
    for (const auto& line : lines) {
        func << "    " << line << "\n";
    }
    
    // العودة
    if (returns.size() == 1) {
        func << "    أرجع " << returns[0] << "\n";
    } else if (returns.size() > 1) {
        func << "    أرجع [";
        for (size_t i = 0; i < returns.size(); ++i) {
            if (i > 0) func << "، ";
            func << returns[i];
        }
        func << "]\n";
    }
    
    func << "}\n\n";
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 5: بناء استدعاء الدالة
    // ─────────────────────────────────────────────────────────────────────────
    
    std::ostringstream call;
    
    if (returns.size() == 1) {
        call << "متغير " << returns[0] << " = ";
    } else if (returns.size() > 1) {
        call << "متغير [";
        for (size_t i = 0; i < returns.size(); ++i) {
            if (i > 0) call << "، ";
            call << returns[i];
        }
        call << "] = ";
    }
    
    call << اسم_الدالة << "(";
    for (size_t i = 0; i < params.size(); ++i) {
        if (i > 0) call << "، ";
        call << params[i];
    }
    call << ")";
    
    // ─────────────────────────────────────────────────────────────────────────
    // الخطوة 6: إنشاء التعديلات
    // ─────────────────────────────────────────────────────────────────────────
    
    FileEdit file_edit;
    file_edit.مسار_الملف = "<current>";
    
    // التعديل 1: إضافة الدالة الجديدة (قبل النطاق المحدد)
    TextEdit add_function;
    add_function.النطاق.البداية.سطر = 0;
    add_function.النطاق.البداية.عمود = 0;
    add_function.النطاق.النهاية = add_function.النطاق.البداية;
    add_function.النص_الجديد = func.str();
    add_function.الوصف = "إضافة الدالة الجديدة '" + اسم_الدالة + "'";
    file_edit.التعديلات.push_back(add_function);
    
    // التعديل 2: استبدال الكود الأصلي بالاستدعاء
    TextEdit replace_code;
    replace_code.النطاق = النطاق;
    replace_code.النص_الجديد = call.str();
    replace_code.الوصف = "استبدال الكود باستدعاء '" + اسم_الدالة + "'";
    file_edit.التعديلات.push_back(replace_code);
    
    result.التعديلات.push_back(file_edit);
    result.الحالة = RefactoringStatus::جاهز;
    result.الرسالة = "سيتم استخراج الكود المحدد إلى دالة '" + اسم_الدالة + "' مع " +
                    std::to_string(params.size()) + " معامل و" +
                    std::to_string(returns.size()) + " قيمة مُرجعة";
    
    return result;
}

ExtractedFunction RefactoringProvider::تحليل_للاستخراج(
    const std::string& الكود,
    const CodeRange& النطاق) {
    
    ExtractedFunction info;
    
    std::string selected_code = استخراج_نص_(الكود, النطاق);
    info.الجسم = selected_code;
    
    std::set<std::string> المُستخدمة;
    std::set<std::string> المُعرَّفة;
    تحليل_المتغيرات_(الكود, النطاق, المُستخدمة, المُعرَّفة);
    
    for (const auto& v : المُستخدمة) {
        if (المُعرَّفة.count(v) == 0) {
            info.المعاملات.push_back(v);
        }
    }
    
    for (const auto& v : المُعرَّفة) {
        info.المُرجعات.push_back(v);
    }
    
    info.الاسم = اقتراح_اسم_دالة(selected_code);
    
    return info;
}

std::string RefactoringProvider::اقتراح_اسم_دالة(const std::string& الكود) {
    // تحليل بسيط للكود لاقتراح اسم مناسب
    
    // البحث عن كلمات مفتاحية شائعة
    if (الكود.find("حساب") != std::string::npos ||
        الكود.find("+") != std::string::npos ||
        الكود.find("-") != std::string::npos ||
        الكود.find("*") != std::string::npos) {
        return "حساب_القيمة";
    }
    
    if (الكود.find("اطبع") != std::string::npos ||
        الكود.find("عرض") != std::string::npos) {
        return "عرض_النتيجة";
    }
    
    if (الكود.find("إذا") != std::string::npos ||
        الكود.find("تحقق") != std::string::npos) {
        return "تحقق_الشرط";
    }
    
    if (الكود.find("طالما") != std::string::npos ||
        الكود.find("لكل") != std::string::npos) {
        return "معالجة_العناصر";
    }
    
    if (الكود.find("قراءة") != std::string::npos ||
        الكود.find("اقرأ") != std::string::npos) {
        return "قراءة_المدخلات";
    }
    
    // الاسم الافتراضي
    return "دالة_مُستخرجة";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              استخراج المتغيرات
// ═══════════════════════════════════════════════════════════════════════════════

RefactoringResult RefactoringProvider::استخراج_متغير(
    const std::string& الكود,
    const CodeRange& النطاق,
    const ExtractVariableOptions& خيارات) {
    
    RefactoringResult result;
    
    std::string expression = استخراج_نص_(الكود, النطاق);
    
    if (expression.empty()) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("لم يُعثر على تعبير في النطاق المحدد");
        return result;
    }
    
    // تحديد اسم المتغير
    std::string var_name = خيارات.اسم_المتغير;
    if (var_name.empty()) {
        var_name = "القيمة";
    }
    
    if (!معرف_صالح_(var_name)) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("اسم المتغير غير صالح");
        return result;
    }
    
    FileEdit file_edit;
    file_edit.مسار_الملف = "<current>";
    
    // التعديل 1: إضافة تعريف المتغير قبل السطر الحالي
    TextEdit add_var;
    add_var.النطاق.البداية.سطر = النطاق.البداية.سطر;
    add_var.النطاق.البداية.عمود = 0;
    add_var.النطاق.النهاية = add_var.النطاق.البداية;
    
    std::string keyword = خيارات.جعله_ثابتاً ? "ثابت" : "متغير";
    add_var.النص_الجديد = keyword + " " + var_name + " = " + expression + "\n";
    add_var.الوصف = "إضافة تعريف المتغير";
    file_edit.التعديلات.push_back(add_var);
    
    // التعديل 2: استبدال التعبير باسم المتغير
    TextEdit replace_expr;
    replace_expr.النطاق = النطاق;
    replace_expr.النص_الجديد = var_name;
    replace_expr.الوصف = "استبدال التعبير باسم المتغير";
    file_edit.التعديلات.push_back(replace_expr);
    
    // إذا كان مطلوباً استبدال جميع التكرارات
    if (خيارات.استبدال_جميع_التكرارات) {
        // البحث عن تكرارات أخرى
        auto lines = تقسيم_أسطر_(الكود);
        for (size_t i = النطاق.النهاية.سطر; i < lines.size(); ++i) {
            size_t pos = 0;
            while ((pos = lines[i].find(expression, pos)) != std::string::npos) {
                TextEdit extra;
                extra.النطاق.البداية.سطر = static_cast<int>(i);
                extra.النطاق.البداية.عمود = static_cast<int>(pos);
                extra.النطاق.النهاية.سطر = static_cast<int>(i);
                extra.النطاق.النهاية.عمود = static_cast<int>(pos + expression.length());
                extra.النص_الجديد = var_name;
                extra.الوصف = "استبدال تكرار إضافي";
                file_edit.التعديلات.push_back(extra);
                pos += expression.length();
            }
        }
    }
    
    result.التعديلات.push_back(file_edit);
    result.الحالة = RefactoringStatus::جاهز;
    result.الرسالة = "سيتم استخراج '" + expression + "' إلى " + keyword + " '" + var_name + "'";
    
    return result;
}

RefactoringResult RefactoringProvider::استخراج_ثابت(
    const std::string& الكود,
    const CodeRange& النطاق,
    const std::string& اسم_الثابت) {
    
    ExtractVariableOptions opts;
    opts.اسم_المتغير = اسم_الثابت;
    opts.جعله_ثابتاً = true;
    opts.استبدال_جميع_التكرارات = true;
    
    return استخراج_متغير(الكود, النطاق, opts);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              الدمج (Inline)
// ═══════════════════════════════════════════════════════════════════════════════

RefactoringResult RefactoringProvider::دمج_دالة(
    const std::string& الكود,
    const CodeLocation& موقع_الاستدعاء) {
    
    RefactoringResult result;
    
    // إيجاد استدعاء الدالة في الموقع
    auto symbol_opt = إيجاد_الرمز_(الكود, موقع_الاستدعاء);
    
    if (!symbol_opt) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("لم يُعثر على استدعاء دالة في الموقع المحدد");
        return result;
    }
    
    // البحث عن تعريف الدالة
    std::regex func_def_regex("دالة\\s+" + symbol_opt->الاسم + "\\s*\\([^)]*\\)\\s*\\{");
    std::smatch match;
    
    if (!std::regex_search(الكود, match, func_def_regex)) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("لم يُعثر على تعريف الدالة '" + symbol_opt->الاسم + "'");
        return result;
    }
    
    result.الحالة = RefactoringStatus::تحذير;
    result.التحذيرات.push_back("دمج الدوال عملية معقدة وقد تتطلب مراجعة يدوية");
    result.الرسالة = "دمج الدالة '" + symbol_opt->الاسم + "' غير مكتمل التنفيذ حالياً";
    
    return result;
}

RefactoringResult RefactoringProvider::دمج_متغير(
    const std::string& الكود,
    const CodeLocation& الموقع) {
    
    RefactoringResult result;
    
    auto symbol_opt = إيجاد_الرمز_(الكود, الموقع);
    
    if (!symbol_opt) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("لم يُعثر على متغير في الموقع المحدد");
        return result;
    }
    
    // البحث عن تعريف المتغير للحصول على قيمته
    std::regex var_def_regex("متغير\\s+" + symbol_opt->الاسم + "\\s*=\\s*([^\n]+)");
    std::smatch match;
    
    if (!std::regex_search(الكود, match, var_def_regex)) {
        result.الحالة = RefactoringStatus::خطأ;
        result.الأخطاء.push_back("لم يُعثر على تعريف المتغير '" + symbol_opt->الاسم + "'");
        return result;
    }
    
    std::string value = match[1].str();
    
    // إنشاء التعديلات
    FileEdit file_edit;
    file_edit.مسار_الملف = "<current>";
    
    // استبدال كل استخدام بالقيمة
    auto references = إيجاد_المراجع_(الكود, symbol_opt->الاسم);
    
    for (const auto& ref : references) {
        TextEdit edit;
        edit.النطاق = ref;
        edit.النص_الجديد = value;
        edit.الوصف = "استبدال '" + symbol_opt->الاسم + "' بقيمته";
        file_edit.التعديلات.push_back(edit);
    }
    
    result.التعديلات.push_back(file_edit);
    result.الحالة = RefactoringStatus::جاهز;
    result.الرسالة = "سيتم استبدال المتغير '" + symbol_opt->الاسم + 
                    "' بقيمته في " + std::to_string(references.size()) + " موقع";
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تحويلات الكود
// ═══════════════════════════════════════════════════════════════════════════════

RefactoringResult RefactoringProvider::تحويل_إلى_switch(
    const std::string& الكود,
    const CodeLocation& الموقع) {
    
    RefactoringResult result;
    result.الحالة = RefactoringStatus::تحذير;
    result.الرسالة = "تحويل if إلى switch غير مكتمل التنفيذ حالياً";
    result.التحذيرات.push_back("هذه الميزة قيد التطوير");
    return result;
}

RefactoringResult RefactoringProvider::تحويل_إلى_if(
    const std::string& الكود,
    const CodeLocation& الموقع) {
    
    RefactoringResult result;
    result.الحالة = RefactoringStatus::تحذير;
    result.الرسالة = "تحويل switch إلى if غير مكتمل التنفيذ حالياً";
    result.التحذيرات.push_back("هذه الميزة قيد التطوير");
    return result;
}

RefactoringResult RefactoringProvider::تبسيط_شرط(
    const std::string& الكود,
    const CodeLocation& الموقع) {
    
    RefactoringResult result;
    result.الحالة = RefactoringStatus::تحذير;
    result.الرسالة = "تبسيط الشروط غير مكتمل التنفيذ حالياً";
    result.التحذيرات.push_back("هذه الميزة قيد التطوير");
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنظيم الكود
// ═══════════════════════════════════════════════════════════════════════════════

RefactoringResult RefactoringProvider::تنظيم_الاستيرادات(const std::string& الكود) {
    RefactoringResult result;
    
    // البحث عن جميع عبارات الاستيراد
    std::vector<std::string> imports;
    std::vector<int> import_lines;
    
    auto lines = تقسيم_أسطر_(الكود);
    
    std::regex import_regex(R"(استيراد\s+([^\n]+))");
    
    for (size_t i = 0; i < lines.size(); ++i) {
        std::smatch match;
        if (std::regex_search(lines[i], match, import_regex)) {
            imports.push_back(match[0].str());
            import_lines.push_back(static_cast<int>(i));
        }
    }
    
    if (imports.empty()) {
        result.الحالة = RefactoringStatus::جاهز;
        result.الرسالة = "لا توجد عبارات استيراد للتنظيم";
        return result;
    }
    
    // ترتيب الاستيرادات أبجدياً
    std::sort(imports.begin(), imports.end());
    
    // إزالة التكرارات
    imports.erase(std::unique(imports.begin(), imports.end()), imports.end());
    
    // إنشاء النص الجديد
    std::ostringstream new_imports;
    for (const auto& imp : imports) {
        new_imports << imp << "\n";
    }
    
    FileEdit file_edit;
    file_edit.مسار_الملف = "<current>";
    
    // حذف الاستيرادات القديمة
    // (تبسيط: نفترض أنها في بداية الملف)
    if (!import_lines.empty()) {
        TextEdit edit;
        edit.النطاق.البداية.سطر = import_lines.front();
        edit.النطاق.البداية.عمود = 0;
        edit.النطاق.النهاية.سطر = import_lines.back();
        edit.النطاق.النهاية.عمود = static_cast<int>(lines[import_lines.back()].length());
        edit.النص_الجديد = new_imports.str();
        edit.الوصف = "إعادة تنظيم الاستيرادات";
        file_edit.التعديلات.push_back(edit);
    }
    
    result.التعديلات.push_back(file_edit);
    result.الحالة = RefactoringStatus::جاهز;
    result.الرسالة = "تم تنظيم " + std::to_string(imports.size()) + " عبارة استيراد";
    
    return result;
}

RefactoringResult RefactoringProvider::إزالة_كود_ميت(const std::string& الكود) {
    RefactoringResult result;
    
    std::set<std::string> المُعرَّفة;
    std::set<std::string> المُستخدمة;
    
    // تحليل بسيط للمعرفات
    std::regex var_def_regex(R"(متغير\s+(\w+))");
    std::regex var_use_regex(R"(\b(\w+)\b)");
    
    auto lines = تقسيم_أسطر_(الكود);
    
    // جمع التعريفات
    for (const auto& line : lines) {
        std::smatch match;
        std::string::const_iterator start = line.begin();
        while (std::regex_search(start, line.cend(), match, var_def_regex)) {
            المُعرَّفة.insert(match[1].str());
            start = match.suffix().first;
        }
    }
    
    // جمع الاستخدامات (تبسيط)
    for (const auto& line : lines) {
        std::smatch match;
        std::string::const_iterator start = line.begin();
        while (std::regex_search(start, line.cend(), match, var_use_regex)) {
            المُستخدمة.insert(match[1].str());
            start = match.suffix().first;
        }
    }
    
    // إيجاد المتغيرات غير المستخدمة
    std::vector<std::string> unused;
    for (const auto& def : المُعرَّفة) {
        if (المُستخدمة.count(def) <= 1) { // تعريف واحد فقط
            unused.push_back(def);
        }
    }
    
    if (unused.empty()) {
        result.الحالة = RefactoringStatus::جاهز;
        result.الرسالة = "لا يوجد كود ميت";
        return result;
    }
    
    result.الحالة = RefactoringStatus::تحذير;
    result.الرسالة = "وُجد " + std::to_string(unused.size()) + " متغير غير مستخدم";
    
    for (const auto& u : unused) {
        result.التحذيرات.push_back("المتغير '" + u + "' يبدو غير مستخدم");
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              الاقتراحات الذكية
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<std::pair<RefactoringKind, std::string>> 
RefactoringProvider::الحصول_على_العمليات_المتاحة(
    const std::string& الكود,
    const CodeLocation& الموقع) {
    
    std::vector<std::pair<RefactoringKind, std::string>> operations;
    
    auto symbol_opt = إيجاد_الرمز_(الكود, الموقع);
    
    if (symbol_opt) {
        // إعادة التسمية متاحة دائماً للرموز
        operations.push_back({RefactoringKind::إعادة_تسمية_متغير, 
                             "إعادة تسمية '" + symbol_opt->الاسم + "'"});
        
        // دمج المتغير
        operations.push_back({RefactoringKind::دمج_متغير,
                             "دمج المتغير '" + symbol_opt->الاسم + "'"});
    }
    
    // تنظيم الاستيرادات متاح دائماً
    operations.push_back({RefactoringKind::تنظيم_الاستيرادات,
                         "تنظيم الاستيرادات"});
    
    return operations;
}

std::vector<std::pair<RefactoringKind, std::string>>
RefactoringProvider::الحصول_على_عمليات_النطاق(
    const std::string& الكود,
    const CodeRange& النطاق) {
    
    std::vector<std::pair<RefactoringKind, std::string>> operations;
    
    if (!النطاق.فارغ()) {
        // استخراج دالة
        operations.push_back({RefactoringKind::استخراج_دالة,
                             "استخراج إلى دالة جديدة"});
        
        // استخراج متغير
        operations.push_back({RefactoringKind::استخراج_متغير,
                             "استخراج إلى متغير"});
        
        // استخراج ثابت
        operations.push_back({RefactoringKind::استخراج_ثابت,
                             "استخراج إلى ثابت"});
    }
    
    return operations;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              الدوال المساعدة
// ═══════════════════════════════════════════════════════════════════════════════

std::optional<Symbol> RefactoringProvider::إيجاد_الرمز_(
    const std::string& الكود,
    const CodeLocation& الموقع) {
    
    auto lines = تقسيم_أسطر_(الكود);
    
    if (الموقع.سطر < 0 || الموقع.سطر >= static_cast<int>(lines.size())) {
        return std::nullopt;
    }
    
    const auto& line = lines[الموقع.سطر];
    
    if (الموقع.عمود < 0 || الموقع.عمود >= static_cast<int>(line.length())) {
        return std::nullopt;
    }
    
    // إيجاد حدود الكلمة
    int start = الموقع.عمود;
    int end = الموقع.عمود;
    
    // التوسع للخلف
    while (start > 0 && (std::isalnum(line[start - 1]) || 
                         (static_cast<unsigned char>(line[start - 1]) >= 0x80))) {
        start--;
    }
    
    // التوسع للأمام
    while (end < static_cast<int>(line.length()) && 
           (std::isalnum(line[end]) || 
            (static_cast<unsigned char>(line[end]) >= 0x80))) {
        end++;
    }
    
    if (start == end) {
        return std::nullopt;
    }
    
    Symbol symbol;
    symbol.الاسم = line.substr(start, end - start);
    symbol.نطاق_التعريف.البداية.سطر = الموقع.سطر;
    symbol.نطاق_التعريف.البداية.عمود = start;
    symbol.نطاق_التعريف.النهاية.سطر = الموقع.سطر;
    symbol.نطاق_التعريف.النهاية.عمود = end;
    
    return symbol;
}

std::vector<CodeRange> RefactoringProvider::إيجاد_المراجع_(
    const std::string& الكود,
    const std::string& اسم_الرمز) {
    
    std::vector<CodeRange> refs;
    auto lines = تقسيم_أسطر_(الكود);
    
    for (size_t i = 0; i < lines.size(); ++i) {
        size_t pos = 0;
        while ((pos = lines[i].find(اسم_الرمز, pos)) != std::string::npos) {
            // التحقق من أنها كلمة كاملة
            bool word_start = (pos == 0 || 
                              (!std::isalnum(lines[i][pos - 1]) && 
                               static_cast<unsigned char>(lines[i][pos - 1]) < 0x80));
            bool word_end = (pos + اسم_الرمز.length() >= lines[i].length() ||
                            (!std::isalnum(lines[i][pos + اسم_الرمز.length()]) &&
                             static_cast<unsigned char>(lines[i][pos + اسم_الرمز.length()]) < 0x80));
            
            if (word_start && word_end) {
                CodeRange range;
                range.البداية.سطر = static_cast<int>(i);
                range.البداية.عمود = static_cast<int>(pos);
                range.النهاية.سطر = static_cast<int>(i);
                range.النهاية.عمود = static_cast<int>(pos + اسم_الرمز.length());
                refs.push_back(range);
            }
            
            pos += اسم_الرمز.length();
        }
    }
    
    return refs;
}

void RefactoringProvider::تحليل_المتغيرات_(
    const std::string& الكود,
    const CodeRange& النطاق,
    std::set<std::string>& المُستخدمة,
    std::set<std::string>& المُعرَّفة) {
    
    std::string selected = استخراج_نص_(الكود, النطاق);
    
    // البحث عن التعريفات
    std::regex def_regex(R"(متغير\s+(\w+)|ثابت\s+(\w+))");
    std::smatch match;
    std::string::const_iterator start = selected.begin();
    
    while (std::regex_search(start, selected.cend(), match, def_regex)) {
        if (match[1].matched) {
            المُعرَّفة.insert(match[1].str());
        } else if (match[2].matched) {
            المُعرَّفة.insert(match[2].str());
        }
        start = match.suffix().first;
    }
    
    // البحث عن الاستخدامات (كل كلمة)
    std::regex word_regex(R"(\b([^\s\d\W]\w*)\b)");
    start = selected.begin();
    
    while (std::regex_search(start, selected.cend(), match, word_regex)) {
        المُستخدمة.insert(match[1].str());
        start = match.suffix().first;
    }
}

bool RefactoringProvider::معرف_صالح_(const std::string& الاسم) {
    if (الاسم.empty()) return false;
    
    // الحرف الأول: حرف أو شرطة سفلية
    unsigned char first = الاسم[0];
    if (!std::isalpha(first) && first != '_' && first < 0x80) {
        // قد يكون حرفاً عربياً (UTF-8)
        if (!(first >= 0xD8 && first <= 0xDB)) {
            return false;
        }
    }
    
    // الأحرف التالية: أحرف أو أرقام أو شرطة سفلية
    for (size_t i = 1; i < الاسم.size(); ++i) {
        unsigned char c = الاسم[i];
        if (!std::isalnum(c) && c != '_' && c < 0x80) {
            if (!(c >= 0xD8 && c <= 0xDB)) {
                // قد يكون جزءاً من UTF-8
                if ((c & 0xC0) != 0x80) {
                    return false;
                }
            }
        }
    }
    
    return true;
}

std::string RefactoringProvider::استخراج_نص_(
    const std::string& الكود,
    const CodeRange& النطاق) {
    
    auto lines = تقسيم_أسطر_(الكود);
    
    if (النطاق.البداية.سطر < 0 || 
        النطاق.البداية.سطر >= static_cast<int>(lines.size())) {
        return "";
    }
    
    std::ostringstream ss;
    
    for (int i = النطاق.البداية.سطر; i <= النطاق.النهاية.سطر && 
         i < static_cast<int>(lines.size()); ++i) {
        
        const auto& line = lines[i];
        int start_col = (i == النطاق.البداية.سطر) ? النطاق.البداية.عمود : 0;
        int end_col = (i == النطاق.النهاية.سطر) ? النطاق.النهاية.عمود 
                                                : static_cast<int>(line.length());
        
        if (start_col < static_cast<int>(line.length())) {
            ss << line.substr(start_col, end_col - start_col);
        }
        
        if (i < النطاق.النهاية.سطر) {
            ss << "\n";
        }
    }
    
    return ss.str();
}

std::vector<std::string> RefactoringProvider::تقسيم_أسطر_(const std::string& الكود) {
    std::vector<std::string> lines;
    std::istringstream stream(الكود);
    std::string line;
    
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }
    
    return lines;
}

size_t RefactoringProvider::موقع_إلى_إزاحة_(
    const std::string& الكود,
    const CodeLocation& الموقع) {
    
    size_t offset = 0;
    int line = 0;
    
    for (size_t i = 0; i < الكود.size() && line < الموقع.سطر; ++i) {
        if (الكود[i] == '\n') {
            line++;
        }
        offset++;
    }
    
    return offset + الموقع.عمود;
}

CodeLocation RefactoringProvider::إزاحة_إلى_موقع_(
    const std::string& الكود,
    size_t الإزاحة) {
    
    CodeLocation loc;
    
    for (size_t i = 0; i < الإزاحة && i < الكود.size(); ++i) {
        if (الكود[i] == '\n') {
            loc.سطر++;
            loc.عمود = 0;
        } else {
            loc.عمود++;
        }
    }
    
    return loc;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              دوال المصنع
// ═══════════════════════════════════════════════════════════════════════════════

std::unique_ptr<RefactoringProvider> إنشاء_مزود_إعادة_هيكلة() {
    return std::make_unique<RefactoringProvider>();
}

} // namespace lsp
} // namespace sad
