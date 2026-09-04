# ============================================================================
# core_components.cmake - تضمين المكوّنات الرئيسيّة
# Include core sub-components (shared/, compiler/)
# ============================================================================
# (AR) هذا الملفّ يضيف المجلّدات الفرعيّة التي تحتوي على CMakeLists.txt
#      للمكوّنات الرئيسيّة (كان اسمه reorganized.cmake في الجذر).
# (EN) This file adds subdirectories that contain CMakeLists.txt
#      for the core components (formerly reorganized.cmake at the root).
# ============================================================================

# المكوّنات المشتركة / Shared Components
if(EXISTS "${CMAKE_SOURCE_DIR}/shared/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/shared ${CMAKE_BINARY_DIR}/shared)
endif()

# ════════════════════════════════════════════════════════════════════════════
# (AR) 🔑 **رُفِعَ ضمُّ `interpreter/`** — لا لأنّه ضرَّ، بل لأنّه صارَ
#      شرطًا **لا يصدُقُ أبدًا** ولا يقولُ ذلك:
#
#          if(EXISTS "${CMAKE_SOURCE_DIR}/interpreter/CMakeLists.txt")
#
#      المجلَّدُ زالَ مع المفسّرِ في `f17fb5cd0`، فصارَ الشرطُ كاذبًا في كلِّ
#      تهيئةٍ بلا سطرِ حالةٍ واحد. وهو عينُ الصنفِ الذي كُوِيَ به هذا المستودعُ
#      مرارًا: `if(TARGET sad-run)` في `cmake/tests.cmake` أسقطَ بوّابةً كاملةً
#      **صامتةً** — لا اختبارَ يُسجَّلُ ولا شيءَ يحمرّ. ⚠️ والفرقُ أنّ ذاك
#      كان يحرسُ شيئًا وهذا كان يضمُّه؛ والعلاجُ واحدٌ: **يُرفَعُ الشرطُ لا
#      يُترَكُ يكذب**، لأنّ شرطًا باقيًا يُقرأُ «قد يُوجَدُ يومًا» وهو قرارٌ مضى.
# (EN) The interpreter/ inclusion is removed — not because it did harm, but
#      because its condition could no longer be true and said so to no one. The
#      directory went with the interpreter in f17fb5cd0, so the EXISTS test was
#      false in every configure without a single status line. This is the very
#      class that has burned this repo repeatedly (if(TARGET sad-run) silently
#      dropping a whole gate). A condition left standing reads as "it might
#      exist one day", and that is a decision already made.
# ════════════════════════════════════════════════════════════════════════════

# المترجم / Compiler
if(EXISTS "${CMAKE_SOURCE_DIR}/compiler/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/compiler ${CMAKE_BINARY_DIR}/compiler)
endif()

# نظام واجهات المستخدم / UI System
if(SAD_ENABLE_GRAPHICS AND EXISTS "${CMAKE_SOURCE_DIR}/features/graphics/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/features/graphics ${CMAKE_BINARY_DIR}/features/graphics)
endif()

# مصحّح الأداء / Performance Profiler (يجب أن يُبنى قبل sad_interp — الاسم القديم sad_core)
# (AR) الرسوميّات وبيئة التشغيل والأدوات تُضاف عبر cmake/libraries.cmake
# (EN) Graphics, runtime and tools are added via cmake/libraries.cmake
# (AR) ملاحظة: لا تُضِف graphics أو runtime أو tools هنا تفاديًا لأخطاء تكرار المجلّدات الفرعيّة.
# (EN) NOTE: Do NOT add graphics, runtime, tools here to avoid duplicate subdirectory errors.
