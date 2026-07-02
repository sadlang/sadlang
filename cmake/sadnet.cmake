# ═══════════════════════════════════════════════════════════════════
# صNet — ملف بناء CMake
# (AR) مكتبة الشبكات اللامركزية للغة ص
# (EN) SadNet — Decentralized Networking Library for Sad Language
#
# الخيارات:
#   SADNET_FREESTANDING=ON   — بناء بدون نظام تشغيل (kernel/bare-metal)
#   SADNET_BUILD_TESTS=ON    — بناء اختبارات C++
# ═══════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.16)

option(SADNET_FREESTANDING "Build SadNet in freestanding mode (no OS)" OFF)
option(SADNET_BUILD_TESTS "Build SadNet C++ tests" OFF)

# ─── مجلد الرؤوس ───
set(SADNET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/features/network/sadnet/include")

# ─── ملفات المصدر ───
set(SADNET_SOURCES
    # النواة
    features/network/sadnet/src/core/types.cpp
    features/network/sadnet/src/core/transport.cpp

    # التشفير
    features/network/sadnet/src/crypto/sha256.cpp
    features/network/sadnet/src/crypto/aes_gcm.cpp
    features/network/sadnet/src/crypto/x25519.cpp

    # DHT
    features/network/sadnet/src/dht/routing_table.cpp
    features/network/sadnet/src/dht/dht_engine.cpp

    # NAT
    features/network/sadnet/src/nat/stun.cpp
    features/network/sadnet/src/nat/turn.cpp
    features/network/sadnet/src/nat/ice.cpp

    # الاكتشاف
    features/network/sadnet/src/discovery/discovery.cpp

    # المراسلة
    features/network/sadnet/src/messaging/messaging.cpp

    # التوجيه البصلي
    features/network/sadnet/src/routing/onion_routing.cpp

    # الهوية
    features/network/sadnet/src/identity/identity.cpp

    # DNS
    features/network/sadnet/src/dns/dns.cpp

    # التخزين
    features/network/sadnet/src/storage/storage.cpp

    # الأمان
    features/network/sadnet/src/security/security.cpp

    # البث المباشر
    features/network/sadnet/src/streaming/streaming.cpp

    # CDN
    features/network/sadnet/src/cdn/cdn.cpp
)

# ─── ملفات Freestanding ───
if(SADNET_FREESTANDING)
    list(APPEND SADNET_SOURCES
        features/network/sadnet/src/freestanding/freestanding.cpp
    )
endif()

# ─── إنشاء المكتبة ───
add_library(sadnet STATIC ${SADNET_SOURCES})

# (AR) AES-NI: Clang لا يفعّل ميزات الهدف عبر `#pragma GCC target`، فنمرّر
#      الأعلام صراحةً لوحدة aes_gcm على x86. الإرسال وقت التشغيل يحمي ARM/الأقدم.
# (EN) Clang ignores `#pragma GCC target`; pass AES-NI flags explicitly to the
#      aes_gcm TU on x86. Runtime dispatch guards non-AES-NI CPUs.
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|i386|i686")
    set_source_files_properties(
        features/network/sadnet/src/crypto/aes_gcm.cpp
        PROPERTIES COMPILE_OPTIONS "-maes;-mpclmul;-msse4.1;-mssse3"
    )
endif()

target_include_directories(sadnet PUBLIC
    ${SADNET_INCLUDE_DIR}
)

# ─── C++17 ───
target_compile_features(sadnet PUBLIC cxx_std_17)

# ─── تعريفات Freestanding ───
if(SADNET_FREESTANDING)
    target_compile_definitions(sadnet PUBLIC SADNET_FREESTANDING)
endif()

# ─── ربط مع ws2_32 على Windows (غير Freestanding) ───
if(WIN32 AND NOT SADNET_FREESTANDING)
    target_link_libraries(sadnet PUBLIC ws2_32 iphlpapi)
endif()

# ─── ربط مع pthread على Linux (غير Freestanding) ───
if(UNIX AND NOT SADNET_FREESTANDING)
    target_link_libraries(sadnet PUBLIC pthread)
endif()

# ═══════════════════════════════════════════════════════════════════
# (AR) ملف ربط المفسر (يُضاف لـ sad_core إذا تم تفعيل صNet)
# ═══════════════════════════════════════════════════════════════════
set(SADNET_BINDINGS_SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/interpreter/src/builtins/builtin_module_sadnet.cpp"
)

# (AR) يمكن للملف الرئيسي CMakeLists.txt استخدام:
#   target_sources(sad_core PRIVATE ${SADNET_BINDINGS_SOURCE})
#   target_link_libraries(sad_core PRIVATE sadnet)
# لتفعيل: استورد شبكة_لامركزية

message(STATUS "[صNet] SadNet library configured")
message(STATUS "[صNet] Freestanding: ${SADNET_FREESTANDING}")
message(STATUS "[صNet] Sources: ${CMAKE_CURRENT_SOURCE_DIR}/features/network/sadnet/")

# ═══════════════════════════════════════════════════════════════════
# (AR) اختبارات وحدة C++
# ═══════════════════════════════════════════════════════════════════
if(SADNET_BUILD_TESTS)
    add_executable(test_sadnet_unit
        features/network/sadnet/tests/test_sadnet_unit.cpp
    )
    target_link_libraries(test_sadnet_unit PRIVATE sadnet)
    target_include_directories(test_sadnet_unit PRIVATE ${SADNET_INCLUDE_DIR})
    target_compile_features(test_sadnet_unit PRIVATE cxx_std_17)

    # (AR) تسجيل في CTest
    add_test(NAME SadNet_UnitTests COMMAND test_sadnet_unit)

    message(STATUS "[صNet] Tests enabled: test_sadnet_unit")
endif()
