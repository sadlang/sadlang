---
id: ADR-BUILTINS-001
title: "اختيار YAML كمصدر حقيقة لأسماء الدوال المدمجة"
status: Approved
date: 2026-06-06
deciders: [فريق لغة ص]
---

# ADR-BUILTINS-001: YAML كـ SoT للـ builtins

## السياق

`shared/builtins/include/builtin_registry.h` مكتوب يدوياً ويحتوي 107 اسم دالة. إضافة أي دالة جديدة أو تعديل اسمها يتطلب تعديل C++ يدوياً وتحديث كل المستهلكين.

## القرار

استخدام `language-truth/builtins/*.yaml` كـ SoT، يُولَّد منه `builtin_registry_generated.h` وقت البناء عبر `gen_builtins_registry.py`.

## التأثير

- إضافة دالة: YAML فقط → cmake يعيد البناء → كل الكود يعرف الدالة
- تغيير اسم: YAML فقط → لا تعديل C++ يدوي
- `builtin_registry.h` يصبح wrapper فقط

## البدائل المرفوضة

- **إبقاء الوضع الحالي:** يدوي، عرضة للأخطاء، لا يوجد SoT خارجي
- **قاعدة بيانات:** تعقيد زائد لا مبرر له

## النتائج

- **إيجابي:** مصدر حقيقة واحد للمفسر/المترجم/LSP
- **سلبي:** يتطلب كتابة `gen_builtins_registry.py` ببنية معقدة نسبياً
