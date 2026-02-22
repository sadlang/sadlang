/**
 * ╔══════════════════════════════════════════════════════════════════════════════╗
 * ║                       لغة سعد - مترجم عربي الأول                              ║
 * ║                   Sad Language - First Arabic Compiler                        ║
 * ╠══════════════════════════════════════════════════════════════════════════════╣
 * ║                                                                              ║
 * ║   ملف: type_explanations.cpp                                                ║
 * ║   الوصف: شرح تعليمي لأخطاء عدم تطابق الأنواع                                ║
 * ║   المهمة: T162 - Phase 15 (US13)                                            ║
 * ║   المؤلف: فريق سعد | Sad Team                                                ║
 * ╚══════════════════════════════════════════════════════════════════════════════╝
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 *                         دليل المبتدئين | Beginner's Guide
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * 🌟 ما هي الأنواع في البرمجة؟ | What are Types in Programming?
 * ─────────────────────────────────────────────────────────────
 *
 * تخيل أن لديك صناديق مختلفة:
 *
 *    📦 صندوق للأرقام (عدد): 1, 2, 3, 100, -5
 *    📦 صندوق للنصوص (نص): "مرحبا", "أهلاً", "سعد"
 *    📦 صندوق للمنطق (منطقي): صحيح, خطأ
 *    📦 صندوق للقوائم (قائمة): [1, 2, 3], ["أ", "ب", "ج"]
 *
 * كل صندوق مصمم لنوع معين من البيانات!
 *
 * ❌ لماذا لا يمكن خلط الأنواع؟
 * ─────────────────────────────────────
 *
 * مثال من الحياة اليومية:
 *
 *    🍎 + 🍎 = 🍎🍎 (جمع تفاحتين ممكن)
 *    5 + 3 = 8 (جمع رقمين ممكن)
 *    "أهلاً" + " وسهلاً" = "أهلاً وسهلاً" (دمج نصين ممكن)
 *
 *    لكن:
 *    🍎 + 5 = ؟؟؟ (لا معنى له!)
 *    "مرحبا" + 7 = ؟؟؟ (غير منطقي!)
 *
 * 💡 الحل: التحويل (Conversion)
 * ─────────────────────────────────────
 *
 *    إذا أردت جمع نص مع رقم، حوّل الرقم إلى نص أولاً:
 *
 *    "العمر: " + نص(25) = "العمر: 25" ✅
 *
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <memory>

namespace sad {
namespace errors {

// ═══════════════════════════════════════════════════════════════════════════════
//                         أنواع البيانات | Data Types
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📊 SadType - تمثيل أنواع لغة سعد
 */
enum class SadType {
    UNKNOWN,        // غير معروف
    VOID,           // عدم (لا شيء)
    NUMBER,         // عدد
    INTEGER,        // صحيح
    FLOAT,          // عشري
    STRING,         // نص
    BOOLEAN,        // منطقي
    ARRAY,          // مصفوفة
    LIST,           // قائمة
    MAP,            // قاموس
    OBJECT,         // كائن
    FUNCTION,       // دالة
    OPTIONAL,       // اختياري
    UNION,          // اتحاد
    ANY             // أي
};

/**
 * 🔤 تحويل النوع إلى اسم عربي
 */
std::string typeToArabicName(SadType type) {
    static std::unordered_map<SadType, std::string> names = {
        {SadType::UNKNOWN, "غير معروف"},
        {SadType::VOID, "عدم"},
        {SadType::NUMBER, "عدد"},
        {SadType::INTEGER, "صحيح"},
        {SadType::FLOAT, "عشري"},
        {SadType::STRING, "نص"},
        {SadType::BOOLEAN, "منطقي"},
        {SadType::ARRAY, "مصفوفة"},
        {SadType::LIST, "قائمة"},
        {SadType::MAP, "قاموس"},
        {SadType::OBJECT, "كائن"},
        {SadType::FUNCTION, "دالة"},
        {SadType::OPTIONAL, "اختياري"},
        {SadType::UNION, "اتحاد"},
        {SadType::ANY, "أي"}
    };
    
    auto it = names.find(type);
    return it != names.end() ? it->second : "غير معروف";
}

/**
 * 🔤 تحويل النوع إلى اسم إنجليزي
 */
