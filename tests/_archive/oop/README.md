# اختبارات البرمجة الكائنية (OOP Tests)

## نظرة عامة
هذا المجلد يحتوي على 20 اختبار شامل لميزات البرمجة الكائنية في لغة ص (Sad Language).

## قائمة الاختبارات

### اختبارات أساسية (Basic Tests)
1. **test_01_basic_class.s** - تعريف صنف بسيط مع حقول
2. **test_02_class_with_end.s** - صنف ينتهي بكلمة 'نهاية'
3. **test_03_arabic_semicolon.s** - استخدام الفاصلة المنقوطة العربية (؛)
4. **test_08_empty_class.s** - أصناف فارغة بدون حقول
5. **test_14_no_semicolon_fields.s** - حقول بدون فاصلة منقوطة (اختياري)

### اختبارات الوراثة (Inheritance Tests)
6. **test_04_single_inheritance.s** - وراثة بسيطة مع 'يرث'
7. **test_05_multiple_inheritance_arabic_comma.s** - وراثة متعددة بفاصلة عربية (،)
8. **test_06_multiple_inheritance_english_comma.s** - وراثة متعددة بفاصلة إنجليزية (,)
9. **test_07_mixed_commas.s** - خلط الفواصل العربية والإنجليزية
10. **test_09_colon_inheritance.s** - وراثة مع النقطتين (:)
11. **test_13_diamond_problem.s** - مشكلة الماسة (Diamond Problem)
12. **test_15_long_inheritance_chain.s** - سلسلة وراثة طويلة (10 مستويات)

### اختبارات متقدمة (Advanced Tests)
13. **test_10_complex_hierarchy.s** - تسلسل هرمي معقد مع 5 مستويات
14. **test_11_mixed_field_types.s** - أنواع حقول مختلفة (نص، عدد، عشري، منطق، صفيف)
15. **test_12_arabic_identifiers.s** - أسماء معرفات عربية كاملة
16. **test_16_multiple_classes_one_file.s** - 5 أصناف في ملف واحد
17. **test_17_interface_like.s** - أصناف شبيهة بالواجهات

### أمثلة واقعية (Real World Examples)
18. **test_18_real_world_example.s** - نظام إدارة الموظفين
19. **test_19_animal_hierarchy.s** - تسلسل هرمي للحيوانات

### اختبار شامل (Comprehensive Test)
20. **test_20_comprehensive.s** - اختبار شامل لجميع الميزات

## كيفية تشغيل الاختبارات

### تشغيل اختبار واحد:
```powershell
c:\s\s_language\build\bin\Debug\sad.exe tests\oop\test_01_basic_class.s
```

### تشغيل جميع الاختبارات:
```powershell
Get-ChildItem tests\oop\test_*.s | ForEach-Object {
    Write-Host "`n========== Testing: $($_.Name) ==========" -ForegroundColor Cyan
    c:\s\s_language\build\bin\Debug\sad.exe $_.FullName
}
```

### تشغيل اختبارات محددة:
```powershell
# اختبارات الوراثة فقط
Get-ChildItem tests\oop\test_0[4-9]*.s | ForEach-Object {
    c:\s\s_language\build\bin\Debug\sad.exe $_.FullName
}
```

## الميزات المُختبرة

### ✅ الميزات الأساسية
- [x] تعريف صنف بسيط
- [x] حقول الصنف مع أنواع مختلفة
- [x] استخدام كلمة 'نهاية' لإنهاء الصنف
- [x] استخدام كلمة 'end' لإنهاء الصنف
- [x] أصناف فارغة بدون حقول

### ✅ الوراثة
- [x] وراثة بسيطة مع 'يرث'
- [x] وراثة بسيطة مع ':'
- [x] وراثة متعددة بفاصلة عربية (،)
- [x] وراثة متعددة بفاصلة إنجليزية (,)
- [x] خلط الفواصل في نفس الصنف
- [x] سلاسل وراثة طويلة
- [x] التسلسل الهرمي المعقد

### ✅ علامات الترقيم العربية
- [x] الفاصلة العربية (،) U+060C
- [x] الفاصلة المنقوطة العربية (؛) U+061B
- [x] خلط علامات الترقيم العربية والإنجليزية

### ✅ أنواع البيانات
- [x] نص (String)
- [x] عدد (Integer)
- [x] عشري (Float)
- [x] منطق (Boolean)
- [x] صفيف (Array)

## النتائج المتوقعة

كل اختبار يجب أن:
1. يتم تحليله معجمياً بنجاح (Lexical Analysis)
2. يتم تحليله نحوياً بنجاح (Syntactic Analysis)
3. يطبع رسالة نجاح ✅

## ملاحظات فنية

### UTF-8 Encoding
جميع الملفات محفوظة بترميز UTF-8 لدعم:
- الأحرف العربية
- الفاصلة العربية (U+060C)
- الفاصلة المنقوطة العربية (U+061B)

### المواصفات المرجعية
- **Specification**: `docs/language_spec/rules/03_oop.md`
- **Section**: §1 - Class Declaration Syntax

### الكلمات المفتاحية
- `صنف` / `class` - تعريف صنف
- `نهاية` / `end` - إنهاء تعريف الصنف
- `يرث` / `extends` / `:` - الوراثة

## إحصائيات

- **عدد الاختبارات**: 20
- **عدد الأصناف المُختبرة**: 80+
- **أنماط الوراثة**: 4 (بسيطة، متعددة، مع :، مع يرث)
- **أنواع الفواصل**: 3 (عربية، إنجليزية، مختلطة)

## الصيانة

عند إضافة ميزات جديدة للـ OOP:
1. أضف اختبار جديد بترقيم متسلسل
2. حدّث هذا الـ README
3. شغّل جميع الاختبارات للتأكد من عدم كسر الميزات الحالية

---

آخر تحديث: 23 نوفمبر 2025
