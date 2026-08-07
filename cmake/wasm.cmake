# cmake/wasm.cmake
# (AR) إعدادات بناء WebAssembly باستخدام Emscripten
# (EN) WebAssembly build configuration using Emscripten
#
# الاستخدام / Usage:
#   emcmake cmake -S . -B build_wasm -DBUILD_WASM=ON -DCMAKE_BUILD_TYPE=Release
#   cmake --build build_wasm --target sad_wasm
#
# (AR) الرسوماتُ تُطفَأ تلقائيًّا على Emscripten (انظر CMakeLists.txt الجذر): خلفيّةُ
#      الويب لا تُترجَم بعد. فلا حاجةَ إلى `-DSAD_ENABLE_GRAPHICS=OFF` في السطر أعلاه.
# (EN) Graphics is auto-disabled under Emscripten (see the root CMakeLists.txt): the web
#      canvas backend does not compile yet, so no -DSAD_ENABLE_GRAPHICS=OFF is needed.
#
# المتطلبات / Requirements:
#   - Emscripten SDK 3.1+  (https://emscripten.org/docs/getting_started/downloads.html)
#   - emsdk_env.ps1 (Windows) or emsdk_env.sh (Linux/macOS) must be activated

if(NOT EMSCRIPTEN)
    message(STATUS "[wasm] Emscripten not detected — skipping sad_wasm target")
    return()
endif()

# (AR) تعيين مسار المصدر / (EN) Set source directory
if(NOT DEFINED SAD_SOURCE_DIR)
    set(SAD_SOURCE_DIR ${CMAKE_SOURCE_DIR})
endif()

message(STATUS "[wasm] Building sad_wasm with Emscripten ${EMSCRIPTEN_VERSION}")

# ── نقطة الدخول وحدها، والمفسّر يُربَط لا يُنسَخ ─────────────────────────────
#
# كانت هنا قائمةٌ بعشرة ملفّاتِ مصدرٍ **مكتوبةٍ بأسمائها**، تُعاد كتابةُ نصف
# المفسّر بها. وذلك يكسر بطريقتين، وقعتا كلتاهما:
#
# ١. **الانحراف الصامت**: تتحرّك شجرةُ المصدر ولا تتحرّك القائمة. ثلاثةٌ من
#    العشرة لم تعد موجودةً أصلًا (`statement_executor.cpp` و
#    `expression_evaluator.cpp` و`builtin_functions.cpp`)، فصار الهدفُ لا
#    **يُهيَّأ** — لا يُخفق في البناء بل في التهيئة، فلا يراه إلّا من طلبه.
#
# ٢. **مفسّرٌ آخر**: `sad-run` يُبنى من `${ALL_SOURCES}` كاملةً عبر `sad_core`،
#    وهذا كان يُبنى من عشرة ملفّات. فالصفحةُ تعرض خرجَ الأوّل وتشغّل الثاني،
#    وهما ليسا نسختين متباعدتين من شيءٍ واحد — بل شيئان مختلفان. وبوّابةُ
#    التكافؤ في الأكاديمية أحصت ٥٦ اختلافًا، كلُّها تقول «التنفيذ فشل» في
#    تحدّيات الكائنيّة: لا لأنّ المفسّر يحسبها خطأً، بل لأنّه لا يعرفها.
#
# فالمصدرُ هنا نقطةُ الدخول وحدَها، والمفسّر **يُربَط** من الهدف نفسِه الذي
# يربطه `sad-run`. وبهذا يصير الانحرافُ مستحيلًا بنيويًّا لا مُتعقَّبًا بيقظة.
add_executable(sad_wasm ${SAD_SOURCE_DIR}/tools/wasm/sad_wasm.cpp)

target_link_libraries(sad_wasm PRIVATE sad_core)

target_include_directories(sad_wasm PRIVATE ${SAD_SOURCE_DIR})

# ── خيارات Emscripten ────────────────────────────────────────────────────────
# (AR) `--no-entry` مع `EXPORTED_FUNCTIONS` يقتطع كلَّ ما ليس في القائمة. وكانت
#      `sad_render_ui` مُعرَّفةً في `tools/wasm/sad_wasm.cpp` وغائبةً عن القائمة —
#      أي دالّةً تُترجَم وتُصان ثمّ تُحذَف عند الربط، فلا يبلغها نداءٌ أبدًا. ومثالُ
#      البناء في رأس ذلك الملفّ يعلنها، فالوثيقةُ تَعِد بما لا يخرج.
# (EN) --no-entry + EXPORTED_FUNCTIONS strips anything unlisted; sad_render_ui was
#      defined, maintained, and then dropped at link time.
set(SAD_WASM_EXPORTED_FUNCTIONS
    "_sad_execute"
    "_sad_render_ui"
    "_sad_version"
    "_malloc"
    "_free"
)

