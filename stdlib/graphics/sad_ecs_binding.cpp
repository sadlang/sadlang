// ═══════════════════════════════════════════════════════════════════════════════
// sad_ecs_binding.cpp — تنفيذ ربط ECS وناقل الأحداث
// ECS & Event Bus Binding Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_ecs_binding.h"
#include <map>
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <queue>
#include <cmath>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// هياكل البيانات الداخلية / Internal Data Structures
// ═══════════════════════════════════════════════════════════════

struct PositionComponent { float x = 0, y = 0; };
struct VelocityComponent { float vx = 0, vy = 0; };
struct HealthComponent { float hp = 100, maxHp = 100; };
struct TagComponent { std::string tag; };

struct EntityData {
    int id = 0;
    bool alive = true;
    bool hasPosition = false;
    bool hasVelocity = false;
    bool hasHealth = false;
    bool hasTag = false;
    PositionComponent position;
    VelocityComponent velocity;
    HealthComponent health;
    TagComponent tag;
};

struct WorldData {
    int id = 0;
    std::unordered_map<int, EntityData> entities;
    int nextEntityId = 1;
};

struct EventData {
    std::string name;
    std::string data;
};

struct EventBusData {
    int id = 0;
    std::unordered_map<std::string, int> listenerCounts;
    std::queue<EventData> pendingEvents;
};

static std::map<int, WorldData> g_worlds;
static std::map<int, EventBusData> g_eventBuses;
static int g_nextWorldId = 1;
static int g_nextBusId = 1;

// ═══════════════════════════════════════════════════════════════
// تنفيذ ECS / ECS Implementation
// ═══════════════════════════════════════════════════════════════

int ecs_world_create_impl() {
    int id = g_nextWorldId++;
    g_worlds[id].id = id;
    return id;
}

void ecs_world_destroy_impl(int worldId) {
    g_worlds.erase(worldId);
}

int ecs_entity_create_impl(int worldId) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return -1;
    int eid = it->second.nextEntityId++;
    it->second.entities[eid].id = eid;
    return eid;
}

void ecs_entity_destroy_impl(int worldId, int entityId) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return;
    auto eit = it->second.entities.find(entityId);
    if (eit != it->second.entities.end()) {
        eit->second.alive = false;
    }
}

void ecs_add_position_impl(int worldId, int entityId, float x, float y) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return;
    auto eit = it->second.entities.find(entityId);
    if (eit == it->second.entities.end()) return;
    eit->second.hasPosition = true;
    eit->second.position = {x, y};
}

void ecs_add_velocity_impl(int worldId, int entityId, float vx, float vy) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return;
    auto eit = it->second.entities.find(entityId);
    if (eit == it->second.entities.end()) return;
    eit->second.hasVelocity = true;
    eit->second.velocity = {vx, vy};
}

void ecs_add_health_impl(int worldId, int entityId, float hp, float maxHp) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return;
    auto eit = it->second.entities.find(entityId);
    if (eit == it->second.entities.end()) return;
    eit->second.hasHealth = true;
    eit->second.health = {hp, maxHp};
}

void ecs_add_tag_impl(int worldId, int entityId, const std::string& tag) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return;
    auto eit = it->second.entities.find(entityId);
    if (eit == it->second.entities.end()) return;
    eit->second.hasTag = true;
    eit->second.tag = {tag};
}

float ecs_get_x_impl(int worldId, int entityId) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return 0;
    auto eit = it->second.entities.find(entityId);
    if (eit == it->second.entities.end() || !eit->second.hasPosition) return 0;
    return eit->second.position.x;
}

float ecs_get_y_impl(int worldId, int entityId) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return 0;
    auto eit = it->second.entities.find(entityId);
    if (eit == it->second.entities.end() || !eit->second.hasPosition) return 0;
    return eit->second.position.y;
}

