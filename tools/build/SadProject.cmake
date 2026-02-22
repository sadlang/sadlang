# بسم الله الرحمن الرحيم
# SadProject.cmake - Project template for Sad language
# قالب مشروع للغة Sad
# Phase 7.3: Build System Integration

#[=======================================================================[.rst:
SadProject
----------

Helper module for creating Sad language projects.

Functions
^^^^^^^^^

``sad_project``
  Initialize a Sad project with standard settings.

``sad_add_sources``
  Add source files to the project.

``sad_add_dependencies``
  Add dependencies to the project.

``sad_install``
  Install project outputs.

Example Usage
^^^^^^^^^^^^^

.. code-block:: cmake

  cmake_minimum_required(VERSION 3.15)
  
  # Find Sad
  find_package(Sad REQUIRED)
  
  # Initialize project
  sad_project(MyProject
    VERSION 1.0.0
    DESCRIPTION "My Sad project"
    LANGUAGES Sad
  )
  
  # Add sources
  sad_add_sources(
    src/main.s
    src/utils.s
    src/math.s
  )
  
  # Build executable
  add_sad_executable(myprogram
    SOURCES ${PROJECT_SOURCES}
    OPTIMIZATION O3
  )
  
  # Install
  sad_install(TARGETS myprogram)

#]=======================================================================]

include(CMakeParseArguments)

# ============================================================================
# الحمد لله - Project initialization / تهيئة المشروع
# ============================================================================

#[=======================================================================[.rst:
.. command:: sad_project

  Initialize a Sad language project.
  
  .. code-block:: cmake
  
    sad_project(<name>
      [VERSION <major.minor.patch>]
      [DESCRIPTION <description>]
      [HOMEPAGE_URL <url>]
      [LANGUAGES <language1> [<language2>...]]
    )
