# CMakeLists.txt for Virtual Machine
# نظام البناء للآلة الافتراضية

cmake_minimum_required(VERSION 3.15)

# ========================================
# VM Library
# مكتبة الآلة الافتراضية
# ========================================

set(VM_HEADERS
    include/vm.h
)

set(VM_SOURCES
    src/vm.cpp
    src/vm_opcodes.cpp
    src/vm_helpers.cpp
)

# Create VM library
add_library(sad_vm STATIC
    ${VM_HEADERS}
    ${VM_SOURCES}
)

# Include directories
target_include_directories(sad_vm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../compiler/bytecode/include
)

# Link with bytecode
target_link_libraries(sad_vm PUBLIC
    sad_bytecode
)

# C++17 standard
target_compile_features(sad_vm PUBLIC cxx_std_17)

# ========================================
# Installation
# ========================================

install(TARGETS sad_vm
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${VM_HEADERS}
    DESTINATION include/sad/vm
)
