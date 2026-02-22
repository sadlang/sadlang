// ═══════════════════════════════════════════════════════════════════════════════
// تنفيذ نظام الخرائط المربعة — لغة ص
// Tilemap System Implementation — SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
//
// نظام خرائط مربعة كامل يدعم طبقات متعددة، كاميرا، تصادم، ورسم بالألوان
// يستخدم SDL2 للرسم — المُقدِّم (renderer) يُمرَّر كـ void* ويُحوَّل داخلياً
// ═══════════════════════════════════════════════════════════════════════════════

#include "graphics/sad_tilemap.h"
#include <SDL.h>
#include <algorithm>
#include <cstring>
#include <cmath>

namespace sad { namespace tilemap {

// ═══════════════════════════════════════════════════════════════════
// الحالة العامة — تخزين جميع الخرائط بمعرّفات فريدة
// ═══════════════════════════════════════════════════════════════════

// هيكل الخريطة الداخلي — يحتوي على كل بيانات الخريطة الواحدة
struct Tilemap {
    int tilesWide;                                   // عرض الخريطة بالمربعات
    int tilesHigh;                                   // ارتفاع الخريطة بالمربعات
    int tileSize;                                    // حجم المربع الواحد بالبكسل
    std::unordered_map<int, TileType> tileTypes;     // أنواع المربعات (رقم النوع → معلوماته)
    std::vector<TileLayer> layers;                   // الطبقات (الطبقة 0 هي الأساسية)
    float cameraX;                                   // إزاحة الكاميرا الأفقية
    float cameraY;                                   // إزاحة الكاميرا العمودية
};

// خريطة عامة: معرّف الخريطة → بيانات الخريطة
static std::unordered_map<int, Tilemap> g_tilemaps;

// معرّف تلقائي متزايد — يبدأ من 1
static int g_nextId = 1;

// ═══════════════════════════════════════════════════════════════════
// دوال مساعدة داخلية
// ═══════════════════════════════════════════════════════════════════

// البحث عن خريطة بالمعرّف — تُرجع nullptr إذا لم تُوجد
static Tilemap* findMap(int id) {
    auto it = g_tilemaps.find(id);
    if (it == g_tilemaps.end()) return nullptr;
    return &it->second;
}

// التحقق من صلاحية الإحداثيات (عمود، صف) داخل حدود الخريطة
static bool isInBounds(const Tilemap& map, int col, int row) {
    return col >= 0 && col < map.tilesWide &&
           row >= 0 && row < map.tilesHigh;
}

// التحقق من وجود الطبقة في الخريطة
static bool isValidLayer(const Tilemap& map, int layer) {
    return layer >= 0 && layer < static_cast<int>(map.layers.size());
}

// إنشاء مصفوفة بيانات فارغة لطبقة جديدة (كل القيم = 0)
static std::vector<std::vector<int>> createEmptyData(int tilesWide, int tilesHigh) {
    return std::vector<std::vector<int>>(
        tilesHigh, std::vector<int>(tilesWide, 0)
    );
}

// ═══════════════════════════════════════════════════════════════════
// إنشاء وحذف الخرائط
// ═══════════════════════════════════════════════════════════════════

// إنشاء خريطة جديدة — تُسجَّل بمعرّف فريد وتُرجعه
// تُضاف الطبقة الأساسية (0) تلقائياً
// يُسجَّل النوع 0 (فارغ/شفاف) تلقائياً
int createTilemap(int tilesWide, int tilesHigh, int tileSize) {
    // التحقق من القيم المدخلة
    if (tilesWide <= 0) tilesWide = 1;
    if (tilesHigh <= 0) tilesHigh = 1;
    if (tileSize <= 0) tileSize = 32;

    int id = g_nextId++;

    // تهيئة الخريطة الجديدة
    Tilemap map;
    map.tilesWide = tilesWide;
    map.tilesHigh = tilesHigh;
    map.tileSize  = tileSize;
    map.cameraX   = 0.0f;
    map.cameraY   = 0.0f;

    // تسجيل النوع 0 — المربع الفارغ (شفاف، غير صلب، غير مرئي)
    TileType emptyType;
    emptyType.id      = 0;
    emptyType.name    = "فارغ";
    emptyType.color   = { 0, 0, 0, 0 };  // شفاف تماماً
    emptyType.solid   = false;
    emptyType.visible = false;
    map.tileTypes[0]  = emptyType;

    // إنشاء الطبقة الأساسية (الطبقة 0)
    TileLayer defaultLayer;
    defaultLayer.name    = "أساسية";
    defaultLayer.data    = createEmptyData(tilesWide, tilesHigh);
    defaultLayer.opacity = 1.0f;
    defaultLayer.visible = true;
    map.layers.push_back(std::move(defaultLayer));

    // تخزين الخريطة في الحالة العامة
    g_tilemaps[id] = std::move(map);

    return id;
}

// حذف خريطة من الذاكرة — تُزال من الخريطة العامة
void destroyTilemap(int id) {
    g_tilemaps.erase(id);
}

// ═══════════════════════════════════════════════════════════════════
// تعريف أنواع المربعات
// ═══════════════════════════════════════════════════════════════════

// تسجيل نوع مربع جديد بلونه واسمه وخصائصه
// إذا كان النوع موجوداً، يُحدَّث
void defineTileType(int mapId, int typeId, const std::string& name,
                    uint8_t r, uint8_t g, uint8_t b, bool solid) {
    Tilemap* map = findMap(mapId);
    if (!map) return;

    TileType type;
    type.id      = typeId;
    type.name    = name;
    type.color   = { r, g, b, 255 };
    type.solid   = solid;
    type.visible = true;

    map->tileTypes[typeId] = type;
}

// ═══════════════════════════════════════════════════════════════════
// تعديل المربعات — تعيين وقراءة وملء
// ═══════════════════════════════════════════════════════════════════

// تعيين نوع مربع في موضع محدد (عمود، صف) على طبقة معينة
// يتحقق من صلاحية الحدود والطبقة قبل التعديل
void setTile(int mapId, int col, int row, int typeId, int layer) {
    Tilemap* map = findMap(mapId);
    if (!map) return;

    // التحقق من صلاحية الطبقة والإحداثيات
    if (!isValidLayer(*map, layer)) return;
    if (!isInBounds(*map, col, row)) return;

    // تعيين القيمة في مصفوفة البيانات — الصف ثم العمود
    map->layers[layer].data[row][col] = typeId;
}

// قراءة نوع المربع في موضع محدد
// يُرجع 0 (فارغ) إذا كانت الإحداثيات خارج الحدود
int getTile(int mapId, int col, int row, int layer) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0;

