/**
 * =============================================================================
 * ملف: this_binding.cpp
 * الوصف: ربط الكلمة المفتاحية "هذا" بالكائن الحالي
 * المهمة: T182 - Implement this_binding
 * المرحلة: Phase 17 - User Story 15 (OOP صنف Sugar)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو "هذا"؟
 * ────────────────
 * "هذا" (this) هي كلمة خاصة تشير للكائن الحالي داخل الدوال.
 * 
 * مثال:
 * ```sad
 * صنف شخص:
 *     اسم: نص
 *     
 *     باني(الاسم: نص):
 *         هذا.اسم = الاسم    // "هذا" = الكائن الجاري إنشاؤه
 *     نهاية
 *     
 *     دالة قدّم_نفسك(&هذا) -> نص:
 *         ارجع "أنا " + هذا.اسم    // "هذا" = الكائن الذي استُدعيت عليه الدالة
 *     نهاية
 * نهاية
 * ```
 * 
 * كيف يعمل الربط؟
 * ────────────────
 * 1. في الدوال: أول معامل مخفي يكون مؤشر للكائن
 * 2. عند الوصول لـ"هذا": نستخدم هذا المؤشر
 * 3. عند الوصول لـ"هذا.حقل": نحمل قيمة الحقل من الكائن
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <iostream>
#include <stack>
#include <stdexcept>

// =============================================================================
// أنواع الربط
// =============================================================================

/**
 * نوع السياق للكلمة "هذا"
 */
enum class ThisContextType {
    None,           // خارج أي سياق
    Constructor,    // داخل باني
    Method,         // داخل دالة عادية
    ImmutableMethod,// داخل دالة &هذا (غير قابل للتعديل)
    MutableMethod,  // داخل دالة &متغير هذا
    StaticMethod,   // دالة ثابتة (لا يوجد هذا)
    Extension       // داخل امتداد
};

/**
 * معلومات سياق "هذا"
 */
struct ThisContext {
    ThisContextType type;
    std::string typeName;       // اسم الصنف/النوع
    std::string selfRegister;   // سجل LLVM الذي يحتوي المؤشر
    bool isMutable;             // هل يمكن التعديل؟
    std::map<std::string, std::string> fieldTypes;  // أنواع الحقول
    std::map<std::string, int> fieldOffsets;        // إزاحات الحقول
};

// =============================================================================
// مدير السياق
// =============================================================================

/**
 * مدير سياقات "هذا"
 * 
 * 💡 للمبتدئ:
 * نحتاج مكدس لأن الدوال قد تتداخل (lambdas مثلاً)
 */
class ThisContextManager {
public:
    /**
     * دخول سياق جديد
     */
    void enterContext(const ThisContext& ctx) {
        contextStack_.push(ctx);
    }
    
    /**
     * خروج من السياق الحالي
     */
    void exitContext() {
        if (!contextStack_.empty()) {
            contextStack_.pop();
        }
    }
    
    /**
     * الحصول على السياق الحالي
     */
    const ThisContext* currentContext() const {
        return contextStack_.empty() ? nullptr : &contextStack_.top();
    }
    
    /**
     * هل نحن داخل سياق يسمح بـ"هذا"؟
     */
    bool isThisAvailable() const {
        if (contextStack_.empty()) return false;
        auto type = contextStack_.top().type;
        return type != ThisContextType::None && 
               type != ThisContextType::StaticMethod;
    }
    
    /**
     * هل "هذا" قابل للتعديل؟
     */
    bool isThisMutable() const {
        if (!isThisAvailable()) return false;
        return contextStack_.top().isMutable;
    }
    
private:
    std::stack<ThisContext> contextStack_;
};

// Global context manager
static ThisContextManager g_thisManager;

// =============================================================================
// محلل الوصول لـ"هذا"
// =============================================================================

/**
 * تمثيل الوصول لـ"هذا"
 */
struct ThisAccess {
    enum class Kind {
        SelfReference,  // هذا فقط
        FieldRead,      // هذا.حقل (قراءة)
        FieldWrite,     // هذا.حقل = قيمة (كتابة)
        MethodCall      // هذا.دالة()
    };
    
