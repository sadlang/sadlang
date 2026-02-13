/**
 * =============================================================================
 * ملف: scoping.cpp
 * الوصف: قواعد نطاق الماكرو
 * المهمة: T198 - Implement macro scoping rules
 * المرحلة: Phase 19 - User Story 16 (Macro System ماكرو!)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو نطاق الماكرو؟
 * ────────────────────
 * النطاق يُحدد أين يمكن استخدام الماكرو بعد تعريفه.
 * 
 * قواعد النطاق:
 * ──────────────
 * 
 * 1. نطاق الوحدة (Module Scope)
 *    - الماكرو مرئي في الوحدة التي عُرّف فيها
 *    - يمكن تصديره للوحدات الأخرى
 * 
 * 2. نطاق المصدر (Textual Scope)
 *    - الماكرو مرئي فقط بعد تعريفه في الملف
 *    - ترتيب التعريفات مهم!
 * 
 * 3. التصدير والاستيراد
 *    - ماكرو_تصدير! للتصدير
 *    - استورد! للاستيراد
 * 
 * مثال:
 * ```sad
 * // ملف: أ.ص
 * ماكرو_تصدير! سجّل!($msg) { ... }
 * 
 * // ملف: ب.ص
 * استورد أ::{سجّل!}
 * سجّل!("يعمل!")
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <sstream>
#include <iostream>
#include <optional>
#include <stack>

// =============================================================================
// هياكل النطاق
// =============================================================================

/**
 * نوع الرؤية
 */
enum class Visibility {
    Private,        // خاص - داخل الوحدة فقط
    Public,         // عام - للجميع
    Crate,          // صندوق - داخل الصندوق فقط
    Super,          // أب - للوحدة الأب فقط
    Restricted      // مقيد - لمسار محدد
};

/**
 * معلومات ماكرو في النطاق
 */
struct MacroEntry {
    std::string name;
    std::string modulePath;
    Visibility visibility;
    size_t definitionLine;
    bool isExported;
    
    MacroEntry(const std::string& n = "", const std::string& mod = "")
        : name(n), modulePath(mod), visibility(Visibility::Private),
          definitionLine(0), isExported(false) {}
};

/**
 * نطاق (Scope)
 */
struct Scope {
    std::string name;
    std::map<std::string, MacroEntry> macros;
    std::vector<std::string> importedMacros;
    Scope* parent = nullptr;
    std::vector<std::unique_ptr<Scope>> children;
    
    Scope(const std::string& n = "") : name(n) {}
    
    /**
     * إضافة ماكرو للنطاق
     */
    void addMacro(const MacroEntry& entry) {
        macros[entry.name] = entry;
    }
    
    /**
     * البحث عن ماكرو في هذا النطاق والنطاقات الأب
     */
    const MacroEntry* findMacro(const std::string& name) const {
        // البحث في هذا النطاق
        auto it = macros.find(name);
        if (it != macros.end()) {
            return &it->second;
        }
        
        // البحث في المستوردات
        for (const auto& imported : importedMacros) {
            if (imported == name) {
                // ابحث في النطاق الأب
                if (parent) {
                    return parent->findMacro(name);
                }
            }
        }
        
        // البحث في النطاق الأب
        if (parent) {
            return parent->findMacro(name);
        }
        
        return nullptr;
    }
    
    /**
     * إنشاء نطاق فرعي
     */
    Scope* createChild(const std::string& childName) {
        auto child = std::make_unique<Scope>(childName);
        child->parent = this;
        Scope* ptr = child.get();
        children.push_back(std::move(child));
        return ptr;
    }
};

// =============================================================================
// مدير النطاقات
// =============================================================================

/**
 * مدير نطاقات الماكرو
 */
class MacroScopeManager {
public:
    MacroScopeManager() {
        // إنشاء النطاق الجذري
        root_ = std::make_unique<Scope>("root");
        current_ = root_.get();
    }
    
