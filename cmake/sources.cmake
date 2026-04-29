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
    # UI Bridge (يربط المفسر بنظام واجهات sad_ui)
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
    interpreter/src/ui/ui_state_manager.cpp
    interpreter/src/ui/widget_builder.cpp
    interpreter/src/ui/widget_builtins.cpp
    interpreter/src/visitors/expression_evaluator_core.cpp
    interpreter/src/visitors/expression_evaluator_binary_ops.cpp
    interpreter/src/visitors/expression_evaluator_binary_logic.cpp
    interpreter/src/visitors/expression_evaluator_overloads.cpp
    interpreter/src/visitors/expression_evaluator_calls.cpp
    interpreter/src/visitors/expression_evaluator_calls_invoke.cpp
    interpreter/src/visitors/expression_evaluator_calls_macro.cpp
    interpreter/src/visitors/expression_evaluator_calls_user_func.cpp
    interpreter/src/visitors/expression_evaluator_calls_macro.cpp
    interpreter/src/visitors/expression_evaluator_calls_user_func.cpp    interpreter/src/visitors/expression_evaluator_calls_dispatch.cpp    interpreter/src/visitors/expression_evaluator_oop.cpp
    interpreter/src/visitors/expression_evaluator_oop_new.cpp
    interpreter/src/visitors/expression_evaluator_oop_array_methods.cpp
    interpreter/src/visitors/expression_evaluator_oop_string_map_methods.cpp
    interpreter/src/visitors/expression_evaluator_oop_concurrency.cpp
    interpreter/src/visitors/expression_evaluator_members.cpp
    interpreter/src/visitors/expression_evaluator_members_assign.cpp
    interpreter/src/visitors/expression_evaluator_members_advanced.cpp
    interpreter/src/visitors/expression_evaluator_ui.cpp
    interpreter/src/visitors/statement_executor.cpp
    interpreter/src/visitors/statement_executor_control.cpp
    interpreter/src/visitors/statement_executor_control_exceptions.cpp
    interpreter/src/visitors/statement_executor_functions.cpp
    interpreter/src/visitors/statement_executor_functions_templates.cpp
    interpreter/src/visitors/statement_executor_oop.cpp
    interpreter/src/visitors/statement_executor_oop_types.cpp
    interpreter/src/visitors/statement_executor_oop_struct_test.cpp
    interpreter/src/visitors/statement_executor_modules.cpp
    interpreter/src/exception.cpp
    interpreter/src/debug/debug_server.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 6. إدارة البيانات والأنواع / Data & Type Management