    if (!isValidLayer(*map, layer)) return 0;
    if (!isInBounds(*map, col, row)) return 0;

    return map->layers[layer].data[row][col];
}

// ملء منطقة مستطيلة بنوع واحد — من (col, row) بعرض w وارتفاع h
void fillRegion(int mapId, int col, int row, int w, int h, int typeId, int layer) {
    Tilemap* map = findMap(mapId);
    if (!map) return;

    if (!isValidLayer(*map, layer)) return;

    // المرور على كل مربع في المنطقة المحددة
    for (int r = row; r < row + h; ++r) {
        for (int c = col; c < col + w; ++c) {
            if (isInBounds(*map, c, r)) {
                map->layers[layer].data[r][c] = typeId;
            }
        }
    }
}

// ملء الخريطة بالكامل بنوع واحد على طبقة محددة
void fillAll(int mapId, int typeId, int layer) {
    Tilemap* map = findMap(mapId);
    if (!map) return;

    if (!isValidLayer(*map, layer)) return;

    // ملء كل صف وعمود في الطبقة
    for (int r = 0; r < map->tilesHigh; ++r) {
        for (int c = 0; c < map->tilesWide; ++c) {
            map->layers[layer].data[r][c] = typeId;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════
// إدارة الطبقات
// ═══════════════════════════════════════════════════════════════════

// إضافة طبقة جديدة فارغة — تُرجع رقم الطبقة الجديدة
int addLayer(int mapId, const std::string& name) {
    Tilemap* map = findMap(mapId);
    if (!map) return -1;

    TileLayer newLayer;
    newLayer.name    = name;
    newLayer.data    = createEmptyData(map->tilesWide, map->tilesHigh);
    newLayer.opacity = 1.0f;
    newLayer.visible = true;

    map->layers.push_back(std::move(newLayer));

    // رقم الطبقة = الحجم الجديد - 1
    return static_cast<int>(map->layers.size()) - 1;
}

// تبديل حالة ظهور الطبقة (مرئية/مخفية)
void setLayerVisible(int mapId, int layer, bool visible) {
    Tilemap* map = findMap(mapId);
    if (!map) return;

    if (!isValidLayer(*map, layer)) return;

    map->layers[layer].visible = visible;
}

// ═══════════════════════════════════════════════════════════════════
// الكاميرا والإزاحة
// ═══════════════════════════════════════════════════════════════════

// تعيين إزاحة الكاميرا — تُستخدم لتمرير الخريطة على الشاشة
void setCameraOffset(int mapId, float offsetX, float offsetY) {
    Tilemap* map = findMap(mapId);
    if (!map) return;

    map->cameraX = offsetX;
    map->cameraY = offsetY;
}

// الحصول على إزاحة الكاميرا الأفقية
float getCameraX(int mapId) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0.0f;

    return map->cameraX;
}

// الحصول على إزاحة الكاميرا العمودية
float getCameraY(int mapId) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0.0f;

    return map->cameraY;
}

// ═══════════════════════════════════════════════════════════════════
// تحويل الإحداثيات — بين الشاشة والخريطة
// ═══════════════════════════════════════════════════════════════════

// تحويل إحداثي الشاشة الأفقي إلى عمود في الخريطة
// يُضاف إزاحة الكاميرا ثم يُقسَم على حجم المربع ويُقرَّب للأسفل
// النتيجة مقيدة بحدود الخريطة
int screenToTileX(int mapId, float screenX) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0;

    int col = static_cast<int>(std::floor((screenX + map->cameraX) / map->tileSize));

    // تقييد القيمة بحدود الخريطة
    return std::max(0, std::min(col, map->tilesWide - 1));
}

// تحويل إحداثي الشاشة العمودي إلى صف في الخريطة
int screenToTileY(int mapId, float screenY) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0;

