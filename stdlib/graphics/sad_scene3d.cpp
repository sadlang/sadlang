// ═══════════════════════════════════════════════════════════════════════════════
// نظام المشاهد ثلاثي الأبعاد — التنفيذ
// 3D Scene System — Implementation
// ═══════════════════════════════════════════════════════════════════════════════
#include "sad_scene3d.h"
#include <SDL.h>
#include <algorithm>
#include <cmath>

namespace sad { namespace scene3d {

// ═══════════════════════════════════════════════════════════════════
// الحالة العامة
// ═══════════════════════════════════════════════════════════════════

static std::unordered_map<int, Object3D> g_objects;
static int g_nextId = 1;
static Camera g_camera;
static Light g_light;
static uint8_t g_bgR = 20, g_bgG = 20, g_bgB = 30;

static const float PI = 3.14159265358979f;
static const float DEG2RAD = PI / 180.0f;

// ═══════════════════════════════════════════════════════════════════
// تنفيذ Mat4
// ═══════════════════════════════════════════════════════════════════

Mat4 Mat4::identity() {
    Mat4 r;
    r.m[0][0] = r.m[1][1] = r.m[2][2] = r.m[3][3] = 1;
    return r;
}

Mat4 Mat4::translation(float x, float y, float z) {
    Mat4 r = identity();
    r.m[0][3] = x; r.m[1][3] = y; r.m[2][3] = z;
    return r;
}

Mat4 Mat4::scale(float sx, float sy, float sz) {
    Mat4 r = identity();
    r.m[0][0] = sx; r.m[1][1] = sy; r.m[2][2] = sz;
    return r;
}

Mat4 Mat4::rotationX(float angle) {
    Mat4 r = identity();
    float c = cosf(angle), s = sinf(angle);
    r.m[1][1] = c; r.m[1][2] = -s;
    r.m[2][1] = s; r.m[2][2] = c;
    return r;
}

Mat4 Mat4::rotationY(float angle) {
    Mat4 r = identity();
    float c = cosf(angle), s = sinf(angle);
    r.m[0][0] = c;  r.m[0][2] = s;
    r.m[2][0] = -s; r.m[2][2] = c;
    return r;
}

Mat4 Mat4::rotationZ(float angle) {
    Mat4 r = identity();
    float c = cosf(angle), s = sinf(angle);
    r.m[0][0] = c; r.m[0][1] = -s;
    r.m[1][0] = s; r.m[1][1] = c;
    return r;
}

Mat4 Mat4::perspective(float fov, float aspect, float nearP, float farP) {
    Mat4 r;
    float tanHalf = tanf(fov * DEG2RAD / 2.0f);
    r.m[0][0] = 1.0f / (aspect * tanHalf);
    r.m[1][1] = 1.0f / tanHalf;
    r.m[2][2] = -(farP + nearP) / (farP - nearP);
    r.m[2][3] = -(2.0f * farP * nearP) / (farP - nearP);
    r.m[3][2] = -1.0f;
    return r;
}

Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
    Vec3 f = (target - eye).normalized();
    Vec3 r = f.cross(up).normalized();
    Vec3 u = r.cross(f);
    
    Mat4 mat = identity();
    mat.m[0][0] = r.x;  mat.m[0][1] = r.y;  mat.m[0][2] = r.z;
    mat.m[1][0] = u.x;  mat.m[1][1] = u.y;  mat.m[1][2] = u.z;
    mat.m[2][0] = -f.x; mat.m[2][1] = -f.y; mat.m[2][2] = -f.z;
    mat.m[0][3] = -(r.x * eye.x + r.y * eye.y + r.z * eye.z);
    mat.m[1][3] = -(u.x * eye.x + u.y * eye.y + u.z * eye.z);
    mat.m[2][3] = (f.x * eye.x + f.y * eye.y + f.z * eye.z);
    return mat;
}

Mat4 Mat4::operator*(const Mat4& o) const {
    Mat4 r;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++) {
            r.m[i][j] = 0;
            for (int k = 0; k < 4; k++)
                r.m[i][j] += m[i][k] * o.m[k][j];
        }
    return r;
}

