// ==============================================================================
// tilemap.cpp - تنفيذ نظام خريطة البلاط / Tilemap System Implementation
// ==============================================================================

#include "../../include/world/tilemap.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace sad {
namespace graphics {

// ==============================================================================
// TileDefinition - تعريف البلاط
// ==============================================================================

std::string TileDefinition::GetProperty(const std::string& name, const std::string& defaultValue) const {
    for (const auto& prop : properties) {
        if (prop.name == name) return prop.value;
    }
    return defaultValue;
}

// ==============================================================================
// Tileset - مجموعة البلاط
// ==============================================================================

bool Tileset::LoadFromImage(const std::string& imagePath, Int32 tileWidth, Int32 tileHeight,
                           Int32 margin, Int32 spacing) {
    // TODO: تحميل النسيج من الملف / Load texture from file
    // m_texture = TextureManager::Load(imagePath);
    
    m_tileWidth = tileWidth;
    m_tileHeight = tileHeight;
    m_margin = margin;
    m_spacing = spacing;
    
    // حساب عدد الأعمدة والصفوف / Calculate columns and rows
    // افتراض أبعاد النسيج / Assume texture dimensions
    Int32 textureWidth = 256;  // TODO: الحصول من النسيج / Get from texture
    Int32 textureHeight = 256;
    
    m_columns = (textureWidth - margin * 2 + spacing) / (tileWidth + spacing);
    m_rows = (textureHeight - margin * 2 + spacing) / (tileHeight + spacing);
    
    // إنشاء تعريفات البلاط / Create tile definitions
    for (Int32 y = 0; y < m_rows; ++y) {
        for (Int32 x = 0; x < m_columns; ++x) {
            TileId id = m_firstGid + y * m_columns + x;
            TileDefinition def;
            def.id = id;
            
            // حساب إحداثيات UV / Calculate UV coordinates
            Float32 u0 = static_cast<Float32>(margin + x * (tileWidth + spacing)) / textureWidth;
            Float32 v0 = static_cast<Float32>(margin + y * (tileHeight + spacing)) / textureHeight;
            Float32 u1 = u0 + static_cast<Float32>(tileWidth) / textureWidth;
            Float32 v1 = v0 + static_cast<Float32>(tileHeight) / textureHeight;
            
            def.u0 = u0;
            def.v0 = v0;
            def.u1 = u1;
            def.v1 = v1;
            
            m_tiles[id] = def;
        }
    }
    
    return true;
}

bool Tileset::LoadFromAtlas(TextureAtlas* atlas) {
    if (!atlas) return false;
    
    auto names = atlas->GetRegionNames();
    for (const auto& name : names) {
        const AtlasRegion* region = atlas->GetRegion(name);
        if (!region) continue;
        
        TileDefinition def;
        def.id = m_firstGid + static_cast<TileId>(m_tiles.size());
        def.name = name;
        def.u0 = region->u0;
        def.v0 = region->v0;
        def.u1 = region->u1;
        def.v1 = region->v1;
        
        m_tiles[def.id] = def;
    }
    
    return true;
}

void Tileset::AddTile(TileId id, const TileDefinition& def) {
    m_tiles[id] = def;
}

const TileDefinition* Tileset::GetTile(TileId id) const {
    auto it = m_tiles.find(id);
    return (it != m_tiles.end()) ? &it->second : nullptr;
}

void Tileset::GetTileUV(TileId id, Float32& u0, Float32& v0, Float32& u1, Float32& v1) const {
    const TileDefinition* def = GetTile(id);
    if (def) {
        u0 = def->u0;
        v0 = def->v0;
        u1 = def->u1;
        v1 = def->v1;
    } else {
        // حساب UV من المعرّف / Calculate UV from ID
        TileId localId = id - m_firstGid;
        Int32 x = localId % m_columns;
        Int32 y = localId / m_columns;
        
        // افتراض أبعاد النسيج / Assume texture dimensions
        Float32 textureWidth = 256.0f;
        Float32 textureHeight = 256.0f;
        
        u0 = static_cast<Float32>(m_margin + x * (m_tileWidth + m_spacing)) / textureWidth;
        v0 = static_cast<Float32>(m_margin + y * (m_tileHeight + m_spacing)) / textureHeight;
        u1 = u0 + static_cast<Float32>(m_tileWidth) / textureWidth;
        v1 = v0 + static_cast<Float32>(m_tileHeight) / textureHeight;
    }
}

// ==============================================================================
// TilemapLayer - طبقة الخريطة
// ==============================================================================

TilemapLayer::TilemapLayer(Int32 width, Int32 height)
    : m_width(width)
    , m_height(height)
{
    m_data.resize(width * height);
}

void TilemapLayer::SetTile(Int32 x, Int32 y, TileId tileId, UInt32 flags) {
    if (!IsValidCoord(x, y)) return;
    
    TileCell& cell = m_data[y * m_width + x];
    cell.tileId = tileId;
    cell.flags = flags;
}

TileCell TilemapLayer::GetTile(Int32 x, Int32 y) const {
    if (!IsValidCoord(x, y)) return TileCell();
    return m_data[y * m_width + x];
}

void TilemapLayer::ClearTile(Int32 x, Int32 y) {
    if (!IsValidCoord(x, y)) return;
    m_data[y * m_width + x] = TileCell();
}

void TilemapLayer::Fill(TileId tileId, UInt32 flags) {
    for (auto& cell : m_data) {
        cell.tileId = tileId;
        cell.flags = flags;
    }
}

void TilemapLayer::Clear() {
    for (auto& cell : m_data) {
        cell = TileCell();
    }
}

// ==============================================================================
// MapObject - كائن الخريطة
// ==============================================================================

std::string MapObject::GetProperty(const std::string& name, const std::string& defaultValue) const {
    for (const auto& prop : properties) {
        if (prop.name == name) return prop.value;
    }
    return defaultValue;
}

// ==============================================================================
// Tilemap - خريطة البلاط
// ==============================================================================

Tilemap::Tilemap() = default;

Tilemap::Tilemap(Int32 width, Int32 height, Int32 tileWidth, Int32 tileHeight)
    : m_width(width)
    , m_height(height)
    , m_tileWidth(tileWidth)
    , m_tileHeight(tileHeight)
{
}

// ============================================================================
// محلل TMX بسيط / Simple TMX Parser Helpers
// ============================================================================

static std::string GetXmlAttribute(const std::string& line, const std::string& attr) {
    std::string search = attr + "=\"";
    size_t pos = line.find(search);
    if (pos == std::string::npos) return "";
    pos += search.length();
    size_t end = line.find('"', pos);
    if (end == std::string::npos) return "";
    return line.substr(pos, end - pos);
}

static std::vector<Int32> ParseCSV(const std::string& csv) {
    std::vector<Int32> result;
    std::istringstream stream(csv);
    std::string token;
    while (std::getline(stream, token, ',')) {
        // إزالة المسافات / Trim whitespace
        size_t start = token.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) continue;
        size_t end = token.find_last_not_of(" \t\n\r");
        token = token.substr(start, end - start + 1);
        if (!token.empty()) {
            result.push_back(std::stoi(token));
        }
    }
    return result;
}

