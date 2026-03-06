/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  نظام التنقل بين الشاشات — واجهة
 *  Navigator & Routes System for Sad Android UI
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 *  يوفر نظام تنقل مشابه لـ Flutter Navigator 2.0:
 *  - كومة شاشات (Route Stack)
 *  - انتقالات متحركة (Transitions)
 *  - تمرير بيانات بين الشاشات
 *  - زر الرجوع الأصلي
 */

#ifndef SAD_NAVIGATOR_H
#define SAD_NAVIGATOR_H

#include "sad_ui_engine.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  الثوابت
// ═══════════════════════════════════════════════════════════════════════════════

#define SADNAV_MAX_ROUTES       32      // أقصى عدد شاشات في الكومة
#define SADNAV_MAX_NAME_LEN     64      // أقصى طول اسم الشاشة
#define SADNAV_MAX_PARAMS       16      // أقصى عدد معاملات

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع الانتقالات
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SADNAV_TRANSITION_NONE,             // بدون انتقال
    SADNAV_TRANSITION_FADE,             // تلاشي
    SADNAV_TRANSITION_SLIDE_LEFT,       // انزلاق من اليمين لليسار
    SADNAV_TRANSITION_SLIDE_RIGHT,      // انزلاق من اليسار لليمين (عربي)
    SADNAV_TRANSITION_SLIDE_UP,         // انزلاق من الأسفل للأعلى
    SADNAV_TRANSITION_SLIDE_DOWN,       // انزلاق من الأعلى للأسفل
    SADNAV_TRANSITION_SCALE,            // تكبير/تصغير
    SADNAV_TRANSITION_SCALE_FADE,       // تكبير مع تلاشي
} SadNavTransition;

// ═══════════════════════════════════════════════════════════════════════════════
//  هياكل البيانات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * معامل شاشة (key-value)
 */
typedef struct {
    char key[32];
    char value[128];
} SadNavParam;

/**
 * معاملات التنقل
 */
typedef struct {
    SadNavParam params[SADNAV_MAX_PARAMS];
    int count;
} SadNavParams;

/**
 * دالة بناء شاشة
 * @param params معاملات الشاشة
 * @return معرّف الـ widget الجذري للشاشة
 */
typedef SadWidgetId (*SadRouteBuildFunc)(const SadNavParams* params);

/**
 * دالة تنظيف شاشة (اختيارية)
 */
typedef void (*SadRouteDisposeFunc)(SadWidgetId rootWidget);

/**
 * دالة قبل مغادرة الشاشة (للتحقق)
 * @return true للسماح بالمغادرة، false للمنع
 */
typedef bool (*SadRouteCanLeaveFunc)(void);

/**
 * تعريف مسار/شاشة
 */
typedef struct {
    char name[SADNAV_MAX_NAME_LEN];     // اسم الشاشة (مثل "/الرئيسية")
    SadRouteBuildFunc build;            // دالة البناء
    SadRouteDisposeFunc dispose;        // دالة التنظيف (اختيارية)
    SadRouteCanLeaveFunc canLeave;      // دالة التحقق (اختيارية)
    SadNavTransition defaultTransition; // انتقال افتراضي
    bool fullscreen;                    // هل هي شاشة كاملة؟
    bool modal;                         // هل هي شاشة حوارية؟
} SadRouteDefinition;

/**
 * حالة شاشة في الكومة
 */
typedef struct {
    const SadRouteDefinition* definition;
    SadWidgetId rootWidget;
    SadNavParams params;
    float transitionProgress;           // 0.0 - 1.0
    bool isAnimating;
    bool isActive;
} SadRouteState;

/**
 * حالة المتنقل
 */
typedef struct {
    SadRouteState stack[SADNAV_MAX_ROUTES];
    int stackSize;
    
    // الشاشات المسجّلة
    SadRouteDefinition routes[SADNAV_MAX_ROUTES];
    int routeCount;
    
    // إعدادات عامة
    SadNavTransition defaultTransition;
    float transitionDuration;           // بالثواني
    bool rtl;                           // الاتجاه العربي
    
    // حالة الانتقال
    bool isTransitioning;
    SadNavTransition currentTransition;
    float transitionElapsed;
    SadRouteState* outgoingRoute;
    SadRouteState* incomingRoute;
    
} SadNavigatorState;

// ═══════════════════════════════════════════════════════════════════════════════
//  واجهة برمجية أساسية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تهيئة نظام التنقل
 */
bool sadnav_init(void);

/**
 * إنهاء نظام التنقل
 */
void sadnav_shutdown(void);

/**
 * تسجيل شاشة جديدة
 * @param name اسم الشاشة (مثل "/الرئيسية" أو "/الإعدادات")
 * @param build دالة بناء الشاشة
 * @return true عند النجاح
 */
bool sadnav_register(const char* name, SadRouteBuildFunc build);

/**
 * تسجيل شاشة مع خيارات إضافية
 */
