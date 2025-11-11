# 🚀 دليل البدء السريع

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**آخر تحديث:** 4 نوفمبر 2025

---

## 🎯 البدء في 5 دقائق

### للمطور الجديد

```powershell
# 1. Clone المستودع
git clone https://github.com/yourusername/s_language.git
cd s_language

# 2. بناء المشروع
mkdir build
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON
ninja

# 3. تشغيل الاختبارات
ctest --output-on-failure

# 4. افتح المحرر
code ..
```

**تهانينا! 🎉 أنت الآن جاهز للمساهمة.**

---

## 📚 فهم المشروع

### البنية الأساسية (60 ثانية)

```
s_language/
├── include/          → ملفات الهيدر (.h)
│   ├── lexer/       → المحلل المعجمي ✅ مكتمل
│   ├── parser/      → المحلل النحوي ⏳ قادم
│   ├── interpreter/ → المفسر ⏳ قادم
│   └── stdlib/      → المكتبات القياسية ⏳ قادم
│
├── src/              → ملفات المصدر (.cpp)
│   └── (نفس التنظيم)
│
├── tests/            → الاختبارات
├── examples/         → أمثلة البرامج
├── docs/             → التوثيق
└── plans/            → خطط التطوير (هذا المجلد)
```

### تدفق البيانات (30 ثانية)

```
ملف.s → Lexer → Tokens → Parser → AST → Interpreter → Output
```

---

## 🛠️ مهمتك الأولى (30 دقيقة)

### اختر مهمة بسيطة:

#### 1. إصلاح Documentation (سهل)
**الوقت:** 15-30 دقيقة  
**الملفات:** أي ملف `.h`

```powershell
# ابحث عن دوال بدون توثيق
Select-String -Path include/**/*.h -Pattern "^\s*\w+.*\([^)]*\);" | 
    Where-Object { $_.Line -notmatch "/\*\*" }
```

