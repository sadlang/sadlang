/**
 * =============================================================================
 * ملف: cross_reference.cpp
 * الوصف: محرك الروابط المتقاطعة للتوثيق
 * المهمة: T231 - Cross-reference linker
 * المرحلة: Phase 23 - User Story 20 (Documentation System ص وثّق)
 * =============================================================================
 * 
 * 🔗 دليل المبتدئ للروابط المتقاطعة
 * ════════════════════════════════════
 * 
 * ما هي الروابط المتقاطعة؟
 * ─────────────────────────
 * عندما تقرأ توثيق دالة وترى "انظر أيضاً: دالة_أخرى()"
 * هذا رابط متقاطع! يربط أجزاء التوثيق معاً.
 * 
 * مثال بسيط:
 * ───────────
 * 
 * /// تكتب نصاً على الشاشة
 * /// انظر: قرأ() للقراءة من المستخدم
 * دالة اكتب(نص: نص)
 *     ...
 * نهاية
 * 
 * /// تقرأ مدخلات المستخدم
 * /// انظر: اكتب() للكتابة على الشاشة
 * دالة قرأ() -> نص
 *     ...
 * نهاية
 * 
 * أنواع الروابط:
 * ───────────────
 * 1. رابط إلى دالة: `جمع()`
 * 2. رابط إلى صنف: `قائمة`
 * 3. رابط إلى وحدة: `رياضيات::جذر`
 * 4. رابط إلى ثابت: `#باي`
 * 
 * كيف يعمل الربط؟
 * ─────────────────
 * 1. مرحلة التجميع: نجمع كل الأسماء في فهرس
 * 2. مرحلة التحليل: نبحث عن إشارات في النصوص
 * 3. مرحلة الربط: نحول الإشارات إلى روابط HTML
 * 4. مرحلة التحقق: نتأكد أن كل رابط يعمل
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <regex>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace sad::docs {

// =============================================================================
// أنواع الروابط
// =============================================================================

enum class ReferenceType {
    Function,   // دالة()
    Class,      // صنف
    Struct,     // هيكل
    Enum,       // تعداد
    Constant,   // ثابت
    Module,     // وحدة::عنصر
    External,   // رابط خارجي
    Anchor,     // #قسم
    Unknown     // غير محدد
};

// =============================================================================
// عنصر مرجعي
// =============================================================================

struct ReferenceTarget {
    std::string name;           // الاسم
    std::string fullPath;       // المسار الكامل (module::class::method)
    std::string url;            // الرابط
    ReferenceType type;         // النوع
    std::string summary;        // ملخص
    std::string module;         // الوحدة الأم
    std::string file;           // الملف المصدر
    int line = 0;               // رقم السطر
    
    /**
     * إنشاء رابط HTML
     */
    std::string toLink(const std::string& displayText = "") const {
        std::string text = displayText.empty() ? name : displayText;
        
        std::ostringstream ss;
        ss << "<a href=\"" << url << "\" class=\"ref-link ref-" 
           << typeToString() << "\" title=\"" << summary << "\">";
        ss << text << "</a>";
        
        return ss.str();
    }
    
    /**
     * تحويل النوع لنص
     */
    std::string typeToString() const {
        switch (type) {
            case ReferenceType::Function: return "function";
            case ReferenceType::Class: return "class";
            case ReferenceType::Struct: return "struct";
            case ReferenceType::Enum: return "enum";
            case ReferenceType::Constant: return "constant";
            case ReferenceType::Module: return "module";
            case ReferenceType::External: return "external";
            case ReferenceType::Anchor: return "anchor";
            default: return "unknown";
        }
    }
};

// =============================================================================
// نتيجة الربط
// =============================================================================

struct LinkResult {
    bool found;
    ReferenceTarget target;
    std::string originalText;
    std::string linkedText;
    std::string errorMessage;
};

// =============================================================================
// رابط معلق (غير محلول)
// =============================================================================

