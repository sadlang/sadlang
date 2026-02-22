// ==============================================================================
// scene.h - نظام المشاهد والطبقات / Scene & Layer System
// ==============================================================================
// الوصف: إدارة مشاهد اللعبة مع طبقات مرتبة وكاميرا مستقلة لكل طبقة
// Description: Game scene management with ordered layers and per-layer camera
// ==============================================================================

#pragma once

#include "../core/types.h"
#include "../rendering/camera2d.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

namespace sad {
namespace graphics {

// ==============================================================================
// كائن المشهد / Scene Entity
// ==============================================================================
struct Entity {
    UInt32 id = 0;               // المعرّف الفريد / Unique ID
    std::string name;            // الاسم / Name
    std::string tag;             // الوسم / Tag
    Vec2 position;               // الموقع / Position
    Vec2 size;                   // الأبعاد / Size
    Float32 rotation = 0.0f;     // الدوران / Rotation
    Vec2 scale = Vec2(1.0f, 1.0f); // القياس / Scale
    bool visible = true;         // مرئي / Visible
    bool active = true;          // نشط / Active
    Int32 zOrder = 0;            // ترتيب العمق / Z-Order
    
    // بيانات مخصصة / Custom data
    void* userData = nullptr;
    
    // دوال التحديث والرسم / Update and draw callbacks
    std::function<void(Entity&, Float32)> onUpdate;
    std::function<void(const Entity&)> onDraw;
    
    bool operator<(const Entity& other) const { return zOrder < other.zOrder; }
};

// ==============================================================================
// طبقة المشهد / Scene Layer
// ==============================================================================
class Layer {
public:
    /// المُنشئ / Constructor
    /// name: اسم الطبقة / Layer name
    /// zOrder: ترتيب الطبقة / Layer order
    Layer(const std::string& name, Int32 zOrder = 0);
    ~Layer() = default;
    
    // ==============================================================================
    // إدارة الكيانات / Entity Management
    // ==============================================================================
    
    /// إضافة كيان / Add entity
    /// returns: معرّف الكيان / Entity ID
    UInt32 AddEntity(const Entity& entity);
    
    /// إضافة كيان بسيط / Add simple entity
    UInt32 AddEntity(const std::string& name, Float32 x, Float32 y,
                     Float32 w = 0.0f, Float32 h = 0.0f);
    
    /// إزالة كيان / Remove entity
    void RemoveEntity(UInt32 id);
    
    /// الحصول على كيان / Get entity
    Entity* GetEntity(UInt32 id);
    const Entity* GetEntity(UInt32 id) const;
    
    /// البحث عن كيان بالاسم / Find entity by name
    Entity* FindByName(const std::string& name);
    
    /// البحث عن كيانات بالوسم / Find entities by tag
    std::vector<Entity*> FindByTag(const std::string& tag);
    
    /// عدد الكيانات / Entity count
    size_t GetEntityCount() const { return m_entities.size(); }
    
    /// الحصول على جميع الكيانات / Get all entities
    const std::vector<Entity>& GetEntities() const { return m_entities; }
    std::vector<Entity>& GetEntities() { return m_entities; }
    
    // ==============================================================================
    // خصائص الطبقة / Layer Properties
    // ==============================================================================
    
    const std::string& GetName() const { return m_name; }
    Int32 GetZOrder() const { return m_zOrder; }
    void SetZOrder(Int32 z) { m_zOrder = z; }
    
    bool IsVisible() const { return m_visible; }
    void SetVisible(bool vis) { m_visible = vis; }
    
    Float32 GetOpacity() const { return m_opacity; }
    void SetOpacity(Float32 opacity) { m_opacity = opacity; }
    
    /// عامل parallax (للطبقات الخلفية) / Parallax factor (for background layers)
    void SetParallax(Float32 x, Float32 y);
    Vec2 GetParallax() const { return m_parallax; }
    
    /// مسح جميع الكيانات / Clear all entities
    void Clear();
    
    // ==============================================================================
    // التحديث والرسم / Update and Draw
    // ==============================================================================
    
    /// تحديث الطبقة / Update layer
    void Update(Float32 deltaTime);
    
    /// رسم الطبقة / Draw layer
    void Draw() const;
    
    /// ترتيب الكيانات / Sort entities by z-order
    void SortEntities();
    
