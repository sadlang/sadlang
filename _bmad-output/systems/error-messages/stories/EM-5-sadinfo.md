# Story EM-5 — sadinfo --errors (اختياري — Phase 2)

**Epic**: EPIC-EM | **Story Points**: 3 | **الأولوية**: منخفضة | **الحالة**: مؤجَّلة
**التبعيات**: EM-4 | **المُسنَد إلى**: TBD

---

## الهدف
تصدير كتالوج الأخطاء عبر `sadinfo --errors` للاستخدام في LSP/IDE/docs خارجية.

## التنفيذ

- [ ] **T1** — إضافة sub-command `--errors` إلى أداة `sadinfo`
  - [ ] `sadinfo --errors --format=json` → JSON كامل لكل الأخطاء
  - [ ] `sadinfo --errors --format=markdown` → جدول مرتب
  - [ ] `sadinfo --errors --code=SEM002` → تفاصيل خطأ واحد
  - [ ] `sadinfo --errors --category=semantic` → فلتر حسب فئة

- [ ] **T2** — توثيق في README

- [ ] **T3** — اختبار CLI

## معايير القبول (AC)

- **AC-5.1**: ✅ `sadinfo --errors --format=json` يُصدِّر JSON صحيح لكل الـ203.
- **AC-5.2**: ✅ `sadinfo --errors --code=SEM002` يطبع تفاصيل كاملة.
- **AC-5.3**: ✅ `sadinfo --errors --category=semantic` يفلتر بنجاح.

## ملاحظات
هذه القصة اختيارية — يمكن تأجيلها إلى Phase 2 بدون تأثير على الEpic الأساسي.

## File List
- (سيُملأ)

## Dev Agent Record
- (سيُملأ)
