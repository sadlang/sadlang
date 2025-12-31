# ==============================================================================
# CMakeLists.txt للأمثلة / Examples CMakeLists.txt
# ==============================================================================

cmake_minimum_required(VERSION 3.15)

# ==============================================================================
# مثال بسيط / Simple Demo
# ==============================================================================
add_executable(simple_demo
    simple_demo.cpp
)

target_link_libraries(simple_demo
    PRIVATE
        sad_graphics
)

# ==============================================================================
# مثال التكسيشرات / Texture Demo
# ==============================================================================
add_executable(texture_demo
    texture_demo.cpp
)

target_link_libraries(texture_demo
    PRIVATE
        sad_graphics
)

# ==============================================================================
# مثال Sprites / Sprite Demo
# ==============================================================================
add_executable(sprite_demo
    sprite_demo.cpp
)

target_link_libraries(sprite_demo
    PRIVATE
        sad_graphics
)

# ==============================================================================
# مثال النصوص / Text Demo
# ==============================================================================
add_executable(text_demo
    text_demo.cpp
)

target_link_libraries(text_demo
    PRIVATE
        sad_graphics
)

# ==============================================================================
# مثال إدارة الموارد / Resource Management Demo
# ==============================================================================
add_executable(resource_demo
    resource_demo.cpp
)

target_link_libraries(resource_demo
    PRIVATE
        sad_graphics
)

# ==============================================================================
# مثال واجهة المستخدم / UI System Demo
# ==============================================================================
add_executable(ui_demo
    ui_demo.cpp
)

target_link_libraries(ui_demo
    PRIVATE
        sad_graphics
)

# ==============================================================================
# نسخ SDL2.dll للتشغيل / Copy SDL2.dll for execution
# ==============================================================================
set(SDL2_DLL "${CMAKE_SOURCE_DIR}/third_party/SDL2/lib/x64/SDL2.dll")

if(EXISTS ${SDL2_DLL})
    # نسخ DLL إلى مجلد البناء / Copy DLL to build directory
    add_custom_command(TARGET simple_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL}
        $<TARGET_FILE_DIR:simple_demo>
        COMMENT "نسخ SDL2.dll / Copying SDL2.dll"
    )
    
    add_custom_command(TARGET texture_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL}
        $<TARGET_FILE_DIR:texture_demo>
        COMMENT "نسخ SDL2.dll / Copying SDL2.dll"
    )
    
    add_custom_command(TARGET sprite_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL}
        $<TARGET_FILE_DIR:sprite_demo>
        COMMENT "نسخ SDL2.dll / Copying SDL2.dll"
    )
    
    add_custom_command(TARGET text_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL}
        $<TARGET_FILE_DIR:text_demo>
        COMMENT "نسخ SDL2.dll / Copying SDL2.dll"
    )
    
    add_custom_command(TARGET resource_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL}
        $<TARGET_FILE_DIR:resource_demo>
        COMMENT "نسخ SDL2.dll / Copying SDL2.dll"
    )
    
    add_custom_command(TARGET ui_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_DLL}
        $<TARGET_FILE_DIR:ui_demo>
        COMMENT "نسخ SDL2.dll / Copying SDL2.dll"
    )
endif()

message(STATUS "تم إنشاء الأمثلة / Examples created: simple_demo, texture_demo, sprite_demo, text_demo, resource_demo, ui_demo")
