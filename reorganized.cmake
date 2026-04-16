# ============================================================================
# reorganized.cmake - تضمين المكونات المعاد تنظيمها
# Include reorganized sub-components (shared/, interpreter_new/, compiler_new/)
# ============================================================================
# (AR) هذا الملف يضيف المجلدات الفرعية التي تحتوي على CMakeLists.txt
#      للمكونات الرئيسية المعاد تنظيمها.
# (EN) This file adds subdirectories that contain CMakeLists.txt
#      for the main reorganized components.
# ============================================================================

# المكونات المشتركة / Shared Components
if(EXISTS "${CMAKE_SOURCE_DIR}/shared/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/shared ${CMAKE_BINARY_DIR}/shared)
endif()

# المفسر / Interpreter
if(EXISTS "${CMAKE_SOURCE_DIR}/interpreter_new/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/interpreter_new ${CMAKE_BINARY_DIR}/interpreter_new)
endif()

# المترجم / Compiler
if(EXISTS "${CMAKE_SOURCE_DIR}/compiler_new/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/compiler_new ${CMAKE_BINARY_DIR}/compiler_new)
endif()

# نظام واجهات المستخدم / UI System
if(EXISTS "${CMAKE_SOURCE_DIR}/sad_ui/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/sad_ui ${CMAKE_BINARY_DIR}/sad_ui)
endif()

# مصحح الأداء / Performance Profiler (يجب أن يُبنى قبل sad_core)
if(EXISTS "${CMAKE_SOURCE_DIR}/tools/profiler/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/tools/profiler ${CMAKE_BINARY_DIR}/profiler)
endif()

# (AR) الرسوميات، الآلة الافتراضية، بيئة التشغيل والأدوات تُضاف عبر cmake/libraries.cmake
# (EN) Graphics, VM, runtime and tools are added via cmake/libraries.cmake
# NOTE: Do NOT add graphics, vm, runtime_new, tools here to avoid duplicate subdirectory errors.
