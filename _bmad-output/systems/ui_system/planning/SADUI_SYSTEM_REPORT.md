---
title: "تقرير نظام الواجهات والرسومات التصريحي (SadUI)"
date: 2026-06-07
status: ACTIVE
author: Amelia (bmad-agent-dev)
methodology: "فحص مباشر للمحلل النحوي والنواة والباطن — لا تشغيل اختبارات"
sourceOfTruth: shared/parser/src/ui/parser_ui.cpp
evidenceBase:
  - shared/parser/src/ui/parser_ui.cpp
  - shared/parser/src/core/parser_main.cpp
  - shared/lexer/generated/keywords_generated.cpp
  - sad_ui/core/include/sad_ui/types.h
  - platform/android/src/native_ui_builder.cpp
  - tests/integration/test_event_system.ص
fullDoc: docs/نظام_الواجهات_الرسومي_ص.md
---

# تقرير نظام الواجهات والرسومات التصريحي (SadUI)

> **المصدر الموثوق:** المحلل النحوي `parser_ui.cpp` — وليس أمثلة `examples/` القديمة.
> التوثيق الكامل بالمخططات: [`docs/نظام_الواجهات_الرسومي_ص.md`](docs/نظام_الواجهات_الرسومي_ص.md)

## 1. الجوهر
نظام واجهات **تصريحي عربي** مدمج في لغة ص على نمط Flutter/SwiftUI: إعلان `واجهة_تصريحية`، حالة `@حالة`، سلسلة معدّلات `.حجم().لون()`، كتل تُغلَق بـ `نهاية`، RTL أصلي. متعدد المنصات عبر «الهجين الذكي».

## 2. ماذا يدعم (من الكود)
- **15 عنصراً أولياً** (`knownWidgets`): عمود، صف، رصة، شبكة، نص، صورة، أيقونة، زر، حقل_نص، مفتاح، منزلق، حاوية، عرض_تمرير، قائمة_كسولة، فاصل.
- **55+ عنصراً مُهملاً** (`deprecatedWidgets`) يُربط ببديل أولي مع تحذير (هجرة ADR-UI-02).
- **حالة:** `@حالة` / `@ربط` / `@بيئة` / `@محسوب` (`parseUIStateDecl`).
- **معدّلات:** بوسائط / بلا وسائط / معدّل-حدث (`parseModifierChain`).
- **أحداث:** 3 صيغ (`=>` / كتلة...نهاية / لامدا)؛ النواة تدعم 33+ نوع (`IREventType`).
- **رسم ديناميكي:** `إذا/وإلا` و`لكل/بينما` داخل الأبناء (`parseUIConditional`, `parseUILoop`, ADR-UI-01).

## 3. كيف يعمل
`ملف ص → Lexer → parser_ui.cpp → AST(UIDeclarationNode/UIWidgetExprNode) → UINode → IRBuilder+hybrid_routing → SadUI IR → باطن المنصة`.
الكلمتان السياقيتان `واجهة_تصريحية` (KEYWORD_UI_DECL) و`اعرض` (KEYWORD_SHOW) مسجّلتان في `keywords_generated.cpp`.
الباطن: المكتب SDL2+OpenGL (منفّذ)، الويب HTML/CSS/JS (منفّذ)، iOS SwiftUI+Metal (منفّذ — `swiftui_codegen.cpp`، `ios_renderer.mm`)، أندرويد Views أصلية عبر JNI (جزئي — `native_ui_builder.cpp` + `SadViewFactory.java`).

## 4. كيف نكتب (الصيغة المعتمدة)
```
استورد رسومات
واجهة_تصريحية تطبيق_عداد
    @حالة عداد: عدد = 0
    دالة بناء()
        ارجع عمود
            .حشوة(16)
            نص("" + عداد).حجم(48).سمك(.غامق)
            زر("زيادة").عند_النقر => عداد += 1
        نهاية
    نهاية
نهاية
```

## 5. الحالة (GR-01)
| المكوّن | الحالة |
|---------|--------|
| تحليل واجهة_تصريحية/معدّلات/أحداث/حالة | ✅ (`parser_ui.cpp`) |
| 15 أولي + خريطة الإهمال | ✅ |
| رسم شرطي/حلقات | ✅ |
| باطن المكتب/الويب/iOS | ✅ (المكتب 6669، الويب 4082، iOS 2752 سطر) |
| باطن أندرويد | 🟡 جزئي (903 سطر) |

## 6. تصحيح مهم
أمثلة `examples/تطبيق_عداد.ص` (`صنف...يرث حالة_عنصر` + `جديد هيكل(...)`) و`distribution/.../android/*.ص` (أقواس `{}`) **قديمة ولا يقبلها المحلل**. المثال العامل: `tests/integration/test_event_system.ص`. حُذف التوثيق القديم (`docs/تقرير_نظام_الواجهات_SadUI.md`, `docs/تصميم_نظام_الواجهات_v2.md`) واستُبدل بـ `planning/docs/نظام_الواجهات_الرسومي_ص.md`.

---
*فحص مباشر 2026-06-07 — Amelia.*