bool Tilemap::LoadFromTMX(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) return false;
    
    std::string line;
    std::string currentLayerName;
    bool inData = false;
    std::string dataContent;
    
    while (std::getline(file, line)) {
        // تحليل عنصر الخريطة / Parse map element
        if (line.find("<map") != std::string::npos) {
            std::string w = GetXmlAttribute(line, "width");
            std::string h = GetXmlAttribute(line, "height");
            std::string tw = GetXmlAttribute(line, "tilewidth");
            std::string th = GetXmlAttribute(line, "tileheight");
            if (!w.empty()) m_width = std::stoi(w);
            if (!h.empty()) m_height = std::stoi(h);
            if (!tw.empty()) m_tileWidth = std::stoi(tw);
            if (!th.empty()) m_tileHeight = std::stoi(th);
        }
        // تحليل مجموعة البلاط / Parse tileset
        else if (line.find("<tileset") != std::string::npos) {
            std::string gid = GetXmlAttribute(line, "firstgid");
            std::string src = GetXmlAttribute(line, "source");
            std::string tw = GetXmlAttribute(line, "tilewidth");
            std::string th = GetXmlAttribute(line, "tileheight");
            
            auto tileset = std::make_shared<Tileset>();
            if (!gid.empty()) tileset->SetFirstGid(std::stoi(gid));
            if (!tw.empty() && !th.empty()) {
                tileset->LoadFromImage(src.empty() ? "tileset.png" : src,
                                       std::stoi(tw), std::stoi(th));
            }
            m_tilesets.push_back(tileset);
        }
        // تحليل طبقة / Parse layer
        else if (line.find("<layer") != std::string::npos) {
            currentLayerName = GetXmlAttribute(line, "name");
            if (currentLayerName.empty()) currentLayerName = "layer_" + std::to_string(m_layers.size());
        }
        // بداية البيانات / Data start
        else if (line.find("<data") != std::string::npos) {
            inData = true;
            dataContent.clear();
            // البيانات قد تكون في نفس السطر / Data may be on the same line
            size_t close = line.find('>');
            if (close != std::string::npos && close + 1 < line.size()) {
                size_t endTag = line.find("</data>");
                if (endTag != std::string::npos) {
                    dataContent = line.substr(close + 1, endTag - close - 1);
                    inData = false;
                } else {
                    dataContent = line.substr(close + 1);
                }
            }
        }
        else if (inData) {
            size_t endTag = line.find("</data>");
            if (endTag != std::string::npos) {
                dataContent += line.substr(0, endTag);
                inData = false;
            } else {
                dataContent += line;
            }
        }
        
        // معالجة البيانات المقروءة / Process collected data
        if (!inData && !dataContent.empty() && !currentLayerName.empty()) {
            auto tiles = ParseCSV(dataContent);
            auto* layer = CreateLayer(currentLayerName);
            if (layer) {
                Int32 idx = 0;
                for (Int32 y = 0; y < m_height && idx < static_cast<Int32>(tiles.size()); ++y) {
                    for (Int32 x = 0; x < m_width && idx < static_cast<Int32>(tiles.size()); ++x) {
                        if (tiles[idx] != 0) {
                            layer->SetTile(x, y, static_cast<TileId>(tiles[idx]));
                        }
                        idx++;
                    }
                }
            }
            dataContent.clear();
            currentLayerName.clear();
        }
        
        // تحليل كائنات / Parse objects
        if (line.find("<object") != std::string::npos) {
            MapObject obj;
            obj.name = GetXmlAttribute(line, "name");
            obj.type = GetXmlAttribute(line, "type");
            std::string ox = GetXmlAttribute(line, "x");
            std::string oy = GetXmlAttribute(line, "y");
            std::string ow = GetXmlAttribute(line, "width");
            std::string oh = GetXmlAttribute(line, "height");
            if (!ox.empty()) obj.x = std::stof(ox);
            if (!oy.empty()) obj.y = std::stof(oy);
            if (!ow.empty()) obj.width = std::stof(ow);
            if (!oh.empty()) obj.height = std::stof(oh);
            AddObject(obj);
        }
    }
    
    return true;
}

