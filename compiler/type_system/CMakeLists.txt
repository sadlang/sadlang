# ════════════════════════════════════════════════════════════════════════════════
# ملف: CMakeLists.txt
# File: CMakeLists.txt
#
# الوصف: تكوين CMake لمكتبة نظام الأنواع
# Description: CMake configuration for Type System library
#
# المؤلف: Sad Language Type System
# Author: Sad Language Type System
#
# التاريخ: 2 يناير 2026
# Date: January 2, 2026
# ════════════════════════════════════════════════════════════════════════════════

# الحد الأدنى لإصدار CMake / Minimum CMake version
cmake_minimum_required(VERSION 3.15)

# ════════════════════════════════════════════════════════════════════════════════
# إعدادات C++ / C++ Settings
# ════════════════════════════════════════════════════════════════════════════════

# تعيين معيار C++ / Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ════════════════════════════════════════════════════════════════════════════════
# مصادر نظام الأنواع / Type System Sources
# ════════════════════════════════════════════════════════════════════════════════

# الملفات المصدرية / Source files
set(TYPE_SYSTEM_SOURCES
    # الملفات الأساسية / Core files
    src/type.cpp                    # النوع الأساسي / Base type
    src/primitive_type.cpp          # الأنواع البدائية / Primitive types
    src/type_registry.cpp           # سجل الأنواع / Type registry
)

# الملفات الرأسية / Header files
set(TYPE_SYSTEM_HEADERS
    # الملفات الرأسية / Header files
    include/type.h                  # النوع الأساسي / Base type
    include/primitive_type.h        # الأنواع البدائية / Primitive types
    include/type_registry.h         # سجل الأنواع / Type registry
)

# ════════════════════════════════════════════════════════════════════════════════
# إنشاء المكتبة / Library Creation
# ════════════════════════════════════════════════════════════════════════════════

# إنشاء مكتبة static / Create static library
add_library(sad_type_system STATIC 
    ${TYPE_SYSTEM_SOURCES}
    ${TYPE_SYSTEM_HEADERS}
)

# ════════════════════════════════════════════════════════════════════════════════
# مسارات Include / Include Paths
# ════════════════════════════════════════════════════════════════════════════════

# إضافة مسار include العام / Add public include path
target_include_directories(sad_type_system 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ════════════════════════════════════════════════════════════════════════════════
# خيارات الترجمة / Compilation Options
# ════════════════════════════════════════════════════════════════════════════════

# خيارات المترجم / Compiler options
if(MSVC)
    # خيارات Visual Studio / Visual Studio options
    target_compile_options(sad_type_system PRIVATE
        /W4              # مستوى التحذيرات 4 / Warning level 4
        /WX-             # عدم معاملة التحذيرات كأخطاء / Don't treat warnings as errors
        /utf-8           # دعم UTF-8 / UTF-8 support
    )
else()
    # خيارات GCC/Clang / GCC/Clang options
    target_compile_options(sad_type_system PRIVATE
        -Wall            # جميع التحذيرات / All warnings
        -Wextra          # تحذيرات إضافية / Extra warnings
        -Wpedantic       # تحذيرات صارمة / Pedantic warnings
    )
endif()

# ════════════════════════════════════════════════════════════════════════════════
# رسالة الحالة / Status Message
# ════════════════════════════════════════════════════════════════════════════════

message(STATUS "✅ مكتبة نظام الأنواع مُكونة / Type System library configured")
message(STATUS "   المصادر / Sources: ${CMAKE_CURRENT_SOURCE_DIR}/src")
message(STATUS "   الرؤوس / Headers: ${CMAKE_CURRENT_SOURCE_DIR}/include")
