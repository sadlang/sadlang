# Story 1.3 — ربط CMake (code_standards_guard.cmake + CMakeLists.txt)

**Epic:** 1 — نظام Contract-as-Code
**Story:** 1.3
**الحالة:** backlog
**الأولوية:** P0 (حرج)
**الاعتمادية:** Story 1.1 + 1.2 مكتملتان
**المرجع:** `prd-contract-as-code.md` § FR-03

---

## وصف القصة

**كمطوِّر في لغة ص،**
أريد أن يفشل `cmake -B build` تلقائياً إذا وُجدت مخالفة في الكود،
**حتى** يُجبَر أي مطور أو وكيل AI على إصلاح المخالفة قبل البدء بأي compilation.

---

## معايير القبول (AC)

### AC-1.3.1: ملف cmake/code_standards_guard.cmake
- [ ] يستدعي `check_code_standards.py` عبر `execute_process()`
- [ ] يُمرر مسارات: `compiler/src`, `interpreter/src`, `vm/src`, `shared/`
- [ ] يُفشل configure بـ `message(FATAL_ERROR ...)` عند exit code != 0
- [ ] يدعم `SAD_SKIP_STANDARDS_GUARD=ON` لتعطيل الفحص
- [ ] يدعم `SAD_STANDARDS_GUARD_MODE=WARN` لوضع التحذير فقط

### AC-1.3.2: تكامل CMakeLists.txt
- [ ] `include(cmake/code_standards_guard.cmake)` مُضاف بعد `include(cmake/orphan_sources_guard.cmake)`
- [ ] `sad_check_code_standards()` يُستدعى في نهاية `CMakeLists.txt`
- [ ] `add_dependencies(sad sad_code_standards_check)` مُضاف
- [ ] `add_dependencies(sadc sad_code_standards_check)` مُضاف (إذا وُجد target sadc)

### AC-1.3.3: تنسيق رسالة الخطأ
- [ ] الرسالة تتضمن: `━━━` separator + تقرير المخالفات + تعليمات التجاوز
- [ ] كل مخالفة: `[CS-XX] path:LINE` + المشكلة + الحل
- [ ] رسالة ختامية: عدد المخالفات + `cmake -DSAD_SKIP_STANDARDS_GUARD=ON`

### AC-1.3.4: الأداء
- [ ] وقت configure الإضافي < 3 ثوانٍ (مقيس على المشروع كاملاً)

---

## المهام التقنية

### Task 1: إنشاء cmake/code_standards_guard.cmake
```cmake
# الدالة الرئيسية
function(sad_check_code_standards)
    if(SAD_SKIP_STANDARDS_GUARD)
        return()
    endif()
    
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    
    execute_process(
        COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_SOURCE_DIR}/scripts/lint/check_code_standards.py"
            --dirs compiler/src interpreter/src vm/src shared/
            --spec "${CMAKE_SOURCE_DIR}/scripts/lint/code_standards.yaml"
            --allowlist "${CMAKE_SOURCE_DIR}/scripts/lint/code_standards_allowlist.txt"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        RESULT_VARIABLE _exit_code
        OUTPUT_VARIABLE _output
        ERROR_VARIABLE _error
    )
    
    if(NOT _exit_code EQUAL 0)
        if(SAD_STANDARDS_GUARD_MODE STREQUAL "WARN")
            message(WARNING "${_output}${_error}")
        else()
            message(FATAL_ERROR "\n${_output}${_error}")
        endif()
    endif()
endfunction()
```

### Task 2: تعديل CMakeLists.txt
- إضافة `include` بعد `orphan_sources_guard.cmake`
- إضافة `sad_check_code_standards()` في نهاية الملف
- إضافة `add_dependencies`

### Task 3: إنشاء scripts/lint/code_standards_allowlist.txt
```
# (AR) الملفات المستثناة مؤقتاً — قيد الترحيل
# (EN) Temporarily excluded files — pending migration
interpreter/src/visitors/expression_evaluator_ui.cpp
```

---

## سياق التنفيذ

### النموذج الموجود
```cmake
# cmake/orphan_sources_guard.cmake — نمط مطابق
# يُستدعى في CMakeLists.txt:210
include(${CMAKE_SOURCE_DIR}/cmake/orphan_sources_guard.cmake)
# ...
sad_check_orphan_sources()
```

### ملاحظة مهمة
- استخدم `find_package(Python3)` وليس `python` مباشرة (cross-platform)
- `WORKING_DIRECTORY` ضروري لأن السكريبت يستخدم مسارات نسبية
- اختبر على Windows (الجهاز الحالي) أن Python3 موجود في PATH

---

## قائمة الملفات المُنشأة/المُعدَّلة
- [ ] `cmake/code_standards_guard.cmake` ← جديد
- [ ] `scripts/lint/code_standards_allowlist.txt` ← جديد
- [ ] `CMakeLists.txt` ← تعديل (include + استدعاء)

---

## Dev Agent Record
*(تُملأ من قبل وكيل التطوير أثناء التنفيذ)*

**ما تم تنفيذه:**

**الاختبارات التي أُنشئت:**

**القرارات المتخذة:**

---