# (AR) ملفات shared/types/* أصبحت تأتي عبر sad_shared (إزالة الازدواج)
# (EN) shared/types/* now come via sad_shared (de-duplicated)
# ──────────────────────────────────────────────────────────────────────
set(DATA_SOURCES
    interpreter/src/managers/variable_manager.cpp
    interpreter/src/managers/function_manager.cpp
    interpreter/src/managers/scope_manager.cpp
    interpreter/src/managers/ownership_manager.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 7. البرمجة الكائنية / OOP (Object-Oriented Programming)
# (AR) OOP_TYPES_SOURCES و OOP_AST_SOURCES و class_manager نُقلت إلى sad_shared
#      للقضاء على ازدواج البناء (Phase: dedup sad_shared/sad_core).
# (EN) OOP_TYPES_SOURCES, OOP_AST_SOURCES, and class_manager moved to sad_shared
#      to eliminate build duplication (Phase: dedup sad_shared/sad_core).
# ──────────────────────────────────────────────────────────────────────
set(OOP_TYPES_SOURCES)

set(OOP_MANAGERS_SOURCES
    interpreter/src/managers/object_manager.cpp
)

set(OOP_AST_SOURCES)

# معطل - تم نقله إلى parser_core_oop.cpp
set(OOP_PARSER_SOURCES)
set(OOP_INTERPRETER_SOURCES)

# ──────────────────────────────────────────────────────────────────────
# 8. الدوال المضمنة / Built-in Functions
# (AR) Phase 5 (F-05): builtins.cpp نُقل إلى shared/builtins/runtime/
# (EN) Phase 5 (F-05): builtins.cpp moved to shared/builtins/runtime/
# ──────────────────────────────────────────────────────────────────────
set(BUILTINS_SOURCES
    shared/builtins/runtime/builtins.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 9. إدارة الأخطاء / Error Management
# ──────────────────────────────────────────────────────────────────────
set(ERROR_SOURCES
    shared/errors/src/error_codes.cpp
    shared/errors/src/diagnostic.cpp
    shared/errors/src/error_manager.cpp
    shared/errors/src/smart_errors.cpp
    shared/errors/src/smart_suggestions.cpp
    shared/errors/src/smart_type_explanations.cpp
    shared/errors/src/smart_multi_error.cpp
    shared/errors/src/smart_fix_suggestions.cpp
    shared/errors/src/smart_teacher_mode.cpp
    shared/errors/src/smart_error_recovery.cpp
    shared/errors/src/smart_cascade_prevention.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 10. المكتبة القياسية / Standard Library
# (AR) Phase 5 (F-05): الجزء الأساسي نُقل إلى shared/builtins/runtime/
# (EN) Phase 5 (F-05): core runtime moved to shared/builtins/runtime/
# ──────────────────────────────────────────────────────────────────────
set(STDLIB_SOURCES
    shared/builtins/runtime/stdlib_manager.cpp
    shared/builtins/runtime/type_functions.cpp
    shared/builtins/runtime/array_functions.cpp
    shared/builtins/runtime/other_functions.cpp
    stdlib/string/string_functions.cpp
    stdlib/math/math_functions.cpp
    stdlib/math/advanced_math.cpp
    stdlib/io/io_functions.cpp
    stdlib/filesystem/filesystem_module.cpp
    stdlib/database/database_module.cpp
    stdlib/json/json_module.cpp
    stdlib/xml/xml_module.cpp
    stdlib/crypto/crypto_module.cpp
    # stdlib/crypto/crypto_builtins.cpp  # (AR) معطّل — يستخدم نظام include مختلف عن sad_core
    stdlib/system/system_functions.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 11. المحسن / Optimizer
# ──────────────────────────────────────────────────────────────────────
set(OPTIMIZER_SOURCES
    compiler/src/optimizer/advanced_optimizer.cpp
    compiler/src/optimizer/optimization_pass.cpp
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
# 15. دعم وضع Freestanding / Freestanding Mode Support
# ──────────────────────────────────────────────────────────────────────
# (AR) دعم شامل لتوليد كود وضع بلا_مكتبة_قياسية (bare-metal)
# (EN) Comprehensive support for no_std / freestanding code generation
# ──────────────────────────────────────────────────────────────────────
# (AR) أُرشف FREESTANDING_SOURCES إلى archived/compiler_pipeline_legacy/
#      السبب: ENABLE_FREESTANDING لم يُعرَّف أبداً عبر option() فكان الفرع ميتاً،
#      ومصادر compiler/src/pipeline/* لها 0 مستدعون خارجيون.
# (EN) FREESTANDING_SOURCES archived: ENABLE_FREESTANDING was never defined
#      and compiler/src/pipeline/* had 0 external callers.
set(FREESTANDING_SOURCES "")

# ──────────────────────────────────────────────────────────────────────
# 14. نظام الوحدات / Module System
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
#       sad_core يربط sad_shared كـ PUBLIC في cmake/libraries.cmake.
# (EN) The following groups were removed because sad_shared already builds them:
#       UTILS_SOURCES, LEXER_SOURCES, PARSER_SOURCES, AST_SOURCES,
#       OOP_TYPES_SOURCES, OOP_AST_SOURCES, ERROR_SOURCES, MODULES_SOURCES,
#       plus shared/types files from DATA_SOURCES and class_manager.
#       sad_core links sad_shared PUBLIC in cmake/libraries.cmake.
set(ALL_SOURCES
    ${INTERPRETER_SOURCES}
    ${DATA_SOURCES}
    ${OOP_MANAGERS_SOURCES}
    ${OOP_PARSER_SOURCES}
    ${OOP_INTERPRETER_SOURCES}
    ${STDLIB_SOURCES}
    ${BUILTINS_SOURCES}
    ${OPTIMIZER_SOURCES}
    ${LOW_LEVEL_SOURCES}
    ${COMPILER_FRONTEND_SOURCES}
    ${HOT_RELOAD_SOURCES}
)