struct BrokenLink {
    std::string text;
    std::string sourceFile;
    int line;
    std::string context;
};

// =============================================================================
// تقرير الروابط
// =============================================================================

struct CrossReferenceReport {
    int totalReferences = 0;
    int resolvedCount = 0;
    int brokenCount = 0;
    std::vector<BrokenLink> brokenLinks;
    std::map<std::string, int> referencesByType;
    
    /**
     * طباعة التقرير
     */
    void print() const {
        std::cout << "═══════════════════════════════════════════════\n";
        std::cout << "   تقرير الروابط المتقاطعة\n";
        std::cout << "═══════════════════════════════════════════════\n\n";
        
        std::cout << "📊 إحصائيات:\n";
        std::cout << "   إجمالي الروابط: " << totalReferences << "\n";
        std::cout << "   روابط صحيحة: " << resolvedCount << " ✅\n";
        std::cout << "   روابط معطلة: " << brokenCount << " ❌\n";
        
        if (!referencesByType.empty()) {
            std::cout << "\n📈 حسب النوع:\n";
            for (const auto& [type, count] : referencesByType) {
                std::cout << "   " << type << ": " << count << "\n";
            }
        }
        
        if (!brokenLinks.empty()) {
            std::cout << "\n⚠️ روابط معطلة:\n";
            for (const auto& broken : brokenLinks) {
                std::cout << "   ❌ " << broken.text << "\n";
                std::cout << "      في: " << broken.sourceFile << ":" << broken.line << "\n";
            }
        }
    }
};

// =============================================================================
// فهرس الروابط
// =============================================================================

class ReferenceIndex {
public:
    /**
     * إضافة هدف للفهرس
     */
    void add(const ReferenceTarget& target) {
        // بالاسم البسيط
        byName_[target.name] = target;
        
        // بالمسار الكامل
        byFullPath_[target.fullPath] = target;
        
        // بالوحدة
        byModule_[target.module].push_back(target);
        
        // بالنوع
        byType_[target.type].push_back(target);
    }
    
    /**
     * البحث عن مرجع
     */
    LinkResult lookup(const std::string& reference) const {
        LinkResult result;
        result.originalText = reference;
        
        // تنظيف المرجع
        std::string clean = cleanReference(reference);
        
        // بحث بالمسار الكامل أولاً
        auto it = byFullPath_.find(clean);
        if (it != byFullPath_.end()) {
            result.found = true;
            result.target = it->second;
            result.linkedText = result.target.toLink(reference);
            return result;
        }
        
        // بحث بالاسم
        it = byName_.find(clean);
        if (it != byName_.end()) {
            result.found = true;
            result.target = it->second;
            result.linkedText = result.target.toLink(reference);
            return result;
        }
        
        // لم يُوجد
        result.found = false;
        result.errorMessage = "لم يتم العثور على: " + reference;
        result.linkedText = "<span class=\"broken-link\">" + reference + "</span>";
        
        return result;
    }
    
    /**
     * الحصول على جميع المراجع
     */
    std::vector<ReferenceTarget> getAll() const {
        std::vector<ReferenceTarget> all;
        for (const auto& [name, target] : byName_) {
            all.push_back(target);
        }
        return all;
    }
    
    /**
     * الحصول حسب النوع
     */
    std::vector<ReferenceTarget> getByType(ReferenceType type) const {
        auto it = byType_.find(type);
        if (it != byType_.end()) {
            return it->second;
        }
        return {};
    }
    
    /**
     * الحصول حسب الوحدة
     */
    std::vector<ReferenceTarget> getByModule(const std::string& module) const {
        auto it = byModule_.find(module);
        if (it != byModule_.end()) {
            return it->second;
        }
        return {};
    }
    
    /**
     * العدد الإجمالي
     */
    size_t size() const {
        return byName_.size();
    }
    
private:
    std::map<std::string, ReferenceTarget> byName_;
    std::map<std::string, ReferenceTarget> byFullPath_;
    std::map<std::string, std::vector<ReferenceTarget>> byModule_;
    std::map<ReferenceType, std::vector<ReferenceTarget>> byType_;
    
