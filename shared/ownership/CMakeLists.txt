# ============================================================================
# (AR) مكتبة الملكية المشتركة - sad_ownership
# (EN) Shared Ownership Library - sad_ownership
#
# (AR) هذه المكتبة تحتوي على نظام فحص الملكية والاستعارة الموحَّد لكل المسارات:
#      - sad-run (المفسّر)         — يستهلك عبر wrapper في interpreter
#      - sad-build (المترجم)       — يستهلك مباشرة
#      - sad.wasm                  — يستهلك مباشرة (مستقبلاً)
#      - sad-check (أداة مستقلة)   — يستهلك مباشرة (مستقبلاً)
#
# (EN) This library contains the unified ownership/borrow checking system
#      for all execution paths.
#
# (AR) السبب: قواعد الملكية يجب أن تعطي نفس النتائج في كل مسار،
#      ويجب أن تكون قابلة للفحص دون تشغيل الكود (static analysis tool).
# (EN) Rationale: ownership rules must give consistent results across all
#      paths, and must be checkable without executing code.
#
# @phase Architecture Refactor: Ownership Unification
# ============================================================================

cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# (AR) جمع كل ملفات .cpp في src/ تلقائياً
# (EN) Auto-collect all .cpp files in src/
# (AR) CONFIGURE_DEPENDS: يُعيد CMake حسابَ الـglob عند كلِّ بناء. وبدونه
#      يُقيَّم مرّةً عند التهيئة، فحذفُ ملفٍّ يُبقيه في مشروع MSBuild
#      المُخبَّأ ويُخفِق البناء بـC1083 على ملفٍّ لم يعد موجودًا.
# (EN) Without CONFIGURE_DEPENDS the glob is evaluated once at configure
#      time, so deleting a source leaves it in the cached project and the
#      build fails with C1083 on a file that no longer exists.
file(GLOB SAD_OWNERSHIP_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB SAD_OWNERSHIP_HEADERS CONFIGURE_DEPENDS "include/ownership/*.h")

add_library(sad_ownership STATIC ${SAD_OWNERSHIP_SOURCES} ${SAD_OWNERSHIP_HEADERS})

target_include_directories(sad_ownership
    PUBLIC
        # (AR) المسار العام للرؤوس - يتيح #include "ownership/borrow_checker.h"
        # (EN) Public header path - enables #include "ownership/borrow_checker.h"
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        # (AR) رؤوس المشروع المشتركة المطلوبة (ast, types, errors, ...)
        # (EN) Required shared project headers
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        # (AR) الرؤوس المولَّدة من language-truth/ (sadTypeKindIsCopy)
        # (EN) Headers generated from language-truth/ (sadTypeKindIsCopy)
        ${CMAKE_SOURCE_DIR}/shared/types/generated
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/shared/utils/include
        ${CMAKE_SOURCE_DIR}/include
)

# (AR) ربط بالمكتبات المشتركة المطلوبة
# (EN) Link to required shared libraries
target_link_libraries(sad_ownership
    PUBLIC
        sad_shared
        sad_type_system
        sad_semantic_shared  # (AR) لاستخدام type_checker إن لزم
)

# (AR) إصلاح تحذيرات UTF-8 على MSVC
# (EN) Fix UTF-8 warnings on MSVC
if(MSVC)
    target_compile_options(sad_ownership PRIVATE /utf-8 /wd4819 /FS)
endif()

# (AR) Alias قديم للحفاظ على التوافق الخلفي خلال فترة الانتقال
# (EN) Legacy alias to keep backward compatibility during transition
add_library(sad_semantic ALIAS sad_ownership)

message(STATUS "sad_ownership: built from shared/ownership/ (unified ownership system)")
