// ═══════════════════════════════════════════════════════════════════════════════
// sad_renderer3d_atlas.h — ربط التصيير ثلاثي الأبعاد وأطلس النسيج بلغة ص
// 3D Renderer & Texture Atlas Binding for SAD Language
// ═══════════════════════════════════════════════════════════════════════════════
#pragma once

#include <string>
#include <vector>

namespace sad {
namespace stdlib {
namespace graphics {

// ═══════════════════════════════════════════════════════════════
// التصيير ثلاثي الأبعاد / 3D Rendering
// ═══════════════════════════════════════════════════════════════

/// إنشاء كاميرا ثلاثية الأبعاد / Create 3D camera
int camera3d_create_impl(float fov, float nearPlane, float farPlane);

/// تدمير كاميرا / Destroy camera
void camera3d_destroy_impl(int camId);

/// تعيين موقع الكاميرا / Set camera position
void camera3d_set_position_impl(int camId, float x, float y, float z);

/// تعيين هدف الكاميرا / Set camera target
void camera3d_set_target_impl(int camId, float x, float y, float z);

/// تحريك الكاميرا للأمام / Move camera forward
void camera3d_move_forward_impl(int camId, float amount);

/// تحريك الكاميرا للجانب / Strafe camera
void camera3d_strafe_impl(int camId, float amount);

/// تدوير الكاميرا / Rotate camera (yaw, pitch)
void camera3d_rotate_impl(int camId, float yaw, float pitch);

/// إنشاء شبكة مكعب / Create cube mesh
int mesh_create_cube_impl(float size);

/// إنشاء شبكة كرة / Create sphere mesh
int mesh_create_sphere_impl(float radius, int segments);

/// إنشاء شبكة أسطوانة / Create cylinder mesh
int mesh_create_cylinder_impl(float radius, float height, int segments);

/// إنشاء شبكة مخروط / Create cone mesh
int mesh_create_cone_impl(float radius, float height, int segments);

/// إنشاء شبكة مستوى / Create plane mesh
int mesh_create_plane_impl(float width, float depth);

/// تدمير شبكة / Destroy mesh
void mesh_destroy_impl(int meshId);

/// الحصول على عدد المثلثات / Get triangle count
int mesh_triangle_count_impl(int meshId);

/// الحصول على عدد الرؤوس / Get vertex count
int mesh_vertex_count_impl(int meshId);

/// إنشاء مادة / Create material
int material_create_impl(const std::string& type);

/// تعيين لون المادة / Set material color
void material_set_color_impl(int matId, float r, float g, float b, float a);

/// تعيين خشونة المادة / Set material roughness
void material_set_roughness_impl(int matId, float roughness);

/// تعيين انعكاسية المادة / Set material metallic
void material_set_metallic_impl(int matId, float metallic);

/// تدمير مادة / Destroy material
void material_destroy_impl(int matId);

/// إنشاء ضوء اتجاهي / Create directional light
int light_directional_create_impl(float dx, float dy, float dz, float r, float g, float b);

/// إنشاء ضوء نقطي / Create point light
int light_point_create_impl(float x, float y, float z, float r, float g, float b, float range);

/// تعيين لون الإضاءة المحيطة / Set ambient color
void light_set_ambient_impl(float r, float g, float b);

/// تدمير ضوء / Destroy light
void light_destroy_impl(int lightId);

// ═══════════════════════════════════════════════════════════════
// أطلس النسيج / Texture Atlas
// ═══════════════════════════════════════════════════════════════

/// إنشاء أطلس من ملف / Create atlas from file
int atlas_create_impl(const std::string& filepath);

/// تدمير أطلس / Destroy atlas
void atlas_destroy_impl(int atlasId);

/// إضافة منطقة / Add region
void atlas_add_region_impl(int atlasId, const std::string& name,
                           float x, float y, float width, float height);

/// تقسيم شبكي / Slice into grid
void atlas_slice_grid_impl(int atlasId, const std::string& prefix, int cols, int rows);

/// التحقق من وجود منطقة / Check if region exists
bool atlas_has_region_impl(int atlasId, const std::string& name);

/// عدد المناطق / Region count
int atlas_region_count_impl(int atlasId);

/// تعريف رسم متحرك / Define animation from sequential regions
void atlas_define_anim_impl(int atlasId, const std::string& animName,
                            const std::string& prefix, int frameCount, float frameDuration);

/// عدد الرسوم المتحركة / Animation count
int atlas_anim_count_impl(int atlasId);

} // namespace graphics
} // namespace stdlib
} // namespace sad