bool Tilemap::SaveToTMX(const std::string& filePath) const {
    std::ofstream file(filePath);
    if (!file.is_open()) return false;
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<map version=\"1.10\" orientation=\"orthogonal\" renderorder=\"right-down\"";
    file << " width=\"" << m_width << "\" height=\"" << m_height << "\"";
    file << " tilewidth=\"" << m_tileWidth << "\" tileheight=\"" << m_tileHeight << "\">\n";
    
    // كتابة مجموعات البلاط / Write tilesets
    for (const auto& ts : m_tilesets) {
        file << " <tileset firstgid=\"" << ts->GetFirstGid() << "\"";
        file << " tilewidth=\"" << ts->GetTileWidth() << "\"";
        file << " tileheight=\"" << ts->GetTileHeight() << "\"/>\n";
    }
    
    // كتابة الطبقات / Write layers
    for (const auto& layer : m_layers) {
        file << " <layer name=\"" << layer->GetName() << "\"";
        file << " width=\"" << layer->GetWidth() << "\"";
        file << " height=\"" << layer->GetHeight() << "\">\n";
        file << "  <data encoding=\"csv\">\n";
        
        for (Int32 y = 0; y < layer->GetHeight(); ++y) {
            file << "   ";
            for (Int32 x = 0; x < layer->GetWidth(); ++x) {
                TileCell cell = layer->GetTile(x, y);
                file << cell.tileId;
                if (x < layer->GetWidth() - 1 || y < layer->GetHeight() - 1)
                    file << ",";
            }
            file << "\n";
        }
        
        file << "  </data>\n";
        file << " </layer>\n";
    }
    
    // كتابة الكائنات / Write objects
    if (!m_objects.empty()) {
        file << " <objectgroup name=\"objects\">\n";
        for (const auto& obj : m_objects) {
            file << "  <object";
            if (!obj.name.empty()) file << " name=\"" << obj.name << "\"";
            if (!obj.type.empty()) file << " type=\"" << obj.type << "\"";
            file << " x=\"" << obj.x << "\" y=\"" << obj.y << "\"";
            file << " width=\"" << obj.width << "\" height=\"" << obj.height << "\"/>\n";
        }
        file << " </objectgroup>\n";
    }
    
    file << "</map>\n";
    return true;
}

