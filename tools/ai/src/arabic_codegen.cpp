/**
 * ==========================================================
 * ملف: arabic_codegen.cpp
 * الوصف: توليد الكود من الأوامر العربية
 * المهمة: T298 - نظام ذكاء ص
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * 
 * 🗣️ ما هو توليد الكود من اللغة الطبيعية؟
 * ----------------------------------------
 * تخيل أنك تقول للكمبيوتر:
 *   "اكتب دالة تجمع عددين"
 * والكمبيوتر يفهم ويكتب الكود لك!
 * 
 * 🤔 كيف يعمل؟
 * ------------
 * 1. تحليل الأمر: نفهم ماذا يريد المستخدم
 * 2. استخراج المعلومات: الاسم، المعاملات، النوع...
 * 3. توليد الكود: إنشاء كود ص صحيح
 * 
 * 📝 أمثلة:
 * ---------
 * الأمر: "اكتب دالة تحسب مضروب عدد"
 * النتيجة:
 *   دالة مضروب(ن: عدد) ← عدد
 *       إذا ن <= 1
 *           أرجع 1
 *       نهاية
 *       أرجع ن * مضروب(ن - 1)
 *   نهاية
 * 
 * الأمر: "أنشئ هيكل للطالب"
 * النتيجة:
 *   هيكل طالب
 *       الاسم: نص
 *       العمر: عدد
 *       المعدل: عشري
 *   نهاية
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <regex>
#include <functional>

namespace sad {
namespace ai {

// ==========================================================
// 📌 أنواع الكود المُولَّد
// ==========================================================

/**
 * 🏷️ نوع الكود
 */
enum class CodeType {
    FUNCTION,       // دالة
    STRUCT,         // هيكل
    VARIABLE,       // متغير
    LOOP,           // حلقة
    CONDITION,      // شرط
    CLASS,          // صنف
    TRAIT,          // سمة
    TEST,           // اختبار
    MODULE,         // وحدة
    PROGRAM         // برنامج كامل
};

// ==========================================================
// 📌 معلومات مستخرجة من الأمر
// ==========================================================

/**
 * 📋 معلومات الدالة
 */
struct FunctionInfo {
    std::string name = "";
    std::string arabicName = "";
    std::vector<std::pair<std::string, std::string>> params; // (اسم، نوع)
    std::string returnType = "فراغ";
    std::string description = "";
    bool isRecursive = false;
    bool isAsync = false;
};

/**
 * 📋 معلومات الهيكل
 */
struct StructInfo {
    std::string name = "";
    std::vector<std::pair<std::string, std::string>> fields; // (اسم، نوع)
    std::string description = "";
};

/**
 * 📋 معلومات المتغير
 */
struct VariableInfo {
    std::string name = "";
    std::string type = "عدد";
    std::string value = "";
    bool isMutable = true;
    bool isConstant = false;
};

// ==========================================================
// 📌 محلل الأوامر العربية
// ==========================================================

/**
 * 🔍 محلل الأوامر
 */
class CommandParser {
private:
    // كلمات مفتاحية للدوال
    std::vector<std::string> functionKeywords_ = {
        "اكتب دالة", "أنشئ دالة", "عرّف دالة", "اصنع دالة",
        "اكتب وظيفة", "أنشئ وظيفة"
    };
    
    // كلمات مفتاحية للهياكل
    std::vector<std::string> structKeywords_ = {
        "أنشئ هيكل", "اكتب هيكل", "عرّف هيكل",
        "أنشئ بنية", "اكتب بنية"
    };
    
    // كلمات مفتاحية للمتغيرات
    std::vector<std::string> variableKeywords_ = {
        "أنشئ متغير", "عرّف متغير", "اكتب متغير",
        "أنشئ ثابت", "عرّف ثابت"
    };
    
    // كلمات مفتاحية للحلقات
    std::vector<std::string> loopKeywords_ = {
        "اكتب حلقة", "أنشئ حلقة", "كرر"
    };
    
    // كلمات مفتاحية للشروط
    std::vector<std::string> conditionKeywords_ = {
        "اكتب شرط", "أنشئ شرط", "إذا"
    };
    
