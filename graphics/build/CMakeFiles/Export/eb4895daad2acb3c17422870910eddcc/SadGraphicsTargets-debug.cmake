#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Sad::sad_graphics" for configuration "Debug"
set_property(TARGET Sad::sad_graphics APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(Sad::sad_graphics PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "C;CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/sad_graphics.lib"
  )

list(APPEND _cmake_import_check_targets Sad::sad_graphics )
list(APPEND _cmake_import_check_files_for_Sad::sad_graphics "${_IMPORT_PREFIX}/lib/sad_graphics.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
