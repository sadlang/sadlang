# 🧭 التركيب الموحد — اكتشاف 2026-05-28

> **المدير:** John (PM)
> **التاريخ:** 2026-05-28
> **النطاق:** خلاصة موحدة من 10 تقارير اكتشاف (W1-W10)

---

## 1. الحالة الكلية للمشروع

| النظام | الصحة | الملاحظة الرئيسية |
|---|---|---|
| **Shared Core (17 lib)** | 🟢 ممتاز | بنية نظيفة، توثيق ثنائي، 295+ ملف |
| **Compiler + Interpreter (497)** | 🟢 ممتاز | 140 SIR opcode، Phase 6 + A2، VM ضعيف فقط |
| **CI/CD + Workflows** | 🟢 ممتاز | 6 workflows متكاملة |
| **Governance/PMF** | 🟢 قوي | PMF v1.9.2، Bus Factor=1 مشكلة |
| **LSP + Tools** | 🟢 متقدم | 9+ ميزات (AI, Borrow Graph, Voice) |
| **Tests (29 + 50)** | 🟢 ممتاز | ~900+ assertions |
| **Documentation** | 🟡 جيد | 220+ ملف لكن تكرارات + توثيق قديم |
| **Stdlib + Graphics + Network** | 🟡 متوسط | الكود موجود لكن غير مدمج |
| **sadinfo_core** | 🟡 جزئي | Sprint 2: 60%، Sprint 3+: 0% |
| **Documentation Pipeline (ADR-006b)** | � مُسترد 2026-05-29 | 37 ملف من pycdc، 190 pytest pass |
| **sad_ui/** | 🔴 WIP | بنية بدون تنفيذ |
| **copilot-instructions.md** | 🔴 متضخم | 911 سطر يختلط فيه 5 مشاريع |

---

## 2. الاكتشافات الحرجة (P0) — تتطلب قراراً فورياً

### � الاكتشاف #1: ADR-006a Status Drift كارثي — **حُلّ في 2026-05-29**

**الحالة الأصلية (2026-05-28):** ادعاءات Done على ≥4 stories بدون كود فعلي.

**الإجراء المُتَّخذ (2026-05-29):**
- تم استرداد 37 ملف Python من `__pycache__` عبر pycdc decompilation (ADR-006a-recovery)
- إصلاح 5 فئات من الانحرافات بعد الاسترداد:
  1. API drift من pycdc
  2. branch resource missing (skipif مع توثيق صريح للسبب)
  3. property vs method confusion
  4. mojibake في bytes-level (✅/⚠️/❌)
  5. semantic drift في assertions

**النتيجة الفعلية:**
- ✅ pytest: **190 pass / 247 skip / 0 fail** (تحقق عملي تام)
- ✅ جميع ملفات `scripts/codegen/` تجتاز `py_compile`
- ✅ Backup التحقّق: نسخةٌ احتياطيّةٌ خارجَ المستودعِ (مسارُها محلّيٌّ لا يُودَع) تحتوي **فقط** على `{error_messages.yaml, keywords.schema.json, keywords.yaml}` — `builtins/` غير موجود فعلاً، فـ skipif مبرّر هندسياً.

**التوثيق:**
- `RECOVERY_FINAL_REPORT.md` (9 أقسام كاملة)
- `/memories/repo/adr_006a_recovery_2026-05-28.md` (محدّث: 37/37 ✅)

**القرارات المتبقية من هذا P0:**
- (ج) ✅ تم تطبيق Status Verification: لن يُعلن done إلا بدليل عملي (pytest، compile، تشغيل فعلي).
- (أ) ✅ لا حاجة لإعلان BLOCKED — الكود مُسترد ويعمل.
- (ب) ✅ مصدر الاسترداد كان `__pycache__` (.pyc) وليس git history — pycdc decompiler نجح.

**التحدّيات الباقية (خارج نطاق هذا P0):**
- `docs/` و باقي `scripts/` (خارج `codegen/`) لم تخضع للاسترداد بعد — قرار المستخدم تأجيلها (موثَّق في RECOVERY_FINAL_REPORT.md §7).

### 🔴 الاكتشاف #2: تكرار بنيوي أنشأته جلسة سابقة

| الملف الذي أُنشئ | الأصل المتعارض معه |
|---|---|
| `docs/governance/GUARDED_FILES.md` | `_bmad-output/governance/1-policy/EDGE_CASE_GUARDS.md` |
| `docs/governance/README.md` | `_bmad-output/governance/1-policy/README.md` |
| `_bmad-output/governance/2-agents/` (6 ملفات) | `_bmad-output/governance/1-policy/PM_REPORT_AND_AGENT_PROTOCOL.md` + BACKLOG |

**القرار المطلوب:**
- (أ) حذف `docs/governance/` (Mirror غير مطلوب)
- (ب) حذف `_bmad-output/governance/2-agents/` أو دمج محتواه في execution/

### 🔴 الاكتشاف #3: Bus Factor = 1

- صالح وحده يملك 95% من قرارات المشروع
- لا نائب (Deputy Owner)
- **B-002 يعالج هذا في Sprint الحالي** — يجب التنفيذ الفوري

### 🔴 الاكتشاف #4: copilot-instructions.md متضخم

- 4,500+ سطر يختلط فيه: لغة ص + C++ + إصلاح + BMAD + APIs
- صعوبة عثور على قاعدة معينة
- **التقسيم الفوري إلى 4 ملفات منفصلة**

---

## 3. الاكتشافات المهمة (P1)

| # | الاكتشاف | المنطقة |
|---|---|---|
| 1 | تضارب Contract-as-Code: PMF=63% / PRD=5% / codeRolePlan=63% | W2 |
| 2 | `_bmad-output/eroor_system/` فارغ (خطأ إملائي) — يجب حذفه | W2 |
| 3 | `docplan/` و `doc_plan/` متوازيان بنفس الموضوع | W2 |
| 4 | 12 ملف مفقود في governance (daily/, tasks/, DEPUTY_OWNER) | W1 |
| 5 | sadinfo_core Sprint 3+ صفر (لكن Sprint 0-2 تعمل) | W7 |
| 6 | hot_reload + profiler غير مستخدمين | W7 |
| 7 | graphics/ مجرد SDL2 wrapper بدون كود رابط بـ Sad | W9 |
| 8 | sadnet/ نظام P2P احترافي معزول عن stdlib | W9 |
| 9 | sad_ui/ بنية موجودة بدون أي تنفيذ | W9 |
| 10 | تكرار CIS skills (6 مهارات تغطي نفس الفكرة) | W6 |

---

## 4. الاكتشافات الأقل أولوية (P2-P3)

- 50+ ملف SDL2 docs مختلط مع توثيق المشروع (W4)
- 100% عربي في `وثائق/` بدون نسخ إنجليزية (W4)
- 55 ملف تدريبي بدون README مرشد (W4)
- توثيق قديم "يوليو 2025" في بنية_المشروع/ (W4)
- archived/ آمن للحذف 100% (105 MB) (W5)
- _scratch/ قابل للتنظيف بـ 350 MB (W5)
- _proj_pdf/ لغز بـ 2 ملف — يمكن حذفه (W5)
- ملفات log في الجذر تحتاج تنظيف (W10)
- _recovery_*.py عملية قديمة في الجذر (W10)
- dependabot.yml قد يكون قديم (W6)
- platform/ فقط Android (الحقيقي في runtime/) (W9)
- type_checker.cpp ضخم 500+ سطر (W7)

---

## 5. المساحة القابلة للاسترداد

| المصدر | المساحة |
|---|---|
| archived/ | 105 MB |
| _scratch/ | 350 MB |
| _proj_pdf/ + logs الجذر | ~5 MB |
| stdlib duplicates في distribution/ | ~50 MB |
| **الإجمالي** | **~500 MB** 💾 |

---

## 6. ملخص الإحصائيات

| المنطقة | الملفات | المساحة | الحالة |
|---|---|---|---|
| W1 governance | 34 | ~104 KB | 🟢 |
| W2 planning | 95 | ~1.61 MB | 🟡 (Status Drift) |
| W3 _bmad | 200+ | ~5-8 MB | 🟢 |
| W4 docs | 330 | ~50 MB | 🟡 (تكرار + قدم) |
| W5 archived | 700+ | ~470 MB | 🔴 (للحذف) |
| W6 .github | 110+ | ~3.5 MB | 🟢 (مع تحفظات) |
| W7 shared+runtime | 295+ | ~25K LOC | 🟢 |
| W8 compiler+interp+vm | 497 | ~500K LOC | 🟢 |
| W9 libs | 1,650 | ~70 MB | 🟡 |
| W10 tools+tests+misc | 200+ | متغير | 🟢 |

**إجمالي تقريبي:** ~4,000+ ملف، ~750 MB، ~525,000+ سطر كود.

---

## 7. اقتراح مصفوفة الأولويات

### الأسبوع الحالي (P0)
1. ✅ **حُلّ في 2026-05-29 — ADR-006a Status Drift** (190 pytest ✅، 37 ملف مسترد، 0 fail) — راجع الاكتشاف #1 أعلاه
2. **حذف docs/governance/ و _bmad-output/governance/2-agents/** المتكررة
3. **تعيين Deputy Owner** (B-002)
4. **تقسيم copilot-instructions.md** إلى 4 ملفات

### الأسبوعان التاليان (P1)
5. توحيد أرقام Contract-as-Code (5%/63%)
6. حذف `eroor_system/` الفارغ
7. توحيد docplan/ + doc_plan/
8. ملء 12 ملف مفقود في governance
9. استكمال sadinfo_core Sprint 2-3
10. توضيح حالة sad_ui/ (continue or kill)

### شهري (P2)
11. تنظيف archived/ + _scratch/
12. مراجعة dependabot.yml
13. توحيد CIS skills
14. فحص graphics/ لإيجاد/إنشاء الكود الرابط
15. دمج sadnet/ في stdlib أو توثيق العزل

---

## 8. خلاصة PM

**الحقيقة الجوهرية:** المشروع تقنياً قوي جداً (Compiler/Interpreter/Shared) لكن طبقة Governance/Planning تعاني من:
1. **Status Drift كارثي** — Stories مدّعاة done بدون كود
2. **تكرار بنيوي** ولّدته جلسات سابقة
3. **Bus Factor=1** يهدد استمرارية المشروع
4. **توثيق متضخم** يصعب الصيانة

**التوصية:** قبل بدء أي ميزة جديدة، عالج P0 الأربعة. هذا سيُحرّر طاقة كبيرة ويزيل ادعاءات وهمية تعيق التخطيط الواقعي.

---

**أُنشئ:** 2026-05-28 | **المؤلف:** John (PM) | **النوع:** PM Synthesis Report

**آخر تحديث:** 2026-05-29 — تحديث P0 #1 (ADR-006a Status Drift) إلى **حُلّ** ✅
- 37/37 ملف مسترد
- pytest: 190 pass / 247 skip / 0 fail
- توثيق كامل في `RECOVERY_FINAL_REPORT.md`
- باقي الـ P0 (#2, #3, #4) لا تزال معلَّقة