    // أنواع البيانات المعروفة
    std::map<std::string, std::string> typeMapping_ = {
        {"عدد", "عدد"}, {"أعداد", "عدد"},
        {"عشري", "عشري"}, {"أعشار", "عشري"},
        {"نص", "نص"}, {"نصوص", "نص"},
        {"منطقي", "منطقي"}, {"صح أو خطأ", "منطقي"},
        {"قائمة", "قائمة"}, {"مصفوفة", "قائمة"},
        {"قاموس", "قاموس"}, {"خريطة", "قاموس"}
    };
    
    // أفعال شائعة ومعانيها
    std::map<std::string, std::string> verbMapping_ = {
        {"تجمع", "جمع"}, {"يجمع", "جمع"}, {"جمع", "جمع"},
        {"تطرح", "طرح"}, {"يطرح", "طرح"}, {"طرح", "طرح"},
        {"تضرب", "ضرب"}, {"يضرب", "ضرب"}, {"ضرب", "ضرب"},
        {"تقسم", "قسمة"}, {"يقسم", "قسمة"}, {"قسمة", "قسمة"},
        {"تحسب", "حساب"}, {"يحسب", "حساب"}, {"حساب", "حساب"},
        {"تقرأ", "قراءة"}, {"يقرأ", "قراءة"}, {"قراءة", "قراءة"},
        {"تكتب", "كتابة"}, {"يكتب", "كتابة"}, {"كتابة", "كتابة"},
        {"تبحث", "بحث"}, {"يبحث", "بحث"}, {"بحث", "بحث"},
        {"ترتب", "ترتيب"}, {"يرتب", "ترتيب"}, {"ترتيب", "ترتيب"},
        {"تحول", "تحويل"}, {"يحول", "تحويل"}, {"تحويل", "تحويل"}
    };
    
public:
    /**
     * تحديد نوع الكود المطلوب
     */
    CodeType detectCodeType(const std::string& command) {
        // البحث عن كلمات مفتاحية
        for (const auto& kw : functionKeywords_) {
            if (command.find(kw) != std::string::npos) {
                return CodeType::FUNCTION;
            }
        }
        
        for (const auto& kw : structKeywords_) {
            if (command.find(kw) != std::string::npos) {
                return CodeType::STRUCT;
            }
        }
        
        for (const auto& kw : variableKeywords_) {
            if (command.find(kw) != std::string::npos) {
                return CodeType::VARIABLE;
            }
        }
        
        for (const auto& kw : loopKeywords_) {
            if (command.find(kw) != std::string::npos) {
                return CodeType::LOOP;
            }
        }
        
        for (const auto& kw : conditionKeywords_) {
            if (command.find(kw) != std::string::npos) {
                return CodeType::CONDITION;
            }
        }
        
        // البحث عن كلمات ضمنية
        if (command.find("هيكل") != std::string::npos ||
            command.find("بنية") != std::string::npos) {
            return CodeType::STRUCT;
        }
        
        if (command.find("اختبار") != std::string::npos ||
            command.find("اختبر") != std::string::npos) {
            return CodeType::TEST;
        }
        
        // افتراضياً: دالة
        return CodeType::FUNCTION;
    }
    
    /**
     * استخراج معلومات الدالة
     */
    FunctionInfo parseFunctionCommand(const std::string& command) {
        FunctionInfo info;
        
        // استخراج اسم الدالة
        info.name = extractFunctionName(command);
        info.arabicName = info.name;
        
        // استخراج المعاملات
        info.params = extractParameters(command);
        
        // استخراج نوع الإرجاع
        info.returnType = extractReturnType(command);
        
        // هل الدالة تكرارية؟
        info.isRecursive = command.find("تكراري") != std::string::npos ||
                          command.find("مضروب") != std::string::npos ||
                          command.find("فيبوناتشي") != std::string::npos;
        
        // هل الدالة غير متزامنة؟
        info.isAsync = command.find("غير متزامن") != std::string::npos ||
                      command.find("async") != std::string::npos;
        
        info.description = command;
        
        return info;
    }
    
