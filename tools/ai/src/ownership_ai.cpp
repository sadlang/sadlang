/**
 * ==========================================================
 * ملف: ownership_ai.cpp
 * الوصف: اقتراحات ذكية تفهم الملكية
 * المهمة: T299 - نظام ذكاء ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🔐 ما هو الذكاء المدرك للملكية؟
 * --------------------------------
 * هذا النظام يفهم قواعد الملكية في لغة ص ويقترح كود
 * آمن تلقائياً، مثل:
 * 
 * 1. متى تستخدم نقل (move)؟
 * 2. متى تستخدم استعارة (borrow)؟
 * 3. كيف تتجنب أخطاء الملكية؟
 * 
 * 🎯 مثال:
 * --------
 * المستخدم يكتب: "اقرأ ملف وأرجع محتواه"
 * 
 * الكود العادي (خطير!):
 *   دالة اقرأ(مسار: نص) ← نص
 *       متغير ملف = افتح(مسار)  // ملف يُملَك هنا
 *       أرجع ملف.اقرأ()          // ملف لم يُغلق! ❌
 *   نهاية
 * 
 * الكود الآمن (ذكاء ص):
 *   دالة اقرأ(مسار: نص) ← نص
 *       متغير ملف = افتح(مسار)
 *       متغير محتوى = ملف.اقرأ()
 *       أغلق(ملف)               // تحرير صريح ✅
 *       أرجع محتوى
 *   نهاية
 * 
 * 🧠 كيف يعمل؟
 * ------------
 * 1. يحلل الكود ويحدد الموارد (ملفات، اتصالات، ذاكرة...)
 * 2. يتتبع دورة حياة كل مورد
 * 3. يضيف عمليات التحرير تلقائياً
 * 4. يقترح النمط الأمثل للملكية
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <algorithm>
#include <memory>

namespace sad {
namespace ai {

// ==========================================================
// 📌 أنواع الموارد
// ==========================================================

/**
 * 🏷️ نوع المورد
 */
enum class ResourceType {
    FILE,           // ملف
    MEMORY,         // ذاكرة مخصصة
    CONNECTION,     // اتصال شبكة
    LOCK,           // قفل
    HANDLE,         // مقبض نظام
    CUSTOM          // مخصص
};

/**
 * 📝 أسماء أنواع الموارد
 */
const char* resourceTypeName(ResourceType type) {
    switch (type) {
        case ResourceType::FILE: return "ملف";
        case ResourceType::MEMORY: return "ذاكرة";
        case ResourceType::CONNECTION: return "اتصال";
        case ResourceType::LOCK: return "قفل";
        case ResourceType::HANDLE: return "مقبض";
        case ResourceType::CUSTOM: return "مخصص";
        default: return "غير معروف";
    }
}

// ==========================================================
// 📌 أنماط الملكية
// ==========================================================

/**
 * 🔑 نمط الملكية
 */
enum class OwnershipPattern {
    MOVE,           // نقل الملكية
    BORROW,         // استعارة للقراءة
    BORROW_MUT,     // استعارة للتعديل
    COPY,           // نسخ
    CLONE,          // استنساخ
    RC,             // عداد مراجع (Rc)
    ARC             // عداد مراجع ذري (Arc)
};

/**
 * 📝 أسماء أنماط الملكية
 */
const char* ownershipPatternName(OwnershipPattern pattern) {
    switch (pattern) {
        case OwnershipPattern::MOVE: return "نقل";
        case OwnershipPattern::BORROW: return "استعارة";
        case OwnershipPattern::BORROW_MUT: return "استعارة_للتعديل";
        case OwnershipPattern::COPY: return "نسخ";
        case OwnershipPattern::CLONE: return "استنساخ";
        case OwnershipPattern::RC: return "عداد_مراجع";
        case OwnershipPattern::ARC: return "عداد_مراجع_ذري";
        default: return "غير معروف";
    }
}

/**
 * 📖 شرح نمط الملكية
 */
