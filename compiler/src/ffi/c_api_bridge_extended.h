/**
 * ============================================================================
 * رأس واجهة C API الموسّعة — Extended C API Bridge Header
 * ============================================================================
 *
 * يُعرّف 50+ دالة خارجية (extern "C") تغطّي:
 * - دورة الحياة: init / shutdown / health_check
 * - بيانات اللغة: meta_language / meta_docs / meta_project_schema
 * - البناء: compile_file / compile_string / check / run_jit / build_project
 * - LSP: complete / hover / definition / references / diagnostics / symbols
 * - التصحيح: debug_launch / debug_breakpoint / debug_step / debug_variables
 * - التصوّر: ownership_map / lifetime_graph / ast_dump / sir_dump
 *
 * المرجع: contracts/ffi-bridge.md — القسم Q4
 * ============================================================================
 */
#ifndef SADC_C_API_BRIDGE_EXTENDED_H
#define SADC_C_API_BRIDGE_EXTENDED_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ══════════════════════════════════════════════════════════════
 * أنواع البيانات المشتركة
 * ══════════════════════════════════════════════════════════════ */

/// تشخيص واحد (خطأ / تحذير / ملاحظة)
typedef struct {
    const char* file;       ///< مسار الملف
    int32_t     line;       ///< رقم السطر
    int32_t     column;     ///< رقم العمود
    int32_t     severity;   ///< 0=خطأ, 1=تحذير, 2=ملاحظة, 3=تلميح
    const char* message;    ///< الرسالة
    const char* message_ar; ///< الرسالة بالعربية
    const char* code;       ///< رمز الخطأ
    const char* fix;        ///< إصلاح مقترح (يمكن أن يكون NULL)
} SadcDiagnostic;

/// نتيجة عملية عامة
typedef struct {
    int32_t          success;          ///< 1=نجاح, 0=فشل
    const char*      output;          ///< المُخرج النصي
    const char*      errors;          ///< الأخطاء النصية
    SadcDiagnostic*  diagnostics;     ///< مصفوفة التشخيصات
    int32_t          diagnostic_count; ///< عدد التشخيصات
    const uint8_t*   binary_data;     ///< بيانات ثنائية (FlatBuffers)
    int32_t          binary_size;     ///< حجم البيانات الثنائية
} SadcResult;

/// معلومات الإصدار
typedef struct {
    int32_t     major;        ///< الرقم الأساسي
    int32_t     minor;        ///< الرقم الفرعي
    int32_t     patch;        ///< رقم التصحيح
    const char* commit_hash;  ///< تجزئة الالتزام
    const char* build_date;   ///< تاريخ البناء
} SadcVersionInfo;

/// عنصر إكمال تلقائي
typedef struct {
    const char* label;        ///< النص المعروض
    const char* label_ar;     ///< النص بالعربية
    const char* detail;       ///< التفاصيل
    const char* insert_text;  ///< النص المُدرج
    int32_t     kind;         ///< نوع العنصر (0=كلمة_مفتاحية, ...)
    int32_t     priority;     ///< الأولوية (أعلى = أهم)
} SadcCompletionItem;

/// نتيجة الإكمال التلقائي
typedef struct {
    SadcCompletionItem* items;   ///< مصفوفة العناصر
    int32_t             count;   ///< عدد العناصر
    int32_t             is_incomplete; ///< 1=قائمة غير مكتملة
} SadcCompletionList;

/// موقع في الكود
typedef struct {
    const char* file;    ///< مسار الملف
    int32_t     line;    ///< رقم السطر
    int32_t     column;  ///< رقم العمود
    int32_t     end_line;   ///< نهاية السطر
    int32_t     end_column; ///< نهاية العمود
} SadcLocation;

/// معلومات التمرير (hover)
typedef struct {
    const char* content;      ///< المحتوى (Markdown)
    const char* type_info;    ///< معلومات النوع
    const char* doc_comment;  ///< التوثيق
    const char* ownership;    ///< معلومات الملكية
} SadcHoverInfo;

