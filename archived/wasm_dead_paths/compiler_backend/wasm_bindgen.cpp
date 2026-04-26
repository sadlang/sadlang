/**
 * @file wasm_bindgen.cpp
 * @brief wasm-bindgen integration for Sad compiler
 * 
 * تكامل wasm-bindgen لربط كود ص مع JavaScript
 */

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>

namespace sad {
namespace backend {

/**
 * توليد ربط WASM-JavaScript
 */
class WasmBindGen {
public:
    /**
     * نوع الربط
     */
    enum class BindingType {
        Function,       // دالة
        Struct,         // هيكل/صنف
        Enum,           // تعداد
        Constant,       // ثابت
        Global          // متغير عام
    };
    
    /**
     * معلومات دالة مُصدَّرة
     */
    struct ExportedFunction {
        std::string wasmName;       // الاسم في WASM
        std::string jsName;         // الاسم في JS
        std::string arabicName;     // الاسم العربي
        std::string returnType;     // نوع الإرجاع
        std::vector<std::pair<std::string, std::string>> params;  // (اسم, نوع)
        bool isAsync = false;       // دالة غير متزامنة
        std::string docComment;     // تعليق التوثيق
    };
    
    /**
     * معلومات صنف/هيكل مُصدَّر
     */
    struct ExportedClass {
        std::string name;
        std::string arabicName;
        std::vector<std::pair<std::string, std::string>> fields;
        std::vector<ExportedFunction> methods;
        std::string docComment;
    };
    
    /**
     * معلومات استيراد من JS
     */
    struct JSImport {
        std::string jsModule;       // وحدة JS
        std::string jsName;         // الاسم في JS
        std::string wasmName;       // الاسم في WASM
        std::string signature;      // توقيع الدالة
    };
    
private:
    std::vector<ExportedFunction> exports_;
    std::vector<ExportedClass> classes_;
    std::vector<JSImport> imports_;
    std::string moduleName_;
    
public:
    explicit WasmBindGen(const std::string& moduleName)
        : moduleName_(moduleName) {}
    
    /**
     * إضافة دالة مُصدَّرة
     */
    void addExport(const ExportedFunction& func) {
        exports_.push_back(func);
    }
    
    /**
     * إضافة صنف مُصدَّر
     */
    void addClass(const ExportedClass& cls) {
        classes_.push_back(cls);
    }
    
    /**
     * إضافة استيراد JS
     */
    void addImport(const JSImport& import) {
        imports_.push_back(import);
    }
    
    /**
     * توليد ملف TypeScript definitions (.d.ts)
     */
    std::string generateTypeScript() {
        std::ostringstream ts;
        
        ts << "// Auto-generated TypeScript definitions for Sad module: " << moduleName_ << "\n";
        ts << "// تعريفات TypeScript مُولَّدة تلقائياً لوحدة ص\n\n";
        
        ts << "declare module '" << moduleName_ << "' {\n";
        
        // تصدير الدوال
        for (const auto& func : exports_) {
            if (!func.docComment.empty()) {
                ts << "  /** " << func.docComment << " */\n";
            }
            
            ts << "  export function " << func.jsName << "(";
            
            for (size_t i = 0; i < func.params.size(); ++i) {
                if (i > 0) ts << ", ";
                ts << func.params[i].first << ": " 
                   << sadTypeToTS(func.params[i].second);
            }
            
            ts << "): ";
            if (func.isAsync) ts << "Promise<";
            ts << sadTypeToTS(func.returnType);
            if (func.isAsync) ts << ">";
            ts << ";\n";
            
            // أضف الاسم العربي كـ alias
            if (!func.arabicName.empty() && func.arabicName != func.jsName) {
                ts << "  export { " << func.jsName << " as " << func.arabicName << " };\n";
            }
        }
        
        ts << "\n";
        
        // تصدير الأصناف
        for (const auto& cls : classes_) {
            if (!cls.docComment.empty()) {
                ts << "  /** " << cls.docComment << " */\n";
            }
            
            ts << "  export class " << cls.name << " {\n";
            
            // الحقول
            for (const auto& field : cls.fields) {
                ts << "    " << field.first << ": " << sadTypeToTS(field.second) << ";\n";
            }
            
            // الدوال
            for (const auto& method : cls.methods) {
                ts << "    " << method.jsName << "(";
                for (size_t i = 0; i < method.params.size(); ++i) {
                    if (i > 0) ts << ", ";
                    ts << method.params[i].first << ": " 
                       << sadTypeToTS(method.params[i].second);
                }
                ts << "): " << sadTypeToTS(method.returnType) << ";\n";
            }
            
            ts << "  }\n";
            
            // الاسم العربي
            if (!cls.arabicName.empty() && cls.arabicName != cls.name) {
                ts << "  export { " << cls.name << " as " << cls.arabicName << " };\n";
            }
        }
        
        ts << "}\n";
        
        return ts.str();
    }
    
