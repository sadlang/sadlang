cmake_minimum_required(VERSION 3.16)

# أداة بناء APK
add_executable(sad-apk
    src/apk_builder.cpp
)

target_compile_features(sad-apk PRIVATE cxx_std_17)

target_include_directories(sad-apk PRIVATE
    ${CMAKE_SOURCE_DIR}/shared/utils/include
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(sad-apk PRIVATE
        _CRT_SECURE_NO_WARNINGS
        UNICODE
        _UNICODE
    )
endif()

# Install
install(TARGETS sad-apk
    RUNTIME DESTINATION bin
)
