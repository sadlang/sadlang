/**
 * ============================================================================
 * C API Bridge — طبقة extern "C" للمترجم
 * تتيح استدعاء المترجم من لغات أخرى عبر FFI
 * ============================================================================
 */
#ifndef C_API_BRIDGE_H
#define C_API_BRIDGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ──── أنواع البيانات ──── */

typedef enum {
    SADC_SEVERITY_ERROR   = 0,
    SADC_SEVERITY_WARNING = 1,
    SADC_SEVERITY_INFO    = 2,
    SADC_SEVERITY_HINT    = 3
} SadcSeverity;

typedef struct {
    const char* file;
    int32_t     line;
    int32_t     column;
    int32_t     severity;
    const char* message;
    const char* code;
} SadcDiagnostic;

typedef struct {
    int32_t          success;
    const char*      output;
    const char*      errors;
    int32_t          diagnostic_count;
    SadcDiagnostic*  diagnostics;
} SadcResult;

typedef struct {
    int32_t     major;
    int32_t     minor;
    int32_t     patch;
    const char* commit_hash;
    const char* build_date;
} SadcVersionInfo;

/* ──── API الأساسي ──── */

/** تهيئة المترجم — يجب استدعاؤها أولاً */
int32_t sadc_init(void);

/** إيقاف المترجم وتحرير الموارد */
void sadc_shutdown(void);

/** ترجمة ملف */
SadcResult* sadc_compile_file(const char* file_path, const char* target);

/** فحص ملف (دون ترجمة) */
SadcResult* sadc_check_file(const char* file_path);

/** ترجمة نص مباشر */
SadcResult* sadc_compile_string(const char* source, const char* filename, const char* target);

/** تحرير نتيجة */
void sadc_free_result(SadcResult* result);

/** معلومات الإصدار */
SadcVersionInfo* sadc_get_version(void);

/* ──── API المشاريع ──── */

/** تحليل ملف مشروع.ص */
SadcResult* sadc_parse_project(const char* project_file_path);

/** بناء مشروع كامل */
SadcResult* sadc_build_project(const char* project_dir, const char* profile);

/* ──── API الإكمال (LSP) ──── */

/** الحصول على اقتراحات إكمال */
SadcResult* sadc_complete(const char* file_path, int32_t line, int32_t column);

/** الحصول على تعريف رمز */
SadcResult* sadc_goto_definition(const char* file_path, int32_t line, int32_t column);

/** الحصول على معلومات Hover */
SadcResult* sadc_hover(const char* file_path, int32_t line, int32_t column);

#ifdef __cplusplus
}
#endif

#endif /* C_API_BRIDGE_H */
