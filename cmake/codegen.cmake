# ============================================================================
# (AR) cmake/codegen.cmake — توليد كود C++ للمعجم من YAML وقت البناء
# (EN) cmake/codegen.cmake — Build-time C++ codegen for keyword lexicon
# ----------------------------------------------------------------------------
# (AR) يضيف custom command يستدعي scripts/codegen/gen_keywords.py
#      كلما تغيرت language-truth/keywords.yaml، يُعاد توليد:
#        - shared/lexer/generated/keywords_generated.h
#        - shared/lexer/generated/keywords_generated.cpp
#      المصدر الأصلي data/language/keywords.yaml محفوظ كنسخة احتياطية حتى M2-004.
# (EN) Adds custom command calling scripts/codegen/gen_keywords.py.
#      Whenever language-truth/keywords.yaml changes, regenerates:
#        - shared/lexer/generated/keywords_generated.h
#        - shared/lexer/generated/keywords_generated.cpp
#      Original data/language/keywords.yaml kept as fallback until M2-004.
# ============================================================================

# ─── (AR) اشتراط Python 3.9+ ─── (Winston: شرط واضح + رسالة)
find_package(Python3 3.9 COMPONENTS Interpreter)
if(NOT Python3_FOUND)
    message(FATAL_ERROR
        "\n"
        "==============================================================\n"
        "  Sad Language: Python 3.9+ is REQUIRED for keyword codegen.\n"
        "  Install:  https://www.python.org/downloads/\n"
        "  Then:     pip install pyyaml jsonschema\n"
        "==============================================================\n"
    )
endif()

# ─── (AR) المسارات الكاملة ───
# (AR) V5: المصدر الجديد language-truth/ — المصدر القديم محفوظ كـ SAD_KW_YAML_LEGACY حتى M2-004
set(SAD_KW_YAML       "${CMAKE_SOURCE_DIR}/language-truth/keywords.yaml"         CACHE INTERNAL "")
set(SAD_KW_SCHEMA     "${CMAKE_SOURCE_DIR}/language-truth/_schemas/keywords.schema.json" CACHE INTERNAL "")
set(SAD_KW_YAML_LEGACY "${CMAKE_SOURCE_DIR}/data/language/keywords.yaml"         CACHE INTERNAL "")
set(SAD_KW_GEN_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/codegen/gen_keywords.py"      CACHE INTERNAL "")
set(SAD_KW_GEN_DIR    "${CMAKE_SOURCE_DIR}/shared/lexer/generated"               CACHE INTERNAL "")
set(SAD_KW_GEN_H      "${SAD_KW_GEN_DIR}/keywords_generated.h"                   CACHE INTERNAL "")
set(SAD_KW_GEN_CPP    "${SAD_KW_GEN_DIR}/keywords_generated.cpp"                 CACHE INTERNAL "")

# ─── (AR) فحص بيئة Python (Murat: bootstrap منفصل) ───
add_custom_target(sad_check_codegen_env
    COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
            ${Python3_EXECUTABLE} -c "import yaml, jsonschema; print('[codegen] env OK')"
    COMMENT "(sad) Verifying Python codegen environment (pyyaml, jsonschema)..."
    VERBATIM
)

# ─── (AR) custom command للتوليد عند تغير YAML (نمط stamp لـ idempotency) ───
#     السبب: gen_keywords يستخدم write_if_changed فلا يُحدّث mtime عند عدم التغيير،
#     فيُعيد CMake التوليد كل بناء. الـ stamp يُلمَس بعد كل توليد ناجح فيمنع ذلك.
add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/sad_keywords_codegen.stamp
    BYPRODUCTS ${SAD_KW_GEN_H} ${SAD_KW_GEN_CPP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SAD_KW_GEN_DIR}
    COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
            ${Python3_EXECUTABLE} ${SAD_KW_GEN_SCRIPT}
                --yaml   ${SAD_KW_YAML}
                --schema ${SAD_KW_SCHEMA}
                --header ${SAD_KW_GEN_H}
                --source ${SAD_KW_GEN_CPP}
                --quiet
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/sad_keywords_codegen.stamp
    DEPENDS ${SAD_KW_YAML} ${SAD_KW_SCHEMA} ${SAD_KW_GEN_SCRIPT}
    COMMENT "(sad) Generating C++ from language-truth/keywords.yaml (V5)..."
    VERBATIM
)

