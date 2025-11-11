# 📝 القوالب الجاهزة

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**آخر تحديث:** 4 نوفمبر 2025

---

## 📋 جدول المحتويات

1. [قالب Commit Message](#قالب-commit-message)
2. [قالب Pull Request](#قالب-pull-request)
3. [قالب Issue](#قالب-issue)
4. [قالب Bug Report](#قالب-bug-report)
5. [قالب Feature Request](#قالب-feature-request)
6. [Checklist للـ PR](#checklist-للـ-pr)

---

## 💬 قالب Commit Message

### النموذج الأساسي

```
[component] Short summary (≤50 characters)

Detailed explanation (optional, wrap at 72 chars):
- What was changed
- Why it was changed  
- Any breaking changes or side effects

Related: #issue-number
```

---

### أمثلة

#### مثال 1: ميزة بسيطة
```
[lexer] add UTF-8 support for Arabic identifiers

- Implement isArabicLetter() helper function
- Update scanIdentifier() to handle multi-byte UTF-8
- Add tests for Arabic variable names

Related: #23
```

#### مثال 2: إصلاح خطأ
```
[parser] fix operator precedence for multiplication

The parser was treating * and / with same precedence as + and -.
Fixed by reorganizing the precedence climbing in parseExpression().

Fixes: #45
```

#### مثال 3: بالعربية
```
[مفسر] إضافة دعم الدوال المتداخلة

- تحديث ScopeManager لدعم نطاقات متداخلة غير محدودة
- إضافة اختبارات للدوال المتداخلة حتى 5 مستويات
- إصلاح memory leak في pushScope()
```

---

## 🔀 قالب Pull Request

### قالب كامل

````markdown
## 📝 الوصف / Description

(AR) وصف موجز بالعربية للتغييرات المنفذة.
(EN) Brief description in English of the changes made.

---

## 🎯 نوع التغيير / Change Type

- [ ] ✨ ميزة جديدة / New feature
- [ ] 🐛 إصلاح خطأ / Bug fix
- [ ] 📚 توثيق / Documentation
- [ ] 🎨 تحسين الكود / Code improvement
- [ ] ⚡ تحسين أداء / Performance improvement
- [ ] 🧪 اختبارات / Tests
- [ ] 🔧 صيانة / Maintenance

---

## 📋 التغييرات التفصيلية / Detailed Changes

### الملفات المضافة / Added Files:
- `path/to/new_file.cpp` - وصف مختصر
- `path/to/another_file.h` - وصف مختصر

### الملفات المعدلة / Modified Files:
- `path/to/modified.cpp` - ماذا تغير
- `path/to/another.h` - ماذا تغير

### الملفات المحذوفة / Deleted Files:
- `path/to/old_file.cpp` - لماذا حُذف

---

## 🧪 الاختبارات / Testing

### الاختبارات المضافة / Tests Added:
```cpp
TEST(ComponentTest, NewFeature) {
    // وصف الاختبار
}
```

### كيفية الاختبار / How to Test:
```powershell
cd build
cmake --build . --config Debug
ctest -R component --output-on-failure
```

### نتائج الاختبارات / Test Results:
```
[==========] 15 tests from 3 test suites ran. (234 ms total)
[  PASSED  ] 15 tests.
```

---

## 📊 الإحصائيات / Statistics

- **الأسطر المضافة:** +250
- **الأسطر المحذوفة:** -50
- **الملفات المتأثرة:** 5
- **التغطية:** 85% → 90%

---

## 🖼️ لقطات الشاشة / Screenshots

(إن وجدت - خاصة للميزات المرئية أو لوج DEBUG)

```
[DEBUG][LEXER] → Entering scanNumber()
[DEBUG][LEXER] Found decimal point
[DEBUG][LEXER] Scanned number: 3.14
[DEBUG][LEXER] ← Exiting scanNumber()
```

---

## 🔗 المراجع / References

- Closes #issue-number
- Related to #another-issue
- Depends on #pr-number
- [ملف المعمارية](../معمارية_لغة_ص.md)

---

## ✅ Checklist

- [ ] الكود يبني بدون أخطاء / Code builds without errors
- [ ] جميع الاختبارات تمر / All tests pass
- [ ] التوثيق محدث / Documentation updated
- [ ] DEBUG_PRINT مضاف / DEBUG_PRINT added
- [ ] clang-format مطبق / clang-format applied
- [ ] لا تحذيرات compiler / No compiler warnings
- [ ] الاختبارات تغطي ≥80% / Tests cover ≥80%
- [ ] رسائل commit واضحة / Clear commit messages
- [ ] PR مرتبط بـ issue / PR linked to issue

---

## 💭 ملاحظات إضافية / Additional Notes

(أي ملاحظات أو توضيحات إضافية للمراجعين)

---

## 👥 المراجعون المقترحون / Suggested Reviewers

@reviewer1 @reviewer2

---

**تاريخ الإنشاء:** YYYY-MM-DD  
**المطور:** @your-username
````

---

### مثال PR حقيقي

````markdown
## 📝 الوصف

(AR) إضافة المحلل النحوي الأساسي (Parser Core) مع دوال المساعدة الأساسية.
(EN) Add Parser Core with basic helper functions.

تنفيذ الهيكل الأساسي للمحلل النحوي مع الدوال:
- `peek()`, `advance()`, `match()`, `consume()`
- معالجة أخطاء أولية
- رسائل خطأ بالعربية

---

## 🎯 نوع التغيير

- [x] ✨ ميزة جديدة
- [ ] 🐛 إصلاح خطأ
- [ ] 📚 توثيق

---

## 📋 التغييرات التفصيلية

### الملفات المضافة:
- `include/parser/parser_core.h` - واجهة المحلل النحوي
- `src/parser/parser_core.cpp` - تنفيذ الدوال الأساسية
- `tests/parser_tests/test_parser_core.cpp` - اختبارات وحدة

### الملفات المعدلة:
- `CMakeLists.txt` - إضافة parser targets

---

## 🧪 الاختبارات

### الاختبارات المضافة:
```cpp
TEST(ParserCoreTest, PeekReturnsCurrentToken)
TEST(ParserCoreTest, AdvanceMovesToNext)
TEST(ParserCoreTest, MatchSuccessful)
TEST(ParserCoreTest, ConsumeThrowsOnMismatch)
```

### كيفية الاختبار:
```powershell
cd build
ninja
ctest -R parser --output-on-failure
```

### نتائج الاختبارات:
```
[==========] 12 tests from ParserCoreTest ran. (45 ms total)
[  PASSED  ] 12 tests.
```

---

## 📊 الإحصائيات

- **الأسطر المضافة:** +420
- **الملفات:** 3 جديدة
- **التغطية:** 92%

---

## 🖼️ لقطات لوج DEBUG

```
[DEBUG][PARSER] → Entering peek()
[DEBUG][PARSER] Current token: INTEGER (42)
[DEBUG][PARSER] ← Exiting peek()
[DEBUG][PARSER] → Entering advance()
[DEBUG][PARSER] Advanced from position 0 to 1
[DEBUG][PARSER] ← Exiting advance()
```

---

## 🔗 المراجع

- Part of #15 (Parser implementation)
- Depends on Lexer (already merged)

---

## ✅ Checklist

- [x] الكود يبني بدون أخطاء
- [x] جميع الاختبارات تمر (12/12)
- [x] التوثيق Doxygen كامل
- [x] DEBUG_PRINT في كل دالة
- [x] clang-format مطبق
- [x] لا تحذيرات
- [x] التغطية 92%

---

## 💭 ملاحظات

هذا الـ PR يُكمل المرحلة الأولى من Parser. المرحلة التالية: `parseExpression()`.

---

**تاريخ الإنشاء:** 2025-11-05  
**المطور:** @dev-name
````

---

## 🐛 قالب Bug Report

### قالب Issue للأخطاء

````markdown
---
name: 🐛 Bug Report / تقرير خطأ
about: الإبلاغ عن خطأ برمجي / Report a bug
title: '[BUG] '
labels: bug
assignees: ''
---

## 🐛 وصف الخطأ / Bug Description

(AR) وصف واضح ومختصر للخطأ.
(EN) A clear and concise description of the bug.

---

## 🔄 خطوات إعادة الإنتاج / Steps to Reproduce

1. اذهب إلى... / Go to...
2. قم بـ... / Do...
3. الخطأ يحدث / Error occurs

---

## ✅ السلوك المتوقع / Expected Behavior

(AR) ماذا كان يجب أن يحدث.
(EN) What should have happened.

---

## ❌ السلوك الفعلي / Actual Behavior

(AR) ماذا حدث بالفعل.
(EN) What actually happened.

---

## 💻 البيئة / Environment

- **OS:** Windows 11 / Windows 10 / Linux / macOS
- **Compiler:** MSVC 2022 / GCC 11 / Clang 14
- **CMake Version:** 3.25
- **Branch/Commit:** develop / feat/... / commit-hash

---

## 📝 كود المثال / Code Example

```cpp
// الكود الذي يسبب الخطأ
LexerCore lexer("\"unclosed string);
auto tokens = lexer.tokenize(); // يُسبب crash
```

---

## 📋 رسالة الخطأ / Error Message

```
terminate called after throwing an instance of 'std::runtime_error'
  what():  Unclosed string at line 1
```

---

## 📸 لقطات شاشة / Screenshots

(إن وجدت)

---

## 🔍 معلومات إضافية / Additional Context

(أي معلومات أخرى مفيدة)

---

## 🎯 الأولوية المقترحة / Suggested Priority

- [ ] 🔴 حرجة / Critical (يمنع الاستخدام)
- [ ] 🟠 عالية / High (يؤثر على وظيفة أساسية)
- [ ] 🟡 متوسطة / Medium (مزعج لكن يمكن تجاوزه)
- [ ] 🟢 منخفضة / Low (تأثير طفيف)
````

---

## ✨ قالب Feature Request

````markdown
---
name: ✨ Feature Request / طلب ميزة
about: اقتراح ميزة جديدة / Suggest a new feature
title: '[FEATURE] '
labels: enhancement
assignees: ''
---

## 💡 الميزة المقترحة / Feature Description

(AR) وصف واضح ومفصل للميزة المقترحة.
(EN) A clear and detailed description of the feature.

---

## 🎯 المشكلة التي تحلها / Problem It Solves

(AR) ما المشكلة التي ستحلها هذه الميزة؟
(EN) What problem does this feature solve?

**مثال:**
حالياً، لا يمكن استخدام... / Currently, it's not possible to...

---

## 🔧 الحل المقترح / Proposed Solution

(AR) كيف ترى تنفيذ هذه الميزة؟
(EN) How do you envision implementing this feature?

**مثال كود:**
```s
# مثال على الاستخدام المقترح
دالة جديدة(معامل) = نوع
    # ...
نهاية
```

---

## 🔀 بدائل / Alternatives

(AR) هل هناك بدائل أو حلول مؤقتة؟
(EN) Are there alternatives or workarounds?

---

## 📊 تأثير الميزة / Feature Impact

- **المستخدمون المتأثرون:** جميع المستخدمين / مطورو المكتبات / ...
- **حجم التغيير:** صغير / متوسط / كبير
- **التوافق:** لا يكسر الكود الحالي / يحتاج تحديثات / ...

---

## ✅ معايير القبول / Acceptance Criteria

- [ ] يمكن استخدام... / Can use...
- [ ] يعمل مع... / Works with...
- [ ] لا يكسر... / Doesn't break...
- [ ] موثق بالكامل / Fully documented
- [ ] اختبارات شاملة / Comprehensive tests

---

## 🎯 الأولوية المقترحة / Suggested Priority

- [ ] 🔴 عالية / High (ميزة أساسية)
- [ ] 🟠 متوسطة / Medium (مفيدة جداً)
- [ ] 🟡 منخفضة / Low (nice-to-have)

---

## 📚 مراجع / References

- مشاريع مشابهة / Similar projects
- مقالات / Articles
- مناقشات / Discussions
````

---

## ✅ Checklist للـ PR

### قائمة فحص ما قبل PR

````markdown
## ✅ Pre-PR Checklist

### البناء والأداء / Build & Performance
- [ ] `cmake --build build` ينجح بدون أخطاء
- [ ] لا توجد تحذيرات compiler
- [ ] الكود يعمل على Windows
- [ ] لا memory leaks (إذا تم الفحص)

### الاختبارات / Testing
- [ ] `ctest` جميع الاختبارات تمر
- [ ] اختبارات جديدة للميزات الجديدة
- [ ] اختبارات edge cases
- [ ] اختبارات error handling
- [ ] التغطية ≥ 80% للكود الجديد

### التوثيق / Documentation
- [ ] كل دالة عامة موثقة (AR + EN)
- [ ] أمثلة استخدام موجودة
- [ ] رأس الملف محدث
- [ ] CHANGELOG محدث (إن وُجد)
- [ ] README محدث (إذا لزم)

### جودة الكود / Code Quality
- [ ] clang-format مطبق
- [ ] أسماء واضحة ومعبرة
- [ ] لا code duplication
- [ ] معالجة أخطاء مناسبة
- [ ] DEBUG_PRINT في نقاط حرجة
- [ ] لا hardcoded values (استخدم constants)

### Git & Workflow
- [ ] رسائل commit واضحة
- [ ] الفرع محدث من develop
- [ ] لا merge conflicts
- [ ] PR description كامل
- [ ] PR مرتبط بـ issue

### المراجعة / Review
- [ ] الكود جاهز للمراجعة
- [ ] حجم PR مناسب (< 500 سطر)
- [ ] مراجع ذاتية تمت
- [ ] تعليقات واضحة للكود المعقد
````

---

### قائمة فحص للمراجع

````markdown
## 👀 Reviewer Checklist

### الوظيفة / Functionality
- [ ] الكود يحقق الهدف المطلوب
- [ ] لا توجد أخطاء منطقية
- [ ] Edge cases معالجة
- [ ] Error handling مناسب

### الجودة / Quality
- [ ] الكود واضح وقابل للقراءة
- [ ] الأسماء معبرة
- [ ] التعليقات مفيدة (إن وُجدت)
- [ ] لا code smells
- [ ] يتبع نمط المشروع

### الاختبارات / Tests
- [ ] الاختبارات شاملة
- [ ] الاختبارات تمر
- [ ] اختبارات edge cases موجودة
- [ ] لا اختبارات زائدة

### التوثيق / Documentation
- [ ] التوثيق كامل وواضح
- [ ] أمثلة صحيحة
- [ ] ثنائي اللغة (AR + EN)

### الأداء / Performance
- [ ] لا اختناقات أداء واضحة
- [ ] الخوارزميات مناسبة
- [ ] استخدام الذاكرة معقول

### الأمان / Security
- [ ] لا ثغرات أمنية واضحة
- [ ] Input validation موجود
- [ ] لا buffer overflows

### القرار / Decision
- [ ] ✅ Approve (موافقة)
- [ ] 💬 Comment (تعليقات)
- [ ] 🔄 Request Changes (طلب تعديلات)
````

---

## 📋 قوالب ملفات Git

### .gitignore

```gitignore
# Build directories
build/
out/
bin/
lib/

# IDE
.vscode/
.vs/
*.suo
*.user
*.userosscache
*.sln.docstates

# Compiled files
*.obj
*.o
*.exe
*.dll
*.so
*.dylib
*.a
*.lib

# CMake
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
compile_commands.json

# Tests
test_results/
coverage/
*.gcda
*.gcno

# Logs
*.log
```

---

### .gitattributes

```gitattributes
# Auto detect text files and normalize line endings to LF
* text=auto

# Source files
*.cpp text
*.h text
*.hpp text
*.c text
*.s text

# Scripts
*.sh text eol=lf
*.ps1 text eol=crlf

# Documents
*.md text
*.txt text

# Binary files
*.exe binary
*.dll binary
*.so binary
*.a binary
*.lib binary
```

---

## 🎯 ملخص سريع

### عند Commit:
```
[component] short message

Details (optional)

Related: #issue
```

### عند فتح PR:
1. استخدم [قالب PR](#قالب-pull-request)
2. املأ [Checklist](#checklist-للـ-pr)
3. أرفق لقطات/لوج إن لزم
4. اربط بـ issue

### عند الإبلاغ عن Bug:
1. استخدم [قالب Bug Report](#قالب-bug-report)
2. ضع خطوات إعادة الإنتاج
3. أرفق رسالة الخطأ
4. حدد الأولوية

---

**آخر تحديث:** 4 نوفمبر 2025  
**المرجع:** [الخطة الرئيسية](00_MASTER_PLAN.md) | [سير العمل](03_WORKFLOW_GUIDE.md)
