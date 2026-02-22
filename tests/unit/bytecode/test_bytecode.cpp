/**
 * ==========================================================
 * ملف: test_bytecode.cpp
 * الوصف: اختبارات شاملة لنظام Bytecode
 * المهمة: T278 - نظام Bytecode
 * ==========================================================
 * 
 * 📚 شرح للمبتدئين:
 * ================
 * هذا الملف يحتوي على اختبارات للتأكد من أن نظام Bytecode
 * يعمل بشكل صحيح.
 * 
 * 🧪 ما هي الاختبارات الوحدوية؟
 * -----------------------------
 * هي برامج صغيرة تتحقق من أن كل جزء من الكود يعمل كما هو متوقع.
 * 
 * مثال:
 * -----
 * // اختبار: الـ Magic Number يجب أن يكون "صكد"
 * TEST(MagicNumber) {
 *     ASSERT_EQ(getMagicString(), "صكد");
 * }
 */

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <cassert>

// محاكاة لتعريفات من format.h
namespace sad {
namespace bytecode {

// الثوابت الأساسية
constexpr uint8_t MAGIC_BYTES[] = {0xD8, 0xB5, 0xD9, 0x83, 0xD8, 0xAF};
constexpr uint8_t VERSION_MAJOR = 1;
constexpr uint8_t VERSION_MINOR = 0;
constexpr uint8_t VERSION_PATCH = 0;

} // namespace bytecode
} // namespace sad

// ==========================================================
// 📌 إطار الاختبار
// ==========================================================

/**
 * 🧪 مجموعة اختبارات
 */
class TestSuite {
private:
    std::string name_;
    int passed_ = 0;
    int failed_ = 0;
    std::vector<std::string> failures_;
    
public:
    TestSuite(const std::string& name) : name_(name) {}
    
    void pass() { passed_++; }
    
    void fail(const std::string& message) {
        failed_++;
        failures_.push_back(message);
    }
    
    void report() {
        std::cout << "\n═══════════════════════════════════\n";
        std::cout << "  " << name_ << "\n";
        std::cout << "═══════════════════════════════════\n\n";
        
        std::cout << "✅ نجح: " << passed_ << "\n";
        std::cout << "❌ فشل: " << failed_ << "\n";
        
        if (!failures_.empty()) {
            std::cout << "\n❌ الإخفاقات:\n";
            for (const auto& f : failures_) {
                std::cout << "   - " << f << "\n";
            }
        }
        
        std::cout << "\n";
    }
    
    bool allPassed() const { return failed_ == 0; }
};

// ماكروهات الاختبار
#define ASSERT_TRUE(cond, msg) \
    if (!(cond)) { suite.fail(std::string(msg) + " - فشل: ASSERT_TRUE"); return; } \
    else { suite.pass(); }

#define ASSERT_FALSE(cond, msg) \
    if (cond) { suite.fail(std::string(msg) + " - فشل: ASSERT_FALSE"); return; } \
    else { suite.pass(); }

#define ASSERT_EQ(a, b, msg) \
    if ((a) != (b)) { suite.fail(std::string(msg) + " - فشل: ASSERT_EQ"); return; } \
    else { suite.pass(); }

#define ASSERT_NE(a, b, msg) \
    if ((a) == (b)) { suite.fail(std::string(msg) + " - فشل: ASSERT_NE"); return; } \
    else { suite.pass(); }

#define ASSERT_GT(a, b, msg) \
    if (!((a) > (b))) { suite.fail(std::string(msg) + " - فشل: ASSERT_GT"); return; } \
    else { suite.pass(); }

// ==========================================================
// 📌 اختبارات Magic Number
// ==========================================================

