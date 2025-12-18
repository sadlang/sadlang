# 📂 فهرس قواعد دعم تطوير الأنظمة
# System Development Support Rules Index

**التاريخ / Date:** 2 ديسمبر 2025  
**الإصدار / Version:** 1.0

---

## 📋 نظرة عامة / Overview

### (AR) الوصف
هذا المجلد يحتوي على قواعد Syntax للميزات منخفضة المستوى التي تدعم تطوير أنظمة التشغيل والبرمجة النظامية. هذه القواعد مبنية على المرحلة الأولى من خطة تطوير النظام.

### (EN) Description
This directory contains syntax rules for low-level features supporting OS development and system programming. These rules are based on Stage 1 of the system development plan.

---

## 📚 قائمة الملفات / Files List

### 1️⃣ [المؤشرات / Pointers](01_pointers.md)
- **الموضوع / Topic:** نظام المؤشرات الكامل
- **المحتوى / Content:**
  - تعريف أنواع المؤشرات (pointer<type>)
  - عمليات الحصول على العنوان (address_of)
  - إلغاء المرجعية (value_of)
  - الكتابة عبر المؤشرات (write_to)
  - حساب المؤشرات (pointer arithmetic)
  - مقارنة المؤشرات
  - مؤشرات متعددة المستويات
  - قواعد الأمان والتحقق

### 2️⃣ [العمليات الثنائية / Bitwise Operations](02_bitwise_operations.md)
- **الموضوع / Topic:** العمليات الثنائية على مستوى البت
- **المحتوى / Content:**
  - AND الثنائية (&)
  - OR الثنائية (|)
  - XOR الثنائية (^)
  - NOT الثنائية (~)
  - الإزاحة لليسار (<<)
  - الإزاحة لليمين (>>)
  - دوران البتات
  - عد البتات المفعلة
  - أقنعة الصلاحيات

### 3️⃣ [إدارة الذاكرة / Memory Management](03_memory_management.md)
- **الموضوع / Topic:** تخصيص وإدارة الذاكرة الديناميكية
- **المحتوى / Content:**
  - malloc/free
  - calloc/realloc
  - aligned_malloc
  - mmap/munmap
  - mprotect
  - إدارة المناطق (regions)
  - كشف التسريبات
  - إحصائيات الذاكرة

### 4️⃣ [منافذ الإدخال/الإخراج / I/O Ports](04_io_ports.md)
- **الموضوع / Topic:** الوصول المباشر لمنافذ الأجهزة
- **المحتوى / Content:**
  - inb/outb (8-bit)
  - inw/outw (16-bit)
  - inl/outl (32-bit)
  - io_wait
  - المنافذ الشائعة (PIC, PIT, Keyboard)
  - Serial/Parallel ports
  - VGA ports

### 5️⃣ [معالجة المقاطعات / Interrupt Handling](05_interrupts.md)
- **الموضوع / Topic:** نظام معالجة المقاطعات
- **المحتوى / Content:**
  - تسجيل معالجات المقاطعات
  - IDT (Interrupt Descriptor Table)
  - ISR (Interrupt Service Routines)
  - IRQ management
  - Exception handlers
  - Enable/Disable interrupts
  - Volatile keyword

### 6️⃣ [الأنواع محددة الحجم / Fixed-Size Types](06_fixed_size_types.md)
- **الموضوع / Topic:** أنواع بيانات بأحجام ثابتة
- **المحتوى / Content:**
  - صحيح8 / int8 (8-bit signed)
  - صحيح16 / int16 (16-bit signed)
  - صحيح32 / int32 (32-bit signed)
  - صحيح64 / int64 (64-bit signed)
  - غير_صحيح8 / uint8 (8-bit unsigned)
  - غير_صحيح16 / uint16 (16-bit unsigned)
  - غير_صحيح32 / uint32 (32-bit unsigned)
  - غير_صحيح64 / uint64 (64-bit unsigned)

---

## 🎯 الحالة / Status

| الملف | File | الحالة | Status | التقدم | Progress |
|------|------|--------|--------|--------|----------|
| 01_pointers.md | Pointers | ✅ مكتمل | Complete | 100% | █████████ |
| 02_bitwise_operations.md | Bitwise Ops | ✅ مكتمل | Complete | 100% | █████████ |
| 03_memory_management.md | Memory Mgmt | ✅ مكتمل | Complete | 100% | █████████ |
| 04_io_ports.md | I/O Ports | ✅ مكتمل | Complete | 100% | █████████ |
| 05_interrupts.md | Interrupts | ✅ مكتمل | Complete | 100% | █████████ |
| 06_fixed_size_types.md | Fixed Types | ✅ مكتمل | Complete | 100% | █████████ |

---

## 🔗 الاعتماديات / Dependencies

### خطة المشروع / Project Plan
- [المرحلة 1: الميزات منخفضة المستوى](../../system_build_plan/01_stage1_low_level.md)
- [خارطة الطريق العامة](../../system_build_plan/00_overview.md)