Vec3 Mat4::transform(const Vec3& v) const {
    return {
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3],
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]
    };
}

Vec3 Mat4::transformPoint(const Vec3& v) const {
    float w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3];
    if (std::abs(w) < 0.0001f) w = 0.0001f;
    return {
        (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) / w,
        (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) / w,
        (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) / w
    };
}

// ═══════════════════════════════════════════════════════════════════
// مساعدات داخلية
// ═══════════════════════════════════════════════════════════════════

// تحويل نقطة ثلاثية الأبعاد إلى إحداثيات شاشة
struct ScreenPoint {
    int x, y;
    float depth;
    bool visible;
};

static ScreenPoint projectPoint(const Vec3& p, const Mat4& viewProj, int scrW, int scrH) {
    Vec3 clip = viewProj.transformPoint(p);
    ScreenPoint sp;
    sp.visible = (clip.z > -1.0f && clip.z < 1.0f);
    sp.x = (int)((clip.x + 1.0f) * 0.5f * scrW);
    sp.y = (int)((1.0f - clip.y) * 0.5f * scrH); // عكس Y
    sp.depth = clip.z;
    return sp;
}

// حساب الإضاءة على مثلث
static float calculateLighting(const Vec3& normal) {
    Vec3 lightDir = g_light.direction.normalized();
    float diffuse = std::max(0.0f, -(normal.dot(lightDir)));
    return g_light.ambient + g_light.diffuse * diffuse;
}

// بناء مصفوفة النموذج (Model matrix) لكائن
static Mat4 buildModelMatrix(const Object3D& obj) {
    Mat4 t = Mat4::translation(obj.position.x, obj.position.y, obj.position.z);
    Mat4 rx = Mat4::rotationX(obj.rotation.x * DEG2RAD);
    Mat4 ry = Mat4::rotationY(obj.rotation.y * DEG2RAD);
    Mat4 rz = Mat4::rotationZ(obj.rotation.z * DEG2RAD);
    Mat4 s = Mat4::scale(obj.scaleVec.x, obj.scaleVec.y, obj.scaleVec.z);
    return t * ry * rx * rz * s;
}

// إنشاء مثلثات المكعب
static std::vector<Triangle3D> generateCube(const Object3D& obj, const Mat4& mvp, const Mat4& model) {
    float s = obj.size / 2.0f;
    // 8 رؤوس للمكعب
    Vec3 verts[8] = {
        {-s, -s, -s}, {s, -s, -s}, {s, s, -s}, {-s, s, -s},
        {-s, -s,  s}, {s, -s,  s}, {s, s,  s}, {-s, s,  s}
    };
    
    // 12 مثلث (6 أوجه × 2 مثلث لكل وجه)
    int faces[12][3] = {
        {0,1,2}, {0,2,3},  // أمام
        {5,4,7}, {5,7,6},  // خلف
        {4,0,3}, {4,3,7},  // يسار
        {1,5,6}, {1,6,2},  // يمين
        {3,2,6}, {3,6,7},  // أعلى
        {4,5,1}, {4,1,0}   // أسفل
    };
    
    Vec3 normals[6] = {
        {0,0,-1}, {0,0,1}, {-1,0,0}, {1,0,0}, {0,1,0}, {0,-1,0}
    };
    
    std::vector<Triangle3D> tris;
    for (int i = 0; i < 12; i++) {
        Triangle3D tri;
        for (int j = 0; j < 3; j++) {
            tri.v[j] = model.transform(verts[faces[i][j]]);
        }
        tri.normal = model.transform(normals[i / 2]).normalized();
        
        float light = calculateLighting(tri.normal);
        tri.r = (uint8_t)std::min(255.0f, obj.r * light);
        tri.g = (uint8_t)std::min(255.0f, obj.g * light);
        tri.b = (uint8_t)std::min(255.0f, obj.b * light);
        tri.a = obj.a;
        tri.depth = (tri.v[0].z + tri.v[1].z + tri.v[2].z) / 3.0f;
        tris.push_back(tri);
    }
    return tris;
}

