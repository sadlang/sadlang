/**
 * ============================================================================
 * C API Bridge — تنفيذ طبقة FFI
 * ============================================================================
 */
#include "ffi/c_api_bridge.h"
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <mutex>

/* ──── حالة عامة ──── */
static bool g_initialized = false;
static std::mutex g_mutex;

static const int VERSION_MAJOR = 0;
static const int VERSION_MINOR = 1;
static const int VERSION_PATCH = 0;
static const char* VERSION_HASH = "dev";
static const char* VERSION_DATE = __DATE__;

/* ──── أدوات مساعدة ──── */

static char* strdup_safe(const char* s) {
    if (!s) return nullptr;
    size_t len = strlen(s) + 1;
    char* dup = (char*)malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

static SadcResult* make_result(bool success, const std::string& output,
                                const std::string& errors,
                                const std::vector<SadcDiagnostic>& diags = {}) {
    auto* r = (SadcResult*)calloc(1, sizeof(SadcResult));
    r->success = success ? 1 : 0;
    r->output = strdup_safe(output.c_str());
    r->errors = strdup_safe(errors.c_str());
    r->diagnostic_count = (int32_t)diags.size();
    if (!diags.empty()) {
        r->diagnostics = (SadcDiagnostic*)calloc(diags.size(), sizeof(SadcDiagnostic));
        for (size_t i = 0; i < diags.size(); i++) {
            r->diagnostics[i].file     = strdup_safe(diags[i].file);
            r->diagnostics[i].line     = diags[i].line;
            r->diagnostics[i].column   = diags[i].column;
            r->diagnostics[i].severity = diags[i].severity;
            r->diagnostics[i].message  = strdup_safe(diags[i].message);
            r->diagnostics[i].code     = strdup_safe(diags[i].code);
        }
    }
    return r;
}

/* ──── تنفيذ API ──── */

extern "C" {

int32_t sadc_init(void) {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_initialized) return 0;
    // TODO: تهيئة المحلل والمترجم الحقيقيين
    g_initialized = true;
    return 0;
}

void sadc_shutdown(void) {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_initialized) return;
    // TODO: تحرير موارد المترجم
    g_initialized = false;
}

SadcResult* sadc_compile_file(const char* file_path, const char* target) {
    if (!g_initialized) return make_result(false, "", "المترجم غير مُهيّأ");
    if (!file_path) return make_result(false, "", "مسار الملف فارغ");

    // TODO: استدعاء pipeline الترجمة الحقيقي
    // Lexer → Parser → Semantic → CodeGen
    std::string msg = "ترجمة: ";
    msg += file_path;
    msg += " → ";
    msg += (target ? target : "llvm-ir");
    return make_result(true, msg, "");
}

SadcResult* sadc_check_file(const char* file_path) {
    if (!g_initialized) return make_result(false, "", "المترجم غير مُهيّأ");
    if (!file_path) return make_result(false, "", "مسار الملف فارغ");

    // TODO: فحص بدون ترجمة
    return make_result(true, "فحص ناجح", "");
}

SadcResult* sadc_compile_string(const char* source, const char* filename, const char* target) {
    if (!g_initialized) return make_result(false, "", "المترجم غير مُهيّأ");
    if (!source) return make_result(false, "", "النص المصدري فارغ");

    // TODO: ترجمة نص مباشر
    return make_result(true, "ترجمة نص مباشر", "");
}

void sadc_free_result(SadcResult* result) {
    if (!result) return;
    free((void*)result->output);
    free((void*)result->errors);
    for (int32_t i = 0; i < result->diagnostic_count; i++) {
        free((void*)result->diagnostics[i].file);
        free((void*)result->diagnostics[i].message);
        free((void*)result->diagnostics[i].code);
    }
    free(result->diagnostics);
    free(result);
}

SadcVersionInfo* sadc_get_version(void) {
    static SadcVersionInfo info;
    info.major = VERSION_MAJOR;
    info.minor = VERSION_MINOR;
    info.patch = VERSION_PATCH;
    info.commit_hash = VERSION_HASH;
    info.build_date = VERSION_DATE;
    return &info;
}

SadcResult* sadc_parse_project(const char* project_file_path) {
    if (!g_initialized) return make_result(false, "", "المترجم غير مُهيّأ");
    // TODO: تحليل مشروع.ص
    return make_result(true, "مشروع محلل", "");
}

SadcResult* sadc_build_project(const char* project_dir, const char* profile) {
    if (!g_initialized) return make_result(false, "", "المترجم غير مُهيّأ");
    // TODO: بناء مشروع كامل
    return make_result(true, "بناء ناجح", "");
}

SadcResult* sadc_complete(const char* file_path, int32_t line, int32_t column) {
    if (!g_initialized) return make_result(false, "", "المترجم غير مُهيّأ");
    // TODO: إكمال تلقائي
    return make_result(true, "[]", "");
}

SadcResult* sadc_goto_definition(const char* file_path, int32_t line, int32_t column) {
    if (!g_initialized) return make_result(false, "", "المترجم غير مُهيّأ");
    return make_result(true, "", "");
}

SadcResult* sadc_hover(const char* file_path, int32_t line, int32_t column) {
    if (!g_initialized) return make_result(false, "", "المترجم غير مُهيّأ");
    return make_result(true, "", "");
}

} // extern "C"