    /**
     * استخراج معلومات الهيكل
     */
    StructInfo parseStructCommand(const std::string& command) {
        StructInfo info;
        
        // استخراج اسم الهيكل
        info.name = extractStructName(command);
        
        // استخراج الحقول
        info.fields = extractFields(command);
        
        info.description = command;
        
        return info;
    }
    
private:
    /**
     * استخراج اسم الدالة
     */
    std::string extractFunctionName(const std::string& command) {
        // البحث عن أنماط شائعة
        std::vector<std::string> patterns = {
            "تحسب مضروب", "حساب مضروب", "مضروب",
            "تجمع عددين", "جمع عددين", "جمع",
            "تقرأ ملف", "قراءة ملف", "قراءة",
            "تكتب ملف", "كتابة ملف", "كتابة",
            "ترتب قائمة", "ترتيب قائمة", "ترتيب",
            "تبحث في", "بحث في", "بحث",
            "تحول", "تحويل"
        };
        
        for (const auto& pattern : patterns) {
            if (command.find(pattern) != std::string::npos) {
                // تحويل النمط إلى اسم دالة
                if (pattern.find("مضروب") != std::string::npos) return "مضروب";
                if (pattern.find("جمع") != std::string::npos) return "اجمع";
                if (pattern.find("قراءة") != std::string::npos) return "اقرأ_ملف";
                if (pattern.find("كتابة") != std::string::npos) return "اكتب_ملف";
                if (pattern.find("ترتيب") != std::string::npos) return "رتّب";
                if (pattern.find("بحث") != std::string::npos) return "ابحث";
                if (pattern.find("تحويل") != std::string::npos) return "حوّل";
            }
        }
        
        // اسم افتراضي
        return "دالتي";
    }
    
    /**
     * استخراج المعاملات
     */
    std::vector<std::pair<std::string, std::string>> extractParameters(
        const std::string& command) {
        std::vector<std::pair<std::string, std::string>> params;
        
        // أنماط شائعة
        if (command.find("عددين") != std::string::npos) {
            params.push_back({"أ", "عدد"});
            params.push_back({"ب", "عدد"});
        } else if (command.find("عدد") != std::string::npos) {
            params.push_back({"ن", "عدد"});
        } else if (command.find("نص") != std::string::npos) {
            params.push_back({"نص", "نص"});
        } else if (command.find("ملف") != std::string::npos) {
            params.push_back({"مسار", "نص"});
        } else if (command.find("قائمة") != std::string::npos) {
            params.push_back({"قائمة", "قائمة<عدد>"});
        }
        
        return params;
    }
    
    /**
     * استخراج نوع الإرجاع
     */
    std::string extractReturnType(const std::string& command) {
        // بناءً على السياق
        if (command.find("مضروب") != std::string::npos ||
            command.find("جمع") != std::string::npos ||
            command.find("حساب") != std::string::npos) {
            return "عدد";
        }
        
        if (command.find("قراءة") != std::string::npos) {
            return "نص";
        }
        
        if (command.find("بحث") != std::string::npos ||
            command.find("يوجد") != std::string::npos) {
            return "منطقي";
        }
        
        if (command.find("ترتيب") != std::string::npos) {
            return "قائمة<عدد>";
        }
        
        return "فراغ";
    }
    
    /**
     * استخراج اسم الهيكل
     */
    std::string extractStructName(const std::string& command) {
        std::vector<std::pair<std::string, std::string>> entities = {
            {"طالب", "طالب"}, {"للطالب", "طالب"},
            {"موظف", "موظف"}, {"للموظف", "موظف"},
            {"سيارة", "سيارة"}, {"للسيارة", "سيارة"},
            {"شخص", "شخص"}, {"للشخص", "شخص"},
            {"منتج", "منتج"}, {"للمنتج", "منتج"},
            {"كتاب", "كتاب"}, {"للكتاب", "كتاب"},
            {"نقطة", "نقطة"}, {"للنقطة", "نقطة"},
            {"مستطيل", "مستطيل"}, {"للمستطيل", "مستطيل"}
        };
        
        for (const auto& [keyword, name] : entities) {
            if (command.find(keyword) != std::string::npos) {
                return name;
            }
        }
        
        return "هيكلي";
    }
    
