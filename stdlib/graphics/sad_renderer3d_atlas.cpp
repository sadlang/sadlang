// ═══════════════════════════════════════════════════════════════════════════════
// sad_renderer3d_atlas.cpp — تنفيذ التصيير ثلاثي الأبعاد وأطلس النسيج
// 3D Renderer & Texture Atlas Implementation
// ═══════════════════════════════════════════════════════════════════════════════

#include "sad_renderer3d_atlas.h"
#include <map>
#include <unordered_map>
#include <cmath>
#include <vector>
#include <string>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// هياكل داخلية / Internal Structures
// ═══════════════════════════════════════════════════════════════

struct Vec3f { float x = 0, y = 0, z = 0; };

struct Camera3DData {
    int id = 0;
    Vec3f position = {0, 0, 5};
    Vec3f target = {0, 0, 0};
    Vec3f up = {0, 1, 0};
    float fov = 60.0f;
    float nearPlane = 0.1f;
    float farPlane = 1000.0f;
    float yaw = -90.0f, pitch = 0.0f;
};

struct MeshData {
    int id = 0;
    std::string type;
    int triangleCount = 0;
    int vertexCount = 0;
};

struct MaterialData {
    int id = 0;
    std::string type;
    float color[4] = {1, 1, 1, 1};
    float roughness = 0.5f;
    float metallic = 0.0f;
};

struct LightData {
    int id = 0;
    std::string type; // "directional" or "point"
    Vec3f posOrDir;
    float color[3] = {1, 1, 1};
    float range = 10.0f;
};

struct AtlasRegionData {
    std::string name;
    float x, y, width, height;
};

struct AnimDefData {
    std::string name;
    std::string prefix;
    int frameCount;
    float frameDuration;
};

struct TextureAtlasData {
    int id = 0;
    std::string filepath;
    std::unordered_map<std::string, AtlasRegionData> regions;
    std::unordered_map<std::string, AnimDefData> animations;
};

// ─── المخازن العامة / Global Stores ───
static std::map<int, Camera3DData> g_cameras3d;
static std::map<int, MeshData> g_meshes;
static std::map<int, MaterialData> g_materials;
static std::map<int, LightData> g_lights;
static std::map<int, TextureAtlasData> g_atlases;
static int g_nextCam3dId = 1;
static int g_nextMeshId = 1;
static int g_nextMatId = 1;
static int g_nextLightId = 1;
static int g_nextAtlasId = 1;
static float g_ambientColor[3] = {0.2f, 0.2f, 0.2f};

// ═══════════════════════════════════════════════════════════════
// كاميرا ثلاثية الأبعاد / 3D Camera
// ═══════════════════════════════════════════════════════════════

int camera3d_create_impl(float fov, float nearPlane, float farPlane) {
    int id = g_nextCam3dId++;
    auto& cam = g_cameras3d[id];
    cam.id = id;
    cam.fov = fov;
    cam.nearPlane = nearPlane;
    cam.farPlane = farPlane;
    return id;
}

void camera3d_destroy_impl(int camId) { g_cameras3d.erase(camId); }

void camera3d_set_position_impl(int camId, float x, float y, float z) {
    auto it = g_cameras3d.find(camId);
    if (it == g_cameras3d.end()) return;
    it->second.position = {x, y, z};
}

void camera3d_set_target_impl(int camId, float x, float y, float z) {
    auto it = g_cameras3d.find(camId);
    if (it == g_cameras3d.end()) return;
    it->second.target = {x, y, z};
}

void camera3d_move_forward_impl(int camId, float amount) {
    auto it = g_cameras3d.find(camId);
    if (it == g_cameras3d.end()) return;
    float dx = it->second.target.x - it->second.position.x;
    float dy = it->second.target.y - it->second.position.y;
    float dz = it->second.target.z - it->second.position.z;
    float len = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (len > 0.001f) {
        dx /= len; dy /= len; dz /= len;
        it->second.position.x += dx * amount;
        it->second.position.y += dy * amount;
        it->second.position.z += dz * amount;
    }
}

void camera3d_strafe_impl(int camId, float amount) {
    auto it = g_cameras3d.find(camId);
    if (it == g_cameras3d.end()) return;
    float dx = it->second.target.x - it->second.position.x;
    float dz = it->second.target.z - it->second.position.z;
    float len = std::sqrt(dx * dx + dz * dz);
    if (len > 0.001f) {
        float rx = -dz / len;
        float rz = dx / len;
        it->second.position.x += rx * amount;
        it->second.position.z += rz * amount;
    }
}

void camera3d_rotate_impl(int camId, float yaw, float pitch) {
    auto it = g_cameras3d.find(camId);
    if (it == g_cameras3d.end()) return;
    it->second.yaw += yaw;
    it->second.pitch += pitch;
    if (it->second.pitch > 89.0f) it->second.pitch = 89.0f;
    if (it->second.pitch < -89.0f) it->second.pitch = -89.0f;
    float yawRad = it->second.yaw * 3.14159265f / 180.0f;
    float pitchRad = it->second.pitch * 3.14159265f / 180.0f;
    it->second.target.x = it->second.position.x + std::cos(pitchRad) * std::cos(yawRad);
    it->second.target.y = it->second.position.y + std::sin(pitchRad);
    it->second.target.z = it->second.position.z + std::cos(pitchRad) * std::sin(yawRad);
}

// ═══════════════════════════════════════════════════════════════
// الأشكال / Meshes
// ═══════════════════════════════════════════════════════════════

