# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# مستودع حزم لغة ص - Sad Language Package Registry
# ═══════════════════════════════════════════════════════════════════════════════
#
# هذا الملف يبني خادم مستودع الحزم المركزي للغة ص.
# الخادم يوفر:
#   - واجهة REST API لإدارة الحزم
#   - قاعدة بيانات SQLite للبيانات الوصفية
#   - تخزين محتوى-موجه (Content-Addressable) للحزم
#   - محرك بحث عربي ذكي
#   - نظام اتحاد مستودعات لامركزي (Federation)
#   - واجهة ويب عربية بالكامل
#
# المتطلبات:
#   - CMake 3.15+
#   - مترجم C++17
#   - مكتبة SQLite3 (مضمّنة كملف واحد)
#
# البناء:
#   mkdir build && cd build
#   cmake .. -DCMAKE_BUILD_TYPE=Release
#   cmake --build .
#
# التشغيل:
#   ./sad-registry --port 8080
#   ./sad-registry --port 8080 --db ./data/packages.db
#
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

project(sad-registry
    VERSION 1.0.0
    DESCRIPTION "مستودع حزم لغة ص - Sad Language Package Registry"
    LANGUAGES CXX C
)

# ─────────────────────────────────────────────────────────────────────────────
# إعدادات المترجم
# ─────────────────────────────────────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# دعم UTF-8 على MSVC
if(MSVC)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# ─────────────────────────────────────────────────────────────────────────────
# SQLite3 المضمّنة
# ─────────────────────────────────────────────────────────────────────────────
# نستخدم SQLite كملف مصدري واحد (amalgamation) لتسهيل البناء
# يمكن تنزيلها من: https://www.sqlite.org/download.html
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite3.c")
    add_library(sqlite3 STATIC
        third_party/sqlite3.c
    )
    target_include_directories(sqlite3 PUBLIC third_party/)
    # تفعيل ميزات FTS5 للبحث النصي الكامل
    target_compile_definitions(sqlite3 PRIVATE
        SQLITE_ENABLE_FTS5
        SQLITE_ENABLE_JSON1
        SQLITE_ENABLE_RTREE
    )
    set(SQLITE3_LIBRARIES sqlite3)
    set(SQLITE3_FOUND TRUE)
else()
    # محاولة إيجاد SQLite3 مثبتة على النظام
    find_package(SQLite3)
    if(NOT SQLite3_FOUND)
        message(STATUS "⚠ SQLite3 غير موجودة - سيتم استخدام المحاكاة المضمّنة")
        set(SQLITE3_FOUND FALSE)
    else()
        set(SQLITE3_LIBRARIES SQLite::SQLite3)
        set(SQLITE3_FOUND TRUE)
    endif()
endif()

# ─────────────────────────────────────────────────────────────────────────────
# ملفات المصدر
# ─────────────────────────────────────────────────────────────────────────────

set(REGISTRY_SOURCES
    # النواة
    src/main.cpp

    # الخادم
    src/server/http_server.cpp
    src/server/api_routes.cpp

    # التخزين
    src/storage/database.cpp
    src/storage/blob_store.cpp

    # البحث العربي
    src/search/arabic_search.cpp

    # الاتحاد اللامركزي
    src/federation/federation.cpp

    # المصادقة
    src/auth/auth.cpp

    # التشفير
    src/auth/crypto.cpp

    # حل التبعيات
    src/deps/dependency_resolver.cpp
)

# الملفات الترويسية (headers)
set(REGISTRY_HEADERS
    src/server/http_server.h
    src/server/api_routes.h
    src/server/middleware.h
    src/storage/database.h
    src/storage/blob_store.h
    src/search/arabic_search.h
    src/search/bilingual_index.h
    src/federation/federation.h
    src/auth/auth.h
    src/auth/crypto.h
    src/deps/dependency_resolver.h
)

# ─────────────────────────────────────────────────────────────────────────────
# هدف البناء الرئيسي
# ─────────────────────────────────────────────────────────────────────────────
add_executable(sad-registry ${REGISTRY_SOURCES} ${REGISTRY_HEADERS})

target_include_directories(sad-registry PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ربط المكتبات
if(SQLITE3_FOUND)
    target_link_libraries(sad-registry PRIVATE ${SQLITE3_LIBRARIES})
    target_compile_definitions(sad-registry PRIVATE SAD_HAS_SQLITE=1)
else()
    target_compile_definitions(sad-registry PRIVATE SAD_HAS_SQLITE=0)
endif()

# مكتبات النظام
if(WIN32)
    # مكتبات الشبكة على ويندوز
    target_link_libraries(sad-registry PRIVATE ws2_32 winhttp)
    target_compile_definitions(sad-registry PRIVATE _WIN32_WINNT=0x0601)
else()
    # على لينكس: pthread
    target_link_libraries(sad-registry PRIVATE pthread)
endif()

# ─────────────────────────────────────────────────────────────────────────────
# مجلد الإخراج
# ─────────────────────────────────────────────────────────────────────────────
set_target_properties(sad-registry PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ─────────────────────────────────────────────────────────────────────────────
# نسخ الملفات الثابتة (واجهة الويب)
# ─────────────────────────────────────────────────────────────────────────────
# نسخ ملفات الويب إلى مجلد البناء
add_custom_command(TARGET sad-registry POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/web"
        "${CMAKE_BINARY_DIR}/bin/web"
    COMMENT "نسخ ملفات واجهة الويب..."
)

# نسخ ملفات SQL
add_custom_command(TARGET sad-registry POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/migrations"
        "${CMAKE_BINARY_DIR}/bin/migrations"
    COMMENT "نسخ ملفات قاعدة البيانات..."
)

# ─────────────────────────────────────────────────────────────────────────────
# رسالة ملخص البناء
# ─────────────────────────────────────────────────────────────────────────────
message(STATUS "═══════════════════════════════════════════════")
message(STATUS "  مستودع حزم لغة ص v${PROJECT_VERSION}")
message(STATUS "  SQLite3: ${SQLITE3_FOUND}")
message(STATUS "  النظام: ${CMAKE_SYSTEM_NAME}")
message(STATUS "═══════════════════════════════════════════════")
