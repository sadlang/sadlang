// ═══════════════════════════════════════════════════════════════════════════════
// sad_ecs_binding.h — ربط نظام ECS وناقل الأحداث بلغة ص
// ECS & Event Bus Binding for SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>
#include <vector>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// نظام الكيانات والمكونات / Entity-Component-System
// ═══════════════════════════════════════════════════════════════

/// إنشاء عالم ECS جديد / Create a new ECS world
int ecs_world_create_impl();

/// تدمير عالم ECS / Destroy ECS world
void ecs_world_destroy_impl(int worldId);

/// إنشاء كيان جديد / Create a new entity
int ecs_entity_create_impl(int worldId);

/// تدمير كيان / Destroy entity
void ecs_entity_destroy_impl(int worldId, int entityId);

/// إضافة مكون موقع لكيان / Add position component
void ecs_add_position_impl(int worldId, int entityId, float x, float y);

/// إضافة مكون سرعة لكيان / Add velocity component
void ecs_add_velocity_impl(int worldId, int entityId, float vx, float vy);

/// إضافة مكون صحة لكيان / Add health component
void ecs_add_health_impl(int worldId, int entityId, float hp, float maxHp);

/// إضافة مكون اسم لكيان / Add name/tag component
void ecs_add_tag_impl(int worldId, int entityId, const std::string& tag);

/// الحصول على موقع X الكيان / Get entity X position
float ecs_get_x_impl(int worldId, int entityId);

/// الحصول على موقع Y الكيان / Get entity Y position
float ecs_get_y_impl(int worldId, int entityId);

/// تعيين موقع الكيان / Set entity position
void ecs_set_position_impl(int worldId, int entityId, float x, float y);

/// الحصول على الصحة / Get health
float ecs_get_health_impl(int worldId, int entityId);

/// تعيين الصحة / Set health
void ecs_set_health_impl(int worldId, int entityId, float hp);

/// تحديث العالم / Update world systems
void ecs_world_update_impl(int worldId, float deltaTime);

/// عدد الكيانات / Get entity count
int ecs_entity_count_impl(int worldId);

/// هل الكيان حي؟ / Is entity alive?
bool ecs_entity_alive_impl(int worldId, int entityId);

// ═══════════════════════════════════════════════════════════════
// ناقل الأحداث / Event Bus
// ═══════════════════════════════════════════════════════════════

/// إنشاء ناقل أحداث جديد / Create event bus
int eventbus_create_impl();

/// تدمير ناقل أحداث / Destroy event bus
void eventbus_destroy_impl(int busId);

/// نشر حدث / Publish event by name
void eventbus_publish_impl(int busId, const std::string& eventName);

/// نشر حدث مع بيانات نصية / Publish event with string data
void eventbus_publish_data_impl(int busId, const std::string& eventName, const std::string& data);

/// عدد المستمعين لحدث / Get listener count for event
int eventbus_listener_count_impl(int busId, const std::string& eventName);

/// مسح كل المستمعين / Clear all listeners
void eventbus_clear_impl(int busId);

/// مسح مستمعي حدث معين / Clear listeners for specific event
void eventbus_clear_event_impl(int busId, const std::string& eventName);

/// عدد الأحداث المعلقة / Get pending event count
int eventbus_pending_count_impl(int busId);

/// معالجة الأحداث المعلقة / Process pending events
void eventbus_process_impl(int busId);

} // namespace graphics
} // namespace stdlib
} // namespace sad
