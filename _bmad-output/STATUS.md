---
title: لوحة حالة جميع الخطط (Unified Status Dashboard)
date: 2025-11-21
lastUpdated: 2026-05-30
relatedDocuments:
  - _bmad-output/README.md
  - _bmad-output/governance/README.md
  - _bmad-output/governance/ARCHITECTURE.md
  - _bmad-output/governance/3-code-contract/implementation_status.md
  - _bmad-output/SYSTEMS_ARCHITECTURE_PROPOSAL.md
  - _bmad-output/systems/doc-ir/ADR-006b-epics.md
  - _bmad-output/systems/doc-ir/REVIEW-2026-05-19-docplan-status.md
  - _bmad-output/systems/error-messages/prd-error-messages.md
  - _bmad-output/planning-artifacts/epics.md
  - _bmad-output/discovery/2026-05-28/00_SYNTHESIS.md
  - RECOVERY_FINAL_REPORT.md
author: Amelia (مراجعة هندسية موحَّدة) | John (PM, تحديث 2026-05-29) | Amelia (توحيد governance, 2026-05-30)
purpose: |
  مصدر حقيقة واحد لحالة جميع خطط _bmad-output.
  يتجنّب status drift ويمنع التكرار بين تقارير الخطط المنفصلة.
---

# لوحة حالة الخطط الموحَّدة

> هذه اللوحة **مفهرس** لتقارير الحالة الموزَّعة — لا تُكرِّر التفاصيل،
> بل تُشير إلى مصدر الحقيقة لكل خطة.
> عند أي تحديث في خطة فرعية، حدِّث الجدول الموجَز هنا أيضاً.

> **🔔 تحديث معماري — 2026-05-30:**
> - **الحوكمة موحَّدة** الآن تحت [`governance/`](governance/README.md) بثلاث طبقات (Policy / Agents / Code Contract).
>   المجلدات القديمة `agents/` و `codeRolePlan/` و `management/` أُحيلت لسلة المهملات
>   (الأرشيف التاريخي في [`archive/2026-05-30-governance-unification/`](archive/2026-05-30-governance-unification/)).
> - **أنظمة لغة ص** — تم توحيدها تحت [`systems/`](systems/README.md) في 2026-05-30:
>   - ✅ `systems/error-messages/` (منقول من `error_system/`)
>   - ✅ `systems/type-system/` (منقول من `type_system/`)
>   - ✅ `systems/doc-ir/` (منقول من `docplan/`)
>   - ✅ `systems/doc-plan-v2/` (منقول من `doc_plan/`)
>   - ✅ `systems/sadinfo/` (منقول من `planning-artifacts/sadinfo/`)
>   - 🟡 `systems/error-recovery/` — قيد التصميم (placeholder)
>   - الخطة الكاملة: [`SYSTEMS_ARCHITECTURE_PROPOSAL.md`](SYSTEMS_ARCHITECTURE_PROPOSAL.md)
>   - سجل الترحيل: [`archive/2026-05-30-systems-unification/`](archive/2026-05-30-systems-unification/)
> - `eroor_system/` (خطأ إملائي مكرّر) **حُذف** في 2026-05-30 (SHA-256 متطابق مع الأصل).

---

## 1. الخطط النشطة

### 1.1. codeRolePlan — Contract-as-Code (SAD_INVARIANT + ENFORCER)

| البند | القيمة |
|---|---|
| الحالة الكلية | **63%** (10/16 من الخطة الأصلية) + توسعات P5–P11 |
| الثوابت | **156** (INT001–INT156) |
| الاختبارات السلبية | **34/34** يمر |
| الفجوات الحرجة | Epic 3 (4 قصص) + Epic 4 (2 قصص) |
| مصدر التفاصيل | [implementation_status.md](_bmad-output/governance/3-code-contract/implementation_status.md) |
| آخر تحديث جوهري | 2025-11-21 (دفعة P11) |

### 1.2. systems/doc-ir — DocIR + Renderers (ADR-006b)

