// ==============================================================================
// ecs.h - نظام الكيانات والمكونات / Entity-Component-System
// ==============================================================================
// الوصف: نمط تصميم ECS لبناء ألعاب مرنة وعالية الأداء
// Description: ECS design pattern for building flexible and high-performance games
// ==============================================================================
// المميزات / Features:
//   - كيانات خفيفة (معرفات فقط) / Lightweight entities (IDs only)
//   - مكونات بيانات نقية / Pure data components
//   - أنظمة تعالج المكونات / Systems that process components
//   - استعلامات مرنة / Flexible queries
//   - تجمع الذاكرة / Memory pooling
//   - دعم المكونات الديناميكية / Dynamic component support
// ==============================================================================
// الاستخدام / Usage:
//   // إنشاء عالم / Create world
//   World world;
//
//   // إنشاء كيان / Create entity
//   Entity player = world.CreateEntity();
//
//   // إضافة مكونات / Add components
//   world.AddComponent<Position>(player, {100.0f, 200.0f});
//   world.AddComponent<Velocity>(player, {5.0f, 0.0f});
//
//   // تسجيل نظام / Register system
//   world.RegisterSystem<MovementSystem>();
//
//   // تحديث / Update
//   world.Update(deltaTime);
// ==============================================================================

#ifndef SAD_GRAPHICS_ECS_H
#define SAD_GRAPHICS_ECS_H

#include "types.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <bitset>
#include <functional>
#include <typeindex>
#include <algorithm>
#include <queue>

namespace sad {
namespace graphics {
namespace ecs {

// ==============================================================================
// الثوابت / Constants
// ==============================================================================

/// أقصى عدد أنواع المكونات / Maximum component types
constexpr size_t MAX_COMPONENTS = 64;

/// معرف كيان غير صالح / Invalid entity ID
constexpr UInt32 INVALID_ENTITY = 0;

// ==============================================================================
// الأنواع الأساسية / Basic Types
// ==============================================================================

/// معرف الكيان / Entity identifier
/// الكيان هو مجرد معرف رقمي - البيانات الفعلية في المكونات
using Entity = UInt32;

/// توقيع المكونات - قناع بت يمثل المكونات الموجودة
/// Component signature - bitmask representing present components
using ComponentMask = std::bitset<MAX_COMPONENTS>;

/// معرف نوع المكون / Component type identifier
using ComponentTypeId = UInt32;

// ==============================================================================
// مسجل أنواع المكونات / Component Type Registry
// ==============================================================================

/// يعطي كل نوع مكون معرفاً فريداً
/// Assigns each component type a unique ID
class ComponentTypeRegistry {
public:
    /// الحصول على معرف نوع المكون
    /// Get component type ID
    template<typename T>
    static ComponentTypeId GetId() {
        static ComponentTypeId id = s_nextId++;
        return id;
    }
    
    /// عدد الأنواع المسجلة / Number of registered types
    static ComponentTypeId GetCount() { return s_nextId; }
    
private:
    static inline ComponentTypeId s_nextId = 0;
};

// ==============================================================================
// تجمع المكونات / Component Pool
// ==============================================================================

/// واجهة تجمع المكونات / Component pool interface
class IComponentPool {
public:
    virtual ~IComponentPool() = default;
    virtual void RemoveEntity(Entity entity) = 0;
    virtual void Clear() = 0;
    virtual size_t Size() const = 0;
    virtual bool HasEntity(Entity entity) const = 0;
};

/// تجمع مكونات مكتوب
/// Typed component pool
/// يخزن جميع مكونات من نوع معين بكفاءة
template<typename T>
class ComponentPool : public IComponentPool {
public:
    /// إضافة مكون لكيان / Add component to entity
    T& Add(Entity entity, const T& component) {
        if (m_entityToIndex.count(entity)) {
            // تحديث المكون الموجود / Update existing component
            return m_components[m_entityToIndex[entity]] = component;
        }
        
        size_t index = m_components.size();
        m_components.push_back(component);
        m_entityToIndex[entity] = index;
        m_indexToEntity[index] = entity;
        
        return m_components.back();
    }
    
