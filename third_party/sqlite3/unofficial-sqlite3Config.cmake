# unofficial-sqlite3Config.cmake
# يُستخدم هذا الملف من قِبل find_package(unofficial-sqlite3 CONFIG)

if(NOT TARGET unofficial::sqlite3::sqlite3)
    add_library(sqlite3_amalgamation_u STATIC 
        "${CMAKE_CURRENT_LIST_DIR}/sqlite3.c"
    )
    target_include_directories(sqlite3_amalgamation_u PUBLIC 
        "${CMAKE_CURRENT_LIST_DIR}"
    )
    if(MSVC)
        target_compile_options(sqlite3_amalgamation_u PRIVATE /W0)
        target_compile_definitions(sqlite3_amalgamation_u PRIVATE 
            _CRT_SECURE_NO_WARNINGS 
            SQLITE_THREADSAFE=1
        )
    endif()
    set_target_properties(sqlite3_amalgamation_u PROPERTIES 
        POSITION_INDEPENDENT_CODE ON
    )
    add_library(unofficial::sqlite3::sqlite3 ALIAS sqlite3_amalgamation_u)
endif()

set(unofficial-sqlite3_FOUND TRUE)
