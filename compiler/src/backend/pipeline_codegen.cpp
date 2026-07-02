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
 * ║  ملف: pipeline_codegen.cpp                                                   ║
 * ║  الوصف: توليد كود LLVM IR لعامل الأنبوب (|>)                                 ║
 * ║  المهمة: T151 - Phase 13 (US11)                                             ║
 * ║  المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                              نظرة عامة | Overview
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 الهدف | Goal:
 *    توليد كود LLVM IR فعال لعمليات الأنبوب (Pipeline Operator |>)
 *    Generate efficient LLVM IR for pipeline operations
 *
 * 📋 استراتيجيات التوليد | Generation Strategies:
 *
 *    1️⃣ التوليد المباشر | Direct Generation:
 *       قيمة |> دالة → دالة(قيمة)
 *       Simple function call transformation
 *
 *    2️⃣ توليد السلسلة | Chain Generation:
 *       قيمة |> د1 |> د2 |> د3 → د3(د2(د1(قيمة)))
 *       Nested function calls with inlining
 *
 *    3️⃣ توليد كسول | Lazy Generation:
 *       مدى(∞) |> صفّي(ش) |> خذ(10)
 *       Iterator state machine
 *
 *    4️⃣ توليد متوازي | Parallel Generation:
 *       قائمة |> حوّل_متوازي(د)
 *       SIMD or thread-based parallelization
 *
 * 🎨 الصيغة العربية | Arabic Syntax:
 *
 *    // سلسلة بسيطة
 *    قيمة |> دالة1 |> دالة2
 *    
 *    // مع placeholder
 *    قيمة |> دالة(_, معامل2)
 *    
 *    // سلسلة lambda
 *    قيمة |> |س| س + 1 |> |س| س * 2
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <memory>
#include <vector>
#include <string>
#include <sstream>
#include <functional>
#include <unordered_map>
#include <optional>
#include <variant>

namespace sad {
namespace compiler {
namespace codegen {

// ═══════════════════════════════════════════════════════════════════════════════
//                           التصريحات الأمامية | Forward Declarations
// ═══════════════════════════════════════════════════════════════════════════════

class LLVMCodegen;
class PipelineCodegen;
class PipelineOptimizer;

// نفترض وجود هذه الأنواع من الـ AST
struct Expression;
struct FunctionCallExpr;
struct LambdaExpr;
struct PipelineExpr;
struct Type;

// ═══════════════════════════════════════════════════════════════════════════════
//                          أنواع الأنبوب | Pipeline Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🏷️ نوع_عملية_الأنبوب | PipelineOperationType
 */
enum class PipelineOperationType {
    // ═══════════════════════════════════════════════════════════════════════════
    //                      استدعاء_دالة | Function Call
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * استدعاء دالة عادي
     * قيمة |> دالة
     */
    FUNCTION_CALL,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                    استدعاء_مع_وسيط | Call with Placeholder
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * استدعاء مع تحديد موضع الوسيط
     * قيمة |> دالة(_, معامل2)
     */
    CALL_WITH_PLACEHOLDER,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          لامدا | Lambda
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * تطبيق lambda
     * قيمة |> |س| س + 1
     */
    LAMBDA,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                        وصول_حقل | Field Access
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * وصول لحقل
     * كائن |> .حقل
     */
    FIELD_ACCESS,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                      استدعاء_طريقة | Method Call
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * استدعاء طريقة
     * كائن |> .طريقة(معاملات)
     */
    METHOD_CALL,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                        عملية_كسولة | Lazy Operation
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * عملية كسولة على مكرر
     * متكرر |> صفّي(ش) |> حوّل(د)
     */
    LAZY_OPERATION,
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                      عملية_متوازية | Parallel Operation
    // ═══════════════════════════════════════════════════════════════════════════
    /**
     * عملية متوازية
     * قائمة |> حوّل_متوازي(د)
     */
    PARALLEL_OPERATION
};

/**
 * 📦 عملية_الأنبوب | PipelineOperation
 * 
 * تمثيل عملية واحدة في سلسلة الأنبوب
 */
struct PipelineOperation {
    PipelineOperationType type;
    
