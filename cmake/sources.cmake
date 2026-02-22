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
    shared/parser/src/parser_core.cpp
    shared/parser/src/parser_core_main.cpp
    shared/parser/src/parser_declarations.cpp
    shared/parser/src/parser_statements.cpp
    shared/parser/src/parser_expressions.cpp
    shared/parser/src/parser_advanced.cpp
    shared/parser/src/parser_core_helpers.cpp
    shared/parser/src/parser_core_oop.cpp
    shared/parser/src/parser_modules.cpp
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
    interpreter_new/src/core/interpreter_core.cpp
    interpreter_new/src/builtins/builtin_registry.cpp
    interpreter_new/src/builtins/builtin_registry_part1.cpp
    interpreter_new/src/builtins/builtin_registry_part2.cpp
    interpreter_new/src/builtins/builtin_registry_part3.cpp
    interpreter_new/src/builtins/builtin_registry_part4.cpp
    interpreter_new/src/builtins/builtin_registry_part5.cpp
    interpreter_new/src/builtins/builtin_registry_part6.cpp
    interpreter_new/src/builtins/builtin_registry_part7.cpp
    interpreter_new/src/builtins/builtin_registry_part8.cpp
    interpreter_new/src/builtins/builtin_registry_part9.cpp
    interpreter_new/src/builtins/builtin_registry_part10.cpp
    interpreter_new/src/builtins/builtin_registry_part11.cpp
    interpreter_new/src/builtins/builtin_registry_part12.cpp
    interpreter_new/src/builtins/builtin_registry_part14.cpp
    interpreter_new/src/builtins/builtin_registry_part15.cpp
    interpreter_new/src/builtins/builtin_registry_part13.cpp
    interpreter_new/src/builtins/builtin_registry_part16.cpp
    interpreter_new/src/builtins/builtin_registry_part17.cpp
    interpreter_new/src/builtins/builtin_registry_part18.cpp
    interpreter_new/src/visitors/expression_evaluator_core.cpp
    interpreter_new/src/visitors/expression_evaluator_calls.cpp
    interpreter_new/src/visitors/expression_evaluator_oop.cpp
    interpreter_new/src/visitors/expression_evaluator_members.cpp
    interpreter_new/src/visitors/statement_executor.cpp
    interpreter_new/src/visitors/statement_executor_oop.cpp
    interpreter_new/src/visitors/statement_executor_modules.cpp
    interpreter_new/src/exception.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 6. إدارة البيانات والأنواع / Data & Type Management
