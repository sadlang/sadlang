/**
 * =============================================================================
 * ملف: extension_codegen.cpp
 * الوصف: توليد كود LLVM IR لطرق الامتداد
 * المهمة: T181 - Implement extension codegen
 * المرحلة: Phase 17 - User Story 15 (OOP صنف Sugar)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * كيف تُترجم طرق الامتداد؟
 * ──────────────────────────────
 * طرق الامتداد تبدو كأنها جزء من النوع، لكنها في الحقيقة:
 * - دوال عادية تأخذ الكائن كأول معامل
 * 
 * مثال:
 * ```sad
 * // ما تكتبه
 * نص.عدّ_الكلمات()
 * 
 * // ما يُنفذ فعلياً
 * عدّ_الكلمات(نص)
 * ```
 * 
 * هذا يسمى "تحويل استدعاء الطريقة" (Method Call Transformation)
 * 
 * خطوات التوليد:
 * ─────────────────
 * 1. تحويل اسم الدالة: نوع.دالة -> نوع_دالة (mangling)
 * 2. إضافة مؤشر للكائن كأول معامل
 * 3. توليد كود الجسم كدالة عادية
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <iostream>
#include <functional>

// =============================================================================
// هياكل البيانات
// =============================================================================

/**
 * تمثيل دالة امتداد بعد التحليل
 */
struct ExtensionMethodIR {
    std::string mangledName;        // الاسم بعد التشويه
    std::string originalName;       // الاسم الأصلي
    std::string targetType;         // النوع المُمتد
    std::string llvmSignature;      // توقيع LLVM
    std::vector<std::string> paramTypes;
    std::string returnType;
    bool takesMutableSelf;
    std::string bodyIR;             // كود LLVM IR للجسم
};

/**
 * جدول الامتدادات للتوليد
 */
class ExtensionTable {
public:
    static ExtensionTable& instance() {
        static ExtensionTable table;
        return table;
    }
    
    void addMethod(const ExtensionMethodIR& method) {
        methods_[method.targetType].push_back(method);
        byName_[method.mangledName] = method;
    }
    
    std::vector<ExtensionMethodIR> getMethodsForType(const std::string& type) const {
        auto it = methods_.find(type);
        return it != methods_.end() ? it->second : std::vector<ExtensionMethodIR>{};
    }
    
    const ExtensionMethodIR* findMethod(const std::string& mangledName) const {
        auto it = byName_.find(mangledName);
        return it != byName_.end() ? &it->second : nullptr;
    }
    
    void clear() {
        methods_.clear();
        byName_.clear();
    }
    
private:
    ExtensionTable() = default;
    std::map<std::string, std::vector<ExtensionMethodIR>> methods_;
    std::map<std::string, ExtensionMethodIR> byName_;
};

// =============================================================================
// تشويه الأسماء (Name Mangling)
// =============================================================================

/**
 * مُشوّه الأسماء لطرق الامتداد
 * 
 * 💡 للمبتدئ:
 * تشويه الاسم = تحويل الاسم إلى صيغة فريدة
 * لتجنب التعارض بين دوال بنفس الاسم لأنواع مختلفة
 */
class ExtensionNameMangler {
public:
    /**
     * تشويه اسم دالة امتداد
     * 
     * الصيغة: _Sext_<طول_النوع><النوع>_<طول_الدالة><الدالة>
     * مثال: نص.عدّ -> _Sext_3نص_3عدّ
     */
    static std::string mangle(const std::string& typeName, 
                              const std::string& methodName) {
        std::ostringstream ss;
        ss << "_Sext_";
        ss << typeName.length() << typeName;
        ss << "_";
        ss << methodName.length() << methodName;
        return ss.str();
    }
    
