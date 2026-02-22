# SQLite3Config.cmake - تهيئة SQLite3 المُدمجة
# يُستخدم هذا الملف من قِبل find_package(SQLite3)

set(SQLite3_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}")
set(SQLite3_LIBRARIES    "")  # يُضاف لاحقاً عبر add_subdirectory

if(NOT TARGET SQLite3::SQLite3)
    add_library(sqlite3_amalgamation STATIC 
        "${CMAKE_CURRENT_LIST_DIR}/sqlite3.c"
    )
    target_include_directories(sqlite3_amalgamation PUBLIC 
        "${CMAKE_CURRENT_LIST_DIR}"
    )
    if(MSVC)
        target_compile_options(sqlite3_amalgamation PRIVATE /W0)
        target_compile_definitions(sqlite3_amalgamation PRIVATE 
            _CRT_SECURE_NO_WARNINGS 
            SQLITE_THREADSAFE=1
        )
    endif()
    set_target_properties(sqlite3_amalgamation PROPERTIES 
        POSITION_INDEPENDENT_CODE ON
    )

    add_library(SQLite3::SQLite3 ALIAS sqlite3_amalgamation)
    set(SQLite3_LIBRARIES sqlite3_amalgamation)
endif()

set(SQLite3_FOUND TRUE)