    /// إضافة مكون بالبناء المباشر / Add component with direct construction
    template<typename... Args>
    T& Emplace(Entity entity, Args&&... args) {
        if (m_entityToIndex.count(entity)) {
            return m_components[m_entityToIndex[entity]] = T(std::forward<Args>(args)...);
        }
        
        size_t index = m_components.size();
        m_components.emplace_back(std::forward<Args>(args)...);
        m_entityToIndex[entity] = index;
        m_indexToEntity[index] = entity;
        
        return m_components.back();
    }
    
    /// الحصول على مكون الكيان / Get entity's component
    T& Get(Entity entity) {
        return m_components[m_entityToIndex.at(entity)];
    }
    
    const T& Get(Entity entity) const {
        return m_components[m_entityToIndex.at(entity)];
    }
    
    /// محاولة الحصول على مكون / Try to get component
    T* TryGet(Entity entity) {
        auto it = m_entityToIndex.find(entity);
        if (it != m_entityToIndex.end()) {
            return &m_components[it->second];
        }
        return nullptr;
    }
    
    const T* TryGet(Entity entity) const {
        auto it = m_entityToIndex.find(entity);
        if (it != m_entityToIndex.end()) {
            return &m_components[it->second];
        }
        return nullptr;
    }
    
    /// هل الكيان يملك هذا المكون / Does entity have this component
    bool HasEntity(Entity entity) const override {
        return m_entityToIndex.count(entity) > 0;
    }
    
    /// إزالة مكون كيان / Remove entity's component
    void RemoveEntity(Entity entity) override {
        if (!m_entityToIndex.count(entity)) return;
        
        size_t indexOfRemoved = m_entityToIndex[entity];
        size_t indexOfLast = m_components.size() - 1;
        
        if (indexOfRemoved != indexOfLast) {
            // نقل الأخير لملء الفجوة / Move last to fill gap
            m_components[indexOfRemoved] = std::move(m_components[indexOfLast]);
            
            Entity lastEntity = m_indexToEntity[indexOfLast];
            m_entityToIndex[lastEntity] = indexOfRemoved;
            m_indexToEntity[indexOfRemoved] = lastEntity;
        }
        
        m_components.pop_back();
        m_entityToIndex.erase(entity);
        m_indexToEntity.erase(indexOfLast);
    }
    
    /// مسح الكل / Clear all
    void Clear() override {
        m_components.clear();
        m_entityToIndex.clear();
        m_indexToEntity.clear();
    }
    
    /// الحجم / Size
    size_t Size() const override { return m_components.size(); }
    
    /// التكرار على المكونات / Iterate over components
    template<typename Func>
    void ForEach(Func&& func) {
        for (size_t i = 0; i < m_components.size(); ++i) {
            func(m_indexToEntity[i], m_components[i]);
        }
    }
    
    /// الحصول على جميع الكيانات / Get all entities
    std::vector<Entity> GetEntities() const {
        std::vector<Entity> entities;
        entities.reserve(m_indexToEntity.size());
        for (const auto& pair : m_indexToEntity) {
            entities.push_back(pair.second);
        }
        return entities;
    }

private:
    std::vector<T> m_components;                        // المكونات المتجاورة / Contiguous components
    std::unordered_map<Entity, size_t> m_entityToIndex; // كيان -> فهرس / Entity -> Index
    std::unordered_map<size_t, Entity> m_indexToEntity; // فهرس -> كيان / Index -> Entity
};

// ==============================================================================
// واجهة النظام / System Interface
// ==============================================================================

/// واجهة النظام الأساسية
/// Base system interface
/// النظام يعالج الكيانات التي تملك مكونات معينة
class ISystem {
public:
    virtual ~ISystem() = default;
    
    /// التهيئة / Initialize
    virtual void OnCreate() {}
    
    /// التدمير / Destroy
    virtual void OnDestroy() {}
    
    /// التحديث / Update
    virtual void Update(Float32 deltaTime) = 0;
    
    /// الرسم (اختياري) / Draw (optional)
    virtual void Draw() {}
    
    /// الأولوية (أعلى = يُنفذ أولاً) / Priority (higher = executed first)
    virtual Int32 GetPriority() const { return 0; }
    