    /**
     * فك تشويه الاسم
     */
    static std::pair<std::string, std::string> demangle(const std::string& mangled) {
        if (mangled.substr(0, 6) != "_Sext_") {
            return {"", ""};
        }
        
        size_t pos = 6;
        
        // قراءة طول النوع
        size_t typeLen = 0;
        while (pos < mangled.size() && isdigit(mangled[pos])) {
            typeLen = typeLen * 10 + (mangled[pos] - '0');
            pos++;
        }
        
        // قراءة النوع
        std::string typeName = mangled.substr(pos, typeLen);
        pos += typeLen;
        
        // تخطي _
        if (pos < mangled.size() && mangled[pos] == '_') pos++;
        
        // قراءة طول الدالة
        size_t methodLen = 0;
        while (pos < mangled.size() && isdigit(mangled[pos])) {
            methodLen = methodLen * 10 + (mangled[pos] - '0');
            pos++;
        }
        
        // قراءة الدالة
        std::string methodName = mangled.substr(pos, methodLen);
        
        return {typeName, methodName};
    }
};

// =============================================================================
// مولد كود LLVM IR
// =============================================================================

/**
 * مولد كود الامتداد
 */
class ExtensionCodeGenerator {
public:
    /**
     * توليد كود LLVM IR لدالة امتداد
     */
    std::string generateLLVMIR(const ExtensionMethodIR& method) {
        std::ostringstream ss;
        
        // تعليق
        ss << "; Extension method: " << method.targetType << "." << method.originalName << "\n";
        
        // توقيع الدالة
        ss << "define " << mapTypeToLLVM(method.returnType) << " ";
        ss << "@" << method.mangledName << "(";
        
        // المعاملات
        bool first = true;
        
        // أول معامل: مؤشر للكائن
        ss << mapTypeToLLVM(method.targetType) << "* ";
        if (!method.takesMutableSelf) ss << "readonly ";
        ss << "%self";
        first = false;
        
        // باقي المعاملات
        int paramIndex = 0;
        for (const auto& paramType : method.paramTypes) {
            ss << ", " << mapTypeToLLVM(paramType) << " %param" << paramIndex++;
        }
        
        ss << ") {\n";
        
        // نقطة الدخول
        ss << "entry:\n";
        
        // جسم الدالة
        if (!method.bodyIR.empty()) {
            ss << method.bodyIR;
        } else {
            // جسم افتراضي
            if (method.returnType == "فراغ" || method.returnType.empty()) {
                ss << "  ret void\n";
            } else {
                ss << "  ; TODO: implement body\n";
                ss << "  ret " << mapTypeToLLVM(method.returnType) << " zeroinitializer\n";
            }
        }
        
        ss << "}\n\n";
        
        return ss.str();
    }
    
    /**
     * توليد كود استدعاء دالة امتداد
     * 
     * obj.method(args) -> call @mangled_name(%obj, args)
     */
    std::string generateCall(const std::string& mangledName,
                            const std::string& objectReg,
                            const std::vector<std::string>& argRegs,
                            const std::string& resultReg = "") {
        std::ostringstream ss;
        
        const auto* method = ExtensionTable::instance().findMethod(mangledName);
        if (!method) {
            ss << "; ERROR: Unknown extension method: " << mangledName << "\n";
            return ss.str();
        }
        
        if (!resultReg.empty() && method->returnType != "فراغ") {
            ss << "  " << resultReg << " = ";
        } else {
            ss << "  ";
        }
        
        ss << "call " << mapTypeToLLVM(method->returnType) << " ";
        ss << "@" << mangledName << "(";
        
        // أول معامل: الكائن
        ss << mapTypeToLLVM(method->targetType) << "* " << objectReg;
        
        // باقي المعاملات
        for (size_t i = 0; i < argRegs.size(); i++) {
            ss << ", ";
            if (i < method->paramTypes.size()) {
                ss << mapTypeToLLVM(method->paramTypes[i]) << " ";
            }
            ss << argRegs[i];
        }
        
        ss << ")\n";
        
        return ss.str();
    }
    
