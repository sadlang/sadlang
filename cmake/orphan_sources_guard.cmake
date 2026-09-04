# ============================================================================
# orphan_sources_guard.cmake — حارس الملفات اليتيمة
# ============================================================================
#
#      vm/src) مذكور في خصائص SOURCES لأي target مُسجَّل. يفشل البناء إن وُجد
#      ملف يتيم (موجود على القرص لكن لا يُربط بأي مكتبة/تنفيذي).
#
# (EN) Verifies that every .cpp/.cc under guarded directories is referenced by
#      some target's SOURCES. Fails configure step on orphans.
#
# الاستخدام / Usage:
#   include(cmake/orphan_sources_guard.cmake)
#   sad_check_orphan_sources()  # يُستدعى في نهاية CMakeLists الرئيسي
#
# للتعطيل المؤقت / Temporary disable:
#   set(SAD_SKIP_ORPHAN_GUARD ON)
#
# لإعادة قائمة بدلاً من الفشل / Warning-only mode:
#   set(SAD_ORPHAN_GUARD_MODE "WARN")  # القيم: "FATAL" (افتراضي) أو "WARN"
# ============================================================================

# (AR) المسارات المحروسة (نسبية لجذر المشروع)
# (EN) Guarded paths (relative to project root)
set(SAD_GUARDED_DIRS
    "compiler/src"
)

# (AR) أنماط استثناء — ملفات معروفة أنها قوالب/توليد/أرشفة
# (EN) Exclusion patterns — known templates/generated/archived files
set(SAD_ORPHAN_EXCLUDE_PATTERNS
    # (AR) /archived/ حُذف فعليًّا في م5 (RFC sadlang-rfcs#10)؛ يبقى النمط دفاعيًّا لأيّ أرشفة مستقبليّة
    # (EN) /archived/ was deleted in phase 5 (RFC #10); pattern kept defensively for any future archiving
    "/archived/"
    "/_archive/"
    "/templates/"
    "/generated/"
    "\\.template\\.cpp$"
    "\\.in\\.cpp$"
)

# ----------------------------------------------------------------------------
# دالة: جمع جميع الـ targets المُعرَّفة (بما في ذلك في subdirectories)
# Function: collect all defined targets recursively
# ----------------------------------------------------------------------------
function(_sad_collect_all_targets out_var)
    set(_targets "")
    _sad_collect_targets_recursive(_targets "${CMAKE_SOURCE_DIR}")
    set(${out_var} "${_targets}" PARENT_SCOPE)
endfunction()

function(_sad_collect_targets_recursive out_var dir)
    get_property(_subdirs DIRECTORY "${dir}" PROPERTY SUBDIRECTORIES)
    get_property(_dir_targets DIRECTORY "${dir}" PROPERTY BUILDSYSTEM_TARGETS)
    set(_collected "${${out_var}}")
    list(APPEND _collected ${_dir_targets})
    foreach(_sub IN LISTS _subdirs)
        _sad_collect_targets_recursive(_collected "${_sub}")
    endforeach()
    set(${out_var} "${_collected}" PARENT_SCOPE)
endfunction()

