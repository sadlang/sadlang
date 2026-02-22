// ==============================================================================
// tilemap.h - نظام خريطة البلاط / Tilemap System
// ==============================================================================
// الوصف: نظام متكامل لإنشاء وإدارة خرائط البلاط للألعاب ثنائية الأبعاد
// Description: Complete tilemap system for 2D game development
// ==============================================================================
// المميزات / Features:
//   - طبقات متعددة / Multiple layers
//   - تحريك البلاط / Animated tiles
//   - طبقات التصادم / Collision layers
//   - خصائص البلاط / Tile properties
//   - تحميل من Tiled / Load from Tiled
// ==============================================================================

#ifndef SAD_GRAPHICS_TILEMAP_H
#define SAD_GRAPHICS_TILEMAP_H

#include "../core/types.h"
#include "../texture/texture.h"
#include "../texture/texture_atlas.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace sad {
namespace graphics {

// ==============================================================================
// المعرّفات والثوابت / Identifiers and Constants
// ==============================================================================

/// معرّف البلاط / Tile identifier
using TileId = UInt32;

/// معرّف الطبقة / Layer identifier  
using TilemapLayerId = UInt32;

/// بلاط فارغ / Empty tile
constexpr TileId EMPTY_TILE = 0;

// ==============================================================================
// أعلام البلاط / Tile Flags
// ==============================================================================

namespace TileFlags {
    constexpr UInt32 None           = 0;
    constexpr UInt32 FlipHorizontal = 1 << 0;   // قلب أفقي / Horizontal flip
    constexpr UInt32 FlipVertical   = 1 << 1;   // قلب عمودي / Vertical flip
    constexpr UInt32 FlipDiagonal   = 1 << 2;   // قلب قطري / Diagonal flip
    constexpr UInt32 Rotate90       = 1 << 3;   // دوران 90 درجة / Rotate 90 degrees
    constexpr UInt32 Rotate180      = 1 << 4;   // دوران 180 درجة / Rotate 180 degrees
    constexpr UInt32 Rotate270      = 1 << 5;   // دوران 270 درجة / Rotate 270 degrees
    constexpr UInt32 Collision      = 1 << 6;   // صلب / Solid collision
    constexpr UInt32 Trigger        = 1 << 7;   // مُشغّل / Trigger
    constexpr UInt32 OneWay         = 1 << 8;   // اتجاه واحد / One-way platform
    constexpr UInt32 Animated       = 1 << 9;   // متحرك / Animated
}

// ==============================================================================
// تعريف البلاط / Tile Definition
// ==============================================================================

/// خصائص البلاط / Tile properties
struct TileProperty {
    std::string name;
    std::string value;
};

/// تعريف البلاط في مجموعة البلاط / Tile definition in tileset
struct TileDefinition {
    TileId id = EMPTY_TILE;
    std::string name;                       // اسم البلاط / Tile name
    UInt32 flags = TileFlags::None;         // الأعلام / Flags
    Float32 u0, v0, u1, v1;                 // إحداثيات UV / UV coordinates
    std::vector<TileProperty> properties;   // خصائص مخصصة / Custom properties
    
    // للبلاط المتحرك / For animated tiles
    std::vector<TileId> animationFrames;    // إطارات الرسوم المتحركة / Animation frames
    Float32 animationSpeed = 0.1f;          // سرعة الحركة / Animation speed
    
    /// الحصول على خاصية / Get property
    std::string GetProperty(const std::string& name, const std::string& defaultValue = "") const;
    
    /// هل لديه علم / Has flag
    bool HasFlag(UInt32 flag) const { return (flags & flag) != 0; }
};

// ==============================================================================
// مجموعة البلاط / Tileset
// ==============================================================================

/// مجموعة البلاط / Tileset
class Tileset {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    Tileset() = default;
    ~Tileset() = default;
    
    // ============================================================================
    // التحميل / Loading
    // ============================================================================
    
    /// تحميل من صورة / Load from image
    /// @param imagePath مسار الصورة / Image path
    /// @param tileWidth عرض البلاط / Tile width
    /// @param tileHeight ارتفاع البلاط / Tile height
    /// @param margin الهامش / Margin
    /// @param spacing التباعد / Spacing
    bool LoadFromImage(const std::string& imagePath, Int32 tileWidth, Int32 tileHeight,
                      Int32 margin = 0, Int32 spacing = 0);
    
    /// تحميل من أطلس / Load from atlas
    bool LoadFromAtlas(TextureAtlas* atlas);
    