# ─── (AR) هدف يُعتمد عليه من sad_lexer_lib لاحقاً ───
add_custom_target(sad_keywords_codegen
    DEPENDS ${CMAKE_BINARY_DIR}/sad_keywords_codegen.stamp
)
add_dependencies(sad_keywords_codegen sad_check_codegen_env)

message(STATUS "(sad) Keyword codegen configured: YAML=${SAD_KW_YAML}")

# ============================================================================
# (AR) Types Codegen — توليد SadTypeKind من types.yaml (S-TS-P0.5)
# (EN) Types Codegen — generate SadTypeKind enum from types.yaml
# ============================================================================

set(SAD_TY_YAML       "${CMAKE_SOURCE_DIR}/language-truth/types.yaml"                        CACHE INTERNAL "")
set(SAD_TY_SCHEMA     "${CMAKE_SOURCE_DIR}/language-truth/_schemas/type.schema.json"         CACHE INTERNAL "")
set(SAD_TY_GEN_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/codegen/gen_types.py"                     CACHE INTERNAL "")
set(SAD_TY_GEN_DIR    "${CMAKE_SOURCE_DIR}/shared/types/generated"                           CACHE INTERNAL "")
set(SAD_TY_GEN_H      "${SAD_TY_GEN_DIR}/sad_type_kind_generated.h"                          CACHE INTERNAL "")

add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/sad_types_codegen.stamp
    BYPRODUCTS ${SAD_TY_GEN_H}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SAD_TY_GEN_DIR}
    COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
            ${Python3_EXECUTABLE} ${SAD_TY_GEN_SCRIPT}
                --yaml   ${SAD_TY_YAML}
                --schema ${SAD_TY_SCHEMA}
                --header ${SAD_TY_GEN_H}
                --quiet
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/sad_types_codegen.stamp
    DEPENDS ${SAD_TY_YAML} ${SAD_TY_SCHEMA} ${SAD_TY_GEN_SCRIPT}
    COMMENT "(sad) Generating SadTypeKind from language-truth/types.yaml (V5)..."
    VERBATIM
)

add_custom_target(sad_types_codegen
    DEPENDS ${CMAKE_BINARY_DIR}/sad_types_codegen.stamp
)
add_dependencies(sad_types_codegen sad_check_codegen_env)

message(STATUS "(sad) Types codegen configured: YAML=${SAD_TY_YAML}")

# ============================================================================
# (AR) Builtin Registry Codegen — توليد builtin_registry_generated.h من YAML
# (EN) Builtin Registry Codegen — generate builtin_registry_generated.h from YAML
# ============================================================================

# ─── (AR) المسارات ───
set(SAD_BR_YAML_DIR   "${CMAKE_SOURCE_DIR}/language-truth/builtins"                          CACHE INTERNAL "")
set(SAD_BR_INDEX      "${SAD_BR_YAML_DIR}/_index.yaml"                                       CACHE INTERNAL "")
set(SAD_BR_GEN_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/codegen/gen_builtins_registry.py"         CACHE INTERNAL "")
set(SAD_BR_GEN_DIR    "${CMAKE_SOURCE_DIR}/shared/builtins/generated"                        CACHE INTERNAL "")
set(SAD_BR_GEN_H      "${SAD_BR_GEN_DIR}/builtin_registry_generated.h"                       CACHE INTERNAL "")

# ─── (AR) custom command للتوليد عند تغير أي YAML في builtins/ ───
file(GLOB SAD_BR_YAML_FILES "${SAD_BR_YAML_DIR}/*.yaml")

add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/sad_builtin_registry_codegen.stamp
    BYPRODUCTS ${SAD_BR_GEN_H}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SAD_BR_GEN_DIR}
    COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
            ${Python3_EXECUTABLE} ${SAD_BR_GEN_SCRIPT}
                --yaml-dir ${SAD_BR_YAML_DIR}
                --index    ${SAD_BR_INDEX}
                --out-h    ${SAD_BR_GEN_H}
                --quiet
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/sad_builtin_registry_codegen.stamp
    DEPENDS ${SAD_BR_YAML_FILES} ${SAD_BR_GEN_SCRIPT}
    COMMENT "(sad) Generating builtin_registry_generated.h from language-truth/builtins/*.yaml..."
    VERBATIM
)

