# ============================================================================
# core_components.cmake - تضمين المكوّنات الرئيسيّة
# Include core sub-components (shared/, interpreter/, compiler/)
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

# المفسّر / Interpreter
if(EXISTS "${CMAKE_SOURCE_DIR}/interpreter/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/interpreter ${CMAKE_BINARY_DIR}/interpreter)
endif()

# المترجم / Compiler
if(EXISTS "${CMAKE_SOURCE_DIR}/compiler/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/compiler ${CMAKE_BINARY_DIR}/compiler)
endif()

# نظام واجهات المستخدم / UI System
if(EXISTS "${CMAKE_SOURCE_DIR}/features/graphics/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/features/graphics ${CMAKE_BINARY_DIR}/features/graphics)
endif()

# مصحّح الأداء / Performance Profiler (يجب أن يُبنى قبل sad_interp — الاسم القديم sad_core)
if(EXISTS "${CMAKE_SOURCE_DIR}/tools/profiler/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/tools/profiler ${CMAKE_BINARY_DIR}/profiler)
endif()

# (AR) الرسوميّات وبيئة التشغيل والأدوات تُضاف عبر cmake/libraries.cmake
# (EN) Graphics, runtime and tools are added via cmake/libraries.cmake
# (AR) ملاحظة: لا تُضِف graphics أو runtime أو tools هنا تفاديًا لأخطاء تكرار المجلّدات الفرعيّة.
# (EN) NOTE: Do NOT add graphics, runtime, tools here to avoid duplicate subdirectory errors.
