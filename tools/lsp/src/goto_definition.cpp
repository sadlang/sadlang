// بسم الله الرحمن الرحيم
/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: goto_definition.cpp
 * الوصف: مزود الانتقال للتعريف للغة ص - Go to Definition Provider
 * المرحلة: Phase 8 - US6 - أدوات التطوير
 * المهمة: T096
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يوفر خدمة الانتقال للتعريف (Go to Definition) للغة ص.
 * 
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║  ما هو الانتقال للتعريف؟                                                 ║
 * ╠═══════════════════════════════════════════════════════════════════════════╣
 * ║  عند الضغط على F12 أو Ctrl+Click على رمز (متغير، دالة، صنف)،            ║
 * ║  ينتقل المحرر مباشرة إلى مكان تعريف هذا الرمز.                           ║
 * ║                                                                           ║
 * ║  هذه الميزة تساعد المطور على:                                            ║
 * ║  - فهم الكود بسرعة                                                       ║
 * ║  - التنقل في المشاريع الكبيرة                                            ║
 * ║  - اكتشاف تفاصيل التنفيذ                                                 ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * الخدمات المقدمة:
 * 
 * ┌────────────────────────────────────────────────────────────────────────────┐
 * │ الخدمة              │ الوصف                                               │
 * ├────────────────────────────────────────────────────────────────────────────┤
 * │ Go to Definition    │ الانتقال لمكان تعريف الرمز                          │
 * │ Go to Declaration   │ الانتقال لمكان إعلان الرمز                          │
 * │ Find References     │ إيجاد جميع مواقع استخدام الرمز                      │
 * │ Find Implementations│ إيجاد تنفيذات الواجهة                               │
 * │ Peek Definition     │ عرض التعريف في نافذة صغيرة                          │
 * └────────────────────────────────────────────────────────────────────────────┘
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * مثال:
 * 
 * ```ص
 * دالة حساب_المجموع(قائمة: قائمة[رقم]) -> رقم    <- التعريف (سطر 1)
 *     رقم مجموع = 0
 *     لكل عنصر في قائمة
 *         مجموع = مجموع + عنصر
 *     نهاية
 *     إرجاع مجموع
 * نهاية
 * 
 * رقم نتيجة = حساب_المجموع([1, 2, 3])           <- الاستخدام (سطر 9)
 *                 ^
 *                 |
 *          Ctrl+Click هنا ينقل للسطر 1
 * ```
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0.0
 * 
 * الحمد لله رب العالمين
 */

#include "goto_definition.hpp"
#include "../lsp_types.h"

#include <regex>
#include <unordered_map>

namespace sad {
namespace lsp {

// ═══════════════════════════════════════════════════════════════════════════════
// صنف مزود الانتقال للتعريف - Goto Definition Provider Class
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @class GotoDefinitionProvider
 * @brief يوفر خدمة الانتقال للتعريف وإيجاد المراجع
 */
class GotoDefinitionProvider {
public:
    /**
     * @brief الانتقال للتعريف
     * @param doc المستند الحالي
     * @param pos الموقع
     * @param all_docs جميع المستندات المفتوحة
     * @return موقع التعريف
     */
    std::optional<Location> get_definition(
        const DocumentInfo& doc,
        const Position& pos,
        const std::unordered_map<std::string, DocumentInfo>& all_docs) {
        
        // الحصول على الكلمة تحت المؤشر
        std::string word = get_word_at_position(doc, pos);
        
        if (word.empty()) {
            return std::nullopt;
        }
        
        // البحث في المستند الحالي
        auto local_def = find_definition_in_document(doc, word);
        if (local_def.has_value()) {
            Location loc;
            loc.uri = doc.uri;
            loc.range = local_def.value();
            return loc;
        }
        
        // البحث في المستندات الأخرى
        for (const auto& [uri, other_doc] : all_docs) {
            if (uri == doc.uri) continue;
            
            auto def = find_definition_in_document(other_doc, word);
            if (def.has_value()) {
                Location loc;
                loc.uri = uri;
                loc.range = def.value();
                return loc;
            }
        }
        
        return std::nullopt;
    }
    
