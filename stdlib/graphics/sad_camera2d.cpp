// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ نظام الكاميرا ثنائي الأبعاد — لغة ص
// 2D Camera System Implementation — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام كاميرا رياضي بحت — لا يعتمد على SDL أو أي مكتبة رسومية
// يُدير الحالة والتحويلات الرياضية فقط
// ═══════════════════════════════════════════════════════════════════════════════

#include "graphics/sad_camera2d.h"
#include <cmath>
#include <algorithm>
#include <random>
#include <unordered_map>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace sad { namespace camera2d {

// ═══════════════════════════════════════════════════════════════════
// هيكل الكاميرا الداخلي — يحتوي على كل حالة الكاميرا
// ═══════════════════════════════════════════════════════════════════
struct Camera {
    // --- الموضع والعرض ---
    float x = 0.0f;               // موضع المركز في العالم — المحور الأفقي
    float y = 0.0f;               // موضع المركز في العالم — المحور العمودي
    float zoom = 1.0f;            // مستوى التكبير (1.0 = عادي)
    float rotation = 0.0f;        // زاوية الدوران بالدرجات
    int viewportW = 800;          // عرض نافذة العرض بالبكسل
    int viewportH = 600;          // ارتفاع نافذة العرض بالبكسل

    // --- حدود التكبير ---
    float minZoom = 0.1f;         // أدنى مستوى تكبير مسموح
    float maxZoom = 10.0f;        // أقصى مستوى تكبير مسموح

    // --- نظام المتابعة ---
    bool following = false;       // هل الكاميرا تتابع هدفاً؟
    float targetX = 0.0f;         // موضع الهدف — أفقي
    float targetY = 0.0f;         // موضع الهدف — عمودي
    float smoothing = 0.1f;       // معامل التنعيم (0 = فوري، 1 = بطيء جداً)

    // --- النطاق الميت ---
    float deadZoneW = 0.0f;       // عرض النطاق الميت — المنطقة التي لا تتحرك فيها الكاميرا
    float deadZoneH = 0.0f;       // ارتفاع النطاق الميت

    // --- النظر للأمام ---
    float lookAheadAmount = 0.0f; // مقدار النظر في اتجاه الحركة
    float lookAheadX = 0.0f;      // إزاحة النظر الحالية — أفقي
    float lookAheadY = 0.0f;      // إزاحة النظر الحالية — عمودي

    // --- حدود الكاميرا ---
    bool hasBounds = false;       // هل توجد حدود مفعّلة؟
    float boundsLeft = 0.0f;      // الحد الأيسر
    float boundsTop = 0.0f;       // الحد العلوي
    float boundsRight = 0.0f;     // الحد الأيمن
    float boundsBottom = 0.0f;    // الحد السفلي

    // --- الاهتزاز ---
    float shakeDuration = 0.0f;   // المدة الإجمالية للاهتزاز بالثواني
    float shakeTimer = 0.0f;      // المؤقت الحالي للاهتزاز
    float shakeIntensity = 0.0f;  // قوة الاهتزاز بالبكسل
    float shakeOffsetX = 0.0f;    // إزاحة الاهتزاز الحالية — أفقي
    float shakeOffsetY = 0.0f;    // إزاحة الاهتزاز الحالية — عمودي

    // --- الحركة السينمائية ---
    bool panning = false;         // هل الكاميرا تتحرك سينمائياً؟
    float panFromX = 0.0f;        // نقطة البداية — أفقي
    float panFromY = 0.0f;        // نقطة البداية — عمودي
    float panToX = 0.0f;          // نقطة النهاية — أفقي
    float panToY = 0.0f;          // نقطة النهاية — عمودي
    float panDuration = 0.0f;     // مدة الحركة بالثواني
    float panTimer = 0.0f;        // المؤقت الحالي للحركة
};

// ═══════════════════════════════════════════════════════════════════
// الحالة العامة — تخزين جميع الكاميرات
// ═══════════════════════════════════════════════════════════════════

// خريطة الكاميرات — المعرّف يربط بالكاميرا
static std::unordered_map<int, Camera> g_cameras;

// العدّاد التلقائي لتوليد معرّفات فريدة
static int g_nextCamId = 1;

// معرّف الكاميرا النشطة — تُستخدم مع beginCamera/endCamera
static int g_activeCamId = -1;

// مولّد الأرقام العشوائية — يُستخدم للاهتزاز
static std::mt19937 g_rng(std::random_device{}());
static std::uniform_real_distribution<float> g_dist(-1.0f, 1.0f);