> 📦 **انتقل (2026-05-30):** من `_bmad-output/docplan/` إلى
> [`_bmad-output/systems/doc-ir/`](systems/doc-ir/).

> **⚠️ تحديث 2026-05-29:** كان PM Synthesis (2026-05-28) قد كشف Status Drift كارثياً —
> ملفات `scripts/codegen/` كانت **مفقودة** رغم ادعاء done. تم استرداد 37 ملف عبر pycdc
> decompilation في جلسة 2026-05-29، وأصبحت **190 pytest تمر فعلياً (0 fail)**.
> راجع `RECOVERY_FINAL_REPORT.md` و `_bmad-output/discovery/2026-05-28/00_SYNTHESIS.md` (P0 #1 ✅).

| البند | القيمة |
|---|---|
| الحالة الكلية | **88%** (15/17 منجَز، 2/17 جزئي، 0/17 غير منفَّذ) |
| القنوات المُطبَّقة | vitepress, lsp, repl, **man** (Story 3.3 ✅ 2025-11-21), tutorials |
| الـ DocIR | builder + validator (10 قواعد) + doc_type (Diátaxis) |
| اختبارات pytest | **190 pass / 247 skip / 0 fail** (محقَّق فعلياً 2026-05-29، بعد الاسترداد) |
| الفجوات المتبقية | Story 5.2 (توسيع tutorials/AI) — Story 4.3 أُغلقت بإلزام CI |
| مصدر التفاصيل | [ADR-006b-epics.md](_bmad-output/systems/doc-ir/ADR-006b-epics.md) |
| مراجعة هندسية | [REVIEW-2026-05-19-docplan-status.md](_bmad-output/systems/doc-ir/REVIEW-2026-05-19-docplan-status.md) |
| تقرير الاسترداد | [RECOVERY_FINAL_REPORT.md](RECOVERY_FINAL_REPORT.md) (2026-05-29) |
| آخر تحديث جوهري | 2026-05-29 (استرداد 37 ملف + إصلاح 5 فئات انحرافات) |

### 1.3. systems/error-messages — نظام رسائل الأخطاء

| البند | القيمة |
|---|---|
| الحالة | مرحلة PRD (لم يبدأ التنفيذ) |
| الموقع | `_bmad-output/systems/error-messages/` (منقول من `error_system/` في 2026-05-30) |
| مصدر التفاصيل | [prd-error-messages.md](_bmad-output/systems/error-messages/prd-error-messages.md) |
| التناغم مع docplan | موثّق في [_alignment_with_docplan.md](_bmad-output/systems/error-messages/_alignment_with_docplan.md) |

### 1.4. planning-artifacts — خطط قديمة/مرحلية

| الملف | الحالة |
|---|---|
| [epics.md](_bmad-output/planning-artifacts/epics.md) | تاريخي — مُدمج في codeRolePlan |
| [epics-phase2.md](_bmad-output/planning-artifacts/epics-phase2.md) | تاريخي |
| [epics-vscode-extension.md](_bmad-output/planning-artifacts/epics-vscode-extension.md) | امتداد VS Code (مرحلة منفصلة) |
| [compiler_restructure_plan.md](_bmad-output/planning-artifacts/compiler_restructure_plan.md) | منجَز (compiler_new موجود) |

### 1.5. sadinfo v2 — نظام معلومات اللغة (Loader + Validator)

| البند | القيمة |
|---|---|
| الحالة الكلية | Sprint 0 ✅ + Sprint 1 (3/3) ✅ + Sprint 2 (1/4): S-004 ✅ |
| الاختبارات | `ctest -L sadinfo`: **7/7 PASSED** (Logger + Schemas + Loader + EntityView + EntityViewBench + Tier1Validator + HashStrategy) |
| المُنجَز | S-000a ✅، S-001 (Loader+yaml-cpp, CR-1/2/3) ✅، S-002 (EntityView+FieldNameTable) ✅، S-003 (Tier1 Validator + BF-04) ✅، S-004 (Hash Store: fast_hash FNV-1a + full_hash SHA-256 inline RFC 6234 + mmap RAII + content_equals fast→full) ✅ بدون تبعيات خارجية |
| التالي | S-007 (State + Lock) — يجب قبل S-005/S-006 في Sprint 2 |
| مصدر التفاصيل | [sprint-plan.md](_bmad-output/systems/sadinfo/sprint-plan.md) |
| القصص | [stories/](_bmad-output/systems/sadinfo/stories/) |
| آخر تحديث جوهري | 2025-12-15 (S-002 closed) |

---

## 2. مؤشرات صحة عامة

| المؤشر | القيمة | الهدف |
|---|---|---|
| ثوابت SAD_INVARIANT | 156 | يتوسَّع تدريجياً |
| death tests | 34/34 ✅ | لا تراجع |
| pytest DocIR | 40/40 ✅ | لا تراجع |
| صفحات VitePress مُولَّدة | يتولَّد من YAML | فحص آلي عبر `--enforce` (Story 5.1) |
| صفحات man مُولَّدة | **34** (`docs/generated/man/*.1`) | جديد 2025-11-21 |
| تغطية description_ar | 100% (BASIC ≥10 حرف) | رفع تدريجي |
| **جودة محتوى description_ar** | **STANDARD ~54% + BASIC ~46% + 0% PLACEHOLDER + 0% NONE** (Story 4.3 — 2025-11-21) | رفع BASIC ← STANDARD/RICH |

---

## 3. مبادئ تجنّب status drift

1. **مصدر الحقيقة الوحيد** لكل خطة هو ملف `epics.md` أو `implementation_status.md` الخاص بها — هذه اللوحة فهرس فقط.
2. **عند إنجاز story** يُحدَّث:
   - ملف epics المعني (إضافة `✅ DONE YYYY-MM-DD`)
   - الجدول الموجَز في هذه اللوحة (إن تغيَّرت النسبة الكلية)
3. **لا تكرار**: تفاصيل التنفيذ تبقى في تقرير الخطة الفرعية فقط.
4. **مراجعة دورية**: عند تجاوز 10 إنجازات بدون مراجعة، أنشئ ملف `REVIEW-YYYY-MM-DD-<plan>-status.md` جديد.

---

## 4. سجل تحديثات اللوحة

| التاريخ | التغيير |
|---|---|
| 2026-05-30 | **توحيد أنظمة لغة ص (Phases 1-4):** إنشاء `systems/{_TEMPLATE,error-messages,type-system,doc-ir,doc-plan-v2,sadinfo}/` (95 ملف منقول). حذف `eroor_system/` (مكرر SHA-256). تحديث الروابط النشطة في `README.md` + `STATUS.md` + `data/_schemas/README.md` + `docs/sadinfo_v2/{README,BASELINE}.md`. **0 رابط مكسور**. `doc_plan/` + `docplan/` نُقلا منفصلَين (الدمج مؤجَّل لجلسة لاحقة). |
| 2026-05-30 | **توحيد الحوكمة:** نقل `agents/` + `codeRolePlan/` + `management/` تحت `governance/` بثلاث طبقات. تحديث 30 مرجعاً عبر المشروع. أرشفة وثائق المهاجرة. إضافة مقترح بنية `systems/` لأنظمة لغة ص. |
| 2025-11-21 | إنشاء اللوحة الموحَّدة + توثيق إنجاز Story 3.3 (render_man) + Story 4.3 (نظام طبقات الجودة) |
| 2025-12-15 | إضافة قسم 1.5 sadinfo v2: إغلاق S-000a + S-001 (مع CR-1/2/3). ctest 3/3 أخضر. |
| 2025-12-15 | إغلاق S-002 (EntityView + FieldNameTable): 6 unit + 1 stress + bench. ctest sadinfo_v2 5/5 أخضر. تنقيح AC-6 من <1KB إلى <2KB موثَّق في Change Log. |
| 2025-12-15 | S-002 CR Round-1 (intern_existing لمنع تلويث الجدول + erase + اختبارَيْن) و Round-2 (erase O(N)→O(1) عبر contiguous-prefix invariant + 3 negative tests). 5/5 أخضر. |
| 2025-12-15 | إغلاق S-003 (Tier1 Validator Inline): 16 unit + 1 integration (validated_data: 10 fixtures × 4 categories). ctest sadinfo **6/6** أخضر. جولتا Amelia + إصلاح BF-04 حرج + إعادة تسمية معمارية (`validator_tier1.cpp → tier1_cheap.cpp`) + اكتشاف أن DuplicateId غير قابل للتفعيل من scan قرصي (entity.id() مشتق من اسم المجلد). |
| 2025-12-15 | إغلاق S-004 (Hash Store) — Sprint 2 يبدأ بالتوازي: `fast_hash` = size<<32 \| FNV-1a(4KB) بدون heap (CW-25)، `full_hash` = SHA-256 RFC 6234 inline (4 متجهات اختبار) مع mmap للملفات ≥64KB، `content_equals` بمسار short-circuit. **بدون تبعية على OpenSSL**. ctest sadinfo **7/7** أخضر. |
| 2025-12-15 | S-004 **CR Round-1 (Amelia)** — 10 نتائج (4 مُهمَلة + 6 مُصلَحة): **F1** ferror بعد fread (منع تجزئة كاذبة عند قراءة جزئية)، **F2** UTF-8→UTF-16 على Windows (`CreateFileW` + `_wfopen` لدعم المسارات العربية)، **F5** short-circuit `path_a==path_b` (يتجنب قراءتين)، **F6** عقد `MmapFile::is_valid()` صريح عبر `valid_` flag (تنظيف كامل عند فشل `MapViewOfFile`/`mmap`)، **F9** `vector<unsigned char>` بدل `string`+`reinterpret_cast`، **F10** fallback من mmap إلى streaming عند فشل التخطيط. تم إضافة 5 اختبارات حدود (64KB-1/64KB/64KB+1/128KB + same-path shortcut). **28/28** assertions داخلية + **7/7** ctest sadinfo أخضر. |
| 2025-12-15 | S-004 **CR Round-2 (Amelia)** — 10 نتائج (6 مُهمَلة + 3 مُصلَحة + 1 موثَّقة): **G1** 🔴 `sha256_update` من حلقة byte-by-byte إلى `memcpy` + معالجة كتل كاملة مباشرة من المدخل (تسريع ~10× على ملفات MB)، **G4+G8** 🟠 فحص `is_regular_file` مبكراً في `fast_hash` + `full_hash` (منع digest الفراغ الكاذب `e3b0c442` للمجلدات — كان `file_size=0` + `exists()=true` يُنتجان نتيجة مضللة)، **G2/G3** علّقت سلوك TOCTOU + `mf.size()==0`. أُضيف `test_directory_handling` (5 تأكيدات). **33/33** assertions داخلية + **7/7** ctest sadinfo أخضر. |
| 2025-12-15 | S-004 **CR Round-3 (Amelia)** — 8 نتائج (5 مُهمَلة/مُؤجَّلة + 2 مُصلَحة + 1 موثَّقة): **H2** 🔴 `stream_hash` كان يخصص `vector<unsigned char>(65536)` heap لكل ملف — خرق ذاتي لـ CW-25؛ نُقل إلى stack buffer `unsigned char[64*1024]` (آمن ضمن حد 1MB stack افتراضي)، **H1** 🟠 فحص دفاعي لـ `n==0` بدون `ferror`/`feof` يمنع حلقة لانهائية نظرية، **H8** عقد `MmapFile::is_valid()` كان موثقاً مسبقاً — تأكيد. H3/H4/H5/H6/H7 مُهمَلة (سلوكية صحيحة أو تأجيل لـ S-005). **33/33** + **7/7** ctest أخضر. |
| 2025-12-15 | S-004 **CR Round-4 (Amelia)** — 10 نتائج (6 مُهمَلة + 4 مُصلَحة): **I9** 🔴 `stream_hash` نُقل من stack مباشر إلى `thread_local unsigned char[64*1024]` — يحفظ thread-safety + يلغي خطر stack overflow على خيوط بـ stack ≤ 256KB (fiber/threadpool)، **I1** 🟠 فحص دفاعي `li.QuadPart < 0` على Windows و `st.st_size < 0` على POSIX (BF-06)، **I2** 🟠 `static_assert(sizeof(size_t) >= sizeof(int64_t))` يحرس متطلب 64-bit ساكناً، **I4** 🟠 `assert(buffer_len < 64)` في `sha256_final` يحمي عقد `sha256_update` الداخلي. I3/I5/I6/I7/I8/I10 مُهمَلة (سلوكية صحيحة أو خارج النطاق). **33/33** + **7/7** ctest أخضر. |
| 2025-12-15 | S-004 **اختبارات تزامن (Amelia)** — `test_concurrent_hashing` أُضيف: 8 خيوط × 100 تكرار × 3 دوال (`full_hash` + `fast_hash` + `content_equals`) على ملف 256KB. **2400 عملية متزامنة، 0 race condition، 0 mismatch**. يُثبت فعلياً سلامة `thread_local` (I9) ويُغطي عقد thread-safety الذي يضمنه `Sha256Ctx` المحلي لكل استدعاء. **38/38** assertions + **7/7** ctest أخضر. |
| 2025-12-15 | بدء S-007 (State + Lock) **Part A — CacheState + atomic write**: `cache_state.h` (struct CacheMetadata/Versions/EntityState/DependencyEdge/CacheState + StateIoResult enum، header-private بدون nlohmann::json)، `atomic_write.h/.cpp` (cross-platform: Windows = CreateFileW + WRITE_THROUGH + MoveFileExW؛ POSIX = open(O_EXCL) + fsync + rename(2)، tmp فريد بـ pid+counter)، `cache_state.cpp` (nlohmann/json **PRIVATE** عبر FetchContent، to_json/from_json/read_state/write_state، schema_version check، safe getters بدون exceptions). 10 اختبارات (38 assertion) تغطي: roundtrip، disk write/read، missing file = FileNotFound، JSON معطوب = ParseError، schema غير مدعوم = SchemaMismatch، empty path = InvalidPath، atomic overwrite. **38/38 cache_state + 38/38 hash + 8/8 ctest** أخضر بلا تراجع. |
| 2025-12-15 | S-007 Part A **CR Round-1 (Amelia)** — 10 ملاحظات (7 مُهمَلة + 3 مُصلَحة): **J1** 🔴 `atomic_write.cpp` — حراس RAII داخليون لـ `HANDLE`/`fd` و tmp-file (CW-16 صريح؛ يضمن التنظيف حتى لو رُمي استثناء غير متوقع). **J3/J6** 🔴 `cache_state.cpp from_json` — إعادة تعيين `out = CacheState{}` صراحةً قبل `return SchemaMismatch` (منع تسرّب metadata الجزئية). **J8** 🔴 `from_json` — سياسة موحَّدة: مفتاح الـmap يكسب دائماً؛ `es.id = eit.key()` (إزالة تناقض داخلي إن حُرِّر الـjson يدوياً). **J2** 🟠 إضافة `fsync(dirfd)` POSIX بعد rename لدوام directory entry (best-effort). J4/J5/J7/J9/J10 مُهمَلة (تحسينات سرعة غير محرجة أو حالات نادرة). اختباران جديدان: `test_schema_mismatch` موسَّع (3 asserts لتأكيد J3) + `test_key_wins_over_id_field` (3 asserts لـJ8). **44/44 cache_state** + **38/38 hash** + 8/8 ctest أخضر. |
| 2025-12-15 | S-007 Part A **CR Round-2 (Amelia)** — 10 ملاحظات (6 مُهمَلة + 4 مُصلَحة): **K1** 🔴 `read_state` — فحص `fs::is_regular_file` صريح قبل فتح `ifstream` (يميّز directory/symlink/device عن "ملف غير موجود"؛ ينتج `InvalidPath` بدل `ParseError` المضلِّل). **K7** 🔴 `read_state` — حد حجم 256 MB عبر `fs::file_size` كحاجز DoS محلي (constexpr `kMaxStateFileBytes`؛ ملف أكبر = `ReadError` بدون قراءة كاملة). **K2** 🔴 `get_u64` — قبول signed integer غير سالب أيضاً (interop مع أدوات خارجية تكتب بدون علامة u)؛ رفض السالب صراحةً يمنع silent cast (CW-14). **K10** 🟠 `cache_state_tests.cpp main()` — `fs::remove_all` نهائي لمجلد `sadinfo_s007_tests` لمنع تراكم ملفات tmp بين runs. K3/K4/K5/K6/K8/K9 مُهمَلة (تحسينات API/تسمية/perf خارج نطاق Round-2). 3 اختبارات جديدة: `test_read_state_on_directory` + `test_signed_integer_field_accepted` + `test_negative_integer_field_rejected` (5 asserts إضافية لتأكيد K1/K2). **49/49 cache_state** + **38/38 hash** أخضر بلا تراجع. |
| 2025-12-15 | S-007 **Parts C+D — CacheLock (cross-platform)** (Amelia): [cache_lock.h](shared/sadinfo_core/include/cache_lock.h) — CacheLock RAII (non-copyable/movable) + LockInfo + LockResult{Ok,AlreadyHeld,StaleAcquired,InvalidPath,IoError,ParseError,Mismatch,NotHeld} + LockOwnerStatus{Alive,Dead,Reused,Unknown}. [cache_lock.cpp](shared/sadinfo_core/src/cache/cache_lock.cpp) — driver محايد للمنصة، 
lohmann/json PRIVATE فقط (CW-21/AC-8)، 
ead_lock_file بحراس s::is_regular_file + سقف 4 KB DoS، **	ry_acquire** يكتشف stale-lock تلقائياً (check_owner → Dead/Reused → force_break آمن TOCTOU → retry CREATE_EXCL → StaleAcquired)، **
elease** يقرأ-يقارن-يحذف فقط إذا (pid,start_time_ns) يطابقان self (يمنع حذف قفل غير-ملكنا)، **orce_break** TOCTOU-safe (re-read + compare expected). [cache_lock_win.cpp](shared/sadinfo_core/src/cache/cache_lock_win.cpp) — CreateFileW + CREATE_NEW + WRITE_THROUGH + FlushFileBuffers، OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION) + GetProcessTimes لكشف PID-reuse بـ ±1ms tolerance، UTF-16 paths لدعم العربية، ACCESS_DENIED → Alive (محافظ). [cache_lock_posix.cpp](shared/sadinfo_core/src/cache/cache_lock_posix.cpp) — open(O_CREAT|O_EXCL|O_CLOEXEC,0644) + sync(fd) + sync(dirfd)، kill(pid,0) للوجود (ESRCH→Dead, EPERM→Alive)، Linux: قراءة /proc/<pid>/stat field 22 + sysinfo().uptime + sysconf(_SC_CLK_TCK) لحساب start_time_ns، ±50ms tolerance، macOS fallback يُرجع 0 (سياسة محافظة Alive). [cache_lock_tests.cpp](tests/sadinfo/unit/cache_lock_tests.cpp) — 14 اختباراً يغطون T9 (acquire/AlreadyHeld بحامل-حي) + T10 (4 سيناريوهات: stale auto-recovery، parse-error، force_break match، force_break mismatch) + RAII/move/empty-path/double-release. **35/35 lock + 49/49 cache_state + 38/38 hash = 122/122** أخضر بلا تراجع. |
| 2025-12-15 | S-007 **Parts C+D — CacheLock (cross-platform)** (Amelia): [cache_lock.h](shared/sadinfo_core/include/cache_lock.h) — CacheLock RAII (non-copyable/movable) + LockInfo + LockResult{Ok,AlreadyHeld,StaleAcquired,InvalidPath,IoError,ParseError,Mismatch,NotHeld} + LockOwnerStatus{Alive,Dead,Reused,Unknown}. [cache_lock.cpp](shared/sadinfo_core/src/cache/cache_lock.cpp) — driver محايد للمنصة، 
lohmann/json PRIVATE فقط (CW-21/AC-8)، 
ead_lock_file بحراس s::is_regular_file + سقف 4 KB DoS، **	ry_acquire** يكتشف stale-lock تلقائياً (check_owner → Dead/Reused → force_break آمن TOCTOU → retry CREATE_EXCL → StaleAcquired)، **
elease** يقرأ-يقارن-يحذف فقط إذا (pid,start_time_ns) يطابقان self (يمنع حذف قفل غير-ملكنا)، **orce_break** TOCTOU-safe (re-read + compare expected). [cache_lock_win.cpp](shared/sadinfo_core/src/cache/cache_lock_win.cpp) — CreateFileW + CREATE_NEW + WRITE_THROUGH + FlushFileBuffers، OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION) + GetProcessTimes لكشف PID-reuse بـ ±1ms tolerance، UTF-16 paths لدعم العربية، ACCESS_DENIED → Alive (محافظ). [cache_lock_posix.cpp](shared/sadinfo_core/src/cache/cache_lock_posix.cpp) — open(O_CREAT|O_EXCL|O_CLOEXEC,0644) + sync(fd) + sync(dirfd)، kill(pid,0) للوجود (ESRCH→Dead, EPERM→Alive)، Linux: قراءة /proc/<pid>/stat field 22 + sysinfo().uptime + sysconf(_SC_CLK_TCK) لحساب start_time_ns، ±50ms tolerance، macOS fallback يُرجع 0 (سياسة محافظة Alive). [cache_lock_tests.cpp](tests/sadinfo/unit/cache_lock_tests.cpp) — 14 اختباراً يغطون T9 (acquire/AlreadyHeld بحامل-حي) + T10 (4 سيناريوهات: stale auto-recovery، parse-error، force_break match، force_break mismatch) + RAII/move/empty-path/double-release. **35/35 lock + 49/49 cache_state + 38/38 hash = 122/122** أخضر بلا تراجع. |
| 2025-12-15 | S-007 Parts C+D **CR Round-1 (Amelia)** — 12 ملاحظات (7 مُهمَلة + 5 مُصلَحة): **L1** 🔴 [cache_lock.cpp](shared/sadinfo_core/src/cache/cache_lock.cpp) try_acquire — مسح self_=LockInfo{} قبل platform_fill_self_info لمنع تسرّب metadata قديم في مسارات الفشل. **L2** 🔴 release() — سياسة موحَّدة 4 حالات: read Ok+match→unlink+Ok؛ Ok+mismatch→Mismatch (لا حذف)؛ IoError (ملف غير موجود)→Ok idempotent؛ ParseError/InvalidPath→أعد كما هو (لا حذف لقفل غريب). السلوك السابق كان قد يحذف قفل شخص آخر. **L3** 🔴 [cache_lock_win.cpp](shared/sadinfo_core/src/cache/cache_lock_win.cpp) platform_check_owner — ERROR_ACCESS_DENIED سابقاً Alive (يمنع كسر قفل لـsystem service مُعاد PID-ه أبداً)؛ الآن Unknown → AlreadyHeld محافظ بدون تأكيد حياة كاذب. **L4** 🟠 serialize_lock — j.dump() بلا indent (lock file داخلي). **L6** 🟡 [cache_lock_tests.cpp](tests/sadinfo/unit/cache_lock_tests.cpp) T10 — dead_pid=0xFFFFFFFE بدل 4_194_301 (آمن Win+Linux). L5/L7-L12 مُهمَلة. اختباران جديدان: **T15** test_release_after_external_delete + **T16** test_release_when_lock_replaced (يثبتان L2). **43/43 cache_lock + 49/49 cache_state + 38/38 hash = 130/130** أخضر بلا تراجع. |
| 2025-12-15 | S-007 Parts C+D **CR Round-2 (Amelia)** — 16 ملاحظات (9 مُهمَلة + 7 مُصلَحة): **R1** 🔴 [cache_lock_posix.cpp](shared/sadinfo_core/src/cache/cache_lock_posix.cpp) `boot_time_unix_ns` — تخزين ثابت من `/proc/stat btime` (سابقاً now-uptime يُحسب كل استدعاء → false-positive PID-reuse على >50ms drift). **R3** 🔴 [cache_lock.cpp](shared/sadinfo_core/src/cache/cache_lock.cpp) `try_acquire` — حلقة محدودة (max=3) لمنع ping-pong عند سباق stale-break بين عمليتين. **R4** 🔴 [cache_lock_win.cpp](shared/sadinfo_core/src/cache/cache_lock_win.cpp) + [cache_lock_posix.cpp](shared/sadinfo_core/src/cache/cache_lock_posix.cpp) — `expected==0`/`actual==0` → `Unknown` بدل `Alive` (يمنع تأكيد حياة كاذب لقفل قديم بلا start_time). **R5** 🟠 `read_lock_file` — `sz>4096` → `InvalidPath` (دلالياً أدق + يمنع release حذف ملف غريب). **R7** 🟠 `try_acquire` ParseError — لا تلمس `out_existing` (PID=0 مضلِّل). **R11** 🟡 [cache_lock_tests.cpp](tests/sadinfo/unit/cache_lock_tests.cpp) — تحديث رأس التعليق ليطابق T1-T16. **R12** 🟡 حذف namespace anonymous فارغ. **R14** 🟢 [cache_lock.h](shared/sadinfo_core/include/cache_lock.h) — `[[nodiscard]]` على `try_acquire/release/inspect/force_break`. R2/R6/R8-R10/R13/R15-R16 مُهمَلة (cosmetic/خارج النطاق). البناء أخضر بلا تحذيرات. **43/43 + 49/49 + 38/38 = 130/130** ✅ |
| 2025-12-15 | S-007 Parts C+D **CR Round-3 (Amelia)** — 10 ملاحظات (جميعها مُطبَّقة): **R17** 🔴 [cache_lock_posix.cpp](shared/sadinfo_core/src/cache/cache_lock_posix.cpp) `boot_time_unix_ns` — إزالة static cached (منع false-Reused على NTP drift). **R18** 🟠 disk-full في `platform_create_excl` — cleanup + return false بدل break. **R19** 🟠 [cache_lock.cpp](shared/sadinfo_core/src/cache/cache_lock.cpp) `try_acquire` — مسح out_existing في بداية كل iteration + توثيق العقد في header. **R20** 🟠 `parse_lock` — إزالة pid==0→ParseError (فصل طبقات؛ check_owner يتعامل). **R21** 🟡 [cache_lock_win.cpp](shared/sadinfo_core/src/cache/cache_lock_win.cpp) — إضافة ERROR_NOT_FOUND لقائمة Dead. **R24** 🟡 POSIX tolerance 50ms→20ms (يقلل false-Alive على PID-reuse سريع). **R22/R23/R25/R26** 🟡🟢 توثيق فقط (DeleteFileW edge، Mismatch edge، LockInfo ==، ping-pong test). البناء أخضر بلا تحذيرات. **130/130** ✅ (43+49+38). |


## S-007 Part E1 — CacheManager Facade ✅
- أنشئ `tools/sadinfo/include/cache_manager.h` (5 طرق فقط، AC-E1.1)
- أنشئ `tools/sadinfo/src/cache_manager.cpp` (facade لـCacheLock + CacheState)
- أضيف `--cache-info` و `--cache-clear` و `--cache-dir <path>` إلى CLI
- أنشئ 12 اختبار في `tests/sadinfo/unit/cache_manager_tests.cpp` (M1-M12)
- النتيجة: 142/142 PASS (hash:38 + state:49 + lock:43 + manager:12)
- nlohmann/json PRIVATE فقط (AC-E1.6)
- ملاحظة: binary `sadinfo` لا يُبنى بسبب ملفات مفقودة سابقة من recovery branch (`sadinfo_main.cpp`, `json_writer.cpp`, إلخ) — خارج نطاق Part E.