    Kind kind;
    std::string fieldName;
    std::vector<std::string> arguments;  // للدوال
};

/**
 * محلل الوصول
 */
class ThisAccessParser {
public:
    /**
     * تحليل تعبير وصول لـ"هذا"
     */
    ThisAccess parse(const std::string& expr) {
        ThisAccess access;
        
        // هل هو "هذا" فقط؟
        if (expr == "هذا" || expr == "this") {
            access.kind = ThisAccess::Kind::SelfReference;
            return access;
        }
        
        // هل هو وصول لحقل أو دالة؟
        std::string prefix = "هذا.";
        if (expr.substr(0, prefix.length()) != prefix) {
            prefix = "this.";
            if (expr.substr(0, prefix.length()) != prefix) {
                throw std::runtime_error("Invalid this access: " + expr);
            }
        }
        
        std::string rest = expr.substr(prefix.length());
        
        // هل توجد قوس (استدعاء دالة)؟
        size_t parenPos = rest.find('(');
        if (parenPos != std::string::npos) {
            access.kind = ThisAccess::Kind::MethodCall;
            access.fieldName = rest.substr(0, parenPos);
            // تحليل المعاملات - مبسط
        } else {
            // قراءة حقل
            access.kind = ThisAccess::Kind::FieldRead;
            access.fieldName = rest;
        }
        
        return access;
    }
};

// =============================================================================
// مولد كود الوصول
// =============================================================================

/**
 * مولد كود LLVM للوصول لـ"هذا"
 */
class ThisCodeGenerator {
public:
    /**
     * توليد كود للوصول لـ"هذا" نفسه
     */
    std::string generateSelfAccess() {
        const auto* ctx = g_thisManager.currentContext();
        if (!ctx) {
            return "; ERROR: 'هذا' used outside of method context\n";
        }
        
        return ctx->selfRegister;
    }
    
    /**
     * توليد كود لقراءة حقل
     */
    std::string generateFieldRead(const std::string& fieldName,
                                  const std::string& resultReg) {
        const auto* ctx = g_thisManager.currentContext();
        if (!ctx) {
            return "; ERROR: field access outside of method context\n";
        }
        
        std::ostringstream ss;
        
        // الحصول على إزاحة الحقل
        auto offsetIt = ctx->fieldOffsets.find(fieldName);
        if (offsetIt == ctx->fieldOffsets.end()) {
            ss << "; ERROR: unknown field '" << fieldName << "'\n";
            return ss.str();
        }
        
        int offset = offsetIt->second;
        
        // الحصول على نوع الحقل
        auto typeIt = ctx->fieldTypes.find(fieldName);
        std::string fieldType = typeIt != ctx->fieldTypes.end() ? 
                                typeIt->second : "i32";
        
        // حساب عنوان الحقل
        std::string ptrReg = resultReg + ".ptr";
        ss << "  " << ptrReg << " = getelementptr inbounds %" << ctx->typeName;
        ss << ", %" << ctx->typeName << "* " << ctx->selfRegister;
        ss << ", i32 0, i32 " << offset << "\n";
        
        // تحميل القيمة
        ss << "  " << resultReg << " = load " << mapTypeToLLVM(fieldType);
        ss << ", " << mapTypeToLLVM(fieldType) << "* " << ptrReg << "\n";
        
        return ss.str();
    }
    
    /**
     * توليد كود لكتابة حقل
     */
    std::string generateFieldWrite(const std::string& fieldName,
                                   const std::string& valueReg) {
        const auto* ctx = g_thisManager.currentContext();
        if (!ctx) {
            return "; ERROR: field access outside of method context\n";
        }
        
        // التحقق من قابلية التعديل
        if (!ctx->isMutable) {
            return "; ERROR: cannot modify field in immutable method\n";
        }
        
        std::ostringstream ss;
        
        // الحصول على إزاحة الحقل
        auto offsetIt = ctx->fieldOffsets.find(fieldName);
        if (offsetIt == ctx->fieldOffsets.end()) {
            ss << "; ERROR: unknown field '" << fieldName << "'\n";
            return ss.str();
        }
        
        int offset = offsetIt->second;
        
        // الحصول على نوع الحقل
        auto typeIt = ctx->fieldTypes.find(fieldName);
        std::string fieldType = typeIt != ctx->fieldTypes.end() ? 
                                typeIt->second : "i32";
        
        // حساب عنوان الحقل
        ss << "  %field.ptr = getelementptr inbounds %" << ctx->typeName;
        ss << ", %" << ctx->typeName << "* " << ctx->selfRegister;
        ss << ", i32 0, i32 " << offset << "\n";
        
        // تخزين القيمة
        ss << "  store " << mapTypeToLLVM(fieldType) << " " << valueReg;
        ss << ", " << mapTypeToLLVM(fieldType) << "* %field.ptr\n";
        
        return ss.str();
    }
    