// ═══════════════════════════════════════════════════════════════════
// دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════

// البحث عن كاميرا بالمعرّف — تُرجع مؤشراً أو nullptr
static Camera* findCamera(int camId) {
    auto it = g_cameras.find(camId);
    if (it == g_cameras.end()) return nullptr;
    return &it->second;
}

// تقييد قيمة ضمن نطاق محدد
static float clampf(float val, float lo, float hi) {
    return std::max(lo, std::min(val, hi));
}

// الاستيفاء الخطي بين قيمتين
static float lerpf(float a, float b, float t) {
    return a + (b - a) * t;
}

// ═══════════════════════════════════════════════════════════════════
// إنشاء وحذف الكاميرات
// ═══════════════════════════════════════════════════════════════════

int createCamera(int viewportW, int viewportH) {
    int id = g_nextCamId++;
    Camera cam;
    cam.viewportW = viewportW;
    cam.viewportH = viewportH;
    // القيم الافتراضية مُعيَّنة في تعريف الهيكل
    g_cameras[id] = cam;
    return id;
}

void destroyCamera(int camId) {
    // إذا كانت الكاميرا المحذوفة هي النشطة — نُلغي التفعيل
    if (g_activeCamId == camId) {
        g_activeCamId = -1;
    }
    g_cameras.erase(camId);
}

// ═══════════════════════════════════════════════════════════════════
// إدارة الموضع
// ═══════════════════════════════════════════════════════════════════

void setCamPosition(int camId, float x, float y) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->x = x;
    cam->y = y;
}

float getCamX(int camId) {
    Camera* cam = findCamera(camId);
    return cam ? cam->x : 0.0f;
}

float getCamY(int camId) {
    Camera* cam = findCamera(camId);
    return cam ? cam->y : 0.0f;
}

void moveCam(int camId, float dx, float dy) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->x += dx;
    cam->y += dy;
}

// ═══════════════════════════════════════════════════════════════════
// التكبير والتصغير
// ═══════════════════════════════════════════════════════════════════

void setCamZoom(int camId, float zoom) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    // تقييد التكبير ضمن الحدود المسموحة
    cam->zoom = clampf(zoom, cam->minZoom, cam->maxZoom);
}

float getCamZoom(int camId) {
    Camera* cam = findCamera(camId);
    return cam ? cam->zoom : 1.0f;
}

void setCamZoomLimits(int camId, float minZoom, float maxZoom) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->minZoom = minZoom;
    cam->maxZoom = maxZoom;
    // إعادة تقييد التكبير الحالي بالحدود الجديدة
    cam->zoom = clampf(cam->zoom, minZoom, maxZoom);
}

// ═══════════════════════════════════════════════════════════════════
// الدوران
// ═══════════════════════════════════════════════════════════════════

void setCamRotation(int camId, float degrees) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->rotation = degrees;
}

float getCamRotation(int camId) {
    Camera* cam = findCamera(camId);
    return cam ? cam->rotation : 0.0f;
}

// ═══════════════════════════════════════════════════════════════════
// نظام المتابعة — الكاميرا تتبع هدفاً متحركاً
// ═══════════════════════════════════════════════════════════════════

void followTarget(int camId, float targetX, float targetY, float smoothing) {
    Camera* cam = findCamera(camId);
    if (!cam) return;

    cam->following = true;
    cam->targetX = targetX;
    cam->targetY = targetY;
    // تقييد معامل التنعيم بين 0 و 1
    cam->smoothing = clampf(smoothing, 0.0f, 1.0f);
}

void setDeadZone(int camId, float width, float height) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->deadZoneW = width;
    cam->deadZoneH = height;
}

void setLookAhead(int camId, float amount) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->lookAheadAmount = amount;
}

// ═══════════════════════════════════════════════════════════════════
// حدود الكاميرا — لمنع الخروج عن الخريطة
// ═══════════════════════════════════════════════════════════════════

void setCamBounds(int camId, float left, float top, float right, float bottom) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->hasBounds = true;
    cam->boundsLeft = left;
    cam->boundsTop = top;
    cam->boundsRight = right;
    cam->boundsBottom = bottom;
}

void clearCamBounds(int camId) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->hasBounds = false;
}

// ═══════════════════════════════════════════════════════════════════
// اهتزاز الشاشة — تأثير بصري للصدمات والانفجارات
// ═══════════════════════════════════════════════════════════════════

void shakeCamera(int camId, float duration, float intensity) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->shakeDuration = duration;
    cam->shakeTimer = duration;
    cam->shakeIntensity = intensity;
}

