/**
 * @file builtin_common.h
 * @brief (AR) ملف رأس موحد للدوال المضمنة — يجمع الـ includes المشتركة بين جميع ملفات builtins
 * @brief (EN) Unified header for builtin modules — aggregates common includes shared by all builtin files
 *
 * بدلاً من تكرار 11 project include في كل ملف builtin، يتم تضمين هذا الملف مرة واحدة.
 * يحتوي على جميع الـ headers المشتركة بين ملفات builtin_module_*.cpp
 */

#pragma once

// ─── Project Headers (المشتركة بين جميع الوحدات) ───
#include "builtins.h"
#include "interpreter_core.h"
#include "io/io_functions.h"
#include "type_functions.h"
#include "array_functions.h"
#include "other_functions.h"
#include "string/string_functions.h"
#include "math/math_functions.h"
#include "math/advanced_math.h"
#include "system/system_functions.h"
#include "filesystem/filesystem_module.h"

// ─── Standard Headers (الأساسية المشتركة) ───
#include <memory>
#include <string>
#include <vector>
