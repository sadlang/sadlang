#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Sad::sad_graphics" for configuration "RelWithDebInfo"
set_property(TARGET Sad::sad_graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(Sad::sad_graphics PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C;CXX"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/sad_graphics.lib"
  )

list(APPEND _cmake_import_check_targets Sad::sad_graphics )
list(APPEND _cmake_import_check_files_for_Sad::sad_graphics "${_IMPORT_PREFIX}/lib/sad_graphics.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
