/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ███████╗  █████╗  ██████╗      ██╗      █████╗  ███╗   ██╗ ██████╗        ║
 * ║   ██╔════╝ ██╔══██╗ ██╔══██╗     ██║     ██╔══██╗ ████╗  ██║ ██╔════╝       ║
 * ║   ███████╗ ███████║ ██║  ██║     ██║     ███████║ ██╔██╗ ██║ ██║  ███╗      ║
 * ║   ╚════██║ ██╔══██║ ██║  ██║     ██║     ██╔══██║ ██║╚██╗██║ ██║   ██║      ║
 * ║   ███████║ ██║  ██║ ██████╔╝     ███████╗██║  ██║ ██║ ╚████║ ╚██████╔╝      ║
 * ║   ╚══════╝ ╚═╝  ╚═╝ ╚═════╝      ╚══════╝╚═╝  ╚═╝ ╚═╝  ╚═══╝  ╚═════╝       ║
 * ║                                                                              ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║  ملف: async_comprehension.cpp                                                ║
 * ║  الوصف: دعم الـ Comprehensions غير المتزامنة                                  ║
 * ║  المهمة: T152 - Phase 13 (US11)                                             ║
 * ║  المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 الهدف | Goal:
 *    دعم الـ Comprehensions غير المتزامنة التي تتعامل مع async iterators
 *    Support async comprehensions that work with async iterators
 *
 * 📋 أنواع الـ Async Comprehensions:
 *
 *    1️⃣ قائمة غير متزامنة | Async List Comprehension:
 *       [انتظر س لكل_غير_متزامن س في تدفق_غير_متزامن]
 *       [await x async for x in async_stream]
 *
 *    2️⃣ قاموس غير متزامن | Async Dict Comprehension:
 *       {مفتاح: انتظر قيمة لكل_غير_متزامن (مفتاح، قيمة) في تدفق}
 *
 *    3️⃣ مجموعة غير متزامنة | Async Set Comprehension:
 *       {|انتظر س لكل_غير_متزامن س في تدفق|}
 *
 *    4️⃣ مولد غير متزامن | Async Generator:
 *       (انتظر س لكل_غير_متزامن س في تدفق)
 *
 * 🎨 الصيغة العربية | Arabic Syntax:
 *
 *    // قائمة من طلبات HTTP
 *    دع النتائج = [
 *        انتظر استجابة.json()
 *        لكل_غير_متزامن استجابة في طلبات_HTTP(روابط)
 *    ]
 *    
 *    // معالجة تدفق البيانات
 *    دع البيانات = [
 *        انتظر معالجة(عنصر)
 *        لكل_غير_متزامن عنصر في تدفق_البيانات
 *        إذا عنصر.صالح
 *    ]
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <variant>
#include <unordered_map>
#include <sstream>