    /// إضافة تعريف بلاط / Add tile definition
    void AddTile(TileId id, const TileDefinition& def);
    
    // ============================================================================
    // الخصائص / Properties
    // ============================================================================
    
    /// الحصول على النسيج / Get texture
    TextureRef GetTexture() const { return m_texture; }
    void SetTexture(TextureRef texture) { m_texture = texture; }
    
    /// أبعاد البلاط / Tile dimensions
    Int32 GetTileWidth() const { return m_tileWidth; }
    Int32 GetTileHeight() const { return m_tileHeight; }
    
    /// عدد البلاطات في الصف / Tiles per row
    Int32 GetColumns() const { return m_columns; }
    
    /// عدد الصفوف / Row count
    Int32 GetRows() const { return m_rows; }
    
    /// أول معرّف / First GID
    TileId GetFirstGid() const { return m_firstGid; }
    void SetFirstGid(TileId gid) { m_firstGid = gid; }
    
    /// الحصول على تعريف البلاط / Get tile definition
    const TileDefinition* GetTile(TileId id) const;
    
    /// الحصول على إحداثيات UV للبلاط / Get tile UV coordinates
    void GetTileUV(TileId id, Float32& u0, Float32& v0, Float32& u1, Float32& v1) const;
    
    /// عدد البلاطات / Tile count
    Int32 GetTileCount() const { return m_columns * m_rows; }

private:
    TextureRef m_texture;
    Int32 m_tileWidth = 0;
    Int32 m_tileHeight = 0;
    Int32 m_columns = 0;
    Int32 m_rows = 0;
    Int32 m_margin = 0;
    Int32 m_spacing = 0;
    TileId m_firstGid = 1;
    
    std::unordered_map<TileId, TileDefinition> m_tiles;
};

// ==============================================================================
// خلية البلاط / Tile Cell
// ==============================================================================

/// خلية البلاط في الخريطة / Tile cell in map
struct TileCell {
    TileId tileId = EMPTY_TILE;     // معرّف البلاط / Tile ID
    UInt32 flags = TileFlags::None; // أعلام الخلية / Cell flags
    
    /// هل الخلية فارغة / Is cell empty
    bool IsEmpty() const { return tileId == EMPTY_TILE; }
    
    /// هل الخلية صلبة / Is cell solid
    bool IsSolid() const { return (flags & TileFlags::Collision) != 0; }
};

// ==============================================================================
// طبقة الخريطة / Tilemap Layer
// ==============================================================================

/// نوع الطبقة / Layer type
enum class TilemapLayerType {
    Tiles,          // طبقة بلاط / Tile layer
    Objects,        // طبقة كائنات / Object layer
    Image           // طبقة صورة / Image layer
};

/// طبقة الخريطة / Map layer
class TilemapLayer {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    TilemapLayer(Int32 width, Int32 height);
    ~TilemapLayer() = default;
    
    // ============================================================================
    // الخصائص / Properties
    // ============================================================================
    
    /// الاسم / Name
    const std::string& GetName() const { return m_name; }
    void SetName(const std::string& name) { m_name = name; }
    
    /// الأبعاد / Dimensions
    Int32 GetWidth() const { return m_width; }
    Int32 GetHeight() const { return m_height; }
    
    /// الرؤية / Visibility
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool visible) { m_visible = visible; }
    
    /// الشفافية / Opacity
    Float32 GetOpacity() const { return m_opacity; }
    void SetOpacity(Float32 opacity) { m_opacity = std::max(0.0f, std::min(1.0f, opacity)); }
    
    /// الإزاحة / Offset
    const Vec2& GetOffset() const { return m_offset; }
    void SetOffset(const Vec2& offset) { m_offset = offset; }
    void SetOffset(Float32 x, Float32 y) { m_offset = Vec2(x, y); }
    
    /// المنظر المتوازي / Parallax
    const Vec2& GetParallax() const { return m_parallax; }
    void SetParallax(const Vec2& parallax) { m_parallax = parallax; }
    void SetParallax(Float32 x, Float32 y) { m_parallax = Vec2(x, y); }
    
    /// نوع الطبقة / Layer type
    TilemapLayerType GetType() const { return m_type; }
    void SetType(TilemapLayerType type) { m_type = type; }
    
    // ============================================================================
    // إدارة البلاط / Tile Management
    // ============================================================================
    
    /// تعيين بلاط / Set tile
    void SetTile(Int32 x, Int32 y, TileId tileId, UInt32 flags = TileFlags::None);
    
    /// الحصول على بلاط / Get tile
    TileCell GetTile(Int32 x, Int32 y) const;
    