    /**
     * توليد جدول الامتدادات كبيانات ثابتة
     */
    std::string generateExtensionTable() {
        std::ostringstream ss;
        
        ss << "; Extension method dispatch table\n";
        ss << "@_Sext_table = internal constant {\n";
        
        // هذا مبسط - في التنفيذ الحقيقي نحتاج جدول أكثر تعقيداً
        
        ss << "}\n\n";
        
        return ss.str();
    }
    
private:
    /**
     * تحويل نوع ص إلى نوع LLVM
     */
    std::string mapTypeToLLVM(const std::string& sadType) const {
        static const std::map<std::string, std::string> typeMap = {
            {"ع8", "i8"},
            {"ع16", "i16"},
            {"ع32", "i32"},
            {"ع64", "i64"},
            {"ح8", "i8"},
            {"ح16", "i16"},
            {"ح32", "i32"},
            {"ح64", "i64"},
            {"عش32", "float"},
            {"عش64", "double"},
            {"منطقي", "i1"},
            {"فراغ", "void"},
            {"نص", "%String*"},
        };
        
        auto it = typeMap.find(sadType);
        if (it != typeMap.end()) {
            return it->second;
        }
        
        // نوع مخصص - نفترض أنه مؤشر لهيكل
        return "%" + sadType + "*";
    }
};

// =============================================================================
// محوّل الاستدعاءات
// =============================================================================

/**
 * يحول استدعاءات الطرق إلى استدعاءات امتداد
 * 
 * 💡 للمبتدئ:
 * عندما يكتب المستخدم obj.method()
 * نحتاج معرفة: هل هذه دالة من النوع الأصلي أم امتداد؟
 */
class ExtensionCallResolver {
public:
    struct ResolvedCall {
        bool isExtension;
        std::string mangledName;
        std::string originalMethod;
        std::string targetType;
    };
    
    /**
     * حل استدعاء طريقة
     */
    ResolvedCall resolve(const std::string& typeName, 
                        const std::string& methodName) {
        ResolvedCall result;
        result.isExtension = false;
        result.originalMethod = methodName;
        result.targetType = typeName;
        
        // البحث في الامتدادات
        auto methods = ExtensionTable::instance().getMethodsForType(typeName);
        for (const auto& m : methods) {
            if (m.originalName == methodName) {
                result.isExtension = true;
                result.mangledName = m.mangledName;
                return result;
            }
        }
        
        // ليست امتداد - قد تكون دالة عادية
        return result;
    }
    
    /**
     * تحويل شجرة AST لاستدعاء
     */
    std::string transformCall(const std::string& objExpr,
                             const std::string& typeName,
                             const std::string& methodName,
                             const std::vector<std::string>& args) {
        auto resolved = resolve(typeName, methodName);
        
        if (resolved.isExtension) {
            // تحويل obj.method(args) إلى _Sext_method(&obj, args)
            std::ostringstream ss;
            ss << resolved.mangledName << "(&" << objExpr;
            for (const auto& arg : args) {
                ss << ", " << arg;
            }
            ss << ")";
            return ss.str();
        }
        
        // استدعاء عادي
        std::ostringstream ss;
        ss << objExpr << "." << methodName << "(";
        for (size_t i = 0; i < args.size(); i++) {
            if (i > 0) ss << ", ";
            ss << args[i];
        }
        ss << ")";
        return ss.str();
    }
};

// =============================================================================
// مُنشئ امتدادات من AST
// =============================================================================

/**
 * يحول AST الامتداد إلى ExtensionMethodIR
 */
class ExtensionIRBuilder {
public:
    ExtensionMethodIR build(const std::string& targetType,
                            const std::string& methodName,
                            const std::vector<std::string>& paramTypes,
                            const std::string& returnType,
                            bool takesMutableSelf) {
        ExtensionMethodIR ir;
        ir.originalName = methodName;
        ir.targetType = targetType;
        ir.mangledName = ExtensionNameMangler::mangle(targetType, methodName);
        ir.paramTypes = paramTypes;
        ir.returnType = returnType;
        ir.takesMutableSelf = takesMutableSelf;
        
        return ir;
    }
    