void ecs_set_position_impl(int worldId, int entityId, float x, float y) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return;
    auto eit = it->second.entities.find(entityId);
    if (eit == it->second.entities.end()) return;
    eit->second.hasPosition = true;
    eit->second.position = {x, y};
}

float ecs_get_health_impl(int worldId, int entityId) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return 0;
    auto eit = it->second.entities.find(entityId);
    if (eit == it->second.entities.end() || !eit->second.hasHealth) return 0;
    return eit->second.health.hp;
}

void ecs_set_health_impl(int worldId, int entityId, float hp) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return;
    auto eit = it->second.entities.find(entityId);
    if (eit == it->second.entities.end() || !eit->second.hasHealth) return;
    eit->second.health.hp = hp;
}

void ecs_world_update_impl(int worldId, float deltaTime) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return;
    // نظام الحركة: تحديث الموقع بناء على السرعة
    // Movement system: update position based on velocity
    for (auto& [eid, e] : it->second.entities) {
        if (!e.alive) continue;
        if (e.hasPosition && e.hasVelocity) {
            e.position.x += e.velocity.vx * deltaTime;
            e.position.y += e.velocity.vy * deltaTime;
        }
    }
    // تنظيف الكيانات الميتة / Clean dead entities
    for (auto eit = it->second.entities.begin(); eit != it->second.entities.end();) {
        if (!eit->second.alive) {
            eit = it->second.entities.erase(eit);
        } else {
            ++eit;
        }
    }
}

int ecs_entity_count_impl(int worldId) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return 0;
    return static_cast<int>(it->second.entities.size());
}

bool ecs_entity_alive_impl(int worldId, int entityId) {
    auto it = g_worlds.find(worldId);
    if (it == g_worlds.end()) return false;
    auto eit = it->second.entities.find(entityId);
    return eit != it->second.entities.end() && eit->second.alive;
}

// ═══════════════════════════════════════════════════════════════
// تنفيذ ناقل الأحداث / Event Bus Implementation
// ═══════════════════════════════════════════════════════════════

int eventbus_create_impl() {
    int id = g_nextBusId++;
    g_eventBuses[id].id = id;
    return id;
}

void eventbus_destroy_impl(int busId) {
    g_eventBuses.erase(busId);
}

void eventbus_publish_impl(int busId, const std::string& eventName) {
    auto it = g_eventBuses.find(busId);
    if (it == g_eventBuses.end()) return;
    it->second.pendingEvents.push({eventName, ""});
}

void eventbus_publish_data_impl(int busId, const std::string& eventName, const std::string& data) {
    auto it = g_eventBuses.find(busId);
    if (it == g_eventBuses.end()) return;
    it->second.pendingEvents.push({eventName, data});
}

int eventbus_listener_count_impl(int busId, const std::string& eventName) {
    auto it = g_eventBuses.find(busId);
    if (it == g_eventBuses.end()) return 0;
    auto lit = it->second.listenerCounts.find(eventName);
    return lit != it->second.listenerCounts.end() ? lit->second : 0;
}

void eventbus_clear_impl(int busId) {
    auto it = g_eventBuses.find(busId);
    if (it == g_eventBuses.end()) return;
    it->second.listenerCounts.clear();
    while (!it->second.pendingEvents.empty()) it->second.pendingEvents.pop();
}

void eventbus_clear_event_impl(int busId, const std::string& eventName) {
    auto it = g_eventBuses.find(busId);
    if (it == g_eventBuses.end()) return;
    it->second.listenerCounts.erase(eventName);
}

int eventbus_pending_count_impl(int busId) {
    auto it = g_eventBuses.find(busId);
    if (it == g_eventBuses.end()) return 0;
    return static_cast<int>(it->second.pendingEvents.size());
}

void eventbus_process_impl(int busId) {
    auto it = g_eventBuses.find(busId);
    if (it == g_eventBuses.end()) return;
    while (!it->second.pendingEvents.empty()) {
        it->second.pendingEvents.pop();
    }
}

} // namespace graphics
} // namespace stdlib
} // namespace sad