    /**
     * تنظيف المرجع
     */
    std::string cleanReference(const std::string& ref) const {
        std::string clean = ref;
        
        // إزالة () للدوال
        if (!clean.empty() && clean.back() == ')') {
            size_t paren = clean.rfind('(');
            if (paren != std::string::npos) {
                clean = clean.substr(0, paren);
            }
        }
        
        // إزالة # للثوابت
        if (!clean.empty() && clean.front() == '#') {
            clean = clean.substr(1);
        }
        
        return clean;
    }
};

// =============================================================================
// محرك الربط
// =============================================================================

class CrossReferenceLinkker {
public:
    CrossReferenceLinkker(const ReferenceIndex& index)
        : index_(index) {}
    
    /**
     * معالجة نص وتحويل المراجع لروابط
     */
    std::string processText(
        const std::string& text,
        const std::string& sourceFile = "",
        int lineNumber = 0
    ) {
        std::string result = text;
        
        // البحث عن مراجع الدوال: `دالة()`
        result = processPattern(result, R"(`([^`]+\(\))`)", sourceFile, lineNumber);
        
        // البحث عن مراجع الأصناف/الهياكل: `اسم`
        result = processPattern(result, R"(`([^`\(\)]+)`)", sourceFile, lineNumber);
        
        // البحث عن مراجع الثوابت: `#ثابت`
        result = processPattern(result, R"(`(#[^`]+)`)", sourceFile, lineNumber);
        
        // البحث عن مراجع الوحدات: `وحدة::عنصر`
        result = processPattern(result, R"(`([^`]+::[^`]+)`)", sourceFile, lineNumber);
        
        // معالجة @انظر
        result = processSeeAlso(result, sourceFile, lineNumber);
        
        return result;
    }
    
    /**
     * التحقق من جميع الروابط في ملف
     */
    std::vector<BrokenLink> validateFile(
        const std::string& content,
        const std::string& fileName
    ) {
        std::vector<BrokenLink> broken;
        
        // نمط المراجع
        std::regex refPattern(R"(`([^`]+)`|@انظر\s+(\S+)|انظر:\s*([^\n]+))");
        
        int lineNumber = 1;
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            std::sregex_iterator it(line.begin(), line.end(), refPattern);
            std::sregex_iterator end;
            
            while (it != end) {
                std::string ref = (*it)[1].str();
                if (ref.empty()) ref = (*it)[2].str();
                if (ref.empty()) ref = (*it)[3].str();
                
                if (!ref.empty()) {
                    auto result = index_.lookup(ref);
                    if (!result.found) {
                        BrokenLink broken_link;
                        broken_link.text = ref;
                        broken_link.sourceFile = fileName;
                        broken_link.line = lineNumber;
                        broken_link.context = line;
                        broken.push_back(broken_link);
                    }
                }
                
                ++it;
            }
            
            lineNumber++;
        }
        
        return broken;
    }
    
    /**
     * الحصول على تقرير
     */
    CrossReferenceReport getReport() const {
        return report_;
    }
    