// إنشاء مثلثات الكرة (UV sphere)
static std::vector<Triangle3D> generateSphere(const Object3D& obj, const Mat4& model) {
    std::vector<Triangle3D> tris;
    int seg = obj.segments;
    int rings = seg / 2;
    float r = obj.radius;
    
    for (int i = 0; i < rings; i++) {
        float phi1 = PI * (float)i / rings - PI / 2;
        float phi2 = PI * (float)(i + 1) / rings - PI / 2;
        
        for (int j = 0; j < seg; j++) {
            float theta1 = 2.0f * PI * (float)j / seg;
            float theta2 = 2.0f * PI * (float)(j + 1) / seg;
            
            Vec3 p1 = {r * cosf(phi1) * cosf(theta1), r * sinf(phi1), r * cosf(phi1) * sinf(theta1)};
            Vec3 p2 = {r * cosf(phi1) * cosf(theta2), r * sinf(phi1), r * cosf(phi1) * sinf(theta2)};
            Vec3 p3 = {r * cosf(phi2) * cosf(theta2), r * sinf(phi2), r * cosf(phi2) * sinf(theta2)};
            Vec3 p4 = {r * cosf(phi2) * cosf(theta1), r * sinf(phi2), r * cosf(phi2) * sinf(theta1)};
            
            // مثلث 1
            {
                Triangle3D tri;
                tri.v[0] = model.transform(p1);
                tri.v[1] = model.transform(p2);
                tri.v[2] = model.transform(p3);
                Vec3 edge1 = tri.v[1] - tri.v[0];
                Vec3 edge2 = tri.v[2] - tri.v[0];
                tri.normal = edge1.cross(edge2).normalized();
                float light = calculateLighting(tri.normal);
                tri.r = (uint8_t)std::min(255.0f, obj.r * light);
                tri.g = (uint8_t)std::min(255.0f, obj.g * light);
                tri.b = (uint8_t)std::min(255.0f, obj.b * light);
                tri.a = obj.a;
                tri.depth = (tri.v[0].z + tri.v[1].z + tri.v[2].z) / 3.0f;
                tris.push_back(tri);
            }
            // مثلث 2
            {
                Triangle3D tri;
                tri.v[0] = model.transform(p1);
                tri.v[1] = model.transform(p3);
                tri.v[2] = model.transform(p4);
                Vec3 edge1 = tri.v[1] - tri.v[0];
                Vec3 edge2 = tri.v[2] - tri.v[0];
                tri.normal = edge1.cross(edge2).normalized();
                float light = calculateLighting(tri.normal);
                tri.r = (uint8_t)std::min(255.0f, obj.r * light);
                tri.g = (uint8_t)std::min(255.0f, obj.g * light);
                tri.b = (uint8_t)std::min(255.0f, obj.b * light);
                tri.a = obj.a;
                tri.depth = (tri.v[0].z + tri.v[1].z + tri.v[2].z) / 3.0f;
                tris.push_back(tri);
            }
        }
    }
    return tris;
}