std::string typeToEnglishName(SadType type) {
    static std::unordered_map<SadType, std::string> names = {
        {SadType::UNKNOWN, "unknown"},
        {SadType::VOID, "void"},
        {SadType::NUMBER, "number"},
        {SadType::INTEGER, "integer"},
        {SadType::FLOAT, "float"},
        {SadType::STRING, "string"},
        {SadType::BOOLEAN, "boolean"},
        {SadType::ARRAY, "array"},
        {SadType::LIST, "list"},
        {SadType::MAP, "map"},
        {SadType::OBJECT, "object"},
        {SadType::FUNCTION, "function"},
        {SadType::OPTIONAL, "optional"},
        {SadType::UNION, "union"},
        {SadType::ANY, "any"}
    };
    
    auto it = names.find(type);
    return it != names.end() ? it->second : "unknown";
}

// ═══════════════════════════════════════════════════════════════════════════════
//                      معلومات النوع | Type Information
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📋 TypeInfo - معلومات تفصيلية عن النوع
 */
struct TypeInfo {
    SadType type;
    std::string arabicName;
    std::string englishName;
    std::string description;        // وصف بسيط
    std::string analogy;            // تشبيه من الحياة
    std::string examples;           // أمثلة
    std::vector<std::string> validOperations;  // عمليات صالحة
    std::vector<SadType> canConvertTo;         // يمكن تحويله إلى
    std::vector<SadType> canConvertFrom;       // يمكن تحويله من
    
    TypeInfo() : type(SadType::UNKNOWN) {}
};

// ═══════════════════════════════════════════════════════════════════════════════
//                   قاعدة بيانات الأنواع | Type Database
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📚 TypeDatabase - قاعدة بيانات شاملة للأنواع
 */
class TypeDatabase {
public:
    TypeDatabase() {
        initializeTypes();
    }
    
    /**
     * 🔍 الحصول على معلومات نوع
     */
    const TypeInfo* getTypeInfo(SadType type) const {
        auto it = types_.find(type);
        return it != types_.end() ? &it->second : nullptr;
    }
    
    /**
     * 🔍 الحصول على معلومات نوع بالاسم
     */
    const TypeInfo* getTypeInfoByName(const std::string& name) const {
        auto it = nameToType_.find(name);
        if (it != nameToType_.end()) {
            return getTypeInfo(it->second);
        }
        return nullptr;
    }
    
private:
    std::unordered_map<SadType, TypeInfo> types_;
    std::unordered_map<std::string, SadType> nameToType_;
    