# ─── (AR) هدف يُعتمد عليه من sad_shared_lib (stamp لـ idempotency) ───
add_custom_target(sad_builtin_registry_codegen
    DEPENDS ${CMAKE_BINARY_DIR}/sad_builtin_registry_codegen.stamp
)
add_dependencies(sad_builtin_registry_codegen sad_check_codegen_env)

message(STATUS "(sad) Builtin registry codegen configured: DIR=${SAD_BR_YAML_DIR}")

# ============================================================================
# (AR) Error Messages Codegen (V5) — توليد كتالوج الأخطاء من language-truth/errors/
# (EN) Error Messages Codegen (V5) — generate error catalog from language-truth/errors/
# (AR) القرار: ADR-DOCS-V4-005 (المصدر الموحَّد language-truth/) + EM-V5-1.
#      يُولّد من 7 ملفات فئات (الصيغة المتداخلة المُعتمَدة) بدل data/language/ القديم.
# ============================================================================
set(SAD_EM_YAML_DIR    "${CMAKE_SOURCE_DIR}/language-truth/errors"                          CACHE INTERNAL "")
set(SAD_EM_SCHEMA      "${CMAKE_SOURCE_DIR}/language-truth/_schemas/error.schema.json"      CACHE INTERNAL "")
set(SAD_EM_ENUM_HEADER "${CMAKE_SOURCE_DIR}/shared/errors/include/error_codes.h"            CACHE INTERNAL "")
set(SAD_EM_GEN_SCRIPT  "${CMAKE_SOURCE_DIR}/scripts/codegen/gen_error_messages.py"          CACHE INTERNAL "")
set(SAD_EM_GEN_DIR     "${CMAKE_SOURCE_DIR}/shared/errors/generated"                        CACHE INTERNAL "")
set(SAD_EM_GEN_H       "${SAD_EM_GEN_DIR}/error_messages_generated.h"                        CACHE INTERNAL "")
set(SAD_EM_GEN_CPP     "${SAD_EM_GEN_DIR}/error_messages_generated.cpp"                      CACHE INTERNAL "")

file(GLOB SAD_EM_YAML_FILES "${SAD_EM_YAML_DIR}/*.yaml")

add_custom_command(
    OUTPUT  ${CMAKE_BINARY_DIR}/sad_error_messages_codegen.stamp
    BYPRODUCTS ${SAD_EM_GEN_H} ${SAD_EM_GEN_CPP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SAD_EM_GEN_DIR}
    COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
            ${Python3_EXECUTABLE} ${SAD_EM_GEN_SCRIPT}
                --yaml-dir    ${SAD_EM_YAML_DIR}
                --schema      ${SAD_EM_SCHEMA}
                --enum-header ${SAD_EM_ENUM_HEADER}
                --header      ${SAD_EM_GEN_H}
                --source      ${SAD_EM_GEN_CPP}
                --quiet
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/sad_error_messages_codegen.stamp
    DEPENDS ${SAD_EM_YAML_FILES} ${SAD_EM_SCHEMA} ${SAD_EM_ENUM_HEADER} ${SAD_EM_GEN_SCRIPT}
    COMMENT "(sad) Generating error_messages_generated.{h,cpp} from language-truth/errors/*.yaml (V5)..."
    VERBATIM
)

# ─── (AR) stamp لـ idempotency (write_if_changed لا يُحدّث mtime) ───
add_custom_target(sad_error_messages_codegen
    DEPENDS ${CMAKE_BINARY_DIR}/sad_error_messages_codegen.stamp
)
add_dependencies(sad_error_messages_codegen sad_check_codegen_env)

message(STATUS "(sad) Error messages codegen configured (V5): DIR=${SAD_EM_YAML_DIR}")