// رسم مثلث على الشاشة (SDL2 rasterizer بسيط)
static void renderTriangle(SDL_Renderer* rend, const Triangle3D& tri,
                            const Mat4& viewProj, int scrW, int scrH, RenderMode mode) {
    ScreenPoint sp[3];
    for (int i = 0; i < 3; i++) {
        sp[i] = projectPoint(tri.v[i], viewProj, scrW, scrH);
    }
    
    // Backface culling — تجاهل الأوجه المعاكسة
    int cross2d = (sp[1].x - sp[0].x) * (sp[2].y - sp[0].y) 
                - (sp[1].y - sp[0].y) * (sp[2].x - sp[0].x);
    if (cross2d < 0 && mode == RenderMode::Filled) return;
    
    SDL_SetRenderDrawColor(rend, tri.r, tri.g, tri.b, tri.a);
    
    if (mode == RenderMode::Wireframe || mode == RenderMode::Points) {
        // رسم شبكي
        SDL_RenderDrawLine(rend, sp[0].x, sp[0].y, sp[1].x, sp[1].y);
        SDL_RenderDrawLine(rend, sp[1].x, sp[1].y, sp[2].x, sp[2].y);
        SDL_RenderDrawLine(rend, sp[2].x, sp[2].y, sp[0].x, sp[0].y);
        return;
    }
    
    // ملء المثلث بخوارزمية scanline بسيطة
    // ترتيب الرؤوس حسب Y
    int idx[3] = {0, 1, 2};
    if (sp[idx[0]].y > sp[idx[1]].y) std::swap(idx[0], idx[1]);
    if (sp[idx[0]].y > sp[idx[2]].y) std::swap(idx[0], idx[2]);
    if (sp[idx[1]].y > sp[idx[2]].y) std::swap(idx[1], idx[2]);
    
    auto& top = sp[idx[0]];
    auto& mid = sp[idx[1]];
    auto& bot = sp[idx[2]];
    
    if (top.y == bot.y) return; // مثلث أفقي — لا يُملأ
    
    for (int y = std::max(0, top.y); y <= std::min(scrH - 1, bot.y); y++) {
        int x1, x2;
        
        if (y < mid.y) {
            // النصف العلوي
            if (mid.y == top.y) continue;
            float t1 = (float)(y - top.y) / (mid.y - top.y);
            float t2 = (float)(y - top.y) / (bot.y - top.y);
            x1 = (int)(top.x + (mid.x - top.x) * t1);
            x2 = (int)(top.x + (bot.x - top.x) * t2);
        } else {
            // النصف السفلي
            if (bot.y == mid.y) continue;
            float t1 = (float)(y - mid.y) / (bot.y - mid.y);
            float t2 = (float)(y - top.y) / (bot.y - top.y);
            x1 = (int)(mid.x + (bot.x - mid.x) * t1);
            x2 = (int)(top.x + (bot.x - top.x) * t2);
        }
        
        if (x1 > x2) std::swap(x1, x2);
        x1 = std::max(0, x1);
        x2 = std::min(scrW - 1, x2);
        
        SDL_RenderDrawLine(rend, x1, y, x2, y);
    }
}

// ═══════════════════════════════════════════════════════════════════
// الواجهة العامة — إنشاء الكائنات
// ═══════════════════════════════════════════════════════════════════

int createCube(float x, float y, float z, float size) {
    int id = g_nextId++;
    Object3D& o = g_objects[id];
    o.id = id;
    o.type = ObjectType::Cube;
    o.position = {x, y, z};
    o.size = size;
    return id;
}

int createSphere(float x, float y, float z, float radius, int segments) {
    int id = g_nextId++;
    Object3D& o = g_objects[id];
    o.id = id;
    o.type = ObjectType::Sphere;
    o.position = {x, y, z};
    o.radius = radius;
    o.segments = segments;
    return id;
}

int createCone(float x, float y, float z, float radius, float height) {
    int id = g_nextId++;
    Object3D& o = g_objects[id];
    o.id = id;
    o.type = ObjectType::Cone;
    o.position = {x, y, z};
    o.radius = radius;
    o.height = height;
    return id;
}

int createCylinder(float x, float y, float z, float radius, float height) {
    int id = g_nextId++;
    Object3D& o = g_objects[id];
    o.id = id;
    o.type = ObjectType::Cylinder;
    o.position = {x, y, z};
    o.radius = radius;
    o.height = height;
    return id;
}