# ----------------------------------------------------------------------------
# الدالة الرئيسية: فحص الملفات اليتيمة
# Main: check for orphan source files
# ----------------------------------------------------------------------------
function(sad_check_orphan_sources)
    if(SAD_SKIP_ORPHAN_GUARD)
        message(STATUS "[orphan-guard] تخطٍ مُفعَّل (SAD_SKIP_ORPHAN_GUARD=ON)")
        return()
    endif()

    if(NOT DEFINED SAD_ORPHAN_GUARD_MODE)
        set(SAD_ORPHAN_GUARD_MODE "FATAL")
    endif()

    # (AR) عند إيقاف LLVM (مثل Windows Debug على CI حيث لا يتوفّر LLVM Debug)،
    #      مصادر مولّد LLVM في compiler/src/backend/ لا تُضاف لأيّ هدف شرعًا،
    #      فلا يجوز عدّها «يتيمة». نستثنيها ديناميكيًا حسب ENABLE_LLVM_BACKEND.
    # (EN) When LLVM is OFF (e.g. CI Windows Debug, no Debug LLVM), the LLVM
    #      codegen sources under compiler/src/backend/ are legitimately not added
    #      to any target, so they must not be flagged as orphans. Exclude them
    #      dynamically based on ENABLE_LLVM_BACKEND.
    if(NOT ENABLE_LLVM_BACKEND)
        list(APPEND SAD_ORPHAN_EXCLUDE_PATTERNS "/compiler/src/backend/")
    endif()

    # (AR) عند إطفاء الرسومات (بناء headless/initramfs)، مصادر جسر الواجهات
    #      (INTERPRETER_UI_BRIDGE_SOURCES) لا تُضاف لأيّ هدف شرعًا فلا تُعدّ يتيمة.
    # (EN) When graphics is OFF (headless/initramfs), the UI-bridge sources are
    #      legitimately not added to any target, so they must not be flagged.
    # (AR) 🔑 شجرةُ المفسّرِ حُذفت، ومعها مصادرُ جسرِ الواجهات — فلا استثناءَ يُذكر.

    # (1) جمع جميع الـ targets ومصادرها
    _sad_collect_all_targets(_all_targets)
    set(_referenced_sources "")

    foreach(_tgt IN LISTS _all_targets)
        if(NOT TARGET ${_tgt})
            continue()
        endif()
        get_target_property(_tgt_type ${_tgt} TYPE)
        if(_tgt_type STREQUAL "INTERFACE_LIBRARY")
            continue()
        endif()
        get_target_property(_srcs ${_tgt} SOURCES)
        if(_srcs)
            get_target_property(_src_dir ${_tgt} SOURCE_DIR)
            foreach(_s IN LISTS _srcs)
                # (AR) تطبيع المسار إلى absolute
                if(IS_ABSOLUTE "${_s}")
                    set(_abs "${_s}")
                else()
                    set(_abs "${_src_dir}/${_s}")
                endif()
                get_filename_component(_norm "${_abs}" ABSOLUTE)
                list(APPEND _referenced_sources "${_norm}")
            endforeach()
        endif()
    endforeach()

    list(REMOVE_DUPLICATES _referenced_sources)

    # (2) جمع جميع الملفات الفعلية في المسارات المحروسة
    set(_actual_sources "")
    foreach(_dir IN LISTS SAD_GUARDED_DIRS)
        set(_full_dir "${CMAKE_SOURCE_DIR}/${_dir}")
        if(NOT EXISTS "${_full_dir}")
            continue()
        endif()
        file(GLOB_RECURSE _found
            "${_full_dir}/*.cpp"
            "${_full_dir}/*.cc"
            "${_full_dir}/*.cxx"
        )
        foreach(_f IN LISTS _found)
            set(_skip OFF)
            foreach(_pat IN LISTS SAD_ORPHAN_EXCLUDE_PATTERNS)
                if(_f MATCHES "${_pat}")
                    set(_skip ON)
                    break()
                endif()
            endforeach()
            if(NOT _skip)
                list(APPEND _actual_sources "${_f}")
            endif()
        endforeach()
    endforeach()

    # (3) تحديد اليتامى (موجود فعلياً + غير مذكور في أي target)
    set(_orphans "")
    foreach(_f IN LISTS _actual_sources)
        list(FIND _referenced_sources "${_f}" _idx)
        if(_idx EQUAL -1)
            list(APPEND _orphans "${_f}")
        endif()
    endforeach()

    # (4) التقرير
    list(LENGTH _actual_sources _total)
    list(LENGTH _orphans _orphan_count)
    list(LENGTH _referenced_sources _ref_count)

    message(STATUS "")
    message(STATUS "════════════════════════════════════════════════════════════")
    message(STATUS "  حارس الملفات اليتيمة / Orphan Sources Guard")
    message(STATUS "════════════════════════════════════════════════════════════")
    message(STATUS "  الملفات المفحوصة      : ${_total}")
    message(STATUS "  مراجع الـ targets     : ${_ref_count}")
    message(STATUS "  ملفات يتيمة           : ${_orphan_count}")
    message(STATUS "  الوضع                : ${SAD_ORPHAN_GUARD_MODE}")
    message(STATUS "════════════════════════════════════════════════════════════")

    if(_orphan_count GREATER 0)
        message(STATUS "")
        message(STATUS "الملفات اليتيمة المُكتشفة:")
        foreach(_o IN LISTS _orphans)
            file(RELATIVE_PATH _rel "${CMAKE_SOURCE_DIR}" "${_o}")
            message(STATUS "  ❌ ${_rel}")
        endforeach()
        message(STATUS "")
        message(STATUS "كل ملف أعلاه إما:")
        message(STATUS "  • يجب إضافته إلى مكتبة/تنفيذي عبر add_library/add_executable")
        message(STATUS "  • أو حذفه إن كان كوداً ميتاً")
        message(STATUS "  • أو أرشفته/حذفه مع توثيق السبب (مجلّد archived/ أُزيل في م5، RFC sadlang-rfcs#10)")
        message(STATUS "")

        if(SAD_ORPHAN_GUARD_MODE STREQUAL "FATAL")
            message(FATAL_ERROR
                "[orphan-guard] فشل البناء: ${_orphan_count} ملف يتيم في مسارات محروسة.\n"
                "  لتعطيل مؤقت: cmake -DSAD_SKIP_ORPHAN_GUARD=ON\n"
                "  لتحويل لتحذير: cmake -DSAD_ORPHAN_GUARD_MODE=WARN")
        else()
            message(WARNING "[orphan-guard] ${_orphan_count} ملف يتيم (وضع تحذير)")
        endif()
    else()
        message(STATUS "  ✅ لا ملفات يتيمة — جميع المصادر مربوطة بـ targets")
        message(STATUS "")
    endif()
endfunction()