    /// هل النظام مفعّل / Is system enabled
    bool IsEnabled() const { return m_enabled; }
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    
protected:
    bool m_enabled = true;
};

// ==============================================================================
// العالم / World
// ==============================================================================

/// العالم - الحاوية الرئيسية لـ ECS
/// World - Main ECS container
/// يدير الكيانات والمكونات والأنظمة
class World {
public:
    // ==========================================================================
    // البناء والهدم / Construction & Destruction
    // ==========================================================================
    
    World() = default;
    ~World() {
        // تدمير جميع الأنظمة / Destroy all systems
        for (auto& pair : m_systems) {
            pair.second->OnDestroy();
        }
    }
    
    // ==========================================================================
    // إدارة الكيانات / Entity Management
    // ==========================================================================
    
    /// إنشاء كيان جديد / Create new entity
    Entity CreateEntity() {
        Entity entity;
        
        if (!m_freeEntities.empty()) {
            // إعادة استخدام معرف حر / Reuse free ID
            entity = m_freeEntities.front();
            m_freeEntities.pop();
        } else {
            // معرف جديد / New ID
            entity = ++m_nextEntity;
        }
        
        m_entities.insert(entity);
        m_signatures[entity].reset();
        
        return entity;
    }
    
    /// إنشاء كيان مع اسم / Create entity with name
    Entity CreateEntity(const std::string& name) {
        Entity entity = CreateEntity();
        m_entityNames[entity] = name;
        m_nameToEntity[name] = entity;
        return entity;
    }
    
    /// تدمير كيان / Destroy entity
    void DestroyEntity(Entity entity) {
        if (!IsEntityValid(entity)) return;
        
        // إزالة من جميع تجمعات المكونات / Remove from all component pools
        for (auto& pair : m_componentPools) {
            pair.second->RemoveEntity(entity);
        }
        
        // إزالة الاسم / Remove name
        auto nameIt = m_entityNames.find(entity);
        if (nameIt != m_entityNames.end()) {
            m_nameToEntity.erase(nameIt->second);
            m_entityNames.erase(nameIt);
        }
        
        m_entities.erase(entity);
        m_signatures.erase(entity);
        m_freeEntities.push(entity);
    }
    
    /// هل الكيان صالح / Is entity valid
    bool IsEntityValid(Entity entity) const {
        return m_entities.count(entity) > 0;
    }
    
    /// عدد الكيانات / Entity count
    size_t GetEntityCount() const {
        return m_entities.size();
    }
    
    /// الحصول على كيان بالاسم / Get entity by name
    Entity GetEntityByName(const std::string& name) const {
        auto it = m_nameToEntity.find(name);
        return (it != m_nameToEntity.end()) ? it->second : INVALID_ENTITY;
    }
    
    /// الحصول على اسم الكيان / Get entity name
    const std::string& GetEntityName(Entity entity) const {
        static const std::string empty;
        auto it = m_entityNames.find(entity);
        return (it != m_entityNames.end()) ? it->second : empty;
    }
    
    // ==========================================================================
    // إدارة المكونات / Component Management
    // ==========================================================================
    
    /// إضافة مكون لكيان / Add component to entity
    template<typename T, typename... Args>
    T& AddComponent(Entity entity, Args&&... args) {
        ComponentTypeId typeId = ComponentTypeRegistry::GetId<T>();
        
        // إنشاء تجمع إذا لم يكن موجوداً / Create pool if doesn't exist
        if (m_componentPools.find(typeId) == m_componentPools.end()) {
            m_componentPools[typeId] = std::make_unique<ComponentPool<T>>();
        }
        
        auto* pool = static_cast<ComponentPool<T>*>(m_componentPools[typeId].get());
        T& component = pool->Emplace(entity, std::forward<Args>(args)...);
        
        // تحديث التوقيع / Update signature
        m_signatures[entity].set(typeId);
        
        return component;
    }
    
    /// إزالة مكون من كيان / Remove component from entity
    template<typename T>
    void RemoveComponent(Entity entity) {
        ComponentTypeId typeId = ComponentTypeRegistry::GetId<T>();
        
        auto it = m_componentPools.find(typeId);
        if (it != m_componentPools.end()) {
            it->second->RemoveEntity(entity);
            m_signatures[entity].reset(typeId);
        }
    }
    
    /// الحصول على مكون / Get component
    template<typename T>
    T& GetComponent(Entity entity) {
        ComponentTypeId typeId = ComponentTypeRegistry::GetId<T>();
        auto* pool = static_cast<ComponentPool<T>*>(m_componentPools[typeId].get());
        return pool->Get(entity);
    }
    
