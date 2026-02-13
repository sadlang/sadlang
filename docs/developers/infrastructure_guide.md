# البنية التحتية الأساسية للغة ص
# Sad Language Core Infrastructure Guide

## دليل المطور الشامل - المرحلة 1 و 2

---

## 📋 الفهرس

1. [مقدمة](#مقدمة)
2. [هيكل المشروع](#هيكل-المشروع)
3. [المحلل المعجمي (Lexer)](#المحلل-المعجمي)
4. [نظام الرموز (Tokens)](#نظام-الرموز)
5. [الكلمات المفتاحية العربية](#الكلمات-المفتاحية-العربية)
6. [دعم Unicode وUTF-8](#دعم-unicode)
7. [مولد التوثيق](#مولد-التوثيق)
8. [الاختبارات](#الاختبارات)
9. [المرجع السريع للمطورين](#المرجع-السريع)

---

## مقدمة

هذا الدليل يشرح البنية التحتية الأساسية للغة ص التي تم بناؤها في المرحلتين الأولى والثانية من خطة التطوير. هذه البنية هي الأساس الذي تُبنى عليه جميع ميزات اللغة.

### ما هي المرحلة 1 و 2؟

| المرحلة | الوصف | الحالة |
|---------|-------|--------|
| **المرحلة 1** | إعداد البنية التحتية للمشروع | ✅ مكتمل |
| **المرحلة 2** | الأساسيات الضرورية (Lexer, Parser, Types) | ✅ مكتمل |

---

## هيكل المشروع

```
s_language/
├── shared/                      # الكود المشترك بين جميع المكونات
│   ├── lexer/                   # المحلل المعجمي
│   │   ├── include/
│   │   │   ├── token.h          # تعريف أنواع الرموز (468 سطر)
│   │   │   ├── lexer_core.h     # واجهة المحلل المعجمي
│   │   │   └── lexer_keywords.h # جدول الكلمات المفتاحية (188 سطر)
│   │   └── src/
│   │       ├── lexer_core.cpp   # تنفيذ المحلل (1551 سطر)
│   │       ├── lexer_keywords.cpp # الكلمات المفتاحية (441 سطر)
│   │       └── token.cpp        # تنفيذ الرموز
│   ├── parser/                  # المحلل النحوي
│   ├── ast/                     # شجرة التركيب المجرد
│   ├── errors/                  # نظام الأخطاء
│   └── types/                   # نظام الأنواع
├── compiler_new/                # المترجم الجديد
├── tools/
│   └── docgen/                  # مولد التوثيق
│       ├── docgen.h             # واجهة المولد (428 سطر)
│       └── docgen.cpp           # التنفيذ (858 سطر)
└── tests/                       # الاختبارات
    ├── test_lexer_comprehensive.py
    └── lexer_tests/
```

---

## المحلل المعجمي (Lexer)

### نظرة عامة

المحلل المعجمي هو المكون الأول في خط معالجة الكود. يحول النص المصدري إلى سلسلة من الرموز (Tokens).

### الملف الرئيسي: `lexer_core.cpp`

```cpp
// مثال على إنشاء واستخدام المحلل المعجمي
#include "lexer_core.h"

using namespace Sad::Lexer;

// إنشاء محلل معجمي جديد
LexerCore lexer("رقم س = 42");

// تحليل الكود
auto tokens = lexer.tokenize();

// طباعة الرموز
for (const auto& token : tokens) {
    std::cout << token.toString() << std::endl;
}
```

### الوظائف الرئيسية

| الوظيفة | الوصف |
|---------|-------|
| `LexerCore(source)` | إنشاء محلل معجمي جديد |
| `tokenize()` | تحليل النص وإرجاع قائمة الرموز |
| `nextToken()` | الحصول على الرمز التالي |
| `peek()` | النظر للحرف الحالي بدون تقدم |
| `advance()` | التقدم حرف واحد |

### دعم اللغة العربية

المحلل المعجمي يدعم:
- ✅ الحروف العربية (أ-ي، ء، آ، إ، ؤ، ئ، ة، ى)
- ✅ الأرقام العربية (٠-٩) والهندية
- ✅ الفاصلة العربية (،) والفاصلة المنقوطة العربية (؛)
- ✅ الكلمات المفتاحية العربية (40+ كلمة)

---

## نظام الرموز (Tokens)

### الملف الرئيسي: `token.h`

يحتوي على تعريف جميع أنواع الرموز المدعومة.

### هيكل الموقع (Position)

```cpp
struct Position {
    size_t line;      // رقم السطر (يبدأ من 1)
    size_t column;    // رقم العمود (يبدأ من 1)
    size_t offset;    // الإزاحة من بداية الملف
    size_t length;    // طول الرمز
    
    std::string toString() const;  // تحويل لنص "سطر:عمود"
};
```

### فئة الرمز (Token)

```cpp
class Token {
public:
    Token(TokenType type, const std::string& value, const Position& position);
    
    TokenType getType() const;      // نوع الرمز
    std::string getValue() const;   // القيمة النصية
    Position getPosition() const;   // الموقع
    
    bool is(TokenType type) const;  // فحص النوع
    bool isKeyword() const;         // هل كلمة مفتاحية؟
    bool isOperator() const;        // هل عامل؟
    bool isLiteral() const;         // هل قيمة حرفية؟
};
```

### أنواع الرموز الرئيسية

#### كلمات مفتاحية - الدوال والبنيات
```cpp
KEYWORD_FUNCTION,       // دالة
KEYWORD_MAIN,           // رئيسية
KEYWORD_RETURN,         // إرجاع
KEYWORD_CLASS,          // صنف
KEYWORD_NEW,            // جديد
KEYWORD_THIS,           // هذا
KEYWORD_CONSTRUCTOR,    // منشئ
KEYWORD_DESTRUCTOR,     // مدمر
KEYWORD_END,            // نهاية
```

#### كلمات مفتاحية - التحكم في التدفق
```cpp
KEYWORD_IF,             // إذا
KEYWORD_ELSE,           // وإلا
KEYWORD_ELSE_IF,        // وإلا_إذا
KEYWORD_WHILE,          // بينما
KEYWORD_FOR,            // لكل
KEYWORD_BREAK,          // اخرج
KEYWORD_CONTINUE,       // استمر
KEYWORD_MATCH,          // طابق
KEYWORD_CASE,           // حالة
```

#### كلمات مفتاحية - Async/Await
```cpp
KEYWORD_ASYNC,          // غير_متزامن
KEYWORD_AWAIT,          // انتظر
```

#### أنواع البيانات
```cpp
TYPE_INTEGER,           // رقم
TYPE_DOUBLE,            // عشري
TYPE_STRING,            // نص
TYPE_BOOLEAN,           // منطقي
TYPE_VOID,              // فراغ
TYPE_ARRAY,             // مصفوفة
TYPE_MAP,               // خريطة
```

#### العوامل
```cpp
OP_PLUS,                // +
OP_MINUS,               // -
OP_MULTIPLY,            // *
OP_DIVIDE,              // /
OP_ASSIGN,              // =
OP_EQUAL,               // ==
OP_NOT_EQUAL,           // !=
OP_AND,                 // &&
OP_OR,                  // ||
```

---

## الكلمات المفتاحية العربية

### الملف الرئيسي: `lexer_keywords.cpp`

يحتوي على أكثر من **40 كلمة مفتاحية عربية** مع نظيراتها الإنجليزية.

### جدول الكلمات المفتاحية

```cpp
class KeywordTable {
public:
    static void initialize();                           // تهيئة الجدول
    static bool isKeyword(const std::string& word);     // هل كلمة مفتاحية؟
    static TokenType getKeywordType(const std::string& word); // الحصول على النوع
    static std::string getKeyword(TokenType type);      // الحصول على الكلمة
    static std::vector<std::string> getAllKeywords();   // جميع الكلمات
};
```

### قائمة الكلمات المفتاحية الكاملة

| الكلمة العربية | المعادل الإنجليزي | الوظيفة |
|---------------|-------------------|---------|
| `دالة` | function | تعريف دالة |
| `رئيسية` | main | نقطة الدخول |
| `إرجاع` | return | إرجاع قيمة |
| `صنف` | class | تعريف صنف |
| `يرث` | inherits | الوراثة |
| `نهاية` | end | إنهاء كتلة |
| `إذا` | if | شرط |
| `وإلا` | else | خلاف ذلك |
| `وإلا_إذا` | elif | شرط إضافي |
| `بينما` | while | حلقة |
| `لكل` | for | حلقة تكرارية |
| `في` | in | داخل مجموعة |
| `طابق` | match | مطابقة أنماط |
| `حالة` | case | حالة |
| `غير_متزامن` | async | دالة غير متزامنة |
| `انتظر` | await | انتظار |
| `حاول` | try | محاولة |
| `امسك` | catch | التقاط خطأ |
| `ارمِ` | throw | رمي استثناء |
| `استورد` | import | استيراد وحدة |
| `صدّر` | export | تصدير |
| `عام` | public | وصول عام |
| `خاص` | private | وصول خاص |
| `محمي` | protected | وصول محمي |
| `ثابت` | const | ثابت |
| `ساكن` | static | ساكن |
| `صحيح` | true | قيمة منطقية |
| `خطأ` | false | قيمة منطقية |
| `عدم` | null | قيمة فارغة |

### دعم الصيغ البديلة

المحلل يدعم أيضاً الصيغ بدون همزة:
- `إذا` = `اذا`
- `وإلا` = `والا`
- `وإلا_إذا` = `والا_اذا`
- `إرجاع` = `ارجع`

---

## دعم Unicode وUTF-8 {#دعم-unicode}

### كيف يعمل؟

المحلل المعجمي يدعم UTF-8 بشكل كامل من خلال:

1. **قراءة متعددة البايت**: التعامل مع الأحرف العربية (2-4 بايت)
2. **التعرف على النطاقات**: تحديد الأحرف العربية من U+0600 إلى U+06FF
3. **الأرقام العربية**: دعم ٠-٩ و 0-9

### مثال على معالجة UTF-8

```cpp
bool LexerCore::isArabicChar(char c) const {
    // التحقق من أن الحرف عربي
    unsigned char uc = static_cast<unsigned char>(c);
    
    // UTF-8 متعدد البايت للعربية
    if (uc >= 0xD8 && uc <= 0xDB) {
        return true;  // نطاق العربية
    }
    return false;
}
```

### الفاصلات العربية

```cpp
COMMA,                  // , فاصلة لاتينية
ARABIC_COMMA,           // ، فاصلة عربية (U+060C)
SEMICOLON,              // ; فاصلة منقوطة لاتينية
ARABIC_SEMICOLON,       // ؛ فاصلة منقوطة عربية (U+061B)
```

---

## مولد التوثيق

### الموقع: `tools/docgen/`

مولد التوثيق يستخرج التعليقات التوثيقية من كود لغة ص وينتج وثائق HTML/Markdown.

### الاستخدام

```cpp
#include "docgen.h"

using namespace sad::docgen;

// استخراج التوثيق من ملف
DocExtractor extractor;
extractor.extract_from_file("مثال.ص");

// توليد HTML
HtmlGenerator html_gen;
html_gen.generate(extractor.get_docs(), "output/");
```

### صيغ التعليقات التوثيقية

```س
### دالة حساب المجموع
### @معامل أ - الرقم الأول
### @معامل ب - الرقم الثاني
### @إرجاع - مجموع الرقمين
دالة اجمع(رقم أ، رقم ب) ترجع رقم
    إرجاع أ + ب
نهاية
```

### أنواع التوثيق المدعومة

| العلامة | الوصف |
|---------|-------|
| `@معامل` | وصف معامل الدالة |
| `@إرجاع` | وصف القيمة المرجعة |
| `@مثال` | مثال على الاستخدام |
| `@ملاحظة` | ملاحظة مهمة |
| `@تحذير` | تحذير للمستخدم |

---

## الاختبارات

### اختبارات المحلل المعجمي

#### Python: `test_lexer_comprehensive.py`

```python
# تشغيل الاختبارات
python tests/test_lexer_comprehensive.py
```

**الاختبارات المتوفرة:**
- ✅ التعرف على الرموز الأساسية
- ✅ الكلمات المفتاحية العربية
- ✅ المعرّفات العربية
- ✅ الأرقام (صحيحة وعشرية)
- ✅ النصوص الحرفية
- ✅ العوامل
- ✅ معالجة الأخطاء

#### C++: `tests/lexer_tests/`

```cpp
// مثال على اختبار C++
TEST(LexerTest, ArabicKeywords) {
    LexerCore lexer("إذا (صحيح) اطبع(\"مرحبا\") نهاية");
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].getType(), TokenType::KEYWORD_IF);
    EXPECT_EQ(tokens[0].getValue(), "إذا");
}
```

### تشغيل جميع الاختبارات

```powershell
# من مجلد المشروع
.\tests\run_comprehensive_tests.ps1
```

---

## المرجع السريع للمطورين

### إضافة كلمة مفتاحية جديدة

1. **أضف النوع في `token.h`:**
```cpp
enum class TokenType {
    // ... 
    KEYWORD_NEW_WORD,  // الكلمة_الجديدة
    // ...
};
```

2. **أضف الكلمة في `lexer_keywords.cpp`:**
```cpp
keywords_["الكلمة_الجديدة"] = TokenType::KEYWORD_NEW_WORD;
keywords_["new_word"] = TokenType::KEYWORD_NEW_WORD;  // اختياري
```

3. **أضف اختباراً:**
```python
# في test_lexer_comprehensive.py
def test_new_keyword():
    result = run_sad_lexer_test("الكلمة_الجديدة")
    assert result['exit_code'] == 0
```

### إضافة عامل جديد

1. **أضف النوع في `token.h`:**
```cpp
OP_NEW_OPERATOR,  // العامل الجديد
```

2. **أضف المعالجة في `lexer_core.cpp`:**
```cpp
case '⊕':  // مثال لعامل Unicode
    return makeToken(TokenType::OP_NEW_OPERATOR, "⊕");
```

### تصحيح الأخطاء (Debugging)

استخدم `DEBUG_PRINT` للتتبع:
```cpp
DEBUG_PRINT("معالجة الرمز: " + token.getValue());
```

فعّل وضع التصحيح في `CMakeLists.txt`:
```cmake
add_definitions(-DDEBUG_MODE)
```

---

## الخلاصة

المرحلتان 1 و 2 توفران البنية التحتية الأساسية للغة ص:

| المكون | الحالة | الملف الرئيسي |
|--------|--------|---------------|
| المحلل المعجمي | ✅ | `lexer_core.cpp` |
| نظام الرموز | ✅ | `token.h` |
| الكلمات المفتاحية | ✅ | `lexer_keywords.cpp` |
| مولد التوثيق | ✅ | `tools/docgen/` |
| الاختبارات | ✅ | `tests/` |

هذه البنية جاهزة لدعم جميع ميزات المراحل التالية.

---

## المراجع

- [مواصفات لغة ص](../specs/)
- [دليل مطابقة الأنماط](pattern_matching_guide.md)
- [دليل نظام الأنواع](../type_system_guide.md)
- [أمثلة](../examples/)

---

**آخر تحديث**: 2026-01-23
**المرحلة**: 1 و 2 (البنية التحتية الأساسية)
**الحالة**: ✅ مكتمل