    // للاستدعاءات
    std::string functionName;
    std::vector<std::string> arguments;
    int placeholderPosition = 0;  // -1 = آخر معامل
    
    // للـ lambda
    std::vector<std::string> lambdaParams;
    std::string lambdaBody;
    
    // للحقول والطرق
    std::string memberName;
    
    // معلومات النوع
    std::string inputType;
    std::string outputType;
    
    // علامات التحسين
    bool canInline = true;
    bool isPure = true;  // بدون آثار جانبية
};

/**
 * 🔗 سلسلة_الأنبوب | PipelineChain
 * 
 * سلسلة كاملة من عمليات الأنبوب
 */
struct PipelineChain {
    std::string sourceExpr;       // التعبير المصدر
    std::string sourceType;       // نوع المصدر
    std::vector<PipelineOperation> operations;
    std::string finalType;        // النوع النهائي
    
    // علامات التحسين
    bool isLazy = false;          // سلسلة كسولة
    bool canFuse = true;          // يمكن دمج العمليات
    bool hasPlaceholders = false; // تحتوي على _
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         سياق التوليد | Generation Context
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📝 سياق_التوليد | GenerationContext
 * 
 * يحفظ معلومات السياق أثناء التوليد
 */
class GenerationContext {
private:
    int tempCounter_ = 0;
    int labelCounter_ = 0;
    std::stringstream code_;
    std::vector<std::string> allocations_;
    
public:
    /**
     * إنشاء اسم متغير مؤقت | Create temp variable name
     */
    std::string newTemp(const std::string& prefix = "tmp") {
        return "%" + prefix + std::to_string(tempCounter_++);
    }
    
    /**
     * إنشاء اسم تسمية | Create label name
     */
    std::string newLabel(const std::string& prefix = "label") {
        return prefix + std::to_string(labelCounter_++);
    }
    
    /**
     * إضافة كود | Add code
     */
    void emit(const std::string& s) {
        code_ << s << "\n";
    }
    
    /**
     * إضافة تعليق | Add comment
     */
    void comment(const std::string& s) {
        code_ << "  ; " << s << "\n";
    }
    
    /**
     * الحصول على الكود المُنتج | Get generated code
     */
    std::string getCode() const {
        return code_.str();
    }
    
    /**
     * تسجيل تخصيص ذاكرة | Register allocation
     */
    void registerAllocation(const std::string& name) {
        allocations_.push_back(name);
    }
    
