# CMakeLists.txt for SIR Optimizer
# ملف CMake لمحسّن SIR

cmake_minimum_required(VERSION 3.15)

# إعدادات المشروع / Project settings
set(OPTIMIZER_SOURCES
    src/optimizer.cpp
    src/pass.cpp
    src/pass_manager.cpp
    src/constant_folding_pass.cpp
    src/dead_code_elimination_pass.cpp
    src/cse_pass.cpp
    src/copy_propagation_pass.cpp
    src/register_coalescing_pass.cpp
)

set(OPTIMIZER_HEADERS
    include/optimizer.h
    include/pass.h
    include/pass_manager.h
    include/constant_folding_pass.h
    include/dead_code_elimination_pass.h
    include/cse_pass.h
    include/copy_propagation_pass.h
    include/register_coalescing_pass.h
)

# إنشاء مكتبة المحسن / Create optimizer library
add_library(sad_optimizer STATIC
    ${OPTIMIZER_SOURCES}
    ${OPTIMIZER_HEADERS}
)

# تحديد مجلدات التضمين / Specify include directories
target_include_directories(sad_optimizer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/compiler/frontend/include
    ${CMAKE_SOURCE_DIR}/compiler/sir/include
)

# ربط مع المكتبات المطلوبة / Link with required libraries
target_link_libraries(sad_optimizer PUBLIC
    # sad_sir  # سيتم إضافته عندما تكون مكتبة SIR جاهزة
)

# إعدادات C++17 / C++17 settings
target_compile_features(sad_optimizer PUBLIC cxx_std_17)

# إعدادات التصحيح / Debug settings
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(sad_optimizer PRIVATE
        SIR_OPTIMIZER_DEBUG
    )
endif()

# إعدادات التحذيرات / Warning settings
if(MSVC)
    target_compile_options(sad_optimizer PRIVATE /W4)
else()
    target_compile_options(sad_optimizer PRIVATE -Wall -Wextra -pedantic)
endif()

# رسالة الحالة / Status message
message(STATUS "SIR Optimizer library configured")
message(STATUS "  Sources: ${OPTIMIZER_SOURCES}")
message(STATUS "  Headers: ${OPTIMIZER_HEADERS}")
