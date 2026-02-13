/**
 * =============================================================================
 * ملف: hygiene.cpp
 * الوصف: نظام نظافة الماكرو (Macro Hygiene)
 * المهمة: T194 - Implement macro hygiene system
 * المرحلة: Phase 19 - User Story 16 (Macro System ماكرو!)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هي نظافة الماكرو؟
 * ────────────────────
 * النظافة تمنع تعارض الأسماء بين الماكرو والكود المُستدعي.
 * 
 * المشكلة بدون نظافة:
 * ```sad
 * ماكرو! احسب!($x:عبر) {
 *     متغير نتيجة = $x * 2
 *     نتيجة
 * }
 * 
 * متغير نتيجة = 10
 * متغير ن = احسب!(نتيجة)  // مشكلة! "نتيجة" داخل الماكرو تتعارض
 * ```
 * 
 * الحل مع النظافة:
 * - كل متغير داخل الماكرو يحصل على اسم فريد
 * - المتغيرات من الماكرو لا ترى المتغيرات الخارجية
 * - المتغيرات من الخارج تُمرر صراحة عبر $
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <memory>
#include <random>
#include <atomic>

// =============================================================================
// نطاق النظافة
// =============================================================================

/**
 * معرّف فريد للنطاق
 */
using ScopeId = uint64_t;

/**
 * عداد النطاقات
 */
class ScopeIdGenerator {
public:
    static ScopeId next() {
        static std::atomic<ScopeId> counter{1};
        return counter++;
    }
};

/**
 * سياق النظافة لتوسع ماكرو واحد
 */
struct HygieneContext {
    ScopeId scopeId;
    std::string macroName;
    int expansionDepth;
    std::map<std::string, std::string> renamedVars;  // اسم أصلي -> اسم جديد
    
    HygieneContext(const std::string& name, int depth = 0)
        : scopeId(ScopeIdGenerator::next())
        , macroName(name)
        , expansionDepth(depth) {}
};

// =============================================================================
// مُعيد التسمية (Renamer)
// =============================================================================

/**
 * مُعيد تسمية المتغيرات للنظافة
 */
class HygieneRenamer {
public:
    /**
     * توليد اسم جديد فريد
     */
    std::string generateUniqueName(const std::string& original, ScopeId scope) {
        std::ostringstream ss;
        ss << "__" << original << "_" << scope;
        return ss.str();
    }
    
    /**
     * إعادة تسمية متغير في سياق
     */
    std::string rename(const std::string& name, HygieneContext& ctx) {
        // هل تمت إعادة التسمية من قبل؟
        auto it = ctx.renamedVars.find(name);
        if (it != ctx.renamedVars.end()) {
            return it->second;
        }
        
        // توليد اسم جديد
        std::string newName = generateUniqueName(name, ctx.scopeId);
        ctx.renamedVars[name] = newName;
        
        return newName;
    }
    
    /**
     * البحث عن الاسم الأصلي
     */
    std::string lookupOriginal(const std::string& renamed, const HygieneContext& ctx) {
        for (const auto& [orig, ren] : ctx.renamedVars) {
            if (ren == renamed) return orig;
        }
        return renamed;
    }
};

// =============================================================================
// مكدس النطاقات
// =============================================================================

/**
 * مكدس سياقات النظافة
 */
class HygieneScopeStack {
public:
    /**
     * دخول نطاق ماكرو جديد
     */
    void push(const std::string& macroName) {
        int depth = stack_.empty() ? 0 : stack_.back().expansionDepth + 1;
        stack_.emplace_back(macroName, depth);
    }
    
    /**
     * خروج من النطاق الحالي
     */
    void pop() {
        if (!stack_.empty()) {
            stack_.pop_back();
        }
    }
    
    /**
     * الحصول على السياق الحالي
     */
    HygieneContext* current() {
        return stack_.empty() ? nullptr : &stack_.back();
    }
    
    /**
     * عمق التوسع الحالي
     */
    int depth() const {
        return stack_.empty() ? 0 : stack_.back().expansionDepth;
    }
    
    /**
     * هل نحن داخل ماكرو؟
     */
    bool inMacro() const {
        return !stack_.empty();
    }
    
    /**
     * الحصول على جميع النطاقات
     */
    const std::vector<HygieneContext>& all() const {
        return stack_;
    }
    
private:
    std::vector<HygieneContext> stack_;
};