    int row = static_cast<int>(std::floor((screenY + map->cameraY) / map->tileSize));

    // تقييد القيمة بحدود الخريطة
    return std::max(0, std::min(row, map->tilesHigh - 1));
}

// تحويل عمود في الخريطة إلى إحداثي أفقي على الشاشة
float tileToScreenX(int mapId, int col) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0.0f;

    return static_cast<float>(col * map->tileSize) - map->cameraX;
}

// تحويل صف في الخريطة إلى إحداثي عمودي على الشاشة
float tileToScreenY(int mapId, int row) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0.0f;

    return static_cast<float>(row * map->tileSize) - map->cameraY;
}

// ═══════════════════════════════════════════════════════════════════
// كشف التصادم — التحقق من المربعات الصلبة
// ═══════════════════════════════════════════════════════════════════

// هل المربع في الموضع (عمود، صف) صلب؟
// يفحص الطبقة 0 (الأساسية) ويتحقق من خاصية الصلابة للنوع
bool isSolid(int mapId, int col, int row) {
    Tilemap* map = findMap(mapId);
    if (!map) return false;

    // خارج الحدود يُعتبر صلباً (لمنع الخروج من الخريطة)
    if (!isInBounds(*map, col, row)) return true;

    // قراءة نوع المربع من الطبقة 0 (الأساسية)
    int typeId = map->layers[0].data[row][col];

    // البحث عن تعريف النوع
    auto it = map->tileTypes.find(typeId);
    if (it == map->tileTypes.end()) return false;

    return it->second.solid;
}

// هل يمكن التحرك إلى الموضع المحدد؟
// يفحص الزوايا الأربع للمستطيل المُعطى
// إذا كانت أي زاوية فوق مربع صلب، لا يمكن التحرك
bool canMoveTo(int mapId, float worldX, float worldY, float width, float height) {
    Tilemap* map = findMap(mapId);
    if (!map) return false;

    int ts = map->tileSize;

    // حساب أعمدة وصفوف الزوايا الأربع
    // الزاوية العلوية اليسرى
    int leftCol  = static_cast<int>(std::floor(worldX / ts));
    int topRow   = static_cast<int>(std::floor(worldY / ts));

    // الزاوية السفلية اليمنى — ننقص بمقدار صغير لتجنب الحافة
    int rightCol   = static_cast<int>(std::floor((worldX + width  - 0.01f) / ts));
    int bottomRow  = static_cast<int>(std::floor((worldY + height - 0.01f) / ts));

    // فحص كل الزوايا الأربع
    if (isSolid(mapId, leftCol,  topRow))    return false;  // أعلى يسار
    if (isSolid(mapId, rightCol, topRow))    return false;  // أعلى يمين
    if (isSolid(mapId, leftCol,  bottomRow)) return false;  // أسفل يسار
    if (isSolid(mapId, rightCol, bottomRow)) return false;  // أسفل يمين

    return true;
}

// ═══════════════════════════════════════════════════════════════════
// الرسم — رسم الخريطة على الشاشة باستخدام SDL2
// ═══════════════════════════════════════════════════════════════════