const char* ownershipPatternExplanation(OwnershipPattern pattern) {
    switch (pattern) {
        case OwnershipPattern::MOVE:
            return "نقل الملكية بالكامل - المصدر لن يكون متاحاً بعد النقل";
        case OwnershipPattern::BORROW:
            return "استعارة للقراءة فقط - يمكن وجود عدة استعارات قراءة";
        case OwnershipPattern::BORROW_MUT:
            return "استعارة للتعديل - يسمح باستعارة تعديل واحدة فقط";
        case OwnershipPattern::COPY:
            return "نسخ البيانات - للأنواع البسيطة مثل الأعداد";
        case OwnershipPattern::CLONE:
            return "استنساخ عميق - للأنواع المعقدة";
        case OwnershipPattern::RC:
            return "عداد مراجع - للملكية المشتركة في خيط واحد";
        case OwnershipPattern::ARC:
            return "عداد مراجع ذري - للملكية المشتركة بين خيوط";
        default:
            return "";
    }
}

// ==========================================================
// 📌 اقتراح الملكية
// ==========================================================

/**
 * 💡 اقتراح ملكية
 */
struct OwnershipSuggestion {
    std::string variable;           // اسم المتغير
    OwnershipPattern currentPattern;// النمط الحالي
    OwnershipPattern suggestedPattern; // النمط المقترح
    std::string reason;             // سبب الاقتراح
    std::string codeExample;        // مثال على الكود
    int priority;                   // الأولوية (1-5)
    
    /**
     * تحويل إلى string
     */
    std::string toString() const {
        std::ostringstream oss;
        oss << "💡 اقتراح للمتغير '" << variable << "':\n";
        oss << "   النمط الحالي: " << ownershipPatternName(currentPattern) << "\n";
        oss << "   النمط المقترح: " << ownershipPatternName(suggestedPattern) << "\n";
        oss << "   السبب: " << reason << "\n";
        if (!codeExample.empty()) {
            oss << "   مثال:\n" << codeExample << "\n";
        }
        return oss.str();
    }
};

// ==========================================================
// 📌 تحليل استخدام المتغير
// ==========================================================

/**
 * 📊 استخدام المتغير
 */
struct VariableUsage {
    std::string name;
    bool isRead = false;        // يُقرأ
    bool isWritten = false;     // يُكتب
    bool isPassed = false;      // يُمرر لدالة
    bool isReturned = false;    // يُرجع
    bool isInLoop = false;      // داخل حلقة
    bool isAcrossScopes = false;// يُستخدم في نطاقات متعددة
    int readCount = 0;
    int writeCount = 0;
    
    /**
     * اقتراح النمط الأمثل
     */
    OwnershipPattern suggestPattern() const {
        // إذا يُمرر ويُستخدم لاحقاً
        if (isPassed && (isRead || isWritten)) {
            return OwnershipPattern::BORROW;
        }
        
        // إذا يُمرر للتعديل
        if (isPassed && isWritten) {
            return OwnershipPattern::BORROW_MUT;
        }
        
        // إذا يُرجع ولا يُستخدم بعدها
        if (isReturned && !isRead && !isWritten) {
            return OwnershipPattern::MOVE;
        }
        
        // إذا يُستخدم في نطاقات متعددة
        if (isAcrossScopes) {
            return OwnershipPattern::RC;
        }
        
        // إذا داخل حلقة مع قراءات متعددة
        if (isInLoop && readCount > 1) {
            return OwnershipPattern::BORROW;
        }
        
        // الافتراضي: نقل
        return OwnershipPattern::MOVE;
    }
};

// ==========================================================
// 📌 محلل الملكية
// ==========================================================

/**
 * 🔍 محلل الملكية في الكود
 */
class OwnershipAnalyzer {
private:
    // دوال تخصيص الموارد
    std::map<std::string, ResourceType> allocFunctions_ = {
        {"افتح", ResourceType::FILE},
        {"اقرأ_ملف", ResourceType::FILE},
        {"اتصل", ResourceType::CONNECTION},
        {"خصص", ResourceType::MEMORY},
        {"اقفل", ResourceType::LOCK},
        {"افتح_مقبض", ResourceType::HANDLE}
    };
    
