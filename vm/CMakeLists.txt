# ============================================================================
# CMakeLists.txt - الآلة الافتراضية (Bytecode VM)
# ============================================================================
# الوصف:
#   ملف بناء CMake لمكتبة الآلة الافتراضية لغة ص.
#   تُترجم شجرة AST إلى بايت كود ثم تنفّذه.
#
# المكوّنات:
#   1. sad_vm_opcodes.cpp    - تعريف رموز العمليات وأسمائها
#   2. sad_vm_compiler.cpp   - المُترجم: AST → بايت كود
#   3. sad_vm_executor.cpp   - المُنفّذ: حلقة التنفيذ الرئيسية
#
# الاعتمادات:
#   - sad_shared: المكونات المشتركة (AST, Parser, Lexer)
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# ملفات المصدر
# ============================================================================

set(VM_SOURCES
    src/sad_vm_opcodes.cpp
    src/sad_vm_compiler.cpp
    src/sad_vm_executor.cpp
)

set(VM_HEADERS
    include/sad_vm_opcodes.h
    include/sad_vm_value.h
    include/sad_vm_chunk.h
    include/sad_vm_compiler.h
    include/sad_vm_executor.h
    include/sad_vm_debug.h
)

# ============================================================================
# بناء المكتبة الثابتة
# ============================================================================

add_library(sad_vm STATIC ${VM_SOURCES} ${VM_HEADERS})

# ============================================================================
# مسارات الترويسات
# ============================================================================

target_include_directories(sad_vm
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# الاعتمادات
# ============================================================================

# المكونات المشتركة (AST, Parser, Lexer, Types)
target_link_libraries(sad_vm PUBLIC sad_shared)

# ============================================================================
# خيارات المُترجم
# ============================================================================

target_compile_features(sad_vm PUBLIC cxx_std_17)

# إصلاح مشكلة PDB على MSVC
if(MSVC)
    target_compile_options(sad_vm PRIVATE /FS)
    # تفعيل UTF-8 لدعم الأسماء العربية
    target_compile_options(sad_vm PRIVATE /utf-8)
endif()

# لمترجم GCC/Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(sad_vm PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()

# ============================================================================
# رسالة تأكيد
# ============================================================================

message(STATUS "✓ تم إعداد مكتبة الآلة الافتراضية (sad_vm) / Configured VM library")