    /**
     * استخراج الحقول
     */
    std::vector<std::pair<std::string, std::string>> extractFields(
        const std::string& command) {
        std::vector<std::pair<std::string, std::string>> fields;
        
        // حقول شائعة بناءً على نوع الهيكل
        if (command.find("طالب") != std::string::npos) {
            fields.push_back({"الاسم", "نص"});
            fields.push_back({"العمر", "عدد"});
            fields.push_back({"المعدل", "عشري"});
        } else if (command.find("موظف") != std::string::npos) {
            fields.push_back({"الاسم", "نص"});
            fields.push_back({"الراتب", "عشري"});
            fields.push_back({"القسم", "نص"});
        } else if (command.find("نقطة") != std::string::npos) {
            fields.push_back({"س", "عشري"});
            fields.push_back({"ص", "عشري"});
        } else if (command.find("مستطيل") != std::string::npos) {
            fields.push_back({"س", "عشري"});
            fields.push_back({"ص", "عشري"});
            fields.push_back({"العرض", "عشري"});
            fields.push_back({"الطول", "عشري"});
        } else {
            // حقول افتراضية
            fields.push_back({"اسم", "نص"});
            fields.push_back({"قيمة", "عدد"});
        }
        
        return fields;
    }
};

// ==========================================================
// 📌 مولّد الكود
// ==========================================================

/**
 * ✍️ مولّد كود ص
 */
class CodeGenerator {
private:
    int indentLevel_ = 0;
    std::string indentStr_ = "    ";
    
    std::string indent() const {
        std::string result;
        for (int i = 0; i < indentLevel_; ++i) {
            result += indentStr_;
        }
        return result;
    }
    
public:
    /**
     * توليد دالة
     */
    std::string generateFunction(const FunctionInfo& info) {
        std::ostringstream oss;
        
        // توثيق الدالة
        oss << "/**\n";
        oss << " * " << info.description << "\n";
        oss << " */\n";
        
        // توقيع الدالة
        oss << "دالة " << info.name << "(";
        
        // المعاملات
        for (size_t i = 0; i < info.params.size(); ++i) {
            if (i > 0) oss << "، ";
            oss << info.params[i].first << ": " << info.params[i].second;
        }
        
        oss << ")";
        
        // نوع الإرجاع
        if (info.returnType != "فراغ") {
            oss << " ← " << info.returnType;
        }
        
        oss << "\n";
        
        // جسم الدالة
        indentLevel_++;
        
        if (info.isRecursive) {
            oss << generateRecursiveBody(info);
        } else {
            oss << generateSimpleBody(info);
        }
        
        indentLevel_--;
        oss << "نهاية\n";
        
        return oss.str();
    }
    
    /**
     * توليد هيكل
     */
    std::string generateStruct(const StructInfo& info) {
        std::ostringstream oss;
        
        // توثيق الهيكل
        oss << "/**\n";
        oss << " * هيكل: " << info.name << "\n";
        oss << " * " << info.description << "\n";
        oss << " */\n";
        
        // تعريف الهيكل
        oss << "هيكل " << info.name << "\n";
        
        indentLevel_++;
        
        // الحقول
        for (const auto& [name, type] : info.fields) {
            oss << indent() << name << ": " << type << "\n";
        }
        
        indentLevel_--;
        oss << "نهاية\n";
        
        // إضافة دالة إنشاء
        oss << "\n";
        oss << "دالة أنشئ_" << info.name << "(";
        for (size_t i = 0; i < info.fields.size(); ++i) {
            if (i > 0) oss << "، ";
            oss << info.fields[i].first << ": " << info.fields[i].second;
        }
        oss << ") ← " << info.name << "\n";
        
        indentLevel_++;
        oss << indent() << "أرجع " << info.name << " {\n";
        indentLevel_++;
        for (const auto& [name, type] : info.fields) {
            oss << indent() << name << ": " << name << "\n";
        }
        indentLevel_--;
        oss << indent() << "}\n";
        indentLevel_--;
        oss << "نهاية\n";
        
        return oss.str();
    }
    
    /**
     * توليد متغير
     */
    std::string generateVariable(const VariableInfo& info) {
        std::ostringstream oss;
        
        if (info.isConstant) {
            oss << "ثابت " << info.name;
        } else {
            oss << "متغير " << info.name;
        }
        
        if (!info.type.empty()) {
            oss << ": " << info.type;
        }
        
        if (!info.value.empty()) {
            oss << " = " << info.value;
        }
        
        oss << "\n";
        
        return oss.str();
    }
    
    /**
     * توليد حلقة
     */
    std::string generateLoop(const std::string& variable, 
                             const std::string& range,
                             const std::string& body) {
        std::ostringstream oss;
        
        oss << "لكل " << variable << " في " << range << "\n";
        
        indentLevel_++;
        oss << indent() << body << "\n";
        indentLevel_--;
        
        oss << "نهاية\n";
        
        return oss.str();
    }
    