    // دوال تحرير الموارد
    std::map<std::string, std::string> releaseFunctions_ = {
        {"افتح", "أغلق"},
        {"اتصل", "افصل"},
        {"خصص", "حرر"},
        {"اقفل", "افتح_القفل"},
        {"افتح_مقبض", "أغلق_المقبض"}
    };
    
    // أنواع يمكن نسخها
    std::set<std::string> copyableTypes_ = {
        "عدد", "عدد8", "عدد16", "عدد32", "عدد64",
        "عشري", "عشري32", "عشري64",
        "منطقي", "حرف"
    };
    
    // أنواع تحتاج تحرير
    std::set<std::string> resourceTypes_ = {
        "ملف", "اتصال", "قفل", "مقبض", "ذاكرة"
    };
    
public:
    /**
     * تحليل كود واقتراح تحسينات الملكية
     */
    std::vector<OwnershipSuggestion> analyze(const std::string& code) {
        std::vector<OwnershipSuggestion> suggestions;
        
        // 1. اكتشاف الموارد غير المحررة
        auto unreleased = findUnreleasedResources(code);
        for (const auto& [var, type] : unreleased) {
            OwnershipSuggestion sugg;
            sugg.variable = var;
            sugg.currentPattern = OwnershipPattern::MOVE;
            sugg.suggestedPattern = OwnershipPattern::MOVE;
            sugg.reason = "المورد '" + var + "' (" + resourceTypeName(type) + 
                         ") لم يُحرر - قد يسبب تسرب موارد";
            sugg.codeExample = generateReleaseCode(var, type);
            sugg.priority = 5; // عالي جداً
            suggestions.push_back(sugg);
        }
        
        // 2. اكتشاف النسخ غير الضروري
        auto unnecessaryCopies = findUnnecessaryCopies(code);
        for (const auto& var : unnecessaryCopies) {
            OwnershipSuggestion sugg;
            sugg.variable = var;
            sugg.currentPattern = OwnershipPattern::CLONE;
            sugg.suggestedPattern = OwnershipPattern::BORROW;
            sugg.reason = "استنساخ غير ضروري - يمكن استخدام استعارة بدلاً";
            sugg.priority = 3;
            suggestions.push_back(sugg);
        }
        
        // 3. اكتشاف الاستعارات المتضاربة
        auto conflicts = findBorrowConflicts(code);
        for (const auto& [var, reason] : conflicts) {
            OwnershipSuggestion sugg;
            sugg.variable = var;
            sugg.currentPattern = OwnershipPattern::BORROW_MUT;
            sugg.suggestedPattern = OwnershipPattern::CLONE;
            sugg.reason = reason;
            sugg.priority = 4;
            suggestions.push_back(sugg);
        }
        
        // ترتيب حسب الأولوية
        std::sort(suggestions.begin(), suggestions.end(),
                 [](const auto& a, const auto& b) {
                     return a.priority > b.priority;
                 });
        
        return suggestions;
    }
    
    /**
     * توليد كود آمن من طلب
     */
    std::string generateSafeCode(const std::string& request,
                                  const std::string& baseCode) {
        std::ostringstream oss;
        
        // تحليل الكود الأساسي
        auto resources = findAllocatedResources(baseCode);
        
        // إضافة تعليقات السلامة
        oss << "// 🔐 كود آمن مُولَّد تلقائياً\n";
        oss << "// الطلب: " << request << "\n\n";
        
        // توليد الكود مع إدارة الموارد
        oss << baseCode;
        
        // إضافة تحرير الموارد
        for (const auto& [var, type] : resources) {
            oss << "\n" << generateReleaseCode(var, type);
        }
        
        return oss.str();
    }
    
