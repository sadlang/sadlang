# بسم الله الرحمن الرحيم
# FindSad.cmake - CMake module for Sad language
# وحدة CMake للغة Sad
# Phase 7.3: Build System Integration

#[=======================================================================[.rst:
FindSad
-------

Finds the Sad compiler (sad-build).

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``Sad_FOUND``
  True if the Sad compiler is available.

``SADC_EXECUTABLE``
  Path to the sadc executable.

``Sad_VERSION``
  Version of the Sad compiler.

``Sad_VERSION_MAJOR``
  Major version number.

``Sad_VERSION_MINOR``
  Minor version number.

``Sad_VERSION_PATCH``
  Patch version number.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``SADC_EXECUTABLE``
  Path to the sadc executable.

Functions
^^^^^^^^^

``add_sad_executable``
  Compile Sad source files into an executable.

``add_sad_library``
  Compile Sad source files into a library.

``sad_add_test``
  Add a test for Sad code.

Example Usage
^^^^^^^^^^^^^

.. code-block:: cmake

  find_package(Sad REQUIRED)
  
  # Add executable
  add_sad_executable(myprogram
    SOURCES src/main.s src/utils.s
    OUTPUT myprogram
  )
  
  # Add library
  add_sad_library(mylib
    SOURCES src/lib.s
    TYPE SHARED
    OUTPUT libmylib.so
  )
  
  # Add test
  sad_add_test(test_myprogram
    SOURCE tests/test.s
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

#]=======================================================================]

# ============================================================================
# الحمد لله - Find sad-build executable / البحث عن المترجم
# ============================================================================

# (AR) اسمُ الهدف مُوحَّدٌ مع اسم المُخرَج: sad-build ⇒ sad-build.exe. والاسمُ
#      `sadc` متقاعد — لا يُنتجه أيُّ هدفٍ في CMakeLists، فكان هذا الباحثُ
#      يطلب ثنائيًّا لا وجودَ له ويفشل صامتًا. يُبقى `sadc` آخرَ القائمة
#      لتثبيتاتٍ قديمةٍ على أقراص المستخدمين فقط.
# (EN) Target name is unified with output name: sad-build ⇒ sad-build.exe.
#      `sadc` is retired — no target produces it; kept last for old installs.
find_program(SADC_EXECUTABLE
    NAMES sad-build sad-build.exe sadc sadc.exe
    PATHS
        # User-specified paths
        ${Sad_ROOT}
        ${Sad_ROOT}/bin
        $ENV{Sad_ROOT}
        $ENV{Sad_ROOT}/bin
        # Standard installation paths
        /usr/local/bin
        /usr/bin
        /opt/sad/bin
        # Windows paths
        "C:/Program Files/Sad/bin"
        "C:/Sad/bin"
    DOC "Path to the Sad compiler (sad-build)"
)

# ============================================================================
# Get version information / الحصول على معلومات الإصدار
# ============================================================================

if(SADC_EXECUTABLE)
    # (AR) العَلَمُ القانونيُّ الوحيد `--إصدار` (language-truth/cli_flags.yaml:
    #      flag.version)، و`sad-build` أحدُ المحرّكات الملزَمة به — يحرسه
    #      scripts/ci/check_version_flags.py. وكان هنا `--version`: خيارٌ غير
    #      معروفٍ يبتلعه ERROR_QUIET، فتبقى Sad_VERSION فارغةً والكتلةُ كلُّها
    #      ميّتةٌ صامتة.
    # (EN) The only canonical flag is `--إصدار`; `--version` is not accepted and
    #      ERROR_QUIET swallowed the failure, leaving Sad_VERSION empty.
    execute_process(
        COMMAND ${SADC_EXECUTABLE} --إصدار
        OUTPUT_VARIABLE Sad_VERSION_OUTPUT
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    
    # Parse version string
    # (AR) المخرَجُ مقيسٌ من المصدر لا مفترَضًا: get_compiler_name() تعيد
    #      "sadc - Sad Compiler" وget_version() تعيد "1.0.0"
    #      (tools/compiler/compiler_driver.h:588-589). فالنصُّ هنا يطابق
    #      **الثنائيَّ كما هو اليوم**، لا اسمَ الهدفِ في CMake. ولا تُضَف مجموعةُ
    #      تبديلٍ هنا: كلُّ `()` تُزحزح CMAKE_MATCH_N أدناه فيصير الإصدارُ
    #      "sadc.1.0" وتنكسر مقارنةُ find_package(Sad 1.0.0) صامتة.
    if(Sad_VERSION_OUTPUT MATCHES "sadc - Sad Compiler ([0-9]+)\\.([0-9]+)\\.([0-9]+)")
        set(Sad_VERSION_MAJOR ${CMAKE_MATCH_1})
        set(Sad_VERSION_MINOR ${CMAKE_MATCH_2})
        set(Sad_VERSION_PATCH ${CMAKE_MATCH_3})
        set(Sad_VERSION "${Sad_VERSION_MAJOR}.${Sad_VERSION_MINOR}.${Sad_VERSION_PATCH}")
    endif()
endif()

# ============================================================================
# Standard CMake find_package handling
# ============================================================================

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sad
    REQUIRED_VARS SADC_EXECUTABLE
    VERSION_VAR Sad_VERSION
    FOUND_VAR Sad_FOUND
)

mark_as_advanced(SADC_EXECUTABLE)

# ============================================================================
# Functions / الدوال المساعدة
# ============================================================================

if(Sad_FOUND)

    #[=======================================================================[.rst:
    .. command:: add_sad_executable
    
      Add an executable compiled from Sad source files.
      
      .. code-block:: cmake
      
        add_sad_executable(<target>
          SOURCES <source1> [<source2>...]
          [OUTPUT <output_name>]
          [OPTIMIZATION <O0|O1|O2|O3|Os|Oz>]
          [DEBUG]
          [VERBOSE]
          [OPTIONS <option1> [<option2>...]]
        )
      
      Arguments:
        ``SOURCES``
          List of Sad source files (.s or .sad).
        
        ``OUTPUT``
          Name of the output executable (default: <target>).
        
        ``OPTIMIZATION``
          Optimization level (default: O2).
        
        ``DEBUG``
          Generate debug information (-g).
        
        ``VERBOSE``
          Enable verbose output (-v).
        
        ``OPTIONS``
          Additional options to pass to sadc.
    #]=======================================================================]
    function(add_sad_executable TARGET)
        # Parse arguments
        set(options DEBUG VERBOSE)
        set(oneValueArgs OUTPUT OPTIMIZATION)
        set(multiValueArgs SOURCES OPTIONS)
        cmake_parse_arguments(SAD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
        
        # Check required arguments
        if(NOT SAD_SOURCES)
            message(FATAL_ERROR "add_sad_executable: SOURCES is required")
        endif()
        
        # Set defaults
        if(NOT SAD_OUTPUT)
            set(SAD_OUTPUT ${TARGET})
        endif()
        
        if(NOT SAD_OPTIMIZATION)
            set(SAD_OPTIMIZATION O2)
        endif()
        
        # Build command
        set(COMPILE_COMMAND ${SADC_EXECUTABLE})
        
        # Add sources
        list(APPEND COMPILE_COMMAND ${SAD_SOURCES})
        
        # Add output
        list(APPEND COMPILE_COMMAND -o ${SAD_OUTPUT})
        
        # Add optimization
        list(APPEND COMPILE_COMMAND -${SAD_OPTIMIZATION})
        
        # Add debug flag
        if(SAD_DEBUG)
            list(APPEND COMPILE_COMMAND -g)
        endif()
        
        # Add verbose flag
        if(SAD_VERBOSE)
            list(APPEND COMPILE_COMMAND -v)
        endif()
        
        # Add custom options
        if(SAD_OPTIONS)
            list(APPEND COMPILE_COMMAND ${SAD_OPTIONS})
        endif()
        
        # Add custom command
        add_custom_command(
            OUTPUT ${SAD_OUTPUT}
            COMMAND ${COMPILE_COMMAND}
            DEPENDS ${SAD_SOURCES}
            COMMENT "الحمد لله - Compiling Sad executable: ${TARGET}"
            VERBATIM
        )
        
        # Add custom target
        add_custom_target(${TARGET} ALL
            DEPENDS ${SAD_OUTPUT}
        )
        
        # Set properties
        set_target_properties(${TARGET} PROPERTIES
            OUTPUT_NAME ${SAD_OUTPUT}
        )
    endfunction()

    #[=======================================================================[.rst:
    .. command:: add_sad_library
    
      Add a library compiled from Sad source files.
      
      .. code-block:: cmake
      
        add_sad_library(<target>
          SOURCES <source1> [<source2>...]
          [TYPE <SHARED|STATIC>]
          [OUTPUT <output_name>]
          [OPTIMIZATION <O0|O1|O2|O3|Os|Oz>]
          [OPTIONS <option1> [<option2>...]]
        )
    #]=======================================================================]
    function(add_sad_library TARGET)
        # Parse arguments
        set(options)
        set(oneValueArgs OUTPUT OPTIMIZATION TYPE)
        set(multiValueArgs SOURCES OPTIONS)
        cmake_parse_arguments(SAD "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
        
        # Check required arguments
        if(NOT SAD_SOURCES)
            message(FATAL_ERROR "add_sad_library: SOURCES is required")
        endif()
        
        # Set defaults
        if(NOT SAD_TYPE)
            set(SAD_TYPE STATIC)
        endif()
        
        if(NOT SAD_OPTIMIZATION)
            set(SAD_OPTIMIZATION O2)
        endif()
        
        # Determine library extension
        if(SAD_TYPE STREQUAL "SHARED")
            if(WIN32)
                set(LIB_EXT ".dll")
            elseif(APPLE)
                set(LIB_EXT ".dylib")
            else()
                set(LIB_EXT ".so")
            endif()
            # (AR) الاسمُ القانونيُّ الوحيد (cli_flags.yaml · flag.shared)؛
            #      و`--shared` كان خيارًا غيرَ معروفٍ يُفشِلُ كلَّ
            #      sad_add_library(... SHARED). ويُضَمُّ أدناه إلى أمرٍ رأسُه
            #      ${SADC_EXECUTABLE}، فهو عَلَمُ مترجمٍ لا عَلَمُ CMake.
            # (EN) The only canonical name; --shared was an unknown option
            #      failing every sad_add_library(... SHARED).
            set(SHARED_FLAG --مشترك)
        else()
            if(WIN32)
                set(LIB_EXT ".lib")
            else()
                set(LIB_EXT ".a")
            endif()
            set(SHARED_FLAG)
        endif()
        
        # Set output name
        if(NOT SAD_OUTPUT)
            if(WIN32)
                set(SAD_OUTPUT ${TARGET}${LIB_EXT})
            else()
                set(SAD_OUTPUT lib${TARGET}${LIB_EXT})
            endif()
        endif()
        
        # Build command
        set(COMPILE_COMMAND ${SADC_EXECUTABLE})
        
        # Add sources
        list(APPEND COMPILE_COMMAND ${SAD_SOURCES})
        
        # Add output
        list(APPEND COMPILE_COMMAND -o ${SAD_OUTPUT})
        
        # Add optimization
        list(APPEND COMPILE_COMMAND -${SAD_OPTIMIZATION})
        
        # Add shared flag
        if(SHARED_FLAG)
            list(APPEND COMPILE_COMMAND ${SHARED_FLAG})
        endif()
        
        # Add custom options
        if(SAD_OPTIONS)
            list(APPEND COMPILE_COMMAND ${SAD_OPTIONS})
        endif()
        
        # Add custom command
        add_custom_command(
            OUTPUT ${SAD_OUTPUT}
            COMMAND ${COMPILE_COMMAND}
            DEPENDS ${SAD_SOURCES}
            COMMENT "الحمد لله - Compiling Sad library: ${TARGET}"
            VERBATIM
        )
        
        # Add custom target
        add_custom_target(${TARGET} ALL
            DEPENDS ${SAD_OUTPUT}
        )
    endfunction()

    #[=======================================================================[.rst:
    .. command:: sad_add_test
    
      Add a test for Sad code.
      
      .. code-block:: cmake
      
        sad_add_test(<name>
          SOURCE <source_file>
          [WORKING_DIRECTORY <dir>]
          [OPTIONS <option1> [<option2>...]]
        )
    #]=======================================================================]
    function(sad_add_test NAME)
        # Parse arguments
        set(options)
        set(oneValueArgs SOURCE WORKING_DIRECTORY)
        set(multiValueArgs OPTIONS)
        cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
        
        # Check required arguments
        if(NOT TEST_SOURCE)
            message(FATAL_ERROR "sad_add_test: SOURCE is required")
        endif()
        
        # Set defaults
        if(NOT TEST_WORKING_DIRECTORY)
            set(TEST_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        endif()
        
        # Create test executable name
        set(TEST_EXECUTABLE ${NAME}_test)
        
        # Compile test
        add_sad_executable(${TEST_EXECUTABLE}
            SOURCES ${TEST_SOURCE}
            OUTPUT ${TEST_EXECUTABLE}
            DEBUG
            ${TEST_OPTIONS}
        )
        
        # Add CTest test
        add_test(
            NAME ${NAME}
            COMMAND ${TEST_EXECUTABLE}
            WORKING_DIRECTORY ${TEST_WORKING_DIRECTORY}
        )
    endfunction()

    #[=======================================================================[.rst:
    .. command:: sad_compile_to_object
    
      Compile Sad source to object file.
      
      .. code-block:: cmake
      
        sad_compile_to_object(<source> <output>
          [OPTIMIZATION <O0|O1|O2|O3|Os|Oz>]
          [OPTIONS <option1> [<option2>...]]
        )
    #]=======================================================================]
    function(sad_compile_to_object SOURCE OUTPUT)
        # Parse arguments
        set(options)
        set(oneValueArgs OPTIMIZATION)
        set(multiValueArgs OPTIONS)
        cmake_parse_arguments(OBJ "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
        
        # Set defaults
        if(NOT OBJ_OPTIMIZATION)
            set(OBJ_OPTIMIZATION O2)
        endif()
        
        # Build command
        add_custom_command(
            OUTPUT ${OUTPUT}
            COMMAND ${SADC_EXECUTABLE} -c ${SOURCE} -o ${OUTPUT} -${OBJ_OPTIMIZATION} ${OBJ_OPTIONS}
            DEPENDS ${SOURCE}
            COMMENT "Compiling ${SOURCE} to object file"
            VERBATIM
        )
    endfunction()

endif()

# ============================================================================
# الحمد لله - Module complete / الوحدة مكتملة
# ============================================================================

if(Sad_FOUND)
    message(STATUS "الحمد لله - Found Sad compiler: ${SADC_EXECUTABLE}")
    if(Sad_VERSION)
        message(STATUS "  Version: ${Sad_VERSION}")
    endif()
endif()
