# بسم الله الرحمن الرحيم
# CMakeLists.txt for sad-repl

cmake_minimum_required(VERSION 3.15)

# ========================================
# REPL Target / هدف REPL
# ========================================

# Note: Implementation files (.cpp) will be created in Phase 1.5.1
# Currently we only have headers

# Collect sources
set(REPL_SOURCES
    main.cpp
    repl_engine.cpp
    history_manager.cpp
    repl_commands.cpp
)

set(REPL_HEADERS
    repl_engine.h
    history_manager.h
    repl_commands.h
)

# Create sad-repl executable
add_executable(sad-repl ${REPL_SOURCES} ${REPL_HEADERS})

# Include directories
target_include_directories(sad-repl PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}
    # (AR) Phase 4 (F-03): مسارات shared لتجنب #include النسبية
    # (EN) Phase 4 (F-03): shared paths to avoid relative #includes
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/utils/include
)

# Link libraries
target_link_libraries(sad-repl PRIVATE
    sad_core
    sad_semantic
)

# C++ Standard
set_target_properties(sad-repl PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Output directory
# (AR) نستعمل ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} كبقيّة الأهداف ليقع sad-repl في
#      نفس مجلّد sad-hub (الذي يكتشف الأدوات بمسح أشقّائه). الصيغة القديمة
#      bin/${CMAKE_BUILD_TYPE} كانت تُنتج bin/Debug/Debug على MSVC (إلحاق تلقائيّ
#      للتهيئة) وbin/Debug على Linux بينما الأشقّاء في bin/ — فيختفي repl من --list.
# (EN) Use ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} like every other target so sad-repl
#      lands next to sad-hub (which discovers tools by scanning siblings). The old
#      bin/${CMAKE_BUILD_TYPE} produced bin/Debug/Debug on MSVC (auto config append)
#      and bin/Debug on Linux while siblings sit in bin/ — hiding repl from --list.
set_target_properties(sad-repl PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# ========================================
# Platform-specific settings
# ========================================

if(WIN32)
    # Windows: Enable UTF-8
    target_compile_options(sad-repl PRIVATE /utf-8)
endif()

# ========================================
# Installation / التثبيت
# ========================================

install(TARGETS sad-repl
    RUNTIME DESTINATION bin
    COMPONENT repl
)

# Install README
install(FILES README.md
    DESTINATION share/doc/sad-repl
    COMPONENT repl
)

message(STATUS "sad-repl target configured")
