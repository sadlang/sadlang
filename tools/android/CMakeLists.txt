# ═══════════════════════════════════════════════════════════════════════════
# CMakeLists.txt - بناء مكتبة Sad Runtime لنظام Android
# ═══════════════════════════════════════════════════════════════════════════
#
# هذا الملف يُنشئ:
# 1. libsad_android.so - مكتبة الـ runtime الأصلية
# 2. JNI bindings للتواصل مع Java/Kotlin
# 3. Android UI bindings
#
# الاستخدام:
#   mkdir build-android && cd build-android
#   cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
#         -DANDROID_ABI=arm64-v8a \
#         -DANDROID_PLATFORM=android-21 \
#         ..
#   cmake --build .
# ═══════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.18)
project(sad_android VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ═══════════════════════════════════════════════════════════════════════════
# المصادر / Sources
# ═══════════════════════════════════════════════════════════════════════════

set(SAD_ANDROID_SOURCES
    src/sad_android_runtime.cpp
    src/sad_jni_bridge.cpp
    src/sad_android_ui.cpp
    src/sad_android_apis.cpp
)

set(SAD_ANDROID_HEADERS
    include/sad_android_runtime.h
    include/sad_jni_bridge.h
    include/sad_android_ui.h
    include/sad_android_apis.h
)

# ═══════════════════════════════════════════════════════════════════════════
# المكتبة الأصلية / Native Library
# ═══════════════════════════════════════════════════════════════════════════

# فقط بناء المكتبة إذا كانت الملفات موجودة
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/sad_android_runtime.cpp")
    add_library(sad_android SHARED ${SAD_ANDROID_SOURCES})

    target_include_directories(sad_android PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/lexer/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/parser/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/ast/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/types/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../interpreter_new/include
    )

    # Android specific
    if(ANDROID)
        find_library(log-lib log)
        find_library(android-lib android)
        
        target_link_libraries(sad_android
            ${log-lib}
            ${android-lib}
        )
        
        # تعريف للتجميع على Android
        target_compile_definitions(sad_android PRIVATE
            SAD_ANDROID=1
            SAD_PLATFORM_ANDROID=1
        )
    endif()

    # ═══════════════════════════════════════════════════════════════════════════
    # التثبيت / Installation
    # ═══════════════════════════════════════════════════════════════════════════

    install(TARGETS sad_android
        LIBRARY DESTINATION lib/${ANDROID_ABI}
        ARCHIVE DESTINATION lib/${ANDROID_ABI}
    )

    install(FILES ${SAD_ANDROID_HEADERS}
        DESTINATION include/sad_android
    )
endif() # End of if(EXISTS src/sad_android_runtime.cpp)

# ═══════════════════════════════════════════════════════════════════════════
# أداة sad-android CLI (للحاسوب، ليس Android)
# ═══════════════════════════════════════════════════════════════════════════

if(NOT ANDROID)
    add_executable(sad-android sad_android.cpp)
    target_compile_features(sad-android PRIVATE cxx_std_17)
    
    if(WIN32)
        target_compile_options(sad-android PRIVATE /utf-8)
    endif()
    
    # نسخ إلى مجلد الإخراج
    add_custom_command(TARGET sad-android POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:sad-android>
            ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/sad-android${CMAKE_EXECUTABLE_SUFFIX}
    )
    
    install(TARGETS sad-android RUNTIME DESTINATION bin)

    # ═══════════════════════════════════════════════════════════════════════════
    # أداة بناء APK / APK Builder Tool
    # ═══════════════════════════════════════════════════════════════════════════
    
    add_executable(sad_android_builder sad_android_builder.cpp)
    target_compile_features(sad_android_builder PRIVATE cxx_std_17)
    
    if(WIN32)
        target_compile_options(sad_android_builder PRIVATE /utf-8)
    else()
        target_link_libraries(sad_android_builder PRIVATE stdc++fs)
    endif()
    
    # نسخ إلى مجلد الإخراج
    add_custom_command(TARGET sad_android_builder POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:sad_android_builder>
            ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/sad_android_builder${CMAKE_EXECUTABLE_SUFFIX}
    )
    
    install(TARGETS sad_android_builder RUNTIME DESTINATION bin)
    
    message(STATUS "✓ تمت إضافة أداة بناء APK: sad_android_builder")
endif()