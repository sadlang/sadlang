// ═══════════════════════════════════════════════════════════════════════════════
// نظام الخرائط المربعة — لغة ص
// Tilemap System — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام خرائط مربعة مدمج للألعاب ثنائية الأبعاد — Flutter لا يملك أي نظام مشابه!
// في Flutter تحتاج flame_tiled أو bonfire وهي مكتبات خارجية ثقيلة.
//
// الميزات:
//   🗺️ خرائط بمربعات (Tiles) قابلة للتخصيص بالحجم واللون
//   🏗️ طبقات متعددة (Layers) — خلفية، أرضية، عقبات، واجهة أمامية
//   🎨 تلوين المربعات بألوان مختلفة لكل نوع
//   🚶 كشف التصادم التلقائي مع المربعات الصلبة
//   📷 دعم الإزاحة (Scrolling/Camera Offset) للخرائط الكبيرة
//   🔍 تحويل بين إحداثيات الشاشة وإحداثيات الخريطة
//   💾 تحميل وحفظ الخرائط كنصوص
//
// الاستخدام:
//   مثالي للألعاب مثل: ماريو، زيلدا، لعبة متاهة، لعبة مغامرات
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

namespace sad { namespace tilemap {

// ═══════════════════════════════════════════════════════════════════
// أنواع المربعات — كل مربع له نوع ولون وخصائص تصادم
// ═══════════════════════════════════════════════════════════════════

// لون المربع
struct TileColor {
    uint8_t r = 100, g = 100, b = 100, a = 255;
};

// تعريف نوع مربع — لكل رقم (نوع) خصائص محددة
struct TileType {
    int id = 0;                  // رقم النوع (0 = فارغ)
    std::string name;            // اسم النوع (مثلاً: "جدار"، "أرضية"، "ماء")
    TileColor color;             // اللون عند الرسم
    bool solid = false;          // هل هو صلب؟ (يمنع المرور)
    bool visible = true;         // هل يُرسم؟
};

// ═══════════════════════════════════════════════════════════════════
// هياكل البيانات الأساسية
// ═══════════════════════════════════════════════════════════════════

// طبقة واحدة في الخريطة
struct TileLayer {
    std::string name;                    // اسم الطبقة (مثلاً: "أرضية")
    std::vector<std::vector<int>> data;  // مصفوفة ثنائية الأبعاد من أرقام الأنواع
    float opacity = 1.0f;               // شفافية الطبقة
    bool visible = true;                 // هل الطبقة مرئية؟
};

// ═══════════════════════════════════════════════════════════════════
// واجهة برمجة التطبيقات — API
// ═══════════════════════════════════════════════════════════════════

// ─── إنشاء الخريطة ───

// إنشاء خريطة جديدة — تُرجع معرّف الخريطة
// عرض_مربعات × ارتفاع_مربعات = عدد المربعات
// حجم_المربع = حجم كل مربع بالبكسل
int createTilemap(int tilesWide, int tilesHigh, int tileSize = 32);

// حذف خريطة
void destroyTilemap(int id);

// ─── تعريف أنواع المربعات ───

// تسجيل نوع مربع جديد مع لونه وخصائصه
void defineTileType(int mapId, int typeId, const std::string& name,
                    uint8_t r, uint8_t g, uint8_t b, bool solid);

// ─── تعديل مربعات الخريطة ───

// تعيين مربع في موضع (صف، عمود) بنوع معين (في الطبقة 0 افتراضياً)
void setTile(int mapId, int col, int row, int typeId, int layer = 0);

// الحصول على نوع المربع في موضع معين
int getTile(int mapId, int col, int row, int layer = 0);

// ملء منطقة مستطيلة بنوع معين
void fillRegion(int mapId, int col, int row, int w, int h, int typeId, int layer = 0);

// ملء الخريطة بالكامل بنوع واحد
void fillAll(int mapId, int typeId, int layer = 0);

// ─── الطبقات ───

// إضافة طبقة جديدة — تُرجع رقم الطبقة
int addLayer(int mapId, const std::string& name);

// إظهار/إخفاء طبقة
void setLayerVisible(int mapId, int layer, bool visible);

// ─── الإزاحة والكاميرا ───

// تعيين إزاحة الكاميرا (للتمرير)
void setCameraOffset(int mapId, float offsetX, float offsetY);

// الحصول على إزاحة الكاميرا
float getCameraX(int mapId);
float getCameraY(int mapId);

// ─── تحويل الإحداثيات ───

// تحويل إحداثيات الشاشة إلى إحداثيات المربع
int screenToTileX(int mapId, float screenX);
int screenToTileY(int mapId, float screenY);

// تحويل إحداثيات المربع إلى إحداثيات الشاشة
float tileToScreenX(int mapId, int col);
float tileToScreenY(int mapId, int row);

// ─── كشف التصادم ───

// هل المربع في هذا الموضع صلب؟
bool isSolid(int mapId, int col, int row);

// هل يمكن التحرك إلى إحداثيات الشاشة هذه؟
bool canMoveTo(int mapId, float worldX, float worldY, float width, float height);

// ─── الرسم ───

// رسم الخريطة على الشاشة
void renderTilemap(void* renderer, int mapId);

// ─── معلومات ───
int getMapWidth(int mapId);
int getMapHeight(int mapId);
int getTileSize(int mapId);

}} // namespace sad::tilemap
