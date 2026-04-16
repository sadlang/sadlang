// ═══════════════════════════════════════════════════════════════════════════════
// sad_android.h - واجهات C لمكتبة ص على أندرويد
// C API for Sad Android Library
// ═══════════════════════════════════════════════════════════════════════════════
//
// @brief (AR) هذا الملف يحتوي على تعريفات API العامة لمكتبة ص على أندرويد
// @brief (EN) This file contains public API definitions for Sad Android library
//
// ═══════════════════════════════════════════════════════════════════════════════

#ifndef SAD_ANDROID_H
#define SAD_ANDROID_H

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
// Version Information
// ═══════════════════════════════════════════════════════════════════════════════

#define SAD_ANDROID_VERSION_MAJOR 1
#define SAD_ANDROID_VERSION_MINOR 0
#define SAD_ANDROID_VERSION_PATCH 0
#define SAD_ANDROID_VERSION_STRING "1.0.0-alpha"

// ═══════════════════════════════════════════════════════════════════════════════
// Runtime API
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) تهيئة runtime ص
 * @brief (EN) Initialize Sad runtime
 * @return 1 if successful, 0 otherwise
 */
int sad_runtime_init(void);

/**
 * @brief (AR) إيقاف runtime ص
 * @brief (EN) Shutdown Sad runtime
 */
void sad_runtime_shutdown(void);

/**
 * @brief (AR) تنفيذ كود ص
 * @brief (EN) Execute Sad code
 * @param code UTF-8 encoded Sad source code
 * @return 1 if successful, 0 otherwise
 */
int sad_execute_code(const char* code);

/**
 * @brief (AR) الحصول على آخر خطأ
 * @brief (EN) Get last error message
 * @return Error message or NULL if no error
 */
const char* sad_get_last_error(void);

/**
 * @brief (AR) التحقق من حالة التهيئة
 * @brief (EN) Check if runtime is initialized
 * @return 1 if initialized, 0 otherwise
 */
int sad_is_initialized(void);

// ═══════════════════════════════════════════════════════════════════════════════
// UI API
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) إنشاء عنصر UI
 * @brief (EN) Create a UI element
 * @param type Element type (e.g., "نص", "زر", "صورة")
 * @param props JSON properties string
 * @return Element handle, or 0 on failure
 */
long sad_ui_create_element(const char* type, const char* props);

/**
 * @brief (AR) تحديث عنصر
 * @brief (EN) Update an element
 * @param handle Element handle
 * @param props JSON properties string
 * @return 1 if successful, 0 otherwise
 */
int sad_ui_update_element(long handle, const char* props);

/**
 * @brief (AR) حذف عنصر
 * @brief (EN) Delete an element
 * @param handle Element handle
 */
void sad_ui_delete_element(long handle);

/**
 * @brief (AR) عدد العناصر
 * @brief (EN) Get element count
 * @return Number of active elements
 */
int sad_ui_element_count(void);

#ifdef __cplusplus
}
#endif

#endif // SAD_ANDROID_H
