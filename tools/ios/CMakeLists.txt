# ============================================================================
# CMakeLists.txt — أداة iOS لغة ص
# iOS CLI Tool for Sad Language
# ============================================================================
#
# الوصف (AR):
#   يُعرّف هدف sad-ios وهو أداة سطر أوامر لبناء تطبيقات iOS
#   من كود لغة ص. يعتمد على sad_core للتحليل المعجمي والنحوي.
#
# Description (EN):
#   Defines the sad-ios target — a CLI tool for building iOS apps
#   from Sad language code. Depends on sad_core for lexing/parsing.
# ============================================================================

cmake_minimum_required(VERSION 3.20)

# ============================================
# هدف sad-ios / sad-ios target
# ============================================
add_executable(sad-ios
    sad_ios_cli.cpp
)

target_include_directories(sad-ios PRIVATE
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
)

# ربط بالنواة المشتركة / Link to shared core
target_link_libraries(sad-ios PRIVATE
    sad_core
)

# C++17 مطلوب / C++17 required
target_compile_features(sad-ios PRIVATE cxx_std_17)

# إعدادات التثبيت / Install settings
install(TARGETS sad-ios
    RUNTIME DESTINATION bin
)

message(STATUS "[sad-ios] أداة iOS CLI مُعرّفة / iOS CLI tool configured")