bool Tilemap::LoadFromJSON(const std::string& filePath) {
    // محلل JSON بسيط لصيغة Tiled JSON / Simple parser for Tiled JSON format
    std::ifstream file(filePath);
    if (!file.is_open()) return false;
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    
    // قراءة الأبعاد / Read dimensions
    auto readInt = [&content](const std::string& key) -> Int32 {
        std::string search = "\"" + key + "\":";
        size_t pos = content.find(search);
        if (pos == std::string::npos) return 0;
        pos += search.length();
        while (pos < content.size() && content[pos] == ' ') pos++;
        return std::stoi(content.substr(pos));
    };
    
    m_width = readInt("width");
    m_height = readInt("height");
    m_tileWidth = readInt("tilewidth");
    m_tileHeight = readInt("tileheight");
    
    // قراءة الطبقات / Read layers - find "data":[ arrays
    size_t searchPos = 0;
    Int32 layerIdx = 0;
    while (true) {
        size_t dataPos = content.find("\"data\":", searchPos);
        if (dataPos == std::string::npos) break;
        
        size_t arrStart = content.find('[', dataPos);
        size_t arrEnd = content.find(']', arrStart);
        if (arrStart == std::string::npos || arrEnd == std::string::npos) break;
        
        std::string csvData = content.substr(arrStart + 1, arrEnd - arrStart - 1);
        auto tiles = ParseCSV(csvData);
        
        std::string layerName = "layer_" + std::to_string(layerIdx++);
        auto* layer = CreateLayer(layerName);
        if (layer) {
            Int32 idx = 0;
            for (Int32 y = 0; y < m_height && idx < static_cast<Int32>(tiles.size()); ++y) {
                for (Int32 x = 0; x < m_width && idx < static_cast<Int32>(tiles.size()); ++x) {
                    if (tiles[idx] != 0) {
                        layer->SetTile(x, y, static_cast<TileId>(tiles[idx]));
                    }
                    idx++;
                }
            }
        }
        
        searchPos = arrEnd + 1;
    }
    
    return m_width > 0 && m_height > 0;
}

void Tilemap::AddTileset(std::shared_ptr<Tileset> tileset) {
    m_tilesets.push_back(tileset);
}

Tileset* Tilemap::GetTilesetForTile(TileId tileId) const {
    // البحث عن مجموعة البلاط المناسبة
    // Find the appropriate tileset
    Tileset* best = nullptr;
    TileId bestGid = 0;
    
    for (const auto& ts : m_tilesets) {
        TileId gid = ts->GetFirstGid();
        if (tileId >= gid && gid > bestGid) {
            best = ts.get();
            bestGid = gid;
        }
    }
    
    return best;
}

TilemapLayer* Tilemap::CreateLayer(const std::string& name) {
    auto layer = std::make_unique<TilemapLayer>(m_width, m_height);
    layer->SetName(name);
    m_layers.push_back(std::move(layer));
    return m_layers.back().get();
}

TilemapLayer* Tilemap::GetLayer(const std::string& name) {
    for (auto& layer : m_layers) {
        if (layer->GetName() == name) return layer.get();
    }
    return nullptr;
}

