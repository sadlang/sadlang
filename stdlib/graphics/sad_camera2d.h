// ═══════════════════════════════════════════════════════════════════════════════
// نظام الكاميرا ثنائي الأبعاد — لغة ص
// 2D Camera System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام كاميرا ثنائي الأبعاد متقدم — Flutter لا يملك مفهوم الكاميرا إطلاقاً!
// في Flutter عليك بناء نظام تمرير يدوياً مع Transform widgets.
//
// الميزات:
//   📷 نافذة عرض (Viewport) مع موضع وتكبير
//   🎯 متابعة هدف (Follow Target) — مع تنعيم وتأخير اختياري
//   📐 حدود الكاميرا (Bounds) — لمنع الخروج عن الخريطة
//   💫 اهتزاز الشاشة (Screen Shake) — مع تحكم بالقوة والمدة
//   🔄 دوران الكاميرا — تدوير حول المركز
//   🔍 تكبير وتصغير (Zoom) — مع حدود دنيا وقصوى
//   📌 عرض متعدد الشاشات (Split Screen) — كاميرتين أو أكثر
//   🎬 حركة سينمائية — تحريك الكاميرا بسلاسة بين نقطتين
//
// كل هذا بدون أي مكتبة خارجية — مدمج في المحرك!
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <cstdint>
#include <string>

namespace sad { namespace camera2d {

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — API
// ═══════════════════════════════════════════════════════════════════

// ─── إنشاء الكاميرا ───

// إنشاء كاميرا جديدة — تُرجع معرّف الكاميرا
// عرض ونافذة العرض (عادةً = حجم الشاشة)
int createCamera(int viewportW, int viewportH);

// حذف كاميرا
void destroyCamera(int camId);

// ─── الموضع ───

// تعيين موضع الكاميرا (المركز)
void setCamPosition(int camId, float x, float y);

// الحصول على موضع الكاميرا
float getCamX(int camId);
float getCamY(int camId);

// تحريك الكاميرا بمقدار نسبي
void moveCam(int camId, float dx, float dy);

// ─── التكبير ───

// تعيين مستوى التكبير (1.0 = عادي، 2.0 = مضاعف)
void setCamZoom(int camId, float zoom);
float getCamZoom(int camId);

// تعيين حدود التكبير
void setCamZoomLimits(int camId, float minZoom, float maxZoom);

// ─── الدوران ───

// تعيين زاوية الدوران (بالدرجات)
void setCamRotation(int camId, float degrees);
float getCamRotation(int camId);

// ─── المتابعة ───

// متابعة هدف — الكاميرا تتحرك تلقائياً نحو النقطة المعطاة
void followTarget(int camId, float targetX, float targetY, float smoothing);

// تعيين نطاق ميت (Dead Zone) — منطقة وسط الشاشة لا تتحرك فيها الكاميرا
void setDeadZone(int camId, float width, float height);

// تعيين نقطة نظر أمامية (Look Ahead) — الكاميرا تنظر في اتجاه الحركة
void setLookAhead(int camId, float amount);

// ─── الحدود ───

// تعيين حدود الكاميرا (لمنع الخروج عن الخريطة)
void setCamBounds(int camId, float left, float top, float right, float bottom);

// إلغاء الحدود
void clearCamBounds(int camId);

// ─── الاهتزاز (Screen Shake) ───

// بدء اهتزاز — المدة بالثواني، القوة بالبكسل
void shakeCamera(int camId, float duration, float intensity);

// إيقاف الاهتزاز فوراً
void stopShake(int camId);

// ─── الحركة السينمائية ───

// تحريك الكاميرا بسلاسة من الموضع الحالي إلى (هدف_س، هدف_ص) خلال المدة المحددة
void panTo(int camId, float targetX, float targetY, float duration);

// هل الكاميرا تتحرك حالياً؟
bool isPanning(int camId);

// ─── التحديث ───

// تحديث الكاميرا — يُستدعى كل إطار
// يُعالج: المتابعة، الاهتزاز، الحركة السينمائية
void updateCamera(int camId, float dt);

// ─── تحويل الإحداثيات ───

// تحويل من إحداثيات العالم إلى إحداثيات الشاشة
float worldToScreenX(int camId, float worldX);
float worldToScreenY(int camId, float worldY);

// تحويل من إحداثيات الشاشة إلى إحداثيات العالم
float screenToWorldX(int camId, float screenX);
float screenToWorldY(int camId, float screenY);

// ─── تطبيق التحويل ───

// تعيين هذه الكاميرا كالكاميرا النشطة — كل الرسم بعدها يُحوَّل
void beginCamera(int camId);

// إنهاء تحويل الكاميرا — العودة للإحداثيات العادية
void endCamera();

// الحصول على إزاحة الكاميرا الحالية (للاستخدام اليدوي مع SDL)
float getOffsetX(int camId);
float getOffsetY(int camId);

}} // namespace sad::camera2d