void stopShake(int camId) {
    Camera* cam = findCamera(camId);
    if (!cam) return;
    cam->shakeDuration = 0.0f;
    cam->shakeTimer = 0.0f;
    cam->shakeIntensity = 0.0f;
    cam->shakeOffsetX = 0.0f;
    cam->shakeOffsetY = 0.0f;
}

// ═══════════════════════════════════════════════════════════════════
// الحركة السينمائية — تحريك الكاميرا بسلاسة بين نقطتين
// ═══════════════════════════════════════════════════════════════════

void panTo(int camId, float targetX, float targetY, float duration) {
    Camera* cam = findCamera(camId);
    if (!cam) return;

    cam->panning = true;
    cam->panFromX = cam->x;
    cam->panFromY = cam->y;
    cam->panToX = targetX;
    cam->panToY = targetY;
    // ضمان ألا تكون المدة صفراً — تجنب القسمة على صفر
    cam->panDuration = (duration > 0.001f) ? duration : 0.001f;
    cam->panTimer = 0.0f;
}

bool isPanning(int camId) {
    Camera* cam = findCamera(camId);
    if (!cam) return false;
    return cam->panning;
}

// ═══════════════════════════════════════════════════════════════════
// التحديث الرئيسي — يُستدعى كل إطار
// يُعالج: المتابعة، الاهتزاز، الحركة السينمائية، الحدود
// ═══════════════════════════════════════════════════════════════════

void updateCamera(int camId, float dt) {
    Camera* cam = findCamera(camId);
    if (!cam) return;

    // ─── المرحلة 1: معالجة المتابعة ───
    if (cam->following) {
        float dx = cam->targetX - cam->x;
        float dy = cam->targetY - cam->y;

        // التحقق من النطاق الميت — لا نتحرك إذا كان الهدف قريباً كفاية
        float halfDeadW = cam->deadZoneW * 0.5f;
        float halfDeadH = cam->deadZoneH * 0.5f;

        // حساب المسافة الفعلية خارج النطاق الميت
        float moveX = 0.0f;
        float moveY = 0.0f;

        if (std::abs(dx) > halfDeadW) {
            // الهدف خارج النطاق الميت أفقياً — نتحرك نحوه
            moveX = dx - (dx > 0 ? halfDeadW : -halfDeadW);
        }
        if (std::abs(dy) > halfDeadH) {
            // الهدف خارج النطاق الميت عمودياً — نتحرك نحوه
            moveY = dy - (dy > 0 ? halfDeadH : -halfDeadH);
        }

        // استيفاء خطي نحو الهدف — التنعيم يجعل الحركة أكثر سلاسة
        float factor = 1.0f - std::pow(1.0f - cam->smoothing, dt * 60.0f);
        cam->x += moveX * factor;
        cam->y += moveY * factor;

        // حساب النظر للأمام — الكاميرا تنظر في اتجاه الحركة
        if (cam->lookAheadAmount > 0.0f) {
            float targetLAX = (dx > 0.5f) ? cam->lookAheadAmount :
                              (dx < -0.5f) ? -cam->lookAheadAmount : 0.0f;
            float targetLAY = (dy > 0.5f) ? cam->lookAheadAmount :
                              (dy < -0.5f) ? -cam->lookAheadAmount : 0.0f;

            // تنعيم النظر للأمام لتجنب القفز المفاجئ
            cam->lookAheadX = lerpf(cam->lookAheadX, targetLAX, factor * 0.5f);
            cam->lookAheadY = lerpf(cam->lookAheadY, targetLAY, factor * 0.5f);

            cam->x += cam->lookAheadX;
            cam->y += cam->lookAheadY;
        }
    }

    // ─── المرحلة 2: معالجة الاهتزاز ───
    if (cam->shakeTimer > 0.0f) {
        cam->shakeTimer -= dt;

        if (cam->shakeTimer > 0.0f) {
            // حساب نسبة الاضمحلال — الاهتزاز يضعف مع الوقت
            float decay = cam->shakeTimer / cam->shakeDuration;
            float currentIntensity = cam->shakeIntensity * decay;

            // توليد إزاحة عشوائية
            cam->shakeOffsetX = g_dist(g_rng) * currentIntensity;
            cam->shakeOffsetY = g_dist(g_rng) * currentIntensity;
        } else {
            // انتهى الاهتزاز — إعادة تصفير كل شيء
            cam->shakeTimer = 0.0f;
            cam->shakeOffsetX = 0.0f;
            cam->shakeOffsetY = 0.0f;
        }
    }

    // ─── المرحلة 3: معالجة الحركة السينمائية ───
    if (cam->panning) {
        cam->panTimer += dt;

        if (cam->panTimer >= cam->panDuration) {
            // وصلنا للنهاية — تعيين الموضع النهائي بدقة
            cam->x = cam->panToX;
            cam->y = cam->panToY;
            cam->panning = false;
        } else {
            // استيفاء بين نقطة البداية والنهاية
            // نستخدم دالة تنعيم (ease in-out) لحركة أكثر طبيعية
            float t = cam->panTimer / cam->panDuration;
            float smoothT = t * t * (3.0f - 2.0f * t); // دالة هيرمايت المكعبة

            cam->x = lerpf(cam->panFromX, cam->panToX, smoothT);
            cam->y = lerpf(cam->panFromY, cam->panToY, smoothT);
        }
    }

    // ─── المرحلة 4: تقييد الموضع ضمن الحدود ───
    if (cam->hasBounds) {
        // حساب نصف حجم العرض المرئي مع الأخذ بالتكبير بعين الاعتبار
        float halfViewW = (cam->viewportW * 0.5f) / cam->zoom;
        float halfViewH = (cam->viewportH * 0.5f) / cam->zoom;

        // تقييد المركز بحيث لا يخرج المنظر عن الحدود
        cam->x = clampf(cam->x, cam->boundsLeft + halfViewW, cam->boundsRight - halfViewW);
        cam->y = clampf(cam->y, cam->boundsTop + halfViewH, cam->boundsBottom - halfViewH);
    }
}