// رسم الخريطة كاملة بجميع الطبقات المرئية
// يستخدم الكاميرا لتحديد نطاق المربعات المرئية
// يرسم كل مربع كمستطيل ملون حسب نوعه
void renderTilemap(void* renderer, int mapId) {
    // التحقق من المدخلات
    if (!renderer) return;

    Tilemap* map = findMap(mapId);
    if (!map) return;

    SDL_Renderer* sdlRenderer = static_cast<SDL_Renderer*>(renderer);
    int ts = map->tileSize;

    // ───────────────────────────────────────────────────────
    // حساب نطاق المربعات المرئية على الشاشة (تحسين الأداء)
    // بدلاً من رسم الخريطة كاملة، نرسم فقط المربعات المرئية
    // ───────────────────────────────────────────────────────

    // الحصول على أبعاد نافذة العرض
    int viewportW = 0, viewportH = 0;
    SDL_GetRendererOutputSize(sdlRenderer, &viewportW, &viewportH);

    // حساب نطاق الأعمدة المرئية
    int startCol = static_cast<int>(std::floor(map->cameraX / ts));
    int endCol   = static_cast<int>(std::floor((map->cameraX + viewportW) / ts)) + 1;

    // حساب نطاق الصفوف المرئية
    int startRow = static_cast<int>(std::floor(map->cameraY / ts));
    int endRow   = static_cast<int>(std::floor((map->cameraY + viewportH) / ts)) + 1;

    // تقييد النطاق بحدود الخريطة
    startCol = std::max(0, startCol);
    startRow = std::max(0, startRow);
    endCol   = std::min(endCol, map->tilesWide);
    endRow   = std::min(endRow, map->tilesHigh);

    // ───────────────────────────────────────────────────────
    // رسم الطبقات — من الخلف إلى الأمام
    // ───────────────────────────────────────────────────────

    for (size_t layerIdx = 0; layerIdx < map->layers.size(); ++layerIdx) {
        const TileLayer& layer = map->layers[layerIdx];

        // تخطي الطبقات المخفية
        if (!layer.visible) continue;

        // رسم المربعات المرئية في هذه الطبقة
        for (int row = startRow; row < endRow; ++row) {
            for (int col = startCol; col < endCol; ++col) {
                int typeId = layer.data[row][col];

                // تخطي المربعات الفارغة (نوع 0)
                if (typeId == 0) continue;

                // البحث عن تعريف النوع
                auto typeIt = map->tileTypes.find(typeId);
                if (typeIt == map->tileTypes.end()) continue;

                const TileType& type = typeIt->second;

                // تخطي الأنواع غير المرئية
                if (!type.visible) continue;

                // حساب موضع المربع على الشاشة (مع إزاحة الكاميرا)
                int screenX = static_cast<int>(col * ts - map->cameraX);
                int screenY = static_cast<int>(row * ts - map->cameraY);

                // تعيين لون الرسم — مع مراعاة شفافية الطبقة
                uint8_t alpha = static_cast<uint8_t>(type.color.a * layer.opacity);
                SDL_SetRenderDrawColor(sdlRenderer,
                    type.color.r, type.color.g, type.color.b, alpha);

                // رسم المستطيل الملون
                SDL_Rect rect = { screenX, screenY, ts, ts };
                SDL_RenderFillRect(sdlRenderer, &rect);
            }
        }
    }

    // ───────────────────────────────────────────────────────
    // رسم خطوط الشبكة (اختياري — للمساعدة في التصميم)
    // خطوط رمادية شفافة بين المربعات
    // ───────────────────────────────────────────────────────

#ifdef SAD_TILEMAP_SHOW_GRID
    SDL_SetRenderDrawColor(sdlRenderer, 60, 60, 60, 80);

    // الخطوط العمودية
    for (int col = startCol; col <= endCol; ++col) {
        int x = static_cast<int>(col * ts - map->cameraX);
        int y1 = static_cast<int>(startRow * ts - map->cameraY);
        int y2 = static_cast<int>(endRow * ts - map->cameraY);
        SDL_RenderDrawLine(sdlRenderer, x, y1, x, y2);
    }

    // الخطوط الأفقية
    for (int row = startRow; row <= endRow; ++row) {
        int y = static_cast<int>(row * ts - map->cameraY);
        int x1 = static_cast<int>(startCol * ts - map->cameraX);
        int x2 = static_cast<int>(endCol * ts - map->cameraX);
        SDL_RenderDrawLine(sdlRenderer, x1, y, x2, y);
    }
#endif
}

// ═══════════════════════════════════════════════════════════════════
// معلومات الخريطة — القراءة فقط
// ═══════════════════════════════════════════════════════════════════

// عرض الخريطة بالمربعات
int getMapWidth(int mapId) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0;
    return map->tilesWide;
}

// ارتفاع الخريطة بالمربعات
int getMapHeight(int mapId) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0;
    return map->tilesHigh;
}

// حجم المربع الواحد بالبكسل
int getTileSize(int mapId) {
    Tilemap* map = findMap(mapId);
    if (!map) return 0;
    return map->tileSize;
}

}} // namespace sad::tilemap