namespace sad {
namespace compiler {
namespace async_comprehension {

// ═══════════════════════════════════════════════════════════════════════════════
//                           التصريحات الأمامية | Forward Declarations
// ═══════════════════════════════════════════════════════════════════════════════

class AsyncIterator;
class AsyncComprehensionBuilder;
class AsyncComprehensionTransformer;
class AsyncComprehensionCodegen;

// ═══════════════════════════════════════════════════════════════════════════════
//                          أنواع الـ Async | Async Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🏷️ نوع_الـ_Comprehension_غير_المتزامن | AsyncComprehensionKind
 */
enum class AsyncComprehensionKind {
    // ═══════════════════════════════════════════════════════════════════════════
    //                          قائمة | List
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * قائمة غير متزامنة
     * [انتظر س لكل_غير_متزامن س في تدفق]
     */
    ASYNC_LIST,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          قاموس | Dict
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * قاموس غير متزامن
     * {م: انتظر ق لكل_غير_متزامن (م، ق) في تدفق}
     */
    ASYNC_DICT,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          مجموعة | Set
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * مجموعة غير متزامنة
     * {|انتظر س لكل_غير_متزامن س في تدفق|}
     */
    ASYNC_SET,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          مولد | Generator
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * مولد غير متزامن
     * (انتظر س لكل_غير_متزامن س في تدفق)
     */
    ASYNC_GENERATOR
};

/**
 * 🔧 نوع_العملية_غير_المتزامنة | AsyncOperationKind
 */
enum class AsyncOperationKind {
    AWAIT_ELEMENT,    // انتظار العنصر
    AWAIT_FILTER,     // انتظار شرط الترشيح
    AWAIT_TRANSFORM,  // انتظار التحويل
    YIELD_ASYNC,      // إنتاج غير متزامن
    PARALLEL_AWAIT    // انتظار متوازي
};

// ═══════════════════════════════════════════════════════════════════════════════
//                     عقدة الـ Comprehension | Comprehension Node
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📦 حلقة_غير_متزامنة | AsyncLoop
 * 
 * تمثيل حلقة لكل_غير_متزامن
 */
struct AsyncLoop {
    std::string variableName;     // اسم المتغير (س)
    std::string iterableExpr;     // تعبير المتكرر (تدفق)
    std::string iterableType;     // نوع المتكرر
    bool isAsync = true;          // هل الحلقة غير متزامنة؟
    std::vector<std::string> destructuringPattern; // أنماط التفكيك إن وجدت
};

/**
 * 📦 شرط_غير_متزامن | AsyncCondition
 * 
 * شرط ترشيح قد يكون غير متزامن
 */
struct AsyncCondition {
    std::string expression;
    bool requiresAwait = false;   // هل يتطلب await؟
    std::string awaitedExpr;      // التعبير المنتظر إن وجد
};

/**
 * 📦 عقدة_Comprehension_غير_متزامنة | AsyncComprehensionNode
 * 
 * تمثيل AST للـ comprehension غير المتزامن
 */
struct AsyncComprehensionNode {
    AsyncComprehensionKind kind;
    
    // التعبير الناتج (قد يحتوي على await)
    std::string outputExpression;
    bool outputRequiresAwait = false;
    
    // للقواميس فقط
    std::string keyExpression;
    std::string valueExpression;
    bool keyRequiresAwait = false;
    bool valueRequiresAwait = false;
    
    // الحلقات (قد تكون متداخلة)
    std::vector<AsyncLoop> loops;
    
    // الشروط
    std::vector<AsyncCondition> conditions;
    
    // معلومات النوع
    std::string elementType;
    std::string resultType;
    
    // موقع المصدر
    int line = 0;
    int column = 0;
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      محول الـ Comprehension | Comprehension Transformer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔄 محول_Comprehension_غير_متزامن | AsyncComprehensionTransformer
 * 
 * يحول الـ comprehension غير المتزامن إلى دوال async عادية
 */
class AsyncComprehensionTransformer {
public:
    /**
     * 📝 نتيجة_التحويل | TransformResult
     */
    struct TransformResult {
        std::string transformedCode;
        std::string generatedFunctionName;
        std::vector<std::string> capturedVariables;
        bool isGenerator = false;
    };
    
    /**
     * تحويل comprehension غير متزامن | Transform async comprehension
     * 
     * يحول:
     *   [انتظر س لكل_غير_متزامن س في تدفق إذا شرط]
     * إلى:
     *   غير_متزامن دالة _comp_123() {
     *       دع نتيجة = []
     *       لكل_غير_متزامن س في تدفق {
     *           إذا شرط {
     *               نتيجة.أضف(انتظر س)
     *           }
     *       }
     *       أرجع نتيجة
     *   }
     */
    TransformResult transform(const AsyncComprehensionNode& node) {
        TransformResult result;
        result.generatedFunctionName = generateUniqueName();
        
        switch (node.kind) {
            case AsyncComprehensionKind::ASYNC_LIST:
                result.transformedCode = transformAsyncList(node, result.generatedFunctionName);
                break;
                
            case AsyncComprehensionKind::ASYNC_DICT:
                result.transformedCode = transformAsyncDict(node, result.generatedFunctionName);
                break;
                
            case AsyncComprehensionKind::ASYNC_SET:
                result.transformedCode = transformAsyncSet(node, result.generatedFunctionName);
                break;
                
            case AsyncComprehensionKind::ASYNC_GENERATOR:
                result.transformedCode = transformAsyncGenerator(node, result.generatedFunctionName);
                result.isGenerator = true;
                break;
        }
        
        return result;
    }
    
private:
    int nameCounter_ = 0;
    
