/**
 * =============================================================================
 * ملف: class_desugar.cpp
 * الوصف: تحويل صيغة صنف إلى هيكل + نفّذ
 * المهمة: T179 - Implement class desugaring to struct+impl
 * المرحلة: Phase 17 - User Story 15 (OOP صنف Sugar)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما معنى "Desugaring"؟
 * ────────────────────────
 * "Sugar" = سكّر = صيغة مختصرة لتسهيل الكتابة
 * "Desugaring" = إزالة السكّر = تحويل الصيغة المختصرة للصيغة الكاملة
 * 
 * مثال من الحياة:
 * - "أريد كوب قهوة" (مختصر/sugar)
 * - "أريد كوب من القهوة المحمصة المطحونة مع ماء ساخن" (كامل/desugared)
 * 
 * في لغة ص:
 * ─────────────
 * ```sad
 * // الصيغة المختصرة (صنف)
 * صنف شخص:
 *     اسم: نص
 *     عمر: ع32
 *     
 *     باني(الاسم: نص, العمر: ع32):
 *         هذا.اسم = الاسم
 *         هذا.عمر = العمر
 *     نهاية
 *     
 *     دالة قدّم():
 *         اطبع("أنا " + هذا.اسم)
 *     نهاية
 * نهاية
 * ```
 * 
 * تتحول إلى:
 * 
 * ```sad
 * // الصيغة الكاملة (هيكل + نفّذ)
 * هيكل شخص:
 *     اسم: نص
 *     عمر: ع32
 * نهاية
 * 
 * نفّذ شخص:
 *     دالة جديد(الاسم: نص, العمر: ع32) -> شخص:
 *         ارجع شخص {
 *             اسم: الاسم,
 *             عمر: العمر,
 *         }
 *     نهاية
 *     
 *     دالة قدّم(&هذا):
 *         اطبع("أنا " + هذا.اسم)
 *     نهاية
 * نهاية
 * ```
 * 
 * لماذا نفعل هذا؟
 * ─────────────────
 * 1. المترجم يفهم صيغة واحدة فقط (هيكل + نفّذ)
 * 2. يُبسّط عمل المترجم
 * 3. يحافظ على التوافق مع الكود القديم
 * 4. يسمح بدعم أكثر من صيغة للمستخدم
 * 
 * =============================================================================
 */

#include <string>
#include <cstring>
#include <vector>
#include <memory>
#include <map>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <functional>

// =============================================================================
// هياكل بيانات AST المبسطة
// =============================================================================

/**
 * نوع العضو في الصنف
 */
enum class MemberType {
    Field,          // حقل
    Method,         // دالة
    Constructor,    // باني
    StaticMethod,   // دالة ثابتة
    StaticField,    // حقل ثابت
};

/**
 * مُحدد الوصول
 */
enum class Visibility {
    Public,         // عام
    Private,        // خاص
    Protected,      // محمي
};

/**
 * معامل
 */
struct Parameter {
    std::string name;
    std::string type;
    bool hasDefault = false;
    std::string defaultValue;
    bool isReference = false;
    bool isMutable = false;
};

/**
 * حقل في الصنف
 */
struct ClassField {
    std::string name;
    std::string type;
    Visibility visibility = Visibility::Private;
    bool isStatic = false;
    bool hasDefault = false;
    std::string defaultValue;
};

/**
 * دالة في الصنف
 */
struct ClassMethod {
    std::string name;
    std::vector<Parameter> parameters;
    std::string returnType;
    std::vector<std::string> body;
    Visibility visibility = Visibility::Public;
    bool isStatic = false;
    bool isVirtual = false;
    bool isOverride = false;
    bool takesThis = true;  // هل تأخذ هذا كمعامل ضمني؟
};

/**
 * باني الصنف
 */
struct ClassConstructor {
    std::vector<Parameter> parameters;
    std::vector<std::string> body;
    std::map<std::string, std::string> fieldInitializers;  // حقل -> قيمة
    Visibility visibility = Visibility::Public;
};

