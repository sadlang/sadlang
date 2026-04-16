// ═══════════════════════════════════════════════════════════════════════════════
// sad_android_bridge.cpp - جسر التواصل بين Android و SadUI
// Bridge between Android and SadUI
// ═══════════════════════════════════════════════════════════════════════════════

#include <jni.h>
#include <android/log.h>
#include <string>
#include <unordered_map>

#define LOG_TAG "SadBridge"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// ═══════════════════════════════════════════════════════════════════════════════
// UI Element Registry
// ═══════════════════════════════════════════════════════════════════════════════

namespace {
    std::unordered_map<long, std::string> g_elements;
    long g_nextElementId = 1;
}

// ═══════════════════════════════════════════════════════════════════════════════
// Bridge API
// ═══════════════════════════════════════════════════════════════════════════════

extern "C" {

/**
 * @brief (AR) إنشاء عنصر UI
 * @brief (EN) Create a UI element
 */
long sad_ui_create_element(const char* type, const char* props) {
    long id = g_nextElementId++;
    g_elements[id] = type;
    LOGD("Created element %ld of type %s", id, type);
    return id;
}

/**
 * @brief (AR) تحديث عنصر
 * @brief (EN) Update an element
 */
bool sad_ui_update_element(long id, const char* props) {
    if (g_elements.find(id) == g_elements.end()) {
        return false;
    }
    LOGD("Updated element %ld", id);
    return true;
}

/**
 * @brief (AR) حذف عنصر
 * @brief (EN) Delete an element
 */
void sad_ui_delete_element(long id) {
    g_elements.erase(id);
    LOGD("Deleted element %ld", id);
}

/**
 * @brief (AR) عدد العناصر
 * @brief (EN) Get element count
 */
int sad_ui_element_count() {
    return static_cast<int>(g_elements.size());
}

} // extern "C"