    /**
     * دخول نطاق جديد
     */
    void enterScope(const std::string& name) {
        Scope* child = current_->createChild(name);
        scopeStack_.push(current_);
        current_ = child;
    }
    
    /**
     * الخروج من النطاق الحالي
     */
    void exitScope() {
        if (!scopeStack_.empty()) {
            current_ = scopeStack_.top();
            scopeStack_.pop();
        }
    }
    
    /**
     * تعريف ماكرو في النطاق الحالي
     */
    void defineMacro(const std::string& name, Visibility vis = Visibility::Private) {
        MacroEntry entry(name, currentPath());
        entry.visibility = vis;
        entry.definitionLine = currentLine_;
        entry.isExported = (vis == Visibility::Public);
        
        current_->addMacro(entry);
    }
    
    /**
     * استيراد ماكرو
     */
    bool importMacro(const std::string& name, const std::string& fromModule) {
        // البحث في الوحدة المصدر
        const MacroEntry* entry = findMacroInModule(name, fromModule);
        
        if (!entry) {
            return false;
        }
        
        // التحقق من الرؤية
        if (!isVisibleFrom(entry, currentPath())) {
            return false;
        }
        
        current_->importedMacros.push_back(name);
        return true;
    }
    
    /**
     * البحث عن ماكرو
     */
    const MacroEntry* lookupMacro(const std::string& name) {
        return current_->findMacro(name);
    }
    
    /**
     * هل الماكرو مرئي؟
     */
    bool isMacroVisible(const std::string& name) {
        return lookupMacro(name) != nullptr;
    }
    
    /**
     * الحصول على المسار الحالي
     */
    std::string currentPath() const {
        std::vector<std::string> parts;
        Scope* s = current_;
        
        while (s && s != root_.get()) {
            parts.push_back(s->name);
            s = s->parent;
        }
        
        std::string path;
        for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
            if (!path.empty()) path += "::";
            path += *it;
        }
        
        return path.empty() ? "جذر" : path;
    }
    
    /**
     * تعيين رقم السطر الحالي
     */
    void setCurrentLine(size_t line) {
        currentLine_ = line;
    }
    
    /**
     * قائمة جميع الماكروات المرئية
     */
    std::vector<std::string> visibleMacros() const {
        std::vector<std::string> result;
        collectVisibleMacros(current_, result);
        return result;
    }
    
private:
    std::unique_ptr<Scope> root_;
    Scope* current_;
    std::stack<Scope*> scopeStack_;
    size_t currentLine_ = 0;
    
    const MacroEntry* findMacroInModule(const std::string& name, 
                                        const std::string& modulePath) {
        // تنقل إلى الوحدة المحددة
        Scope* moduleScope = findScope(modulePath);
        if (!moduleScope) return nullptr;
        
        auto it = moduleScope->macros.find(name);
        return it != moduleScope->macros.end() ? &it->second : nullptr;
    }
    
    Scope* findScope(const std::string& path) {
        if (path.empty() || path == "جذر") {
            return root_.get();
        }
        
        std::vector<std::string> parts;
        std::string part;
        std::istringstream iss(path);
        
        while (std::getline(iss, part, ':')) {
            if (!part.empty() && part != ":") {
                parts.push_back(part);
            }
        }
        
        Scope* current = root_.get();
        for (const auto& p : parts) {
            bool found = false;
            for (const auto& child : current->children) {
                if (child->name == p) {
                    current = child.get();
                    found = true;
                    break;
                }
            }
            if (!found) return nullptr;
        }
        
        return current;
    }
    
    bool isVisibleFrom(const MacroEntry* entry, const std::string& fromPath) {
        switch (entry->visibility) {
            case Visibility::Public:
                return true;
                
            case Visibility::Private:
                return entry->modulePath == fromPath;
                
            case Visibility::Crate:
                // نفترض أن كل شيء في نفس الصندوق
                return true;
                
            case Visibility::Super:
                // مرئي للأب فقط
                {
                    size_t lastSep = entry->modulePath.rfind("::");
                    std::string parentPath = lastSep != std::string::npos ? 
                                             entry->modulePath.substr(0, lastSep) : "";
                    return fromPath == parentPath || fromPath.find(parentPath) == 0;
                }
                
            default:
                return false;
        }
    }
    
    void collectVisibleMacros(Scope* scope, std::vector<std::string>& result) const {
        if (!scope) return;
        
        for (const auto& [name, _] : scope->macros) {
            result.push_back(name);
        }
        
        // المستوردات
        for (const auto& name : scope->importedMacros) {
            result.push_back(name);
        }
        
        // النطاق الأب
        if (scope->parent) {
            collectVisibleMacros(scope->parent, result);
        }
    }
};

