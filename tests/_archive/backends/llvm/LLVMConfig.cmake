# LLVM CMake Configuration File
# Generated for Windows Pre-built LLVM 18.1.8

set(LLVM_VERSION "18.1.8")
set(LLVM_PACKAGE_VERSION "18.1.8")
set(LLVM_VERSION_MAJOR 18)
set(LLVM_VERSION_MINOR 1)
set(LLVM_VERSION_PATCH 8)

# Set LLVM directories - Windows paths
set(LLVM_INSTALL_PREFIX "C:/Program Files/LLVM")
set(LLVM_INCLUDE_DIRS "${LLVM_INSTALL_PREFIX}/include")
set(LLVM_LIBRARY_DIRS "${LLVM_INSTALL_PREFIX}/lib")
set(LLVM_BINARY_DIR "${LLVM_INSTALL_PREFIX}/bin")
set(LLVM_TOOLS_BINARY_DIR "${LLVM_INSTALL_PREFIX}/bin")

# Set definitions
set(LLVM_DEFINITIONS "-D_WIN32 -D_WINDOWS")

# Set found flag
set(LLVM_FOUND TRUE)
set(LLVM_DIR "${CMAKE_CURRENT_LIST_DIR}")

message(STATUS "Found LLVM ${LLVM_VERSION}")
message(STATUS "LLVM Include Dir: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM Library Dir: ${LLVM_LIBRARY_DIRS}")

# Provide llvm_map_components_to_libnames macro
# This is a simplified version that just returns the available libs
macro(llvm_map_components_to_libnames out_libs)
    set(${out_libs} 
        "${LLVM_LIBRARY_DIRS}/LLVM-C.lib"
        "${LLVM_LIBRARY_DIRS}/LTO.lib"
    )
    message(STATUS "LLVM libraries: ${${out_libs}}")
endmacro()