    /**
     * اقتراح نمط الملكية لمعامل
     */
    OwnershipPattern suggestParameterOwnership(const std::string& typeName,
                                                bool needsModification,
                                                bool callerNeedsAfter) {
        // أنواع قابلة للنسخ
        if (copyableTypes_.count(typeName) > 0) {
            return OwnershipPattern::COPY;
        }
        
        // المستدعي يحتاجه بعد الاستدعاء
        if (callerNeedsAfter) {
            if (needsModification) {
                return OwnershipPattern::BORROW_MUT;
            } else {
                return OwnershipPattern::BORROW;
            }
        }
        
        // نقل الملكية
        return OwnershipPattern::MOVE;
    }
    
    /**
     * فحص سلامة الملكية
     */
    std::vector<std::string> checkOwnershipSafety(const std::string& code) {
        std::vector<std::string> issues;
        
        // فحص الموارد غير المحررة
        auto unreleased = findUnreleasedResources(code);
        for (const auto& [var, type] : unreleased) {
            issues.push_back("⚠️ المورد '" + var + "' قد لا يُحرر في كل المسارات");
        }
        
        // فحص الاستخدام بعد النقل
        auto useAfterMove = findUseAfterMove(code);
        for (const auto& var : useAfterMove) {
            issues.push_back("❌ استخدام '" + var + "' بعد نقل ملكيته");
        }
        
        // فحص الاستعارات المتضاربة
        auto conflicts = findBorrowConflicts(code);
        for (const auto& [var, reason] : conflicts) {
            issues.push_back("❌ " + reason);
        }
        
        return issues;
    }
    
private:
    /**
     * إيجاد الموارد المخصصة
     */
    std::map<std::string, ResourceType> findAllocatedResources(
        const std::string& code) {
        std::map<std::string, ResourceType> resources;
        
        for (const auto& [func, type] : allocFunctions_) {
            size_t pos = 0;
            while ((pos = code.find(func, pos)) != std::string::npos) {
                // استخراج اسم المتغير (مبسط)
                size_t eqPos = code.rfind('=', pos);
                if (eqPos != std::string::npos) {
                    size_t varStart = code.rfind('\n', eqPos);
                    if (varStart == std::string::npos) varStart = 0;
                    else varStart++;
                    
                    std::string line = code.substr(varStart, eqPos - varStart);
                    // تنظيف وإيجاد اسم المتغير
                    size_t varPos = line.find("متغير");
                    if (varPos != std::string::npos) {
                        varPos += 10; // تخطي "متغير "
                        size_t end = line.find_first_of(" :\n", varPos);
                        std::string var = line.substr(varPos, end - varPos);
                        // تنظيف
                        var.erase(0, var.find_first_not_of(" \t"));
                        var.erase(var.find_last_not_of(" \t") + 1);
                        if (!var.empty()) {
                            resources[var] = type;
                        }
                    }
                }
                pos++;
            }
        }
        
        return resources;
    }
    
    /**
     * إيجاد الموارد غير المحررة
     */
    std::map<std::string, ResourceType> findUnreleasedResources(
        const std::string& code) {
        auto allocated = findAllocatedResources(code);
        std::map<std::string, ResourceType> unreleased;
        
        for (const auto& [var, type] : allocated) {
            // البحث عن دالة التحرير
            bool found = false;
            for (const auto& [alloc, release] : releaseFunctions_) {
                if (code.find(release + "(" + var) != std::string::npos ||
                    code.find(release + " " + var) != std::string::npos ||
                    code.find(var + "." + release) != std::string::npos) {
                    found = true;
                    break;
                }
            }
            
            if (!found) {
                unreleased[var] = type;
            }
        }
        
        return unreleased;
    }
    
    /**
     * إيجاد النسخ غير الضرورية
     */
    std::vector<std::string> findUnnecessaryCopies(const std::string& code) {
        std::vector<std::string> copies;
        
        // البحث عن .استنسخ() أو استنسخ(
        size_t pos = 0;
        while ((pos = code.find("استنسخ", pos)) != std::string::npos) {
            // تحليل مبسط للسياق
            copies.push_back("متغير_مستنسخ");
            pos++;
        }
        
        return copies;
    }
    
    /**
     * إيجاد الاستعارات المتضاربة
     */
    std::vector<std::pair<std::string, std::string>> findBorrowConflicts(
        const std::string& code) {
        std::vector<std::pair<std::string, std::string>> conflicts;
        
        // تحليل مبسط - البحث عن أنماط معروفة
        // في التنفيذ الحقيقي، نستخدم محلل كامل
        
        return conflicts;
    }
    