    template<typename T>
    const T& GetComponent(Entity entity) const {
        ComponentTypeId typeId = ComponentTypeRegistry::GetId<T>();
        auto* pool = static_cast<const ComponentPool<T>*>(m_componentPools.at(typeId).get());
        return pool->Get(entity);
    }
    
    /// محاولة الحصول على مكون / Try to get component
    template<typename T>
    T* TryGetComponent(Entity entity) {
        ComponentTypeId typeId = ComponentTypeRegistry::GetId<T>();
        auto it = m_componentPools.find(typeId);
        if (it == m_componentPools.end()) return nullptr;
        
        auto* pool = static_cast<ComponentPool<T>*>(it->second.get());
        return pool->TryGet(entity);
    }
    
    /// هل الكيان يملك مكوناً / Does entity have component
    template<typename T>
    bool HasComponent(Entity entity) const {
        ComponentTypeId typeId = ComponentTypeRegistry::GetId<T>();
        auto it = m_signatures.find(entity);
        return (it != m_signatures.end()) && it->second.test(typeId);
    }
    
    /// هل الكيان يملك جميع المكونات / Does entity have all components
    template<typename... Ts>
    bool HasComponents(Entity entity) const {
        return (HasComponent<Ts>(entity) && ...);
    }
    
    // ==========================================================================
    // الاستعلامات / Queries
    // ==========================================================================
    
    /// الحصول على جميع الكيانات بمكونات معينة
    /// Get all entities with specific components
    template<typename... Ts>
    std::vector<Entity> Query() const {
        std::vector<Entity> result;
        
        ComponentMask required;
        (required.set(ComponentTypeRegistry::GetId<Ts>()), ...);
        
        for (Entity entity : m_entities) {
            auto it = m_signatures.find(entity);
            if (it != m_signatures.end()) {
                if ((it->second & required) == required) {
                    result.push_back(entity);
                }
            }
        }
        
        return result;
    }
    
    /// التكرار على كيانات بمكونات معينة
    /// Iterate over entities with specific components
    template<typename... Ts, typename Func>
    void ForEach(Func&& func) {
        ComponentMask required;
        (required.set(ComponentTypeRegistry::GetId<Ts>()), ...);
        
        for (Entity entity : m_entities) {
            auto it = m_signatures.find(entity);
            if (it != m_signatures.end() && (it->second & required) == required) {
                func(entity, GetComponent<Ts>(entity)...);
            }
        }
    }
    
    // ==========================================================================
    // إدارة الأنظمة / System Management
    // ==========================================================================
    
    /// تسجيل نظام / Register system
    template<typename T, typename... Args>
    T& RegisterSystem(Args&&... args) {
        static_assert(std::is_base_of<ISystem, T>::value,
                      "T يجب أن يرث من ISystem / T must derive from ISystem");
        
        std::type_index typeIndex(typeid(T));
        
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        T& ref = *system;
        
        m_systems[typeIndex] = std::move(system);
        
        // ترتيب الأنظمة حسب الأولوية / Sort systems by priority
        RebuildSystemOrder();
        
        ref.OnCreate();
        return ref;
    }
    
    /// إلغاء تسجيل نظام / Unregister system
    template<typename T>
    void UnregisterSystem() {
        std::type_index typeIndex(typeid(T));
        
        auto it = m_systems.find(typeIndex);
        if (it != m_systems.end()) {
            it->second->OnDestroy();
            m_systems.erase(it);
            RebuildSystemOrder();
        }
    }
    
    /// الحصول على نظام / Get system
    template<typename T>
    T* GetSystem() {
        std::type_index typeIndex(typeid(T));
        auto it = m_systems.find(typeIndex);
        return (it != m_systems.end()) ? static_cast<T*>(it->second.get()) : nullptr;
    }
    
    // ==========================================================================
    // التحديث والرسم / Update & Draw
    // ==========================================================================
    
    /// تحديث جميع الأنظمة / Update all systems
    void Update(Float32 deltaTime) {
        for (auto* system : m_systemOrder) {
            if (system->IsEnabled()) {
                system->Update(deltaTime);
            }
        }
    }
    
