---
status: done
slug: utm-6-5-gen-builtins-network-platform
created: 2026-06-09
completed: 2026-06-09
scope: UTM-6.5 — ترحيل Network + Platform/Kernel (216 ثابت)
epic: UTM-6 (codegen unification — ADR-006a)
priority: high
---

# UTM-6.5 — ترحيل Network + Platform/Kernel (216 ثابت)

## الهدف
ترحيل 12 namespace من `builtin_registry.h` إلى YAML files مُولَّدة تلقائياً:
- **Network (108 ثابت):** Sockets(19) + HttpClient(18) + HttpServer(19) + NetworkUtils(12) + WebSocketClient(13) + WebSocketServer(12) + SadNet(15)
- **Platform/Kernel (108 ثابت):** Platform(9) + Kernel(30) + KernelTimers(30) + KernelThreads(20) + KernelUSB(19)

**بعد الانتهاء:** `builtin_names_generated.h` سيحتوي **393 ثابت** في 21 namespace.

---

## معايير القبول (ACs)

- [x] **AC-1:** `data/language/builtins/stdlib_network.yaml` موجود ويحتوي بالضبط 108 ثوابت (7 namespaces).
- [x] **AC-2:** `data/language/builtins/stdlib_platform.yaml` موجود ويحتوي بالضبط 108 ثوابت (5 namespaces).
- [x] **AC-3:** `gen_all.py` يُمرِّر 9 ملفات YAML لـ `gen_builtins`.
- [x] **AC-4:** `cmake/codegen.cmake` يراقب 9 ملفات YAML في DEPENDS.
- [x] **AC-5:** `builtin_names_generated.h` مُولَّد بـ 393 ثابت في 21 namespace.
- [x] **AC-6:** 12 namespace يدوية مُزالة من `builtin_registry.h` واستُبدلت بتعليق UTM-6.5.
- [x] **AC-7:** pytest ≥ 85 اختبار — كلها تمر (86/86 ✅).
- [x] **AC-8:** `cmake --build build --config Debug --target sad` → EXIT_CODE=0.

---

## المهام

### T1: إنشاء ملفَّي YAML
- [x] `data/language/builtins/stdlib_network.yaml` — 7 namespaces × ثوابتها (108 إجمالاً)
- [x] `data/language/builtins/stdlib_platform.yaml` — 5 namespaces × ثوابتها (108 إجمالاً)
- [x] التحقق diff=0: كل قيمة canonical مطابقة لـ builtin_registry.h

### T2: تحديث gen_all.py + cmake/codegen.cmake
- [x] `gen_all.py`: تمرير 9 ملفات (7 سابقة + 2 جديدة)
- [x] `cmake/codegen.cmake`: SAD_BN_ALL_YAMLS تُضاف إليها الملفان الجديدان
- [x] `[gen_builtins] OK: 393 builtins from 9 YAML files`

### T3: تحديث builtin_registry.h
- [x] حذف 12 namespace يدوية (Sockets..KernelUSB)
- [x] إضافة تعليق UTM-6.5

### T4: الاختبارات + بناء C++
- [x] `test_gen_builtins.py`: إضافة `class TestUTM65` (11 اختبار)
  - [x] test_stdlib_network_count_is_108
  - [x] test_stdlib_platform_count_is_108
  - [x] test_run_with_9_yaml_files_produces_393
  - [x] test_diff_zero_sockets_tcp_new
  - [x] test_diff_zero_kernel_port_write
  - [x] test_network_has_7_namespaces
  - [x] test_platform_has_5_namespaces
  - [x] test_total_21_namespaces_in_generated_header
- [x] pytest: **86/86** passed (≥85 ✅)
- [x] `cmake --build build --config Debug --target sad` → EXIT_CODE=0

---

## ملف القائمة (File List)

| الملف | النوع | الحالة |
|-------|-------|--------|
| `data/language/builtins/stdlib_network.yaml` | جديد | ✅ |
| `data/language/builtins/stdlib_platform.yaml` | جديد | ✅ |
| `scripts/codegen/gen_all.py` | تعديل | ✅ |
| `cmake/codegen.cmake` | تعديل | ✅ |
| `shared/builtins/include/builtin_registry.h` | تعديل | ✅ |
| `shared/builtins/generated/builtin_names_generated.h` | مُولَّد | ✅ |
| `scripts/codegen/test_gen_builtins.py` | تعديل | ✅ |
| `scripts/codegen/gen_builtins.py` | تعديل | ✅ |

---

## سجل وكيل التطوير

**2026-06-09 — Amelia**

- **T1 ✅:** إنشاء `stdlib_network.yaml` (108 / 7 ns) و `stdlib_platform.yaml` (108 / 5 ns). diff=0 مُؤكَّد.
- **T2 ✅:** `gen_all.py` يُمرِّر 9 ملفات. `cmake/codegen.cmake` SAD_BN_ALL_YAMLS = 9 ملفات. `gen_all.py` → `OK: 393 builtins from 9 YAML files`.
- **T2-fix ✅:** `gen_builtins.py` — إصلاح `validate_functions` لفحص `(cpp_id, namespace)` كزوج بدل `cpp_id` منفرداً (سبب: `NEW_CLIENT` مشترك بين HttpClient وWebSocketClient — نمطان مختلفان مسموح به).
- **T3 ✅:** سكريبت Python يحذف الكتلة من أول `UIWidgets` الوهمي إلى أول `UIWidgets` الحقيقي (20379 حرف). فحص: `namespace Sockets|HttpClient|...|KernelUSB` = 0 نتيجة في الملف.
- **T4 ✅:** إضافة `class TestUTM65` (11 اختبار) في `test_gen_builtins.py`. إجمالي pytest = **86/86**. C++ build → EXIT_CODE=0.

**نقاط الضعف التي اكتُشفت وعُولجت:**
1. `validate_functions` كانت تفحص `cpp_id` منفرداً → قد تمنع نفس الاسم في namespaces مختلفة (صواب: الزوج).
2. الاستبدال الجزئي في `builtin_registry.h` أفضى لكتلة وهمية — الحل: Python script يعتمد على "أول ظهور" vs "ثاني ظهور" للعلامة نفسها.
3. `std::set<BasicBlock*>` في LLVM codegen يُرتِّب أبجدياً → قد يكسر ترتيب التنفيذ (BF-16 — موثَّق لـ UTM لاحقة).