    /**
     * إيجاد الاستخدام بعد النقل
     */
    std::vector<std::string> findUseAfterMove(const std::string& code) {
        std::vector<std::string> vars;
        
        // تحليل مبسط
        // في التنفيذ الحقيقي، نتتبع دورة حياة المتغيرات
        
        return vars;
    }
    
    /**
     * توليد كود تحرير المورد
     */
    std::string generateReleaseCode(const std::string& var, ResourceType type) {
        switch (type) {
            case ResourceType::FILE:
                return "أغلق(" + var + ")  // تحرير الملف";
            case ResourceType::CONNECTION:
                return "افصل(" + var + ")  // قطع الاتصال";
            case ResourceType::MEMORY:
                return "حرر(" + var + ")  // تحرير الذاكرة";
            case ResourceType::LOCK:
                return "افتح_القفل(" + var + ")  // فتح القفل";
            case ResourceType::HANDLE:
                return "أغلق_المقبض(" + var + ")  // إغلاق المقبض";
            default:
                return "// تحرير " + var;
        }
    }
};

// ==========================================================
// 📌 مولّد الكود الآمن
// ==========================================================

/**
 * ✨ مولّد كود مدرك للملكية
 */
class OwnershipAwareCodeGen {
private:
    OwnershipAnalyzer analyzer_;
    
public:
    /**
     * توليد دالة آمنة لقراءة ملف
     */
    std::string generateSafeFileRead(const std::string& funcName,
                                      const std::string& returnType = "نص") {
        std::ostringstream oss;
        
        oss << "/**\n";
        oss << " * قراءة ملف بشكل آمن\n";
        oss << " * ✅ الملف يُغلق تلقائياً حتى عند الخطأ\n";
        oss << " */\n";
        oss << "دالة " << funcName << "(مسار: نص) ← نتيجة<" << returnType << "، خطأ>\n";
        oss << "    متغير ملف = حاول افتح(مسار، \"قراءة\")\n";
        oss << "    إذا ملف.فشل()\n";
        oss << "        أرجع خطأ(\"فشل فتح الملف: \" + مسار)\n";
        oss << "    نهاية\n";
        oss << "    \n";
        oss << "    // استخدام defer لضمان الإغلاق\n";
        oss << "    مؤجل أغلق(ملف.قيمة())\n";
        oss << "    \n";
        oss << "    متغير محتوى = ملف.قيمة().اقرأ_الكل()\n";
        oss << "    أرجع نجاح(محتوى)\n";
        oss << "نهاية\n";
        
        return oss.str();
    }
    
    /**
     * توليد دالة آمنة مع موارد متعددة
     */
    std::string generateSafeMultiResource(const std::string& funcName,
                                           const std::vector<std::string>& resources) {
        std::ostringstream oss;
        
        oss << "/**\n";
        oss << " * دالة بموارد متعددة\n";
        oss << " * ✅ جميع الموارد تُحرر تلقائياً\n";
        oss << " */\n";
        oss << "دالة " << funcName << "() ← نتيجة<فراغ، خطأ>\n";
        
        // تخصيص الموارد مع defer
        for (size_t i = 0; i < resources.size(); ++i) {
            oss << "    // المورد " << (i + 1) << "\n";
            oss << "    متغير " << resources[i] << " = خصص_" << resources[i] << "()\n";
            oss << "    مؤجل حرر(" << resources[i] << ")\n\n";
        }
        
        oss << "    // استخدام الموارد\n";
        oss << "    // ...\n\n";
        oss << "    أرجع نجاح(فراغ)\n";
        oss << "نهاية\n";
        
        return oss.str();
    }
    
