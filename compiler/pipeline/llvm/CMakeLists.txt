# ============================================================================
# LLVM Compiler Pipeline - CMake Configuration
# تكوين LLVM Compiler Pipeline
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# LLVM Pipeline Library  
# مكتبة LLVM Pipeline
add_library(llvm_pipeline
    src/llvm_compiler_pipeline.cpp
    # LLVM Codegen files - ملفات LLVM Codegen
    ${CMAKE_SOURCE_DIR}/compiler/backends/llvm/llvm_codegen.cpp
    ${CMAKE_SOURCE_DIR}/compiler/backends/llvm/llvm_optimizer.cpp
    ${CMAKE_SOURCE_DIR}/compiler/backends/llvm/llvm_type_mapper.cpp
    ${CMAKE_SOURCE_DIR}/compiler/backends/llvm/llvm_control_flow.cpp
    ${CMAKE_SOURCE_DIR}/compiler/backends/llvm/llvm_expression_builder.cpp
    ${CMAKE_SOURCE_DIR}/compiler/backends/llvm/llvm_target.cpp
)

target_include_directories(llvm_pipeline
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/compiler/include
        ${CMAKE_SOURCE_DIR}/compiler/frontend/include
        ${CMAKE_SOURCE_DIR}/compiler/backends/llvm
        ${LLVM_INCLUDE_DIRS}
)

# Link with LLVM
# الربط مع LLVM
target_link_libraries(llvm_pipeline
    PUBLIC
        lexer
        parser
        sir_builder
        ${LLVM_LIBS}
)

# C++17 Required
# C++17 مطلوب
target_compile_features(llvm_pipeline PUBLIC cxx_std_17)

# Disable opaque pointers for compatibility with legacy code
# تعطيل Opaque Pointers للتوافق مع الكود القديم
if (MSVC)
    target_compile_options(llvm_pipeline PRIVATE 
        /D_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
    )
endif()

# Installation
# التثبيت
install(TARGETS llvm_pipeline
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include/compiler/pipeline/llvm
)