/**
 * تعريف صنف كامل
 */
struct ClassDefinition {
    std::string name;
    std::string parentClass;
    std::vector<std::string> interfaces;  // سمات مُنفذة
    std::vector<std::string> typeParams;  // معاملات النوع
    std::vector<ClassField> fields;
    std::vector<ClassMethod> methods;
    std::vector<ClassConstructor> constructors;
    
    bool hasDefaultConstructor() const {
        for (const auto& ctor : constructors) {
            if (ctor.parameters.empty()) return true;
            bool allDefault = std::all_of(ctor.parameters.begin(), ctor.parameters.end(),
                [](const Parameter& p) { return p.hasDefault; });
            if (allDefault) return true;
        }
        return false;
    }
};

// =============================================================================
// هياكل الخرج (هيكل + نفّذ)
// =============================================================================

/**
 * حقل في هيكل
 */
struct StructField {
    std::string name;
    std::string type;
    Visibility visibility = Visibility::Public;
};

/**
 * تعريف هيكل
 */
struct StructDefinition {
    std::string name;
    std::vector<std::string> typeParams;
    std::vector<StructField> fields;
};

/**
 * دالة في كتلة نفّذ
 */
struct ImplMethod {
    std::string name;
    std::vector<Parameter> parameters;
    std::string returnType;
    std::vector<std::string> body;
    bool isStatic = false;
    
    /**
     * هل هذه دالة الإنشاء (جديد)؟
     */
    bool isConstructor() const {
        return name == "جديد" || name == "new";
    }
};

/**
 * كتلة نفّذ
 */
struct ImplBlock {
    std::string typeName;
    std::string traitName;  // فارغ إذا لم تكن تنفيذ سمة
    std::vector<std::string> typeParams;
    std::vector<ImplMethod> methods;
};

/**
 * نتيجة التحويل
 */
struct DesugarResult {
    StructDefinition structDef;
    ImplBlock implBlock;
    std::vector<ImplBlock> traitImpls;  // تنفيذات السمات
};

// =============================================================================
// مُحوّل الصنف (Desugarer)
// =============================================================================

/**
 * مُحوّل صنف إلى هيكل + نفّذ
 * 
 * 💡 للمبتدئ:
 * يأخذ تعريف صنف ويُنتج هيكل + كتلة نفّذ مكافئة
 */
class ClassDesugarer {
public:
    /**
     * تحويل صنف إلى هيكل + نفّذ
     */
    DesugarResult desugar(const ClassDefinition& classDef) {
        DesugarResult result;
        
        // 1. إنشاء الهيكل
        result.structDef = createStruct(classDef);
        
        // 2. إنشاء كتلة نفّذ الرئيسية
        result.implBlock = createImplBlock(classDef);
        
        // 3. إنشاء تنفيذات السمات
        for (const auto& iface : classDef.interfaces) {
            result.traitImpls.push_back(createTraitImpl(classDef, iface));
        }
        
        return result;
    }
    
    /**
     * توليد كود لغة ص
     */
    std::string generateCode(const DesugarResult& result) {
        std::ostringstream ss;
        
        // الهيكل
        ss << generateStructCode(result.structDef);
        ss << "\n";
        
        // كتلة نفّذ الرئيسية
        ss << generateImplCode(result.implBlock);
        
        // تنفيذات السمات
        for (const auto& traitImpl : result.traitImpls) {
            ss << "\n";
            ss << generateImplCode(traitImpl);
        }
        
        return ss.str();
    }
    
private:
    /**
     * إنشاء تعريف الهيكل
     */
    StructDefinition createStruct(const ClassDefinition& classDef) {
        StructDefinition structDef;
        structDef.name = classDef.name;
        structDef.typeParams = classDef.typeParams;
        
        // نسخ الحقول (غير الثابتة فقط)
        for (const auto& field : classDef.fields) {
            if (!field.isStatic) {
                StructField sf;
                sf.name = field.name;
                sf.type = field.type;
                sf.visibility = field.visibility;
                structDef.fields.push_back(sf);
            }
        }
        
        return structDef;
    }
    