set(SAD_WASM_EXPORTED_RUNTIME
    "cwrap"
    "ccall"
    "UTF8ToString"
    "stringToUTF8"
    "lengthBytesUTF8"
)

# تحويل القوائم إلى صيغة JSON
string(REPLACE ";" "," _fns  "${SAD_WASM_EXPORTED_FUNCTIONS}")
string(REPLACE ";" "," _rt   "${SAD_WASM_EXPORTED_RUNTIME}")

set_target_properties(sad_wasm PROPERTIES
    SUFFIX ".js"
    OUTPUT_NAME "sad"
)

target_link_options(sad_wasm PRIVATE
    -O2
    -sWASM=1
    -sALLOW_MEMORY_GROWTH=1
    -sMAXIMUM_MEMORY=256MB
    -sMODULARIZE=1
    -sEXPORT_NAME=SadWasm
    "-sEXPORTED_FUNCTIONS=[${_fns}]"
    "-sEXPORTED_RUNTIME_METHODS=[${_rt}]"
    # (AR) نظامُ الملفّات مُشغَّلٌ لا لأنّنا نريده، بل لأنّ المفسّر صار يُربَط
    #      كاملًا: مدمجاتُ المقابس تشير إلى `$SOCKFS`/`$FS` فلا يلتئم الرابط
    #      بدونها. والبديلُ نزعُ تلك المدمجات — أي مفسّرٌ آخرُ من جديد، وهو
    #      العيبُ الذي أُصلح هنا. ونظامُ الملفّات في المتصفّح **في الذاكرة
    #      وحدها**: لا يقرأ قرصَ الزائر ولا يكتب فيه. وأيُّ درسٍ يعتمد ملفًّا
    #      يظهر انحرافُه في بوّابة التكافؤ فيُحجَب زرُّه — لا يمرّ صامتًا.
    # (EN) FILESYSTEM=1 because the whole interpreter is linked now: the socket
    #      builtins reference $SOCKFS/$FS. The alternative — stripping them — is
    #      a different interpreter again, the very defect fixed here. The FS is
    #      in-memory only; it never touches the visitor's disk.
    -sFILESYSTEM=1
    -sDYNAMIC_EXECUTION=0
    --no-entry
)

# ── نسخ الناتج إلى مجلد الموقع، إن كان قائمًا ─────────────────────────────
# (AR) كان `make_directory` **يُنشئ** `website/docs/public` ثمّ ينسخ إليه. وقد
#      حُذف `website/` من المستودع، فصار البناءُ يبعث شجرةً ميّتةً في كلّ مرّة
#      ويملؤها بملفّاتٍ غيرِ متتبَّعة. فالنسخُ الآن **مشروطٌ بوجود** المجلّد:
#      من يملكه يأخذ نسختَه، ومن لا يملكه لا يُخلَق له.
# (EN) make_directory used to resurrect website/docs/public — a tree deleted from
#      the repo — and fill it with untracked build output on every build.
set(WASM_OUTPUT_DIR "${SAD_SOURCE_DIR}/website/docs/public")
if(EXISTS "${WASM_OUTPUT_DIR}")
    add_custom_command(TARGET sad_wasm POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:sad_wasm>"           "${WASM_OUTPUT_DIR}/sad.js"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:sad_wasm>/sad.wasm" "${WASM_OUTPUT_DIR}/sad.wasm"
        COMMENT "[wasm] Copied sad.js + sad.wasm → website/docs/public/"
        VERBATIM
    )
endif()

# (AR) `message()` لا يُقيّم تعابيرَ المولِّد — `$<TARGET_FILE_DIR:…>` كان يُطبَع
#      حرفيًّا. والمسارُ معلومٌ وقتَ التهيئة، فيُقال كما هو.
# (EN) message() does not evaluate generator expressions; use the configure-time path.
message(STATUS "[wasm] Target: sad_wasm → ${CMAKE_BINARY_DIR}/bin/sad.js + sad.wasm")
