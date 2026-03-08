// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  SadNotes — Bridging Header
//  جسر C++ ⟷ Swift
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SadNotes_Bridging_Header_h
#define SadNotes_Bridging_Header_h

// C Standard Library
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ─────────────────────────────────────────────────────────────────────
// الأنواع الأساسية / Basic Types
// ─────────────────────────────────────────────────────────────────────

typedef struct SadValue {
    int type;           // نوع القيمة
    int64_t intValue;   // قيمة رقمية
    double floatValue;  // قيمة عشرية
    const char* strValue; // قيمة نصية
    bool boolValue;     // قيمة منطقية
} SadValue;

typedef struct SadError {
    int code;
    const char* message;
    int line;
    int column;
} SadError;

// ─────────────────────────────────────────────────────────────────────
// تهيئة المحرك / Engine Initialization
// ─────────────────────────────────────────────────────────────────────

/// تهيئة محرك لغة ص
/// @return true إذا نجحت التهيئة
bool sad_init(void);

/// إنهاء محرك لغة ص وتحرير الموارد
void sad_shutdown(void);

/// الحصول على نسخة المحرك
const char* sad_version(void);

// ─────────────────────────────────────────────────────────────────────
// تنفيذ الكود / Code Execution
// ─────────────────────────────────────────────────────────────────────

/// تنفيذ كود لغة ص من نص
/// @param code الكود المراد تنفيذه
/// @param error مؤشر لتخزين الخطأ (يمكن أن يكون NULL)
/// @return النتيجة أو NULL إذا فشل
SadValue* sad_eval(const char* code, SadError* error);

/// تنفيذ ملف لغة ص
/// @param filepath مسار الملف
/// @param error مؤشر لتخزين الخطأ
/// @return النتيجة أو NULL إذا فشل
SadValue* sad_eval_file(const char* filepath, SadError* error);

/// تحرير قيمة
void sad_free_value(SadValue* value);

// ─────────────────────────────────────────────────────────────────────
// استدعاء الدوال / Function Calls
// ─────────────────────────────────────────────────────────────────────

/// استدعاء دالة معرّفة في لغة ص
/// @param name اسم الدالة
/// @param args المعاملات (مصفوفة)
/// @param argc عدد المعاملات
/// @param error مؤشر للخطأ
/// @return النتيجة
SadValue* sad_call(const char* name, SadValue* args[], int argc, SadError* error);

/// تسجيل دالة Swift ليتم استدعاؤها من لغة ص
/// @param name اسم الدالة في لغة ص
/// @param func مؤشر الدالة
/// @param userData بيانات المستخدم
typedef SadValue* (*SadNativeFunc)(SadValue* args[], int argc, void* userData);
bool sad_register_function(const char* name, SadNativeFunc func, void* userData);

// ─────────────────────────────────────────────────────────────────────
// UI Integration (للاستخدام المستقبلي)
// ─────────────────────────────────────────────────────────────────────

/// تسجيل عرض SwiftUI
typedef void* SadViewHandle;
bool sad_register_view(const char* name, SadViewHandle view);

/// تحديث العرض
void sad_update_view(SadViewHandle view);

// ─────────────────────────────────────────────────────────────────────
// مساعدات / Helpers
// ─────────────────────────────────────────────────────────────────────

/// إنشاء قيمة رقمية
SadValue* sad_create_int(int64_t value);

/// إنشاء قيمة عشرية
SadValue* sad_create_float(double value);

/// إنشاء قيمة نصية
SadValue* sad_create_string(const char* value);

/// إنشاء قيمة منطقية
SadValue* sad_create_bool(bool value);

/// إنشاء قيمة فارغة
SadValue* sad_create_null(void);

#ifdef __cplusplus
}
#endif

#endif /* SadNotes_Bridging_Header_h */
