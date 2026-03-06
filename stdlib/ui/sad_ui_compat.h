/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  sad_ui_compat.h — طبقة التوافق: اعادة توجيه sad::ui → sad::unified
 *  Compatibility Layer: redirects sad::ui calls to sad::unified when active
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  الهدف: السماح بان يبقى كود builtins الحالي (part13-16) بدون تعديل
 *  على سطح المكتب يستخدم sad::ui مباشرة — على أندرويد/ويب يمرّر عبر الموحد
 *
 *  الاستخدام:
 *    في بداية main() أو interpreter init:
 *      sad::unified::initPlatformUI();  // يهيئ الBackend المناسب
 *      sad::compat::enableUnifiedRedirect();  // يفعّل التحويل
 *
 *    بعدها، جميع استدعاءات sad::ui::* ستُحوَّل تلقائياً إلى sad::unified::*
 *    (فقط إذا كان الBackend الموحد مُفعّلاً وليس desktop)
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#pragma once

#include "sad_ui_unified.h"
#include "sad_ui_platform.h"

namespace sad {
namespace compat {

/**
 * @brief (AR) هل التحويل الموحد مفعّل؟
 * @brief (EN) Is unified redirect active?
 */
inline bool& unifiedRedirectEnabled() {
    static bool enabled = false;
    return enabled;
}

/**
 * @brief (AR) تفعيل التحويل الموحد — جميع استدعاءات sad::ui تمر عبر sad::unified
 * @brief (EN) Enable unified redirect — all sad::ui calls go through sad::unified
 */
inline void enableUnifiedRedirect() {
    if (sad::unified::isUIReady()) {
        unifiedRedirectEnabled() = true;
    }
}

/**
 * @brief (AR) تعطيل التحويل الموحد — العودة للاستدعاء المباشر
 * @brief (EN) Disable unified redirect — return to direct sad::ui calls
 */
inline void disableUnifiedRedirect() {
    unifiedRedirectEnabled() = false;
}

/**
 * @brief (AR) هل يجب استخدام الطبقة الموحدة؟
 * @brief (EN) Should we use the unified layer?
 *
 * يعيد true فقط إذا:
 *   1. التحويل مفعّل
 *   2. الBackend الموحد مُهيّأ
 */
inline bool shouldUseUnified() {
    return unifiedRedirectEnabled() && sad::unified::isUIReady();
}

}} // namespace sad::compat