    void initializeTypes() {
        // ═══════════════════════════════════════════════════════════════════════
        //                              نوع: عدد
        // ═══════════════════════════════════════════════════════════════════════
        {
            TypeInfo info;
            info.type = SadType::NUMBER;
            info.arabicName = "عدد";
            info.englishName = "number";
            info.description = 
                "العدد هو أي رقم يمكن استخدامه في الحسابات.\n"
                "يمكن أن يكون موجباً أو سالباً، صحيحاً أو عشرياً.";
            info.analogy = 
                "🔢 تخيل العدد كمقياس الحرارة:\n"
                "   يمكن أن يكون 25 (موجب)\n"
                "   أو -5 (سالب)\n"
                "   أو 36.6 (عشري)";
            info.examples = 
                "متغير عمري = 25\n"
                "متغير درجة_الحرارة = 36.6\n"
                "متغير رصيدي = -100";
            info.validOperations = {"+", "-", "*", "/", "%", "**", "==", "!=", "<", ">", "<=", ">="};
            info.canConvertTo = {SadType::STRING, SadType::BOOLEAN, SadType::FLOAT, SadType::INTEGER};
            info.canConvertFrom = {SadType::STRING, SadType::BOOLEAN};
            
            types_[SadType::NUMBER] = info;
            nameToType_["عدد"] = SadType::NUMBER;
            nameToType_["number"] = SadType::NUMBER;
        }
        
        // ═══════════════════════════════════════════════════════════════════════
        //                              نوع: نص
        // ═══════════════════════════════════════════════════════════════════════
        {
            TypeInfo info;
            info.type = SadType::STRING;
            info.arabicName = "نص";
            info.englishName = "string";
            info.description = 
                "النص هو سلسلة من الحروف والرموز.\n"
                "يُستخدم لتخزين الكلمات والجمل والرسائل.";
            info.analogy = 
                "📝 تخيل النص كسلسلة من الخرز:\n"
                "   كل خرزة هي حرف\n"
                "   \"سعد\" = س + ع + د\n"
                "   يمكنك إضافة خرز (حروف) جديدة";
            info.examples = 
                "متغير اسمي = \"أحمد\"\n"
                "متغير رسالة = \"مرحباً بكم في لغة سعد\"\n"
                "متغير رمز = \"🎉\"";
            info.validOperations = {"+", "==", "!=", "<", ">", "[]", "طول", "يحتوي"};
            info.canConvertTo = {SadType::NUMBER, SadType::BOOLEAN, SadType::LIST};
            info.canConvertFrom = {SadType::NUMBER, SadType::BOOLEAN, SadType::LIST, SadType::OBJECT};
            
            types_[SadType::STRING] = info;
            nameToType_["نص"] = SadType::STRING;
            nameToType_["string"] = SadType::STRING;
        }
        
        // ═══════════════════════════════════════════════════════════════════════
        //                              نوع: منطقي
        // ═══════════════════════════════════════════════════════════════════════
        {
            TypeInfo info;
            info.type = SadType::BOOLEAN;
            info.arabicName = "منطقي";
            info.englishName = "boolean";
            info.description = 
                "النوع المنطقي له قيمتان فقط: صحيح أو خطأ.\n"
                "يُستخدم للتحقق من الشروط واتخاذ القرارات.";
            info.analogy = 
                "💡 تخيل المنطقي كمفتاح إضاءة:\n"
                "   إما مفتوح (صحيح) ✅\n"
                "   أو مغلق (خطأ) ❌\n"
                "   لا يوجد حالة وسطى!";
            info.examples = 
                "متغير متزوج = صحيح\n"
                "متغير طالب = خطأ\n"
                "متغير بالغ = عمري >= 18";
            info.validOperations = {"&&", "||", "!", "==", "!="};
            info.canConvertTo = {SadType::STRING, SadType::NUMBER};
            info.canConvertFrom = {SadType::STRING, SadType::NUMBER};
            
            types_[SadType::BOOLEAN] = info;
            nameToType_["منطقي"] = SadType::BOOLEAN;
            nameToType_["boolean"] = SadType::BOOLEAN;
        }
        
        // ═══════════════════════════════════════════════════════════════════════
        //                              نوع: قائمة
        // ═══════════════════════════════════════════════════════════════════════
        {
            TypeInfo info;
            info.type = SadType::LIST;
            info.arabicName = "قائمة";
            info.englishName = "list";
            info.description = 
                "القائمة هي مجموعة مرتبة من العناصر.\n"
                "يمكن إضافة وحذف عناصر منها.";
            info.analogy = 
                "📋 تخيل القائمة كقائمة التسوق:\n"
                "   [\"حليب\", \"خبز\", \"بيض\"]\n"
                "   يمكنك إضافة أو حذف أشياء\n"
                "   كل شيء له رقم (0, 1, 2, ...)";
            info.examples = 
                "متغير أرقام = [1, 2, 3, 4, 5]\n"
                "متغير أسماء = [\"أحمد\", \"محمد\", \"علي\"]\n"
                "أرقام.أضف(6)";
            info.validOperations = {"+", "[]", "طول", "أضف", "احذف", "يحتوي"};
            info.canConvertTo = {SadType::STRING, SadType::ARRAY};
            info.canConvertFrom = {SadType::STRING, SadType::ARRAY};
            
            types_[SadType::LIST] = info;
            nameToType_["قائمة"] = SadType::LIST;
            nameToType_["list"] = SadType::LIST;
        }
        
        // ═══════════════════════════════════════════════════════════════════════
        //                              نوع: قاموس
        // ═══════════════════════════════════════════════════════════════════════
        {
            TypeInfo info;
            info.type = SadType::MAP;
            info.arabicName = "قاموس";
            info.englishName = "map";
            info.description = 
                "القاموس يخزن أزواج من المفاتيح والقيم.\n"
                "كل مفتاح يشير إلى قيمة معينة.";
            info.analogy = 
                "📖 تخيل القاموس كدفتر العناوين:\n"
                "   الاسم → رقم الهاتف\n"
                "   \"أحمد\" → \"0501234567\"\n"
                "   تبحث بالاسم وتجد الرقم";
            info.examples = 
                "متغير عناوين = {\n"
                "    \"أحمد\": \"0501234567\",\n"
                "    \"محمد\": \"0507654321\"\n"
                "}";
            info.validOperations = {"[]", "مفاتيح", "قيم", "يحتوي_مفتاح"};
            info.canConvertTo = {SadType::STRING, SadType::LIST};
            info.canConvertFrom = {};
            
            types_[SadType::MAP] = info;
            nameToType_["قاموس"] = SadType::MAP;
            nameToType_["map"] = SadType::MAP;
        }
        
        // ═══════════════════════════════════════════════════════════════════════
        //                              نوع: دالة
        // ═══════════════════════════════════════════════════════════════════════
        {
            TypeInfo info;
            info.type = SadType::FUNCTION;
            info.arabicName = "دالة";
            info.englishName = "function";
            info.description = 
                "الدالة هي كتلة من الكود يمكن استدعاؤها.\n"
                "تأخذ مدخلات وتُرجع مخرجات.";
            info.analogy = 
                "🏭 تخيل الدالة كآلة:\n"
                "   تضع مواد خام (المدخلات)\n"
                "   الآلة تعالجها\n"
                "   تحصل على منتج (المخرجات)";
            info.examples = 
                "دالة مربع(عدد):\n"
                "    أرجع عدد * عدد\n"
                "\n"
                "متغير نتيجة = مربع(5)  // 25";
            info.validOperations = {"()"};
            info.canConvertTo = {SadType::STRING};
            info.canConvertFrom = {};
            
            types_[SadType::FUNCTION] = info;
            nameToType_["دالة"] = SadType::FUNCTION;
            nameToType_["function"] = SadType::FUNCTION;
        }
        
        // ═══════════════════════════════════════════════════════════════════════
        //                              نوع: اختياري
        // ═══════════════════════════════════════════════════════════════════════
        {
            TypeInfo info;
            info.type = SadType::OPTIONAL;
            info.arabicName = "اختياري";
            info.englishName = "optional";
            info.description = 
                "النوع الاختياري قد يحتوي على قيمة أو لا.\n"
                "يُستخدم عندما تكون القيمة غير مضمونة.";
            info.analogy = 
                "📭 تخيل صندوق البريد:\n"
                "   قد يكون فيه رسالة (بعض)\n"
                "   أو فارغ (عدم)\n"
                "   يجب فحصه قبل القراءة!";
            info.examples = 
                "دالة ابحث_عن_مستخدم(معرف): نص؟\n"
                "    إذا وجد:\n"
                "        أرجع بعض(اسم)\n"
                "    وإلا:\n"
                "        أرجع عدم";
            info.validOperations = {"؟", "??"};
            info.canConvertTo = {};
            info.canConvertFrom = {};
            
            types_[SadType::OPTIONAL] = info;
            nameToType_["اختياري"] = SadType::OPTIONAL;
            nameToType_["optional"] = SadType::OPTIONAL;
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                    شارح عدم التطابق | Mismatch Explainer
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * 📖 TypeMismatchExplainer - يشرح أخطاء عدم التطابق بطريقة تعليمية
 */
class TypeMismatchExplainer {
public:
    TypeMismatchExplainer() : db_() {}
    
    /**
     * 📝 توليد شرح لخطأ عدم تطابق الأنواع
     */
    std::string explain(
        const std::string& expectedType,
        const std::string& actualType,
        const std::string& operation,
        const std::string& context
    ) {
        std::ostringstream explanation;
        
        // العنوان
        explanation << "\n";
        explanation << "╔══════════════════════════════════════════════════════════════╗\n";
        explanation << "║                   خطأ: عدم تطابق الأنواع                      ║\n";
        explanation << "║                   Error: Type Mismatch                        ║\n";
        explanation << "╚══════════════════════════════════════════════════════════════╝\n";
        explanation << "\n";
        
        // الشرح الأساسي
        explanation << "📋 ما حدث | What Happened:\n";
        explanation << "   ─────────────────────────────────────────────────────────\n";
        explanation << "   حاولت استخدام قيمة من نوع (" << actualType << ")\n";
        explanation << "   في مكان يتوقع نوعاً مختلفاً (" << expectedType << ")\n";
        explanation << "\n";
        
        // شرح الأنواع
        const TypeInfo* expectedInfo = db_.getTypeInfoByName(expectedType);
        const TypeInfo* actualInfo = db_.getTypeInfoByName(actualType);
        
        if (expectedInfo) {
            explanation << "📦 النوع المتوقع: " << expectedType << "\n";
            explanation << "   " << expectedInfo->description << "\n";
            if (!expectedInfo->analogy.empty()) {
                explanation << "\n" << expectedInfo->analogy << "\n";
            }
            explanation << "\n";
        }
        
        if (actualInfo) {
            explanation << "📦 النوع الفعلي: " << actualType << "\n";
            explanation << "   " << actualInfo->description << "\n";
            explanation << "\n";
        }
        
        // لماذا لا يمكن الخلط
        explanation << generateWhyExplanation(expectedType, actualType, operation);
        
        // كيف تصلح
        explanation << generateFixExplanation(expectedType, actualType, context);
        
        // أمثلة
        explanation << generateExamples(expectedType, actualType);
        
        return explanation.str();
    }
    
    /**
     * 📊 الحصول على معلومات نوع
     */
    std::string getTypeDescription(const std::string& typeName) {
        const TypeInfo* info = db_.getTypeInfoByName(typeName);
        if (!info) return "";
        
        std::ostringstream desc;
        desc << "📦 " << info->arabicName << " | " << info->englishName << "\n";
        desc << "   ─────────────────────────────────────────\n";
        desc << "   " << info->description << "\n";
        
        if (!info->analogy.empty()) {
            desc << "\n" << info->analogy << "\n";
        }
        
        if (!info->examples.empty()) {
            desc << "\n   أمثلة:\n";
            desc << "   ─────\n";
            std::istringstream stream(info->examples);
            std::string line;
            while (std::getline(stream, line)) {
                desc << "   " << line << "\n";
            }
        }
        
        return desc.str();
    }
    
private:
    TypeDatabase db_;
    
    /**
     * ❓ شرح لماذا لا يمكن الخلط
     */
    std::string generateWhyExplanation(
        const std::string& expected,
        const std::string& actual,
        const std::string& operation
    ) {
        std::ostringstream why;
        
        why << "❓ لماذا لا يعمل؟ | Why Doesn't It Work?\n";
        why << "   ─────────────────────────────────────────────────────────\n";
        
        // حالات خاصة
        if ((expected == "عدد" || expected == "number") &&
            (actual == "نص" || actual == "string")) {
            why << "   لا يمكن إجراء عمليات حسابية على النصوص!\n";
            why << "   \"5\" ليس نفس الشيء مثل 5\n";
            why << "\n";
            why << "   تخيل أنك تحاول جمع كلمة \"خمسة\" مع رقم 3\n";
            why << "   الحاسوب لا يعرف أن \"خمسة\" تعني 5\n";
        }
        else if ((expected == "نص" || expected == "string") &&
                 (actual == "عدد" || actual == "number")) {
            why << "   لا يمكن دمج رقم مع نص مباشرة!\n";
            why << "   يجب تحويل الرقم إلى نص أولاً\n";
            why << "\n";
            why << "   مثال: \"العمر: \" + 25 ❌\n";
            why << "         \"العمر: \" + نص(25) ✅\n";
        }
        else if ((expected == "منطقي" || expected == "boolean")) {
            why << "   الشرط يحتاج قيمة منطقية (صحيح/خطأ)\n";
            why << "   لا يمكن استخدام " << actual << " مباشرة كشرط\n";
        }
        else if ((expected == "قائمة" || expected == "list") &&
                 (actual != "قائمة" && actual != "list")) {
            why << "   هذه العملية تحتاج قائمة!\n";
            why << "   لا يمكن تكرار على قيمة واحدة\n";
        }
        else {
            why << "   النوعان غير متوافقان للعملية المطلوبة\n";
            why << "   كل نوع له عمليات محددة يمكن إجراؤها عليه\n";
        }
        
        why << "\n";
        return why.str();
    }
    
    /**
     * 🔧 شرح كيفية الإصلاح
     */
    std::string generateFixExplanation(
        const std::string& expected,
        const std::string& actual,
        const std::string& context
    ) {
        std::ostringstream fix;
        
        fix << "💡 كيف تصلح؟ | How To Fix?\n";
        fix << "   ─────────────────────────────────────────────────────────\n";
        
        // اقتراحات حسب الأنواع
        if ((expected == "عدد" || expected == "number") &&
            (actual == "نص" || actual == "string")) {
            fix << "   الخيار 1: حوّل النص إلى عدد\n";
            fix << "   ────────────────────────────\n";
            fix << "   متغير القيمة = عدد(\"123\")  // يصبح 123\n";
            fix << "\n";
            fix << "   ⚠️ تحذير: تأكد أن النص يحتوي على أرقام فقط!\n";
            fix << "   عدد(\"abc\") سيسبب خطأ\n";
        }
        else if ((expected == "نص" || expected == "string") &&
                 (actual == "عدد" || actual == "number")) {
            fix << "   الخيار 1: حوّل العدد إلى نص\n";
            fix << "   ────────────────────────────\n";
            fix << "   متغير النص = نص(123)  // يصبح \"123\"\n";
            fix << "\n";
            fix << "   الخيار 2: استخدم تنسيق النصوص\n";
            fix << "   ─────────────────────────────\n";
            fix << "   متغير الرسالة = صيغة(\"العمر: {}\", العمر)\n";
        }
        else if ((expected == "منطقي" || expected == "boolean")) {
            fix << "   استخدم مقارنة صريحة:\n";
            fix << "   ────────────────────\n";
            fix << "   // بدلاً من:\n";
            fix << "   إذا قيمة:  // ❌\n";
            fix << "\n";
            fix << "   // استخدم:\n";
            fix << "   إذا قيمة != 0:  // ✅ للأعداد\n";
            fix << "   إذا قيمة != \"\":  // ✅ للنصوص\n";
            fix << "   إذا طول(قيمة) > 0:  // ✅ للقوائم\n";
        }
        else {
            fix << "   حوّل القيمة إلى النوع المطلوب:\n";
            fix << "   متغير القيمة_الجديدة = " << expected << "(القيمة_القديمة)\n";
        }
        
        fix << "\n";
        return fix.str();
    }
    
    /**
     * 📝 توليد أمثلة
     */
    std::string generateExamples(
        const std::string& expected,
        const std::string& actual
    ) {
        std::ostringstream examples;
        
        examples << "📝 أمثلة | Examples:\n";
        examples << "   ─────────────────────────────────────────────────────────\n";
        
        if ((expected == "عدد" || expected == "number") &&
            (actual == "نص" || actual == "string")) {
            examples << "   // ❌ خاطئ:\n";
            examples << "   متغير مجموع = \"5\" + 3\n";
            examples << "\n";
            examples << "   // ✅ صحيح:\n";
            examples << "   متغير مجموع = عدد(\"5\") + 3  // النتيجة: 8\n";
        }
        else if ((expected == "نص" || expected == "string") &&
                 (actual == "عدد" || actual == "number")) {
            examples << "   // ❌ خاطئ:\n";
            examples << "   متغير رسالة = \"العمر: \" + 25\n";
            examples << "\n";
            examples << "   // ✅ صحيح:\n";
            examples << "   متغير رسالة = \"العمر: \" + نص(25)  // النتيجة: \"العمر: 25\"\n";
        }
        else {
            examples << "   // ❌ خاطئ:\n";
            examples << "   استخدام " << actual << " بدلاً من " << expected << "\n";
            examples << "\n";
            examples << "   // ✅ صحيح:\n";
            examples << "   استخدم التحويل المناسب أو غيّر المنطق\n";
        }
        
        examples << "\n";
        return examples.str();
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
//                        واجهة خارجية | External Interface
// ═══════════════════════════════════════════════════════════════════════════════

static std::unique_ptr<TypeMismatchExplainer> globalExplainer;

extern "C" {
    /**
     * 🔧 تهيئة النظام
     */
    void sad_type_explainer_init() {
        if (!globalExplainer) {
            globalExplainer = std::make_unique<TypeMismatchExplainer>();
        }
    }
    
    /**
     * 📖 شرح خطأ عدم تطابق الأنواع
     */
    const char* sad_explain_type_mismatch(
        const char* expectedType,
        const char* actualType,
        const char* operation,
        const char* context
    ) {
        if (!globalExplainer) {
            sad_type_explainer_init();
        }
        
        static std::string result;
        result = globalExplainer->explain(
            expectedType ? expectedType : "",
            actualType ? actualType : "",
            operation ? operation : "",
            context ? context : ""
        );
        
        return result.c_str();
    }
    
    /**
     * 📊 الحصول على وصف نوع
     */
    const char* sad_get_type_description(const char* typeName) {
        if (!globalExplainer) {
            sad_type_explainer_init();
        }
        
        static std::string result;
        result = globalExplainer->getTypeDescription(typeName ? typeName : "");
        return result.c_str();
    }
}

} // namespace errors
} // namespace sad
