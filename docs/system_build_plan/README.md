# خطة تطوير لغة ص لبناء أنظمة التشغيل  
# Sad Language OS Development Plan

📁 **المسار / Path:** `docs/system_build_plan/`  
📅 **آخر تحديث / Last Updated:** ديسمبر 2025 / December 2025

---

## 📖 حول هذه الخطة / About This Plan

### العربية

هذا المجلد يحتوي على خطة شاملة ومفصلة لتحويل لغة ص إلى لغة برمجة قادرة على بناء أنظمة تشغيل كاملة. الخطة مقسمة إلى 7 مراحل رئيسية تغطي جميع الجوانب التقنية الضرورية.

**الرؤية:** تمكين المبرمجين العرب من كتابة أنظمة تشغيل كاملة باللغة العربية.

**المدة المتوقعة:** 18-24 شهراً

### English

This folder contains a comprehensive and detailed plan to transform Sad Language into a programming language capable of building complete operating systems. The plan is divided into 7 main stages covering all necessary technical aspects.

**Vision:** Enable Arab programmers to write complete operating systems in Arabic.

**Expected Duration:** 18-24 months

---

## 📂 محتويات المجلد / Folder Contents

| الملف / File | الوصف العربي | English Description |
|-------------|---------------|---------------------|
| `00_overview.md` | نظرة عامة على المشروع | Project overview |
| `01_stage1_low_level.md` | المرحلة 1: دعم الميزات منخفضة المستوى | Stage 1: Low-Level Features |
| `02_stage2_compiler.md` | المرحلة 2: مترجم مستقل | Stage 2: Native Compiler |
| `03_stage3_stdlib.md` | المرحلة 3: المكتبة القياسية للنظام | Stage 3: System Library |
| `04_stage4_assembly.md` | المرحلة 4: دعم لغة التجميع | Stage 4: Assembly Support |
| `05_stage5_toolchain.md` | المرحلة 5: أدوات البناء | Stage 5: Build Toolchain |
| `06_stage6_kernel.md` | المرحلة 6: Kernel تجريبي | Stage 6: Experimental Kernel |
| `07_stage7_documentation.md` | المرحلة 7: التوثيق والمجتمع | Stage 7: Documentation |
| `08_technical_specs.md` | المواصفات التقنية | Technical Specifications |
| `09_timeline_milestones.md` | الجدول الزمني | Timeline & Milestones |
| `README.md` | هذا الملف / This file | - |

---

## 🚀 كيفية البدء / Getting Started

### العربية

1. **اقرأ النظرة العامة:** ابدأ بملف `00_overview.md`
2. **راجع المراحل بالترتيب:** من المرحلة 1 إلى 7
3. **افهم المواصفات:** راجع `08_technical_specs.md`
4. **خطط للتنفيذ:** استخدم `09_timeline_milestones.md`
5. **ابدأ التطوير:** ابدأ بالمرحلة 1

### English

1. **Read Overview:** Start with `00_overview.md`
2. **Review Stages:** From Stage 1 to 7 in order
3. **Understand Specs:** Review `08_technical_specs.md`
4. **Plan Implementation:** Use `09_timeline_milestones.md`
5. **Start Development:** Begin with Stage 1

---

## 📊 ملخص المراحل / Stage Summary

### المرحلة 1: دعم الميزات منخفضة المستوى (3-4 أشهر)

**العربية:** إضافة مؤشرات، عمليات bitwise، memory mapping، I/O ports، interrupts

**English:** Add pointers, bitwise operations, memory mapping, I/O ports, interrupts

**الحالة:** 🔴 لم تبدأ

---

### المرحلة 2: مترجم مستقل (4-5 أشهر)

**العربية:** تطوير مترجم ينتج ملفات تنفيذية native باستخدام LLVM

**English:** Develop compiler producing native executables using LLVM

**الحالة:** 🔴 لم تبدأ

---

### المرحلة 3: المكتبة القياسية للنظام (3-4 أشهر)

**العربية:** مكتبات لإدارة الذاكرة، العمليات، الملفات، الشبكات، الأجهزة

**English:** Libraries for memory, processes, files, networking, devices

**الحالة:** 🔴 لم تبدأ

---

### المرحلة 4: دعم لغة التجميع (2-3 أشهر)

**العربية:** inline assembly و الربط مع ملفات asm خارجية

**English:** Inline assembly and linking with external asm files

**الحالة:** 🔴 لم تبدأ

---

### المرحلة 5: أدوات البناء والتجميع (2-3 أشهر)

**العربية:** cross-compiler، linker، bootloader integration

**English:** Cross-compiler, linker, bootloader integration

**الحالة:** 🔴 لم تبدأ

---

### المرحلة 6: Kernel تجريبي (2-3 أشهر)

**العربية:** بناء kernel بسيط يعمل على QEMU

**English:** Build simple kernel running on QEMU