    std::string generateUniqueName() {
        return "_async_comp_" + std::to_string(nameCounter_++);
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                      تحويل القائمة | Transform List
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string transformAsyncList(const AsyncComprehensionNode& node, const std::string& funcName) {
        std::stringstream ss;
        
        ss << "// ═══ قائمة غير متزامنة مُحولة ═══\n";
        ss << "غير_متزامن دالة " << funcName << "() -> قائمة<" << node.elementType << "> {\n";
        ss << "    دع نتيجة: قائمة<" << node.elementType << "> = []\n";
        ss << "    \n";
        
        // توليد الحلقات المتداخلة
        int indent = 1;
        for (const auto& loop : node.loops) {
            std::string indentStr(indent * 4, ' ');
            
            if (loop.isAsync) {
                ss << indentStr << "لكل_غير_متزامن " << loop.variableName 
                   << " في " << loop.iterableExpr << " {\n";
            } else {
                ss << indentStr << "لكل " << loop.variableName 
                   << " في " << loop.iterableExpr << " {\n";
            }
            indent++;
        }
        
        // توليد الشروط
        for (const auto& cond : node.conditions) {
            std::string indentStr(indent * 4, ' ');
            
            if (cond.requiresAwait) {
                ss << indentStr << "إذا انتظر " << cond.awaitedExpr << " {\n";
            } else {
                ss << indentStr << "إذا " << cond.expression << " {\n";
            }
            indent++;
        }
        
        // إضافة العنصر
        std::string innerIndent(indent * 4, ' ');
        if (node.outputRequiresAwait) {
            ss << innerIndent << "نتيجة.أضف(انتظر " << node.outputExpression << ")\n";
        } else {
            ss << innerIndent << "نتيجة.أضف(" << node.outputExpression << ")\n";
        }
        
        // إغلاق الأقواس
        for (size_t i = 0; i < node.conditions.size(); ++i) {
            indent--;
            ss << std::string(indent * 4, ' ') << "}\n";
        }
        for (size_t i = 0; i < node.loops.size(); ++i) {
            indent--;
            ss << std::string(indent * 4, ' ') << "}\n";
        }
        
        ss << "    \n";
        ss << "    أرجع نتيجة\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                      تحويل القاموس | Transform Dict
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string transformAsyncDict(const AsyncComprehensionNode& node, const std::string& funcName) {
        std::stringstream ss;
        
        ss << "// ═══ قاموس غير متزامن مُحول ═══\n";
        ss << "غير_متزامن دالة " << funcName << "() -> قاموس<" << node.resultType << "> {\n";
        ss << "    دع نتيجة: قاموس<" << node.resultType << "> = {}\n";
        ss << "    \n";
        
        // الحلقة الرئيسية
        for (const auto& loop : node.loops) {
            if (loop.isAsync) {
                ss << "    لكل_غير_متزامن " << loop.variableName 
                   << " في " << loop.iterableExpr << " {\n";
            } else {
                ss << "    لكل " << loop.variableName 
                   << " في " << loop.iterableExpr << " {\n";
            }
        }
        
        // الشروط
        for (const auto& cond : node.conditions) {
            if (cond.requiresAwait) {
                ss << "        إذا انتظر " << cond.awaitedExpr << " {\n";
            } else {
                ss << "        إذا " << cond.expression << " {\n";
            }
        }
        
        // إضافة المدخل
        std::string keyExpr = node.keyRequiresAwait ? 
            "انتظر " + node.keyExpression : node.keyExpression;
        std::string valueExpr = node.valueRequiresAwait ? 
            "انتظر " + node.valueExpression : node.valueExpression;
        
        ss << "            نتيجة[" << keyExpr << "] = " << valueExpr << "\n";
        
        // إغلاق
        for (size_t i = 0; i < node.conditions.size(); ++i) {
            ss << "        }\n";
        }
        for (size_t i = 0; i < node.loops.size(); ++i) {
            ss << "    }\n";
        }
        
        ss << "    \n";
        ss << "    أرجع نتيجة\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                      تحويل المجموعة | Transform Set
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string transformAsyncSet(const AsyncComprehensionNode& node, const std::string& funcName) {
        std::stringstream ss;
        
        ss << "// ═══ مجموعة غير متزامنة مُحولة ═══\n";
        ss << "غير_متزامن دالة " << funcName << "() -> مجموعة<" << node.elementType << "> {\n";
        ss << "    دع نتيجة: مجموعة<" << node.elementType << "> = {||}\n";
        ss << "    \n";
        
        for (const auto& loop : node.loops) {
            ss << "    لكل_غير_متزامن " << loop.variableName 
               << " في " << loop.iterableExpr << " {\n";
        }
        
        for (const auto& cond : node.conditions) {
            ss << "        إذا " << cond.expression << " {\n";
        }
        
        if (node.outputRequiresAwait) {
            ss << "            نتيجة.أضف(انتظر " << node.outputExpression << ")\n";
        } else {
            ss << "            نتيجة.أضف(" << node.outputExpression << ")\n";
        }
        
        for (size_t i = 0; i < node.conditions.size(); ++i) {
            ss << "        }\n";
        }
        for (size_t i = 0; i < node.loops.size(); ++i) {
            ss << "    }\n";
        }
        
        ss << "    \n";
        ss << "    أرجع نتيجة\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                      تحويل المولد | Transform Generator
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string transformAsyncGenerator(const AsyncComprehensionNode& node, const std::string& funcName) {
        std::stringstream ss;
        
        ss << "// ═══ مولد غير متزامن مُحول ═══\n";
        ss << "غير_متزامن دالة* " << funcName << "() -> تدفق_غير_متزامن<" << node.elementType << "> {\n";
        
        for (const auto& loop : node.loops) {
            ss << "    لكل_غير_متزامن " << loop.variableName 
               << " في " << loop.iterableExpr << " {\n";
        }
        
        for (const auto& cond : node.conditions) {
            ss << "        إذا " << cond.expression << " {\n";
        }
        
        if (node.outputRequiresAwait) {
            ss << "            أنتج انتظر " << node.outputExpression << "\n";
        } else {
            ss << "            أنتج " << node.outputExpression << "\n";
        }
        
        for (size_t i = 0; i < node.conditions.size(); ++i) {
            ss << "        }\n";
        }
        for (size_t i = 0; i < node.loops.size(); ++i) {
            ss << "    }\n";
        }
        
        ss << "}\n";
        
        return ss.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      مولد كود LLVM | LLVM Codegen
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 مولد_كود_Comprehension_غير_متزامن | AsyncComprehensionCodegen
 * 
 * يولد كود LLVM IR للـ comprehensions غير المتزامنة
 */
class AsyncComprehensionCodegen {
public:
    /**
     * توليد كود لـ comprehension غير متزامن | Generate code for async comprehension
     */
    std::string generate(const AsyncComprehensionNode& node) {
        std::stringstream ss;
        
        ss << "; ═══ Async Comprehension LLVM IR ═══\n";
        ss << "; النوع: " << kindToString(node.kind) << "\n";
        ss << "; العنصر: " << node.elementType << "\n\n";
        
        // توليد هيكل حالة الكوروتين
        std::string stateName = generateCoroutineState(node);
        
        // توليد دالة resume
        std::string resumeFunc = generateResumeFunction(node, stateName);
        
        // توليد دالة destroy
        std::string destroyFunc = generateDestroyFunction(stateName);
        
        ss << stateName << "\n\n";
        ss << resumeFunc << "\n\n";
        ss << destroyFunc << "\n";
        
        return ss.str();
    }
    
private:
    int stateCounter_ = 0;
    
    std::string kindToString(AsyncComprehensionKind kind) {
        switch (kind) {
            case AsyncComprehensionKind::ASYNC_LIST: return "قائمة_غير_متزامنة";
            case AsyncComprehensionKind::ASYNC_DICT: return "قاموس_غير_متزامن";
            case AsyncComprehensionKind::ASYNC_SET: return "مجموعة_غير_متزامنة";
            case AsyncComprehensionKind::ASYNC_GENERATOR: return "مولد_غير_متزامن";
        }
        return "مجهول";
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    هيكل حالة الكوروتين | Coroutine State Struct
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string generateCoroutineState(const AsyncComprehensionNode& node) {
        std::stringstream ss;
        std::string name = "async_comp_state_" + std::to_string(stateCounter_++);
        
        ss << "; هيكل حالة الـ comprehension غير المتزامن\n";
        ss << "%" << name << " = type {\n";
        ss << "  ptr,             ; مؤشر دالة resume\n";
        ss << "  ptr,             ; مؤشر دالة destroy\n";
        ss << "  i8,              ; حالة التنفيذ (0=بداية، 1=يعمل، 2=معلق، 3=منتهي)\n";
        ss << "  i32,             ; نقطة الاستئناف\n";
        ss << "  ptr,             ; مؤشر النتيجة (القائمة/القاموس/المجموعة)\n";
        ss << "  ptr,             ; مؤشر المكرر الحالي\n";
        
        // حقول إضافية للحلقات المتداخلة
        int fieldIndex = 6;
        for (size_t i = 0; i < node.loops.size(); ++i) {
            ss << "  ptr,             ; حالة المكرر " << i << "\n";
            ss << "  ptr,             ; العنصر الحالي " << i << "\n";
            fieldIndex += 2;
        }
        
        // حقول للمتغيرات الملتقطة
        ss << "  ; متغيرات ملتقطة\n";
        
        ss << "}\n";
        
        return ss.str();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                       دالة الاستئناف | Resume Function
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string generateResumeFunction(const AsyncComprehensionNode& node, const std::string& stateName) {
        std::stringstream ss;
        
        ss << "; دالة استئناف الـ comprehension\n";
        ss << "define void @" << stateName << "_resume(ptr %frame) {\n";
        ss << "entry:\n";
        ss << "  ; تحميل نقطة الاستئناف\n";
        ss << "  %resume_ptr = getelementptr %" << stateName << ", ptr %frame, i32 0, i32 3\n";
        ss << "  %resume_point = load i32, ptr %resume_ptr\n";
        ss << "  \n";
        ss << "  ; القفز لنقطة الاستئناف\n";
        ss << "  switch i32 %resume_point, label %unreachable [\n";
        ss << "    i32 0, label %init\n";
        ss << "    i32 1, label %loop_start\n";
        ss << "    i32 2, label %await_element\n";
        ss << "    i32 3, label %await_filter\n";
        ss << "    i32 4, label %await_output\n";
        ss << "    i32 5, label %finalize\n";
        ss << "  ]\n\n";
        
        // نقطة البداية
        ss << "init:\n";
        ss << "  ; تهيئة حاوية النتيجة\n";
        ss << "  %result = call ptr @sad_" << resultContainerName(node.kind) << "_create()\n";
        ss << "  %result_ptr = getelementptr %" << stateName << ", ptr %frame, i32 0, i32 4\n";
        ss << "  store ptr %result, ptr %result_ptr\n";
        ss << "  \n";
        ss << "  ; تهيئة المكرر\n";
        ss << "  ; ... (حسب نوع المتكرر)\n";
        ss << "  \n";
        ss << "  store i32 1, ptr %resume_ptr\n";
        ss << "  br label %loop_start\n\n";
        
        // بداية الحلقة
        ss << "loop_start:\n";
        ss << "  ; طلب العنصر التالي من المكرر غير المتزامن\n";
        ss << "  %iter_ptr = getelementptr %" << stateName << ", ptr %frame, i32 0, i32 5\n";
        ss << "  %iter = load ptr, ptr %iter_ptr\n";
        ss << "  %next_future = call ptr @async_iter_next(ptr %iter)\n";
        ss << "  \n";
        ss << "  ; استطلاع المستقبل\n";
        ss << "  %poll_result = call { i8, ptr } @future_poll(ptr %next_future)\n";
        ss << "  %is_ready = extractvalue { i8, ptr } %poll_result, 0\n";
        ss << "  %is_done = icmp eq i8 %is_ready, 0  ; 0 = منتهي\n";
        ss << "  br i1 %is_done, label %finalize, label %check_pending\n\n";
        
        ss << "check_pending:\n";
        ss << "  %is_pending = icmp eq i8 %is_ready, 2  ; 2 = معلق\n";
        ss << "  br i1 %is_pending, label %suspend_await_element, label %process_element\n\n";
        
        // تعليق في انتظار العنصر
        ss << "suspend_await_element:\n";
        ss << "  store i32 2, ptr %resume_ptr\n";
        ss << "  ret void\n\n";
        
        ss << "await_element:\n";
        ss << "  ; استئناف بعد انتظار العنصر\n";
        ss << "  br label %loop_start\n\n";
        
        // معالجة العنصر
        ss << "process_element:\n";
        ss << "  %element = extractvalue { i8, ptr } %poll_result, 1\n";
        ss << "  \n";
        
        // الشروط
        if (!node.conditions.empty()) {
            ss << "  ; تحقق من الشروط\n";
            for (size_t i = 0; i < node.conditions.size(); ++i) {
                ss << "  ; شرط " << i << ": " << node.conditions[i].expression << "\n";
                if (node.conditions[i].requiresAwait) {
                    ss << "  ; (يتطلب await)\n";
                    // توليد كود انتظار الشرط
                }
            }
            ss << "  %cond_result = icmp eq i1 true, true  ; مبسط\n";
            ss << "  br i1 %cond_result, label %add_element, label %loop_start\n\n";
        } else {
            ss << "  br label %add_element\n\n";
        }
        
        ss << "add_element:\n";
        if (node.outputRequiresAwait) {
            ss << "  ; انتظار تحويل العنصر\n";
            ss << "  %transform_future = call ptr @eval_output(ptr %element)\n";
            ss << "  %transform_result = call { i8, ptr } @future_poll(ptr %transform_future)\n";
            ss << "  %transform_pending = extractvalue { i8, ptr } %transform_result, 0\n";
            ss << "  %is_transform_pending = icmp eq i8 %transform_pending, 2\n";
            ss << "  br i1 %is_transform_pending, label %suspend_await_output, label %store_element\n\n";
            
            ss << "suspend_await_output:\n";
            ss << "  store i32 4, ptr %resume_ptr\n";
            ss << "  ret void\n\n";
            
            ss << "await_output:\n";
            ss << "  br label %store_element\n\n";
            
            ss << "store_element:\n";
            ss << "  %output = extractvalue { i8, ptr } %transform_result, 1\n";
        } else {
            ss << "  %output = %element  ; بدون تحويل\n";
        }
        
        // إضافة للنتيجة
        ss << "  %result_container = load ptr, ptr %result_ptr\n";
        ss << "  call void @" << addFunctionName(node.kind) << "(ptr %result_container, ptr %output)\n";
        ss << "  br label %loop_start\n\n";
        
        // الانتهاء
        ss << "finalize:\n";
        ss << "  ; تحديث الحالة لمنتهي\n";
        ss << "  %state_ptr = getelementptr %" << stateName << ", ptr %frame, i32 0, i32 2\n";
        ss << "  store i8 3, ptr %state_ptr\n";
        ss << "  ret void\n\n";
        
        // تسمية لا يمكن الوصول إليها
        ss << "await_filter:\n";
        ss << "  ; استئناف بعد انتظار الشرط\n";
        ss << "  br label %process_element\n\n";
        
        ss << "unreachable:\n";
        ss << "  unreachable\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                       دالة التدمير | Destroy Function
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string generateDestroyFunction(const std::string& stateName) {
        std::stringstream ss;
        
        ss << "; دالة تدمير حالة الـ comprehension\n";
        ss << "define void @" << stateName << "_destroy(ptr %frame) {\n";
        ss << "entry:\n";
        ss << "  ; تحرير النتيجة إن وجدت\n";
        ss << "  %result_ptr = getelementptr %" << stateName << ", ptr %frame, i32 0, i32 4\n";
        ss << "  %result = load ptr, ptr %result_ptr\n";
        ss << "  %has_result = icmp ne ptr %result, null\n";
        ss << "  br i1 %has_result, label %free_result, label %free_frame\n\n";
        
        ss << "free_result:\n";
        ss << "  call void @sad_free(ptr %result)\n";
        ss << "  br label %free_frame\n\n";
        
        ss << "free_frame:\n";
        ss << "  ; تحرير إطار الكوروتين\n";
        ss << "  call void @sad_free(ptr %frame)\n";
        ss << "  ret void\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    std::string resultContainerName(AsyncComprehensionKind kind) {
        switch (kind) {
            case AsyncComprehensionKind::ASYNC_LIST: return "list";
            case AsyncComprehensionKind::ASYNC_DICT: return "dict";
            case AsyncComprehensionKind::ASYNC_SET: return "set";
            case AsyncComprehensionKind::ASYNC_GENERATOR: return "generator";
        }
        return "list";
    }
    
    std::string addFunctionName(AsyncComprehensionKind kind) {
        switch (kind) {
            case AsyncComprehensionKind::ASYNC_LIST: return "sad_list_append";
            case AsyncComprehensionKind::ASYNC_DICT: return "sad_dict_insert";
            case AsyncComprehensionKind::ASYNC_SET: return "sad_set_insert";
            case AsyncComprehensionKind::ASYNC_GENERATOR: return "sad_yield";
        }
        return "sad_list_append";
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      التحقق من الأنواع | Type Checking
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔍 مدقق_أنواع_Comprehension_غير_متزامن | AsyncComprehensionTypeChecker
 * 
 * يتحقق من صحة الأنواع في الـ comprehensions غير المتزامنة
 */
class AsyncComprehensionTypeChecker {
public:
    struct TypeCheckResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::string inferredType;
        std::string elementType;
    };
    
    /**
     * التحقق من الأنواع | Check types
     */
    TypeCheckResult check(const AsyncComprehensionNode& node) {
        TypeCheckResult result;
        
        // التحقق من أن المتكررات غير متزامنة فعلاً
        for (const auto& loop : node.loops) {
            if (loop.isAsync) {
                if (!isAsyncIterable(loop.iterableType)) {
                    result.isValid = false;
                    result.errors.push_back(
                        "المتكرر '" + loop.iterableExpr + "' يجب أن يكون من نوع تدفق_غير_متزامن أو AsyncIterable"
                    );
                }
            }
        }
        
        // التحقق من أن await يُستخدم فقط داخل سياق async
        if (node.outputRequiresAwait || node.keyRequiresAwait || node.valueRequiresAwait) {
            // هذا صحيح في async comprehension
        }
        
        // استنتاج نوع النتيجة
        result.elementType = node.elementType;
        
        switch (node.kind) {
            case AsyncComprehensionKind::ASYNC_LIST:
                result.inferredType = "مستقبل<قائمة<" + node.elementType + ">>";
                break;
            case AsyncComprehensionKind::ASYNC_DICT:
                result.inferredType = "مستقبل<قاموس<" + node.resultType + ">>";
                break;
            case AsyncComprehensionKind::ASYNC_SET:
                result.inferredType = "مستقبل<مجموعة<" + node.elementType + ">>";
                break;
            case AsyncComprehensionKind::ASYNC_GENERATOR:
                result.inferredType = "تدفق_غير_متزامن<" + node.elementType + ">";
                break;
        }
        
        return result;
    }
    
private:
    bool isAsyncIterable(const std::string& type) {
        return type.find("تدفق_غير_متزامن") != std::string::npos ||
               type.find("AsyncIterable") != std::string::npos ||
               type.find("AsyncIterator") != std::string::npos ||
               type.find("مكرر_غير_متزامن") != std::string::npos;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                      واجهة التكامل | Integration Interface
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔌 واجهة_Comprehension_غير_متزامن | AsyncComprehensionInterface
 * 
 * الواجهة الرئيسية لمعالجة الـ comprehensions غير المتزامنة
 */
class AsyncComprehensionInterface {
private:
    AsyncComprehensionTransformer transformer_;
    AsyncComprehensionCodegen codegen_;
    AsyncComprehensionTypeChecker typeChecker_;
    
public:
    /**
     * معالجة comprehension غير متزامن | Process async comprehension
     */
    struct ProcessResult {
        bool success = true;
        std::string transformedCode;
        std::string llvmIR;
        std::string inferredType;
        std::vector<std::string> errors;
    };
    
    ProcessResult process(const AsyncComprehensionNode& node) {
        ProcessResult result;
        
        // التحقق من الأنواع
        auto typeResult = typeChecker_.check(node);
        if (!typeResult.isValid) {
            result.success = false;
            result.errors = typeResult.errors;
            return result;
        }
        
        result.inferredType = typeResult.inferredType;
        
        // التحويل
        auto transformResult = transformer_.transform(node);
        result.transformedCode = transformResult.transformedCode;
        
        // توليد LLVM IR
        result.llvmIR = codegen_.generate(node);
        
        return result;
    }
    
    /**
     * إنشاء عقدة من المعاملات | Create node from parameters
     */
    AsyncComprehensionNode createNode(
        AsyncComprehensionKind kind,
        const std::string& outputExpr,
        const std::string& variable,
        const std::string& iterable,
        bool isAsyncLoop,
        bool outputRequiresAwait
    ) {
        AsyncComprehensionNode node;
        node.kind = kind;
        node.outputExpression = outputExpr;
        node.outputRequiresAwait = outputRequiresAwait;
        
        AsyncLoop loop;
        loop.variableName = variable;
        loop.iterableExpr = iterable;
        loop.isAsync = isAsyncLoop;
        node.loops.push_back(loop);
        
        return node;
    }
};

} // namespace async_comprehension
} // namespace compiler
} // namespace sad

// ═══════════════════════════════════════════════════════════════════════════════
//                              اختبارات | Tests
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef SAD_ASYNC_COMPREHENSION_TESTS

#include <iostream>

void testAsyncListComprehension() {
    using namespace sad::compiler::async_comprehension;
    
    std::cout << "═══ اختبار قائمة غير متزامنة ═══\n\n";
    
    AsyncComprehensionInterface iface;
    
    // [انتظر استجابة.json() لكل_غير_متزامن استجابة في طلبات_HTTP(روابط)]
    auto node = iface.createNode(
        AsyncComprehensionKind::ASYNC_LIST,
        "استجابة.json()",
        "استجابة",
        "طلبات_HTTP(روابط)",
        true,   // async loop
        true    // output requires await
    );
    node.elementType = "قاموس<نص، أي>";
    
    auto result = iface.process(node);
    
    if (result.success) {
        std::cout << "✅ نجح التحقق من الأنواع\n";
        std::cout << "النوع المستنتج: " << result.inferredType << "\n\n";
        std::cout << "الكود المحول:\n" << result.transformedCode << "\n";
    } else {
        std::cout << "❌ فشل التحقق من الأنواع:\n";
        for (const auto& error : result.errors) {
            std::cout << "  - " << error << "\n";
        }
    }
}

void testAsyncGenerator() {
    using namespace sad::compiler::async_comprehension;
    
    std::cout << "\n═══ اختبار مولد غير متزامن ═══\n\n";
    
    AsyncComprehensionInterface iface;
    
    // (انتظر معالجة(عنصر) لكل_غير_متزامن عنصر في تدفق_البيانات إذا عنصر.صالح)
    auto node = iface.createNode(
        AsyncComprehensionKind::ASYNC_GENERATOR,
        "معالجة(عنصر)",
        "عنصر",
        "تدفق_البيانات",
        true,
        true
    );
    node.elementType = "بيانات_معالجة";
    
    AsyncCondition cond;
    cond.expression = "عنصر.صالح";
    node.conditions.push_back(cond);
    
    auto result = iface.process(node);
    
    if (result.success) {
        std::cout << "✅ نجح\n";
        std::cout << "النوع: " << result.inferredType << "\n\n";
        std::cout << "الكود:\n" << result.transformedCode << "\n";
    } else {
        std::cout << "❌ فشل\n";
    }
}

void testAsyncDictComprehension() {
    using namespace sad::compiler::async_comprehension;
    
    std::cout << "\n═══ اختبار قاموس غير متزامن ═══\n\n";
    
    AsyncComprehensionNode node;
    node.kind = AsyncComprehensionKind::ASYNC_DICT;
    node.keyExpression = "مفتاح";
    node.valueExpression = "احضر_قيمة(مفتاح)";
    node.keyRequiresAwait = false;
    node.valueRequiresAwait = true;
    node.resultType = "نص، عدد";
    
    AsyncLoop loop;
    loop.variableName = "مفتاح";
    loop.iterableExpr = "المفاتيح";
    loop.isAsync = true;
    node.loops.push_back(loop);
    
    AsyncComprehensionInterface iface;
    auto result = iface.process(node);
    
    if (result.success) {
        std::cout << "✅ نجح\n";
        std::cout << "الكود:\n" << result.transformedCode << "\n";
    }
}

int main() {
    testAsyncListComprehension();
    testAsyncGenerator();
    testAsyncDictComprehension();
    return 0;
}

#endif // SAD_ASYNC_COMPREHENSION_TESTS
