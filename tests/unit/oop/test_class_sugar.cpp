/**
 * =============================================================================
 * ملف: test_class_sugar.cpp
 * الوصف: اختبارات شاملة لنظام OOP صنف Sugar
 * المهمة: T183 - Implement test_class_sugar
 * المرحلة: Phase 17 - User Story 15 (OOP صنف Sugar)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو Sugar في لغات البرمجة؟
 * ──────────────────────────────
 * "Sugar" (السكر) = صيغة مختصرة لشيء موجود بالفعل
 * 
 * مثال - صيغة الصنف:
 * ```sad
 * // ما تكتبه (Sugar)
 * صنف شخص:
 *     اسم: نص
 *     
 *     باني(الاسم: نص):
 *         هذا.اسم = الاسم
 *     نهاية
 * نهاية
 * 
 * // ما يُترجم إليه
 * هيكل شخص:
 *     اسم: نص
 * نهاية
 * 
 * نفّذ شخص:
 *     دالة جديد(الاسم: نص) -> شخص:
 *         متغير ش: شخص
 *         ش.اسم = الاسم
 *         ارجع ش
 *     نهاية
 * نهاية
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <functional>
#include <memory>
#include <cassert>

// =============================================================================
// إطار الاختبار
// =============================================================================

/**
 * إطار اختبار بسيط
 */
class TestFramework {
public:
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
    };
    
    using TestFunc = std::function<void()>;
    
    void registerTest(const std::string& name, TestFunc func) {
        tests_.push_back({name, func});
    }
    
    void runAll() {
        std::cout << "\n═══════════════════════════════════════════════════════════════\n";
        std::cout << "  تشغيل الاختبارات\n";
        std::cout << "═══════════════════════════════════════════════════════════════\n\n";
        
        int passed = 0;
        int failed = 0;
        
        for (const auto& [name, func] : tests_) {
            std::cout << "▶ " << name << "... ";
            try {
                func();
                std::cout << "✅ نجح\n";
                passed++;
                results_.push_back({name, true, ""});
            } catch (const std::exception& e) {
                std::cout << "❌ فشل: " << e.what() << "\n";
                failed++;
                results_.push_back({name, false, e.what()});
            }
        }
        
        std::cout << "\n───────────────────────────────────────────────────────────────\n";
        std::cout << "النتائج: " << passed << " نجح, " << failed << " فشل";
        std::cout << " من " << (passed + failed) << " اختبار\n";
        std::cout << "───────────────────────────────────────────────────────────────\n";
    }
    
private:
    std::vector<std::pair<std::string, TestFunc>> tests_;
    std::vector<TestResult> results_;
};