    /// رسم جميع الأنظمة / Draw all systems
    void Draw() {
        for (auto* system : m_systemOrder) {
            if (system->IsEnabled()) {
                system->Draw();
            }
        }
    }
    
    // ==========================================================================
    // التنظيف / Cleanup
    // ==========================================================================
    
    /// مسح جميع الكيانات / Clear all entities
    void ClearEntities() {
        for (auto& pair : m_componentPools) {
            pair.second->Clear();
        }
        m_entities.clear();
        m_signatures.clear();
        m_entityNames.clear();
        m_nameToEntity.clear();
        while (!m_freeEntities.empty()) m_freeEntities.pop();
        m_nextEntity = 0;
    }
    
    /// مسح كل شيء / Clear everything
    void Clear() {
        ClearEntities();
        for (auto& pair : m_systems) {
            pair.second->OnDestroy();
        }
        m_systems.clear();
        m_systemOrder.clear();
        m_componentPools.clear();
    }

private:
    /// إعادة بناء ترتيب الأنظمة / Rebuild system order
    void RebuildSystemOrder() {
        m_systemOrder.clear();
        for (auto& pair : m_systems) {
            m_systemOrder.push_back(pair.second.get());
        }
        std::sort(m_systemOrder.begin(), m_systemOrder.end(),
            [](ISystem* a, ISystem* b) {
                return a->GetPriority() > b->GetPriority();
            });
    }
    
    // الكيانات / Entities
    std::unordered_set<Entity> m_entities;
    std::queue<Entity> m_freeEntities;
    Entity m_nextEntity = 0;
    
    // التوقيعات / Signatures
    std::unordered_map<Entity, ComponentMask> m_signatures;
    
    // الأسماء / Names
    std::unordered_map<Entity, std::string> m_entityNames;
    std::unordered_map<std::string, Entity> m_nameToEntity;
    
    // تجمعات المكونات / Component pools
    std::unordered_map<ComponentTypeId, std::unique_ptr<IComponentPool>> m_componentPools;
    
    // الأنظمة / Systems
    std::unordered_map<std::type_index, std::unique_ptr<ISystem>> m_systems;
    std::vector<ISystem*> m_systemOrder;
};

// ==============================================================================
// مكونات مدمجة شائعة / Common Built-in Components
// ==============================================================================

/// مكون الموقع / Position component
struct PositionComponent {
    Float32 x = 0.0f;
    Float32 y = 0.0f;
    
    PositionComponent() = default;
    PositionComponent(Float32 x_, Float32 y_) : x(x_), y(y_) {}
    PositionComponent(const Vec2& v) : x(v.x), y(v.y) {}
    
    Vec2 ToVec2() const { return Vec2(x, y); }
};

/// مكون السرعة / Velocity component
struct VelocityComponent {
    Float32 x = 0.0f;
    Float32 y = 0.0f;
    
    VelocityComponent() = default;
    VelocityComponent(Float32 x_, Float32 y_) : x(x_), y(y_) {}
    VelocityComponent(const Vec2& v) : x(v.x), y(v.y) {}
    
    Vec2 ToVec2() const { return Vec2(x, y); }
};

/// مكون التحويل / Transform component
struct TransformComponent {
    Vec2 position{0.0f, 0.0f};
    Vec2 scale{1.0f, 1.0f};
    Float32 rotation = 0.0f;    // بالراديان / In radians
    
    TransformComponent() = default;
    TransformComponent(const Vec2& pos) : position(pos) {}
    TransformComponent(const Vec2& pos, const Vec2& scl, Float32 rot = 0.0f) 
        : position(pos), scale(scl), rotation(rot) {}
};

/// مكون الرسوم المتحركة / Sprite component
struct SpriteComponent {
    UInt32 textureId = 0;       // معرف النسيج / Texture ID
    Rect sourceRect{0, 0, 0, 0};// مستطيل المصدر / Source rectangle
    Color tint = Color::White;  // لون التلوين / Tint color
    Int32 layer = 0;            // طبقة الرسم / Draw layer
    bool flipX = false;         // قلب أفقي / Horizontal flip
    bool flipY = false;         // قلب عمودي / Vertical flip
    bool visible = true;        // مرئي / Visible
};

/// مكون الصحة / Health component
struct HealthComponent {
    Float32 current = 100.0f;   // الصحة الحالية / Current health
    Float32 maximum = 100.0f;   // الصحة القصوى / Maximum health
    bool isDead = false;        // ميت / Dead
    