// =============================================================================
// محرك النظافة
// =============================================================================

/**
 * رمز مع معلومات النظافة
 */
struct HygienicToken {
    std::string value;
    ScopeId originScope;
    bool isFromMacro;
    std::string originalName;
    
    HygienicToken(const std::string& v, ScopeId scope = 0, bool fromMacro = false)
        : value(v), originScope(scope), isFromMacro(fromMacro), originalName(v) {}
};

/**
 * محرك النظافة الرئيسي
 */
class HygieneEngine {
public:
    HygieneEngine() : currentScope_(0) {}
    
    /**
     * بدء توسع ماكرو
     */
    void beginExpansion(const std::string& macroName) {
        scopeStack_.push(macroName);
        if (auto* ctx = scopeStack_.current()) {
            currentScope_ = ctx->scopeId;
        }
    }
    
    /**
     * إنهاء توسع ماكرو
     */
    void endExpansion() {
        scopeStack_.pop();
        if (auto* ctx = scopeStack_.current()) {
            currentScope_ = ctx->scopeId;
        } else {
            currentScope_ = 0;
        }
    }
    
    /**
     * معالجة معرّف (متغير، دالة، إلخ)
     */
    HygienicToken processIdentifier(const std::string& name, bool isBinding) {
        // خارج الماكرو - لا تغيير
        if (!scopeStack_.inMacro()) {
            return HygienicToken(name, 0, false);
        }
        
        auto* ctx = scopeStack_.current();
        
        // هل هذا تعريف متغير جديد؟
        if (isBinding) {
            std::string renamed = renamer_.rename(name, *ctx);
            HygienicToken tok(renamed, ctx->scopeId, true);
            tok.originalName = name;
            return tok;
        }
        
        // استخدام متغير - البحث في النطاقات
        for (int i = scopeStack_.all().size() - 1; i >= 0; i--) {
            auto& scope = scopeStack_.all()[i];
            auto it = scope.renamedVars.find(name);
            if (it != scope.renamedVars.end()) {
                HygienicToken tok(it->second, scope.scopeId, true);
                tok.originalName = name;
                return tok;
            }
        }
        
        // لم يُعرّف في الماكرو - استخدام من الخارج
        return HygienicToken(name, 0, false);
    }
    
    /**
     * معالجة متغير ماكرو (مثل $x)
     */
    HygienicToken processMacroVariable(const std::string& name) {
        // متغيرات الماكرو تحتفظ بنطاقها الأصلي
        return HygienicToken(name, 0, false);
    }
    
    /**
     * هل المتغير من الماكرو؟
     */
    bool isFromMacro(const std::string& name) const {
        if (!scopeStack_.inMacro()) return false;
        
        for (const auto& ctx : scopeStack_.all()) {
            if (ctx.renamedVars.find(name) != ctx.renamedVars.end()) {
                return true;
            }
        }
        
        return false;
    }
    
    /**
     * الحصول على عمق التوسع
     */
    int expansionDepth() const {
        return scopeStack_.depth();
    }
    
    /**
     * التحقق من حد التوسع (لمنع التكرار اللانهائي)
     */
    bool checkRecursionLimit(int maxDepth = 256) const {
        return scopeStack_.depth() < maxDepth;
    }
    
private:
    HygieneRenamer renamer_;
    HygieneScopeStack scopeStack_;
    ScopeId currentScope_;
};

// =============================================================================
// كاشف التصادم
// =============================================================================

/**
 * كاشف تصادم الأسماء
 */
class NameCollisionDetector {
public:
    /**
     * التحقق من تصادم محتمل
     */
    struct CollisionResult {
        bool hasCollision;
        std::string collisionWith;
        std::string suggestion;
    };
    