    /**
     * إنشاء كتلة نفّذ
     */
    ImplBlock createImplBlock(const ClassDefinition& classDef) {
        ImplBlock impl;
        impl.typeName = classDef.name;
        impl.typeParams = classDef.typeParams;
        
        // تحويل البناة إلى دوال "جديد"
        for (size_t i = 0; i < classDef.constructors.size(); i++) {
            const auto& ctor = classDef.constructors[i];
            ImplMethod method = createConstructorMethod(classDef, ctor, i);
            impl.methods.push_back(method);
        }
        
        // إضافة باني افتراضي إذا لم يوجد
        if (classDef.constructors.empty()) {
            impl.methods.push_back(createDefaultConstructor(classDef));
        }
        
        // تحويل الدوال
        for (const auto& method : classDef.methods) {
            ImplMethod implMethod = createMethod(classDef, method);
            impl.methods.push_back(implMethod);
        }
        
        // إضافة الحقول الثابتة كدوال getter
        for (const auto& field : classDef.fields) {
            if (field.isStatic) {
                impl.methods.push_back(createStaticFieldGetter(field));
            }
        }
        
        return impl;
    }
    
    /**
     * تحويل باني إلى دالة "جديد"
     */
    ImplMethod createConstructorMethod(const ClassDefinition& classDef,
                                       const ClassConstructor& ctor,
                                       size_t index) {
        ImplMethod method;
        
        // اسم الدالة
        if (classDef.constructors.size() == 1) {
            method.name = "جديد";
        } else {
            // عدة بناة - نستخدم أسماء مختلفة
            if (ctor.parameters.empty()) {
                method.name = "جديد";
            } else {
                method.name = "جديد_" + std::to_string(index + 1);
            }
        }
        
        method.parameters = ctor.parameters;
        method.returnType = classDef.name;
        method.isStatic = true;  // دوال الإنشاء ثابتة
        
        // بناء جسم الدالة
        std::ostringstream body;
        body << "ارجع " << classDef.name << " {\n";
        
        // تهيئة الحقول
        for (const auto& field : classDef.fields) {
            if (field.isStatic) continue;
            
            body << "    " << field.name << ": ";
            
            // البحث عن تهيئة صريحة
            auto it = ctor.fieldInitializers.find(field.name);
            if (it != ctor.fieldInitializers.end()) {
                body << it->second;
            } else if (field.hasDefault) {
                body << field.defaultValue;
            } else {
                // البحث عن معامل بنفس الاسم
                bool found = false;
                for (const auto& param : ctor.parameters) {
                    if (param.name == field.name || 
                        param.name == "ال" + field.name) {
                        body << param.name;
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    body << "افتراضي()";  // قيمة افتراضية للنوع
                }
            }
            
            body << ",\n";
        }
        
        body << "}";
        method.body.push_back(body.str());
        
        // إضافة أوامر الباني الإضافية
        for (const auto& stmt : ctor.body) {
            // تحويل هذا.حقل إلى الصيغة الجديدة
            std::string converted = convertThisReferences(stmt);
            // تخطي تهيئة الحقول (تمت في الأعلى)
            if (converted.find("هذا.") == std::string::npos) {
                method.body.push_back(converted);
            }
        }
        
        return method;
    }
    
    /**
     * إنشاء باني افتراضي
     */
    ImplMethod createDefaultConstructor(const ClassDefinition& classDef) {
        ImplMethod method;
        method.name = "جديد";
        method.returnType = classDef.name;
        method.isStatic = true;
        
        std::ostringstream body;
        body << "ارجع " << classDef.name << " {\n";
        
        for (const auto& field : classDef.fields) {
            if (field.isStatic) continue;
            
            body << "    " << field.name << ": ";
            if (field.hasDefault) {
                body << field.defaultValue;
            } else {
                body << "افتراضي()";
            }
            body << ",\n";
        }
        
        body << "}";
        method.body.push_back(body.str());
        
        return method;
    }
    
    /**
     * تحويل دالة عادية
     */
    ImplMethod createMethod(const ClassDefinition& classDef,
                           const ClassMethod& classMethod) {
        ImplMethod method;
        method.name = classMethod.name;
        method.returnType = classMethod.returnType;
        method.isStatic = classMethod.isStatic;
        
        // إضافة &هذا أو &متغير هذا كأول معامل
        if (!classMethod.isStatic && classMethod.takesThis) {
            Parameter thisParam;
            thisParam.name = "هذا";
            thisParam.isReference = true;
            thisParam.isMutable = true;  // افتراضياً قابل للتعديل
            thisParam.type = classDef.name;
            method.parameters.push_back(thisParam);
        }
        
        // نسخ باقي المعاملات
        for (const auto& param : classMethod.parameters) {
            method.parameters.push_back(param);
        }
        
        // نسخ جسم الدالة مع تحويل المراجع
        for (const auto& stmt : classMethod.body) {
            method.body.push_back(convertThisReferences(stmt));
        }
        
        return method;
    }
    
    /**
     * إنشاء getter للحقل الثابت
     */
    ImplMethod createStaticFieldGetter(const ClassField& field) {
        ImplMethod method;
        method.name = field.name;
        method.returnType = field.type;
        method.isStatic = true;
        
        if (field.hasDefault) {
            method.body.push_back("ارجع " + field.defaultValue);
        } else {
            method.body.push_back("ارجع افتراضي()");
        }
        
        return method;
    }
    
    /**
     * إنشاء تنفيذ سمة
     */
    ImplBlock createTraitImpl(const ClassDefinition& classDef,
                              const std::string& traitName) {
        ImplBlock impl;
        impl.typeName = classDef.name;
        impl.traitName = traitName;
        impl.typeParams = classDef.typeParams;
        
        // البحث عن الدوال التي تنتمي لهذه السمة
        // (هذا مبسط - في التنفيذ الحقيقي نحتاج معلومات السمة)
        
        return impl;
    }
    
    /**
     * تحويل مراجع هذا
     * هذا.حقل -> (*هذا).حقل أو self.حقل
     */
    std::string convertThisReferences(const std::string& code) {
        // للبساطة، نبقي هذا. كما هي
        // في التنفيذ الكامل، قد نحتاج تحويل أكثر
        return code;
    }
    
    /**
     * توليد كود الهيكل
     */
    std::string generateStructCode(const StructDefinition& structDef) {
        std::ostringstream ss;
        
        ss << "هيكل " << structDef.name;
        
        // معاملات النوع
        if (!structDef.typeParams.empty()) {
            ss << "[";
            for (size_t i = 0; i < structDef.typeParams.size(); i++) {
                if (i > 0) ss << ", ";
                ss << structDef.typeParams[i];
            }
            ss << "]";
        }
        
        ss << ":\n";
        
        // الحقول
        for (const auto& field : structDef.fields) {
            ss << "    ";
            if (field.visibility == Visibility::Public) {
                ss << "عام ";
            } else if (field.visibility == Visibility::Protected) {
                ss << "محمي ";
            }
            ss << field.name << ": " << field.type << "\n";
        }
        
        ss << "نهاية\n";
        
        return ss.str();
    }
    
    /**
     * توليد كود كتلة نفّذ
     */
    std::string generateImplCode(const ImplBlock& impl) {
        std::ostringstream ss;
        
        if (impl.traitName.empty()) {
            ss << "نفّذ " << impl.typeName;
        } else {
            ss << "نفّذ " << impl.traitName << " لـ " << impl.typeName;
        }
        
        // معاملات النوع
        if (!impl.typeParams.empty()) {
            ss << "[";
            for (size_t i = 0; i < impl.typeParams.size(); i++) {
                if (i > 0) ss << ", ";
                ss << impl.typeParams[i];
            }
            ss << "]";
        }
        
        ss << ":\n";
        
        // الدوال
        for (const auto& method : impl.methods) {
            ss << generateMethodCode(method);
            ss << "\n";
        }
        
        ss << "نهاية\n";
        
        return ss.str();
    }
    
    /**
     * توليد كود دالة
     */
    std::string generateMethodCode(const ImplMethod& method) {
        std::ostringstream ss;
        
        ss << "    دالة " << method.name << "(";
        
        // المعاملات
        for (size_t i = 0; i < method.parameters.size(); i++) {
            if (i > 0) ss << ", ";
            const auto& param = method.parameters[i];
            
            if (param.name == "هذا") {
                ss << "&";
                if (param.isMutable) ss << "متغير ";
                ss << "هذا";
            } else {
                ss << param.name << ": ";
                if (param.isReference) ss << "&";
                if (param.isMutable) ss << "متغير ";
                ss << param.type;
                if (param.hasDefault) {
                    ss << " = " << param.defaultValue;
                }
            }
        }
        
        ss << ")";
        
        // نوع الإرجاع
        if (!method.returnType.empty() && method.returnType != "فراغ") {
            ss << " -> " << method.returnType;
        }
        
        ss << ":\n";
        
        // الجسم
        for (const auto& stmt : method.body) {
            // إضافة مسافة بادئة للأسطر المتعددة
            std::istringstream iss(stmt);
            std::string line;
            while (std::getline(iss, line)) {
                ss << "        " << line << "\n";
            }
        }
        
        ss << "    نهاية\n";
        
        return ss.str();
    }
};

// =============================================================================
// مُنشئ الصنف من AST
// =============================================================================

/**
 * بناء ClassDefinition من AST
 * (يُستخدم مع class_parser.cpp)
 */
class ClassBuilder {
public:
    /**
     * بناء صنف من عقدة AST
     */
    ClassDefinition buildFromSource(const std::string& source);
    