/// معلومات نقطة التوقف
typedef struct {
    int32_t     id;          ///< معرّف نقطة التوقف
    const char* file;        ///< مسار الملف
    int32_t     line;        ///< رقم السطر
    int32_t     verified;    ///< 1=مؤكدة, 0=لم تتحقق
    const char* condition;   ///< شرط (اختياري)
} SadcBreakpointInfo;

/// متغير تصحيح
typedef struct {
    const char* name;         ///< اسم المتغير
    const char* value;        ///< القيمة
    const char* type_name;    ///< اسم النوع
    const char* ownership;    ///< حالة الملكية
    int32_t     children_count; ///< عدد العناصر الفرعية
} SadcVariable;

/// معلومات الصحة
typedef struct {
    int32_t     initialized;     ///< 1=مُهيّأ
    int32_t     stdlib_loaded;   ///< 1=المكتبة القياسية محمّلة
    uint64_t    memory_usage;    ///< الذاكرة المستخدمة (بايت)
    uint64_t    ffi_latency_us;  ///< تأخير FFI (ميكروثانية)
    const char* compiler_version; ///< إصدار المترجم
} SadcHealthInfo;

/* ══════════════════════════════════════════════════════════════
 * 1. دورة الحياة (Lifecycle)
 * ══════════════════════════════════════════════════════════════ */

/// تهيئة المترجم مع مسار SDK
int32_t sadc_init_with_sdk(const char* sdk_path);

/// إيقاف المترجم وتحرير الذاكرة
void sadc_shutdown(void);

/// فحص صحة المترجم
SadcHealthInfo* sadc_health_check(void);

/// الإصدار
SadcVersionInfo* sadc_get_version(void);

/// تحرير نتيجة عامة
void sadc_free_result(SadcResult* result);

/// تحرير نص
void sadc_free_string(char* str);

/* ══════════════════════════════════════════════════════════════
 * 2. بيانات اللغة (Meta)
 * ══════════════════════════════════════════════════════════════ */

/// تصدير بيانات اللغة (كلمات مفتاحية + أنواع + ...)
SadcResult* sadc_meta_language(void);

/// تصدير توثيق المكتبة القياسية
SadcResult* sadc_meta_docs(const char* module_name);

/// تصدير مخطط مشروع.ص
SadcResult* sadc_meta_project_schema(void);

/* ══════════════════════════════════════════════════════════════
 * 3. البناء والتشغيل (Build)
 * ══════════════════════════════════════════════════════════════ */

/// ترجمة ملف واحد
SadcResult* sadc_compile_file(const char* file_path, const char* target);

/// ترجمة نص مباشر
SadcResult* sadc_compile_string(const char* source, const char* filename,
                                 const char* target);

/// فحص بدون ترجمة
SadcResult* sadc_check_file(const char* file_path);

/// فحص نص مباشر
SadcResult* sadc_check_string(const char* source, const char* filename);

/// تشغيل فوري (JIT)
SadcResult* sadc_run_jit(const char* file_path, const char** args,
                          int32_t arg_count);

/// تشغيل نص فوري (JIT)
SadcResult* sadc_run_string_jit(const char* source, const char* filename);

/// بناء مشروع كامل
SadcResult* sadc_build_project(const char* project_dir, const char* profile);

/// تحليل ملف مشروع.ص
SadcResult* sadc_parse_project(const char* project_file_path);

/// بناء تزايدي (ملفات متغيرة فقط)
SadcResult* sadc_build_incremental(const char* project_dir,
                                     const char** changed_files,
                                     int32_t file_count);

/* ══════════════════════════════════════════════════════════════
 * 4. خدمات LSP (Language Server Protocol)
 * ══════════════════════════════════════════════════════════════ */

/// إكمال تلقائي
SadcCompletionList* sadc_complete(const char* file_path,
                                   int32_t line, int32_t column,
                                   const char* trigger_char);

/// تحرير قائمة الإكمال
void sadc_free_completions(SadcCompletionList* list);

/// معلومات التمرير
SadcHoverInfo* sadc_hover(const char* file_path,
                            int32_t line, int32_t column);

/// تحرير معلومات التمرير
void sadc_free_hover(SadcHoverInfo* info);

/// الذهاب للتعريف
SadcLocation* sadc_goto_definition(const char* file_path,
                                     int32_t line, int32_t column);

