# ============================================================================
# المجلدات المعاد تنظيمها / Reorganized Folders
# ============================================================================
# هذا الملف يضيف المجلدات الجديدة المنظمة إلى نظام البناء
# This file adds the new organized folders to the build system
# ============================================================================

message(STATUS "")
message(STATUS "======================================")
message(STATUS "تحميل المجلدات المعاد تنظيمها...")
message(STATUS "Loading reorganized folders...")
message(STATUS "======================================")

# ============================================================================
# المجلدات الجديدة معطلة مؤقتاً - تحتاج إصلاح مسارات include
# New folders temporarily disabled - need include path fixes
# ============================================================================

# shared/ - المكونات المشتركة
if(EXISTS "${CMAKE_SOURCE_DIR}/shared/CMakeLists.txt")
    add_subdirectory(shared)
    message(STATUS "✅ shared/ - المكونات المشتركة")
endif()

# interpreter_new/ - المفسر
if(EXISTS "${CMAKE_SOURCE_DIR}/interpreter_new/CMakeLists.txt")
    add_subdirectory(interpreter_new)
    message(STATUS "✅ interpreter_new/ - المفسر")
endif()

# compiler_new/ - المترجم
if(EXISTS "${CMAKE_SOURCE_DIR}/compiler_new/CMakeLists.txt")
    add_subdirectory(compiler_new)
    message(STATUS "✅ compiler_new/ - المترجم")
endif()

# runtime_new/ - وقت التشغيل (معطل - يستخدم API قديم غير متوافق)
# runtime_new/ - Runtime (disabled - uses incompatible old API)
#if(EXISTS "${CMAKE_SOURCE_DIR}/runtime_new/CMakeLists.txt")
#    add_subdirectory(runtime_new)
#    message(STATUS "✅ runtime_new/ - وقت التشغيل")
#endif()
message(STATUS "⏸️ runtime_new/ - معطل مؤقتاً (يحتاج إعادة كتابة)")
message(STATUS "⏸️ runtime_new/ - Disabled (needs API rewrite)")

message(STATUS "======================================")
message(STATUS "")