int mesh_create_cube_impl(float size) {
    int id = g_nextMeshId++;
    g_meshes[id] = {id, "cube", 12, 36};
    (void)size;
    return id;
}

int mesh_create_sphere_impl(float radius, int segments) {
    int id = g_nextMeshId++;
    int tris = segments * segments * 2;
    int verts = (segments + 1) * (segments + 1);
    g_meshes[id] = {id, "sphere", tris, verts};
    (void)radius;
    return id;
}

int mesh_create_cylinder_impl(float radius, float height, int segments) {
    int id = g_nextMeshId++;
    g_meshes[id] = {id, "cylinder", segments * 4, segments * 4 + 2};
    (void)radius; (void)height;
    return id;
}

int mesh_create_cone_impl(float radius, float height, int segments) {
    int id = g_nextMeshId++;
    g_meshes[id] = {id, "cone", segments * 2, segments + 2};
    (void)radius; (void)height;
    return id;
}

int mesh_create_plane_impl(float width, float depth) {
    int id = g_nextMeshId++;
    g_meshes[id] = {id, "plane", 2, 4};
    (void)width; (void)depth;
    return id;
}

void mesh_destroy_impl(int meshId) { g_meshes.erase(meshId); }

int mesh_triangle_count_impl(int meshId) {
    auto it = g_meshes.find(meshId);
    return it != g_meshes.end() ? it->second.triangleCount : 0;
}

int mesh_vertex_count_impl(int meshId) {
    auto it = g_meshes.find(meshId);
    return it != g_meshes.end() ? it->second.vertexCount : 0;
}

// ═══════════════════════════════════════════════════════════════
// المواد / Materials
// ═══════════════════════════════════════════════════════════════

int material_create_impl(const std::string& type) {
    int id = g_nextMatId++;
    g_materials[id] = {id, type, {1,1,1,1}, 0.5f, 0.0f};
    return id;
}

void material_set_color_impl(int matId, float r, float g, float b, float a) {
    auto it = g_materials.find(matId);
    if (it == g_materials.end()) return;
    it->second.color[0] = r; it->second.color[1] = g;
    it->second.color[2] = b; it->second.color[3] = a;
}

void material_set_roughness_impl(int matId, float roughness) {
    auto it = g_materials.find(matId);
    if (it == g_materials.end()) return;
    it->second.roughness = roughness;
}

void material_set_metallic_impl(int matId, float metallic) {
    auto it = g_materials.find(matId);
    if (it == g_materials.end()) return;
    it->second.metallic = metallic;
}

void material_destroy_impl(int matId) { g_materials.erase(matId); }

// ═══════════════════════════════════════════════════════════════
// الإضاءة / Lighting
// ═══════════════════════════════════════════════════════════════

int light_directional_create_impl(float dx, float dy, float dz, float r, float g, float b) {
    int id = g_nextLightId++;
    g_lights[id] = {id, "directional", {dx, dy, dz}, {r, g, b}, 0};
    return id;
}

int light_point_create_impl(float x, float y, float z, float r, float g, float b, float range) {
    int id = g_nextLightId++;
    g_lights[id] = {id, "point", {x, y, z}, {r, g, b}, range};
    return id;
}

void light_set_ambient_impl(float r, float g, float b) {
    g_ambientColor[0] = r; g_ambientColor[1] = g; g_ambientColor[2] = b;
}

void light_destroy_impl(int lightId) { g_lights.erase(lightId); }

// ═══════════════════════════════════════════════════════════════
// أطلس النسيج / Texture Atlas
// ═══════════════════════════════════════════════════════════════

int atlas_create_impl(const std::string& filepath) {
    int id = g_nextAtlasId++;
    g_atlases[id] = {id, filepath, {}, {}};
    return id;
}

void atlas_destroy_impl(int atlasId) { g_atlases.erase(atlasId); }

void atlas_add_region_impl(int atlasId, const std::string& name,
                           float x, float y, float width, float height) {
    auto it = g_atlases.find(atlasId);
    if (it == g_atlases.end()) return;
    it->second.regions[name] = {name, x, y, width, height};
}

void atlas_slice_grid_impl(int atlasId, const std::string& prefix, int cols, int rows) {
    auto it = g_atlases.find(atlasId);
    if (it == g_atlases.end()) return;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            std::string name = prefix + std::to_string(r * cols + c);
            float cellW = 1.0f / cols;
            float cellH = 1.0f / rows;
            it->second.regions[name] = {name, c * cellW, r * cellH, cellW, cellH};
        }
    }
}

bool atlas_has_region_impl(int atlasId, const std::string& name) {
    auto it = g_atlases.find(atlasId);
    if (it == g_atlases.end()) return false;
    return it->second.regions.count(name) > 0;
}

int atlas_region_count_impl(int atlasId) {
    auto it = g_atlases.find(atlasId);
    return it != g_atlases.end() ? static_cast<int>(it->second.regions.size()) : 0;
}

void atlas_define_anim_impl(int atlasId, const std::string& animName,
                            const std::string& prefix, int frameCount, float frameDuration) {
    auto it = g_atlases.find(atlasId);
    if (it == g_atlases.end()) return;
    it->second.animations[animName] = {animName, prefix, frameCount, frameDuration};
}

int atlas_anim_count_impl(int atlasId) {
    auto it = g_atlases.find(atlasId);
    return it != g_atlases.end() ? static_cast<int>(it->second.animations.size()) : 0;
}

} // namespace graphics
} // namespace stdlib
} // namespace sad