/// البحث عن المراجع
SadcResult* sadc_find_references(const char* file_path,
                                   int32_t line, int32_t column);

/// التشخيصات لملف
SadcResult* sadc_get_diagnostics(const char* file_path);

/// رموز الملف (outline)
SadcResult* sadc_document_symbols(const char* file_path);

/// رموز المشروع (workspace symbols)
SadcResult* sadc_workspace_symbols(const char* query);

/// تنسيق الكود
SadcResult* sadc_format_file(const char* file_path);

/// تنسيق نطاق
SadcResult* sadc_format_range(const char* file_path,
                                int32_t start_line, int32_t start_col,
                                int32_t end_line, int32_t end_col);

/// إعادة التسمية
SadcResult* sadc_rename(const char* file_path,
                          int32_t line, int32_t column,
                          const char* new_name);

/// معلومات التوقيع (signature help)
SadcResult* sadc_signature_help(const char* file_path,
                                  int32_t line, int32_t column);

/// إجراءات الكود (code actions / quick fix)
SadcResult* sadc_code_actions(const char* file_path,
                                int32_t start_line, int32_t start_col,
                                int32_t end_line, int32_t end_col);

/// طي الكود (folding ranges)
SadcResult* sadc_folding_ranges(const char* file_path);

/// التلوين الدلالي (semantic tokens)
SadcResult* sadc_semantic_tokens(const char* file_path);

/* ══════════════════════════════════════════════════════════════
 * 5. التصحيح (Debug)
 * ══════════════════════════════════════════════════════════════ */

/// بدء جلسة تصحيح
SadcResult* sadc_debug_launch(const char* file_path, const char** args,
                                int32_t arg_count);

/// إيقاف جلسة التصحيح
void sadc_debug_terminate(void);

/// إضافة نقطة توقف
SadcBreakpointInfo* sadc_debug_set_breakpoint(const char* file_path,
                                                int32_t line,
                                                const char* condition);

/// إزالة نقطة توقف
void sadc_debug_remove_breakpoint(int32_t breakpoint_id);

/// استئناف التنفيذ
SadcResult* sadc_debug_continue(void);

/// خطوة للأمام
SadcResult* sadc_debug_step_over(void);

/// دخول الدالة
SadcResult* sadc_debug_step_into(void);

/// الخروج من الدالة
SadcResult* sadc_debug_step_out(void);

/// خطوة للخلف (time-travel)
SadcResult* sadc_debug_step_back(void);

/// قراءة المتغيرات في النطاق الحالي
SadcResult* sadc_debug_get_variables(int32_t frame_id);

/// تقييم تعبير
SadcResult* sadc_debug_evaluate(const char* expression, int32_t frame_id);

/// إطارات المكدس
SadcResult* sadc_debug_get_stack_frames(void);

/// تحرير نقطة توقف
void sadc_free_breakpoint(SadcBreakpointInfo* bp);

/* ══════════════════════════════════════════════════════════════
 * 6. التصوّر (Visualization)
 * ══════════════════════════════════════════════════════════════ */

/// خريطة الملكية لملف
SadcResult* sadc_vis_ownership_map(const char* file_path);

/// رسم بياني لأعمار المتغيرات
SadcResult* sadc_vis_lifetime_graph(const char* file_path);

/// تفريغ AST (شجرة الصياغة)
SadcResult* sadc_vis_ast_dump(const char* file_path, const char* format);

/// تفريغ SIR (التمثيل الوسيط)
SadcResult* sadc_vis_sir_dump(const char* file_path, const char* format);

/* ══════════════════════════════════════════════════════════════
 * 7. إدارة الوثيقة (Document Management)
 * ══════════════════════════════════════════════════════════════ */

/// فتح وثيقة في ذاكرة المترجم
SadcResult* sadc_document_open(const char* file_path, const char* content);

/// تحديث وثيقة
SadcResult* sadc_document_change(const char* file_path, const char* content,
                                   int32_t version);

/// إغلاق وثيقة
void sadc_document_close(const char* file_path);

#ifdef __cplusplus
}
#endif

#endif /* SADC_C_API_BRIDGE_EXTENDED_H */
