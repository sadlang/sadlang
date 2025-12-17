# CMakeLists.txt for Bytecode Backend
# نظام البناء لمكون البايت كود

cmake_minimum_required(VERSION 3.15)

# ========================================
# Bytecode Library
# مكتبة البايت كود
# ========================================

set(BYTECODE_HEADERS
    include/opcodes.h
    include/value.h
    include/bytecode.h
    include/emitter.h
)

set(BYTECODE_SOURCES
    src/opcodes.cpp
    src/value.cpp
    src/bytecode.cpp
    src/emitter.cpp
)

# Create bytecode library
add_library(sad_bytecode STATIC
    ${BYTECODE_HEADERS}
    ${BYTECODE_SOURCES}
)

# Include directories
target_include_directories(sad_bytecode PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../frontend/include
)

# Link with frontend
target_link_libraries(sad_bytecode PUBLIC
    sad_frontend
)

# C++17 standard
target_compile_features(sad_bytecode PUBLIC cxx_std_17)

# ========================================
# Installation
# ========================================

install(TARGETS sad_bytecode
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${BYTECODE_HEADERS}
    DESTINATION include/sad/bytecode
)