# ──────────────────────────────────────────────────────────────────────
set(DATA_SOURCES
    shared/types/src/value.cpp
    shared/types/src/generator.cpp
    interpreter_new/src/managers/variable_manager.cpp
    interpreter_new/src/managers/function_manager.cpp
    interpreter_new/src/managers/scope_manager.cpp
    interpreter_new/src/managers/ownership_manager.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 7. البرمجة الكائنية / OOP (Object-Oriented Programming)
# ──────────────────────────────────────────────────────────────────────
set(OOP_TYPES_SOURCES
    shared/types/src/class_type.cpp
    shared/types/src/object_instance.cpp
)

set(OOP_MANAGERS_SOURCES
    interpreter_new/src/managers/class_manager.cpp
    interpreter_new/src/managers/object_manager.cpp
)

set(OOP_AST_SOURCES
    shared/ast/src/class_nodes.cpp
)

# معطل - تم نقله إلى parser_core_oop.cpp
set(OOP_PARSER_SOURCES)
set(OOP_INTERPRETER_SOURCES)

# ──────────────────────────────────────────────────────────────────────
# 8. الدوال المضمنة / Built-in Functions
# ──────────────────────────────────────────────────────────────────────
set(BUILTINS_SOURCES
    stdlib/core/builtins.cpp
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
# ──────────────────────────────────────────────────────────────────────
set(STDLIB_SOURCES
    stdlib/core/stdlib_manager.cpp
    stdlib/core/type_functions.cpp
    stdlib/core/array_functions.cpp
    stdlib/core/other_functions.cpp
    stdlib/string/string_functions.cpp
    stdlib/math/math_functions.cpp
    stdlib/math/advanced_math.cpp
    stdlib/io/io_functions.cpp
    stdlib/filesystem/filesystem_module.cpp
    stdlib/http/http_module.cpp
    stdlib/database/database_module.cpp
    stdlib/json/json_module.cpp
    stdlib/xml/xml_module.cpp
    stdlib/crypto/crypto_module.cpp
    stdlib/graphics/graphics_module.cpp
    stdlib/graphics/sad_ui_fonts_arabic.cpp
    stdlib/graphics/sad_ui_layout_render.cpp
    stdlib/graphics/sad_ui_api.cpp
    stdlib/graphics/sad_animation.cpp
    stdlib/graphics/sad_state.cpp
    stdlib/graphics/sad_gesture.cpp
    stdlib/graphics/sad_navigator.cpp
    stdlib/graphics/sad_canvas.cpp
    stdlib/graphics/sad_responsive.cpp
    stdlib/graphics/sad_particles.cpp
    stdlib/graphics/sad_effects.cpp
    stdlib/graphics/sad_physics.cpp
    stdlib/graphics/sad_scene3d.cpp
    stdlib/graphics/sad_ai.cpp
    stdlib/graphics/sad_camera2d.cpp
    stdlib/graphics/sad_charts.cpp
    stdlib/graphics/sad_audio.cpp
    stdlib/graphics/sad_tilemap.cpp
    stdlib/graphics/sad_islamic_art.cpp
    stdlib/graphics/sad_dhikr.cpp
    stdlib/graphics/sad_qibla.cpp
    stdlib/graphics/sad_prayer.cpp
    stdlib/graphics/sad_hijri.cpp
    stdlib/graphics/sad_android.cpp
    stdlib/system/system_functions.cpp
    stdlib/mobile/mobile_module.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 11. المحسن / Optimizer
# ──────────────────────────────────────────────────────────────────────
set(OPTIMIZER_SOURCES
    compiler_new/src/optimizer/advanced_optimizer.cpp
    compiler_new/src/optimizer/optimization_pass.cpp
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
)

# ──────────────────────────────────────────────────────────────────────
# 13. الواجهة الأمامية للمترجم / Compiler Frontend (SIR)
# ──────────────────────────────────────────────────────────────────────
set(COMPILER_FRONTEND_SOURCES
    compiler_new/src/frontend/sir_types.cpp
    compiler_new/src/frontend/sir_module.cpp
    compiler_new/src/frontend/sir_instruction.cpp
    compiler_new/src/frontend/sir_builder_module.cpp
    compiler_new/src/frontend/sir_builder_classes.cpp
    compiler_new/src/frontend/sir_builder_statements.cpp
    compiler_new/src/frontend/sir_builder_control_flow.cpp
    compiler_new/src/frontend/sir_builder_helpers.cpp
    compiler_new/src/frontend/sir_builder_operators.cpp
    compiler_new/src/frontend/sir_builder_calls.cpp
    compiler_new/src/frontend/sir_builder_builtins_core.cpp
    compiler_new/src/frontend/sir_builder_builtins_system.cpp
    compiler_new/src/frontend/sir_builder_templates.cpp
)

# ──────────────────────────────────────────────────────────────────────
# 15. دعم وضع Freestanding / Freestanding Mode Support
# ──────────────────────────────────────────────────────────────────────
# (AR) دعم شامل لتوليد كود وضع بلا_مكتبة_قياسية (bare-metal)
# (EN) Comprehensive support for no_std / freestanding code generation
# ──────────────────────────────────────────────────────────────────────
set(FREESTANDING_SOURCES
    # ─── runtime: بيئة التشغيل الحرة ───────────────────────────────
    runtime_new/freestanding/sad_core.cpp       # دوال الذاكرة الأساسية
    runtime_new/freestanding/sad_panic.cpp      # معالج الذعر
    runtime_new/freestanding/sad_allocator.cpp  # واجهة المُخصّص
    runtime_new/freestanding/sad_bump_allocator.cpp   # مُخصّص bump
    runtime_new/freestanding/sad_port_io.cpp    # منافذ I/O للـ bare-metal

    # ─── مولّد الكود للمترجم ────────────────────────────────────────
    compiler_new/src/pipeline/freestanding_codegen.cpp  # مولّد كود freestanding
    compiler_new/src/pipeline/no_std_mode.cpp           # وضع بلا_مكتبة_قياسية
    compiler_new/src/pipeline/no_std_integration.cpp    # تكامل مع خط الترجمة
)

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
# تجميع جميع المصادر / Aggregate all sources
# ──────────────────────────────────────────────────────────────────────
set(ALL_SOURCES
    ${UTILS_SOURCES}
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
    ${AST_SOURCES}
    ${INTERPRETER_SOURCES}
    ${DATA_SOURCES}
    ${OOP_TYPES_SOURCES}
    ${OOP_MANAGERS_SOURCES}
    ${OOP_AST_SOURCES}
    ${OOP_PARSER_SOURCES}
    ${OOP_INTERPRETER_SOURCES}
    ${ERROR_SOURCES}
    ${STDLIB_SOURCES}
    ${BUILTINS_SOURCES}
    ${OPTIMIZER_SOURCES}
    ${LOW_LEVEL_SOURCES}
    ${COMPILER_FRONTEND_SOURCES}
    ${MODULES_SOURCES}
)