    /// مسح بلاط / Clear tile
    void ClearTile(Int32 x, Int32 y);
    
    /// ملء الطبقة / Fill layer
    void Fill(TileId tileId, UInt32 flags = TileFlags::None);
    
    /// مسح الطبقة / Clear layer
    void Clear();
    
    /// هل الإحداثيات صالحة / Are coordinates valid
    bool IsValidCoord(Int32 x, Int32 y) const {
        return x >= 0 && x < m_width && y >= 0 && y < m_height;
    }
    
    /// الحصول على البيانات الخام / Get raw data
    const std::vector<TileCell>& GetData() const { return m_data; }
    std::vector<TileCell>& GetData() { return m_data; }

private:
    std::string m_name;
    TilemapLayerType m_type = TilemapLayerType::Tiles;
    Int32 m_width;
    Int32 m_height;
    bool m_visible = true;
    Float32 m_opacity = 1.0f;
    Vec2 m_offset;
    Vec2 m_parallax = Vec2(1.0f, 1.0f);
    
    std::vector<TileCell> m_data;
};

// ==============================================================================
// كائن في الخريطة / Map Object
// ==============================================================================

/// نوع الكائن / Object shape
enum class MapObjectShape {
    Rectangle,      // مستطيل / Rectangle
    Ellipse,        // بيضوي / Ellipse
    Point,          // نقطة / Point
    Polygon,        // مضلع / Polygon
    Polyline        // خط متعدد / Polyline
};

/// كائن في الخريطة / Map object
struct MapObject {
    UInt32 id = 0;
    std::string name;
    std::string type;
    MapObjectShape shape = MapObjectShape::Rectangle;
    Float32 x = 0.0f;
    Float32 y = 0.0f;
    Float32 width = 0.0f;
    Float32 height = 0.0f;
    Float32 rotation = 0.0f;
    bool visible = true;
    std::vector<Vec2> points;               // للمضلع / For polygon/polyline
    std::vector<TileProperty> properties;   // خصائص مخصصة / Custom properties
    
    /// الحصول على خاصية / Get property
    std::string GetProperty(const std::string& name, const std::string& defaultValue = "") const;
};

// ==============================================================================
// خريطة البلاط / Tilemap
// ==============================================================================

/// خريطة البلاط / Tilemap
class Tilemap {
public:
    // ============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ============================================================================
    
    Tilemap();
    Tilemap(Int32 width, Int32 height, Int32 tileWidth, Int32 tileHeight);
    ~Tilemap() = default;
    
    // ============================================================================
    // التحميل والحفظ / Loading and Saving
    // ============================================================================
    
    /// تحميل من ملف TMX / Load from TMX file
    bool LoadFromTMX(const std::string& filePath);
    
    /// حفظ إلى ملف TMX / Save to TMX file
    bool SaveToTMX(const std::string& filePath) const;
    
    /// تحميل من JSON / Load from JSON (Tiled JSON format)
    bool LoadFromJSON(const std::string& filePath);
    
    // ============================================================================
    // الخصائص / Properties
    // ============================================================================
    
    /// أبعاد الخريطة بالبلاطات / Map dimensions in tiles
    Int32 GetWidth() const { return m_width; }
    Int32 GetHeight() const { return m_height; }
    
    /// أبعاد البلاط / Tile dimensions
    Int32 GetTileWidth() const { return m_tileWidth; }
    Int32 GetTileHeight() const { return m_tileHeight; }
    
    /// أبعاد الخريطة بالبكسل / Map dimensions in pixels
    Float32 GetPixelWidth() const { return static_cast<Float32>(m_width * m_tileWidth); }
    Float32 GetPixelHeight() const { return static_cast<Float32>(m_height * m_tileHeight); }
    
    // ============================================================================
    // مجموعات البلاط / Tilesets
    // ============================================================================
    
    /// إضافة مجموعة بلاط / Add tileset
    void AddTileset(std::shared_ptr<Tileset> tileset);
    
    /// الحصول على مجموعة البلاط للبلاط / Get tileset for tile
    Tileset* GetTilesetForTile(TileId tileId) const;
    
    /// عدد مجموعات البلاط / Tileset count
    Int32 GetTilesetCount() const { return static_cast<Int32>(m_tilesets.size()); }
    
    // ============================================================================
    // الطبقات / Layers
    // ============================================================================
    
    /// إنشاء طبقة / Create layer
    TilemapLayer* CreateLayer(const std::string& name);
    
