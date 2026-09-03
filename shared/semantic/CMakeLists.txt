# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: shared/semantic/CMakeLists.txt
# الوصف: مكتبة فاحص الأنواع المشتركة (Type Checker shared library)
#
# (AR) فاحص الأنواع كان في compiler/ لكنه طبقة دلالية مشتركة:
#      - يستخدمها المفسر (interpreter_core.cpp) للفحص قبل التنفيذ
#      - يستخدمها المترجم (sadc) كجزء من pipeline الترجمة
#      هذا النقل يصلح انتهاك F-01 (المفسر يضم رأس من المترجم).
#
# (EN) Type checker was under compiler/ but is a shared semantic layer:
#      - Used by interpreter (interpreter_core.cpp) for pre-execution checking
#      - Used by compiler (sadc) as part of the compilation pipeline
#      This move fixes F-01 violation (interpreter including from compiler).
# ═══════════════════════════════════════════════════════════════════════════════

# (AR) جمع كل ملفات type_checker / (EN) Collect type_checker sources
set(SEMANTIC_SHARED_SOURCES
    src/semantic/type_checker.cpp
    src/semantic/type_checker_expr_visitors.cpp
    src/semantic/type_checker_stmt_visitors.cpp
)

add_library(sad_semantic_shared STATIC ${SEMANTIC_SHARED_SOURCES})

if(MSVC)
    target_compile_options(sad_semantic_shared PRIVATE /utf-8 /wd4819 /FS)
endif()

target_include_directories(sad_semantic_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/semantic
        ${CMAKE_CURRENT_SOURCE_DIR}/generated
        ${CMAKE_SOURCE_DIR}/shared/types/include
        # (AR) الرأسُ المولَّدُ للأنواعِ لا يصلُ بالوراثةِ من `sad_type_system`،
        #      وهو مُضمَّنٌ صراحةً في كلِّ هدفٍ يحتاجُه (compiler · runtime · graphics).
        # (EN) The generated types header does not arrive by propagation; every
        #      target that needs it lists it explicitly.
        ${CMAKE_SOURCE_DIR}/shared/types/generated
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(sad_semantic_shared PUBLIC cxx_std_17)

# (AR) يعتمد على نظام الأنواع المشترك / (EN) Depends on shared type system
# (AR) sad_null_safety: قاعدة SEM045 «الفراغ اليقيني» المشتركة (اتجاه الربط آمن —
#      محلّل null لا يربط الطبقة الدلالية). / (EN) shared certainly-void rule; safe direction.
target_link_libraries(sad_semantic_shared PUBLIC sad_shared sad_type_system sad_null_safety)

message(STATUS "✓ مكتبة الفحص الدلالي المشتركة / Shared semantic: sad_semantic_shared")