    /**
     * إعادة تعيين | Reset
     */
    void reset() {
        tempCounter_ = 0;
        labelCounter_ = 0;
        code_.str("");
        code_.clear();
        allocations_.clear();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         مولد كود الأنبوب | Pipeline Codegen
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 🔧 مولد_كود_الأنبوب | PipelineCodegen
 * 
 * يولد كود LLVM IR لعمليات الأنبوب
 */
class PipelineCodegen {
private:
    GenerationContext ctx_;
    
    // خريطة أسماء الدوال العربية → الداخلية
    std::unordered_map<std::string, std::string> functionNameMap_ = {
        {"طول", "sad_len"},
        {"اطبع", "sad_print"},
        {"حوّل", "sad_map"},
        {"صفّي", "sad_filter"},
        {"خذ", "sad_take"},
        {"أسقط", "sad_drop"},
        {"اجمع", "sad_collect"},
        {"طيّ", "sad_fold"},
        {"مدى", "sad_range"},
        {"عكس", "sad_reverse"},
        {"رتّب", "sad_sort"},
        {"فريد", "sad_unique"},
        {"سطّح", "sad_flatten"},
        {"دمج", "sad_zip"},
        {"عدّد", "sad_enumerate"},
        {"أول", "sad_first"},
        {"آخر", "sad_last"},
        {"أي", "sad_any"},
        {"كل", "sad_all"},
        {"عد", "sad_count"},
        {"جد", "sad_find"},
        {"موضع", "sad_position"}
    };
    
public:
    // ═══════════════════════════════════════════════════════════════════════════
    //                        التوليد الرئيسي | Main Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد كود لسلسلة أنبوب | Generate code for pipeline chain
     * 
     * @param chain سلسلة الأنبوب
     * @return الكود المُنتج
     */
    std::string generate(const PipelineChain& chain) {
        ctx_.reset();
        
        ctx_.comment("═══ بداية سلسلة الأنبوب ═══");
        ctx_.comment("المصدر: " + chain.sourceExpr);
        ctx_.comment("النوع: " + chain.sourceType + " → " + chain.finalType);
        
        // تحديد استراتيجية التوليد
        if (chain.isLazy) {
            return generateLazyChain(chain);
        } else if (chain.operations.size() <= 3 && chain.canFuse) {
            return generateFusedChain(chain);
        } else {
            return generateSequentialChain(chain);
        }
    }
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    //                      التوليد التسلسلي | Sequential Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد سلسلة تسلسلية | Generate sequential chain
     * 
     * كل عملية تُنتج نتيجة تُستخدم كمدخل للتالية
     */
    std::string generateSequentialChain(const PipelineChain& chain) {
        std::string currentValue = chain.sourceExpr;
        std::string currentType = chain.sourceType;
        
        for (size_t i = 0; i < chain.operations.size(); ++i) {
            const auto& op = chain.operations[i];
            
            ctx_.comment("عملية " + std::to_string(i + 1) + ": " + op.functionName);
            
            std::string result = ctx_.newTemp("pipe_result");
            
            switch (op.type) {
                case PipelineOperationType::FUNCTION_CALL:
                    currentValue = generateFunctionCall(op, currentValue, currentType);
                    break;
                    
                case PipelineOperationType::CALL_WITH_PLACEHOLDER:
                    currentValue = generateCallWithPlaceholder(op, currentValue, currentType);
                    break;
                    
                case PipelineOperationType::LAMBDA:
                    currentValue = generateLambdaCall(op, currentValue, currentType);
                    break;
                    
                case PipelineOperationType::METHOD_CALL:
                    currentValue = generateMethodCall(op, currentValue, currentType);
                    break;
                    
                case PipelineOperationType::FIELD_ACCESS:
                    currentValue = generateFieldAccess(op, currentValue, currentType);
                    break;
                    
                default:
                    ctx_.comment("عملية غير مدعومة");
                    break;
            }
            
            currentType = op.outputType;
        }
        
        ctx_.comment("═══ نهاية سلسلة الأنبوب ═══");
        ctx_.comment("النتيجة: " + currentValue);
        
        return ctx_.getCode();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                       التوليد المدمج | Fused Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد سلسلة مدمجة | Generate fused chain
     * 
     * دمج العمليات في استدعاء واحد عندما يكون ذلك ممكناً
     */
    std::string generateFusedChain(const PipelineChain& chain) {
        ctx_.comment("═══ توليد مدمج ═══");
        
        // بناء تعبير متداخل
        std::stringstream nested;
        
        // نبدأ من النهاية
        for (int i = chain.operations.size() - 1; i >= 0; --i) {
            const auto& op = chain.operations[i];
            std::string funcName = translateFunctionName(op.functionName);
            
            if (i == chain.operations.size() - 1) {
                // آخر عملية
                nested << "call " << op.outputType << " @" << funcName << "(";
            } else {
                // عملية وسيطة
                std::string prevOutput = chain.operations[i + 1].inputType;
                nested << "call " << prevOutput << " @" << funcName << "(";
            }
        }
        
        // المصدر في المنتصف
        nested << chain.sourceExpr;
        
        // إغلاق الأقواس
        for (size_t i = 0; i < chain.operations.size(); ++i) {
            nested << ")";
        }
        
        ctx_.emit("  " + nested.str());
        
        return ctx_.getCode();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                       التوليد الكسول | Lazy Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد سلسلة كسولة | Generate lazy chain
     * 
     * ينشئ آلة حالة للتكرار الكسول
     */
    std::string generateLazyChain(const PipelineChain& chain) {
        ctx_.comment("═══ توليد كسول ═══");
        
        // إنشاء هيكل حالة المكرر
        std::string iteratorStruct = generateIteratorStruct(chain);
        
        // إنشاء دالة next
        std::string nextFunc = generateNextFunction(chain);
        
        // إنشاء دالة التهيئة
        std::string initFunc = generateInitFunction(chain);
        
        return iteratorStruct + "\n" + nextFunc + "\n" + initFunc + "\n" + ctx_.getCode();
    }
    
    /**
     * توليد هيكل المكرر | Generate iterator struct
     */
    std::string generateIteratorStruct(const PipelineChain& chain) {
        std::stringstream ss;
        std::string structName = "pipeline_iter_" + std::to_string(reinterpret_cast<uintptr_t>(&chain));
        
        ss << "; هيكل حالة المكرر الكسول\n";
        ss << "%." << structName << " = type {\n";
        ss << "  ptr,    ; مؤشر vtable\n";
        ss << "  i32,    ; حالة التنفيذ (0=جاهز، 1=يعمل، 2=منتهي)\n";
        ss << "  i32,    ; نقطة الاستئناف\n";
        ss << "  ptr,    ; المكرر المصدر\n";
        
        // إضافة حقول للعمليات الكسولة
        int fieldIndex = 4;
        for (const auto& op : chain.operations) {
            if (op.type == PipelineOperationType::LAZY_OPERATION) {
                if (op.functionName == "خذ" || op.functionName == "أسقط") {
                    ss << "  i64,    ; عداد " << op.functionName << "\n";
                    fieldIndex++;
                } else if (op.functionName == "صفّي" || op.functionName == "حوّل") {
                    ss << "  ptr,    ; مؤشر دالة " << op.functionName << "\n";
                    fieldIndex++;
                }
            }
        }
        
        ss << "}\n";
        
        return ss.str();
    }
    
    /**
     * توليد دالة next | Generate next function
     */
    std::string generateNextFunction(const PipelineChain& chain) {
        std::stringstream ss;
        std::string structName = "pipeline_iter_" + std::to_string(reinterpret_cast<uintptr_t>(&chain));
        
        ss << "; دالة الحصول على العنصر التالي\n";
        ss << "define { i1, " << chain.finalType << " } @" << structName << "_next(ptr %self) {\n";
        ss << "entry:\n";
        
        // تحميل الحالة
        ss << "  ; تحميل حالة المكرر\n";
        ss << "  %state_ptr = getelementptr %." << structName << ", ptr %self, i32 0, i32 1\n";
        ss << "  %state = load i32, ptr %state_ptr\n";
        ss << "  %is_done = icmp eq i32 %state, 2\n";
        ss << "  br i1 %is_done, label %return_none, label %get_next\n\n";
        
        ss << "get_next:\n";
        ss << "  ; الحصول على العنصر التالي من المصدر\n";
        ss << "  %source_ptr = getelementptr %." << structName << ", ptr %self, i32 0, i32 3\n";
        ss << "  %source = load ptr, ptr %source_ptr\n";
        
        // توليد كود كل عملية كسولة
        int opIndex = 0;
        for (const auto& op : chain.operations) {
            ss << "\n  ; عملية: " << op.functionName << "\n";
            
            if (op.functionName == "صفّي") {
                ss << "filter_" << opIndex << ":\n";
                ss << "  ; تحقق من شرط الترشيح\n";
                ss << "  %filter_fn_" << opIndex << " = load ptr, ptr %filter_fn_ptr_" << opIndex << "\n";
                ss << "  %keep_" << opIndex << " = call i1 %filter_fn_" << opIndex << "(ptr %current)\n";
                ss << "  br i1 %keep_" << opIndex << ", label %continue_" << opIndex << ", label %get_next\n";
                ss << "continue_" << opIndex << ":\n";
            } else if (op.functionName == "حوّل") {
                ss << "map_" << opIndex << ":\n";
                ss << "  ; تطبيق التحويل\n";
                ss << "  %map_fn_" << opIndex << " = load ptr, ptr %map_fn_ptr_" << opIndex << "\n";
                ss << "  %mapped_" << opIndex << " = call ptr %map_fn_" << opIndex << "(ptr %current)\n";
            } else if (op.functionName == "خذ") {
                ss << "take_" << opIndex << ":\n";
                ss << "  ; تحقق من الحد\n";
                ss << "  %count_ptr_" << opIndex << " = getelementptr %." << structName << ", ptr %self, i32 0, i32 " << (4 + opIndex) << "\n";
                ss << "  %count_" << opIndex << " = load i64, ptr %count_ptr_" << opIndex << "\n";
                ss << "  %limit_reached = icmp eq i64 %count_" << opIndex << ", 0\n";
                ss << "  br i1 %limit_reached, label %return_none, label %decrement_" << opIndex << "\n";
                ss << "decrement_" << opIndex << ":\n";
                ss << "  %new_count = sub i64 %count_" << opIndex << ", 1\n";
                ss << "  store i64 %new_count, ptr %count_ptr_" << opIndex << "\n";
            }
            
            opIndex++;
        }
        
        ss << "\nreturn_value:\n";
        ss << "  ; إرجاع القيمة\n";
        ss << "  %result = insertvalue { i1, " << chain.finalType << " } { i1 true, " << chain.finalType << " undef }, " << chain.finalType << " %final_value, 1\n";
        ss << "  ret { i1, " << chain.finalType << " } %result\n\n";
        
        ss << "return_none:\n";
        ss << "  ; إرجاع فارغ\n";
        ss << "  store i32 2, ptr %state_ptr\n";
        ss << "  ret { i1, " << chain.finalType << " } { i1 false, " << chain.finalType << " undef }\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    /**
     * توليد دالة التهيئة | Generate init function
     */
    std::string generateInitFunction(const PipelineChain& chain) {
        std::stringstream ss;
        std::string structName = "pipeline_iter_" + std::to_string(reinterpret_cast<uintptr_t>(&chain));
        
        ss << "; دالة تهيئة المكرر\n";
        ss << "define ptr @" << structName << "_init(ptr %source) {\n";
        ss << "entry:\n";
        ss << "  ; تخصيص ذاكرة للمكرر\n";
        ss << "  %iter = call ptr @malloc(i64 ptrtoint (%." << structName << "* getelementptr (%." << structName << ", ptr null, i32 1) to i64))\n";
        ss << "  \n";
        ss << "  ; تهيئة الحالة\n";
        ss << "  %state_ptr = getelementptr %." << structName << ", ptr %iter, i32 0, i32 1\n";
        ss << "  store i32 0, ptr %state_ptr\n";
        ss << "  \n";
        ss << "  ; تهيئة نقطة الاستئناف\n";
        ss << "  %resume_ptr = getelementptr %." << structName << ", ptr %iter, i32 0, i32 2\n";
        ss << "  store i32 0, ptr %resume_ptr\n";
        ss << "  \n";
        ss << "  ; حفظ المصدر\n";
        ss << "  %source_ptr = getelementptr %." << structName << ", ptr %iter, i32 0, i32 3\n";
        ss << "  store ptr %source, ptr %source_ptr\n";
        ss << "  \n";
        ss << "  ret ptr %iter\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                      توليد العمليات الفردية | Individual Operation Generation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد استدعاء دالة | Generate function call
     */
    std::string generateFunctionCall(
        const PipelineOperation& op,
        const std::string& input,
        const std::string& inputType
    ) {
        std::string result = ctx_.newTemp("call_result");
        std::string funcName = translateFunctionName(op.functionName);
        
        ctx_.emit("  " + result + " = call " + op.outputType + " @" + funcName + 
                  "(" + inputType + " " + input + ")");
        
        return result;
    }
    
    /**
     * توليد استدعاء مع placeholder | Generate call with placeholder
     */
    std::string generateCallWithPlaceholder(
        const PipelineOperation& op,
        const std::string& input,
        const std::string& inputType
    ) {
        std::string result = ctx_.newTemp("placeholder_result");
        std::string funcName = translateFunctionName(op.functionName);
        
        // بناء قائمة المعاملات مع إدراج القيمة في الموضع المناسب
        std::stringstream argsStr;
        bool first = true;
        int argIndex = 0;
        
        for (const auto& arg : op.arguments) {
            if (!first) argsStr << ", ";
            first = false;
            
            if (arg == "_" || argIndex == op.placeholderPosition) {
                argsStr << inputType << " " << input;
            } else {
                argsStr << arg;  // معامل عادي
            }
            argIndex++;
        }
        
        // إذا لم يكن هناك placeholder صريح، نضيف المدخل في النهاية
        if (op.placeholderPosition == -1) {
            if (!first) argsStr << ", ";
            argsStr << inputType << " " << input;
        }
        
        ctx_.emit("  " + result + " = call " + op.outputType + " @" + funcName + 
                  "(" + argsStr.str() + ")");
        
        return result;
    }
    
    /**
     * توليد استدعاء lambda | Generate lambda call
     */
    std::string generateLambdaCall(
        const PipelineOperation& op,
        const std::string& input,
        const std::string& inputType
    ) {
        std::string result = ctx_.newTemp("lambda_result");
        
        // توليد دالة lambda مضمنة
        std::string lambdaName = ctx_.newTemp("lambda_").substr(1);  // إزالة %
        
        ctx_.comment("تطبيق lambda: " + op.lambdaBody);
        
        // للتبسيط، نفترض أن الـ lambda بسيط ويمكن تضمينه
        // في التنفيذ الفعلي، يجب توليد دالة منفصلة
        
        if (!op.lambdaParams.empty()) {
            std::string param = op.lambdaParams[0];
            // استبدال المعامل بالقيمة الفعلية
            std::string body = op.lambdaBody;
            // هنا يجب تحليل وتوليد كود الـ body
        }
        
        // استدعاء الـ lambda المُولّد
        ctx_.emit("  " + result + " = call " + op.outputType + " @" + lambdaName + 
                  "(" + inputType + " " + input + ")");
        
        return result;
    }
    
    /**
     * توليد استدعاء طريقة | Generate method call
     */
    std::string generateMethodCall(
        const PipelineOperation& op,
        const std::string& input,
        const std::string& inputType
    ) {
        std::string result = ctx_.newTemp("method_result");
        
        ctx_.comment("استدعاء طريقة: " + op.memberName);
        
        // الحصول على vtable
        std::string vtable = ctx_.newTemp("vtable");
        ctx_.emit("  " + vtable + " = load ptr, ptr " + input);
        
        // الحصول على مؤشر الطريقة من vtable
        std::string methodPtr = ctx_.newTemp("method_ptr");
        // هنا نحتاج لمعرفة فهرس الطريقة في vtable
        
        // استدعاء الطريقة
        ctx_.emit("  " + result + " = call " + op.outputType + " " + methodPtr + 
                  "(" + inputType + " " + input + ")");
        
        return result;
    }
    
    /**
     * توليد وصول حقل | Generate field access
     */
    std::string generateFieldAccess(
        const PipelineOperation& op,
        const std::string& input,
        const std::string& inputType
    ) {
        std::string result = ctx_.newTemp("field_result");
        
        ctx_.comment("وصول حقل: " + op.memberName);
        
        // حساب إزاحة الحقل
        std::string fieldPtr = ctx_.newTemp("field_ptr");
        // هنا نحتاج لمعرفة إزاحة الحقل في الهيكل
        ctx_.emit("  " + fieldPtr + " = getelementptr %" + inputType + 
                  ", ptr " + input + ", i32 0, i32 " + "0");  // فهرس الحقل
        
        // تحميل قيمة الحقل
        ctx_.emit("  " + result + " = load " + op.outputType + ", ptr " + fieldPtr);
        
        return result;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //                          أدوات مساعدة | Utilities
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * ترجمة اسم الدالة | Translate function name
     */
    std::string translateFunctionName(const std::string& name) {
        auto it = functionNameMap_.find(name);
        if (it != functionNameMap_.end()) {
            return it->second;
        }
        // إذا لم يكن اسماً عربياً معروفاً، نفترض أنه اسم داخلي
        return name;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                         محسّن الأنبوب | Pipeline Optimizer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * ⚡ محسّن_الأنبوب | PipelineOptimizer
 * 
 * يُحسّن سلسلة الأنبوب قبل التوليد
 */

} // namespace codegen
} // namespace compiler
} // namespace sad

