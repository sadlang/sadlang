/**
 * =============================================================================
 * ملف: property_test_attr.cpp
 * الوصف: محلل سمة اختبار الخصائص #[اختبار_خاصية]
 * المهمة: T244 - Property test attribute parser
 * المرحلة: Phase 25 - User Story 22 (Property Testing)
 * =============================================================================
 * 
 * 🧪 دليل المبتدئ لاختبار الخصائص
 * ═════════════════════════════════
 * 
 * ما هو اختبار الخصائص (Property Testing)؟
 * ──────────────────────────────────────────
 * بدلاً من كتابة حالات اختبار محددة:
 *   assert(add(2, 3) == 5)
 * 
 * نكتب خصائص يجب أن تكون صحيحة لجميع القيم:
 *   ∀a, ∀b: add(a, b) == add(b, a)  // التبادل
 * 
 * ثم النظام يولد قيم عشوائية ويختبرها!
 * 
 * لماذا هذا مفيد؟
 * ─────────────────
 * ✓ يكتشف أخطاء لم نتوقعها
 * ✓ يختبر حالات الحدود تلقائياً
 * ✓ أقل كود اختبار، تغطية أكبر
 * ✓ يجد أصغر حالة فشل (shrinking)
 * 
 * مثال في لغة ص:
 * ────────────────
 * ```ص
 * #[اختبار_خاصية(تكرارات = 100)]
 * دالة خاصية_جمع_تبادلي(أ: عدد, ب: عدد) ← منطقي:
 *     أرجع اجمع(أ, ب) == اجمع(ب, أ)
 * نهاية
 * 
 * #[اختبار_خاصية(بذرة = 42)]
 * دالة خاصية_قائمة_عكس_عكس(قائمة: مصفوفة<عدد>) ← منطقي:
 *     أرجع اعكس(اعكس(قائمة)) == قائمة
 * نهاية
 * ```
 * 
 * السمات المدعومة:
 * ─────────────────
 * #[اختبار_خاصية]                 اختبار أساسي
 * #[اختبار_خاصية(تكرارات = N)]    عدد التكرارات
 * #[اختبار_خاصية(بذرة = N)]       بذرة العشوائية
 * #[اختبار_خاصية(حد_أقصى = N)]   حد حجم القيم
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <optional>
#include <sstream>
#include <regex>
#include <iostream>

namespace sad::parser {

// =============================================================================
// إعدادات اختبار الخصائص
// =============================================================================

struct PropertyTestConfig {
    // عدد التكرارات (كم مرة نولد قيم ونختبر)
    int iterations = 100;
    
    // بذرة العشوائية (للإعادة)
    std::optional<uint64_t> seed;
    
    // الحد الأقصى لحجم القيم المولدة
    int maxSize = 100;
    
    // هل نُظهر تفاصيل؟
    bool verbose = false;
    
    // هل نقلص الفشل؟
    bool shrinkEnabled = true;
    
    // الحد الأقصى لمحاولات التقليص
    int maxShrinks = 100;
    
    // timeout بالثواني
    double timeout = 30.0;
    
    // اسم الخاصية (للعرض)
    std::string name;
};

// =============================================================================
// نوع المعطى للتوليد
// =============================================================================

enum class PropertyParamType {
    Integer,        // عدد
    Float,          // عشري
    Boolean,        // منطقي
    String,         // نص
    Char,           // حرف
    Array,          // مصفوفة
    Optional,       // اختياري
    Tuple,          // صف
    Custom          // نوع مخصص
};

struct PropertyParam {
    std::string name;
    PropertyParamType type;
    std::string customType;  // للأنواع المخصصة
    
    // قيود
    std::optional<int64_t> minValue;
    std::optional<int64_t> maxValue;
    std::optional<size_t> minLength;
    std::optional<size_t> maxLength;
    
    // للمصفوفات والاختياري
    std::optional<PropertyParamType> elementType;
};

// =============================================================================
// محلل السمة
// =============================================================================

class PropertyTestAttrParser {
public:
    /**
     * تحليل سمة #[اختبار_خاصية(...)]
     */
    std::optional<PropertyTestConfig> parse(const std::string& attrText) {
        PropertyTestConfig config;
        
        // تحقق من بداية السمة
        if (!startsWith(attrText, "#[اختبار_خاصية") &&
            !startsWith(attrText, "#[property_test")) {
            return std::nullopt;
        }
        
        // استخراج المعاملات
        auto paramsStart = attrText.find('(');
        auto paramsEnd = attrText.rfind(')');
        
        if (paramsStart != std::string::npos && paramsEnd != std::string::npos) {
            std::string params = attrText.substr(paramsStart + 1, 
                                                  paramsEnd - paramsStart - 1);
            parseParams(params, config);
        }
        
        return config;
    }
    
    /**
     * تحليل معطيات الدالة
     */
    std::vector<PropertyParam> parseParams(const std::string& funcSignature) {
        std::vector<PropertyParam> params;
        
        // regex لاستخراج المعطيات: اسم: نوع
        std::regex paramRegex(R"((\w+)\s*:\s*(\w+)(?:<(\w+)>)?)");
        
        auto begin = std::sregex_iterator(funcSignature.begin(), 
                                           funcSignature.end(), paramRegex);
        auto end = std::sregex_iterator();
        
        for (auto it = begin; it != end; ++it) {
            PropertyParam param;
            param.name = (*it)[1].str();
            param.type = stringToType((*it)[2].str());
            
            if ((*it)[3].matched) {
                param.elementType = stringToType((*it)[3].str());
            }
            
            params.push_back(param);
        }
        
        return params;
    }
    
    /**
     * التحقق من صحة السمة
     */
    struct ValidationResult {
        bool valid;
        std::string error;
    };
    
    ValidationResult validate(const PropertyTestConfig& config) {
        if (config.iterations <= 0) {
            return {false, "عدد التكرارات يجب أن يكون موجباً"};
        }
        
        if (config.iterations > 10000) {
            return {false, "عدد التكرارات كبير جداً (الحد: 10000)"};
        }
        
        if (config.maxSize <= 0) {
            return {false, "الحد الأقصى للحجم يجب أن يكون موجباً"};
        }
        
        if (config.timeout <= 0) {
            return {false, "المهلة يجب أن تكون موجبة"};
        }
        
        return {true, ""};
    }
    