    /**
     * @brief إيجاد جميع المراجع
     * @param doc المستند الحالي
     * @param pos الموقع
     * @param all_docs جميع المستندات المفتوحة
     * @return قائمة المراجع
     */
    std::vector<Location> get_references(
        const DocumentInfo& doc,
        const Position& pos,
        const std::unordered_map<std::string, DocumentInfo>& all_docs) {
        
        std::vector<Location> references;
        
        // الحصول على الكلمة تحت المؤشر
        std::string word = get_word_at_position(doc, pos);
        
        if (word.empty()) {
            return references;
        }
        
        // البحث في جميع المستندات
        for (const auto& [uri, document] : all_docs) {
            auto refs = find_references_in_document(document, word);
            for (const auto& ref : refs) {
                Location loc;
                loc.uri = uri;
                loc.range = ref;
                references.push_back(loc);
            }
        }
        
        return references;
    }
    
    /**
     * @brief إيجاد التنفيذات (للواجهات والأصناف المجردة)
     */
    std::vector<Location> get_implementations(
        const DocumentInfo& doc,
        const Position& pos,
        const std::unordered_map<std::string, DocumentInfo>& all_docs) {
        
        std::vector<Location> implementations;
        
        // الحصول على الكلمة تحت المؤشر
        std::string word = get_word_at_position(doc, pos);
        
        if (word.empty()) {
            return implementations;
        }
        
        // البحث عن الأصناف التي تنفذ الواجهة
        std::regex impl_regex(R"(صنف\s+\w+\s+ينفذ\s+)" + word);
        
        for (const auto& [uri, document] : all_docs) {
            for (int line = 0; line < static_cast<int>(document.lines.size()); ++line) {
                const std::string& text = document.lines[line];
                
                std::smatch match;
                if (std::regex_search(text, match, impl_regex)) {
                    Location loc;
                    loc.uri = uri;
                    loc.range.start = {line, static_cast<int>(match.position())};
                    loc.range.end = {line, static_cast<int>(match.position() + match.length())};
                    implementations.push_back(loc);
                }
            }
        }
        
        return implementations;
    }

private:
    /**
     * @brief الحصول على الكلمة في موقع محدد
     */
    std::string get_word_at_position(const DocumentInfo& doc, const Position& pos) {
        if (pos.line < 0 || pos.line >= static_cast<int>(doc.lines.size())) {
            return "";
        }
        
        const std::string& line = doc.lines[pos.line];
        
        if (pos.character < 0 || pos.character > static_cast<int>(line.length())) {
            return "";
        }
        
        // إيجاد بداية الكلمة
        int start = pos.character;
        while (start > 0 && is_word_char(line[start - 1])) {
            start--;
        }
        
        // إيجاد نهاية الكلمة
        int end = pos.character;
        while (end < static_cast<int>(line.length()) && is_word_char(line[end])) {
            end++;
        }
        
        if (start >= end) {
            return "";
        }
        
        return line.substr(start, end - start);
    }
    
    /**
     * @brief التحقق من حرف الكلمة
     */
    bool is_word_char(char c) {
        return (c >= 'a' && c <= 'z') ||
               (c >= 'A' && c <= 'Z') ||
               (c >= '0' && c <= '9') ||
               c == '_' ||
               (static_cast<unsigned char>(c) >= 0x80); // UTF-8 للعربية
    }
    