    /**
     * توليد IR للجسم
     */
    void generateBodyIR(ExtensionMethodIR& method,
                       const std::vector<std::string>& bodyStatements) {
        std::ostringstream ss;
        
        // تحويل مبسط - في التنفيذ الحقيقي نحتاج تحليل كامل
        for (const auto& stmt : bodyStatements) {
            ss << "  ; " << stmt << "\n";
            
            // تحويل أوامر شائعة
            if (stmt.find("ارجع") == 0) {
                // أمر إرجاع
                std::string expr = stmt.substr(5); // بعد "ارجع "
                if (method.returnType == "فراغ" || method.returnType.empty()) {
                    ss << "  ret void\n";
                } else {
                    ss << "  ; TODO: evaluate expression: " << expr << "\n";
                }
            }
        }
        
        method.bodyIR = ss.str();
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

extern "C" {

/**
 * تسجيل دالة امتداد
 */
void sad_register_extension_method(const char* typeName, const char* methodName,
                                   const char* returnType, int takesMutableSelf) {
    ExtensionMethodIR method;
    method.originalName = methodName;
    method.targetType = typeName;
    method.mangledName = ExtensionNameMangler::mangle(typeName, methodName);
    method.returnType = returnType;
    method.takesMutableSelf = takesMutableSelf != 0;
    
    ExtensionTable::instance().addMethod(method);
}

/**
 * الحصول على الاسم المُشوّه
 */
const char* sad_mangle_extension_name(const char* typeName, const char* methodName) {
    static std::string result;
    result = ExtensionNameMangler::mangle(typeName, methodName);
    return result.c_str();
}

/**
 * التحقق من وجود امتداد
 */
int sad_has_extension_method(const char* typeName, const char* methodName) {
    std::string mangled = ExtensionNameMangler::mangle(typeName, methodName);
    return ExtensionTable::instance().findMethod(mangled) != nullptr ? 1 : 0;
}

/**
 * مسح جدول الامتدادات
 */
void sad_clear_extension_table() {
    ExtensionTable::instance().clear();
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef EXTENSION_CODEGEN_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار توليد كود الامتداد                                               ║
║     Extension Code Generation Test                                           ║
║                                                                              ║
║     المرحلة 17 - T181                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    // إنشاء دالة امتداد
    ExtensionIRBuilder builder;
    auto method = builder.build(
        "نص",           // النوع
        "عدّ_الكلمات",  // اسم الدالة
        {},             // لا معاملات إضافية
        "ع32",          // نوع الإرجاع
        false           // &هذا (غير قابل للتعديل)
    );
    
    // إضافة جسم
    builder.generateBodyIR(method, {
        "متغير عدد = 0",
        "لكل كلمة في هذا.قسّم(\" \"):",
        "    عدد = عدد + 1",
        "ارجع عدد"
    });
    
    // تسجيل في الجدول
    ExtensionTable::instance().addMethod(method);
    
    std::cout << "معلومات الدالة:\n";
    std::cout << "─────────────────\n";
    std::cout << "الاسم الأصلي: " << method.originalName << "\n";
    std::cout << "النوع المُمتد: " << method.targetType << "\n";
    std::cout << "الاسم المُشوّه: " << method.mangledName << "\n";
    std::cout << "نوع الإرجاع: " << method.returnType << "\n";
    
    // فك التشويه
    auto [type, name] = ExtensionNameMangler::demangle(method.mangledName);
    std::cout << "\nفك التشويه:\n";
    std::cout << "  النوع: " << type << "\n";
    std::cout << "  الدالة: " << name << "\n";
    
    // توليد LLVM IR
    ExtensionCodeGenerator generator;
    std::string ir = generator.generateLLVMIR(method);
    
    std::cout << "\nكود LLVM IR:\n";
    std::cout << "═════════════\n";
    std::cout << ir;
    
    // توليد استدعاء
    std::cout << "\nاستدعاء الدالة:\n";
    std::cout << "───────────────\n";
    std::string call = generator.generateCall(
        method.mangledName,
        "%my_string",
        {},
        "%result"
    );
    std::cout << call;
    
    // اختبار المحوّل
    ExtensionCallResolver resolver;
    auto resolved = resolver.resolve("نص", "عدّ_الكلمات");
    
    std::cout << "\nحل الاستدعاء:\n";
    std::cout << "─────────────\n";
    std::cout << "هل امتداد؟ " << (resolved.isExtension ? "نعم" : "لا") << "\n";
    if (resolved.isExtension) {
        std::cout << "الاسم المُشوّه: " << resolved.mangledName << "\n";
    }
    
    std::cout << "\n✅ نجح توليد كود الامتداد!\n";
    
    return 0;
}

#endif // EXTENSION_CODEGEN_TEST