    /**
     * إضافة حقل
     */
    void addField(ClassDefinition& cls, const std::string& name, 
                  const std::string& type, Visibility vis = Visibility::Private) {
        ClassField field;
        field.name = name;
        field.type = type;
        field.visibility = vis;
        cls.fields.push_back(field);
    }
    
    /**
     * إضافة دالة
     */
    void addMethod(ClassDefinition& cls, const std::string& name,
                   const std::string& returnType,
                   const std::vector<Parameter>& params,
                   const std::vector<std::string>& body) {
        ClassMethod method;
        method.name = name;
        method.returnType = returnType;
        method.parameters = params;
        method.body = body;
        cls.methods.push_back(method);
    }
    
    /**
     * إضافة باني
     */
    void addConstructor(ClassDefinition& cls,
                       const std::vector<Parameter>& params,
                       const std::map<std::string, std::string>& inits,
                       const std::vector<std::string>& body) {
        ClassConstructor ctor;
        ctor.parameters = params;
        ctor.fieldInitializers = inits;
        ctor.body = body;
        cls.constructors.push_back(ctor);
    }
};

// =============================================================================
// واجهة C البرمجية
// =============================================================================

extern "C" {

/**
 * تحويل صنف إلى هيكل + نفّذ
 * 
 * @param class_source كود الصنف
 * @param output_out مؤشر لتخزين الكود الناتج
 * @param error_out مؤشر لتخزين رسالة الخطأ
 * @return 1 للنجاح، 0 للفشل
 */
int sad_desugar_class(const char* class_source, char** output_out, char** error_out) {
    try {
        // للبساطة، نبني صنف يدوياً للاختبار
        // في التنفيذ الكامل، نستخدم ClassParser
        
        ClassDefinition cls;
        cls.name = "TestClass";
        
        ClassDesugarer desugarer;
        auto result = desugarer.desugar(cls);
        
        std::string output = desugarer.generateCode(result);
        
        if (output_out) {
            // إصلاح: استخدام std::nothrow و strncpy
            *output_out = new (std::nothrow) char[output.size() + 1];
            if (*output_out) {
                std::strncpy(*output_out, output.c_str(), output.size());
                (*output_out)[output.size()] = '\0';
            }
        }
        
        return 1;
    }
    catch (const std::exception& e) {
        if (error_out) {
            std::string msg = std::string("خطأ: ") + e.what();
            // إصلاح: استخدام std::nothrow و strncpy
            *error_out = new (std::nothrow) char[msg.size() + 1];
            if (*error_out) {
                std::strncpy(*error_out, msg.c_str(), msg.size());
                (*error_out)[msg.size()] = '\0';
            }
        }
        return 0;
    }
}

/**
 * تحرير ذاكرة الخرج
 */
void sad_desugar_free(char* str) {
    if (str) delete[] str;
}

} // extern "C"

// =============================================================================
// دالة الاختبار
// =============================================================================

#ifdef CLASS_DESUGAR_TEST

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبار تحويل الصنف (Desugaring)                                         ║
║     Class Desugaring Test                                                    ║
║                                                                              ║
║     المرحلة 17 - T179                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    // إنشاء صنف للاختبار
    ClassDefinition cls;
    cls.name = "شخص";
    
