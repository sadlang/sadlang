# ═══════════════════════════════════════════════════════════════════════════════
# ملف: sdl2_platforms.cmake
# المسار: cmake/sdl2_platforms.cmake
# ═══════════════════════════════════════════════════════════════════════════════
#
# الوصف:
# ------
# إدارة إعداد SDL2 عبر المنصات المتعددة:
#   - Desktop (Windows, Linux, macOS)
#   - Android (NDK)
#   - iOS
#   - Web (Emscripten)
#
# ═══════════════════════════════════════════════════════════════════════════════

# ─── الكشف عن المنصة الحالية ───
if(EMSCRIPTEN)
    set(SAD_PLATFORM "web")
    set(SAD_PLATFORM_WEB ON)
elseif(ANDROID)
    set(SAD_PLATFORM "android")
    set(SAD_PLATFORM_ANDROID ON)
elseif(IOS)
    set(SAD_PLATFORM "ios")
    set(SAD_PLATFORM_IOS ON)
elseif(APPLE)
    set(SAD_PLATFORM "macos")
    set(SAD_PLATFORM_MACOS ON)
elseif(WIN32)
    set(SAD_PLATFORM "windows")
    set(SAD_PLATFORM_DESKTOP ON)
elseif(UNIX)
    set(SAD_PLATFORM "linux")
    set(SAD_PLATFORM_DESKTOP ON)
else()
    set(SAD_PLATFORM "unknown")
endif()

message(STATUS "  [sad_graphics] المنصة المكتشفة / Detected platform: ${SAD_PLATFORM}")

# ═══════════════════════════════════════════════════════════════════════════════
# دالة: إعداد SDL2 للمنصة الحالية
# ═══════════════════════════════════════════════════════════════════════════════

function(setup_sdl2_for_platform target_name)
    if(SAD_PLATFORM_WEB)
        # ─── Web (Emscripten) ───
        # SDL2 مدمج في Emscripten
        target_compile_options(${target_name} PRIVATE
            -sUSE_SDL=2
            -sUSE_SDL_TTF=2
            -sUSE_SDL_IMAGE=2
        )
        target_link_options(${target_name} PRIVATE
            -sUSE_SDL=2
            -sUSE_SDL_TTF=2
            -sUSE_SDL_IMAGE=2
            -sALLOW_MEMORY_GROWTH=1
            -sASYNCIFY
        )
        target_compile_definitions(${target_name} PUBLIC
            SAD_UI_USE_SDL2
            SAD_UI_HAS_SDL_TTF
            SAD_PLATFORM_WEB
        )
        message(STATUS "  [${target_name}] ✓ SDL2 via Emscripten")

    elseif(SAD_PLATFORM_ANDROID)
        # ─── Android (NDK) ───
        # SDL2 كمكتبة مشتركة
        find_library(SDL2_LIB SDL2)
        find_library(SDL2_TTF_LIB SDL2_ttf)

        if(SDL2_LIB)
            target_link_libraries(${target_name} PRIVATE ${SDL2_LIB})
            target_compile_definitions(${target_name} PUBLIC
                SAD_UI_USE_SDL2
                SAD_PLATFORM_ANDROID
            )
            if(SDL2_TTF_LIB)
                target_link_libraries(${target_name} PRIVATE ${SDL2_TTF_LIB})
                target_compile_definitions(${target_name} PUBLIC SAD_UI_HAS_SDL_TTF)
            endif()
            message(STATUS "  [${target_name}] ✓ SDL2 for Android NDK")
        else()
            message(STATUS "  [${target_name}] ⚠ SDL2 not found for Android — using fallback")
            target_compile_definitions(${target_name} PUBLIC
                SAD_PLATFORM_ANDROID
                SAD_UI_NO_SDL2
            )
        endif()

    elseif(SAD_PLATFORM_IOS)
        # ─── iOS ───
        # SDL2.framework
        find_library(SDL2_FRAMEWORK SDL2)
        find_library(SDL2_TTF_FRAMEWORK SDL2_ttf)

        if(SDL2_FRAMEWORK)
            target_link_libraries(${target_name} PRIVATE
                ${SDL2_FRAMEWORK}
                "-framework UIKit"
                "-framework CoreGraphics"
                "-framework OpenGLES"
                "-framework Metal"
                "-framework MetalKit"
            )
            target_compile_definitions(${target_name} PUBLIC
                SAD_UI_USE_SDL2
                SAD_PLATFORM_IOS
            )
            if(SDL2_TTF_FRAMEWORK)
                target_link_libraries(${target_name} PRIVATE ${SDL2_TTF_FRAMEWORK})
                target_compile_definitions(${target_name} PUBLIC SAD_UI_HAS_SDL_TTF)
            endif()
            message(STATUS "  [${target_name}] ✓ SDL2.framework for iOS")
        else()
            message(STATUS "  [${target_name}] ⚠ SDL2 not found for iOS")
            target_compile_definitions(${target_name} PUBLIC
                SAD_PLATFORM_IOS
                SAD_UI_NO_SDL2
            )
        endif()

    elseif(SAD_PLATFORM_MACOS)
        # ─── macOS ───
        find_package(SDL2 CONFIG QUIET)
        if(SDL2_FOUND)
            target_link_libraries(${target_name} PRIVATE SDL2::SDL2)
            target_compile_definitions(${target_name} PUBLIC
                SAD_UI_USE_SDL2
                SAD_PLATFORM_MACOS
            )

            find_package(SDL2_ttf CONFIG QUIET)
            if(SDL2_ttf_FOUND)
                target_link_libraries(${target_name} PRIVATE SDL2_ttf::SDL2_ttf)
                target_compile_definitions(${target_name} PUBLIC SAD_UI_HAS_SDL_TTF)
            endif()
            message(STATUS "  [${target_name}] ✓ SDL2 for macOS")
        else()
            message(STATUS "  [${target_name}] ⚠ SDL2 not found via CMake config")
            target_compile_definitions(${target_name} PUBLIC SAD_PLATFORM_MACOS)
        endif()

    else()
        # ─── Desktop (Windows/Linux) ─ handled by main CMakeLists.txt ───
        target_compile_definitions(${target_name} PUBLIC SAD_PLATFORM_DESKTOP)
    endif()
endfunction()

# ═══════════════════════════════════════════════════════════════════════════════
# دالة: إضافة خيارات خاصة بالمنصة
# ═══════════════════════════════════════════════════════════════════════════════

function(add_platform_compile_options target_name)
    if(SAD_PLATFORM_WEB)
        # Emscripten specific
        target_compile_options(${target_name} PRIVATE
            -fexceptions
            -pthread
        )
    elseif(SAD_PLATFORM_ANDROID)
        # Android NDK specific
        target_compile_options(${target_name} PRIVATE
            -fPIC
            -fexceptions
            -frtti
        )
    elseif(SAD_PLATFORM_IOS)
        # iOS specific
        target_compile_options(${target_name} PRIVATE
            -fvisibility=hidden
            -fvisibility-inlines-hidden
        )
        set_target_properties(${target_name} PROPERTIES
            XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
        )
    endif()
endfunction()

# ═══════════════════════════════════════════════════════════════════════════════
# متغيرات للتبليغ
# ═══════════════════════════════════════════════════════════════════════════════

set(SAD_SDL2_PLATFORMS_INCLUDED ON CACHE INTERNAL "SDL2 platforms module loaded")