int createPlane(float x, float y, float z, float size) {
    int id = g_nextId++;
    Object3D& o = g_objects[id];
    o.id = id;
    o.type = ObjectType::Plane;
    o.position = {x, y, z};
    o.size = size;
    o.r = 80; o.g = 80; o.b = 80;
    return id;
}

int createLine3D(float x1, float y1, float z1, float x2, float y2, float z2) {
    int id = g_nextId++;
    Object3D& o = g_objects[id];
    o.id = id;
    o.type = ObjectType::Line3D;
    o.position = {x1, y1, z1};
    // نستخدم scaleVec لتخزين النقطة الثانية
    o.scaleVec = {x2, y2, z2};
    o.r = 255; o.g = 255; o.b = 255;
    return id;
}

int createPoint3D(float x, float y, float z, float size) {
    int id = g_nextId++;
    Object3D& o = g_objects[id];
    o.id = id;
    o.type = ObjectType::Point3D;
    o.position = {x, y, z};
    o.size = size;
    return id;
}

void destroyObject(int id) { g_objects.erase(id); }
void destroyAllObjects() { g_objects.clear(); }

// ─── تحويلات الكائن ───
void setObjectPosition(int id, float x, float y, float z) {
    if (g_objects.count(id)) g_objects[id].position = {x, y, z};
}
void setObjectRotation(int id, float rx, float ry, float rz) {
    if (g_objects.count(id)) g_objects[id].rotation = {rx, ry, rz};
}
void setObjectScale(int id, float sx, float sy, float sz) {
    if (g_objects.count(id)) g_objects[id].scaleVec = {sx, sy, sz};
}
void setObjectColor(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (!g_objects.count(id)) return;
    g_objects[id].r = r; g_objects[id].g = g;
    g_objects[id].b = b; g_objects[id].a = a;
}
void setObjectMode(int id, const std::string& mode) {
    if (!g_objects.count(id)) return;
    if (mode == "wireframe" || mode == "\xd8\xb4\xd8\xa8\xd9\x83\xd9\x8a")
        g_objects[id].mode = RenderMode::Wireframe;
    else if (mode == "points" || mode == "\xd9\x86\xd9\x82\xd8\xa7\xd8\xb7")
        g_objects[id].mode = RenderMode::Points;
    else
        g_objects[id].mode = RenderMode::Filled;
}
void setObjectVisible(int id, bool visible) {
    if (g_objects.count(id)) g_objects[id].visible = visible;
}

void rotateObject(int id, float dx, float dy, float dz) {
    if (!g_objects.count(id)) return;
    g_objects[id].rotation.x += dx;
    g_objects[id].rotation.y += dy;
    g_objects[id].rotation.z += dz;
}
void moveObject(int id, float dx, float dy, float dz) {
    if (!g_objects.count(id)) return;
    g_objects[id].position.x += dx;
    g_objects[id].position.y += dy;
    g_objects[id].position.z += dz;
}

// ─── الكاميرا ───
void setCameraPosition(float x, float y, float z) { g_camera.position = {x, y, z}; }
void setCameraTarget(float x, float y, float z) { g_camera.target = {x, y, z}; }
void setCameraFOV(float fov) { g_camera.fov = fov; }

void orbitCamera(float angleH, float angleV, float distance) {
    float h = angleH * DEG2RAD;
    float v = angleV * DEG2RAD;
    g_camera.position = {
        g_camera.target.x + distance * cosf(v) * sinf(h),
        g_camera.target.y + distance * sinf(v),
        g_camera.target.z + distance * cosf(v) * cosf(h)
    };
}

void moveCameraForward(float amount) {
    Vec3 dir = (g_camera.target - g_camera.position).normalized();
    g_camera.position = g_camera.position + dir * amount;
    g_camera.target = g_camera.target + dir * amount;
}