// ═══════════════════════════════════════════════════════════════════
// تحويل الإحداثيات — بين العالم والشاشة
// ═══════════════════════════════════════════════════════════════════

float worldToScreenX(int camId, float worldX) {
    Camera* cam = findCamera(camId);
    if (!cam) return worldX;

    // الموضع الفعلي للكاميرا مع إضافة إزاحة الاهتزاز
    float effectiveX = cam->x + cam->shakeOffsetX;

    // التحويل: (إحداثيات_العالم - موضع_الكاميرا) × التكبير + نصف_العرض
    return (worldX - effectiveX) * cam->zoom + cam->viewportW * 0.5f;
}

float worldToScreenY(int camId, float worldY) {
    Camera* cam = findCamera(camId);
    if (!cam) return worldY;

    float effectiveY = cam->y + cam->shakeOffsetY;
    return (worldY - effectiveY) * cam->zoom + cam->viewportH * 0.5f;
}

float screenToWorldX(int camId, float screenX) {
    Camera* cam = findCamera(camId);
    if (!cam) return screenX;

    // التحويل العكسي: (إحداثيات_الشاشة - نصف_العرض) / التكبير + موضع_الكاميرا
    float effectiveX = cam->x + cam->shakeOffsetX;
    return (screenX - cam->viewportW * 0.5f) / cam->zoom + effectiveX;
}

float screenToWorldY(int camId, float screenY) {
    Camera* cam = findCamera(camId);
    if (!cam) return screenY;

    float effectiveY = cam->y + cam->shakeOffsetY;
    return (screenY - cam->viewportH * 0.5f) / cam->zoom + effectiveY;
}

// ═══════════════════════════════════════════════════════════════════
// تطبيق التحويل — لتفعيل الكاميرا أثناء الرسم
// ═══════════════════════════════════════════════════════════════════

void beginCamera(int camId) {
    Camera* cam = findCamera(camId);
    if (!cam) return;

    // تعيين هذه الكاميرا كالنشطة — المحرك يستخدمها لتحويل الرسم
    g_activeCamId = camId;
}

void endCamera() {
    // إلغاء تفعيل الكاميرا — العودة لرسم بإحداثيات الشاشة المباشرة
    g_activeCamId = -1;
}

// ═══════════════════════════════════════════════════════════════════
// إزاحة الكاميرا — للاستخدام اليدوي مع محرك الرسم
// ═══════════════════════════════════════════════════════════════════

float getOffsetX(int camId) {
    Camera* cam = findCamera(camId);
    if (!cam) return 0.0f;

    // الإزاحة الفعلية = مركز الكاميرا + الاهتزاز
    return cam->x + cam->shakeOffsetX;
}

float getOffsetY(int camId) {
    Camera* cam = findCamera(camId);
    if (!cam) return 0.0f;

    return cam->y + cam->shakeOffsetY;
}

}} // namespace sad::camera2d