**الحالة:** 🔴 لم تبدأ

---

### المرحلة 7: التوثيق والمجتمع (مستمر)

**العربية:** توثيق شامل، أمثلة، دروس، دعم المجتمع

**English:** Comprehensive docs, examples, tutorials, community support

**الحالة:** 🔴 لم تبدأ

---

## 🎯 الأهداف الرئيسية / Main Goals

### العربية

✅ **عند اكتمال المشروع:**

1. لغة ص تنتج ملفات تنفيذية native سريعة
2. دعم كامل للبرمجة منخفضة المستوى
3. مكتبة قياسية شاملة للنظام
4. kernel تجريبي يعمل على x86-64
5. توثيق كامل بالعربية والإنجليزية
6. مجتمع نشط من المطورين
7. أمثلة وdrivers عملية

### English

✅ **Upon Project Completion:**

1. Sad produces fast native executables
2. Complete low-level programming support
3. Comprehensive system standard library
4. Experimental kernel running on x86-64
5. Complete documentation in Arabic and English
6. Active developer community
7. Practical examples and drivers

---

## 📈 مؤشرات التقدم / Progress Indicators

```
[▱▱▱▱▱▱▱▱▱▱] 0%  - المرحلة 1: دعم الميزات منخفضة المستوى
[▱▱▱▱▱▱▱▱▱▱] 0%  - المرحلة 2: مترجم مستقل
[▱▱▱▱▱▱▱▱▱▱] 0%  - المرحلة 3: المكتبة القياسية
[▱▱▱▱▱▱▱▱▱▱] 0%  - المرحلة 4: دعم لغة التجميع
[▱▱▱▱▱▱▱▱▱▱] 0%  - المرحلة 5: أدوات البناء
[▱▱▱▱▱▱▱▱▱▱] 0%  - المرحلة 6: Kernel تجريبي
[▱▱▱▱▱▱▱▱▱▱] 0%  - المرحلة 7: التوثيق
────────────────────────────────────────
[▱▱▱▱▱▱▱▱▱▱] 0%  - الإجمالي / Total
```

---

## 🤝 كيفية المساهمة / How to Contribute

### العربية

1. **اختر مرحلة:** ابدأ بالمرحلة 1 أو اختر مهمة محددة
2. **اقرأ المواصفات:** راجع الملف الخاص بالمرحلة
3. **ناقش التصميم:** تواصل مع الفريق
4. **نفذ المهمة:** اتبع معايير الكود
5. **اختبر:** اكتب اختبارات شاملة
6. **أرسل PR:** مع توثيق كامل

### English

1. **Choose Stage:** Start with Stage 1 or pick specific task
2. **Read Specs:** Review stage file
3. **Discuss Design:** Connect with team
4. **Implement:** Follow code standards
5. **Test:** Write comprehensive tests
6. **Submit PR:** With full documentation

---

## 📚 موارد تعليمية / Learning Resources

### العربية

#### للمبتدئين
- [أساسيات أنظمة التشغيل](https://wiki.osdev.org/Getting_Started)
- [تعلم لغة التجميع](https://www.nasm.us/doc/)
- [مقدمة لـ LLVM](https://llvm.org/docs/tutorial/)

#### للمتقدمين
- [Intel Manual](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [OSDev Wiki](https://wiki.osdev.org/)
- [Bootlin OS Internals](https://bootlin.com/)

### English

#### For Beginners
- [OS Development Basics](https://wiki.osdev.org/Getting_Started)
- [Learn Assembly](https://www.nasm.us/doc/)
- [LLVM Introduction](https://llvm.org/docs/tutorial/)

#### For Advanced
- [Intel Manual]
- [OSDev Wiki]
- [Bootlin OS Internals]

---

## 🐛 الإبلاغ عن المشاكل / Reporting Issues

### العربية

إذا وجدت مشكلة أو لديك اقتراح:

1. تحقق من Issues الموجودة
2. إن لم تجد مشكلة مشابهة، أنشئ issue جديد
3. اذكر المرحلة والملف المتأثر
4. قدم تفاصيل كافية لإعادة إنتاج المشكلة

### English

If you find an issue or have a suggestion:

1. Check existing Issues
2. If not found, create new issue
3. Mention stage and affected file
4. Provide enough details to reproduce

---

## 📞 التواصل / Contact

- **GitHub Issues:** للمشاكل التقنية / For technical issues
- **Discussions:** للنقاشات العامة / For general discussions
- **Email:** dev@sadlang.org (مثال / example)

---

## 📄 الترخيص / License

هذه الخطة والكود الناتج مرخص تحت نفس ترخيص لغة ص.  
This plan and resulting code are licensed under Sad Language license.

---

**🌟 نتطلع لمساهماتك في بناء أول نظام تشغيل عربي بالكامل!**  
**🌟 We look forward to your contributions in building the first fully Arabic operating system!**