    /**
     * @brief البحث عن تعريف في مستند
     */
    std::optional<Range> find_definition_in_document(
        const DocumentInfo& doc,
        const std::string& name) {
        
        // البحث في الرموز المحللة
        for (const auto& sym : doc.symbols) {
            if (sym.name == name) {
                return sym.range;
            }
        }
        
        // البحث بالتعبيرات النمطية
        
        // دالة: دالة اسم(...)
        std::regex func_def("دالة\\s+" + escape_regex(name) + "\\s*\\(");
        
        // متغير: نوع اسم =
        std::regex var_def("(رقم|نص|منطقي|قائمة|خريطة)\\s+" + escape_regex(name) + "\\s*=");
        
        // صنف: صنف اسم
        std::regex class_def("صنف\\s+" + escape_regex(name));
        
        // ثابت: ثابت اسم =
        std::regex const_def("ثابت\\s+" + escape_regex(name) + "\\s*=");
        
        // واجهة: واجهة اسم
        std::regex interface_def("واجهة\\s+" + escape_regex(name));
        
        // معلمة دالة: (اسم: نوع)
        std::regex param_def("\\(.*" + escape_regex(name) + "\\s*:");
        
        for (int line = 0; line < static_cast<int>(doc.lines.size()); ++line) {
            const std::string& text = doc.lines[line];
            
            std::smatch match;
            
            // البحث عن تعريف دالة
            if (std::regex_search(text, match, func_def)) {
                return create_range(line, text, name);
            }
            
            // البحث عن تعريف متغير
            if (std::regex_search(text, match, var_def)) {
                return create_range(line, text, name);
            }
            
            // البحث عن تعريف صنف
            if (std::regex_search(text, match, class_def)) {
                return create_range(line, text, name);
            }
            
            // البحث عن تعريف ثابت
            if (std::regex_search(text, match, const_def)) {
                return create_range(line, text, name);
            }
            
            // البحث عن تعريف واجهة
            if (std::regex_search(text, match, interface_def)) {
                return create_range(line, text, name);
            }
        }
        
        return std::nullopt;
    }
    
    /**
     * @brief البحث عن المراجع في مستند
     */
    std::vector<Range> find_references_in_document(
        const DocumentInfo& doc,
        const std::string& name) {
        
        std::vector<Range> references;
        
        // تعبير نمطي للبحث عن الكلمة ككلمة كاملة
        // نستخدم word boundary للتأكد من أنها كلمة كاملة
        std::string pattern = "\\b" + escape_regex(name) + "\\b";
        std::regex word_regex(pattern);
        
        for (int line = 0; line < static_cast<int>(doc.lines.size()); ++line) {
            const std::string& text = doc.lines[line];
            
            // تخطي التعليقات
            if (text.find("//") == 0) continue;
            
            std::string::const_iterator search_start = text.cbegin();
            std::smatch match;
            
            while (std::regex_search(search_start, text.cend(), match, word_regex)) {
                Range range;
                int col = static_cast<int>(match.position() + (search_start - text.cbegin()));
                range.start = {line, col};
                range.end = {line, col + static_cast<int>(name.length())};
                references.push_back(range);
                
                search_start = match.suffix().first;
            }
        }
        
        return references;
    }
    
    /**
     * @brief إنشاء نطاق للاسم في السطر
     */
    Range create_range(int line, const std::string& text, const std::string& name) {
        size_t pos = text.find(name);
        if (pos == std::string::npos) {
            // إذا لم نجد بالبحث العادي، نحاول مع UTF-8
            // هذا تبسيط - في التنفيذ الحقيقي يجب استخدام مكتبة UTF-8
            pos = 0;
        }
        
        Range range;
        range.start = {line, static_cast<int>(pos)};
        range.end = {line, static_cast<int>(pos + name.length())};
        
        return range;
    }
    
    /**
     * @brief تهرب الأحرف الخاصة للتعبير النمطي
     */
    std::string escape_regex(const std::string& str) {
        static const std::regex special_chars(R"([-[\]{}()*+?.,\^$|#\s])");
        return std::regex_replace(str, special_chars, R"(\$&)");
    }
};

} // namespace lsp
} // namespace sad