bool sadnav_register_ex(const SadRouteDefinition* definition);

/**
 * الانتقال إلى شاشة جديدة (push)
 * @param routeName اسم الشاشة
 * @return true عند النجاح
 */
bool sadnav_push(const char* routeName);

/**
 * الانتقال مع معاملات
 */
bool sadnav_push_params(const char* routeName, const SadNavParams* params);

/**
 * الانتقال مع انتقال محدد
 */
bool sadnav_push_transition(const char* routeName, SadNavTransition transition);

/**
 * الرجوع للشاشة السابقة (pop)
 * @return true إذا تم الرجوع، false إذا كانت الشاشة الأخيرة
 */
bool sadnav_pop(void);

/**
 * الرجوع مع نتيجة
 */
bool sadnav_pop_result(const char* result);

/**
 * استبدال الشاشة الحالية (replace)
 */
bool sadnav_replace(const char* routeName);

/**
 * مسح الكومة والانتقال لشاشة جديدة
 */
bool sadnav_clear_and_push(const char* routeName);

/**
 * الرجوع لشاشة محددة
 */
bool sadnav_pop_until(const char* routeName);

/**
 * هل يمكن الرجوع؟
 */
bool sadnav_can_pop(void);

/**
 * الحصول على اسم الشاشة الحالية
 */
const char* sadnav_current_route(void);

/**
 * الحصول على عدد الشاشات في الكومة
 */
int sadnav_stack_size(void);

/**
 * الحصول على معامل من الشاشة الحالية
 */
const char* sadnav_get_param(const char* key);

/**
 * تعيين معامل نتيجة للشاشة السابقة
 */
void sadnav_set_result(const char* key, const char* value);

// ═══════════════════════════════════════════════════════════════════════════════
//  تحديث ورسم
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تحديث الانتقالات (يُستدعى كل إطار)
 * @param deltaTime الوقت منذ الإطار السابق بالثواني
 */
void sadnav_update(float deltaTime);

/**
 * الحصول على الـ widget الجذري للشاشة الحالية
 */
SadWidgetId sadnav_get_current_root(void);

/**
 * معالجة زر الرجوع (Back button)
 * @return true إذا تمت معالجة الزر
 */
bool sadnav_handle_back(void);

// ═══════════════════════════════════════════════════════════════════════════════
//  إعدادات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تعيين مدة الانتقال الافتراضية
 */
void sadnav_set_transition_duration(float seconds);

/**
 * تعيين الانتقال الافتراضي
 */
void sadnav_set_default_transition(SadNavTransition transition);

/**
 * تفعيل/تعطيل الاتجاه العربي (RTL)
 */
void sadnav_set_rtl(bool rtl);

// ═══════════════════════════════════════════════════════════════════════════════
//  أدوات مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * إنشاء معاملات فارغة
 */
SadNavParams sadnav_params_create(void);

/**
 * إضافة معامل
 */
void sadnav_params_add(SadNavParams* params, const char* key, const char* value);

/**
 * الحصول على معامل
 */
const char* sadnav_params_get(const SadNavParams* params, const char* key);

// ═══════════════════════════════════════════════════════════════════════════════
//  واجهة عربية
// ═══════════════════════════════════════════════════════════════════════════════

// تهيئة
#define متنقل_تهيئة          sadnav_init
#define متنقل_إنهاء          sadnav_shutdown

// تسجيل
#define متنقل_سجل            sadnav_register
#define متنقل_سجل_موسع       sadnav_register_ex

// تنقل
#define اذهب_إلى             sadnav_push
#define اذهب_مع_معاملات      sadnav_push_params
#define ارجع                 sadnav_pop
#define ارجع_بنتيجة          sadnav_pop_result
#define استبدل_الشاشة        sadnav_replace
#define امسح_واذهب           sadnav_clear_and_push
#define ارجع_حتى             sadnav_pop_until

// استعلام
#define يمكن_الرجوع          sadnav_can_pop
#define الشاشة_الحالية       sadnav_current_route
#define عدد_الشاشات          sadnav_stack_size
#define احصل_معامل           sadnav_get_param
#define عين_نتيجة            sadnav_set_result

// أنواع الانتقالات
#define انتقال_بلا           SADNAV_TRANSITION_NONE
#define انتقال_تلاشي         SADNAV_TRANSITION_FADE
#define انتقال_انزلاق_يسار   SADNAV_TRANSITION_SLIDE_LEFT
#define انتقال_انزلاق_يمين   SADNAV_TRANSITION_SLIDE_RIGHT
#define انتقال_انزلاق_أعلى   SADNAV_TRANSITION_SLIDE_UP
#define انتقال_انزلاق_أسفل   SADNAV_TRANSITION_SLIDE_DOWN
#define انتقال_تكبير         SADNAV_TRANSITION_SCALE

#ifdef __cplusplus
}
#endif

#endif // SAD_NAVIGATOR_H