    /**
     * توليد شرط
     */
    std::string generateCondition(const std::string& condition,
                                  const std::string& ifBody,
                                  const std::string& elseBody = "") {
        std::ostringstream oss;
        
        oss << "إذا " << condition << "\n";
        
        indentLevel_++;
        oss << indent() << ifBody << "\n";
        indentLevel_--;
        
        if (!elseBody.empty()) {
            oss << "وإلا\n";
            indentLevel_++;
            oss << indent() << elseBody << "\n";
            indentLevel_--;
        }
        
        oss << "نهاية\n";
        
        return oss.str();
    }
    
    /**
     * توليد اختبار
     */
    std::string generateTest(const std::string& name,
                             const std::string& body) {
        std::ostringstream oss;
        
        oss << "اختبار \"" << name << "\"\n";
        
        indentLevel_++;
        oss << indent() << "// الترتيب\n";
        oss << indent() << body << "\n";
        oss << indent() << "// التأكيد\n";
        oss << indent() << "تأكد_صح(نتيجة)\n";
        indentLevel_--;
        
        oss << "نهاية\n";
        
        return oss.str();
    }
    
private:
    /**
     * توليد جسم تكراري
     */
    std::string generateRecursiveBody(const FunctionInfo& info) {
        std::ostringstream oss;
        
        // حالة الأساس
        if (info.name == "مضروب") {
            oss << indent() << "إذا ن <= 1\n";
            indentLevel_++;
            oss << indent() << "أرجع 1\n";
            indentLevel_--;
            oss << indent() << "نهاية\n";
            oss << indent() << "أرجع ن * مضروب(ن - 1)\n";
        } else if (info.name.find("فيبوناتشي") != std::string::npos) {
            oss << indent() << "إذا ن <= 1\n";
            indentLevel_++;
            oss << indent() << "أرجع ن\n";
            indentLevel_--;
            oss << indent() << "نهاية\n";
            oss << indent() << "أرجع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)\n";
        } else {
            oss << indent() << "// حالة الأساس\n";
            oss << indent() << "إذا شرط_الإيقاف\n";
            indentLevel_++;
            oss << indent() << "أرجع قيمة_أساسية\n";
            indentLevel_--;
            oss << indent() << "نهاية\n";
            oss << indent() << "// الحالة التكرارية\n";
            oss << indent() << "أرجع " << info.name << "(معامل_أصغر)\n";
        }
        
        return oss.str();
    }
    
    /**
     * توليد جسم بسيط
     */
    std::string generateSimpleBody(const FunctionInfo& info) {
        std::ostringstream oss;
        
        if (info.name == "اجمع") {
            oss << indent() << "أرجع أ + ب\n";
        } else if (info.name == "اقرأ_ملف") {
            oss << indent() << "متغير ملف = افتح(مسار، \"قراءة\")\n";
            oss << indent() << "متغير محتوى = ملف.اقرأ_الكل()\n";
            oss << indent() << "أغلق(ملف)\n";
            oss << indent() << "أرجع محتوى\n";
        } else if (info.name == "اكتب_ملف") {
            oss << indent() << "متغير ملف = افتح(مسار، \"كتابة\")\n";
            oss << indent() << "ملف.اكتب(محتوى)\n";
            oss << indent() << "أغلق(ملف)\n";
        } else if (info.name == "رتّب") {
            oss << indent() << "// خوارزمية الترتيب\n";
            oss << indent() << "أرجع قائمة.رتّب()\n";
        } else if (info.name == "ابحث") {
            oss << indent() << "// خوارزمية البحث\n";
            oss << indent() << "أرجع خطأ // غير موجود\n";
        } else {
            // جسم افتراضي
            if (info.returnType != "فراغ") {
                oss << indent() << "// TODO: تنفيذ الدالة\n";
                if (info.returnType == "عدد") {
                    oss << indent() << "أرجع 0\n";
                } else if (info.returnType == "نص") {
                    oss << indent() << "أرجع \"\"\n";
                } else if (info.returnType == "منطقي") {
                    oss << indent() << "أرجع صح\n";
                } else {
                    oss << indent() << "أرجع فراغ\n";
                }
            } else {
                oss << indent() << "// TODO: تنفيذ الدالة\n";
            }
        }
        
        return oss.str();
    }
};

