# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/doc_gen_dual_tests.cmake
# الوصف: اختبارات استخراج التوثيق المزدوج — مقارنة sad ↔ sadc
# ═══════════════════════════════════════════════════════════════════════════════
#
# (AR) يُعرّف أهدافاً وcmake-tests لتشغيل runner.py الخاص بـ doc_gen_dual_execution.
#      يتطلب Python 3 + sad.exe + sadc.exe (والأخير مطلوب لاختبارات PDF).
#
#      الأهداف:
#        doc_gen_tests_fast — كل الصيغ ما عدا PDF (~2 ثانية)
#        doc_gen_tests_full — كل الصيغ بما فيها PDF (~1 دقيقة، يحتاج Edge/Chrome)
#
# (EN) Defines targets and ctest entries for doc_gen_dual_execution/runner.py.
# ═══════════════════════════════════════════════════════════════════════════════

find_package(Python3 COMPONENTS Interpreter QUIET)

if(NOT Python3_FOUND)
    message(STATUS "⚠ Python3 غير متاح — اختبارات doc_gen_dual_execution معطّلة")
    return()
endif()

set(DOC_GEN_RUNNER "${CMAKE_SOURCE_DIR}/tests/system/docs/doc_gen/runner.py")

if(NOT EXISTS "${DOC_GEN_RUNNER}")
    message(STATUS "⚠ doc_gen_dual_execution/runner.py غير موجود")
    return()
endif()

message(STATUS "✓ اختبارات استخراج التوثيق المزدوج / Doc gen dual tests enabled")

# ──────────────────────────────────────────────────────────────────────
# (AR) المعاملات المشتركة
# ──────────────────────────────────────────────────────────────────────
set(_DOC_GEN_ENV
    "${CMAKE_COMMAND}" -E env
    "PYTHONIOENCODING=utf-8"
    "PYTHONUTF8=1"
    "${Python3_EXECUTABLE}" "${DOC_GEN_RUNNER}"
)

# ──────────────────────────────────────────────────────────────────────
# الأهداف اليدوية: cmake --build build --target doc_gen_tests_fast
# ──────────────────────────────────────────────────────────────────────
add_custom_target(doc_gen_tests_fast
    COMMAND ${_DOC_GEN_ENV} --skip-pdf
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "📄 اختبارات استخراج التوثيق (md/json/html فقط)"
    DEPENDS sad-run
    USES_TERMINAL
)

add_custom_target(doc_gen_tests_full
    COMMAND ${_DOC_GEN_ENV}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "📄 اختبارات استخراج التوثيق الشاملة (يشمل PDF)"
    DEPENDS sad-run
    USES_TERMINAL
)

# (AR) إذا كان sadc هدفاً، نضيف اعتمادية
if(TARGET sad-build)
    add_dependencies(doc_gen_tests_fast sad-build)
    add_dependencies(doc_gen_tests_full sad-build)
endif()

# ──────────────────────────────────────────────────────────────────────
# تسجيل CTest
# ──────────────────────────────────────────────────────────────────────
if(BUILD_TESTS)
    # (AR) الاختبار السريع — يدخل ضمن CI الافتراضي
    add_test(
        NAME DocGenDual_Fast
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DOC_GEN_RUNNER}" --skip-pdf
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DocGenDual_Fast PROPERTIES
        LABELS "doc_gen;fast"
        TIMEOUT 120
        # (AR) Fast وFull يتشاركان مجلّد _tmp بأسماء ملفّات متطابقة؛ القفل يمنع
        #      تزامنهما تحت ctest -j (وإلّا سباق حذف/قراءة ⇒ FileNotFoundError).
        # (EN) Fast & Full share _tmp with identical filenames; lock prevents
        #      concurrent runs under ctest -j (else a delete/read race).
        RESOURCE_LOCK "doc_gen_tmp"
        # (AR) RUN_SERIAL: RESOURCE_LOCK يمنع فقط تزامن Fast/Full مع بعضهما،
        #      لا تزامنهما مع أي اختبار آخر يضغط على I/O/CPU؛ شوهد عملياً
        #      DocGenDual_Full يفشل بـFileNotFoundError عرضياً تحت ctest -j4
        #      (محرك PDF خارجي — Edge/Chrome headless — حساس لضغط النظام عند
        #      flush الكتابة النهائية)، بينما نجح بثقة 100% عند العزل الكامل.
        # (EN) RUN_SERIAL: RESOURCE_LOCK only prevents Fast/Full overlap, not
        #      contention with unrelated I/O/CPU-heavy tests; DocGenDual_Full
        #      was observed failing with FileNotFoundError occasionally under
        #      ctest -j4 (external PDF engine — Edge/Chrome headless — is
        #      sensitive to system load during final write flush), while
        #      passing reliably when run in isolation.
        RUN_SERIAL TRUE
    )

    # (AR) الاختبار الكامل (PDF) — وسم Nightly
    add_test(
        NAME DocGenDual_Full
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DOC_GEN_RUNNER}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DocGenDual_Full PROPERTIES
        LABELS "doc_gen;full;nightly;pdf"
        TIMEOUT 600
        RESOURCE_LOCK "doc_gen_tmp"
        RUN_SERIAL TRUE
    )
endif()