**المطلوب:**
- أضف تعليق Doxygen ثنائي اللغة
- راجع [قالب التوثيق](04_QUALITY_STANDARDS.md#التوثيق-documentation)

---

#### 2. إضافة اختبار (متوسط)
**الوقت:** 30-60 دقيقة  
**الملف:** `tests/lexer_tests/test_lexer.cpp`

```cpp
// أضف اختبار جديد
TEST(LexerTest, TokenizeNegativeNumber) {
    LexerCore lexer("-42");
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(tokens.size(), 2);  // -42 و EOF
    EXPECT_EQ(tokens[0].getType(), TokenType::INTEGER);
    EXPECT_EQ(tokens[0].getLexeme(), "-42");
}
```

**المطلوب:**
- اكتب الاختبار
- تأكد أنه يمر
- أرسل PR

---

#### 3. تنفيذ دالة مساعدة (متقدم)
**الوقت:** 1-2 ساعة  
**الملف:** `src/utils/string_utils.cpp`

```cpp
/**
 * @brief (AR) التحقق من كون النص يحتوي على حروف عربية فقط
 * @brief (EN) Check if string contains only Arabic letters
 */
bool StringUtils::isAllArabic(const std::string& str) {
    // TODO: implement
    return false;
}
```

**المطلوب:**
- نفّذ الدالة
- أضف اختبارات
- وثّق بالكامل

---

## 📝 خطوات إرسال أول PR

### 1. إنشاء فرع (2 دقيقة)

```powershell
cd C:\s\s_language

# تحديث develop
git checkout develop
git pull origin develop

# إنشاء فرعك
git checkout -b feat/docs/add-missing-docs
# أو
git checkout -b feat/tests/lexer-negative-numbers
# أو
git checkout -b feat/utils/is-all-arabic
```

---

### 2. إجراء التعديلات (15-60 دقيقة)

```powershell
# افتح المحرر
code .

# عدّل الملفات المطلوبة
# ...

# احفظ
```

---

### 3. اختبار التغييرات (5 دقائق)

```powershell
# بناء
cd build
ninja

# اختبار
ctest --output-on-failure

# تنسيق (إذا عدّلت كود)
Get-ChildItem -Path ..\src,..\include -Recurse -Include *.cpp,*.h | 
    ForEach-Object { clang-format -i $_.FullName }
```

---

### 4. Commit والـ Push (3 دقائق)

```powershell
# إضافة التغييرات
git add <modified-files>

# Commit
git commit -m "[docs] add missing Doxygen comments for LexerCore"
# أو
git commit -m "[tests] add test for negative number tokenization"

# Push
git push --set-upstream origin feat/docs/add-missing-docs
```

---

### 5. فتح PR على GitHub (5 دقائق)

1. انتقل إلى: `https://github.com/yourusername/s_language`
2. GitHub سيقترح "Compare & pull request" - اضغطه
3. استخدم [قالب PR](06_TEMPLATES.md#قالب-pull-request)
4. املأ الوصف والـ Checklist
5. اضغط "Create pull request"

**تهانينا! أرسلت أول PR 🎉**

---

## 🎓 تعلم المزيد

### وثائق أساسية (اقرأ أولاً)

| الوثيقة | الوقت | الأهمية |
|---------|-------|---------|
| [00_MASTER_PLAN.md](00_MASTER_PLAN.md) | 10 دقائق | 🔴 حرجة |
| [03_WORKFLOW_GUIDE.md](03_WORKFLOW_GUIDE.md) | 15 دقيقة | 🔴 حرجة |
| [04_QUALITY_STANDARDS.md](04_QUALITY_STANDARDS.md) | 20 دقيقة | 🟠 مهمة |
| [06_TEMPLATES.md](06_TEMPLATES.md) | 10 دقائق | 🟠 مهمة |

**الوقت الإجمالي:** ~1 ساعة

---

### وثائق متقدمة (اقرأ لاحقاً)

| الوثيقة | متى تقرأها |
|---------|------------|
| [01_FILES_INVENTORY.md](01_FILES_INVENTORY.md) | عند العمل على مكون جديد |
| [02_PHASE_BREAKDOWN.md](02_PHASE_BREAKDOWN.md) | عند التخطيط لميزة كبيرة |
| [05_BUILD_COMMANDS.md](05_BUILD_COMMANDS.md) | عند مشاكل البناء |
| [07_RISKS_MITIGATION.md](07_RISKS_MITIGATION.md) | عند مشاكل معمارية |

---

### أمثلة الكود (تعلم بالممارسة)

#### مثال 1: كيف يعمل Lexer

```cpp
#include "include/lexer/lexer_core.h"

int main() {
    // 1. إنشاء Lexer
    std::string code = "رقم س = 42";
    Sad::Lexer::LexerCore lexer(code);
    
    // 2. Tokenize
    auto tokens = lexer.tokenize();
    
    // 3. طباعة الرموز
    for (const auto& token : tokens) {
        std::cout << token.toString() << std::endl;
    }
    
    return 0;
}
```

**الإخراج:**
```
INTEGER_TYPE (رقم) at 1:1
IDENTIFIER (س) at 1:5
EQUAL (=) at 1:7
INTEGER (42) at 1:9
EOF () at 1:11
```

---

#### مثال 2: كيف تضيف اختبار

```cpp
// tests/lexer_tests/test_my_feature.cpp

#include <gtest/gtest.h>
#include "../../include/lexer/lexer_core.h"

using namespace Sad::Lexer;

// اختبار بسيط
TEST(MyFeatureTest, BasicTest) {
    LexerCore lexer("test input");
    
    // هنا الاختبار
    EXPECT_TRUE(true);
}

// اختبار مع fixture
class MyFeatureFixture : public ::testing::Test {
protected:
    void SetUp() override {
        // إعداد قبل كل اختبار
    }
};

TEST_F(MyFeatureFixture, AdvancedTest) {
    // اختبار متقدم
    EXPECT_EQ(1, 1);
}
```

---

#### مثال 3: كيف تضيف دالة جديدة

```cpp
// 1. الهيدر: include/utils/string_utils.h
namespace Sad {
namespace Utils {

class StringUtils {
public:
    /**
     * @brief (AR) دالة جديدة
     * @brief (EN) New function
     */
    static bool myNewFunction(const std::string& input);
};

} // namespace Utils
} // namespace Sad
```

```cpp
// 2. التنفيذ: src/utils/string_utils.cpp
#include "../../include/utils/string_utils.h"

namespace Sad {
namespace Utils {

bool StringUtils::myNewFunction(const std::string& input) {
    DEBUG_PRINT("STRING_UTILS", "myNewFunction called");
    
    // التنفيذ هنا
    
    return true;
}

} // namespace Utils
} // namespace Sad
```

```cpp
// 3. الاختبار: tests/utils_tests/test_string_utils.cpp
TEST(StringUtilsTest, MyNewFunction) {
    EXPECT_TRUE(StringUtils::myNewFunction("test"));
    EXPECT_FALSE(StringUtils::myNewFunction(""));
}
```

---

## 🐛 حل المشاكل الشائعة

### المشكلة 1: CMake لا يجد الملفات

```powershell
# الحل: نظّف وأعد البناء
cd C:\s\s_language
Remove-Item -Recurse -Force build
mkdir build
cd build
cmake .. -G "Ninja"
ninja
```

---

### المشكلة 2: الاختبارات تفشل

```powershell
# 1. تأكد من البناء الأخير
cd build
ninja

# 2. شغّل اختبار واحد للتشخيص
.\Debug\sad_tests.exe --gtest_filter=LexerTest.TokenizeInteger

# 3. فعّل DEBUG
cmake .. -DDEBUG=ON
ninja
ctest --verbose
```

---

### المشكلة 3: Git conflicts

```powershell
# 1. حدّث develop
git checkout develop
git pull origin develop

# 2. rebase فرعك
git checkout your-branch
git rebase develop

# 3. حل conflicts يدوياً
# عدّل الملفات...
git add <resolved-files>
git rebase --continue

# 4. force push
git push --force origin your-branch
```

---

### المشكلة 4: clang-format يغير كل شيء

```powershell
# فقط نسّق ملفاتك المعدلة
git diff --name-only | 
    Where-Object { $_ -match '\.(cpp|h)$' } | 
    ForEach-Object { clang-format -i $_ }
```

---

## 💬 الحصول على المساعدة

### قبل السؤال:

1. ✅ راجعت الوثائق؟
2. ✅ بحثت في Issues الموجودة؟
3. ✅ حاولت Google/Stack Overflow؟

### أين تسأل:

#### 1. للأسئلة التقنية:
```
افتح Issue جديد على GitHub:
- عنوان واضح
- وصف المشكلة
- ما حاولت
- البيئة (OS, compiler, etc.)
```

#### 2. للنقاشات:
```
GitHub Discussions (إذا متوفر)
أو Discord channel
```

#### 3. لتحسينات الوثائق:
```
افتح PR مباشرة مع الإصلاح
```

---

## 🎯 الأسئلة الشائعة (FAQ)

### س: هل أحتاج خبرة سابقة في بناء لغات برمجة؟
**ج:** لا! الوثائق شاملة والمجتمع داعم. ابدأ بمهام بسيطة.

---

### س: كم من الوقت أحتاج للمساهمة؟
**ج:** 
- **مهمة صغيرة:** 30 دقيقة - 2 ساعة
- **مهمة متوسطة:** 4-8 ساعات
- **ميزة كاملة:** 20-40 ساعة

---

### س: ماذا لو كسرت شيئاً؟
**ج:** لا مشكلة! 
1. الاختبارات ستكتشف المشكلة
2. Code review سيساعدك
3. Git يسمح بالتراجع

---

### س: هل يجب أن أعرف العربية والإنجليزية؟
**ج:** 
- للبرمجة: الإنجليزية كافية
- للتوثيق: ثنائي اللغة مطلوب (يمكن المساعدة)
- للتواصل: أي لغة مقبولة

---

### س: كيف أختار مهمة مناسبة؟
**ج:** انظر لـ:
- `good first issue` label
- `help wanted` label
- [قائمة الأولويات](00_MASTER_PLAN.md#-الأولويات-الفورية-أول-5-مهام)

---

## 📊 مقاييس تقدمك

### بعد أسبوع:
- [ ] قرأت الوثائق الأساسية
- [ ] بنيت المشروع محلياً
- [ ] فهمت البنية العامة
- [ ] أرسلت أول PR (ولو صغير)

### بعد شهر:
- [ ] ساهمت في 3+ PRs
- [ ] فهمت أحد المكونات بعمق (Lexer/Parser/...)
- [ ] ساعدت مطور آخر
- [ ] راجعت PRs للآخرين

### بعد 3 أشهر:
- [ ] نفذت ميزة متوسطة كاملة
- [ ] مراجع موثوق للـ PRs
- [ ] تساهم في التخطيط
- [ ] تساعد في تطوير الوثائق

---

## 🎉 رحلتك مع لغة "ص"

```
         مبتدئ                متوسط               متقدم              خبير
           │                    │                   │                  │
    ┌──────▼────────┐   ┌───────▼────────┐  ┌──────▼───────┐  ┌──────▼──────┐
    │ أول PR        │   │ ميزة كاملة     │  │ معماري       │  │ Core Team   │
    │ إصلاح docs   │→→ │ اختبارات      │→→│ مراجع PRs   │→→│ قرارات     │
    │ اختبار بسيط  │   │ مساعدة آخرين  │  │ تصميم        │  │ تخطيط       │
    └───────────────┘   └────────────────┘  └──────────────┘  └─────────────┘
         ↑                                                             ↑
         │                     أنت هنا!                               │
         └─────────────────────────────────────────────────────────────┘
```

---

## 📞 جهات الاتصال

- **GitHub:** https://github.com/yourusername/s_language
- **Issues:** https://github.com/yourusername/s_language/issues
- **Discussions:** https://github.com/yourusername/s_language/discussions
- **Email:** dev@sadlang.org (مقترح)

---

## ✨ كلمة أخيرة

**مرحباً بك في مجتمع لغة "ص"!** 🇸🇦

نحن نبني معاً أول لغة برمجة عربية حديثة واحترافية. مساهمتك، مهما كانت صغيرة، تُحدث فرقاً.

**لا تتردد في:**
- طرح الأسئلة
- المحاولة والخطأ
- اقتراح أفكار جديدة
- مساعدة الآخرين

**تذكر:**
- الجودة أهم من السرعة
- التعلم جزء من الرحلة
- الأخطاء فرص للتحسين
- المجتمع هنا للدعم

---

**الآن، ابدأ مساهمتك الأولى! 🚀**

```powershell
git checkout -b feat/my-first-contribution
# ...اكتب كود رائع
git commit -m "[area] my awesome contribution"
git push
# افتح PR وانتظر المراجعة 🎉
```

---

**آخر تحديث:** 4 نوفمبر 2025  
**المرجع:** [الخطة الرئيسية](00_MASTER_PLAN.md)

**لنبني معاً! 🇸🇦✨**
