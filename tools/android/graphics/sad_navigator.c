/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  نظام التنقل بين الشاشات — التنفيذ
 *  Navigator & Routes System Implementation
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_navigator.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __ANDROID__
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SadNav", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SadNav", __VA_ARGS__)
#else
#include <stdio.h>
#define LOGI(...) do { printf("[SadNav] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define LOGE(...) do { printf("[SadNav ERROR] "); printf(__VA_ARGS__); printf("\n"); } while(0)
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  الحالة العامة
// ═══════════════════════════════════════════════════════════════════════════════

static SadNavigatorState g_nav = {0};
static bool g_initialized = false;

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════════════════

static const SadRouteDefinition* find_route(const char* name) {
    if (!name) return NULL;
    for (int i = 0; i < g_nav.routeCount; i++) {
        if (strcmp(g_nav.routes[i].name, name) == 0) {
            return &g_nav.routes[i];
        }
    }
    return NULL;
}

static SadRouteState* get_current_state(void) {
    if (g_nav.stackSize <= 0) return NULL;
    return &g_nav.stack[g_nav.stackSize - 1];
}

// Easing function (ease-out cubic)
static float ease_out_cubic(float t) {
    t = t - 1.0f;
    return t * t * t + 1.0f;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تهيئة وإنهاء
// ═══════════════════════════════════════════════════════════════════════════════

bool sadnav_init(void) {
    if (g_initialized) return true;
    
    memset(&g_nav, 0, sizeof(g_nav));
    g_nav.defaultTransition = SADNAV_TRANSITION_SLIDE_RIGHT; // افتراضي عربي
    g_nav.transitionDuration = 0.3f; // 300ms
    g_nav.rtl = true;
    
    g_initialized = true;
    LOGI("تم تهيئة نظام التنقل");
    return true;
}

void sadnav_shutdown(void) {
    if (!g_initialized) return;
    
    // تنظيف جميع الشاشات في الكومة
    while (g_nav.stackSize > 0) {
        SadRouteState* state = get_current_state();
        if (state && state->definition && state->definition->dispose) {
            state->definition->dispose(state->rootWidget);
        }
        g_nav.stackSize--;
    }
    
    memset(&g_nav, 0, sizeof(g_nav));
    g_initialized = false;
    LOGI("تم إنهاء نظام التنقل");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تسجيل الشاشات
// ═══════════════════════════════════════════════════════════════════════════════

bool sadnav_register(const char* name, SadRouteBuildFunc build) {
    if (!g_initialized) sadnav_init();
    if (!name || !build) return false;
    if (g_nav.routeCount >= SADNAV_MAX_ROUTES) {
        LOGE("وصلنا للحد الأقصى من الشاشات المسجّلة");
        return false;
    }
    
    // تحقق من عدم وجود شاشة بنفس الاسم
    if (find_route(name)) {
        LOGE("شاشة بهذا الاسم موجودة مسبقاً: %s", name);
        return false;
    }
    
    SadRouteDefinition* route = &g_nav.routes[g_nav.routeCount];
    memset(route, 0, sizeof(SadRouteDefinition));
    
    strncpy(route->name, name, SADNAV_MAX_NAME_LEN - 1);
    route->build = build;
    route->defaultTransition = g_nav.defaultTransition;
    
    g_nav.routeCount++;
    LOGI("تم تسجيل شاشة: %s", name);
    return true;
}

bool sadnav_register_ex(const SadRouteDefinition* definition) {
    if (!g_initialized) sadnav_init();
    if (!definition || !definition->name[0] || !definition->build) return false;
    if (g_nav.routeCount >= SADNAV_MAX_ROUTES) return false;
    
    if (find_route(definition->name)) {
        LOGE("شاشة بهذا الاسم موجودة مسبقاً: %s", definition->name);
        return false;
    }
    
    g_nav.routes[g_nav.routeCount] = *definition;
    g_nav.routeCount++;
    LOGI("تم تسجيل شاشة موسّعة: %s", definition->name);
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  التنقل
// ═══════════════════════════════════════════════════════════════════════════════

static bool push_route_internal(const char* routeName, const SadNavParams* params, 
                                SadNavTransition transition) {
    if (!g_initialized) return false;
    
    const SadRouteDefinition* route = find_route(routeName);
    if (!route) {
        LOGE("شاشة غير موجودة: %s", routeName);
        return false;
    }
    
    if (g_nav.stackSize >= SADNAV_MAX_ROUTES) {
        LOGE("كومة الشاشات ممتلئة");
        return false;
    }
    
    // إلغاء تفعيل الشاشة الحالية
    SadRouteState* current = get_current_state();
    if (current) {
        current->isActive = false;
    }
    
    // إنشاء حالة الشاشة الجديدة
    SadRouteState* newState = &g_nav.stack[g_nav.stackSize];
    memset(newState, 0, sizeof(SadRouteState));
    
    newState->definition = route;
    if (params) {
        newState->params = *params;
    }
    newState->isActive = true;
    
    // بناء الشاشة
    newState->rootWidget = route->build(&newState->params);
    if (!newState->rootWidget) {
        LOGE("فشل بناء الشاشة: %s", routeName);
        return false;
    }
    
    // تعيين الشاشة كجذر العرض
    sadui_set_root(newState->rootWidget);
    
    // بدء الانتقال
    if (transition != SADNAV_TRANSITION_NONE && g_nav.transitionDuration > 0) {
        g_nav.isTransitioning = true;
        g_nav.currentTransition = transition;
        g_nav.transitionElapsed = 0;
        g_nav.outgoingRoute = current;
        g_nav.incomingRoute = newState;
        newState->transitionProgress = 0.0f;
        newState->isAnimating = true;
    } else {
        newState->transitionProgress = 1.0f;
        newState->isAnimating = false;
    }
    
    g_nav.stackSize++;
    LOGI("انتقال إلى: %s (الكومة: %d)", routeName, g_nav.stackSize);
    
    return true;
}

bool sadnav_push(const char* routeName) {
    const SadRouteDefinition* route = find_route(routeName);
    SadNavTransition trans = route ? route->defaultTransition : g_nav.defaultTransition;
    return push_route_internal(routeName, NULL, trans);
}

bool sadnav_push_params(const char* routeName, const SadNavParams* params) {
    const SadRouteDefinition* route = find_route(routeName);
    SadNavTransition trans = route ? route->defaultTransition : g_nav.defaultTransition;
    return push_route_internal(routeName, params, trans);
}

bool sadnav_push_transition(const char* routeName, SadNavTransition transition) {
    return push_route_internal(routeName, NULL, transition);
}

bool sadnav_pop(void) {
    if (!g_initialized || g_nav.stackSize <= 1) {
        LOGI("لا يمكن الرجوع — هذه الشاشة الأخيرة");
        return false;
    }
    
    SadRouteState* current = get_current_state();
    if (!current) return false;
    
    // التحقق من إمكانية المغادرة
    if (current->definition && current->definition->canLeave) {
        if (!current->definition->canLeave()) {
            LOGI("تم منع المغادرة من الشاشة");
            return false;
        }
    }
    
    // تنظيف الشاشة الحالية
    if (current->definition && current->definition->dispose) {
        current->definition->dispose(current->rootWidget);
    }
    
    // تدمير widget
    sadui_destroy(current->rootWidget);
    
    g_nav.stackSize--;
    
    // تفعيل الشاشة السابقة
    SadRouteState* previous = get_current_state();
    if (previous) {
        previous->isActive = true;
        // تعيين كـ root
        sadui_set_root(previous->rootWidget);
    }
    
    LOGI("رجوع للشاشة السابقة (الكومة: %d)", g_nav.stackSize);
    return true;
}

bool sadnav_pop_result(const char* result) {
    // TODO: تمرير النتيجة للشاشة السابقة
    return sadnav_pop();
}

bool sadnav_replace(const char* routeName) {
    if (!g_initialized) return false;
    
    // حذف الشاشة الحالية ثم الانتقال
    SadRouteState* current = get_current_state();
    if (current) {
        if (current->definition && current->definition->dispose) {
            current->definition->dispose(current->rootWidget);
        }
        sadui_destroy(current->rootWidget);
        g_nav.stackSize--;
    }
    
    return sadnav_push(routeName);
}

bool sadnav_clear_and_push(const char* routeName) {
    // مسح جميع الشاشات
    while (g_nav.stackSize > 0) {
        SadRouteState* state = get_current_state();
        if (state) {
            if (state->definition && state->definition->dispose) {
                state->definition->dispose(state->rootWidget);
            }
            sadui_destroy(state->rootWidget);
        }
        g_nav.stackSize--;
    }
    
    return sadnav_push(routeName);
}

bool sadnav_pop_until(const char* routeName) {
    while (g_nav.stackSize > 1) {
        SadRouteState* current = get_current_state();
        if (current && current->definition && 
            strcmp(current->definition->name, routeName) == 0) {
            return true; // وصلنا للشاشة المطلوبة
        }
        sadnav_pop();
    }
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  استعلامات
// ═══════════════════════════════════════════════════════════════════════════════

bool sadnav_can_pop(void) {
    return g_initialized && g_nav.stackSize > 1;
}

const char* sadnav_current_route(void) {
    SadRouteState* state = get_current_state();
    if (state && state->definition) {
        return state->definition->name;
    }
    return NULL;
}

int sadnav_stack_size(void) {
    return g_nav.stackSize;
}

const char* sadnav_get_param(const char* key) {
    SadRouteState* state = get_current_state();
    if (!state || !key) return NULL;
    return sadnav_params_get(&state->params, key);
}

void sadnav_set_result(const char* key, const char* value) {
    // TODO: تخزين النتيجة للشاشة السابقة
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تحديث ورسم
// ═══════════════════════════════════════════════════════════════════════════════

void sadnav_update(float deltaTime) {
    if (!g_initialized || !g_nav.isTransitioning) return;
    
    g_nav.transitionElapsed += deltaTime;
    float t = g_nav.transitionElapsed / g_nav.transitionDuration;
    
    if (t >= 1.0f) {
        t = 1.0f;
        g_nav.isTransitioning = false;
        
        if (g_nav.incomingRoute) {
            g_nav.incomingRoute->transitionProgress = 1.0f;
            g_nav.incomingRoute->isAnimating = false;
        }
        
        g_nav.outgoingRoute = NULL;
        g_nav.incomingRoute = NULL;
    } else {
        float eased = ease_out_cubic(t);
        
        if (g_nav.incomingRoute) {
            g_nav.incomingRoute->transitionProgress = eased;
        }
    }
}

SadWidgetId sadnav_get_current_root(void) {
    SadRouteState* state = get_current_state();
    return state ? state->rootWidget : 0;
}

bool sadnav_handle_back(void) {
    if (!g_initialized) return false;
    
    if (sadnav_can_pop()) {
        return sadnav_pop();
    }
    
    // لا يمكن الرجوع — إشارة للخروج من التطبيق
    return false;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إعدادات
// ═══════════════════════════════════════════════════════════════════════════════

void sadnav_set_transition_duration(float seconds) {
    if (seconds >= 0) {
        g_nav.transitionDuration = seconds;
    }
}

void sadnav_set_default_transition(SadNavTransition transition) {
    g_nav.defaultTransition = transition;
}

void sadnav_set_rtl(bool rtl) {
    g_nav.rtl = rtl;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  أدوات المعاملات
// ═══════════════════════════════════════════════════════════════════════════════

SadNavParams sadnav_params_create(void) {
    SadNavParams params = {0};
    return params;
}

void sadnav_params_add(SadNavParams* params, const char* key, const char* value) {
    if (!params || !key || params->count >= SADNAV_MAX_PARAMS) return;
    
    SadNavParam* param = &params->params[params->count];
    strncpy(param->key, key, sizeof(param->key) - 1);
    if (value) {
        strncpy(param->value, value, sizeof(param->value) - 1);
    }
    params->count++;
}

const char* sadnav_params_get(const SadNavParams* params, const char* key) {
    if (!params || !key) return NULL;
    
    for (int i = 0; i < params->count; i++) {
        if (strcmp(params->params[i].key, key) == 0) {
            return params->params[i].value;
        }
    }
    return NULL;
}