    // إضافة الحقول
    ClassField nameField;
    nameField.name = "اسم";
    nameField.type = "نص";
    nameField.visibility = Visibility::Private;
    cls.fields.push_back(nameField);
    
    ClassField ageField;
    ageField.name = "عمر";
    ageField.type = "ع32";
    ageField.visibility = Visibility::Private;
    cls.fields.push_back(ageField);
    
    // إضافة باني
    ClassConstructor ctor;
    Parameter p1; p1.name = "الاسم"; p1.type = "نص";
    Parameter p2; p2.name = "العمر"; p2.type = "ع32"; p2.hasDefault = true; p2.defaultValue = "0";
    ctor.parameters = {p1, p2};
    ctor.fieldInitializers["اسم"] = "الاسم";
    ctor.fieldInitializers["عمر"] = "العمر";
    cls.constructors.push_back(ctor);
    
    // إضافة دالة
    ClassMethod method;
    method.name = "قدّم_نفسك";
    method.returnType = "نص";
    method.body.push_back("ارجع \"أنا \" + هذا.اسم");
    cls.methods.push_back(method);
    
    ClassMethod ageMethod;
    ageMethod.name = "عيد_ميلاد";
    ageMethod.returnType = "فراغ";
    ageMethod.body.push_back("هذا.عمر = هذا.عمر + 1");
    cls.methods.push_back(ageMethod);
    
    std::cout << "الصنف الأصلي:\n";
    std::cout << "═════════════════════\n";
    std::cout << "صنف " << cls.name << ":\n";
    std::cout << "    اسم: نص\n";
    std::cout << "    عمر: ع32\n";
    std::cout << "    \n";
    std::cout << "    باني(الاسم: نص, العمر: ع32 = 0):\n";
    std::cout << "        هذا.اسم = الاسم\n";
    std::cout << "        هذا.عمر = العمر\n";
    std::cout << "    نهاية\n";
    std::cout << "    \n";
    std::cout << "    دالة قدّم_نفسك() -> نص:\n";
    std::cout << "        ارجع \"أنا \" + هذا.اسم\n";
    std::cout << "    نهاية\n";
    std::cout << "نهاية\n\n";
    
    // تحويل الصنف
    ClassDesugarer desugarer;
    auto result = desugarer.desugar(cls);
    
    std::cout << "الكود المُحوَّل (Desugared):\n";
    std::cout << "═══════════════════════════════\n";
    std::cout << desugarer.generateCode(result);
    
    std::cout << "\n✅ تم التحويل بنجاح!\n";
    
    return 0;
}

#endif // CLASS_DESUGAR_TEST