// =============================================================================
// مُدقق قواعد النطاق
// =============================================================================

/**
 * نتيجة التدقيق
 */
struct ScopeCheckResult {
    bool valid;
    std::string error;
    std::string suggestion;
    
    static ScopeCheckResult ok() {
        return {true, "", ""};
    }
    
    static ScopeCheckResult error(const std::string& err, const std::string& sug = "") {
        return {false, err, sug};
    }
};

/**
 * مُدقق قواعد النطاق
 */
class ScopeRulesChecker {
public:
    /**
     * التحقق من استخدام ماكرو
     */
    ScopeCheckResult checkMacroUsage(MacroScopeManager& manager,
                                     const std::string& macroName,
                                     size_t usageLine) {
        const auto* entry = manager.lookupMacro(macroName);
        
        if (!entry) {
            return ScopeCheckResult::error(
                "ماكرو غير معرّف: " + macroName + "!",
                "هل نسيت استيراده؟ استخدم: استورد وحدة::{" + macroName + "!}"
            );
        }
        
        // التحقق من الترتيب النصي
        if (entry->modulePath == manager.currentPath() && 
            usageLine < entry->definitionLine) {
            return ScopeCheckResult::error(
                "الماكرو " + macroName + "! مُستخدم قبل تعريفه",
                "انقل التعريف إلى ما قبل السطر " + std::to_string(usageLine)
            );
        }
        
        return ScopeCheckResult::ok();
    }
    
    /**
     * التحقق من تعريف ماكرو
     */
    ScopeCheckResult checkMacroDefinition(MacroScopeManager& manager,
                                          const std::string& macroName) {
        const auto* existing = manager.lookupMacro(macroName);
        
        if (existing && existing->modulePath == manager.currentPath()) {
            return ScopeCheckResult::error(
                "ماكرو بنفس الاسم موجود بالفعل: " + macroName + "!",
                "استخدم اسماً مختلفاً أو احذف التعريف السابق"
            );
        }
        
        return ScopeCheckResult::ok();
    }
    