    HealthComponent() = default;
    HealthComponent(Float32 hp) : current(hp), maximum(hp), isDead(false) {}
    
    void TakeDamage(Float32 damage) {
        current = (current - damage < 0) ? 0 : current - damage;
        if (current <= 0) isDead = true;
    }
    
    void Heal(Float32 amount) {
        current = (current + amount > maximum) ? maximum : current + amount;
        if (current > 0) isDead = false;
    }
    
    Float32 GetPercentage() const { 
        return (maximum > 0) ? current / maximum : 0.0f; 
    }
};

/// مكون صندوق التصادم / Collider component
struct BoxColliderComponent {
    Float32 width = 0.0f;
    Float32 height = 0.0f;
    Float32 offsetX = 0.0f;     // إزاحة من الموقع / Offset from position
    Float32 offsetY = 0.0f;
    bool isTrigger = false;     // مشغّل (لا فيزياء) / Trigger (no physics)
    bool enabled = true;
    
    BoxColliderComponent() = default;
    BoxColliderComponent(Float32 w, Float32 h) : width(w), height(h) {}
};

/// مكون الجسم الصلب / Rigidbody component
struct RigidbodyComponent {
    Float32 mass = 1.0f;            // الكتلة / Mass
    Float32 drag = 0.0f;            // المقاومة / Drag
    Float32 angularDrag = 0.05f;    // المقاومة الزاوية / Angular drag
    Float32 gravityScale = 1.0f;    // مقياس الجاذبية / Gravity scale
    bool isKinematic = false;       // حركي (لا تأثر فيزيائي) / Kinematic
    bool freezeRotation = false;    // تجميد الدوران / Freeze rotation
    
    Vec2 velocity{0, 0};            // السرعة الخطية / Linear velocity
    Float32 angularVelocity = 0.0f; // السرعة الزاوية / Angular velocity
};

/// مكون العلامة / Tag component
struct TagComponent {
    std::string tag;
    
    TagComponent() = default;
    TagComponent(const std::string& t) : tag(t) {}
};

/// مكون الطبقة / Layer component
struct LayerComponent {
    Int32 layer = 0;
    
    LayerComponent() = default;
    LayerComponent(Int32 l) : layer(l) {}
};

// ==============================================================================
// أنظمة مدمجة / Built-in Systems
// ==============================================================================

/// نظام الحركة / Movement system
/// يحدث المواقع بناءً على السرعة
class MovementSystem : public ISystem {
public:
    void Update(Float32 deltaTime) override {
        if (!m_world) return;
        
        m_world->ForEach<PositionComponent, VelocityComponent>(
            [deltaTime](Entity entity, PositionComponent& pos, VelocityComponent& vel) {
                pos.x += vel.x * deltaTime;
                pos.y += vel.y * deltaTime;
            }
        );
    }
    
    Int32 GetPriority() const override { return 100; }
    
    void SetWorld(World* world) { m_world = world; }
    
private:
    World* m_world = nullptr;
};

/// نظام العمر / Lifetime system
struct LifetimeComponent {
    Float32 remaining = 0.0f;   // الوقت المتبقي / Remaining time
    
    LifetimeComponent() = default;
    LifetimeComponent(Float32 lifetime) : remaining(lifetime) {}
};

class LifetimeSystem : public ISystem {
public:
    void Update(Float32 deltaTime) override {
        if (!m_world) return;
        
        std::vector<Entity> toDestroy;
        
        m_world->ForEach<LifetimeComponent>(
            [deltaTime, &toDestroy](Entity entity, LifetimeComponent& lt) {
                lt.remaining -= deltaTime;
                if (lt.remaining <= 0.0f) {
                    toDestroy.push_back(entity);
                }
            }
        );
        
        for (Entity entity : toDestroy) {
            m_world->DestroyEntity(entity);
        }
    }
    
    Int32 GetPriority() const override { return 90; }
    
    void SetWorld(World* world) { m_world = world; }
    
private:
    World* m_world = nullptr;
};

} // namespace ecs
} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_ECS_H