private:
    bool startsWith(const std::string& str, const std::string& prefix) {
        if (str.size() < prefix.size()) return false;
        return str.compare(0, prefix.size(), prefix) == 0;
    }
    
    void parseParams(const std::string& params, PropertyTestConfig& config) {
        // تقسيم بالفاصلة
        std::vector<std::string> parts;
        std::string current;
        
        for (char c : params) {
            if (c == ',') {
                if (!current.empty()) {
                    parts.push_back(trim(current));
                    current.clear();
                }
            } else {
                current += c;
            }
        }
        if (!current.empty()) {
            parts.push_back(trim(current));
        }
        
        // معالجة كل معامل
        for (const auto& part : parts) {
            auto eqPos = part.find('=');
            if (eqPos != std::string::npos) {
                std::string key = trim(part.substr(0, eqPos));
                std::string value = trim(part.substr(eqPos + 1));
                
                if (key == "تكرارات" || key == "iterations") {
                    config.iterations = std::stoi(value);
                }
                else if (key == "بذرة" || key == "seed") {
                    config.seed = std::stoull(value);
                }
                else if (key == "حد_أقصى" || key == "max_size") {
                    config.maxSize = std::stoi(value);
                }
                else if (key == "تفصيل" || key == "verbose") {
                    config.verbose = (value == "صحيح" || value == "true");
                }
                else if (key == "تقليص" || key == "shrink") {
                    config.shrinkEnabled = (value == "صحيح" || value == "true");
                }
                else if (key == "مهلة" || key == "timeout") {
                    config.timeout = std::stod(value);
                }
            }
        }
    }
    
    std::string trim(const std::string& str) {
        auto start = str.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        auto end = str.find_last_not_of(" \t\n\r");
        return str.substr(start, end - start + 1);
    }
    
    PropertyParamType stringToType(const std::string& str) {
        if (str == "عدد" || str == "int" || str == "i32" || str == "i64") {
            return PropertyParamType::Integer;
        }
        if (str == "عشري" || str == "مضاعف" || str == "float" || str == "f32" || str == "f64" || str == "double") {
            return PropertyParamType::Float;
        }
        if (str == "منطقي" || str == "bool") {
            return PropertyParamType::Boolean;
        }
        if (str == "نص" || str == "string" || str == "String") {
            return PropertyParamType::String;
        }
        if (str == "حرف" || str == "char") {
            return PropertyParamType::Char;
        }
        if (str == "مصفوفة" || str == "Vec" || str == "Array") {
            return PropertyParamType::Array;
        }
        if (str == "اختياري" || str == "Option") {
            return PropertyParamType::Optional;
        }
        
        return PropertyParamType::Custom;
    }
};