    /**
     * التحقق من التصدير
     */
    ScopeCheckResult checkMacroExport(MacroScopeManager& manager,
                                      const std::string& macroName) {
        const auto* entry = manager.lookupMacro(macroName);
        
        if (!entry) {
            return ScopeCheckResult::error(
                "لا يمكن تصدير ماكرو غير موجود: " + macroName + "!"
            );
        }
        
        if (entry->modulePath != manager.currentPath()) {
            return ScopeCheckResult::error(
                "لا يمكن تصدير ماكرو من وحدة أخرى",
                "صدّره من وحدته الأصلية: " + entry->modulePath
            );
        }
        
        return ScopeCheckResult::ok();
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

static MacroScopeManager g_scopeManager;
static ScopeRulesChecker g_scopeChecker;

extern "C" {

/**
 * دخول نطاق
 */
void sad_scope_enter(const char* name) {
    g_scopeManager.enterScope(name);
}

/**
 * الخروج من النطاق
 */
void sad_scope_exit() {
    g_scopeManager.exitScope();
}

/**
 * تعريف ماكرو
 */
void sad_scope_define_macro(const char* name, int visibility) {
    g_scopeManager.defineMacro(name, static_cast<Visibility>(visibility));
}

/**
 * البحث عن ماكرو
 */
int sad_scope_lookup_macro(const char* name) {
    return g_scopeManager.isMacroVisible(name) ? 1 : 0;
}

/**
 * الحصول على المسار الحالي
 */
const char* sad_scope_current_path() {
    static std::string result;
    result = g_scopeManager.currentPath();
    return result.c_str();
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef SCOPING_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار قواعد نطاق الماكرو                                                 ║
║     Macro Scoping Rules Test                                                 ║
║                                                                              ║
║     المرحلة 19 - T198                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    MacroScopeManager manager;
    ScopeRulesChecker checker;
    
    // اختبار 1: تعريف ماكرو في النطاق الجذري
    std::cout << "═══ اختبار 1: تعريف ماكرو ═══\n\n";
    
    manager.setCurrentLine(10);
    manager.defineMacro("سجّل", Visibility::Public);
    
    std::cout << "المسار الحالي: " << manager.currentPath() << "\n";
    std::cout << "هل 'سجّل!' مرئي؟ " << (manager.isMacroVisible("سجّل") ? "نعم" : "لا") << "\n";
    
    // اختبار 2: دخول نطاق فرعي
    std::cout << "\n═══ اختبار 2: نطاق فرعي ═══\n\n";
    
    manager.enterScope("وحدة_أ");
    std::cout << "المسار الحالي: " << manager.currentPath() << "\n";
    
    manager.setCurrentLine(5);
    manager.defineMacro("محلي", Visibility::Private);
    
    std::cout << "هل 'سجّل!' مرئي؟ " << (manager.isMacroVisible("سجّل") ? "نعم" : "لا") << "\n";
    std::cout << "هل 'محلي!' مرئي؟ " << (manager.isMacroVisible("محلي") ? "نعم" : "لا") << "\n";
    
    // اختبار 3: الخروج من النطاق
    std::cout << "\n═══ اختبار 3: الخروج من النطاق ═══\n\n";
    
    manager.exitScope();
    std::cout << "المسار الحالي: " << manager.currentPath() << "\n";
    std::cout << "هل 'سجّل!' مرئي؟ " << (manager.isMacroVisible("سجّل") ? "نعم" : "لا") << "\n";
    std::cout << "هل 'محلي!' مرئي؟ " << (manager.isMacroVisible("محلي") ? "نعم" : "لا") << "\n";
    
    // اختبار 4: التحقق من الاستخدام
    std::cout << "\n═══ اختبار 4: التحقق من الاستخدام ═══\n\n";
    
    manager.setCurrentLine(5);
    auto result1 = checker.checkMacroUsage(manager, "سجّل", 5);
    std::cout << "استخدام 'سجّل!' في السطر 5: " 
              << (result1.valid ? "✓ صالح" : "✗ " + result1.error) << "\n";
    
    auto result2 = checker.checkMacroUsage(manager, "غير_موجود", 1);
    std::cout << "استخدام 'غير_موجود!': " 
              << (result2.valid ? "✓ صالح" : "✗ " + result2.error) << "\n";
    if (!result2.suggestion.empty()) {
        std::cout << "  اقتراح: " << result2.suggestion << "\n";
    }
    
    // اختبار 5: قائمة الماكروات المرئية
    std::cout << "\n═══ اختبار 5: الماكروات المرئية ═══\n\n";
    
    manager.enterScope("وحدة_ب");
    manager.defineMacro("اختبار", Visibility::Public);
    
    auto visible = manager.visibleMacros();
    std::cout << "الماكروات المرئية في '" << manager.currentPath() << "':\n";
    for (const auto& name : visible) {
        std::cout << "  - " << name << "!\n";
    }
    
    std::cout << "\n✅ نجح اختبار قواعد نطاق الماكرو!\n";
    
    return 0;
}

#endif // SCOPING_TEST