### التنفيذ / Implementation
- `include/low_level/pointer_type.h` - Pointer system
- `include/low_level/bitwise_ops.h` - Bitwise operations
- `include/low_level/memory_map.h` - Memory management
- `include/low_level/io_ports.h` - I/O ports
- `include/low_level/interrupts.h` - Interrupt handling

### القواعد الأساسية / Base Rules
- [أنواع البيانات الأساسية](../01_types.md)
- [الدوال](../02_functions.md)
- [قواعد Syntax](../04_syntax.md)

---

## 📖 كيفية الاستخدام / How to Use

### للمطورين / For Developers
1. **ابدأ بالفهرس** - اقرأ هذا الملف لفهم البنية العامة
2. **اختر الموضوع** - انتقل للملف المناسب حسب احتياجك
3. **راجع EBNF** - افهم القواعد النحوية الرسمية
4. **جرب الأمثلة** - استخدم الأمثلة القابلة للتشغيل
5. **راجع التنفيذ** - تحقق من الواجهة البرمجية في C++

### لمطوري اللغة / For Language Implementers
1. **راجع القواعد** - تأكد من توافق التنفيذ مع EBNF
2. **نفذ الدوال** - استخدم الواجهات المحددة
3. **اختبر الأمثلة** - تأكد من نجاح جميع الأمثلة
4. **راجع الأمان** - نفذ جميع فحوصات الأمان المطلوبة

---

## 🎓 أمثلة سريعة / Quick Examples

### مثال المؤشرات / Pointer Example
```s
# العربية
رقم قيمة = 42
مؤشر<رقم> عنوان = عنوان_من(قيمة)
رقم مقروء = قيمة_من(عنوان)
```

### مثال العمليات الثنائية / Bitwise Example
```s
# العربية
رقم أ = 12      # 1100
رقم ب = 10      # 1010
رقم نتيجة = أ & ب   # 1000 = 8
### مثال العمليات الثنائية / Bitwise Example
```s
# العربية
رقم أ = 12      # 1100
رقم ب = 10      # 1010
رقم نتيجة = أ & ب   # 1000 = 8
```

### مثال إدارة الذاكرة / Memory Example
```s
# العربية
مؤشر<فارغ> ذاكرة = خصص_ذاكرة(1024)
اكتب_في(ذاكرة, 0xFF)
حرر(ذاكرة)
```

---

## 📊 إحصائيات / Statistics

### الملفات / Files
- **إجمالي الملفات / Total Files:** 7 (6 + فهرس)
- **الملفات المكتملة / Completed:** 6
- **قيد العمل / In Progress:** 0
- **قريباً / Upcoming:** 0

### المحتوى / Content
- **إجمالي الصفحات / Total Pages:** ~99 KB توثيق شامل
- **أمثلة / Examples:** 150+ مثال قابل للتشغيل
- **قواعد EBNF / EBNF Rules:** 50+ قاعدة نحوية
- **دوال مضمنة / Built-in Functions:** 30+ دالة عربية

---

## 🔄 التحديثات / Updates

### الإصدار 1.0 (2 ديسمبر 2025)
- ✅ إنشاء البنية الأساسية
- ✅ إكمال ملف المؤشرات
- ✅ إكمال ملف العمليات الثنائية
- 🔄 بدء ملف إدارة الذاكرة

### خطط مستقبلية / Future Plans
- ⏳ إكمال ملفات المرحلة 1
- ⏳ إضافة أمثلة متقدمة
- ⏳ إنشاء ملفات المرحلة 2
- ⏳ توثيق inline assembly

---

## 🤝 المساهمة / Contributing

### إرشادات الكتابة / Writing Guidelines
1. **ثنائي اللغة:** جميع التوثيق بالعربية والإنجليزية
2. **EBNF رسمي:** استخدم قواعد نحوية واضحة
3. **أمثلة عملية:** كل قاعدة لها 2-3 أمثلة قابلة للتشغيل
4. **AST تمثيل:** أضف تمثيل AST للعمليات المعقدة
5. **ملاحظات التنفيذ:** وثق الواجهة البرمجية في C++

---

## 📞 الدعم / Support

### للأسئلة / Questions
- راجع الأمثلة في كل ملف
- تحقق من التنفيذ في `include/low_level/`
- راجع خطة المشروع في `docs/system_build_plan/`

### للإبلاغ عن مشاكل / Report Issues
- أخطاء في القواعد → راجع ملف القواعد المحدد
- أخطاء في الأمثلة → راجع ملف الاختبارات
- اقتراحات → راجع خارطة الطريق

---

## 📜 الترخيص / License

جميع الملفات في هذا المجلد جزء من مشروع لغة ص ومرخصة تحت MIT License.

All files in this directory are part of the Sad Language project and are licensed under the MIT License.

---

**آخر تحديث / Last Update:** 2 ديسمبر 2025  
**المؤلف / Author:** فريق تطوير لغة ص / Sad Language Development Team  
**الإصدار / Version:** 1.0.0
