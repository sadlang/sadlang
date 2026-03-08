# نظام Flutter للغة ص — CMakeLists.txt

# إضافة مكتبة Flutter UI
add_library(sad_flutter STATIC
    # Core framework
    sad_flutter_core.cpp
    
    # Basic widgets
    sad_flutter_widgets.cpp
    
    # Layout widgets
    sad_flutter_layout.cpp
    
    # Input widgets
    sad_flutter_input.cpp
    
    # Application framework
    sad_flutter_app.cpp
    
    # Animation system
    sad_flutter_animation.cpp
    
    # Dialogs and popups
    sad_flutter_dialogs.cpp
    
    # Forms and validation
    sad_flutter_forms.cpp
    
    # Navigation 2.0
    sad_flutter_navigation.cpp
    
    # Cupertino (iOS-style) widgets
    sad_flutter_cupertino.cpp
    
    # Material components
    sad_flutter_material.cpp
    
    # Scrolling widgets
    sad_flutter_scroll.cpp
    
    # Theme system (header-only, but included for IDE)
    # sad_flutter_theme.h
    
    # Accessibility support (header-only)
    # sad_flutter_accessibility.h
)

target_include_directories(sad_flutter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# C++17 required
target_compile_features(sad_flutter PUBLIC cxx_std_17)

# Platform-specific dependencies
if(WIN32)
    # Windows: GDI+ or Direct2D for rendering
    target_link_libraries(sad_flutter PRIVATE 
        gdiplus
        # d2d1  # If using Direct2D
    )
elseif(APPLE)
    # macOS/iOS: Core Graphics
    find_library(CORE_GRAPHICS CoreGraphics)
    find_library(UIKIT UIKit)
    target_link_libraries(sad_flutter PRIVATE 
        ${CORE_GRAPHICS}
    )
elseif(ANDROID)
    # Android: Skia or Canvas
    target_link_libraries(sad_flutter PRIVATE 
        android
        log
    )
elseif(EMSCRIPTEN)
    # WASM: HTML Canvas
    # No additional libraries needed
else()
    # Linux: Cairo or SDL2
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(CAIRO cairo)
    if(CAIRO_FOUND)
        target_include_directories(sad_flutter PRIVATE ${CAIRO_INCLUDE_DIRS})
        target_link_libraries(sad_flutter PRIVATE ${CAIRO_LIBRARIES})
    endif()
endif()

# Optional: Link with SDL2 for cross-platform rendering
find_package(SDL2 QUIET)
if(SDL2_FOUND)
    target_include_directories(sad_flutter PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(sad_flutter PRIVATE ${SDL2_LIBRARIES})
    target_compile_definitions(sad_flutter PRIVATE SAD_FLUTTER_USE_SDL2)
endif()

# Export headers
set_target_properties(sad_flutter PROPERTIES
    PUBLIC_HEADER "sad_flutter.h;sad_flutter_core.h;sad_flutter_widgets.h;sad_flutter_layout.h;sad_flutter_input.h;sad_flutter_app.h;sad_flutter_arabic.h"
)