TilemapLayer* Tilemap::GetLayer(Int32 index) {
    if (index < 0 || index >= static_cast<Int32>(m_layers.size())) return nullptr;
    return m_layers[index].get();
}

const TilemapLayer* Tilemap::GetLayer(const std::string& name) const {
    for (const auto& layer : m_layers) {
        if (layer->GetName() == name) return layer.get();
    }
    return nullptr;
}

const TilemapLayer* Tilemap::GetLayer(Int32 index) const {
    if (index < 0 || index >= static_cast<Int32>(m_layers.size())) return nullptr;
    return m_layers[index].get();
}

void Tilemap::RemoveLayer(const std::string& name) {
    auto it = std::remove_if(m_layers.begin(), m_layers.end(),
        [&name](const std::unique_ptr<TilemapLayer>& layer) { return layer->GetName() == name; });
    m_layers.erase(it, m_layers.end());
}

void Tilemap::AddObject(const MapObject& object) {
    m_objects.push_back(object);
    m_objects.back().id = static_cast<UInt32>(m_objects.size());
}

std::vector<const MapObject*> Tilemap::GetObjectsByType(const std::string& type) const {
    std::vector<const MapObject*> result;
    for (const auto& obj : m_objects) {
        if (obj.type == type) result.push_back(&obj);
    }
    return result;
}

const MapObject* Tilemap::GetObjectByName(const std::string& name) const {
    for (const auto& obj : m_objects) {
        if (obj.name == name) return &obj;
    }
    return nullptr;
}

Vec2 Tilemap::WorldToTile(Float32 worldX, Float32 worldY) const {
    return Vec2(
        std::floor(worldX / m_tileWidth),
        std::floor(worldY / m_tileHeight)
    );
}

Vec2 Tilemap::TileToWorld(Int32 tileX, Int32 tileY) const {
    return Vec2(
        static_cast<Float32>(tileX * m_tileWidth),
        static_cast<Float32>(tileY * m_tileHeight)
    );
}

bool Tilemap::ContainsPoint(Float32 worldX, Float32 worldY) const {
    return worldX >= 0 && worldX < GetPixelWidth() &&
           worldY >= 0 && worldY < GetPixelHeight();
}

bool Tilemap::IsSolid(Int32 x, Int32 y, const std::string& layerName) const {
    if (x < 0 || x >= m_width || y < 0 || y >= m_height) return true; // خارج الحدود = صلب / Out of bounds = solid
    
    if (!layerName.empty()) {
        const TilemapLayer* layer = GetLayer(layerName);
        if (layer) {
            return layer->GetTile(x, y).IsSolid();
        }
        return false;
    }
    
    // التحقق من جميع الطبقات / Check all layers
    for (const auto& layer : m_layers) {
        if (layer->GetTile(x, y).IsSolid()) return true;
    }
    return false;
}

std::vector<Rect> Tilemap::GetCollisionRects(const Rect& area, const std::string& layerName) const {
    std::vector<Rect> result;
    
    // حساب نطاق البلاطات / Calculate tile range
    Int32 startX = std::max(0, static_cast<Int32>(std::floor(area.x / m_tileWidth)));
    Int32 startY = std::max(0, static_cast<Int32>(std::floor(area.y / m_tileHeight)));
    Int32 endX = std::min(m_width - 1, static_cast<Int32>(std::ceil((area.x + area.width) / m_tileWidth)));
    Int32 endY = std::min(m_height - 1, static_cast<Int32>(std::ceil((area.y + area.height) / m_tileHeight)));
    
    for (Int32 y = startY; y <= endY; ++y) {
        for (Int32 x = startX; x <= endX; ++x) {
            if (IsSolid(x, y, layerName)) {
                result.push_back(Rect(
                    static_cast<Float32>(x * m_tileWidth),
                    static_cast<Float32>(y * m_tileHeight),
                    static_cast<Float32>(m_tileWidth),
                    static_cast<Float32>(m_tileHeight)
                ));
            }
        }
    }
    
    return result;
}

