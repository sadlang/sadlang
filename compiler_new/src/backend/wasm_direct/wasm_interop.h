/**
 * @file wasm_interop.h
 * @brief (AR) طبقة التفاعل بين WASM و JavaScript — استيراد وتصدير
 * @brief (EN) WASM-JavaScript interop layer — imports and exports
 *
 * يوفر:
 * - تسجيل الدوال المستوردة من JavaScript
 * - تصدير دوال ص إلى JavaScript
 * - تحويل الأنواع بين ص و JS
 * - ربط DOM و Web APIs
 * - توليد TypeScript/JSDoc type definitions
 */

#pragma once
#include "wasm_emitter.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <sstream>

namespace Sad {
namespace Compiler {
namespace WasmDirect {

// ════════════════════════════════════════════════════════════════════════════════
//  أنواع الربط
// ════════════════════════════════════════════════════════════════════════════════

/**
 * نوع القيمة في طبقة التفاعل
 */
enum class InteropType {
    Void,       // فراغ
    Int32,      // عدد صحيح 32 بت
    Int64,      // عدد صحيح 64 بت
    Float32,    // عشري 32 بت
    Float64,    // عشري 64 بت
    String,     // نص (مؤشر + طول)
    Boolean,    // منطقي
    Array,      // مصفوفة
    Object,     // كائن
    Function,   // مرجع دالة
    DOMHandle,  // مقبض DOM
    Promise     // وعد (غير متزامن)
};

/**
 * وصف معامل
 */
struct InteropParam {
    std::string name;       // اسم المعامل
    std::string arabicName; // الاسم العربي
    InteropType type;       // النوع
    bool optional = false;  // اختياري
    std::string defaultVal; // قيمة افتراضية
};

/**
 * وصف دالة مستوردة/مصدرة
 */
struct InteropFunction {
    std::string name;           // الاسم في WASM
    std::string jsName;         // الاسم في JavaScript
    std::string arabicName;     // الاسم العربي
    std::string module;         // وحدة الاستيراد (للمستوردة)
    std::vector<InteropParam> params;
    InteropType returnType = InteropType::Void;
    bool isAsync = false;       // غير متزامنة
    bool isExported = false;    // مُصدّرة
    bool isImported = false;    // مستوردة
    std::string docComment;     // توثيق
    uint32_t wasmIndex = 0;     // فهرس في WASM
};

/**
 * وصف واجهة Web API
 */
struct WebAPIBinding {
    std::string apiName;        // اسم الـ API (مثال: "DOM", "Fetch")
    std::string arabicName;     // الاسم العربي
    std::vector<InteropFunction> functions;
};

// ════════════════════════════════════════════════════════════════════════════════
//  مدير التفاعل WASM ↔ JS
// ════════════════════════════════════════════════════════════════════════════════

/**
 * مدير تسجيل وتوليد التفاعل بين WASM و JavaScript
 */
class WasmInteropManager {
public:
    explicit WasmInteropManager(WasmEmitter& emitter);

    // ─── تسجيل واجهات Web APIs ───

    /**
     * تسجيل واجهة DOM الكاملة
     */
    void registerDOMAPI();

    /**
     * تسجيل واجهة Fetch/HTTP
     */
    void registerFetchAPI();

    /**
     * تسجيل واجهة Canvas
     */
    void registerCanvasAPI();

    /**
     * تسجيل واجهة Console
     */
    void registerConsoleAPI();

    /**
     * تسجيل واجهة LocalStorage
     */
    void registerStorageAPI();

    /**
     * تسجيل واجهة Timer (setTimeout/setInterval)
     */
    void registerTimerAPI();

    /**
     * تسجيل واجهة Events
     */
    void registerEventAPI();

    /**
     * تسجيل جميع الواجهات القياسية
     */
    void registerAllStandardAPIs();

    // ─── تسجيل يدوي ───

    /**
     * تسجيل دالة استيراد مخصصة
     */
    uint32_t registerImport(const InteropFunction& func);

    /**
     * تسجيل دالة تصدير مخصصة
     */
    void registerExport(const InteropFunction& func);

    // ─── توليد الكود ───

    /**
     * توليد كود JavaScript لوقت التشغيل
     */
    std::string generateJSBindings() const;

    /**
     * توليد TypeScript type definitions (.d.ts)
     */
    std::string generateTypeDefinitions() const;

    /**
     * توليد كود JSDoc للتوثيق
     */
    std::string generateJSDoc() const;

    /**
     * توليد ملف HTML للاختبار مع واجهة تفاعلية
     */
    std::string generateTestHTML(const std::string& wasmPath) const;

    // ─── معلومات ───
    const std::vector<InteropFunction>& getImports() const { return imports_; }
    const std::vector<InteropFunction>& getExports() const { return exports_; }
    const std::vector<WebAPIBinding>& getAPIs() const { return apis_; }

private:
    WasmEmitter& emitter_;
    std::vector<InteropFunction> imports_;
    std::vector<InteropFunction> exports_;
    std::vector<WebAPIBinding> apis_;

    // تحويل أنواع
    ValType interopToWasm(InteropType type) const;
    WasmFuncType buildFuncType(const InteropFunction& func) const;
    std::string interopTypeToJS(InteropType type) const;
    std::string interopTypeToTS(InteropType type) const;

    // مساعدات JS
    std::string generateImportJS(const InteropFunction& func) const;
    std::string generateExportWrapper(const InteropFunction& func) const;
};

} // namespace WasmDirect
} // namespace Compiler
} // namespace Sad
