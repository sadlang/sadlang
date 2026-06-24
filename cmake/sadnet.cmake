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
set(SADNET_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/network/sadnet/include")

# ─── ملفات المصدر ───
set(SADNET_SOURCES
    # النواة
    network/sadnet/src/core/types.cpp
    network/sadnet/src/core/transport.cpp

    # التشفير
    network/sadnet/src/crypto/sha256.cpp
    network/sadnet/src/crypto/aes_gcm.cpp
    network/sadnet/src/crypto/x25519.cpp

    # DHT
    network/sadnet/src/dht/routing_table.cpp
    network/sadnet/src/dht/dht_engine.cpp

    # NAT
    network/sadnet/src/nat/stun.cpp
    network/sadnet/src/nat/turn.cpp
    network/sadnet/src/nat/ice.cpp

    # الاكتشاف
    network/sadnet/src/discovery/discovery.cpp

    # المراسلة
    network/sadnet/src/messaging/messaging.cpp

    # التوجيه البصلي
    network/sadnet/src/routing/onion_routing.cpp

    # الهوية
    network/sadnet/src/identity/identity.cpp

    # DNS
    network/sadnet/src/dns/dns.cpp

    # التخزين
    network/sadnet/src/storage/storage.cpp

    # الأمان
    network/sadnet/src/security/security.cpp

    # البث المباشر
    network/sadnet/src/streaming/streaming.cpp

    # CDN
    network/sadnet/src/cdn/cdn.cpp
)

# ─── ملفات Freestanding ───
if(SADNET_FREESTANDING)
    list(APPEND SADNET_SOURCES
        network/sadnet/src/freestanding/freestanding.cpp
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
        network/sadnet/src/crypto/aes_gcm.cpp
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
message(STATUS "[صNet] Sources: ${CMAKE_CURRENT_SOURCE_DIR}/network/sadnet/")

# ═══════════════════════════════════════════════════════════════════
# (AR) اختبارات وحدة C++
# ═══════════════════════════════════════════════════════════════════
if(SADNET_BUILD_TESTS)
    add_executable(test_sadnet_unit
        network/sadnet/tests/test_sadnet_unit.cpp
    )
    target_link_libraries(test_sadnet_unit PRIVATE sadnet)
    target_include_directories(test_sadnet_unit PRIVATE ${SADNET_INCLUDE_DIR})
    target_compile_features(test_sadnet_unit PRIVATE cxx_std_17)

    # (AR) تسجيل في CTest
    add_test(NAME SadNet_UnitTests COMMAND test_sadnet_unit)

    message(STATUS "[صNet] Tests enabled: test_sadnet_unit")
endif()