void Tilemap::Update(Float32 deltaTime) {
    m_animationTime += deltaTime;
    
    // تحديث البلاطات المتحركة / Update animated tiles
    for (auto& layer : m_layers) {
        for (Int32 y = 0; y < layer->GetHeight(); ++y) {
            for (Int32 x = 0; x < layer->GetWidth(); ++x) {
                TileCell cell = layer->GetTile(x, y);
                if (!(cell.flags & TileFlags::Animated)) continue;
                if (cell.tileId == 0) continue;
                
                // البحث عن تعريف البلاط / Find tile definition
                Tileset* tileset = GetTilesetForTile(cell.tileId);
                if (!tileset) continue;
                
                const TileDefinition* def = tileset->GetTile(cell.tileId);
                if (!def || def->animationFrames.empty()) continue;
                
                // حساب الإطار الحالي / Calculate current frame
                Float32 totalDuration = def->animationSpeed * def->animationFrames.size();
                if (totalDuration <= 0.0f) continue;
                
                Float32 t = std::fmod(m_animationTime, totalDuration);
                Int32 frameIndex = static_cast<Int32>(t / def->animationSpeed);
                frameIndex = std::min(frameIndex, static_cast<Int32>(def->animationFrames.size()) - 1);
                
                // تعيين البلاطة الحالية / Set current tile
                TileId newId = def->animationFrames[frameIndex];
                if (newId != cell.tileId) {
                    layer->SetTile(x, y, newId, cell.flags);
                }
            }
        }
    }
}

void Tilemap::SetProperty(const std::string& name, const std::string& value) {
    m_properties[name] = value;
}

std::string Tilemap::GetProperty(const std::string& name, const std::string& defaultValue) const {
    auto it = m_properties.find(name);
    return (it != m_properties.end()) ? it->second : defaultValue;
}

// ==============================================================================
// TilemapBuilder - منشئ الخريطة
// ==============================================================================

TilemapBuilder::TilemapBuilder(Int32 width, Int32 height, Int32 tileWidth, Int32 tileHeight)
    : m_tilemap(std::make_unique<Tilemap>(width, height, tileWidth, tileHeight))
{
}

TilemapBuilder& TilemapBuilder::WithTileset(const std::string& imagePath, Int32 margin, Int32 spacing) {
    auto tileset = std::make_shared<Tileset>();
    // حساب GID الصحيح / Calculate correct GID based on tileset count
    TileId nextGid = 1 + static_cast<TileId>(m_tilemap->GetTilesetCount()) * 256;
    tileset->SetFirstGid(nextGid);
    tileset->LoadFromImage(imagePath, m_tilemap->GetTileWidth(), m_tilemap->GetTileHeight(), margin, spacing);
    m_tilemap->AddTileset(tileset);
    return *this;
}

TilemapBuilder& TilemapBuilder::AddLayer(const std::string& name) {
    m_tilemap->CreateLayer(name);
    m_currentLayer = name;
    return *this;
}

TilemapBuilder& TilemapBuilder::SetTile(Int32 x, Int32 y, TileId tileId, const std::string& layerName) {
    const std::string& layer = layerName.empty() ? m_currentLayer : layerName;
    TilemapLayer* l = m_tilemap->GetLayer(layer);
    if (l) l->SetTile(x, y, tileId);
    return *this;
}

TilemapBuilder& TilemapBuilder::FillArea(Int32 x, Int32 y, Int32 w, Int32 h, TileId tileId, const std::string& layerName) {
    const std::string& layer = layerName.empty() ? m_currentLayer : layerName;
    TilemapLayer* l = m_tilemap->GetLayer(layer);
    if (l) {
        for (Int32 ty = y; ty < y + h; ++ty) {
            for (Int32 tx = x; tx < x + w; ++tx) {
                l->SetTile(tx, ty, tileId);
            }
        }
    }
    return *this;
}

TilemapBuilder& TilemapBuilder::AddObject(const std::string& name, const std::string& type, Float32 x, Float32 y, Float32 w, Float32 h) {
    MapObject obj;
    obj.name = name;
    obj.type = type;
    obj.x = x;
    obj.y = y;
    obj.width = w;
    obj.height = h;
    m_tilemap->AddObject(obj);
    return *this;
}

std::unique_ptr<Tilemap> TilemapBuilder::Build() {
    return std::move(m_tilemap);
}

} // namespace graphics
} // namespace sad
