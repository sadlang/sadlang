/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_ui_callbacks.h — سجل الإرجاعات (callbacks) للمكونات
 *  Widget Callback Registry — maps widget events to ص function names
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  يُستخدم لدعم صيغة فلتر:
 *    زر("اضغط"، عند_الضغط: "دالتي")
 *
 *  عند إنشاء المكون، تُسجَّل الدالة في السجل العام.
 *  في حلقة التحديث، يتم فحص الأحداث واستدعاء الدوال المسجلة.
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include <string>
#include <unordered_map>
#include <mutex>

namespace sad {
namespace callbacks {

/**
 * @brief (AR) خريطة الأحداث لمكون واحد: اسم_الحدث → اسم_الدالة
 * @brief (EN) Event map for one widget: event_name → function_name
 */
using EventMap = std::unordered_map<std::string, std::string>;

/**
 * @brief (AR) السجل العام: widget_id → EventMap
 * @brief (EN) Global registry: widget_id → EventMap
 */
inline std::unordered_map<int, EventMap>& registry() {
    static std::unordered_map<int, EventMap> s_registry;
    return s_registry;
}

/**
 * @brief (AR) تسجيل دالة إرجاع لحدث على مكون
 * @brief (EN) Register a callback function for an event on a widget
 *
 * @param widgetId معرّف المكون
 * @param eventName اسم الحدث (مثل "عند_الضغط" أو "onClick")
 * @param functionName اسم الدالة في ص
 */
inline void registerCallback(int widgetId, const std::string& eventName,
                              const std::string& functionName) {
    // توحيد أسماء الأحداث إلى الأسماء الداخلية
    std::string normalized = eventName;
    if (eventName == "عند_الضغط"       || eventName == "onClick")  normalized = "click";
    else if (eventName == "عند_التغيير" || eventName == "onChange") normalized = "change";
    else if (eventName == "عند_التركيز" || eventName == "onFocus")  normalized = "focus";
    else if (eventName == "عند_التحويم" || eventName == "onHover")  normalized = "hover";
    else if (eventName == "عند_فقد_التركيز" || eventName == "onBlur") normalized = "blur";
    
    registry()[widgetId][normalized] = functionName;
}

/**
 * @brief (AR) الحصول على اسم الدالة المسجلة لحدث
 * @brief (EN) Get the registered function name for an event
 *
 * @return اسم الدالة أو سلسلة فارغة إذا لم يُسجَّل
 */
inline std::string getCallback(int widgetId, const std::string& normalizedEvent) {
    auto& reg = registry();
    auto wit = reg.find(widgetId);
    if (wit == reg.end()) return "";
    auto eit = wit->second.find(normalizedEvent);
    if (eit == wit->second.end()) return "";
    return eit->second;
}

/**
 * @brief (AR) هل هذا الاسم حدث معروف؟
 * @brief (EN) Is this name a known event property?
 */
inline bool isEventProperty(const std::string& name) {
    return name == "عند_الضغط" || name == "onClick" ||
           name == "عند_التغيير" || name == "onChange" ||
           name == "عند_التركيز" || name == "onFocus" ||
           name == "عند_التحويم" || name == "onHover" ||
           name == "عند_فقد_التركيز" || name == "onBlur";
}

/**
 * @brief (AR) إزالة جميع الإرجاعات لمكون
 * @brief (EN) Remove all callbacks for a widget
 */
inline void removeCallbacks(int widgetId) {
    registry().erase(widgetId);
}

/**
 * @brief (AR) مسح كل السجل
 * @brief (EN) Clear entire registry
 */
inline void clearAll() {
    registry().clear();
}

/**
 * @brief (AR) الحصول على جميع المكونات التي لديها إرجاعات مسجلة
 * @brief (EN) Get all widget IDs that have registered callbacks
 */
inline std::vector<int> getRegisteredWidgets() {
    std::vector<int> ids;
    for (auto& [id, _] : registry()) {
        ids.push_back(id);
    }
    return ids;
}

}} // namespace sad::callbacks
