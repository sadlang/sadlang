/**
 * @file wasm_interop.cpp
 * @brief (AR) تطبيق طبقة التفاعل WASM ↔ JavaScript
 * @brief (EN) WASM-JavaScript interop layer implementation
 */

#include "wasm_interop.h"
#include <algorithm>

namespace Sad {
namespace Compiler {
namespace WasmDirect {

WasmInteropManager::WasmInteropManager(WasmEmitter& emitter)
    : emitter_(emitter) {}

// ════════════════════════════════════════════════════════════════════════════════
//  تحويل الأنواع
// ════════════════════════════════════════════════════════════════════════════════

ValType WasmInteropManager::interopToWasm(InteropType type) const {
    switch (type) {
        case InteropType::Void:     return ValType::I32; // placeholder
        case InteropType::Int32:    return ValType::I32;
        case InteropType::Int64:    return ValType::I64;
        case InteropType::Float32:  return ValType::F32;
        case InteropType::Float64:  return ValType::F64;
        case InteropType::String:   return ValType::I32; // مؤشر
        case InteropType::Boolean:  return ValType::I32;
        case InteropType::Array:    return ValType::I32; // مؤشر
        case InteropType::Object:   return ValType::I32; // مؤشر
        case InteropType::Function: return ValType::I32; // فهرس الجدول
        case InteropType::DOMHandle: return ValType::I32;
        case InteropType::Promise:  return ValType::I32; // handle
    }
    return ValType::I32;
}

WasmFuncType WasmInteropManager::buildFuncType(const InteropFunction& func) const {
    WasmFuncType type;
    for (const auto& param : func.params) {
        type.params.push_back(interopToWasm(param.type));
    }
    if (func.returnType != InteropType::Void) {
        type.results.push_back(interopToWasm(func.returnType));
    }
    return type;
}

std::string WasmInteropManager::interopTypeToJS(InteropType type) const {
    switch (type) {
        case InteropType::Void:     return "void";
        case InteropType::Int32:    return "number";
        case InteropType::Int64:    return "bigint";
        case InteropType::Float32:  return "number";
        case InteropType::Float64:  return "number";
        case InteropType::String:   return "string";
        case InteropType::Boolean:  return "boolean";
        case InteropType::Array:    return "Array";
        case InteropType::Object:   return "object";
        case InteropType::Function: return "Function";
        case InteropType::DOMHandle: return "Element";
        case InteropType::Promise:  return "Promise";
    }
    return "any";
}

std::string WasmInteropManager::interopTypeToTS(InteropType type) const {
    switch (type) {
        case InteropType::Void:     return "void";
        case InteropType::Int32:    return "number";
        case InteropType::Int64:    return "bigint";
        case InteropType::Float32:  return "number";
        case InteropType::Float64:  return "number";
        case InteropType::String:   return "string";
        case InteropType::Boolean:  return "boolean";
        case InteropType::Array:    return "any[]";
        case InteropType::Object:   return "Record<string, any>";
        case InteropType::Function: return "(...args: any[]) => any";
        case InteropType::DOMHandle: return "Element | null";
        case InteropType::Promise:  return "Promise<any>";
    }
    return "any";
}

// ════════════════════════════════════════════════════════════════════════════════
//  تسجيل دالة استيراد/تصدير
// ════════════════════════════════════════════════════════════════════════════════

uint32_t WasmInteropManager::registerImport(const InteropFunction& func) {
    auto funcType = buildFuncType(func);
    uint32_t idx = emitter_.addImport(func.module, func.name, funcType);
    
    InteropFunction registered = func;
    registered.wasmIndex = idx;
    registered.isImported = true;
    imports_.push_back(registered);
    return idx;
}

void WasmInteropManager::registerExport(const InteropFunction& func) {
    InteropFunction registered = func;
    registered.isExported = true;
    exports_.push_back(registered);
    emitter_.exportFunction(func.name, func.wasmIndex);
}

// ════════════════════════════════════════════════════════════════════════════════
//  واجهة DOM
// ════════════════════════════════════════════════════════════════════════════════

void WasmInteropManager::registerDOMAPI() {
    WebAPIBinding dom;
    dom.apiName = "DOM";
    dom.arabicName = "واجهة_المستند";

    // اختر_عنصر(selector) → handle
    registerImport({
        "__sad_dom_query", "document.querySelector", "اختر_عنصر",
        "dom", {{  "selector", "محدد", InteropType::String}},
        InteropType::DOMHandle
    });

    // أنشئ_عنصر(tag) → handle
    registerImport({
        "__sad_dom_create", "document.createElement", "أنشئ_عنصر",
        "dom", {{"tag", "وسم", InteropType::String}},
        InteropType::DOMHandle
    });

    // عيّن_نص(handle, text)
    registerImport({
        "__sad_dom_set_text", "setTextContent", "عيّن_النص",
        "dom", {{"handle", "عنصر", InteropType::DOMHandle},
                {"text", "نص", InteropType::String}},
        InteropType::Void
    });

    // عيّن_خاصية(handle, name, value)
    registerImport({
        "__sad_dom_set_attr", "setAttribute", "عيّن_خاصية",
        "dom", {{"handle", "عنصر", InteropType::DOMHandle},
                {"name", "اسم", InteropType::String},
                {"value", "قيمة", InteropType::String}},
        InteropType::Void
    });

    // ألحق(parent, child)
    registerImport({
        "__sad_dom_append", "appendChild", "ألحق",
        "dom", {{"parent", "أب", InteropType::DOMHandle},
                {"child", "ابن", InteropType::DOMHandle}},
        InteropType::Void
    });

    // احذف(handle)
    registerImport({
        "__sad_dom_remove", "removeChild", "احذف_عنصر",
        "dom", {{"handle", "عنصر", InteropType::DOMHandle}},
        InteropType::Void
    });

    // عند_النقر(handle, callback)
    registerImport({
        "__sad_dom_on_click", "onClick", "عند_النقر",
        "dom", {{"handle", "عنصر", InteropType::DOMHandle},
                {"callback", "دالة_رد", InteropType::Function}},
        InteropType::Void
    });

    // عند_الإدخال(handle, callback)
    registerImport({
        "__sad_dom_on_input", "onInput", "عند_الإدخال",
        "dom", {{"handle", "عنصر", InteropType::DOMHandle},
                {"callback", "دالة_رد", InteropType::Function}},
        InteropType::Void
    });

    // عيّن_النمط(handle, property, value)
    registerImport({
        "__sad_dom_set_style", "setStyle", "عيّن_النمط",
        "dom", {{"handle", "عنصر", InteropType::DOMHandle},
                {"property", "خاصية", InteropType::String},
                {"value", "قيمة", InteropType::String}},
        InteropType::Void
    });

    // اقرأ_قيمة(handle) → string_ptr
    registerImport({
        "__sad_dom_get_value", "getValue", "اقرأ_القيمة",
        "dom", {{"handle", "عنصر", InteropType::DOMHandle}},
        InteropType::String
    });

    apis_.push_back(dom);
}

// ════════════════════════════════════════════════════════════════════════════════
//  واجهة Fetch/HTTP
// ════════════════════════════════════════════════════════════════════════════════

void WasmInteropManager::registerFetchAPI() {
    WebAPIBinding fetch;
    fetch.apiName = "Fetch";
    fetch.arabicName = "جلب_شبكة";

    // جلب_نص(url) → promise_handle
    registerImport({
        "__sad_fetch_text", "fetchText", "جلب_نص",
        "fetch", {{"url", "رابط", InteropType::String}},
        InteropType::Promise, true // async
    });

    // جلب_json(url) → promise_handle
    registerImport({
        "__sad_fetch_json", "fetchJSON", "جلب_بيانات",
        "fetch", {{"url", "رابط", InteropType::String}},
        InteropType::Promise, true
    });

    // أرسل(url, method, body) → promise_handle
    registerImport({
        "__sad_fetch_send", "fetchSend", "أرسل_طلب",
        "fetch", {{"url", "رابط", InteropType::String},
                  {"method", "طريقة", InteropType::String},
                  {"body", "جسم", InteropType::String}},
        InteropType::Promise, true
    });

    apis_.push_back(fetch);
}

// ════════════════════════════════════════════════════════════════════════════════
//  واجهة Canvas
// ════════════════════════════════════════════════════════════════════════════════

void WasmInteropManager::registerCanvasAPI() {
    WebAPIBinding canvas;
    canvas.apiName = "Canvas";
    canvas.arabicName = "لوحة_رسم";

    // أنشئ_لوحة(width, height) → handle
    registerImport({
        "__sad_canvas_create", "createCanvas", "أنشئ_لوحة",
        "canvas", {{"width", "عرض", InteropType::Int32},
                   {"height", "ارتفاع", InteropType::Int32}},
        InteropType::DOMHandle
    });

    // ارسم_مستطيل(handle, x, y, w, h)
    registerImport({
        "__sad_canvas_fill_rect", "fillRect", "ارسم_مستطيل",
        "canvas", {{"handle", "لوحة", InteropType::DOMHandle},
                   {"x", "س", InteropType::Int32},
                   {"y", "ص", InteropType::Int32},
                   {"w", "عرض", InteropType::Int32},
                   {"h", "ارتفاع", InteropType::Int32}},
        InteropType::Void
    });

    // عيّن_لون(handle, color_ptr)
    registerImport({
        "__sad_canvas_set_color", "setFillColor", "عيّن_اللون",
        "canvas", {{"handle", "لوحة", InteropType::DOMHandle},
                   {"color", "لون", InteropType::String}},
        InteropType::Void
    });

    // امسح(handle)
    registerImport({
        "__sad_canvas_clear", "clearCanvas", "امسح_اللوحة",
        "canvas", {{"handle", "لوحة", InteropType::DOMHandle}},
        InteropType::Void
    });

    // اكتب_نص(handle, text, x, y)
    registerImport({
        "__sad_canvas_text", "fillText", "اكتب_نص",
        "canvas", {{"handle", "لوحة", InteropType::DOMHandle},
                   {"text", "نص", InteropType::String},
                   {"x", "س", InteropType::Int32},
                   {"y", "ص", InteropType::Int32}},
        InteropType::Void
    });

    // ارسم_دائرة(handle, cx, cy, r)
    registerImport({
        "__sad_canvas_circle", "drawCircle", "ارسم_دائرة",
        "canvas", {{"handle", "لوحة", InteropType::DOMHandle},
                   {"cx", "مركز_س", InteropType::Int32},
                   {"cy", "مركز_ص", InteropType::Int32},
                   {"r", "نصف_قطر", InteropType::Int32}},
        InteropType::Void
    });

    apis_.push_back(canvas);
}

// ════════════════════════════════════════════════════════════════════════════════
//  واجهة Console
// ════════════════════════════════════════════════════════════════════════════════

void WasmInteropManager::registerConsoleAPI() {
    WebAPIBinding console;
    console.apiName = "Console";
    console.arabicName = "وحدة_التحكم";

    registerImport({
        "__sad_console_log", "console.log", "سجّل",
        "console", {{"message", "رسالة", InteropType::String}},
        InteropType::Void
    });

    registerImport({
        "__sad_console_warn", "console.warn", "حذّر",
        "console", {{"message", "رسالة", InteropType::String}},
        InteropType::Void
    });

    registerImport({
        "__sad_console_error", "console.error", "خطأ",
        "console", {{"message", "رسالة", InteropType::String}},
        InteropType::Void
    });

    registerImport({
        "__sad_console_time", "console.time", "ابدأ_مؤقت",
        "console", {{"label", "تسمية", InteropType::String}},
        InteropType::Void
    });

    registerImport({
        "__sad_console_time_end", "console.timeEnd", "أنهِ_مؤقت",
        "console", {{"label", "تسمية", InteropType::String}},
        InteropType::Void
    });

    apis_.push_back(console);
}

// ════════════════════════════════════════════════════════════════════════════════
//  واجهة التخزين المحلي
// ════════════════════════════════════════════════════════════════════════════════

void WasmInteropManager::registerStorageAPI() {
    WebAPIBinding storage;
    storage.apiName = "Storage";
    storage.arabicName = "التخزين_المحلي";

    registerImport({
        "__sad_storage_get", "localStorage.getItem", "اقرأ_تخزين",
        "storage", {{"key", "مفتاح", InteropType::String}},
        InteropType::String
    });

    registerImport({
        "__sad_storage_set", "localStorage.setItem", "اكتب_تخزين",
        "storage", {{"key", "مفتاح", InteropType::String},
                    {"value", "قيمة", InteropType::String}},
        InteropType::Void
    });

    registerImport({
        "__sad_storage_remove", "localStorage.removeItem", "احذف_تخزين",
        "storage", {{"key", "مفتاح", InteropType::String}},
        InteropType::Void
    });

    registerImport({
        "__sad_storage_clear", "localStorage.clear", "امسح_التخزين",
        "storage", {},
        InteropType::Void
    });

    apis_.push_back(storage);
}

// ════════════════════════════════════════════════════════════════════════════════
//  واجهة المؤقتات
// ════════════════════════════════════════════════════════════════════════════════

void WasmInteropManager::registerTimerAPI() {
    WebAPIBinding timer;
    timer.apiName = "Timer";
    timer.arabicName = "المؤقتات";

    registerImport({
        "__sad_set_timeout", "setTimeout", "بعد_مدة",
        "timer", {{"callback", "دالة_رد", InteropType::Function},
                  {"ms", "مللي_ثانية", InteropType::Int32}},
        InteropType::Int32  // timer id
    });

    registerImport({
        "__sad_set_interval", "setInterval", "كل_مدة",
        "timer", {{"callback", "دالة_رد", InteropType::Function},
                  {"ms", "مللي_ثانية", InteropType::Int32}},
        InteropType::Int32
    });

    registerImport({
        "__sad_clear_timeout", "clearTimeout", "ألغِ_مؤقت",
        "timer", {{"id", "معرف", InteropType::Int32}},
        InteropType::Void
    });

    registerImport({
        "__sad_request_animation_frame", "requestAnimationFrame", "إطار_حركة",
        "timer", {{"callback", "دالة_رد", InteropType::Function}},
        InteropType::Int32
    });

    apis_.push_back(timer);
}

// ════════════════════════════════════════════════════════════════════════════════
//  واجهة الأحداث
// ════════════════════════════════════════════════════════════════════════════════

void WasmInteropManager::registerEventAPI() {
    WebAPIBinding events;
    events.apiName = "Events";
    events.arabicName = "الأحداث";

    registerImport({
        "__sad_add_event", "addEventListener", "أضف_حدث",
        "events", {{"handle", "عنصر", InteropType::DOMHandle},
                   {"event", "حدث", InteropType::String},
                   {"callback", "دالة_رد", InteropType::Function}},
        InteropType::Void
    });

    registerImport({
        "__sad_remove_event", "removeEventListener", "احذف_حدث",
        "events", {{"handle", "عنصر", InteropType::DOMHandle},
                   {"event", "حدث", InteropType::String},
                   {"callback", "دالة_رد", InteropType::Function}},
        InteropType::Void
    });

    registerImport({
        "__sad_prevent_default", "preventDefault", "امنع_الافتراضي",
        "events", {{"event_handle", "حدث", InteropType::Int32}},
        InteropType::Void
    });

    apis_.push_back(events);
}

// ════════════════════════════════════════════════════════════════════════════════
//  تسجيل جميع الواجهات
// ════════════════════════════════════════════════════════════════════════════════

void WasmInteropManager::registerAllStandardAPIs() {
    registerDOMAPI();
    registerFetchAPI();
    registerCanvasAPI();
    registerConsoleAPI();
    registerStorageAPI();
    registerTimerAPI();
    registerEventAPI();
}

// ════════════════════════════════════════════════════════════════════════════════
//  توليد ربط JS
// ════════════════════════════════════════════════════════════════════════════════

std::string WasmInteropManager::generateImportJS(const InteropFunction& func) const {
    std::ostringstream js;
    
    // توثيق
    if (!func.docComment.empty()) {
        js << "    // " << func.docComment << "\n";
    }
    js << "    // " << func.arabicName << "\n";
    
    js << "    " << func.name << ": (";
    for (size_t i = 0; i < func.params.size(); ++i) {
        if (i > 0) js << ", ";
        js << func.params[i].name;
    }
    js << ") => {\n";

    // تحويل المعاملات النصية إلى نصوص
    for (const auto& param : func.params) {
        if (param.type == InteropType::String) {
            js << "        const " << param.name << "_str = readString(" 
               << param.name << ");\n";
        }
    }

    js << "        // TODO: implement\n";
    
    if (func.returnType == InteropType::String) {
        js << "        return writeString('');\n";
    } else if (func.returnType != InteropType::Void) {
        js << "        return 0;\n";
    }

    js << "    }";
    return js.str();
}

std::string WasmInteropManager::generateJSBindings() const {
    std::ostringstream js;
    
    js << "/**\n";
    js << " * ربط WASM ↔ JavaScript — تم التوليد تلقائياً\n";
    js << " * Sad Language WASM Interop Bindings\n";
    js << " *\n";
    js << " * APIs:\n";
    for (const auto& api : apis_) {
        js << " *   - " << api.apiName << " (" << api.arabicName << "): " 
           << api.functions.size() << " functions\n";
    }
    js << " */\n\n";

    js << "export function createSadInterop(memory, options = {}) {\n";
    js << "    const encoder = new TextEncoder();\n";
    js << "    const decoder = new TextDecoder('utf-8');\n";
    js << "    let heapBase = options.heapBase || 65536;\n\n";

    js << "    function readString(ptr) {\n";
    js << "        const mem = new Uint8Array(memory.buffer);\n";
    js << "        let end = ptr;\n";
    js << "        while (mem[end] !== 0 && end < mem.length) end++;\n";
    js << "        return decoder.decode(mem.slice(ptr, end));\n";
    js << "    }\n\n";

    js << "    function writeString(str) {\n";
    js << "        const bytes = encoder.encode(str + '\\0');\n";
    js << "        const ptr = heapBase;\n";
    js << "        new Uint8Array(memory.buffer).set(bytes, ptr);\n";
    js << "        heapBase += bytes.length + ((8 - bytes.length % 8) % 8);\n";
    js << "        return ptr;\n";
    js << "    }\n\n";

    // DOM cache
    js << "    const domCache = new Map();\n";
    js << "    let nextHandle = 1;\n\n";
    js << "    function storeElement(el) { if (!el) return 0; const h = nextHandle++; domCache.set(h, el); return h; }\n";
    js << "    function getElement(h) { return domCache.get(h) || null; }\n\n";

    // الدوال المستوردة مجمعة حسب الوحدة
    std::unordered_map<std::string, std::vector<const InteropFunction*>> byModule;
    for (const auto& imp : imports_) {
        byModule[imp.module].push_back(&imp);
    }

    js << "    return {\n";
    for (const auto& [module, funcs] : byModule) {
        js << "        // ═══ " << module << " ═══\n";
        for (size_t i = 0; i < funcs.size(); ++i) {
            js << generateImportJS(*funcs[i]);
            if (i < funcs.size() - 1) js << ",";
            js << "\n";
        }
    }
    js << "    };\n";
    js << "}\n";

    return js.str();
}

// ════════════════════════════════════════════════════════════════════════════════
//  توليد TypeScript Definitions
// ════════════════════════════════════════════════════════════════════════════════

std::string WasmInteropManager::generateTypeDefinitions() const {
    std::ostringstream ts;

    ts << "/**\n";
    ts << " * Sad Language WASM Interop Type Definitions\n";
    ts << " * تعريفات أنواع طبقة التفاعل — تم التوليد تلقائياً\n";
    ts << " */\n\n";

    ts << "declare module 'sad-wasm' {\n\n";

    // واجهة الخيارات
    ts << "    interface SadWasmOptions {\n";
    ts << "        outputElement?: HTMLElement;\n";
    ts << "        onOutput?: (text: string) => void;\n";
    ts << "        heapBase?: number;\n";
    ts << "    }\n\n";

    // الدوال المُصدّرة
    ts << "    interface SadWasmExports {\n";
    ts << "        memory: WebAssembly.Memory;\n";
    ts << "        _start(): void;\n";
    for (const auto& exp : exports_) {
        ts << "        " << exp.name << "(";
        for (size_t i = 0; i < exp.params.size(); ++i) {
            if (i > 0) ts << ", ";
            ts << exp.params[i].name << ": " << interopTypeToTS(exp.params[i].type);
        }
        ts << "): " << interopTypeToTS(exp.returnType) << ";\n";
    }
    ts << "    }\n\n";

    // دالة التحميل
    ts << "    export function loadSadWasm(\n";
    ts << "        wasmPath: string,\n";
    ts << "        options?: SadWasmOptions\n";
    ts << "    ): Promise<WebAssembly.Instance & { exports: SadWasmExports }>;\n\n";

    // واجهات الـ APIs
    for (const auto& api : apis_) {
        ts << "    /** " << api.arabicName << " */\n";
        ts << "    interface " << api.apiName << "API {\n";
        for (const auto& func : api.functions) {
            ts << "        /** " << func.arabicName << " */\n";
            ts << "        " << func.jsName << "(";
            for (size_t i = 0; i < func.params.size(); ++i) {
                if (i > 0) ts << ", ";
                ts << func.params[i].name << ": " << interopTypeToTS(func.params[i].type);
            }
            ts << "): " << interopTypeToTS(func.returnType) << ";\n";
        }
        ts << "    }\n\n";
    }

    ts << "}\n";
    return ts.str();
}

// ════════════════════════════════════════════════════════════════════════════════
//  توليد JSDoc
// ════════════════════════════════════════════════════════════════════════════════

std::string WasmInteropManager::generateJSDoc() const {
    std::ostringstream doc;

    doc << "/**\n";
    doc << " * @module sad-wasm\n";
    doc << " * @description ربط لغة ص مع WebAssembly\n";
    doc << " */\n\n";

    for (const auto& api : apis_) {
        doc << "/**\n";
        doc << " * @namespace " << api.apiName << "\n";
        doc << " * @description " << api.arabicName << "\n";
        doc << " */\n\n";
    }

    for (const auto& imp : imports_) {
        doc << "/**\n";
        doc << " * @function " << imp.name << "\n";
        doc << " * @description " << imp.arabicName << "\n";
        for (const auto& param : imp.params) {
            doc << " * @param {" << interopTypeToJS(param.type) << "} " 
                << param.name << " - " << param.arabicName << "\n";
        }
        if (imp.returnType != InteropType::Void) {
            doc << " * @returns {" << interopTypeToJS(imp.returnType) << "}\n";
        }
        doc << " */\n\n";
    }

    return doc.str();
}

// ════════════════════════════════════════════════════════════════════════════════
//  توليد HTML تفاعلي
// ════════════════════════════════════════════════════════════════════════════════

std::string WasmInteropManager::generateTestHTML(const std::string& wasmPath) const {
    std::ostringstream html;

    html << R"(<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <title>اختبار WASM — لغة ص</title>
    <style>
        body { font-family: 'IBM Plex Arabic', sans-serif; background: #1a1a2e; color: #eee; padding: 2rem; }
        .container { max-width: 900px; margin: 0 auto; }
        h1 { color: #e94560; }
        .api-section { background: #16213e; border-radius: 8px; padding: 1rem; margin: 1rem 0; }
        .api-title { color: #0f3460; font-size: 1.2rem; }
        .func-list { display: grid; grid-template-columns: repeat(auto-fill, minmax(200px, 1fr)); gap: 0.5rem; }
        .func-item { background: #0f3460; padding: 0.5rem; border-radius: 4px; cursor: pointer; }
        .func-item:hover { background: #e94560; }
        #output { background: #0d1117; padding: 1rem; border-radius: 8px; min-height: 150px;
                  font-family: monospace; white-space: pre-wrap; direction: ltr; margin-top: 1rem; }
        .stats { display: flex; gap: 1rem; margin: 1rem 0; }
        .stat { background: #16213e; padding: 0.5rem 1rem; border-radius: 4px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🔗 اختبار تفاعل WASM — لغة ص</h1>
        <div class="stats">
            <div class="stat">📥 استيراد: <strong>)" << imports_.size() << R"(</strong></div>
            <div class="stat">📤 تصدير: <strong>)" << exports_.size() << R"(</strong></div>
            <div class="stat">🔌 APIs: <strong>)" << apis_.size() << R"(</strong></div>
        </div>
)";

    for (const auto& api : apis_) {
        html << "        <div class=\"api-section\">\n";
        html << "            <h3>" << api.arabicName << " (" << api.apiName << ")</h3>\n";
        html << "            <div class=\"func-list\">\n";
        for (const auto& func : api.functions) {
            html << "                <div class=\"func-item\">" << func.arabicName << "</div>\n";
        }
        html << "            </div>\n";
        html << "        </div>\n";
    }

    html << R"(
        <h3>📟 الإخراج:</h3>
        <div id="output"></div>
    </div>

    <script type="module">
        import { loadSadWasm } from './sad_module_runtime.js';
        try {
            const output = document.getElementById('output');
            await loadSadWasm(')" << wasmPath << R"(', {
                outputElement: output
            });
            output.textContent += '\n✅ تم التحميل بنجاح';
        } catch(e) {
            document.getElementById('output').textContent = '❌ ' + e.message;
        }
    </script>
</body>
</html>
)";

    return html.str();
}

std::string WasmInteropManager::generateExportWrapper(const InteropFunction& func) const {
    std::ostringstream js;
    js << "export function " << func.jsName << "(";
    for (size_t i = 0; i < func.params.size(); ++i) {
        if (i > 0) js << ", ";
        js << func.params[i].name;
    }
    js << ") {\n";
    js << "    return wasmInstance.exports." << func.name << "(";
    for (size_t i = 0; i < func.params.size(); ++i) {
        if (i > 0) js << ", ";
        if (func.params[i].type == InteropType::String) {
            js << "writeString(" << func.params[i].name << ")";
        } else {
            js << func.params[i].name;
        }
    }
    js << ");\n";
    js << "}\n";
    return js.str();
}

} // namespace WasmDirect
} // namespace Compiler
} // namespace Sad