// ==========================================================
// 📌 فئة ArabicCodeGen الرئيسية
// ==========================================================

/**
 * 🤖 مولّد الكود من العربية
 */
class ArabicCodeGen {
private:
    CommandParser parser_;
    CodeGenerator generator_;
    
public:
    /**
     * توليد كود من أمر عربي
     */
    std::string generate(const std::string& command) {
        // تحديد نوع الكود
        CodeType type = parser_.detectCodeType(command);
        
        switch (type) {
            case CodeType::FUNCTION:
                return generateFunction(command);
            case CodeType::STRUCT:
                return generateStruct(command);
            case CodeType::VARIABLE:
                return generateVariable(command);
            case CodeType::LOOP:
                return generateLoop(command);
            case CodeType::CONDITION:
                return generateCondition(command);
            case CodeType::TEST:
                return generateTest(command);
            default:
                return generateDefault(command);
        }
    }
    
    /**
     * توليد دالة
     */
    std::string generateFunction(const std::string& command) {
        FunctionInfo info = parser_.parseFunctionCommand(command);
        return generator_.generateFunction(info);
    }
    
    /**
     * توليد هيكل
     */
    std::string generateStruct(const std::string& command) {
        StructInfo info = parser_.parseStructCommand(command);
        return generator_.generateStruct(info);
    }
    
    /**
     * توليد متغير
     */
    std::string generateVariable(const std::string& command) {
        VariableInfo info;
        
        if (command.find("ثابت") != std::string::npos) {
            info.isConstant = true;
            info.name = "ثابتي";
        } else {
            info.name = "متغيري";
        }
        
        if (command.find("عدد") != std::string::npos) {
            info.type = "عدد";
            info.value = "0";
        } else if (command.find("نص") != std::string::npos) {
            info.type = "نص";
            info.value = "\"\"";
        }
        
        return generator_.generateVariable(info);
    }
    
    /**
     * توليد حلقة
     */
    std::string generateLoop(const std::string& command) {
        return generator_.generateLoop("i", "1..10", "اطبع(i)");
    }
    
    /**
     * توليد شرط
     */
    std::string generateCondition(const std::string& command) {
        return generator_.generateCondition(
            "شرط",
            "// إذا صح الشرط",
            "// إذا خطأ الشرط"
        );
    }
    
    /**
     * توليد اختبار
     */
    std::string generateTest(const std::string& command) {
        return generator_.generateTest(
            "اختبار جديد",
            "متغير نتيجة = صح"
        );
    }
    
    /**
     * توليد افتراضي
     */
    std::string generateDefault(const std::string& command) {
        std::ostringstream oss;
        oss << "// كود مُولَّد بناءً على: " << command << "\n\n";
        oss << "دالة رئيسية()\n";
        oss << "    اطبع(\"مرحباً من ص!\")\n";
        oss << "نهاية\n";
        return oss.str();
    }
};

} // namespace ai
} // namespace sad

// ==========================================================
// 📌 C API
// ==========================================================

extern "C" {

/**
 * إنشاء مولّد كود
 */
void* sad_arabic_codegen_new(void) {
    return new sad::ai::ArabicCodeGen();
}

/**
 * تحرير المولّد
 */
void sad_arabic_codegen_free(void* gen) {
    delete static_cast<sad::ai::ArabicCodeGen*>(gen);
}

/**
 * توليد كود من أمر
 */
char* sad_arabic_codegen_generate(void* gen, const char* command) {
    auto* g = static_cast<sad::ai::ArabicCodeGen*>(gen);
    std::string result = g->generate(command);
    
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * توليد دالة
 */
char* sad_arabic_codegen_function(void* gen, const char* command) {
    auto* g = static_cast<sad::ai::ArabicCodeGen*>(gen);
    std::string result = g->generateFunction(command);
    
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * توليد هيكل
 */
char* sad_arabic_codegen_struct(void* gen, const char* command) {
    auto* g = static_cast<sad::ai::ArabicCodeGen*>(gen);
    std::string result = g->generateStruct(command);
    
    char* output = new char[result.size() + 1];
    std::strcpy(output, result.c_str());
    return output;
}

/**
 * تحرير نص مُولَّد
 */
void sad_arabic_codegen_free_string(char* str) {
    delete[] str;
}

} // extern "C"