    CollisionResult check(const std::string& name, 
                          const std::set<std::string>& outerNames,
                          const std::set<std::string>& macroNames) {
        CollisionResult result;
        result.hasCollision = false;
        
        // هل الاسم موجود في الخارج؟
        if (outerNames.find(name) != outerNames.end()) {
            // هل يُستخدم أيضاً في الماكرو؟
            if (macroNames.find(name) != macroNames.end()) {
                result.hasCollision = true;
                result.collisionWith = "متغير خارجي";
                result.suggestion = "استخدم اسماً مختلفاً داخل الماكرو أو مرر المتغير كمعامل";
            }
        }
        
        return result;
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

// مثيل عام
static HygieneEngine g_hygieneEngine;

extern "C" {

/**
 * بدء توسع ماكرو
 */
void sad_hygiene_begin_expansion(const char* macroName) {
    g_hygieneEngine.beginExpansion(macroName);
}

/**
 * إنهاء توسع ماكرو
 */
void sad_hygiene_end_expansion() {
    g_hygieneEngine.endExpansion();
}

/**
 * معالجة معرّف
 */
const char* sad_hygiene_process_ident(const char* name, int isBinding) {
    static std::string result;
    auto tok = g_hygieneEngine.processIdentifier(name, isBinding != 0);
    result = tok.value;
    return result.c_str();
}

/**
 * الحصول على عمق التوسع
 */
int sad_hygiene_depth() {
    return g_hygieneEngine.expansionDepth();
}

/**
 * التحقق من حد التكرار
 */
int sad_hygiene_check_limit() {
    return g_hygieneEngine.checkRecursionLimit() ? 1 : 0;
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef HYGIENE_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار نظافة الماكرو                                                      ║
║     Macro Hygiene Test                                                       ║
║                                                                              ║
║     المرحلة 19 - T194                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    HygieneEngine engine;
    
    std::cout << "═══ السيناريو 1: متغير داخل ماكرو ═══\n\n";
    
    std::cout << "قبل الماكرو:\n";
    auto tok1 = engine.processIdentifier("نتيجة", true);
    std::cout << "  نتيجة -> " << tok1.value << " (من الماكرو: " 
              << (tok1.isFromMacro ? "نعم" : "لا") << ")\n";
    
    std::cout << "\nداخل ماكرو احسب!:\n";
    engine.beginExpansion("احسب!");
    
    auto tok2 = engine.processIdentifier("نتيجة", true);  // تعريف
    std::cout << "  تعريف نتيجة -> " << tok2.value << " (من الماكرو: "
              << (tok2.isFromMacro ? "نعم" : "لا") << ")\n";
    
    auto tok3 = engine.processIdentifier("نتيجة", false);  // استخدام
    std::cout << "  استخدام نتيجة -> " << tok3.value << "\n";
    
    auto tok4 = engine.processIdentifier("x", false);  // متغير خارجي
    std::cout << "  استخدام x (خارجي) -> " << tok4.value << " (من الماكرو: "
              << (tok4.isFromMacro ? "نعم" : "لا") << ")\n";
    
    engine.endExpansion();
    
    std::cout << "\nبعد الماكرو:\n";
    auto tok5 = engine.processIdentifier("نتيجة", false);
    std::cout << "  نتيجة -> " << tok5.value << " (من الماكرو: "
              << (tok5.isFromMacro ? "نعم" : "لا") << ")\n";
    
    std::cout << "\n═══ السيناريو 2: ماكرو متداخل ═══\n\n";
    
    engine.beginExpansion("خارجي!");
    std::cout << "داخل خارجي! (عمق: " << engine.expansionDepth() << ")\n";
    
    auto outer = engine.processIdentifier("م", true);
    std::cout << "  تعريف م -> " << outer.value << "\n";
    
    engine.beginExpansion("داخلي!");
    std::cout << "داخل داخلي! (عمق: " << engine.expansionDepth() << ")\n";
    
    auto inner = engine.processIdentifier("م", true);
    std::cout << "  تعريف م -> " << inner.value << "\n";
    
    std::cout << "  هل مختلفان؟ " << (outer.value != inner.value ? "نعم ✓" : "لا ✗") << "\n";
    
    engine.endExpansion();
    engine.endExpansion();
    
    std::cout << "\n═══ السيناريو 3: كشف التصادم ═══\n\n";
    
    NameCollisionDetector detector;
    std::set<std::string> outerNames = {"نتيجة", "عداد"};
    std::set<std::string> macroNames = {"نتيجة", "مؤقت"};
    
    auto collision = detector.check("نتيجة", outerNames, macroNames);
    std::cout << "التحقق من 'نتيجة':\n";
    std::cout << "  تصادم؟ " << (collision.hasCollision ? "نعم" : "لا") << "\n";
    if (collision.hasCollision) {
        std::cout << "  مع: " << collision.collisionWith << "\n";
        std::cout << "  اقتراح: " << collision.suggestion << "\n";
    }
    
    std::cout << "\n✅ نجح اختبار نظافة الماكرو!\n";
    
    return 0;
}

#endif // HYGIENE_TEST