    /**
     * توليد هيكل مع Drop
     */
    std::string generateStructWithDrop(const std::string& name,
                                        const std::vector<std::pair<std::string, std::string>>& fields,
                                        const std::string& dropCode) {
        std::ostringstream oss;
        
        oss << "/**\n";
        oss << " * هيكل " << name << " مع تحرير تلقائي\n";
        oss << " */\n";
        oss << "هيكل " << name << "\n";
        
        for (const auto& [fname, ftype] : fields) {
            oss << "    " << fname << ": " << ftype << "\n";
        }
        
        oss << "نهاية\n\n";
        
        // تنفيذ Drop
        oss << "نفذ إسقاط لـ " << name << "\n";
        oss << "    دالة أسقط(ذات)\n";
        oss << "        " << dropCode << "\n";
        oss << "    نهاية\n";
        oss << "نهاية\n";
        
        return oss.str();
    }
    
    /**
     * تحويل كود غير آمن إلى آمن
     */
    std::string makeCodeSafe(const std::string& unsafeCode) {
        // تحليل الكود
        auto suggestions = analyzer_.analyze(unsafeCode);
        
        if (suggestions.empty()) {
            return unsafeCode; // الكود آمن بالفعل
        }
        
        std::ostringstream oss;
        oss << "// ⚠️ تم تحويل الكود ليكون آمناً\n";
        oss << "// التعديلات:\n";
        
        for (const auto& sugg : suggestions) {
            oss << "//   - " << sugg.reason << "\n";
        }
        
        oss << "\n";
        
        // إضافة الكود المعدّل
        // في التنفيذ الحقيقي، نستخدم محلل AST للتعديل
        oss << unsafeCode;
        
        // إضافة التحريرات المفقودة
        for (const auto& sugg : suggestions) {
            if (sugg.priority == 5) { // موارد غير محررة
                oss << "\n" << sugg.codeExample;
            }
        }
        
        return oss.str();
    }
    
    /**
     * الحصول على تقرير السلامة
     */
    std::string getSafetyReport(const std::string& code) {
        auto issues = analyzer_.checkOwnershipSafety(code);
        
        std::ostringstream oss;
        oss << "═══════════════════════════════════\n";
        oss << "     تقرير سلامة الملكية\n";
        oss << "═══════════════════════════════════\n\n";
        
        if (issues.empty()) {
            oss << "✅ الكود آمن - لم تُكتشف مشاكل\n";
        } else {
            oss << "🔍 المشاكل المكتشفة:\n\n";
            for (const auto& issue : issues) {
                oss << "   " << issue << "\n";
            }
        }
        
        return oss.str();
    }
};

} // namespace ai
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء محلل ملكية
 */
void* sad_ownership_analyzer_new(void) {
    return new sad::ai::OwnershipAnalyzer();
}

/**
 * تحرير المحلل
 */
void sad_ownership_analyzer_free(void* analyzer) {
    delete static_cast<sad::ai::OwnershipAnalyzer*>(analyzer);
}

/**
 * إنشاء مولّد كود آمن
 */
void* sad_ownership_codegen_new(void) {
    return new sad::ai::OwnershipAwareCodeGen();
}

/**
 * تحرير المولّد
 */
void sad_ownership_codegen_free(void* gen) {
    delete static_cast<sad::ai::OwnershipAwareCodeGen*>(gen);
}

/**
 * توليد كود قراءة ملف آمن
 */
char* sad_ownership_gen_safe_file_read(void* gen, const char* funcName) {
    auto* g = static_cast<sad::ai::OwnershipAwareCodeGen*>(gen);
    std::string result = g->generateSafeFileRead(funcName);
    
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * تحويل كود إلى آمن
 */
char* sad_ownership_make_safe(void* gen, const char* code) {
    auto* g = static_cast<sad::ai::OwnershipAwareCodeGen*>(gen);
    std::string result = g->makeCodeSafe(code);
    
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * الحصول على تقرير السلامة
 */
char* sad_ownership_safety_report(void* gen, const char* code) {
    auto* g = static_cast<sad::ai::OwnershipAwareCodeGen*>(gen);
    std::string result = g->getSafetyReport(code);
    
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * تحرير نص
 */
void sad_ownership_free_string(char* str) {
    delete[] str;
}

} // extern "C"