// =============================================================================
// مستخرج اختبارات الخصائص
// =============================================================================

struct PropertyTestFunction {
    std::string name;
    PropertyTestConfig config;
    std::vector<PropertyParam> params;
    std::string body;
    size_t line;
};

class PropertyTestExtractor {
public:
    /**
     * استخراج جميع اختبارات الخصائص من ملف
     */
    std::vector<PropertyTestFunction> extractFromFile(const std::string& content) {
        std::vector<PropertyTestFunction> tests;
        
        std::istringstream stream(content);
        std::string line;
        size_t lineNum = 0;
        
        PropertyTestAttrParser parser;
        std::optional<PropertyTestConfig> pendingConfig;
        
        while (std::getline(stream, line)) {
            lineNum++;
            
            // البحث عن سمة اختبار الخصائص
            if (line.find("#[اختبار_خاصية") != std::string::npos ||
                line.find("#[property_test") != std::string::npos) {
                pendingConfig = parser.parse(line);
                continue;
            }
            
            // البحث عن دالة بعد السمة
            if (pendingConfig.has_value() && 
                (line.find("دالة ") != std::string::npos ||
                 line.find("fn ") != std::string::npos)) {
                
                PropertyTestFunction test;
                test.config = *pendingConfig;
                test.line = lineNum;
                
                // استخراج اسم الدالة
                size_t nameStart = line.find("دالة ");
                if (nameStart != std::string::npos) {
                    nameStart += 5;  // طول "دالة "
                } else {
                    nameStart = line.find("fn ") + 3;
                }
                
                size_t nameEnd = line.find('(', nameStart);
                if (nameEnd != std::string::npos) {
                    test.name = trim(line.substr(nameStart, nameEnd - nameStart));
                    test.config.name = test.name;
                }
                
                // استخراج المعطيات
                size_t paramsEnd = line.find(')');
                if (nameEnd != std::string::npos && paramsEnd != std::string::npos) {
                    std::string paramsStr = line.substr(nameEnd + 1, 
                                                         paramsEnd - nameEnd - 1);
                    test.params = parser.parseParams(paramsStr);
                }
                
                tests.push_back(test);
                pendingConfig = std::nullopt;
            }
        }
        
        return tests;
    }
    
private:
    std::string trim(const std::string& str) {
        auto start = str.find_first_not_of(" \t");
        if (start == std::string::npos) return "";
        auto end = str.find_last_not_of(" \t");
        return str.substr(start, end - start + 1);
    }
};

// =============================================================================
// مولد كود الاختبار
// =============================================================================

