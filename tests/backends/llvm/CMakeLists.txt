# CMakeLists.txt لاختبارات LLVM Backend
# CMakeLists.txt for LLVM Backend Tests

cmake_minimum_required(VERSION 3.15)

# إعداد المشروع / Setup project
project(SadLanguage_LLVM_Tests)

# تحديد معيار C++ / Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# البحث عن LLVM / Find LLVM
# Set LLVM_DIR to our custom config
set(LLVM_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# إضافة تعريفات LLVM / Add LLVM definitions
add_definitions(${LLVM_DEFINITIONS})

# تضمين دلائل LLVM / Include LLVM directories
include_directories(${LLVM_INCLUDE_DIRS})

# ربط دلائل LLVM / Link LLVM directories
link_directories(${LLVM_LIBRARY_DIRS})

# تضمين دليل المصدر / Include source directory
# Get the actual project root (3 levels up from tests/backends/llvm)
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
include_directories(${PROJECT_ROOT})
include_directories(${PROJECT_ROOT}/compiler/backends/llvm)

# مكونات LLVM المطلوبة / Required LLVM components
llvm_map_components_to_libnames(llvm_libs
    Core
    Support
    Target
    Analysis
    TransformUtils
    Scalar
    InstCombine
    AggressiveInstCombine
    Passes
    IPO
    Vectorize
    X86
    ARM
    AArch64
    RISCV
    WebAssembly
)

# ملفات المصدر للاختبارات / Test source files
set(TEST_SOURCES
    test_runner.cpp
    test_llvm_linker.cpp
    test_llvm_optimizer.cpp
    test_llvm_target.cpp
    test_llvm_debug.cpp
    test_llvm_runtime.cpp
    test_llvm_codegen.cpp
    test_integration.cpp
)

# ملفات المصدر للمكونات المُختبرة / Source files for tested components
set(COMPONENT_SOURCES
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_linker.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_optimizer.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_target.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_debug.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_runtime.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_codegen.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_type_mapper.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_control_flow.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_expression_builder.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_array_support.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_dict_support.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_class_support.cpp
    ${PROJECT_ROOT}/compiler/backends/llvm/llvm_memory_manager.cpp
)

# إنشاء برنامج الاختبار / Create test executable
add_executable(llvm_backend_tests
    ${TEST_SOURCES}
    ${COMPONENT_SOURCES}
)

# ربط المكتبات / Link libraries
target_link_libraries(llvm_backend_tests
    ${llvm_libs}
)

# إضافة الاختبارات إلى CTest / Add tests to CTest
enable_testing()

# اختبار جميع المكونات / Test all components
add_test(
    NAME AllTests
    COMMAND llvm_backend_tests
)

# اختبار الرابط / Test linker
add_test(
    NAME LinkerTests
    COMMAND llvm_backend_tests --suite LLVMLinker
)

# اختبار المُحسّن / Test optimizer
add_test(
    NAME OptimizerTests
    COMMAND llvm_backend_tests --suite LLVMOptimizer
)

# اختبار الهدف / Test target
add_test(
    NAME TargetTests
    COMMAND llvm_backend_tests --suite LLVMTarget
)

# اختبار التنقيح / Test debug
add_test(
    NAME DebugTests
    COMMAND llvm_backend_tests --suite LLVMDebug
)

# اختبار وقت التشغيل / Test runtime
add_test(
    NAME RuntimeTests
    COMMAND llvm_backend_tests --suite LLVMRuntime
)

# اختبار مولد الكود / Test code generator
add_test(
    NAME CodeGenTests
    COMMAND llvm_backend_tests --suite LLVMCodeGen
)

# اختبار التكامل / Test integration
add_test(
    NAME IntegrationTests
    COMMAND llvm_backend_tests --suite Integration
)

# تعيين خصائص الاختبار / Set test properties
set_tests_properties(AllTests PROPERTIES
    TIMEOUT 600
    LABELS "LLVM;Backend;All"
)

set_tests_properties(LinkerTests PROPERTIES
    TIMEOUT 60
    LABELS "LLVM;Backend;Linker"
)

set_tests_properties(OptimizerTests PROPERTIES
    TIMEOUT 120
    LABELS "LLVM;Backend;Optimizer"
)

set_tests_properties(TargetTests PROPERTIES
    TIMEOUT 120
    LABELS "LLVM;Backend;Target"
)

set_tests_properties(DebugTests PROPERTIES
    TIMEOUT 60
    LABELS "LLVM;Backend;Debug"
)

set_tests_properties(RuntimeTests PROPERTIES
    TIMEOUT 60
    LABELS "LLVM;Backend;Runtime"
)

set_tests_properties(CodeGenTests PROPERTIES
    TIMEOUT 120
    LABELS "LLVM;Backend;CodeGen"
)

set_tests_properties(IntegrationTests PROPERTIES
    TIMEOUT 180
    LABELS "LLVM;Backend;Integration"
)

# طباعة معلومات / Print information
message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
message(STATUS "تم تكوين اختبارات LLVM Backend بنجاح")
message(STATUS "LLVM Backend Tests configured successfully")
message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
message(STATUS "")
message(STATUS "لتشغيل الاختبارات / To run tests:")
message(STATUS "  cd build")
message(STATUS "  ctest --output-on-failure")
message(STATUS "")
message(STATUS "أو / Or:")
message(STATUS "  ./llvm_backend_tests")
message(STATUS "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━")
