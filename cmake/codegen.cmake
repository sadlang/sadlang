# ============================================================================
# (AR) cmake/codegen.cmake — توليد كود C++ للمعجم من YAML وقت البناء
# (EN) cmake/codegen.cmake — Build-time C++ codegen for keyword lexicon
# ----------------------------------------------------------------------------
# (AR) يضيف custom command يستدعي scripts/codegen/gen_keywords.py
#      كلما تغيرت data/language/keywords.yaml، يُعاد توليد:
#        - shared/lexer/generated/keywords_generated.h
#        - shared/lexer/generated/keywords_generated.cpp
# (EN) Adds custom command calling scripts/codegen/gen_keywords.py.
#      Whenever data/language/keywords.yaml changes, regenerates:
#        - shared/lexer/generated/keywords_generated.h
#        - shared/lexer/generated/keywords_generated.cpp
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
set(SAD_KW_YAML       "${CMAKE_SOURCE_DIR}/data/language/keywords.yaml"          CACHE INTERNAL "")
set(SAD_KW_SCHEMA     "${CMAKE_SOURCE_DIR}/data/language/keywords.schema.json"   CACHE INTERNAL "")
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

# ─── (AR) custom command للتوليد عند تغير YAML ───
add_custom_command(
    OUTPUT  ${SAD_KW_GEN_H} ${SAD_KW_GEN_CPP}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SAD_KW_GEN_DIR}
    COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
            ${Python3_EXECUTABLE} ${SAD_KW_GEN_SCRIPT}
                --yaml   ${SAD_KW_YAML}
                --schema ${SAD_KW_SCHEMA}
                --header ${SAD_KW_GEN_H}
                --source ${SAD_KW_GEN_CPP}
                --quiet
    DEPENDS ${SAD_KW_YAML} ${SAD_KW_SCHEMA} ${SAD_KW_GEN_SCRIPT}
    COMMENT "(sad) Generating C++ from data/language/keywords.yaml..."
    VERBATIM
)

# ─── (AR) هدف يُعتمد عليه من sad_lexer_lib لاحقاً ───
add_custom_target(sad_keywords_codegen
    DEPENDS ${SAD_KW_GEN_H} ${SAD_KW_GEN_CPP}
)
add_dependencies(sad_keywords_codegen sad_check_codegen_env)

message(STATUS "(sad) Keyword codegen configured: YAML=${SAD_KW_YAML}")
