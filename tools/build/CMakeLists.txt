# بسم الله الرحمن الرحيم
cmake_minimum_required(VERSION 3.15)
project(sad_build VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
set(SOURCES
    src/build_system.cpp
    src/config_parser.cpp
    src/task_runner.cpp
    src/builder.cpp
    src/cache_manager.cpp
    src/watcher.cpp
)

# Header files
set(HEADERS
    include/build_system.h
    include/config_parser.h
    include/task_runner.h
    include/builder.h
    include/cache_manager.h
    include/watcher.h
)

# Build library
add_library(sad_build_lib STATIC ${SOURCES} ${HEADERS})
target_include_directories(sad_build_lib PUBLIC include)

# Find OpenSSL for SHA-256
find_package(OpenSSL)
if(OpenSSL_FOUND)
    target_link_libraries(sad_build_lib PUBLIC OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(sad_build_lib PUBLIC HAS_OPENSSL)
else()
    message(WARNING "OpenSSL not found. Cache hashing will be disabled.")
endif()

# Build executable
add_executable(sad main.cpp)
target_link_libraries(sad PRIVATE sad_build_lib)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(sad_build_lib PUBLIC _WIN32)
else()
    # Unix-specific settings
    target_link_libraries(sad_build_lib PUBLIC pthread)
endif()

# Install targets
install(TARGETS sad
    RUNTIME DESTINATION bin
)

install(TARGETS sad_build_lib
    ARCHIVE DESTINATION lib
)

install(FILES ${HEADERS}
    DESTINATION include/sad/build
)

# Tests (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Print configuration
message(STATUS "Sad Build System Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  OpenSSL: ${OpenSSL_FOUND}")
message(STATUS "  Build Tests: ${BUILD_TESTS}")
