# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/sources.cmake
# الوصف: تعريف جميع ملفات المصدر / All Source File Definitions
# ═══════════════════════════════════════════════════════════════════════════════
# هذا الملف يجمع كل قوائم ملفات المصدر في مكان واحد
# لتسهيل الإضافة والحذف والمراجعة.
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# 1. الأدوات المساعدة / Utilities
# ──────────────────────────────────────────────────────────────────────
set(UTILS_SOURCES
    shared/utils/src/string_utils.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 2. المحلل المعجمي / Lexer
# ──────────────────────────────────────────────────────────────────────
set(LEXER_SOURCES
    shared/lexer/src/token.cpp
    shared/lexer/src/lexer_core.cpp
    shared/lexer/src/lexer_keywords.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 3. المحلل النحوي / Parser
# ──────────────────────────────────────────────────────────────────────
set(PARSER_SOURCES
    shared/parser/src/core/parser_core.cpp
    shared/parser/src/core/parser_main.cpp
    shared/parser/src/declarations/parser_declarations.cpp
    shared/parser/src/statements/parser_statements.cpp
    shared/parser/src/core/parser_expressions.cpp
    shared/parser/src/statements/parser_advanced.cpp
    shared/parser/src/core/parser_helpers.cpp
    shared/parser/src/declarations/parser_oop.cpp
    shared/parser/src/declarations/parser_modules.cpp
    shared/parser/src/ui/parser_ui.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 4. عُقد الشجرة النحوية / AST Nodes
# ──────────────────────────────────────────────────────────────────────
set(AST_SOURCES
    shared/ast/src/ast_node.cpp
    shared/ast/src/expressions.cpp
    shared/ast/src/statements.cpp
    shared/ast/src/declarations.cpp
    shared/ast/src/property_nodes.cpp
    shared/ast/src/module_nodes.cpp
    shared/ast/src/ast_printer.cpp
    shared/ast/src/type_nodes.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 5. المفسر / Interpreter
# ──────────────────────────────────────────────────────────────────────
set(INTERPRETER_SOURCES
    interpreter/src/core/interpreter_core.cpp
    # (AR) EM-CPP-0: سياق استدعاء الدوال المضمنة (BuiltinContext) — أساس ترحيل أخطاء C++.
    interpreter/src/builtins/builtin_context.cpp
    interpreter/src/builtins/builtin_registry.cpp
    interpreter/src/builtins/builtin_core_io.cpp
    interpreter/src/builtins/builtin_module_strings.cpp
    interpreter/src/builtins/builtin_module_basics.cpp
    interpreter/src/builtins/builtin_module_math.cpp
    interpreter/src/builtins/builtin_module_assertions.cpp
    interpreter/src/builtins/builtin_module_async.cpp
    interpreter/src/builtins/builtin_module_maps.cpp
    interpreter/src/builtins/builtin_module_maps_core.cpp
    interpreter/src/builtins/builtin_module_maps_json_xml.cpp
    interpreter/src/builtins/builtin_module_maps_arrays.cpp
    interpreter/src/builtins/builtin_module_maps_text.cpp
    interpreter/src/builtins/builtin_module_maps_utils.cpp
    interpreter/src/builtins/builtin_kernel_cpu.cpp
    interpreter/src/builtins/builtin_kernel_uefi.cpp
    interpreter/src/builtins/builtin_kernel_acpi.cpp
    interpreter/src/builtins/builtin_kernel_gpu.cpp
    interpreter/src/builtins/builtin_kernel_usb.cpp
    interpreter/src/builtins/builtin_kernel_storage.cpp
    interpreter/src/builtins/builtin_kernel_network.cpp
    interpreter/src/builtins/builtin_kernel_audio.cpp
    interpreter/src/builtins/builtin_kernel_timers.cpp
    interpreter/src/builtins/builtin_kernel_serial.cpp
    interpreter/src/builtins/builtin_kernel_memory.cpp
    interpreter/src/builtins/builtin_kernel_threads.cpp
    interpreter/src/builtins/builtin_module_ffi.cpp
    interpreter/src/builtins/builtin_module_exceptions.cpp
    interpreter/src/builtins/builtin_module_platform.cpp
    interpreter/src/builtins/builtin_module_sockets.cpp
    interpreter/src/builtins/builtin_module_http.cpp
    interpreter/src/builtins/builtin_module_sadnet.cpp
    interpreter/src/builtins/builtin_module_websocket.cpp
    # (AR) م2-أ (sadlang-rfcs#10): ملفّات الواجهات المعتمِدة على sad_graphics انتُقِلت إلى
    #      هدف sad_graphics_bridge (INTERPRETER_UI_BRIDGE_SOURCES أدناه) كي لا يعتمد sad_interp
    #      على sad_graphics. يبقى في القلب فقط ما لا يعتمد sad_graphics:
    # (EN) Phase 2-A: sad_graphics-dependent UI files moved to the sad_graphics_bridge target
    #      (INTERPRETER_UI_BRIDGE_SOURCES below) so sad_interp no longer depends on sad_graphics.
    #      Only the sad_graphics-free pieces remain here in the core:
    interpreter/src/ui/ui_state_manager.cpp          # (AR) بلا sad_graphics؛ يحتاجه oop_new (UIStateManager)
    interpreter/src/visitors/ui_eval_bridge_core.cpp   # (AR) بذرة القلب (المؤشّر الذرّيّ + المزوّدون)
    interpreter/src/visitors/ui_widget_expr_dispatch.cpp # (AR) visitUIWidgetExpr الرفيع (TU مستقلّ ليُشبِعه الـstub في الاختبارات)
    interpreter/src/visitors/expression_evaluator_core.cpp
    interpreter/src/visitors/expression_evaluator_binary_ops.cpp
    interpreter/src/visitors/expression_evaluator_binary_logic.cpp
    interpreter/src/visitors/expression_evaluator_overloads.cpp
    interpreter/src/visitors/expression_evaluator_calls.cpp
    interpreter/src/visitors/expression_evaluator_calls_invoke.cpp
    interpreter/src/visitors/expression_evaluator_calls_macro.cpp
    interpreter/src/visitors/expression_evaluator_calls_user_func.cpp
    interpreter/src/visitors/expression_evaluator_calls_dispatch.cpp
    interpreter/src/visitors/expression_evaluator_oop.cpp
    interpreter/src/visitors/expression_evaluator_oop_new.cpp
    interpreter/src/visitors/expression_evaluator_oop_array_methods.cpp
    interpreter/src/visitors/expression_evaluator_oop_string_map_methods.cpp
    interpreter/src/visitors/expression_evaluator_oop_concurrency.cpp
    interpreter/src/visitors/expression_evaluator_members.cpp
    interpreter/src/visitors/expression_evaluator_members_assign.cpp
    interpreter/src/visitors/expression_evaluator_members_advanced.cpp
    # (AR) م2-أ: expression_evaluator_ui.cpp (تطبيق seam 2) انتقل إلى INTERPRETER_UI_BRIDGE_SOURCES
    interpreter/src/visitors/statement_executor.cpp
    interpreter/src/visitors/statement_executor_control.cpp
    interpreter/src/visitors/statement_executor_control_exceptions.cpp
    interpreter/src/visitors/statement_executor_functions.cpp
    interpreter/src/visitors/statement_executor_functions_templates.cpp
    interpreter/src/visitors/statement_executor_oop.cpp
    interpreter/src/visitors/statement_executor_oop_types.cpp
    interpreter/src/visitors/statement_executor_oop_struct_test.cpp
    interpreter/src/visitors/statement_executor_modules.cpp
    # (AR) interpreter/src/exception.cpp حُذف في Phase 4 — استبدل بـ shared/errors
    # (EN) exception.cpp removed in Phase 4 — replaced by shared/errors
    interpreter/src/debug/debug_server.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 6. إدارة البيانات والأنواع / Data & Type Management
# (AR) ملفات shared/types/* أصبحت تأتي عبر sad_shared (إزالة الازدواج)
# (EN) shared/types/* now come via sad_shared (de-duplicated)
# ──────────────────────────────────────────────────────────────────────
# (AR) م3 خطوة 4 (RFC sadlang-rfcs#10): function_manager + ownership_manager خرجا إلى
#      مكتبة sad_runtime (خدمات وقت التشغيل المشتركة، انظر SAD_RUNTIME_SOURCES أدناه).
#      Scope/Variable يبقيان هنا (شجريّان خاصّان بالمفسّر الشجريّ).
# (EN) Phase-3 step-4: function_manager + ownership_manager extracted to the sad_runtime
#      library (shared runtime services). Scope/Variable stay here (tree-walk only).
set(DATA_SOURCES
    interpreter/src/managers/variable_manager.cpp
    interpreter/src/managers/scope_manager.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 7. البرمجة الكائنية / OOP (Object-Oriented Programming)
# (AR) OOP_TYPES_SOURCES و OOP_AST_SOURCES و class_manager نُقلت إلى sad_shared
#      للقضاء على ازدواج البناء (Phase: dedup sad_shared/sad_interp).
# (EN) OOP_TYPES_SOURCES, OOP_AST_SOURCES, and class_manager moved to sad_shared
#      to eliminate build duplication (Phase: dedup sad_shared/sad_interp).
# ──────────────────────────────────────────────────────────────────────
set(OOP_TYPES_SOURCES)

# (AR) م3 خطوة 4: object_manager خرج إلى مكتبة sad_runtime (SAD_RUNTIME_SOURCES).
# (EN) Phase-3 step-4: object_manager extracted to the sad_runtime library.
set(OOP_MANAGERS_SOURCES)

set(OOP_AST_SOURCES)

# معطل - تم نقله إلى parser_core_oop.cpp
set(OOP_PARSER_SOURCES)
set(OOP_INTERPRETER_SOURCES)

# ──────────────────────────────────────────────────────────────────────
# 8. الدوال المضمنة / Built-in Functions
# (AR) Phase F-XX: builtins.cpp في shared/builtins/src/runtime/
# (EN) Phase F-XX: builtins.cpp under shared/builtins/src/runtime/
# ──────────────────────────────────────────────────────────────────────
set(BUILTINS_SOURCES
    shared/builtins/src/runtime/builtins.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 9. إدارة الأخطاء / Error Management
# (AR) أُزيل ERROR_SOURCES اليتيم — جميع ملفات الأخطاء تُبنى عبر sad_shared
#      (راجع shared/CMakeLists.txt). كان متغيراً معرَّفاً بلا استعمال.
# (EN) Orphan ERROR_SOURCES removed — all error files build via sad_shared
#      (see shared/CMakeLists.txt). Variable was defined but never used.
# ──────────────────────────────────────────────────────────────────────

# ──────────────────────────────────────────────────────────────────────
# 10. المكتبة القياسية / Standard Library
# (AR) Phase F-XX: الجزء الأساسي في shared/builtins/src/runtime/
# (EN) Phase F-XX: core runtime under shared/builtins/src/runtime/
# ──────────────────────────────────────────────────────────────────────
set(STDLIB_SOURCES
    shared/builtins/src/runtime/stdlib_manager.cpp
    shared/builtins/src/runtime/type_functions.cpp
    shared/builtins/src/runtime/array_functions.cpp
    shared/builtins/src/runtime/other_functions.cpp
    stdlib/string/string_functions.cpp
    stdlib/math/math_functions.cpp
    stdlib/math/advanced_math.cpp
    stdlib/io/io_functions.cpp
    stdlib/filesystem/filesystem_module.cpp
    stdlib/database/database_module.cpp
    stdlib/json/json_module.cpp
    stdlib/xml/xml_module.cpp
    stdlib/crypto/crypto_module.cpp
    # stdlib/crypto/crypto_builtins.cpp  # (AR) معطّل — يستخدم نظام include مختلف عن sad_interp
    stdlib/system/system_functions.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 11. المحسن / Optimizer
# ──────────────────────────────────────────────────────────────────────
# (AR) ملفات المحسن AST-level أُزيلت بعد توحيد passes في compiler/src/sir_optimizer/
#      (sad_optimizer). راجع compiler/docs/optimization_layers.md
# (EN) AST-level optimizer files removed after unifying passes in
#      compiler/src/sir_optimizer/ (sad_optimizer). See compiler/docs/optimization_layers.md
set(OPTIMIZER_SOURCES
)

# ──────────────────────────────────────────────────────────────────────
# 12. البرمجة منخفضة المستوى / Low-Level Programming
# ──────────────────────────────────────────────────────────────────────
set(LOW_LEVEL_SOURCES
    stdlib/low_level/src/pointer_type.cpp
    stdlib/low_level/src/bitwise_ops.cpp
    stdlib/low_level/src/interrupts.cpp
    stdlib/low_level/src/io_ports.cpp
    stdlib/low_level/src/memory_map.cpp
    stdlib/low_level/src/cpu.cpp
    stdlib/low_level/src/gdt.cpp
    stdlib/low_level/src/paging.cpp
    stdlib/low_level/src/pci.cpp
    stdlib/low_level/src/dma.cpp
    stdlib/low_level/src/framebuffer.cpp
    stdlib/low_level/src/acpi.cpp
    stdlib/low_level/src/sync.cpp
    stdlib/low_level/src/scheduler.cpp
    stdlib/low_level/src/boot.cpp
    stdlib/low_level/src/vfs.cpp
    stdlib/low_level/src/apic.cpp
    stdlib/low_level/src/hpet.cpp
    stdlib/low_level/src/syscall.cpp
    stdlib/low_level/src/uefi.cpp
    stdlib/low_level/src/usb.cpp
    stdlib/low_level/src/nvme.cpp
    stdlib/low_level/src/network_stack.cpp
    stdlib/low_level/src/audio.cpp
    stdlib/low_level/src/timer.cpp
    stdlib/low_level/src/serial.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 13. الواجهة الأمامية للمترجم / Compiler Frontend (SIR)
# ──────────────────────────────────────────────────────────────────────
set(COMPILER_FRONTEND_SOURCES
    compiler/src/frontend/sir_types.cpp
    compiler/src/frontend/sir_module.cpp
    compiler/src/frontend/sir_instruction.cpp
    compiler/src/frontend/sir_builder_module.cpp
    compiler/src/frontend/sir_builder_helpers.cpp
    # (AR) Phase 6 — Step 3: ملفات sir_builder_builtins_*.cpp نُقلت إلى
    #      compiler/src/frontend/builders/builtins_*.cpp وتُلتقط عبر GLOB
    #      في compiler/CMakeLists.txt — لا حاجة لإضافتها هنا.
    # (EN) Phase 6 — Step 3: builtin files moved to builders/ and picked up
    #      via GLOB in compiler/CMakeLists.txt — no need to list here.
    compiler/src/frontend/sir_builder_module_check.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 14. دعم وضع Freestanding / Freestanding Mode Support
# ──────────────────────────────────────────────────────────────────────
# (AR) دعم شامل لتوليد كود وضع بلا_مكتبة_قياسية (bare-metal)
# (EN) Comprehensive support for no_std / freestanding code generation
# ──────────────────────────────────────────────────────────────────────
# (AR) حُذف FREESTANDING_SOURCES في م5 (RFC sadlang-rfcs#10) — كان مؤرشفًا في
#      archived/compiler_pipeline_legacy/ ثمّ أُخلي مجلّد archived/ كلّه (كود ميّت
#      بلا بناء ولا تضمين). السبب الأصليّ: ENABLE_FREESTANDING لم يُعرَّف أبداً عبر
#      option() فكان الفرع ميتاً، ومصادر compiler/src/pipeline/* لها 0 مستدعون خارجيون.
# (EN) FREESTANDING_SOURCES deleted in phase 5 (RFC #10) — was archived under
#      archived/compiler_pipeline_legacy/, then the whole archived/ tree was removed
#      (dead code, no build/include). ENABLE_FREESTANDING was never defined;
#      compiler/src/pipeline/* had 0 external callers.
set(FREESTANDING_SOURCES "")

# ──────────────────────────────────────────────────────────────────────
# 15. نظام الوحدات / Module System
# ──────────────────────────────────────────────────────────────────────
set(MODULES_SOURCES
    shared/modules/src/module.cpp
    shared/modules/src/search_paths.cpp
    shared/modules/src/module_cache.cpp
    shared/modules/src/dependency_graph.cpp
    shared/modules/src/module_loader.cpp
    shared/modules/src/module_resolver.cpp
    shared/modules/src/symbol_resolver.cpp
    shared/modules/src/module_validator.cpp
)

# ──────────────────────────────────────────────────────────────────────
# إعادة التحميل الساخن / Hot Reload
# ──────────────────────────────────────────────────────────────────────
set(HOT_RELOAD_SOURCES
    shared/hot_reload/src/file_watcher.cpp
    shared/hot_reload/src/hot_reload_engine.cpp
)

# ──────────────────────────────────────────────────────────────────────
# تجميع جميع المصادر / Aggregate all sources
# ──────────────────────────────────────────────────────────────────────
# (AR) المجموعات التالية أُزيلت لأنها تُبنى عبر مكتبة sad_shared:
#       UTILS_SOURCES, LEXER_SOURCES, PARSER_SOURCES, AST_SOURCES,
#       OOP_TYPES_SOURCES, OOP_AST_SOURCES, ERROR_SOURCES, MODULES_SOURCES،
#       بالإضافة إلى ملفات shared/types من DATA_SOURCES و class_manager.
#       sad_interp يربط sad_shared كـ PUBLIC في cmake/libraries.cmake.
# (EN) The following groups were removed because sad_shared already builds them:
#       UTILS_SOURCES, LEXER_SOURCES, PARSER_SOURCES, AST_SOURCES,
#       OOP_TYPES_SOURCES, OOP_AST_SOURCES, ERROR_SOURCES, MODULES_SOURCES,
#       plus shared/types files from DATA_SOURCES and class_manager.
#       sad_interp links sad_shared PUBLIC in cmake/libraries.cmake.
# ===============================================================================
# (AR) م2-ج (sadlang-rfcs#10): مصادر مكتبة sad_builtins — المدمجات النقيّة التي
#      تعمل على Sad::Data::Value فقط (لا تعتمد FunctionManager/Interpreter). تُستخرَج
#      من sad_interp إلى هدف مستقلّ يعتمد sad_shared وحده، فيكسر دورة:
#        sad_interp → sad_builtins (اتّجاه واحد؛ stdlib_manager اللِحام يبقى في القلب).
# (EN) Phase 2-C: sad_builtins sources — pure Data::Value builtins (no FunctionManager
#      dependency). Extracted from sad_interp into a standalone target depending only on
#      sad_shared, breaking the sad_interp<->builtins cycle (stdlib_manager glue stays).
# ===============================================================================
# ===============================================================================
# (AR) م3 خطوة 4 (RFC sadlang-rfcs#10): مصادر مكتبة sad_runtime — طبقة خدمات وقت
#      التشغيل المشتركة المستخرَجة من قلب المفسّر. تضمّ المدراء الثلاثة الأقلّ اقترانًا
#      بالمشي على الشجرة:
#        • function_manager  — سجلّ الدوال (يستعمل BuiltinContext كنوعٍ في توقيعات
#          std::function فقط، لا يستدعي مناهجه ⇒ بلا دورة رموز مع sad_interp).
#        • object_manager    — دورة حياة الكائنات (0 اعتماد على دواخل المفسّر).
#        • ownership_manager — غلاف نظام الملكية الموحَّد sad_ownership (0 اعتماد).
#      Scope/Variable يبقيان في sad_interp (شجريّان). sad_interp يربط sad_runtime
#      PUBLIC؛ الاتّجاه أحاديّ (sad_interp → sad_runtime). هذا هو موضع شقيق الآلة
#      الافتراضية عند عودتها (تشترك المحرّكات في خدمات وقت التشغيل، لا في المشي).
# (EN) Phase-3 step-4: sad_runtime library sources — shared runtime-services layer
#      extracted from the interpreter core. The three managers least coupled to tree
#      walking. One-directional (sad_interp → sad_runtime). Future home of the VM's
#      shared runtime services (engines share runtime services, not the walker).
# ===============================================================================
set(SAD_RUNTIME_SOURCES
    interpreter/src/managers/function_manager.cpp
    interpreter/src/managers/object_manager.cpp
    interpreter/src/managers/ownership_manager.cpp
)

# (AR) مصادر مكتبة sad_builtins النقيّة (تعمل على Data::Value فقط). تطابق
#      محتوى STDLIB_SOURCES باستثناء stdlib_manager.cpp (اللِحام يبقى في القلب).
# (EN) Pure sad_builtins library sources (operate on Data::Value only). Mirrors
#      STDLIB_SOURCES minus stdlib_manager.cpp (the glue stays in the core).
set(SAD_BUILTINS_LIB_SOURCES
    shared/builtins/src/runtime/builtins.cpp
    shared/builtins/src/runtime/type_functions.cpp
    shared/builtins/src/runtime/array_functions.cpp
    shared/builtins/src/runtime/other_functions.cpp
    stdlib/string/string_functions.cpp
    stdlib/math/math_functions.cpp
    stdlib/math/advanced_math.cpp
    stdlib/io/io_functions.cpp
    stdlib/filesystem/filesystem_module.cpp
    stdlib/database/database_module.cpp
    stdlib/json/json_module.cpp
    stdlib/xml/xml_module.cpp
    stdlib/crypto/crypto_module.cpp
    stdlib/system/system_functions.cpp
)

set(ALL_SOURCES
    ${INTERPRETER_SOURCES}
    ${DATA_SOURCES}
    ${OOP_MANAGERS_SOURCES}
    ${OOP_PARSER_SOURCES}
    ${OOP_INTERPRETER_SOURCES}
    # (AR) م2-ج: المدمجات النقيّة خرجت إلى sad_builtins؛ يبقى لِحام التسجيل فقط.
    # (EN) Phase 2-C: pure builtins moved to sad_builtins; only glue stays here.
    shared/builtins/src/runtime/stdlib_manager.cpp
    ${OPTIMIZER_SOURCES}
    # (AR) م2-ج شريحة2: مدمجات النواة/العتاد النقيّة خرجت إلى هدف sad_lowlevel
    #      (لا تلمس Data::Value)؛ لِحامها builtin_kernel_*.cpp يبقى في القلب.
    # (EN) Phase 2-C slice2: pure kernel/low-level builtins moved to sad_lowlevel;
    #      their glue (builtin_kernel_*.cpp) stays in the core.
    ${COMPILER_FRONTEND_SOURCES}
    ${HOT_RELOAD_SOURCES}
)

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) م2-أ (sadlang-rfcs#10): مصادر طبقة جسر الواجهات sad_graphics_bridge.
#      هذه الملفّات تعتمد sad_graphics (sad_ui/ir.h …) وتُجمَّع في هدف منفصل يربط
#      sad_core (PUBLIC، alias لـ sad_interp) + sad_graphics (PRIVATE)، فيُعكَس اعتماد القلب
#      على الواجهات: القلب لم يعد يربط sad_graphics؛ الجسر يثبّت نفسه عبر installSadGraphicsBridge().
#      المكتبة تُعرَّف في cmake/libraries.cmake وتُربَط في sad-run/profiler/wasm.
# (EN) Phase 2-A: sources of the UI bridge layer (sad_graphics_bridge). These depend on
#      sad_graphics and compile into a separate target linking sad_core PUBLIC (alias of
#      sad_interp) + sad_graphics PRIVATE, inverting the core→ui dependency. Defined in
#      cmake/libraries.cmake.
# ═══════════════════════════════════════════════════════════════════════════════
set(INTERPRETER_UI_BRIDGE_SOURCES
    interpreter/src/ui/ui_bridge.cpp
    interpreter/src/ui/ui_bridge_events.cpp
    interpreter/src/ui/ui_bridge_platform.cpp
    interpreter/src/ui/ui_builtins.cpp
    interpreter/src/ui/ui_core_builtins.cpp
    interpreter/src/ui/ui_state_builtins.cpp
    interpreter/src/ui/ui_timer_builtins.cpp
    interpreter/src/ui/ui_storage_builtins.cpp
    interpreter/src/ui/ui_dialog_builtins.cpp
    interpreter/src/ui/ui_audio_builtins.cpp
    interpreter/src/ui/ui_io_builtins.cpp
    interpreter/src/ui/ui_device_builtins.cpp
    interpreter/src/ui/ui_network_builtins.cpp
    interpreter/src/ui/ui_crypto_builtins.cpp
    interpreter/src/ui/ui_platform_builtins.cpp
    interpreter/src/ui/widget_builder.cpp
    interpreter/src/ui/widget_builtins.cpp
    interpreter/src/ui/ui_widget_method_call.cpp
    interpreter/src/ui/ui_module_registration.cpp
    interpreter/src/visitors/expression_evaluator_ui.cpp
)