    /**
     * توليد كود JavaScript glue
     */
    std::string generateJSGlue() {
        std::ostringstream js;
        
        js << "// Auto-generated JavaScript bindings for Sad module: " << moduleName_ << "\n";
        js << "// روابط JavaScript مُولَّدة تلقائياً لوحدة ص\n\n";
        
        js << "let wasm;\n";
        js << "let memory;\n";
        js << "let heap;\n\n";
        
        // دوال مساعدة للذاكرة
        js << "// دوال الذاكرة المساعدة\n";
        js << "const encoder = new TextEncoder();\n";
        js << "const decoder = new TextDecoder('utf-8');\n\n";
        
        js << "function getStringFromMemory(ptr, len) {\n";
        js << "  return decoder.decode(new Uint8Array(memory.buffer, ptr, len));\n";
        js << "}\n\n";
        
        js << "function getStringFromCString(ptr) {\n";
        js << "  const mem = new Uint8Array(memory.buffer);\n";
        js << "  let end = ptr;\n";
        js << "  while (mem[end] !== 0) end++;\n";
        js << "  return decoder.decode(mem.slice(ptr, end));\n";
        js << "}\n\n";
        
        js << "function putStringToMemory(str) {\n";
        js << "  const bytes = encoder.encode(str + '\\0');\n";
        js << "  const ptr = wasm.exports.__sad_malloc(bytes.length);\n";
        js << "  new Uint8Array(memory.buffer).set(bytes, ptr);\n";
        js << "  return ptr;\n";
        js << "}\n\n";
        
        // كائنات الاستيراد
        js << "const imports = {\n";
        js << "  env: {\n";
        
        for (const auto& imp : imports_) {
            js << "    " << imp.wasmName << ": " << imp.jsName << ",\n";
        }
        
        // دوال قياسية
        js << "    // دوال قياسية\n";
        js << "    اطبع: (ptr) => console.log(getStringFromCString(ptr)),\n";
        js << "    اطبع_رقم: (n) => console.log(n),\n";
        js << "    __sad_malloc: (size) => {\n";
        js << "      // تخصيص بسيط من الكومة\n";
        js << "      const ptr = heap;\n";
        js << "      heap += size + (8 - (size % 8)) % 8; // محاذاة 8 بايت\n";
        js << "      return ptr;\n";
        js << "    },\n";
        js << "    __sad_free: (ptr) => { /* GC will handle */ },\n";
        js << "  },\n";
        js << "  wasi_snapshot_preview1: {\n";
        js << "    fd_write: () => {},\n";
        js << "    fd_close: () => {},\n";
        js << "    fd_seek: () => {},\n";
        js << "    proc_exit: (code) => { throw new Error('Program exited: ' + code); }\n";
        js << "  }\n";
        js << "};\n\n";
        
        // دالة التحميل
        js << "export async function load(wasmPath) {\n";
        js << "  const response = await fetch(wasmPath || './" << moduleName_ << ".wasm');\n";
        js << "  const bytes = await response.arrayBuffer();\n";
        js << "  const result = await WebAssembly.instantiate(bytes, imports);\n";
        js << "  \n";
        js << "  wasm = result;\n";
        js << "  memory = wasm.instance.exports.memory || new WebAssembly.Memory({ initial: 16 });\n";
        js << "  heap = (wasm.instance.exports.__heap_base?.value || 65536);\n";
        js << "  \n";
        js << "  return api;\n";
        js << "}\n\n";
        
        // API المُصدَّر
        js << "const api = {\n";
        
        for (const auto& func : exports_) {
            js << "  " << func.jsName << ": ";
            
            if (func.params.empty() && func.returnType == "void") {
                js << "() => wasm.instance.exports." << func.wasmName << "(),\n";
            } else {
                js << "(";
                for (size_t i = 0; i < func.params.size(); ++i) {
                    if (i > 0) js << ", ";
                    js << func.params[i].first;
                }
                js << ") => {\n";
                
                // تحويل المعاملات إذا لزم الأمر
                for (const auto& param : func.params) {
                    if (param.second == "نص" || param.second == "string") {
                        js << "    const _" << param.first << " = putStringToMemory(" << param.first << ");\n";
                    }
                }
                
                js << "    const result = wasm.instance.exports." << func.wasmName << "(";
                for (size_t i = 0; i < func.params.size(); ++i) {
                    if (i > 0) js << ", ";
                    if (func.params[i].second == "نص" || func.params[i].second == "string") {
                        js << "_" << func.params[i].first;
                    } else {
                        js << func.params[i].first;
                    }
                }
                js << ");\n";
                
                if (func.returnType == "نص" || func.returnType == "string") {
                    js << "    return getStringFromCString(result);\n";
                } else if (func.returnType != "void" && func.returnType != "فراغ") {
                    js << "    return result;\n";
                }
                
                js << "  },\n";
            }
            
            // الاسم العربي
            if (!func.arabicName.empty() && func.arabicName != func.jsName) {
                js << "  " << func.arabicName << ": null, // سيُعيَّن لاحقاً\n";
            }
        }
        
        js << "};\n\n";
        
        // تعيين الأسماء العربية
        js << "// تعيين الأسماء العربية\n";
        for (const auto& func : exports_) {
            if (!func.arabicName.empty() && func.arabicName != func.jsName) {
                js << "api." << func.arabicName << " = api." << func.jsName << ";\n";
            }
        }
        
        js << "\nexport default api;\n";
        
        return js.str();
    }
    