# ============================================================================
# (AR) sadinfo Errors Projection (V5) — إسقاط كتالوج sadinfo من language-truth/errors/
# (EN) sadinfo Errors Projection — generate sadinfo entity catalog from language-truth.
# (AR) القرار: ADR-DOCS-V4-005 (ق-فرعي-2) — sadinfo إسقاط مُولَّد لا مصدر يدوي + EM-V5-2.
# ============================================================================
set(SAD_SI_GEN_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/codegen/gen_sadinfo_errors.py"           CACHE INTERNAL "")
set(SAD_SI_OUT_SCHEMA "${CMAKE_SOURCE_DIR}/data/_schemas/error.schema.json"                  CACHE INTERNAL "")
set(SAD_SI_OUT_DIR    "${CMAKE_SOURCE_DIR}/data/errors"                                       CACHE INTERNAL "")
set(SAD_SI_STAMP      "${CMAKE_BINARY_DIR}/sad_sadinfo_errors.stamp"                          CACHE INTERNAL "")

add_custom_command(
    OUTPUT  ${SAD_SI_STAMP}
    COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
            ${Python3_EXECUTABLE} ${SAD_SI_GEN_SCRIPT}
                --yaml-dir    ${SAD_EM_YAML_DIR}
                --src-schema  ${SAD_EM_SCHEMA}
                --out-schema  ${SAD_SI_OUT_SCHEMA}
                --out-dir     ${SAD_SI_OUT_DIR}
                --quiet
    COMMAND ${CMAKE_COMMAND} -E touch ${SAD_SI_STAMP}
    DEPENDS ${SAD_EM_YAML_FILES} ${SAD_EM_SCHEMA} ${SAD_SI_OUT_SCHEMA} ${SAD_SI_GEN_SCRIPT}
    COMMENT "(sad) Projecting sadinfo error catalog from language-truth/errors/*.yaml (V5)..."
    VERBATIM
)

add_custom_target(sad_sadinfo_errors_codegen
    DEPENDS ${SAD_SI_STAMP}
)
add_dependencies(sad_sadinfo_errors_codegen sad_check_codegen_env)

message(STATUS "(sad) sadinfo errors projection configured (V5): OUT=${SAD_SI_OUT_DIR}")

# ============================================================================
# (AR) ربط كل نطاقات language-truth المتبقّية في CMake (V5) — EM-V5-4 / ق-فرعي-3.
#      القرار ADR-DOCS-V4-005: توليد آلي وقت البناء لكل نطاق (لا gen_all.py يدوي).
#      دالة موحَّدة تتجنّب تكرار الكتل (CW-19)؛ الوسائط مطابقة لـ gen_all.py المثبت.
# (EN) Wire all remaining language-truth domains into CMake build-time codegen.
# ----------------------------------------------------------------------------
# sad_add_codegen(<name> OUTPUTS <files...> DEPS <yaml+schema...> ARGS <python args...>)
# ============================================================================
set(SAD_LT   "${CMAKE_SOURCE_DIR}/language-truth"          CACHE INTERNAL "")
set(SAD_SCH  "${SAD_LT}/_schemas"                          CACHE INTERNAL "")
set(SAD_GEN  "${CMAKE_SOURCE_DIR}/shared/builtins/generated" CACHE INTERNAL "")
set(SAD_CG   "${CMAKE_SOURCE_DIR}/scripts/codegen"         CACHE INTERNAL "")

function(sad_add_codegen NAME)
    cmake_parse_arguments(GC "" "" "OUTPUTS;DEPS;ARGS" ${ARGN})
    # (AR) نمط stamp لتحقيق idempotency: المولّدات تستخدم write_if_changed فلا تُحدّث mtime
    #      المخرَج عند عدم التغيير؛ لذا نَلمس stamp بعد كل توليد ناجح كي لا يُعاد البناء بلا داعٍ.
    set(_stamp "${CMAKE_BINARY_DIR}/sad_${NAME}_codegen.stamp")
    add_custom_command(
        OUTPUT  ${_stamp}
        BYPRODUCTS ${GC_OUTPUTS}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${SAD_GEN}
        COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
                ${Python3_EXECUTABLE} ${GC_ARGS}
        COMMAND ${CMAKE_COMMAND} -E touch ${_stamp}
        DEPENDS ${GC_DEPS}
        COMMENT "(sad) Generating ${NAME} from language-truth/ (V5)..."
        VERBATIM
    )
    add_custom_target(sad_${NAME}_codegen DEPENDS ${_stamp})
    add_dependencies(sad_${NAME}_codegen sad_check_codegen_env)
endfunction()

# ════════════════════════════════════════════════════════════════════════════
# (AR) ⚠️ نطاقات codegen مُعطَّلة مؤقتاً — سكربتاتها مفقودة (فقدان بيانات سابق،
#      غير متعقّبة في git) ومخرجاتها صفر استهلاك في الشجرة الفعلية (سقالة V5 سابقة
#      لأوانها). تعطيلها يرفع حاجز البناء بلا خسارة وظيفية.
#      الاستعادة الصحيحة: عند بدء استهلاك C++ لهذه الترويسات (راجع
#      _recovered/full_to_restore/) — تُكتب السكربتات وتُتعقّب في git أولاً.
#      النطاقات: operators, directives, types, grammar, modules, stdlib,
#               type_methods, patterns, oop_constructs, expr_constructs.
# (EN) ⚠️ Temporarily disabled codegen domains — scripts lost (prior data loss,
#      untracked) and outputs have ZERO consumers (premature V5 scaffolding).
#      Disabling unblocks the build with no functional loss. Restore properly
#      when C++ starts consuming these headers (see _recovered/full_to_restore/).
# ════════════════════════════════════════════════════════════════════════════
# sad_add_codegen(operators ...)       # مُعطَّل: gen_operators.py مفقود + غير مُستهلَك
# sad_add_codegen(directives ...)      # مُعطَّل: gen_directives.py مفقود + غير مُستهلَك
# sad_add_codegen(types ...)           # ✅ مُفعَّل: انظر كتلة Types Codegen أعلاه (gen_types.py — S-TS-P0.5)
# sad_add_codegen(grammar ...)         # مُعطَّل: gen_grammar.py مفقود + غير مُستهلَك
# sad_add_codegen(modules ...)         # مُعطَّل: gen_modules.py مفقود + غير مُستهلَك
# sad_add_codegen(stdlib ...)          # مُعطَّل: gen_stdlib.py مفقود + غير مُستهلَك
# sad_add_codegen(type_methods ...)    # مُعطَّل: gen_type_methods.py مفقود + غير مُستهلَك
# sad_add_codegen(patterns ...)        # مُعطَّل: gen_constructs.py مفقود + غير مُستهلَك
# sad_add_codegen(oop_constructs ...)  # مُعطَّل: gen_constructs.py مفقود + غير مُستهلَك
# sad_add_codegen(expr_constructs ...) # مُعطَّل: gen_constructs.py مفقود + غير مُستهلَك

# ─── (AR) هدف تجميعي — يقتصر على النطاقات العاملة (سكربت موجود + مخرَج مُستهلَك) ───
# (EN) Aggregate — only the working domains (script present + output consumed).
add_custom_target(sad_all_codegen DEPENDS
    sad_types_codegen sad_keywords_codegen sad_builtin_registry_codegen sad_error_messages_codegen
)
# (AR) sad_sadinfo_errors_codegen مُستبعَد من مُجمِّع بناء C++: مُخرَجه (data/errors) إسقاط
#      تشغيلي للأدوات/الموقع، صفر استهلاك في كود C++، ومخططه (data/_schemas/error.schema.json)
#      مفقود حالياً. الهدف يبقى معرّفاً ويُشغَّل يدوياً عند الحاجة (بعد استعادة المخطط).
# (EN) sad_sadinfo_errors_codegen excluded from the C++ build aggregate: its output
#      (data/errors) is a tooling/site projection with zero C++ consumers and a currently
#      missing schema. The target stays defined; run it manually when needed.
message(STATUS "(sad) Codegen wired: 4 C++ build domains (types/keywords/builtins/error_messages); sadinfo + 9 V5 scaffolds excluded")