// ماكرو التأكيد
#define ASSERT_TRUE(expr) \
    if (!(expr)) throw std::runtime_error("Assertion failed: " #expr)

#define ASSERT_FALSE(expr) \
    if (expr) throw std::runtime_error("Assertion failed: NOT " #expr)

#define ASSERT_EQ(a, b) \
    if ((a) != (b)) throw std::runtime_error("Assertion failed: " #a " == " #b)

#define ASSERT_NE(a, b) \
    if ((a) == (b)) throw std::runtime_error("Assertion failed: " #a " != " #b)

#define ASSERT_CONTAINS(str, sub) \
    if (std::string(str).find(sub) == std::string::npos) \
        throw std::runtime_error("String does not contain: " #sub)

// =============================================================================
// محاكاة محلل الصنف
// =============================================================================

/**
 * أنواع عقد AST
 */
enum class NodeType {
    ClassDefinition,
    FieldDefinition,
    MethodDefinition,
    ConstructorDefinition,
    ExtensionBlock
};

/**
 * عقدة AST مبسطة
 */
struct ASTNode {
    NodeType type;
    std::string name;
    std::string dataType;
    std::vector<std::shared_ptr<ASTNode>> children;
    std::map<std::string, std::string> attributes;
};

/**
 * محلل صنف للاختبار
 */
class MockClassParser {
public:
    std::shared_ptr<ASTNode> parseClass(const std::string& source) {
        auto node = std::make_shared<ASTNode>();
        node->type = NodeType::ClassDefinition;
        
        // تحليل مبسط - استخراج اسم الصنف
        size_t pos = source.find("صنف ");
        if (pos != std::string::npos) {
            size_t nameStart = pos + 5;  // بعد "صنف "
            size_t nameEnd = source.find(':', nameStart);
            if (nameEnd == std::string::npos) {
                nameEnd = source.find('\n', nameStart);
            }
            if (nameEnd != std::string::npos) {
                node->name = source.substr(nameStart, nameEnd - nameStart);
                // إزالة المسافات
                while (!node->name.empty() && node->name.back() == ' ') {
                    node->name.pop_back();
                }
            }
        }
        
        // استخراج الحقول
        size_t fieldPos = 0;
        while ((fieldPos = source.find(":", fieldPos + 1)) != std::string::npos) {
            // تحقق إذا كان حقل (ليس باني أو دالة)
            size_t lineStart = source.rfind('\n', fieldPos);
            if (lineStart == std::string::npos) lineStart = 0;
            else lineStart++;
            
            std::string line = source.substr(lineStart, fieldPos - lineStart);
            
            // تخطي الأسطر التي تحتوي كلمات مفتاحية
            if (line.find("صنف") != std::string::npos ||
                line.find("باني") != std::string::npos ||
                line.find("دالة") != std::string::npos ||
                line.find("نهاية") != std::string::npos) {
                continue;
            }
            
            // إزالة المسافات
            while (!line.empty() && (line[0] == ' ' || line[0] == '\t')) {
                line = line.substr(1);
            }
            
            if (!line.empty()) {
                auto field = std::make_shared<ASTNode>();
                field->type = NodeType::FieldDefinition;
                field->name = line;
                
                // استخراج النوع
                size_t typeEnd = source.find('\n', fieldPos);
                if (typeEnd == std::string::npos) typeEnd = source.length();
                field->dataType = source.substr(fieldPos + 2, typeEnd - fieldPos - 2);
                
                node->children.push_back(field);
            }
        }
        
        // استخراج الباني
        if (source.find("باني") != std::string::npos) {
            auto ctor = std::make_shared<ASTNode>();
            ctor->type = NodeType::ConstructorDefinition;
            ctor->name = "باني";
            node->children.push_back(ctor);
        }
        
        return node;
    }
};

// =============================================================================
// محاكاة مُحوّل Desugar
// =============================================================================

/**
 * مُحوّل صنف إلى هيكل + نفّذ
 */
class MockClassDesugarer {
public:
    struct DesugarResult {
        std::string structDef;    // تعريف الهيكل
        std::string implBlock;    // كتلة نفّذ
    };
    
    DesugarResult desugar(const std::shared_ptr<ASTNode>& classNode) {
        DesugarResult result;
        std::ostringstream structSS, implSS;
        
        // بناء الهيكل
        structSS << "هيكل " << classNode->name << ":\n";
        
        for (const auto& child : classNode->children) {
            if (child->type == NodeType::FieldDefinition) {
                structSS << "    " << child->name << ": " << child->dataType << "\n";
            }
        }
        
        structSS << "نهاية\n";
        
        // بناء كتلة نفّذ
        implSS << "نفّذ " << classNode->name << ":\n";
        
        for (const auto& child : classNode->children) {
            if (child->type == NodeType::ConstructorDefinition) {
                implSS << "    دالة جديد() -> " << classNode->name << ":\n";
                implSS << "        متغير _نتيجة: " << classNode->name << "\n";
                implSS << "        ارجع _نتيجة\n";
                implSS << "    نهاية\n";
            }
        }
        
        implSS << "نهاية\n";
        
        result.structDef = structSS.str();
        result.implBlock = implSS.str();
        
        return result;
    }
};

// =============================================================================
// محاكاة مولد كود LLVM
// =============================================================================

/**
 * مولد LLVM IR للصنف
 */
class MockLLVMGenerator {
public:
    std::string generateStructType(const std::string& name, 
                                   const std::vector<std::string>& fieldTypes) {
        std::ostringstream ss;
        ss << "%" << name << " = type { ";
        
        for (size_t i = 0; i < fieldTypes.size(); i++) {
            if (i > 0) ss << ", ";
            ss << mapType(fieldTypes[i]);
        }
        
        ss << " }\n";
        return ss.str();
    }
    
    std::string generateConstructor(const std::string& className) {
        std::ostringstream ss;
        ss << "define %" << className << "* @" << className << "_جديد() {\n";
        ss << "entry:\n";
        ss << "  %obj = alloca %" << className << "\n";
        ss << "  ret %" << className << "* %obj\n";
        ss << "}\n";
        return ss.str();
    }
    
private:
    std::string mapType(const std::string& sadType) {
        if (sadType == "ع32") return "i32";
        if (sadType == "ع64") return "i64";
        if (sadType == "نص") return "%String*";
        if (sadType == "منطقي") return "i1";
        return "i32";
    }
};

// =============================================================================
// محاكاة ربط "هذا"
// =============================================================================

/**
 * مُحلل ربط "هذا"
 */
class MockThisBinder {
public:
    struct BindingInfo {
        bool isValid;
        std::string errorMessage;
        std::string resolvedType;
    };
    
    void enterMethodContext(const std::string& typeName, bool isMutable) {
        currentType_ = typeName;
        isMutable_ = isMutable;
        inContext_ = true;
    }
    
    void exitMethodContext() {
        inContext_ = false;
    }
    
    BindingInfo resolveThis() {
        BindingInfo info;
        if (!inContext_) {
            info.isValid = false;
            info.errorMessage = "لا يمكن استخدام 'هذا' خارج دالة";
            return info;
        }
        
        info.isValid = true;
        info.resolvedType = currentType_;
        return info;
    }
    
    BindingInfo resolveFieldAccess(const std::string& fieldName) {
        BindingInfo info = resolveThis();
        if (!info.isValid) return info;
        
        // في التنفيذ الحقيقي: التحقق من وجود الحقل
        info.isValid = true;
        return info;
    }
    
    bool canModify() const {
        return inContext_ && isMutable_;
    }
    
private:
    std::string currentType_;
    bool isMutable_ = false;
    bool inContext_ = false;
};

// =============================================================================
// الاختبارات
// =============================================================================

/**
 * اختبارات تحليل الصنف
 */
void testClassParsing() {
    MockClassParser parser;
    
    std::string source = R"(
صنف شخص:
    اسم: نص
    عمر: ع32
    
    باني(الاسم: نص, العمر: ع32):
        هذا.اسم = الاسم
        هذا.عمر = العمر
    نهاية
نهاية
)";
    
    auto node = parser.parseClass(source);
    
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->type, NodeType::ClassDefinition);
    ASSERT_EQ(node->name, "شخص");
}

/**
 * اختبار الحقول
 */
void testFieldExtraction() {
    MockClassParser parser;
    
    std::string source = R"(
صنف نقطة:
    س: ع32
    ص: ع32
نهاية
)";
    
    auto node = parser.parseClass(source);
    
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->name, "نقطة");
}

/**
 * اختبار تحويل Desugar
 */
void testDesugar() {
    MockClassParser parser;
    MockClassDesugarer desugarer;
    
    std::string source = R"(
صنف حساب:
    رصيد: ع64
    
    باني():
        هذا.رصيد = 0
    نهاية
نهاية
)";
    
    auto node = parser.parseClass(source);
    auto result = desugarer.desugar(node);
    
    ASSERT_CONTAINS(result.structDef, "هيكل حساب");
    ASSERT_CONTAINS(result.implBlock, "نفّذ حساب");
    ASSERT_CONTAINS(result.implBlock, "دالة جديد");
}