    /**
     * توليد ملفات الربط
     */
    bool generateBindings(const std::string& outputDir) {
        // TypeScript definitions
        std::ofstream tsFile(outputDir + "/" + moduleName_ + ".d.ts");
        if (tsFile) {
            tsFile << generateTypeScript();
            tsFile.close();
        }
        
        // JavaScript glue
        std::ofstream jsFile(outputDir + "/" + moduleName_ + ".js");
        if (jsFile) {
            jsFile << generateJSGlue();
            jsFile.close();
        }
        
        return true;
    }
    
private:
    /**
     * تحويل نوع ص إلى TypeScript
     */
    std::string sadTypeToTS(const std::string& sadType) {
        if (sadType == "رقم" || sadType == "عدد" || sadType == "ع32" || 
            sadType == "ع64" || sadType == "عش32" || sadType == "عش64") {
            return "number";
        } else if (sadType == "نص" || sadType == "string") {
            return "string";
        } else if (sadType == "منطقي" || sadType == "bool") {
            return "boolean";
        } else if (sadType == "فراغ" || sadType == "void") {
            return "void";
        } else if (sadType.find("مصفوفة") != std::string::npos ||
                   sadType.find("قائمة") != std::string::npos) {
            return "Array<any>";
        } else if (sadType.find("قاموس") != std::string::npos) {
            return "Record<string, any>";
        }
        return "any";
    }
};

/**
 * مولد DOM bindings
 */
class DOMBindGen {
public:
    /**
     * توليد روابط DOM العربية
     */
    static std::string generateDOMBindings() {
        std::ostringstream js;
        
        js << "// روابط DOM العربية\n";
        js << "// Arabic DOM Bindings\n\n";
        
        js << "export const dom = {\n";
        
        // استعلام العناصر
        js << "  // استعلام العناصر\n";
        js << "  استعلم: (محدد) => document.querySelector(محدد),\n";
        js << "  استعلم_الكل: (محدد) => [...document.querySelectorAll(محدد)],\n";
        js << "  بالمعرف: (معرف) => document.getElementById(معرف),\n";
        js << "  بالصنف: (صنف) => [...document.getElementsByClassName(صنف)],\n";
        js << "  بالوسم: (وسم) => [...document.getElementsByTagName(وسم)],\n\n";
        
        // إنشاء العناصر
        js << "  // إنشاء العناصر\n";
        js << "  أنشئ: (وسم) => document.createElement(وسم),\n";
        js << "  أنشئ_نص: (نص) => document.createTextNode(نص),\n";
        js << "  أنشئ_قطعة: () => document.createDocumentFragment(),\n\n";
        
        // التلاعب بالعناصر
        js << "  // التلاعب بالعناصر\n";
        js << "  أضف_ابن: (أب, ابن) => أب.appendChild(ابن),\n";
        js << "  أزل_ابن: (أب, ابن) => أب.removeChild(ابن),\n";
        js << "  أدرج_قبل: (مرجع, جديد) => مرجع.parentNode.insertBefore(جديد, مرجع),\n";
        js << "  استبدل: (قديم, جديد) => قديم.parentNode.replaceChild(جديد, قديم),\n";
        js << "  انسخ: (عنصر, عميق = true) => عنصر.cloneNode(عميق),\n\n";
        
        // السمات
        js << "  // السمات\n";
        js << "  اقرأ_سمة: (عنصر, اسم) => عنصر.getAttribute(اسم),\n";
        js << "  عيّن_سمة: (عنصر, اسم, قيمة) => عنصر.setAttribute(اسم, قيمة),\n";
        js << "  أزل_سمة: (عنصر, اسم) => عنصر.removeAttribute(اسم),\n";
        js << "  له_سمة: (عنصر, اسم) => عنصر.hasAttribute(اسم),\n\n";
        
        // الأصناف
        js << "  // الأصناف CSS\n";
        js << "  أضف_صنف: (عنصر, صنف) => عنصر.classList.add(صنف),\n";
        js << "  أزل_صنف: (عنصر, صنف) => عنصر.classList.remove(صنف),\n";
        js << "  بدّل_صنف: (عنصر, صنف) => عنصر.classList.toggle(صنف),\n";
        js << "  يحوي_صنف: (عنصر, صنف) => عنصر.classList.contains(صنف),\n\n";
        
        // الأنماط
        js << "  // الأنماط CSS\n";
        js << "  نمط: (عنصر, خاصية, قيمة) => { عنصر.style[خاصية] = قيمة; },\n";
        js << "  أنماط: (عنصر, أنماط) => Object.assign(عنصر.style, أنماط),\n\n";
        
        // المحتوى
        js << "  // المحتوى\n";
        js << "  نص: (عنصر, نص) => { if (نص !== undefined) عنصر.textContent = نص; return عنصر.textContent; },\n";
        js << "  html: (عنصر, html) => { if (html !== undefined) عنصر.innerHTML = html; return عنصر.innerHTML; },\n";
        js << "  قيمة: (عنصر, قيمة) => { if (قيمة !== undefined) عنصر.value = قيمة; return عنصر.value; },\n\n";
        
        // الأحداث
        js << "  // الأحداث\n";
        js << "  عند: (عنصر, حدث, معالج) => عنصر.addEventListener(حدث, معالج),\n";
        js << "  أزل_عند: (عنصر, حدث, معالج) => عنصر.removeEventListener(حدث, معالج),\n";
        js << "  أطلق: (عنصر, حدث) => عنصر.dispatchEvent(new Event(حدث)),\n";
        
        js << "};\n\n";
        
        // أحداث مختصرة
        js << "// أحداث مختصرة\n";
        js << "export const أحداث = {\n";
        js << "  نقر: 'click',\n";
        js << "  نقر_مزدوج: 'dblclick',\n";
        js << "  تمرير_فوق: 'mouseover',\n";
        js << "  تمرير_خارج: 'mouseout',\n";
        js << "  ضغط_مفتاح: 'keydown',\n";
        js << "  رفع_مفتاح: 'keyup',\n";
        js << "  إدخال: 'input',\n";
        js << "  تغيير: 'change',\n";
        js << "  إرسال: 'submit',\n";
        js << "  تحميل: 'load',\n";
        js << "  خطأ: 'error',\n";
        js << "};\n";
        
        return js.str();
    }
};

} // namespace backend
} // namespace sad