void moveCameraRight(float amount) {
    Vec3 dir = (g_camera.target - g_camera.position).normalized();
    Vec3 right = dir.cross(g_camera.up).normalized();
    g_camera.position = g_camera.position + right * amount;
    g_camera.target = g_camera.target + right * amount;
}

void moveCameraUp(float amount) {
    g_camera.position.y += amount;
    g_camera.target.y += amount;
}

// ─── الإضاءة ───
void setLightDirection(float x, float y, float z) { g_light.direction = {x, y, z}; }
void setLightColor(uint8_t r, uint8_t g, uint8_t b) { g_light.r = r; g_light.g = g; g_light.b = b; }
void setAmbientLight(float amount) { g_light.ambient = amount; }
void setDiffuseLight(float amount) { g_light.diffuse = amount; }

// ═══════════════════════════════════════════════════════════════════
// الرسم الرئيسي
// ═══════════════════════════════════════════════════════════════════

void render(void* renderer, int screenWidth, int screenHeight) {
    if (!renderer) return;
    SDL_Renderer* rend = (SDL_Renderer*)renderer;
    
    float aspect = (float)screenWidth / (float)screenHeight;
    Mat4 view = Mat4::lookAt(g_camera.position, g_camera.target, g_camera.up);
    Mat4 proj = Mat4::perspective(g_camera.fov, aspect, g_camera.nearPlane, g_camera.farPlane);
    Mat4 viewProj = proj * view;
    
    // جمع كل المثلثات
    std::vector<Triangle3D> allTris;
    
    for (auto& [id, obj] : g_objects) {
        if (!obj.visible) continue;
        
        Mat4 model = buildModelMatrix(obj);
        Mat4 mvp = viewProj * model;
        
        if (obj.type == ObjectType::Cube) {
            auto tris = generateCube(obj, mvp, model);
            allTris.insert(allTris.end(), tris.begin(), tris.end());
        }
        else if (obj.type == ObjectType::Sphere) {
            auto tris = generateSphere(obj, model);
            allTris.insert(allTris.end(), tris.begin(), tris.end());
        }
        else if (obj.type == ObjectType::Plane) {
            // مستوى — مربع كبير
            float s = obj.size / 2;
            Triangle3D t1, t2;
            t1.v[0] = model.transform({-s, 0, -s});
            t1.v[1] = model.transform({s, 0, -s});
            t1.v[2] = model.transform({s, 0, s});
            t2.v[0] = model.transform({-s, 0, -s});
            t2.v[1] = model.transform({s, 0, s});
            t2.v[2] = model.transform({-s, 0, s});
            t1.normal = t2.normal = model.transform({0, 1, 0}).normalized();
            float light = calculateLighting(t1.normal);
            t1.r = t2.r = (uint8_t)std::min(255.0f, obj.r * light);
            t1.g = t2.g = (uint8_t)std::min(255.0f, obj.g * light);
            t1.b = t2.b = (uint8_t)std::min(255.0f, obj.b * light);
            t1.a = t2.a = obj.a;
            t1.depth = (t1.v[0].z + t1.v[1].z + t1.v[2].z) / 3.0f;
            t2.depth = (t2.v[0].z + t2.v[1].z + t2.v[2].z) / 3.0f;
            allTris.push_back(t1);
            allTris.push_back(t2);
        }
        else if (obj.type == ObjectType::Line3D) {
            // الخط يُرسم مباشرة
            ScreenPoint p1 = projectPoint(obj.position, viewProj, screenWidth, screenHeight);
            ScreenPoint p2 = projectPoint(obj.scaleVec, viewProj, screenWidth, screenHeight);
            SDL_SetRenderDrawColor(rend, obj.r, obj.g, obj.b, obj.a);
            SDL_RenderDrawLine(rend, p1.x, p1.y, p2.x, p2.y);
        }
        else if (obj.type == ObjectType::Point3D) {
            ScreenPoint p = projectPoint(model.transform({0, 0, 0}), viewProj, screenWidth, screenHeight);
            SDL_SetRenderDrawColor(rend, obj.r, obj.g, obj.b, obj.a);
            int sz = (int)obj.size;
            SDL_Rect rect = {p.x - sz / 2, p.y - sz / 2, sz, sz};
            SDL_RenderFillRect(rend, &rect);
        }
        else if (obj.type == ObjectType::Cone) {
            // مخروط مبسط — أقسام مثلثية من القمة إلى القاعدة الدائرية
            int seg = obj.segments;
            Vec3 apex = {0, obj.height / 2, 0};
            for (int i = 0; i < seg; i++) {
                float a1 = 2.0f * PI * i / seg;
                float a2 = 2.0f * PI * (i + 1) / seg;
                Vec3 b1 = {obj.radius * cosf(a1), -obj.height / 2, obj.radius * sinf(a1)};
                Vec3 b2 = {obj.radius * cosf(a2), -obj.height / 2, obj.radius * sinf(a2)};
                
                Triangle3D tri;
                tri.v[0] = model.transform(apex);
                tri.v[1] = model.transform(b1);
                tri.v[2] = model.transform(b2);
                Vec3 e1 = tri.v[1] - tri.v[0];
                Vec3 e2 = tri.v[2] - tri.v[0];
                tri.normal = e1.cross(e2).normalized();
                float light = calculateLighting(tri.normal);
                tri.r = (uint8_t)std::min(255.0f, obj.r * light);
                tri.g = (uint8_t)std::min(255.0f, obj.g * light);
                tri.b = (uint8_t)std::min(255.0f, obj.b * light);
                tri.a = obj.a;
                tri.depth = (tri.v[0].z + tri.v[1].z + tri.v[2].z) / 3.0f;
                allTris.push_back(tri);
            }
        }
        else if (obj.type == ObjectType::Cylinder) {
            // أسطوانة — أقسام مستطيلية (مثلثين لكل قسم)
            int seg = obj.segments;
            for (int i = 0; i < seg; i++) {
                float a1 = 2.0f * PI * i / seg;
                float a2 = 2.0f * PI * (i + 1) / seg;
                Vec3 t1 = {obj.radius * cosf(a1), obj.height / 2, obj.radius * sinf(a1)};
                Vec3 t2 = {obj.radius * cosf(a2), obj.height / 2, obj.radius * sinf(a2)};
                Vec3 b1 = {obj.radius * cosf(a1), -obj.height / 2, obj.radius * sinf(a1)};
                Vec3 b2 = {obj.radius * cosf(a2), -obj.height / 2, obj.radius * sinf(a2)};
                
                // مثلث 1
                Triangle3D tri1;
                tri1.v[0] = model.transform(t1);
                tri1.v[1] = model.transform(b1);
                tri1.v[2] = model.transform(b2);
                Vec3 e1 = tri1.v[1] - tri1.v[0];
                Vec3 e2 = tri1.v[2] - tri1.v[0];
                tri1.normal = e1.cross(e2).normalized();
                float light = calculateLighting(tri1.normal);
                tri1.r = (uint8_t)std::min(255.0f, obj.r * light);
                tri1.g = (uint8_t)std::min(255.0f, obj.g * light);
                tri1.b = (uint8_t)std::min(255.0f, obj.b * light);
                tri1.a = obj.a;
                tri1.depth = (tri1.v[0].z + tri1.v[1].z + tri1.v[2].z) / 3.0f;
                allTris.push_back(tri1);
                
                // مثلث 2
                Triangle3D tri2;
                tri2.v[0] = model.transform(t1);
                tri2.v[1] = model.transform(b2);
                tri2.v[2] = model.transform(t2);
                e1 = tri2.v[1] - tri2.v[0];
                e2 = tri2.v[2] - tri2.v[0];
                tri2.normal = e1.cross(e2).normalized();
                light = calculateLighting(tri2.normal);
                tri2.r = (uint8_t)std::min(255.0f, obj.r * light);
                tri2.g = (uint8_t)std::min(255.0f, obj.g * light);
                tri2.b = (uint8_t)std::min(255.0f, obj.b * light);
                tri2.a = obj.a;
                tri2.depth = (tri2.v[0].z + tri2.v[1].z + tri2.v[2].z) / 3.0f;
                allTris.push_back(tri2);
            }
        }
    }
    
    // ─── ترتيب العمق (الأبعد أولاً — Painter's algorithm) ───
    std::sort(allTris.begin(), allTris.end(), [](const Triangle3D& a, const Triangle3D& b) {
        return a.depth > b.depth;
    });
    
    // ─── رسم المثلثات ───
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    for (auto& tri : allTris) {
        // تحديد وضع الرسم — الافتراضي ملء
        RenderMode mode = RenderMode::Filled;
        renderTriangle(rend, tri, viewProj, screenWidth, screenHeight, mode);
    }
}