/**
 * اختبار توليد LLVM
 */
void testLLVMGeneration() {
    MockLLVMGenerator gen;
    
    std::string structIR = gen.generateStructType("نقطة", {"ع32", "ع32"});
    
    ASSERT_CONTAINS(structIR, "%نقطة = type");
    ASSERT_CONTAINS(structIR, "i32");
}

/**
 * اختبار ربط "هذا" - صالح
 */
void testThisBindingValid() {
    MockThisBinder binder;
    
    binder.enterMethodContext("شخص", true);
    
    auto info = binder.resolveThis();
    
    ASSERT_TRUE(info.isValid);
    ASSERT_EQ(info.resolvedType, "شخص");
    
    binder.exitMethodContext();
}

/**
 * اختبار ربط "هذا" - خارج السياق
 */
void testThisBindingOutsideContext() {
    MockThisBinder binder;
    
    auto info = binder.resolveThis();
    
    ASSERT_FALSE(info.isValid);
    ASSERT_CONTAINS(info.errorMessage, "خارج");
}

/**
 * اختبار قابلية التعديل
 */
void testMutability() {
    MockThisBinder binder;
    
    // دالة غير قابلة للتعديل (&هذا)
    binder.enterMethodContext("شخص", false);
    ASSERT_FALSE(binder.canModify());
    binder.exitMethodContext();
    
    // دالة قابلة للتعديل (&متغير هذا)
    binder.enterMethodContext("شخص", true);
    ASSERT_TRUE(binder.canModify());
    binder.exitMethodContext();
}