private:
    const ReferenceIndex& index_;
    CrossReferenceReport report_;
    
    /**
     * معالجة نمط معين
     */
    std::string processPattern(
        const std::string& text,
        const std::string& pattern,
        const std::string& sourceFile,
        int lineNumber
    ) {
        std::string result = text;
        std::regex re(pattern);
        
        std::string::const_iterator searchStart = result.cbegin();
        std::smatch match;
        
        std::ostringstream processed;
        size_t lastEnd = 0;
        
        while (std::regex_search(searchStart, result.cend(), match, re)) {
            size_t matchStart = match.position() + (searchStart - result.cbegin());
            
            // نص قبل المطابقة
            processed << result.substr(lastEnd, matchStart - lastEnd);
            
            // استخراج المرجع
            std::string ref = match[1].str();
            
            // البحث عنه
            auto linkResult = index_.lookup(ref);
            report_.totalReferences++;
            
            if (linkResult.found) {
                processed << linkResult.linkedText;
                report_.resolvedCount++;
                report_.referencesByType[linkResult.target.typeToString()]++;
            } else {
                processed << "<code class=\"broken-ref\">" << ref << "</code>";
                report_.brokenCount++;
                
                BrokenLink broken;
                broken.text = ref;
                broken.sourceFile = sourceFile;
                broken.line = lineNumber;
                report_.brokenLinks.push_back(broken);
            }
            
            lastEnd = matchStart + match.length();
            searchStart = result.cbegin() + lastEnd;
        }
        
        // الباقي
        processed << result.substr(lastEnd);
        
        return processed.str();
    }
    
    /**
     * معالجة @انظر
     */
    std::string processSeeAlso(
        const std::string& text,
        const std::string& sourceFile,
        int lineNumber
    ) {
        std::string result = text;
        std::regex seeRe(R"(@انظر\s+(\S+))");
        
        std::smatch match;
        std::string::const_iterator searchStart = result.cbegin();
        std::ostringstream processed;
        size_t lastEnd = 0;
        
        while (std::regex_search(searchStart, result.cend(), match, seeRe)) {
            size_t matchStart = match.position() + (searchStart - result.cbegin());
            
            processed << result.substr(lastEnd, matchStart - lastEnd);
            
            std::string ref = match[1].str();
            auto linkResult = index_.lookup(ref);
            
            if (linkResult.found) {
                processed << "<span class=\"see-also\">🔗 انظر: " 
                         << linkResult.linkedText << "</span>";
            } else {
                processed << "<span class=\"see-also broken\">🔗 انظر: " 
                         << ref << " (غير موجود)</span>";
            }
            
            lastEnd = matchStart + match.length();
            searchStart = result.cbegin() + lastEnd;
        }
        
        processed << result.substr(lastEnd);
        
        return processed.str();
    }
};

// =============================================================================
// بناء الفهرس من التوثيق
// =============================================================================

class IndexBuilder {
public:
    /**
     * إضافة دالة
     */
    void addFunction(
        const std::string& name,
        const std::string& module,
        const std::string& summary,
        const std::string& file,
        int line
    ) {
        ReferenceTarget target;
        target.name = name;
        target.fullPath = module.empty() ? name : module + "::" + name;
        target.url = "api/" + name + ".html";
        target.type = ReferenceType::Function;
        target.summary = summary;
        target.module = module;
        target.file = file;
        target.line = line;
        
        targets_.push_back(target);
    }
    
    /**
     * إضافة صنف
     */
    void addClass(
        const std::string& name,
        const std::string& module,
        const std::string& summary,
        const std::string& file,
        int line
    ) {
        ReferenceTarget target;
        target.name = name;
        target.fullPath = module.empty() ? name : module + "::" + name;
        target.url = "api/" + name + ".html";
        target.type = ReferenceType::Class;
        target.summary = summary;
        target.module = module;
        target.file = file;
        target.line = line;
        
        targets_.push_back(target);
    }
    
    /**
     * إضافة ثابت
     */
    void addConstant(
        const std::string& name,
        const std::string& module,
        const std::string& summary,
        const std::string& file,
        int line
    ) {
        ReferenceTarget target;
        target.name = name;
        target.fullPath = module.empty() ? name : module + "::" + name;
        target.url = "api/constants.html#" + name;
        target.type = ReferenceType::Constant;
        target.summary = summary;
        target.module = module;
        target.file = file;
        target.line = line;
        
        targets_.push_back(target);
    }
    