// ═══════════════════════════════════════════════════════════════════
// أدوات
// ═══════════════════════════════════════════════════════════════════

void setBackgroundColor(uint8_t r, uint8_t g, uint8_t b) {
    g_bgR = r; g_bgG = g; g_bgB = b;
}

int getObjectCount() { return (int)g_objects.size(); }

void resetScene() {
    g_objects.clear();
    g_nextId = 1;
    g_camera = Camera();
    g_light = Light();
    g_bgR = 20; g_bgG = 20; g_bgB = 30;
}

// ─── أدوات رسم مباشرة ───
void drawLine3D(void* renderer, int scrW, int scrH,
                float x1, float y1, float z1,
                float x2, float y2, float z2,
                uint8_t r, uint8_t g, uint8_t b) {
    if (!renderer) return;
    SDL_Renderer* rend = (SDL_Renderer*)renderer;
    
    float aspect = (float)scrW / (float)scrH;
    Mat4 view = Mat4::lookAt(g_camera.position, g_camera.target, g_camera.up);
    Mat4 proj = Mat4::perspective(g_camera.fov, aspect, g_camera.nearPlane, g_camera.farPlane);
    Mat4 vp = proj * view;
    
    ScreenPoint p1 = projectPoint({x1, y1, z1}, vp, scrW, scrH);
    ScreenPoint p2 = projectPoint({x2, y2, z2}, vp, scrW, scrH);
    
    SDL_SetRenderDrawColor(rend, r, g, b, 255);
    SDL_RenderDrawLine(rend, p1.x, p1.y, p2.x, p2.y);
}

void drawGrid(void* renderer, int scrW, int scrH, float size, int divisions) {
    if (!renderer) return;
    float step = size / divisions;
    float half = size / 2;
    
    for (int i = 0; i <= divisions; i++) {
        float pos = -half + i * step;
        // خطوط على المحور Z
        drawLine3D(renderer, scrW, scrH, pos, 0, -half, pos, 0, half, 60, 60, 60);
        // خطوط على المحور X
        drawLine3D(renderer, scrW, scrH, -half, 0, pos, half, 0, pos, 60, 60, 60);
    }
}

void drawAxes(void* renderer, int scrW, int scrH, float length) {
    if (!renderer) return;
    // المحور X — أحمر
    drawLine3D(renderer, scrW, scrH, 0, 0, 0, length, 0, 0, 255, 50, 50);
    // المحور Y — أخضر
    drawLine3D(renderer, scrW, scrH, 0, 0, 0, 0, length, 0, 50, 255, 50);
    // المحور Z — أزرق
    drawLine3D(renderer, scrW, scrH, 0, 0, 0, 0, 0, length, 50, 50, 255);
}

}} // namespace sad::scene3d