class PropertyTestCodeGen {
public:
    /**
     * توليد كود تشغيل الاختبار
     */
    std::string generateTestRunner(const PropertyTestFunction& test) {
        std::ostringstream code;
        
        code << "// اختبار خاصية مُولد تلقائياً: " << test.name << "\n";
        code << "دالة _اختبر_" << test.name << "() ← منطقي:\n";
        code << "    متغير مولد = مولد_عشوائي_جديد(";
        
        if (test.config.seed.has_value()) {
            code << *test.config.seed;
        } else {
            code << "الوقت_الحالي()";
        }
        code << ")\n";
        
        code << "    متغير نجح = 0\n";
        code << "    متغير فشل = 0\n";
        code << "    \n";
        
        code << "    لكل _ في مدى(0, " << test.config.iterations << "):\n";
        
        // توليد قيم عشوائية
        for (const auto& param : test.params) {
            code << "        متغير " << param.name << " = ";
            code << generateRandomValue(param);
            code << "\n";
        }
        
        // استدعاء الدالة
        code << "        إذا " << test.name << "(";
        for (size_t i = 0; i < test.params.size(); i++) {
            if (i > 0) code << ", ";
            code << test.params[i].name;
        }
        code << "):\n";
        code << "            نجح = نجح + 1\n";
        code << "        وإلا:\n";
        code << "            فشل = فشل + 1\n";
        
        // تقليص عند الفشل
        if (test.config.shrinkEnabled) {
            code << "            // محاولة التقليص\n";
            for (const auto& param : test.params) {
                code << "            " << param.name << " = قلص(" 
                     << param.name << ")\n";
            }
        }
        
        code << "        نهاية\n";
        code << "    نهاية\n";
        code << "    \n";
        code << "    اطبع(\"نجح: \" + نجح + \" فشل: \" + فشل)\n";
        code << "    أرجع فشل == 0\n";
        code << "نهاية\n";
        
        return code.str();
    }
    
private:
    std::string generateRandomValue(const PropertyParam& param) {
        switch (param.type) {
            case PropertyParamType::Integer:
                return "مولد.عدد_عشوائي()";
            case PropertyParamType::Float:
                return "مولد.عشري_عشوائي()";
            case PropertyParamType::Boolean:
                return "مولد.منطقي_عشوائي()";
            case PropertyParamType::String:
                return "مولد.نص_عشوائي()";
            case PropertyParamType::Array:
                return "مولد.مصفوفة_عشوائية()";
            default:
                return "مولد.قيمة_عشوائية()";
        }
    }
};

} // namespace sad::parser

// =============================================================================
// واجهة C
// =============================================================================

extern "C" {

struct SadPropertyTestParser {
    sad::parser::PropertyTestAttrParser* parser;
    sad::parser::PropertyTestExtractor* extractor;
};

SadPropertyTestParser* sad_property_test_parser_new() {
    auto* ctx = new SadPropertyTestParser();
    ctx->parser = new sad::parser::PropertyTestAttrParser();
    ctx->extractor = new sad::parser::PropertyTestExtractor();
    return ctx;
}

int sad_property_test_parse(SadPropertyTestParser* ctx, const char* attr,
                             int* iterations, int* max_size) {
    auto config = ctx->parser->parse(attr);
    if (!config.has_value()) {
        return 0;
    }
    
    *iterations = config->iterations;
    *max_size = config->maxSize;
    return 1;
}

void sad_property_test_parser_free(SadPropertyTestParser* ctx) {
    if (ctx) {
        delete ctx->parser;
        delete ctx->extractor;
        delete ctx;
    }
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef PROPERTY_TEST_ATTR_TEST

#include <cassert>

void testBasicParsing() {
    std::cout << "📝 اختبار التحليل الأساسي...\n";
    
    sad::parser::PropertyTestAttrParser parser;
    
    auto config = parser.parse("#[اختبار_خاصية]");
    assert(config.has_value());
    assert(config->iterations == 100);
    
    std::cout << "   ✅ نجح\n";
}

void testParamsConfig() {
    std::cout << "⚙️ اختبار الإعدادات...\n";
    
    sad::parser::PropertyTestAttrParser parser;
    
    auto config = parser.parse("#[اختبار_خاصية(تكرارات = 500, بذرة = 42)]");
    assert(config.has_value());
    assert(config->iterations == 500);
    assert(config->seed.has_value());
    assert(*config->seed == 42);
    
    std::cout << "   ✅ نجح\n";
}

void testExtraction() {
    std::cout << "🔍 اختبار الاستخراج...\n";
    
    std::string code = R"(
#[اختبار_خاصية(تكرارات = 50)]
دالة خاصية_جمع(أ: عدد, ب: عدد) ← منطقي:
    أرجع اجمع(أ, ب) == اجمع(ب, أ)
نهاية
)";
    
    sad::parser::PropertyTestExtractor extractor;
    auto tests = extractor.extractFromFile(code);
    
    assert(tests.size() == 1);
    assert(tests[0].name == "خاصية_جمع");
    assert(tests[0].config.iterations == 50);
    assert(tests[0].params.size() == 2);
    
    std::cout << "   ✅ نجح\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات محلل سمة اختبار الخصائص\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicParsing();
    testParamsConfig();
    testExtraction();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // PROPERTY_TEST_ATTR_TEST
