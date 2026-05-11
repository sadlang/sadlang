# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/yaml_cpp.cmake
# (AR) جلب وبناء yaml-cpp v0.8.0 عبر FetchContent.
#      تبعية اختيارية لـ sadinfo فقط — لا تُربط بـ sad_shared أو غيرها.
#      مُعطّلة افتراضياً عبر ENABLE_YAML_CPP لتقليل زمن البناء الأول.
# (EN) Fetches and builds yaml-cpp v0.8.0 via FetchContent.
#      Optional dependency for sadinfo only — never linked to sad_shared.
#      Disabled by default via ENABLE_YAML_CPP to keep first-build fast.
# ═══════════════════════════════════════════════════════════════════════════════

include_guard(GLOBAL)

option(ENABLE_YAML_CPP
    "تمكين yaml-cpp لـ sadinfo / Enable yaml-cpp for sadinfo YAML output"
    ON)

if(NOT ENABLE_YAML_CPP)
    message(STATUS "⊘ yaml-cpp معطّلة — sadinfo سيستخدم كاتب YAML اليدوي")
    message(STATUS "  yaml-cpp disabled — sadinfo will use built-in YAML writer")
    return()
endif()

include(FetchContent)

# (AR) خيارات yaml-cpp قبل التضمين — نقلل المخرجات لما نحتاجه فقط
# (EN) yaml-cpp options before inclusion — minimize to what we need
set(YAML_CPP_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS    OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_CONTRIB  OFF CACHE BOOL "" FORCE)
set(YAML_CPP_INSTALL        OFF CACHE BOOL "" FORCE)
set(YAML_CPP_FORMAT_SOURCE  OFF CACHE BOOL "" FORCE)
set(YAML_BUILD_SHARED_LIBS  OFF CACHE BOOL "" FORCE)

# (AR) CMake 4.x أزال التواؤم مع سياسات < 3.5. yaml-cpp v0.8.0 يستخدم
#      cmake_minimum_required قديم — نُجبر minimum policy version لتجنب الفشل.
# (EN) CMake 4.x dropped compatibility with policies < 3.5. yaml-cpp v0.8.0
#      uses an old cmake_minimum_required — force a minimum policy version.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        0.8.0          # (AR) تاج ثابت — لا تستخدم master
    GIT_SHALLOW    TRUE
    GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(yaml-cpp)

# (AR) في MSVC، يصدر yaml-cpp تحذيرات كثيرة — أسكتها على مستوى الهدف فقط
# (EN) On MSVC, yaml-cpp emits many warnings — silence at target level only
if(MSVC AND TARGET yaml-cpp)
    target_compile_options(yaml-cpp PRIVATE /wd4251 /wd4267 /wd4275 /wd4244)
endif()

# (AR) علم مُعرَّف ليُستخدم في الكود الشرطي
# (EN) Define flag for conditional code
set(SAD_HAS_YAML_CPP ON CACHE INTERNAL "yaml-cpp is available")

message(STATUS "✓ yaml-cpp v0.8.0 مفعّل / yaml-cpp v0.8.0 enabled")