    /**
     * بناء الفهرس
     */
    ReferenceIndex build() {
        ReferenceIndex index;
        for (const auto& target : targets_) {
            index.add(target);
        }
        return index;
    }
    
private:
    std::vector<ReferenceTarget> targets_;
};

} // namespace sad::docs

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadRefIndex {
    sad::docs::ReferenceIndex* index;
};

struct SadCrossRef {
    sad::docs::CrossReferenceLinkker* linker;
};

SadRefIndex* sad_ref_index_new() {
    auto* ctx = new SadRefIndex();
    ctx->index = new sad::docs::ReferenceIndex();
    return ctx;
}

void sad_ref_index_add(
    SadRefIndex* idx,
    const char* name,
    const char* url,
    int type
) {
    sad::docs::ReferenceTarget target;
    target.name = name;
    target.fullPath = name;
    target.url = url;
    target.type = static_cast<sad::docs::ReferenceType>(type);
    idx->index->add(target);
}

void sad_ref_index_free(SadRefIndex* idx) {
    if (idx) {
        delete idx->index;
        delete idx;
    }
}

SadCrossRef* sad_cross_ref_new(SadRefIndex* idx) {
    auto* ctx = new SadCrossRef();
    ctx->linker = new sad::docs::CrossReferenceLinkker(*idx->index);
    return ctx;
}

const char* sad_cross_ref_process(SadCrossRef* xref, const char* text) {
    static std::string result;
    result = xref->linker->processText(text);
    return result.c_str();
}

void sad_cross_ref_free(SadCrossRef* xref) {
    if (xref) {
        delete xref->linker;
        delete xref;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef CROSS_REFERENCE_TEST

#include <cassert>

void testBasicLinking() {
    std::cout << "🔗 اختبار الربط الأساسي...\n";
    
    // بناء الفهرس
    sad::docs::IndexBuilder builder;
    builder.addFunction("اكتب", "", "تكتب نصاً على الشاشة", "io.ص", 10);
    builder.addFunction("قرأ", "", "تقرأ مدخلات المستخدم", "io.ص", 25);
    builder.addClass("قائمة", "مجموعات", "قائمة ديناميكية", "list.ص", 1);
    builder.addConstant("باي", "رياضيات", "النسبة التقريبية", "math.ص", 1);
    
    auto index = builder.build();
    
    // اختبار البحث
    auto result = index.lookup("اكتب()");
    assert(result.found);
    assert(result.target.name == "اكتب");
    
    result = index.lookup("قائمة");
    assert(result.found);
    assert(result.target.type == sad::docs::ReferenceType::Class);
    
    result = index.lookup("غير_موجود");
    assert(!result.found);
    
    std::cout << "   ✅ نجح\n";
}

void testTextProcessing() {
    std::cout << "📝 اختبار معالجة النص...\n";
    
    sad::docs::IndexBuilder builder;
    builder.addFunction("طبع", "", "تطبع على الشاشة", "io.ص", 1);
    builder.addClass("نص", "", "نوع النص", "types.ص", 1);
    
    auto index = builder.build();
    sad::docs::CrossReferenceLinkker linker(index);
    
    std::string text = "استخدم `طبع()` لطباعة النص من نوع `نص`.";
    std::string result = linker.processText(text);
    
    // يجب أن يحتوي على روابط
    assert(result.find("href=") != std::string::npos);
    assert(result.find("ref-link") != std::string::npos);
    
    std::cout << "   ✅ نجح\n";
}

void testBrokenLinks() {
    std::cout << "⚠️ اختبار الروابط المعطلة...\n";
    
    sad::docs::ReferenceIndex index;
    sad::docs::CrossReferenceLinkker linker(index);
    
    std::string content = R"(
هذا النص يحتوي على `دالة_غير_موجودة()`.
وأيضاً @انظر صنف_غير_موجود
)";
    
    auto broken = linker.validateFile(content, "test.md");
    assert(broken.size() >= 1);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات الروابط المتقاطعة\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicLinking();
    testTextProcessing();
    testBrokenLinks();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // CROSS_REFERENCE_TEST
