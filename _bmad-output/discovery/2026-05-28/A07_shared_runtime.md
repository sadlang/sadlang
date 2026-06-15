# 🧩 تقرير الاكتشاف — Shared Core + Runtime (W7)

> **تاريخ التقرير:** 2026-05-28
> **النطاق:** `shared/` كاملاً + `runtime/`
> **الوضع:** READ-ONLY

## 1. ملخص المنطقة

| المقياس | القيمة |
|---|---|
| إجمالي الملفات | 295+ (253 shared + 42 runtime) |
| ملفات C++ | ~250 (.h: 100, .cpp: 150) |
| ملفات CMake | 2 |
| ملفات مولّدة (codegen) | 2 (keywords_generated.*) |
| المكتبات الفرعية | 21 (17 shared + 4 runtime) |
| السطور التقريبية | ~25,000+ |

### التواريخ
- **أحدث:** 2026-01-09 (lexer_keywords.cpp)
- **أقدم:** 2025-11-11 (parser_core.h)
- **متوسط:** 2025-12-15

## 2. الأنظمة المُكتشفة — Shared Core (17 مكتبة)

### ① LEXER — المحلل المعجمي
- **الملفات:** [shared/lexer/include/token.h](../../../shared/lexer/include/token.h), [lexer_core.h](../../../shared/lexer/include/lexer_core.h), [lexer_keywords.h](../../../shared/lexer/include/lexer_keywords.h)
- **الواجهات:** `TokenType` enum (100+)، `LexerCore` class
- **الخصائص:** 40 كلمة محجوزة، UTF-8 كامل، جدول كلمات مولّد من YAML

### ② PARSER — المحلل النحوي
- **الملفات:** parser_core.h, parser_classes.h + 14 ملف src
- **الواجهات:** `ParserCore` (Recursive Descent)
- **الخصائص:** 60+ قاعدة نحوية، comprehensions, generators, lambdas, async

### ③ AST
- **الملفات:** ast_node.h, expressions.h, statements.h, class_nodes.h, pattern_nodes.h, ui_nodes.h, type_nodes.h
- **الواجهات:** `ASTNode` (base) + Visitor pattern
- **الخصائص:** ~20 expression + 15 statement + OOP + pattern matching + UI

### ④ TYPES
- **الملفات:** [value.h](../../../shared/types/include/value.h), object_instance.h, sad_type_system.h, type_bridge.h, class_type.h
- **الواجهات:** `Value` (variant), `ObjectInstance` (shared_ptr), `SadType`
- **HUB:** value.h مستخدم في 20+ مكتبة — تغيير فيه يؤثر على الكل

### ⑤ ERRORS
- **الملفات:** error_manager.h, error_catalog.h, smart_errors.h + 18 ملف src
- **الخصائص:** ~700+ رسالة، اقتراحات تلقائية، شرح للمبتدئين

### ⑥ MODULES
- **الملفات:** module.h, module_loader.h, module_resolver.h, module_cache.h, dependency_graph.h
- **الخصائص:** كشف circular imports، package.sadinfo، تخزين مؤقت

### ⑦ SEMANTIC
- **الملفات:** type_checker.h + 3 ملفات تنفيذ
- **ملاحظة:** type_checker.cpp ضخم (500+ سطر) — قد يحتاج تقسيم

### ⑧ OWNERSHIP
- **الملفات:** borrow_checker.h, lifetime_analyzer.h, move_analyzer.h, unsafe_checker.h, exhaustiveness.h
- **الخصائص:** مستوحى من Rust

### ⑨ OWNERSHIP_RUNTIME
- **الملفات:** sad_ownership_capi.h, ownership_tracker.h

### ⑩ BUILTINS
- **الملفات:** [builtin_registry.h](../../../shared/builtins/include/builtin_registry.h), builtin_categories.h
- **ADR-003:** Registry مركزي موحّد

### ⑪ MEMORY_GC
- **الملفات:** garbage_collector.h, cycle_detector.h, policy_bridge.h
- **الخصائص:** GC اختياري (--gc mode)

### ⑫ MEMORY_POLICY
- **الملفات:** gc_mode.h, memory_mode_flag.h
- **الخصائص:** دعم --gc, --learn, --prod

### ⑬ SECURITY
- **الملفات:** security_macros.h, taint_tracker.h, bounds_checker.h, safe_allocator.h, input_sanitizer.h