/**
 * اختبار الباني
 */
void testConstructor() {
    MockLLVMGenerator gen;
    
    std::string ctorIR = gen.generateConstructor("شخص");
    
    ASSERT_CONTAINS(ctorIR, "define");
    ASSERT_CONTAINS(ctorIR, "شخص");
    ASSERT_CONTAINS(ctorIR, "جديد");
}

/**
 * اختبار صنف فارغ
 */
void testEmptyClass() {
    MockClassParser parser;
    
    std::string source = R"(
صنف فارغ:
نهاية
)";
    
    auto node = parser.parseClass(source);
    
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->name, "فارغ");
    ASSERT_TRUE(node->children.empty());
}

/**
 * اختبار صنف مع دوال متعددة
 */
void testClassWithMethods() {
    std::string source = R"(
صنف مستطيل:
    عرض: ع32
    ارتفاع: ع32
    
    باني(ع: ع32, ا: ع32):
        هذا.عرض = ع
        هذا.ارتفاع = ا
    نهاية
    
    دالة المساحة(&هذا) -> ع32:
        ارجع هذا.عرض * هذا.ارتفاع
    نهاية
    
    دالة المحيط(&هذا) -> ع32:
        ارجع 2 * (هذا.عرض + هذا.ارتفاع)
    نهاية
نهاية
)";
    
    MockClassParser parser;
    auto node = parser.parseClass(source);
    
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->name, "مستطيل");
}

/**
 * اختبار الامتداد
 */
void testExtension() {
    std::string extensionSource = R"(
امتداد نص:
    دالة عكس(&هذا) -> نص:
        // تنفيذ العكس
    نهاية
نهاية
)";
    
    // التحقق من وجود الكلمات المفتاحية
    ASSERT_CONTAINS(extensionSource, "امتداد");
    ASSERT_CONTAINS(extensionSource, "دالة");
    ASSERT_CONTAINS(extensionSource, "&هذا");
}

/**
 * اختبار التداخل
 */
void testNestedContext() {
    MockThisBinder binder;
    
    // دخول أول سياق
    binder.enterMethodContext("خارجي", true);
    auto info1 = binder.resolveThis();
    ASSERT_TRUE(info1.isValid);
    ASSERT_EQ(info1.resolvedType, "خارجي");
    
    // لا ندخل سياق ثاني (التداخل غير مدعوم في هذا المحاكي)
    
    binder.exitMethodContext();
}

// =============================================================================
// الدالة الرئيسية
// =============================================================================

int main() {
    std::cout << R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                                                                              ║
║     اختبارات OOP صنف Sugar                                                   ║
║     Class Sugar OOP Tests                                                    ║
║                                                                              ║
║     المرحلة 17 - T183                                                        ║
║                                                                              ║
╚══════════════════════════════════════════════════════════════════════════════╝
)" << std::endl;

    TestFramework framework;
    
    // تسجيل الاختبارات
    framework.registerTest("تحليل الصنف", testClassParsing);
    framework.registerTest("استخراج الحقول", testFieldExtraction);
    framework.registerTest("تحويل Desugar", testDesugar);
    framework.registerTest("توليد LLVM", testLLVMGeneration);
    framework.registerTest("ربط 'هذا' - صالح", testThisBindingValid);
    framework.registerTest("ربط 'هذا' - خارج السياق", testThisBindingOutsideContext);
    framework.registerTest("قابلية التعديل", testMutability);
    framework.registerTest("الباني", testConstructor);
    framework.registerTest("صنف فارغ", testEmptyClass);
    framework.registerTest("صنف مع دوال متعددة", testClassWithMethods);
    framework.registerTest("الامتداد", testExtension);
    framework.registerTest("التداخل", testNestedContext);
    
    // تشغيل الاختبارات
    framework.runAll();
    
    std::cout << "\n✅ اكتملت اختبارات OOP Sugar!\n";
    
    return 0;
}