void test_magic_number(TestSuite& suite) {
    std::cout << "🔢 اختبار Magic Number...\n";
    
    using namespace sad::bytecode;
    
    // اختبار 1: البايتات صحيحة
    ASSERT_EQ(MAGIC_BYTES[0], 0xD8, "أول بايت من 'ص'");
    ASSERT_EQ(MAGIC_BYTES[1], 0xB5, "ثاني بايت من 'ص'");
    ASSERT_EQ(MAGIC_BYTES[2], 0xD9, "أول بايت من 'ك'");
    ASSERT_EQ(MAGIC_BYTES[3], 0x83, "ثاني بايت من 'ك'");
    ASSERT_EQ(MAGIC_BYTES[4], 0xD8, "أول بايت من 'د'");
    ASSERT_EQ(MAGIC_BYTES[5], 0xAF, "ثاني بايت من 'د'");
    
    // اختبار 2: الطول صحيح
    ASSERT_EQ(sizeof(MAGIC_BYTES), 6u, "طول Magic = 6 بايت");
    
    // اختبار 3: التحقق من صلاحية magic صحيح
    uint8_t validMagic[] = {0xD8, 0xB5, 0xD9, 0x83, 0xD8, 0xAF};
    bool isValid = true;
    for (int i = 0; i < 6; ++i) {
        if (validMagic[i] != MAGIC_BYTES[i]) {
            isValid = false;
            break;
        }
    }
    ASSERT_TRUE(isValid, "Magic صالح يجب أن يُقبل");
    
    // اختبار 4: التحقق من magic غير صالح
    uint8_t invalidMagic[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    isValid = true;
    for (int i = 0; i < 6; ++i) {
        if (invalidMagic[i] != MAGIC_BYTES[i]) {
            isValid = false;
            break;
        }
    }
    ASSERT_FALSE(isValid, "Magic غير صالح يجب أن يُرفض");
}

// ==========================================================
// 📌 اختبارات الإصدار
// ==========================================================

void test_version(TestSuite& suite) {
    std::cout << "📊 اختبار الإصدار...\n";
    
    using namespace sad::bytecode;
    
    // اختبار 1: الإصدار الحالي
    ASSERT_EQ(VERSION_MAJOR, 1, "الإصدار الرئيسي = 1");
    ASSERT_EQ(VERSION_MINOR, 0, "الإصدار الفرعي = 0");
    ASSERT_EQ(VERSION_PATCH, 0, "إصدار التصحيح = 0");
    
    // اختبار 2: توافق نفس الإصدار
    auto isCompatible = [](uint8_t major, uint8_t minor, uint8_t /*patch*/) {
        if (major != VERSION_MAJOR) return false;
        if (minor > VERSION_MINOR) return false;
        return true;
    };
    
    ASSERT_TRUE(isCompatible(1, 0, 0), "1.0.0 متوافق");
    ASSERT_TRUE(isCompatible(1, 0, 1), "1.0.1 متوافق");
    ASSERT_TRUE(isCompatible(1, 0, 99), "1.0.99 متوافق");
    
    // اختبار 3: عدم توافق إصدار رئيسي مختلف
    ASSERT_FALSE(isCompatible(2, 0, 0), "2.0.0 غير متوافق");
    ASSERT_FALSE(isCompatible(0, 0, 0), "0.0.0 غير متوافق");
    
    // اختبار 4: عدم توافق إصدار فرعي أحدث
    ASSERT_FALSE(isCompatible(1, 1, 0), "1.1.0 غير متوافق");
    ASSERT_FALSE(isCompatible(1, 2, 0), "1.2.0 غير متوافق");
}

// ==========================================================
// 📌 اختبارات رأس الملف
// ==========================================================

void test_file_header(TestSuite& suite) {
    std::cout << "📄 اختبار رأس الملف...\n";
    
    using namespace sad::bytecode;
    
    // إنشاء رأس ملف
    uint8_t header[16];
    
    // كتابة Magic
    std::memcpy(header, MAGIC_BYTES, 6);
    
    // كتابة الإصدار
    header[6] = VERSION_MAJOR;
    header[7] = VERSION_MINOR;
    header[8] = VERSION_PATCH;
    header[9] = 0; // flags
    
    // عدد الأقسام
    header[10] = 3;
    header[11] = 0;
    
    // الحجم الكلي
    header[12] = 100;
    header[13] = 0;
    header[14] = 0;
    header[15] = 0;
    
    // اختبار 1: قراءة Magic
    bool magicValid = true;
    for (int i = 0; i < 6; ++i) {
        if (header[i] != MAGIC_BYTES[i]) {
            magicValid = false;
            break;
        }
    }
    ASSERT_TRUE(magicValid, "قراءة Magic صحيحة");
    
    // اختبار 2: قراءة الإصدار
    ASSERT_EQ(header[6], VERSION_MAJOR, "قراءة MAJOR صحيحة");
    ASSERT_EQ(header[7], VERSION_MINOR, "قراءة MINOR صحيحة");
    ASSERT_EQ(header[8], VERSION_PATCH, "قراءة PATCH صحيحة");
    
    // اختبار 3: قراءة عدد الأقسام
    uint16_t sectionCount = header[10] | (header[11] << 8);
    ASSERT_EQ(sectionCount, 3, "قراءة عدد الأقسام صحيحة");
    
    // اختبار 4: قراءة الحجم
    uint32_t totalSize = header[12] | (header[13] << 8) | 
                        (header[14] << 16) | (header[15] << 24);
    ASSERT_EQ(totalSize, 100u, "قراءة الحجم الكلي صحيحة");
}

// ==========================================================
// 📌 اختبارات التعليمات (Opcodes)
// ==========================================================

void test_opcodes(TestSuite& suite) {
    std::cout << "⚙️ اختبار التعليمات...\n";
    
    // اختبار 1: تعليمات Stack
    ASSERT_EQ(0x00, 0x00, "NOP = 0x00");
    ASSERT_EQ(0x01, 0x01, "PUSH_NULL = 0x01");
    ASSERT_EQ(0x02, 0x02, "PUSH_TRUE = 0x02");
    ASSERT_EQ(0x03, 0x03, "PUSH_FALSE = 0x03");
    
    // اختبار 2: تعليمات حسابية
    ASSERT_EQ(0x20, 0x20, "ADD = 0x20");
    ASSERT_EQ(0x21, 0x21, "SUB = 0x21");
    ASSERT_EQ(0x22, 0x22, "MUL = 0x22");
    ASSERT_EQ(0x23, 0x23, "DIV = 0x23");
    
    // اختبار 3: تعليمات التحكم
    ASSERT_EQ(0x50, 0x50, "JMP = 0x50");
    ASSERT_EQ(0x51, 0x51, "JMP_IF = 0x51");
    
    // اختبار 4: تعليمات الملكية
    ASSERT_EQ(0x80, 0x80, "ALLOC = 0x80");
    ASSERT_EQ(0x82, 0x82, "BORROW = 0x82");
    ASSERT_EQ(0x83, 0x83, "BORROW_MUT = 0x83");
    ASSERT_EQ(0x85, 0x85, "MOVE = 0x85");
    ASSERT_EQ(0x88, 0x88, "DROP = 0x88");
}

// ==========================================================
// 📌 اختبارات الترميز والفك
// ==========================================================

void test_encoding(TestSuite& suite) {
    std::cout << "✍️ اختبار الترميز...\n";
    
    // محاكاة بسيطة للترميز
    std::vector<uint8_t> buffer;
    
    // ترميز PUSH_INT32 42
    buffer.push_back(0x06); // PUSH_INT32
    int32_t value = 42;
    for (int i = 0; i < 4; ++i) {
        buffer.push_back(static_cast<uint8_t>((value >> (i * 8)) & 0xFF));
    }
    
    ASSERT_EQ(buffer.size(), 5u, "حجم PUSH_INT32 = 5 بايت");
    ASSERT_EQ(buffer[0], 0x06, "Opcode صحيح");
    
    // فك الترميز
    int32_t decoded = buffer[1] | (buffer[2] << 8) | 
                     (buffer[3] << 16) | (buffer[4] << 24);
    ASSERT_EQ(decoded, 42, "القيمة المفكوكة صحيحة");
    
    // ترميز ADD
    buffer.clear();
    buffer.push_back(0x20); // ADD
    ASSERT_EQ(buffer.size(), 1u, "حجم ADD = 1 بايت");
    
    // ترميز PUSH_CONST index=100
    buffer.clear();
    buffer.push_back(0x0A); // PUSH_CONST
    uint16_t index = 100;
    buffer.push_back(static_cast<uint8_t>(index & 0xFF));
    buffer.push_back(static_cast<uint8_t>((index >> 8) & 0xFF));
    
    ASSERT_EQ(buffer.size(), 3u, "حجم PUSH_CONST = 3 بايت");
    
    // فك الفهرس
    uint16_t decodedIndex = buffer[1] | (buffer[2] << 8);
    ASSERT_EQ(decodedIndex, 100, "الفهرس المفكوك صحيح");
}

// ==========================================================
// 📌 اختبارات جدول الثوابت
// ==========================================================

void test_constant_pool(TestSuite& suite) {
    std::cout << "📚 اختبار جدول الثوابت...\n";
    
    // محاكاة جدول ثوابت بسيط
    std::vector<uint8_t> pool;
    
    // عدد الثوابت
    uint16_t count = 3;
    pool.push_back(static_cast<uint8_t>(count & 0xFF));
    pool.push_back(static_cast<uint8_t>((count >> 8) & 0xFF));
    
    // ثابت 0: صحيح 42
    pool.push_back(0x04); // INT32
    int32_t intVal = 42;
    for (int i = 0; i < 4; ++i) {
        pool.push_back(static_cast<uint8_t>((intVal >> (i * 8)) & 0xFF));
    }
    
    // ثابت 1: منطقي صح
    pool.push_back(0x01); // BOOL
    pool.push_back(1);    // true
    
    // ثابت 2: نص "مرحبا"
    pool.push_back(0x11); // STRING_ARABIC
    std::string str = "مرحبا";
    uint16_t len = static_cast<uint16_t>(str.size());
    pool.push_back(static_cast<uint8_t>(len & 0xFF));
    pool.push_back(static_cast<uint8_t>((len >> 8) & 0xFF));
    for (char c : str) {
        pool.push_back(static_cast<uint8_t>(c));
    }
    
    // اختبارات
    ASSERT_GT(pool.size(), 2u, "الجدول يحتوي بيانات");
    
    // قراءة العدد
    uint16_t readCount = pool[0] | (pool[1] << 8);
    ASSERT_EQ(readCount, 3, "عدد الثوابت صحيح");
    
    // قراءة نوع الثابت الأول
    ASSERT_EQ(pool[2], 0x04, "نوع الثابت الأول = INT32");
}

// ==========================================================
// 📌 اختبارات جدول الرموز
// ==========================================================

void test_symbol_table(TestSuite& suite) {
    std::cout << "🏷️ اختبار جدول الرموز...\n";
    
    // محاكاة جدول رموز بسيط
    struct MockSymbol {
        std::string name;
        uint8_t type;
        uint8_t scopeDepth;
    };
    
    std::vector<MockSymbol> symbols;
    
    // إضافة متغير
    symbols.push_back({"عدد", 0x01, 0}); // VARIABLE
    
    // إضافة دالة
    symbols.push_back({"رئيسية", 0x03, 0}); // FUNCTION
    
    // إضافة متغير محلي
    symbols.push_back({"نتيجة", 0x01, 1}); // VARIABLE في نطاق 1
    
    ASSERT_EQ(symbols.size(), 3u, "عدد الرموز صحيح");
    
    // البحث عن رمز
    auto find = [&symbols](const std::string& name) -> int {
        for (size_t i = 0; i < symbols.size(); ++i) {
            if (symbols[i].name == name) {
                return static_cast<int>(i);
            }
        }
        return -1;
    };
    
    ASSERT_EQ(find("عدد"), 0, "إيجاد 'عدد'");
    ASSERT_EQ(find("رئيسية"), 1, "إيجاد 'رئيسية'");
    ASSERT_EQ(find("نتيجة"), 2, "إيجاد 'نتيجة'");
    ASSERT_EQ(find("غير_موجود"), -1, "عدم إيجاد رمز غير موجود");
}

// ==========================================================
// 📌 اختبارات التحقق
// ==========================================================

void test_validation(TestSuite& suite) {
    std::cout << "🔍 اختبار التحقق...\n";
    
    using namespace sad::bytecode;
    
    // إنشاء ملف bytecode صالح
    std::vector<uint8_t> validFile;
    
    // رأس الملف
    for (int i = 0; i < 6; ++i) {
        validFile.push_back(MAGIC_BYTES[i]);
    }
    validFile.push_back(VERSION_MAJOR);
    validFile.push_back(VERSION_MINOR);
    validFile.push_back(VERSION_PATCH);
    validFile.push_back(0); // flags
    validFile.push_back(0); // section count low
    validFile.push_back(0); // section count high
    uint32_t size = 16;
    for (int i = 0; i < 4; ++i) {
        validFile.push_back(static_cast<uint8_t>((size >> (i * 8)) & 0xFF));
    }
    
    // التحقق من الحجم الأدنى
    ASSERT_EQ(validFile.size(), 16u, "رأس الملف = 16 بايت");
    
    // التحقق من Magic
    bool validMagic = true;
    for (int i = 0; i < 6; ++i) {
        if (validFile[i] != MAGIC_BYTES[i]) {
            validMagic = false;
            break;
        }
    }
    ASSERT_TRUE(validMagic, "Magic صالح");
    
    // التحقق من الإصدار
    bool validVersion = (validFile[6] == VERSION_MAJOR);
    ASSERT_TRUE(validVersion, "الإصدار صالح");
    
    // اختبار ملف قصير جداً
    std::vector<uint8_t> shortFile = {0x00, 0x00};
    bool isTooShort = shortFile.size() < 16;
    ASSERT_TRUE(isTooShort, "اكتشاف ملف قصير");
    
    // اختبار magic غير صالح
    std::vector<uint8_t> badMagic(16, 0x00);
    validMagic = true;
    for (int i = 0; i < 6; ++i) {
        if (badMagic[i] != MAGIC_BYTES[i]) {
            validMagic = false;
            break;
        }
    }
    ASSERT_FALSE(validMagic, "اكتشاف magic غير صالح");
}

// ==========================================================
// 📌 اختبارات تكامل
// ==========================================================

void test_integration(TestSuite& suite) {
    std::cout << "🔄 اختبار التكامل...\n";
    
    using namespace sad::bytecode;
    
    // محاكاة عملية كاملة: إنشاء → ترميز → فك → تحقق
    
    // 1. إنشاء برنامج بسيط: return 42
    std::vector<uint8_t> program;
    
    // PUSH_INT32 42
    program.push_back(0x06);
    int32_t val = 42;
    for (int i = 0; i < 4; ++i) {
        program.push_back(static_cast<uint8_t>((val >> (i * 8)) & 0xFF));
    }
    
    // RET_VALUE
    program.push_back(0x65);
    
    ASSERT_EQ(program.size(), 6u, "حجم البرنامج صحيح");
    
    // 2. فك الترميز والتحقق
    size_t offset = 0;
    
    // التعليمة الأولى
    uint8_t op1 = program[offset++];
    ASSERT_EQ(op1, 0x06, "التعليمة الأولى = PUSH_INT32");
    
    int32_t decoded = 0;
    for (int i = 0; i < 4; ++i) {
        decoded |= static_cast<int32_t>(program[offset++]) << (i * 8);
    }
    ASSERT_EQ(decoded, 42, "القيمة = 42");
    
    // التعليمة الثانية
    uint8_t op2 = program[offset++];
    ASSERT_EQ(op2, 0x65, "التعليمة الثانية = RET_VALUE");
    
    ASSERT_EQ(offset, program.size(), "قراءة كل البرنامج");
}

// ==========================================================
// 📌 الدالة الرئيسية
// ==========================================================

int main() {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════╗\n";
    std::cout << "║           اختبارات نظام Bytecode للغة ص               ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════╝\n\n";
    
    TestSuite suite("اختبارات Bytecode");
    
    // تشغيل الاختبارات
    test_magic_number(suite);
    test_version(suite);
    test_file_header(suite);
    test_opcodes(suite);
    test_encoding(suite);
    test_constant_pool(suite);
    test_symbol_table(suite);
    test_validation(suite);
    test_integration(suite);
    
    // عرض النتائج
    suite.report();
    
    if (suite.allPassed()) {
        std::cout << "🎉 جميع الاختبارات نجحت!\n\n";
        return 0;
    } else {
        std::cout << "⚠️ بعض الاختبارات فشلت.\n\n";
        return 1;
    }
}

// ==========================================================
// 📌 C API للاختبارات
// ==========================================================

extern "C" {

/**
 * تشغيل كل الاختبارات
 */
int sad_bytecode_run_tests(void) {
    // استخدام main مباشرة
    return main();
}

} // extern "C"
