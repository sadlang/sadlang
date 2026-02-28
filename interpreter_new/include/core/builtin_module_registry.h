/**
 * @file builtin_module_registry.h
 * @brief (AR) سجل الوحدات المُضمّنة — تسجيل كسول للدوال المُضمّنة حسب الفئة
 * @brief (EN) Builtin Module Registry — lazy category-based registration of builtins
 *
 * @details
 * (AR) بدلاً من تسجيل جميع الدوال المُضمّنة (~2500) عند بدء التشغيل،
 *      يتم تسجيل الوحدات الأساسية فقط (اطبع، نوع، طول، ...).
 *      عندما يكتب المطور `استورد رسوميات` أو `من نظام_ملفات استورد *`
 *      تُسجَّل الدوال ذات الصلة فقط.
 *
 * (EN) Instead of eagerly registering all ~2500 builtins at startup,
 *      only core builtins (print, type, length, ...) are registered.
 *      When the developer writes `استورد رسوميات` or `من نظام_ملفات استورد *`,
 *      only the relevant builtins are registered on demand.
 *
 * الوحدات المتاحة:
 * ═══════════════════
 * | الوحدة               | الملفات        | الوصف                          |
 * |----------------------|---------------|--------------------------------|
 * | (أساسي — تلقائي)     | Part 1-6, 10  | طباعة، أنواع، رياضيات، مصفوفات   |
 * | نظام_ملفات           | Part 7-8      | عمليات الملفات والمجلدات          |
 * | رسوميات              | Part 9, 22    | framebuffer، أشكال، ألوان        |
 * | واجهات               | Part 11-18    | عناصر UI، واجهات Flutter         |
 * | نظام_منخفض           | Part 19-21    | CPU، APIC، UEFI، ACPI           |
 * | usb                  | Part 23       | USB/HID، لوحة مفاتيح، فأرة      |
 *
 * @author فريق تطوير لغة ص / Sad Language Development Team
 * @date 2026
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <algorithm>

namespace Sad {
namespace Interpreter {

// Forward declaration
class Interpreter;

/**
 * @brief (AR) معلومات وحدة مُضمّنة
 * @brief (EN) Built-in module information
 */
struct BuiltinModuleInfo {
    /// (AR) اسم الوحدة العربي / (EN) Arabic module name
    std::string name;

    /// (AR) الأسماء البديلة (عربي + إنجليزي)
    /// (EN) Alternate names (Arabic + English)
    std::vector<std::string> aliases;

    /// (AR) وصف مختصر / (EN) Short description
    std::string description;

    /// (AR) دالة التسجيل — تُنادى عند الاستيراد الأول
    /// (EN) Registration callback — called on first import
    std::function<void(Interpreter&)> registrar;

    /// (AR) أسماء الدوال المُصدَّرة — تُملأ بعد التسجيل
    /// (EN) Exported function names — populated after registration
    std::vector<std::string> exportedFunctions;
};

/**
 * @class BuiltinModuleRegistry
 * @brief (AR) سجل مركزي لجميع الوحدات المُضمّنة مع دعم التحميل الكسول
 * @brief (EN) Central registry for all builtin modules with lazy loading support
 */
class BuiltinModuleRegistry {
public:
    /// @brief (AR) الحصول على المثيل الوحيد / (EN) Get singleton instance
    static BuiltinModuleRegistry& getInstance() {
        static BuiltinModuleRegistry instance;
        return instance;
    }

    /**
     * @brief (AR) تسجيل وحدة مُضمّنة جديدة
     * @brief (EN) Register a new builtin module
     * @param info معلومات الوحدة / Module info
     */
    void registerModule(const BuiltinModuleInfo& info) {
        modules_[info.name] = info;
        for (const auto& alias : info.aliases) {
            moduleAliases_[alias] = info.name;
        }
    }

    /**
     * @brief (AR) هل هذا اسم وحدة مُضمّنة؟
     * @brief (EN) Is this a builtin module name?
     */
    bool isBuiltinModule(const std::string& name) const {
        return modules_.count(name) > 0 || moduleAliases_.count(name) > 0;
    }

    /**
     * @brief (AR) هل تم تحميل هذه الوحدة؟
     * @brief (EN) Has this module been loaded?
     */
    bool isModuleLoaded(const std::string& name) const {
        std::string canonical = resolveAlias(name);
        return loadedModules_.count(canonical) > 0;
    }

    /**
     * @brief (AR) تعيين مرجع المفسر (يُنادى مرة واحدة عند البدء)
     * @brief (EN) Set interpreter reference (called once at startup)
     */
    void setInterpreter(Interpreter* interp) {
        interpreter_ = interp;
    }

    /**
     * @brief (AR) تحميل وحدة مُضمّنة (تسجيل الدوال عند الطلب)
     * @brief (EN) Load a builtin module (register functions on demand)
     * @param name اسم الوحدة / Module name
     * @return true إذا نجح التحميل / if loaded successfully
     *
     * @note يُنفَّذ في builtin_registry.cpp لأنه يحتاج FunctionManager
     */
    bool loadModule(const std::string& name);

    /**
     * @brief (AR) الحصول على قائمة الدوال المُصدَّرة من وحدة
     * @brief (EN) Get list of exported functions from a module
     */
    const std::vector<std::string>& getExportedFunctions(const std::string& name) const {
        std::string canonical = resolveAlias(name);
        auto it = modules_.find(canonical);
        if (it != modules_.end()) return it->second.exportedFunctions;
        static const std::vector<std::string> empty;
        return empty;
    }

    /**
     * @brief (AR) الحصول على معلومات وحدة
     * @brief (EN) Get module info
     */
    const BuiltinModuleInfo* getModuleInfo(const std::string& name) const {
        std::string canonical = resolveAlias(name);
        auto it = modules_.find(canonical);
        return (it != modules_.end()) ? &it->second : nullptr;
    }

    /**
     * @brief (AR) الحصول على جميع أسماء الوحدات المسجلة
     * @brief (EN) Get all registered module names
     */
    std::vector<std::string> getModuleNames() const {
        std::vector<std::string> names;
        names.reserve(modules_.size());
        for (const auto& [name, _] : modules_) {
            names.push_back(name);
        }
        return names;
    }

    /**
     * @brief (AR) الحصول على جميع الوحدات المُحمَّلة
     * @brief (EN) Get all loaded module names
     */
    const std::unordered_set<std::string>& getLoadedModules() const {
        return loadedModules_;
    }

    /**
     * @brief (AR) إعادة تعيين حالة التحميل (للاختبارات)
     * @brief (EN) Reset loading state (for tests)
     */
    void reset() {
        loadedModules_.clear();
    }

private:
    BuiltinModuleRegistry() = default;

    std::string resolveAlias(const std::string& name) const {
        auto it = moduleAliases_.find(name);
        return (it != moduleAliases_.end()) ? it->second : name;
    }

    /// (AR) خريطة الوحدات: اسم → معلومات
    std::unordered_map<std::string, BuiltinModuleInfo> modules_;

    /// (AR) خريطة الأسماء البديلة → الاسم القانوني
    std::unordered_map<std::string, std::string> moduleAliases_;

    /// (AR) الوحدات المُحمَّلة
    std::unordered_set<std::string> loadedModules_;

    /// (AR) مرجع المفسر / (EN) Interpreter reference
    Interpreter* interpreter_ = nullptr;
};

} // namespace Interpreter
} // namespace Sad