    bool operator<(const Layer& other) const { return m_zOrder < other.GetZOrder(); }

private:
    std::string m_name;
    Int32 m_zOrder;
    bool m_visible;
    Float32 m_opacity;
    Vec2 m_parallax;
    std::vector<Entity> m_entities;
    UInt32 m_nextEntityId;
    bool m_needsSort;
};

// ==============================================================================
// فئة المشهد / Scene Class
// ==============================================================================
class Scene {
public:
    /// المُنشئ / Constructor
    /// name: اسم المشهد / Scene name
    explicit Scene(const std::string& name);
    ~Scene() = default;
    
    // ==============================================================================
    // إدارة الطبقات / Layer Management
    // ==============================================================================
    
    /// إضافة طبقة / Add layer
    /// name: اسم الطبقة / Layer name
    /// zOrder: ترتيب الطبقة / Layer order
    Layer* AddLayer(const std::string& name, Int32 zOrder = 0);
    
    /// إزالة طبقة / Remove layer
    void RemoveLayer(const std::string& name);
    
    /// الحصول على طبقة / Get layer
    Layer* GetLayer(const std::string& name);
    const Layer* GetLayer(const std::string& name) const;
    
    /// عدد الطبقات / Layer count
    size_t GetLayerCount() const { return m_layers.size(); }
    
    // ==============================================================================
    // الكاميرا / Camera
    // ==============================================================================
    
    /// تعيين كاميرا المشهد / Set scene camera
    void SetCamera(const Camera2DRef& camera);
    
    /// الحصول على الكاميرا / Get camera
    Camera2DRef GetCamera() const { return m_camera; }
    
    // ==============================================================================
    // دورة الحياة / Lifecycle
    // ==============================================================================
    
    /// دخول المشهد (يُستدعى عند التبديل إليه)
    /// Enter scene (called when switching to it)
    void OnEnter();
    
    /// خروج المشهد (يُستدعى عند التبديل منه)
    /// Exit scene (called when switching from it)
    void OnExit();
    
    /// تحديث المشهد / Update scene
    void Update(Float32 deltaTime);
    
    /// رسم المشهد / Draw scene
    void Draw() const;
    
    // ==============================================================================
    // خصائص / Properties
    // ==============================================================================
    
    const std::string& GetName() const { return m_name; }
    bool IsActive() const { return m_active; }
    
    /// تعيين دوال دورة الحياة / Set lifecycle callbacks
    void SetOnEnter(std::function<void()> callback) { m_onEnter = callback; }
    void SetOnExit(std::function<void()> callback) { m_onExit = callback; }
    void SetOnUpdate(std::function<void(Float32)> callback) { m_onUpdate = callback; }
    void SetOnDraw(std::function<void()> callback) { m_onDraw = callback; }
    
    /// مسح المشهد / Clear scene
    void Clear();

private:
    std::string m_name;
    bool m_active;
    Camera2DRef m_camera;
    std::vector<std::shared_ptr<Layer>> m_layers;
    
    // دوال دورة الحياة / Lifecycle callbacks
    std::function<void()> m_onEnter;
    std::function<void()> m_onExit;
    std::function<void(Float32)> m_onUpdate;
    std::function<void()> m_onDraw;
    
    void SortLayers();
};

// ==============================================================================
// مدير المشاهد / Scene Manager
// ==============================================================================
class SceneManager {
public:
    SceneManager();
    ~SceneManager() = default;
    
    /// إضافة مشهد / Add scene
    Scene* AddScene(const std::string& name);
    
    /// إزالة مشهد / Remove scene
    void RemoveScene(const std::string& name);
    
    /// الحصول على مشهد / Get scene
    Scene* GetScene(const std::string& name);
    
    /// التبديل إلى مشهد / Switch to scene
    void SwitchTo(const std::string& name);
    
    /// الحصول على المشهد الحالي / Get current scene
    Scene* GetCurrentScene() { return m_currentScene; }
    const Scene* GetCurrentScene() const { return m_currentScene; }
    
    /// تحديث المشهد الحالي / Update current scene
    void Update(Float32 deltaTime);
    
    /// رسم المشهد الحالي / Draw current scene
    void Draw() const;
    
    /// المدير العام / Global instance
    static SceneManager& GetInstance();

private:
    std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
    Scene* m_currentScene;
};

// أنواع مستعارة / Type aliases
using SceneRef = std::shared_ptr<Scene>;
using LayerRef = std::shared_ptr<Layer>;

} // namespace graphics
} // namespace sad

// اختصار / Shortcut
namespace SadGraphics {
    using Scene = sad::graphics::Scene;
    using SceneManager = sad::graphics::SceneManager;
    using Layer = sad::graphics::Layer;
    using Entity = sad::graphics::Entity;
}