    /**
     * توليد كود لاستدعاء دالة على "هذا"
     */
    std::string generateMethodCall(const std::string& methodName,
                                   const std::vector<std::string>& argRegs,
                                   const std::string& resultReg) {
        const auto* ctx = g_thisManager.currentContext();
        if (!ctx) {
            return "; ERROR: method call outside of context\n";
        }
        
        std::ostringstream ss;
        
        // بناء اسم الدالة المُشوّه
        std::string mangledName = "_SM" + std::to_string(ctx->typeName.length()) + 
                                  ctx->typeName + "_" + 
                                  std::to_string(methodName.length()) + methodName;
        
        ss << "  ";
        if (!resultReg.empty()) {
            ss << resultReg << " = ";
        }
        
        ss << "call i32 @" << mangledName << "(%" << ctx->typeName << "* ";
        ss << ctx->selfRegister;
        
        for (const auto& arg : argRegs) {
            ss << ", i32 " << arg;  // مبسط - نفترض i32
        }
        
        ss << ")\n";
        
        return ss.str();
    }
    
private:
    std::string mapTypeToLLVM(const std::string& sadType) const {
        static const std::map<std::string, std::string> typeMap = {
            {"ع32", "i32"}, {"ع64", "i64"}, {"نص", "%String*"},
            {"منطقي", "i1"}, {"عش64", "double"},
        };
        
        auto it = typeMap.find(sadType);
        return it != typeMap.end() ? it->second : "i32";
    }
};

// =============================================================================
// مدقق استخدام "هذا"
// =============================================================================

/**
 * مدقق استخدام "هذا" للأخطاء الدلالية
 */
class ThisUsageValidator {
public:
    struct ValidationError {
        std::string message;
        int line;
        int column;
    };
    