#]=======================================================================]
function(sad_project PROJECT_NAME)
    # Parse arguments
    set(options)
    set(oneValueArgs VERSION DESCRIPTION HOMEPAGE_URL)
    set(multiValueArgs LANGUAGES)
    cmake_parse_arguments(PROJ "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Set defaults
    if(NOT PROJ_VERSION)
        set(PROJ_VERSION "1.0.0")
    endif()
    
    if(NOT PROJ_LANGUAGES)
        set(PROJ_LANGUAGES Sad CXX)
    endif()
    
    # Create project
    project(${PROJECT_NAME}
        VERSION ${PROJ_VERSION}
        DESCRIPTION "${PROJ_DESCRIPTION}"
        HOMEPAGE_URL "${PROJ_HOMEPAGE_URL}"
        LANGUAGES ${PROJ_LANGUAGES}
    )
    
    # Set standard properties
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib PARENT_SCOPE)
    
    # Set Sad-specific properties
    set(SAD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src PARENT_SCOPE)
    set(SAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)
    set(SAD_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests PARENT_SCOPE)
    set(SAD_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
    
    # Print project info
    message(STATUS "═══════════════════════════════════════════════")
    message(STATUS "الحمد لله - Sad Project: ${PROJECT_NAME}")
    message(STATUS "Version: ${PROJ_VERSION}")
    if(PROJ_DESCRIPTION)
        message(STATUS "Description: ${PROJ_DESCRIPTION}")
    endif()
    message(STATUS "═══════════════════════════════════════════════")
endfunction()

# ============================================================================
# Source management / إدارة ملفات المصدر
# ============================================================================

#[=======================================================================[.rst:
.. command:: sad_add_sources

  Add source files to the project source list.
  
  .. code-block:: cmake
  
    sad_add_sources(<file1> [<file2>...])
#]=======================================================================]
function(sad_add_sources)
    # Add to PROJECT_SOURCES variable
    set(PROJECT_SOURCES ${PROJECT_SOURCES} ${ARGN} PARENT_SCOPE)
    
    message(STATUS "Added ${ARGC} Sad source file(s)")
endfunction()

# ============================================================================
# Dependency management / إدارة التبعيات
# ============================================================================

#[=======================================================================[.rst:
.. command:: sad_add_dependencies

  Add dependencies to the project.
  
  .. code-block:: cmake
  
    sad_add_dependencies(<dep1> [<dep2>...])
#]=======================================================================]
function(sad_add_dependencies)
    foreach(dep ${ARGN})
        find_package(${dep} REQUIRED)
        message(STATUS "Found dependency: ${dep}")
    endforeach()
endfunction()

# ============================================================================
# Installation / التثبيت
# ============================================================================

#[=======================================================================[.rst:
.. command:: sad_install

  Install project outputs.
  
  .. code-block:: cmake
  
    sad_install(
      TARGETS <target1> [<target2>...]
      [DESTINATION <dir>]
      [RUNTIME_DESTINATION <dir>]
      [LIBRARY_DESTINATION <dir>]
      [ARCHIVE_DESTINATION <dir>]
    )
#]=======================================================================]
function(sad_install)
    # Parse arguments
    set(options)
    set(oneValueArgs DESTINATION RUNTIME_DESTINATION LIBRARY_DESTINATION ARCHIVE_DESTINATION)
    set(multiValueArgs TARGETS)
    cmake_parse_arguments(INSTALL "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Set defaults
    if(NOT INSTALL_RUNTIME_DESTINATION)
        set(INSTALL_RUNTIME_DESTINATION bin)
    endif()
    
    if(NOT INSTALL_LIBRARY_DESTINATION)
        set(INSTALL_LIBRARY_DESTINATION lib)
    endif()
    
    if(NOT INSTALL_ARCHIVE_DESTINATION)
        set(INSTALL_ARCHIVE_DESTINATION lib)
    endif()
    
    # Install each target
    foreach(target ${INSTALL_TARGETS})
        get_target_property(OUTPUT_NAME ${target} OUTPUT_NAME)
        
        if(OUTPUT_NAME)
            install(
                FILES ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_NAME}
                DESTINATION ${INSTALL_RUNTIME_DESTINATION}
                PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                            GROUP_READ GROUP_EXECUTE
                            WORLD_READ WORLD_EXECUTE
            )
            message(STATUS "Will install: ${target} → ${INSTALL_RUNTIME_DESTINATION}")
        endif()
    endforeach()
endfunction()

# ============================================================================
# Testing support / دعم الاختبارات
# ============================================================================

#[=======================================================================[.rst:
.. command:: sad_enable_testing

  Enable testing for the project.
  
  .. code-block:: cmake
  
    sad_enable_testing()
#]=======================================================================]
macro(sad_enable_testing)
    enable_testing()
    message(STATUS "الحمد لله - Testing enabled for Sad project")
endmacro()

# ============================================================================
# Documentation support / دعم التوثيق
# ============================================================================

#[=======================================================================[.rst:
.. command:: sad_add_documentation

  Add documentation generation target.
  
  .. code-block:: cmake
  
    sad_add_documentation(
      [SOURCES <file1> [<file2>...]]
      [OUTPUT_DIR <dir>]
    )
#]=======================================================================]
function(sad_add_documentation)
    # Parse arguments
    set(options)
    set(oneValueArgs OUTPUT_DIR)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(DOC "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
    
    # Set defaults
    if(NOT DOC_OUTPUT_DIR)
        set(DOC_OUTPUT_DIR ${CMAKE_BINARY_DIR}/docs)
    endif()
    
    # Create documentation target
    add_custom_target(docs
        COMMAND ${CMAKE_COMMAND} -E echo "الحمد لله - Generating documentation..."
        COMMAND ${CMAKE_COMMAND} -E make_directory ${DOC_OUTPUT_DIR}
        COMMENT "Generating Sad project documentation"
    )
    
    message(STATUS "Documentation target added: make docs")
endfunction()

# ============================================================================
# الحمد لله - Module complete / الوحدة مكتملة
# ============================================================================