    /// الحصول على طبقة / Get layer
    TilemapLayer* GetLayer(const std::string& name);
    TilemapLayer* GetLayer(Int32 index);
    const TilemapLayer* GetLayer(const std::string& name) const;
    const TilemapLayer* GetLayer(Int32 index) const;
    
    /// إزالة طبقة / Remove layer
    void RemoveLayer(const std::string& name);
    
    /// عدد الطبقات / Layer count
    Int32 GetLayerCount() const { return static_cast<Int32>(m_layers.size()); }
    
    // ============================================================================
    // الكائنات / Objects
    // ============================================================================
    
    /// إضافة كائن / Add object
    void AddObject(const MapObject& object);
    
    /// الحصول على كائنات بالنوع / Get objects by type
    std::vector<const MapObject*> GetObjectsByType(const std::string& type) const;
    
    /// الحصول على كائن بالاسم / Get object by name
    const MapObject* GetObjectByName(const std::string& name) const;
    
    /// جميع الكائنات / All objects
    const std::vector<MapObject>& GetObjects() const { return m_objects; }
    
    // ============================================================================
    // التحويلات / Conversions
    // ============================================================================
    
    /// من موقع عالمي إلى إحداثيات البلاط / World position to tile coordinates
    Vec2 WorldToTile(Float32 worldX, Float32 worldY) const;
    
    /// من إحداثيات البلاط إلى موقع عالمي / Tile coordinates to world position
    Vec2 TileToWorld(Int32 tileX, Int32 tileY) const;
    
    /// هل النقطة داخل الخريطة / Is point inside map
    bool ContainsPoint(Float32 worldX, Float32 worldY) const;
    
    // ============================================================================
    // التصادم / Collision
    // ============================================================================
    
    /// هل البلاط صلب / Is tile solid
    bool IsSolid(Int32 x, Int32 y, const std::string& layerName = "") const;
    
    /// الحصول على بلاطات التصادم في منطقة / Get collision tiles in area
    std::vector<Rect> GetCollisionRects(const Rect& area, const std::string& layerName = "") const;
    
    // ============================================================================
    // الرسم / Rendering
    // ============================================================================
    
    /// تحديث (للرسوم المتحركة) / Update (for animations)
    void Update(Float32 deltaTime);
    
    // ============================================================================
    // الخصائص المخصصة / Custom Properties
    // ============================================================================
    
    /// تعيين خاصية / Set property
    void SetProperty(const std::string& name, const std::string& value);
    
    /// الحصول على خاصية / Get property
    std::string GetProperty(const std::string& name, const std::string& defaultValue = "") const;
    
    /// جميع الخصائص / All properties
    const std::unordered_map<std::string, std::string>& GetProperties() const { return m_properties; }

private:
    Int32 m_width = 0;
    Int32 m_height = 0;
    Int32 m_tileWidth = 0;
    Int32 m_tileHeight = 0;
    
    std::vector<std::shared_ptr<Tileset>> m_tilesets;
    std::vector<std::unique_ptr<TilemapLayer>> m_layers;
    std::vector<MapObject> m_objects;
    std::unordered_map<std::string, std::string> m_properties;
    
    // للرسوم المتحركة / For animations
    Float32 m_animationTime = 0.0f;
};

// ==============================================================================
// مُنشئ الخريطة / Tilemap Builder (Fluent API)
// ==============================================================================

/// منشئ الخريطة بواجهة مبسطة / Tilemap builder with fluent API
class TilemapBuilder {
public:
    TilemapBuilder(Int32 width, Int32 height, Int32 tileWidth, Int32 tileHeight);
    
    /// إضافة مجموعة بلاط / Add tileset
    TilemapBuilder& WithTileset(const std::string& imagePath, Int32 margin = 0, Int32 spacing = 0);
    
    /// إضافة طبقة / Add layer
    TilemapBuilder& AddLayer(const std::string& name);
    
    /// تعيين بلاط / Set tile
    TilemapBuilder& SetTile(Int32 x, Int32 y, TileId tileId, const std::string& layerName = "");
    
    /// ملء منطقة / Fill area
    TilemapBuilder& FillArea(Int32 x, Int32 y, Int32 w, Int32 h, TileId tileId, const std::string& layerName = "");
    
    /// إضافة كائن / Add object
    TilemapBuilder& AddObject(const std::string& name, const std::string& type, Float32 x, Float32 y, Float32 w = 0, Float32 h = 0);
    
    /// بناء الخريطة / Build the tilemap
    std::unique_ptr<Tilemap> Build();

private:
    std::unique_ptr<Tilemap> m_tilemap;
    std::string m_currentLayer;
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_TILEMAP_H
