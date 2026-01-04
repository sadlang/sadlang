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
    
    # ملفات Type Inference (Phase 1.3.2) / Type Inference files
    src/substitution.cpp            # خريطة الاستبدال / Substitution map
    src/type_variable.cpp           # متغيرات الأنواع / Type variables
    src/constraint.cpp              # القيود / Constraints
    src/unification.cpp             # محرك التوحيد / Unification engine
    src/constraint_solver.cpp       # حلال القيود / Constraint solver
    src/type_inferencer.cpp         # محرك الاستنتاج / Inference engine
    
    # ملفات Generics (Phase 1.3.4) / Generics files
    src/generic_type.cpp            # الأنواع العامة / Generic types
    src/type_parameter.cpp          # معاملات الأنواع / Type parameters
    src/type_constraint.cpp         # قيود الأنواع / Type constraints
    src/generic_instantiation.cpp   # Monomorphization / Instantiation
    
    # ملفات Union Types (Phase 1.3.5) / Union Types files
    src/union_type.cpp              # الأنواع الاتحادية / Union types
    src/optional_type.cpp           # الأنواع الاختيارية / Optional types
    src/type_guard.cpp              # حراس الأنواع / Type guards
    src/type_narrowing.cpp          # تضييق الأنواع / Type narrowing
)

# الملفات الرأسية / Header files
set(TYPE_SYSTEM_HEADERS
    # الملفات الرأسية / Header files
    include/type.h                  # النوع الأساسي / Base type
    include/primitive_type.h        # الأنواع البدائية / Primitive types
    include/type_registry.h         # سجل الأنواع / Type registry
    
    # ملفات Type Inference (Phase 1.3.2) / Type Inference files
    include/substitution.h          # خريطة الاستبدال / Substitution map
    include/type_variable.h         # متغيرات الأنواع / Type variables
    include/constraint.h            # القيود / Constraints
    include/unification.h           # التوحيد / Unification
    include/constraint_solver.h     # حلال القيود / Constraint solver
    include/type_inferencer.h       # محرك الاستنتاج / Inference engine
    
    # ملفات Generics (Phase 1.3.4) / Generics files
    include/generic_type.h          # الأنواع العامة / Generic types
    include/type_parameter.h        # معاملات الأنواع / Type parameters
    include/type_constraint.h       # قيود الأنواع / Type constraints
    include/generic_instantiation.h # Monomorphization / Instantiation
    
    # ملفات Union Types (Phase 1.3.5) / Union Types files
    include/union_type.h            # الأنواع الاتحادية / Union types
    include/optional_type.h         # الأنواع الاختيارية / Optional types
    include/type_guard.h            # حراس الأنواع / Type guards
    include/type_narrowing.h        # تضييق الأنواع / Type narrowing
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
# الاختبارات / Tests
# ════════════════════════════════════════════════════════════════════════════════

# اختبارات Type Narrowing مع AST / Type Narrowing AST Tests
add_executable(type_narrowing_ast_tests
    tests/test_type_narrowing_ast.cpp
    ${CMAKE_SOURCE_DIR}/src/parser/ast/expressions.cpp
    ${CMAKE_SOURCE_DIR}/src/lexer/token.cpp
)

# ربط المكتبات المطلوبة / Link required libraries
target_link_libraries(type_narrowing_ast_tests 
    PRIVATE 
        sad_type_system
)

# إضافة مسارات Include للاختبارات / Add include paths for tests
target_include_directories(type_narrowing_ast_tests 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include
)

# إضافة الاختبار إلى CTest / Add test to CTest
add_test(NAME TypeNarrowingASTIntegration COMMAND type_narrowing_ast_tests)

# ════════════════════════════════════════════════════════════════════════════════
# رسالة الحالة / Status Message
# ════════════════════════════════════════════════════════════════════════════════

message(STATUS "✅ مكتبة نظام الأنواع مُكونة / Type System library configured")
message(STATUS "   المصادر / Sources: ${CMAKE_CURRENT_SOURCE_DIR}/src")
message(STATUS "   الرؤوس / Headers: ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "   الاختبارات / Tests: tests/test_type_narrowing_ast.cpp")