### ⑭ SADINFO_CORE
- **الملفات:** [entity.h](../../../shared/sadinfo_core/include/entity.h), entity_view.h, loader.h, validator.h, tier1_schema.h, cache_lock.h, logging.h
- **الحالة:** Sprint 0-1 ✅ (90%), Sprint 2 partial (60%), Sprint 3+ ❌ (0%)

### ⑮ PROFILER
- **الملفات:** profiler_core.h, profiler_hooks.h
- **الخصائص:** اختياري (--profile flag)

### ⑯ HOT_RELOAD
- **الملفات:** hot_reload_engine.h, file_watcher.h
- **ملاحظة:** غير مستخدم كثيراً — قد يحتاج إزالة

### ⑰ UTILS
- **الملفات:** string_utils.h, utf8_utils.h, utf8_args.h

## 3. Runtime (4 مكتبات)

### ① ABI
- c_abi.h, type_marshal.h
- Calling conventions + Type marshalling

### ② FFI
- ffi.h, firewall.cpp, sandbox.cpp + validators
- جدار حماية + Sandbox + Validation

### ③ FREESTANDING
- sad_core.h, sad_allocator.h, sad_bump_allocator.h, sad_mmio.h, sad_port_io.h, sad_interrupt.h
- يُبنى فقط بـ `--freestanding`
- مستخدم في Ufuq kernel

### ④ UI RUNTIME
- sad_ui_runtime.h, sad_ui_runtime.cpp
- Bridge بين llvm_codegen_ui و sad_ui

## 4. الملاحظات الاستراتيجية

### ✅ نقاط قوة
1. بنية منتظمة ونظيفة — 17 مكتبة بأدوار واضحة
2. توثيق ثنائي اللغة `@brief (AR)` + `@brief (EN)` على كل API
3. فصل قوي للاهتمامات + اعتماديات واضحة
4. Visitor pattern متسق
5. نظام أخطاء ذكي (700+ رسالة + اقتراحات)
6. UTF-8 في كل مكان
7. إدارة ذاكرة آمنة (shared_ptr, RAII, GC اختياري)

### ⚠️ مناطق للمراقبة

| المنطقة | الحالة |
|---|---|
| sadinfo_core | Sprint 3+ صفر |
| ownership | قد يحتاج توسّع |
| type_bridge (ADR-01) | Hub أساسي — أي تغيير يؤثر |
| hot_reload | غير مستخدم — مرشح للإزالة |
| profiler | غير مستخدم كثيراً |
| semantic/type_checker.cpp | معقد (500+ سطر) |

### 🟢 اعتماديات Circular
- value.h ↔ object_instance.h: محلولة عبر forward declaration ✓
- باقي المكتبات: لا circular ✓

### 📦 مراكز الاعتماديات

| المكتبة | الاستخدام | التأثير |
|---|---|---|
| value.h | 20+ مكتبات | **HIGH** |
| error_manager.h | 15+ مكتبات | **MEDIUM** |
| ast_node.h | 10+ مكتبات | **MEDIUM** |
| builtin_registry.h | 5+ مكتبات | **LOW** |
| modules | 2-3 مكتبات | **LOW** |

### 🔧 نقاط القرار الموحّدة

1. **shared/errors/src/builders/dispatch.cpp** — قرار --gc/--learn/--prod
2. **shared/builtins/include/builtin_registry.h** — ADR-003 Registry
3. **shared/types/include/type_bridge.h** — ADR-01 SadType↔Value

## 5. أهم 5 اكتشافات

| # | الاكتشاف | التأثير |
|---|---|---|
| 1 | sadinfo_core Sprint 3+ صفر | متوقف |
| 2 | type_bridge HUB أساسي يستخدم 20+ | حذر عند التعديل |
| 3 | hot_reload + profiler غير مستخدمين | مرشحان للإزالة |
| 4 | type_checker.cpp 500+ سطر | يحتاج تقسيم |
| 5 | runtime/freestanding للـ Ufuq kernel | معتمد عليه |

## 6. التوصيات

| الأولوية | الإجراء |
|---|---|
| P1 | استكمال sadinfo_core Sprint 2-3 |
| P2 | مراقبة type_bridge (hub أساسي) |
| P3 | تقييم إزالة hot_reload + profiler |
| P3 | تقسيم type_checker.cpp إن لزم |

---
**أُنشئ:** 2026-05-28 | **النوع:** READ-ONLY