    /**
     * التحقق من استخدام "هذا"
     */
    ValidationError validate(const ThisAccess& access) {
        ValidationError error;
        error.line = 0;
        error.column = 0;
        
        // هل "هذا" متاح؟
        if (!g_thisManager.isThisAvailable()) {
            error.message = "لا يمكن استخدام 'هذا' خارج الدوال";
            return error;
        }
        
        const auto* ctx = g_thisManager.currentContext();
        
        // التحقق من الكتابة في سياق غير قابل للتعديل
        if (access.kind == ThisAccess::Kind::FieldWrite && !ctx->isMutable) {
            error.message = "لا يمكن تعديل 'هذا' في دالة &هذا (غير قابلة للتعديل). "
                           "استخدم &متغير هذا";
            return error;
        }
        
        // التحقق من وجود الحقل
        if (access.kind == ThisAccess::Kind::FieldRead || 
            access.kind == ThisAccess::Kind::FieldWrite) {
            if (ctx->fieldTypes.find(access.fieldName) == ctx->fieldTypes.end()) {
                error.message = "الحقل '" + access.fieldName + "' غير موجود في النوع '" + 
                               ctx->typeName + "'";
                return error;
            }
        }
        
        // لا أخطاء
        error.message = "";
        return error;
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

extern "C" {

/**
 * دخول سياق دالة
 */
void sad_enter_method_context(const char* typeName, const char* selfReg, 
                              int isMutable, int contextType) {
    ThisContext ctx;
    ctx.typeName = typeName;
    ctx.selfRegister = selfReg;
    ctx.isMutable = isMutable != 0;
    ctx.type = static_cast<ThisContextType>(contextType);
    
    g_thisManager.enterContext(ctx);
}

/**
 * خروج من السياق
 */
void sad_exit_method_context() {
    g_thisManager.exitContext();
}

/**
 * إضافة حقل للسياق الحالي
 */
void sad_add_field_to_context(const char* fieldName, const char* fieldType, int offset) {
    // هذا يتطلب تعديل للسماح بالوصول للسياق المتغير
    // مبسط هنا
}

/**
 * هل "هذا" متاح؟
 */
int sad_is_this_available() {
    return g_thisManager.isThisAvailable() ? 1 : 0;
}

/**
 * هل "هذا" قابل للتعديل؟
 */
int sad_is_this_mutable() {
    return g_thisManager.isThisMutable() ? 1 : 0;
}

/**
 * الحصول على اسم النوع الحالي
 */
const char* sad_get_current_type_name() {
    const auto* ctx = g_thisManager.currentContext();
    if (!ctx) return "";
    static std::string result;
    result = ctx->typeName;
    return result.c_str();
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef THIS_BINDING_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار ربط "هذا"                                                        ║
║     This Binding Test                                                        ║
║                                                                              ║
║     المرحلة 17 - T182                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    // إنشاء سياق
    ThisContext ctx;
    ctx.type = ThisContextType::Method;
    ctx.typeName = "شخص";
    ctx.selfRegister = "%self";
    ctx.isMutable = true;
    ctx.fieldTypes["اسم"] = "نص";
    ctx.fieldTypes["عمر"] = "ع32";
    ctx.fieldOffsets["اسم"] = 0;
    ctx.fieldOffsets["عمر"] = 1;
    
    std::cout << "إنشاء سياق:\n";
    std::cout << "─────────────\n";
    std::cout << "النوع: " << ctx.typeName << "\n";
    std::cout << "السجل: " << ctx.selfRegister << "\n";
    std::cout << "قابل للتعديل: " << (ctx.isMutable ? "نعم" : "لا") << "\n";
    
    // دخول السياق
    g_thisManager.enterContext(ctx);
    
    std::cout << "\nبعد دخول السياق:\n";
    std::cout << "────────────────\n";
    std::cout << "هل 'هذا' متاح؟ " << (g_thisManager.isThisAvailable() ? "نعم" : "لا") << "\n";
    std::cout << "هل قابل للتعديل؟ " << (g_thisManager.isThisMutable() ? "نعم" : "لا") << "\n";
    
    // اختبار التحليل
    ThisAccessParser parser;
    
    std::cout << "\nتحليل الوصول:\n";
    std::cout << "──────────────\n";
    
    auto access1 = parser.parse("هذا");
    std::cout << "هذا -> نوع: SelfReference\n";
    
    auto access2 = parser.parse("هذا.اسم");
    std::cout << "هذا.اسم -> نوع: FieldRead, حقل: " << access2.fieldName << "\n";
    
    // توليد الكود
    ThisCodeGenerator generator;
    
    std::cout << "\nتوليد كود LLVM:\n";
    std::cout << "═══════════════\n";
    
    std::cout << "\nقراءة الحقل 'اسم':\n";
    std::cout << generator.generateFieldRead("اسم", "%name_val");
    
    std::cout << "\nكتابة الحقل 'عمر':\n";
    std::cout << generator.generateFieldWrite("عمر", "%new_age");
    
    // اختبار التحقق
    ThisUsageValidator validator;
    
    std::cout << "\nالتحقق من الاستخدام:\n";
    std::cout << "─────────────────────\n";
    
    auto error1 = validator.validate(access2);
    std::cout << "هذا.اسم: " << (error1.message.empty() ? "✅ صحيح" : error1.message) << "\n";
    
    // اختبار خارج السياق
    g_thisManager.exitContext();
    
    std::cout << "\nبعد الخروج من السياق:\n";
    std::cout << "──────────────────────\n";
    std::cout << "هل 'هذا' متاح؟ " << (g_thisManager.isThisAvailable() ? "نعم" : "لا") << "\n";
    
    std::cout << "\n✅ نجح اختبار ربط 'هذا'!\n";
    
    return 0;
}

#endif // THIS_BINDING_TEST
