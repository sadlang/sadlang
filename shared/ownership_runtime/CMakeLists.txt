# ═══════════════════════════════════════════════════════════════════════════════
# (AR) shared/ownership_runtime/ — مكتبة متتبع الملكية في وقت التشغيل.
#
# (AR) تحوي:
#   - Sad::Ownership::Runtime::OwnershipTracker  (singleton آمن للخيوط)
#   - طبقة C-ABI (sad_ownership_*)              (لاستهلاك من runtime sadc)
#
# (AR) مُستخرَجة من compiler/{include/ffi/ffi_wrapper.h, src/backend/ffi_wrapper.cpp}
#      كجزء من DEF-001 لفك قفل تفعيل --prod على المخرج التنفيذي للمترجم.
#
# (EN) Runtime ownership tracker library — extracted from compiler/ffi/ to
#      enable --prod activation in compiled sadc output via a clean C-ABI.
#
# @phase Memory Layer Unification — Phase B / DEF-001
# ═══════════════════════════════════════════════════════════════════════════════

set(SAD_OWNERSHIP_RUNTIME_SOURCES
    src/ownership_tracker.cpp
    src/sad_ownership_capi.cpp
)

add_library(sad_ownership_runtime STATIC ${SAD_OWNERSHIP_RUNTIME_SOURCES})

target_include_directories(sad_ownership_runtime PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(sad_ownership_runtime PRIVATE /utf-8 /FS /wd4819)
endif()

target_compile_features(sad_ownership_runtime PUBLIC cxx_std_17)

# (AR) لا اعتماديات داخلية — STL فقط (mutex, unordered_map, optional, vector).
# (EN) No internal dependencies — STL only.
