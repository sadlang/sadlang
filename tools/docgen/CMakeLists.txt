# بسم الله الرحمن الرحيم
# CMake Configuration for Sad Documentation Generator
# إعدادات CMake لمولد التوثيق
# Phase 7.4: Documentation Generator

cmake_minimum_required(VERSION 3.15)
project(sad_docgen VERSION 1.0.0 LANGUAGES CXX)

# الحمد لله - C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Sources / الملفات المصدرية
set(DOCGEN_SOURCES
    docgen.cpp
    docgen.h
)

set(MAIN_SOURCE
    main.cpp
)

# Create library / إنشاء مكتبة
add_library(sad_docgen_lib STATIC ${DOCGEN_SOURCES})

target_include_directories(sad_docgen_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Create executable / إنشاء برنامج قابل للتنفيذ
add_executable(saddoc ${MAIN_SOURCE})

target_link_libraries(saddoc PRIVATE sad_docgen_lib)

# Installation / التثبيت
install(TARGETS saddoc
    RUNTIME DESTINATION bin
)

install(FILES docgen.h
    DESTINATION include/sad
)

install(TARGETS sad_docgen_lib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# الحمد لله - Documentation Generator Build Complete!
message(STATUS "الحمد لله - Sad Documentation Generator configured successfully!")
