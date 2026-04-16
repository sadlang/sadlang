## وصف التغييرات / Description of Changes
<!-- صف التغييرات التي أجريتها بوضوح / Clearly describe the changes you made -->


## نوع التغيير / Type of Change
<!-- ضع علامة على النوع المناسب / Check the appropriate type -->
- [ ] 🐛 إصلاح خطأ / Bug fix
- [ ] ✨ ميزة جديدة / New feature
- [ ] ♻️ إعادة هيكلة / Refactoring
- [ ] 📝 توثيق / Documentation
- [ ] 🧪 اختبارات / Tests
- [ ] 🏗️ بنية تحتية / Infrastructure (CI/CD, build)

## المكون المتأثر / Affected Component
- [ ] المفسر / Interpreter
- [ ] المترجم / Compiler (sadc)
- [ ] النحو والمعجم / Lexer & Parser
- [ ] نظام الأنواع / Type System (Value)
- [ ] المكتبة القياسية / stdlib
- [ ] الرسوميات / Graphics
- [ ] الأدوات / Tools (LSP, Formatter, REPL, pkg)
- [ ] الآلة الافتراضية / VM
- [ ] البناء / Build System (CMake)
- [ ] التوثيق / Documentation

## Issue مرتبط / Related Issue
<!-- اربط بـ Issue إذا وُجد / Link to related issue if any -->
Closes #

## كيفية الاختبار / How to Test
<!-- كيف يمكن للمراجعين اختبار هذا التغيير؟ / How can reviewers test this change? -->

1. 
2. 
3. 

## كود ص للاختبار / Sad Code for Testing (if applicable)
```sad
# ضع كود لغة ص لاختبار التغيير
# Place Sad code to test the change
```

## قائمة التحقق / Checklist

### إلزامي / Required ✅
- [ ] الكود يبنى بدون أخطاء / Code builds without errors
- [ ] جميع الاختبارات تنجح (لا تراجع) / All tests pass (no regression)
- [ ] قرأت CONTRIBUTING.md / Read CONTRIBUTING.md
- [ ] التعليقات ثنائية اللغة (عربي + إنجليزي) / Bilingual comments (AR + EN)

### جودة الكود / Code Quality
- [ ] لا ملف يتجاوز 800 سطر (CW-05) / No file exceeds 800 lines
- [ ] لا أرقام سحرية (CW-10) / No magic numbers
- [ ] لا تكرار كود (CW-19) / No code duplication
- [ ] لا تحذيرات جديدة / No new warnings

### ميزة جديدة / New Feature (if applicable)
- [ ] أضفت اختبارات تغطي الميزة / Added tests covering the feature
- [ ] أضفت توثيق `@brief (AR)` + `@brief (EN)` / Added bilingual docs

### إصلاح خطأ / Bug Fix (if applicable)
- [ ] أضفت اختبار يعيد إنتاج الخطأ / Added test reproducing the bug
- [ ] أضفت تعليق يشرح السبب الجذري (BF-14) / Added root cause comment

