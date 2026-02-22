// ============================================================================
// mesh.h - نظام الشبكات ثلاثية الأبعاد
// 3D Mesh System
// ============================================================================
// الوصف: يحتوي على هياكل وفئات لإدارة الشبكات ثلاثية الأبعاد
// Description: Contains structures and classes for managing 3D meshes
// ============================================================================

#ifndef SAD_GRAPHICS_MESH_H
#define SAD_GRAPHICS_MESH_H

#include "../core/types.h"
#include "../core/math3d.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>

namespace sad {
namespace graphics {

// ============================================================================
// رأس الشبكة / Mesh Vertex
// ============================================================================

/// رأس ثلاثي الأبعاد كامل / Full 3D vertex
struct Vertex3D {
    Vec3 position;    ///< الموقع / Position
    Vec3 normal;      ///< المتجه العمودي / Normal vector
    Vec2 texCoord;    ///< إحداثيات النسيج / Texture coordinates
    Vec4 tangent;     ///< الظل (xyz) وعلامة البتانج (w) / Tangent (xyz) and bitangent sign (w)
    Color color;      ///< لون الرأس / Vertex color
    
    /// المنشئ الافتراضي / Default constructor
    Vertex3D() 
        : position(Vec3::Zero()), normal(Vec3::UnitY()), texCoord(Vec2::Zero()),
          tangent(Vec4(1, 0, 0, 1)), color(Color::White) {}
    
    /// منشئ بموقع فقط / Constructor with position only
    explicit Vertex3D(const Vec3& pos)
        : position(pos), normal(Vec3::UnitY()), texCoord(Vec2::Zero()),
          tangent(Vec4(1, 0, 0, 1)), color(Color::White) {}
    
    /// منشئ بموقع وإحداثيات نسيج / Constructor with position and UV
    Vertex3D(const Vec3& pos, const Vec2& uv)
        : position(pos), normal(Vec3::UnitY()), texCoord(uv),
          tangent(Vec4(1, 0, 0, 1)), color(Color::White) {}
    
    /// منشئ بموقع وعمودي وإحداثيات نسيج / Constructor with position, normal, and UV
    Vertex3D(const Vec3& pos, const Vec3& norm, const Vec2& uv)
        : position(pos), normal(norm), texCoord(uv),
          tangent(Vec4(1, 0, 0, 1)), color(Color::White) {}
    
    /// منشئ كامل / Full constructor
    Vertex3D(const Vec3& pos, const Vec3& norm, const Vec2& uv, const Vec4& tan, const Color& col)
        : position(pos), normal(norm), texCoord(uv), tangent(tan), color(col) {}
    
    /// المقارنة / Comparison
    bool operator==(const Vertex3D& other) const {
        return position == other.position && normal == other.normal &&
               texCoord == other.texCoord && tangent == other.tangent;
    }
};

/// رأس بسيط للأداء / Simple vertex for performance
struct SimpleVertex3D {
    Vec3 position;  ///< الموقع / Position
    Vec3 normal;    ///< العمودي / Normal
    Vec2 texCoord;  ///< إحداثيات النسيج / Texture coordinates
    
    SimpleVertex3D() : position(Vec3::Zero()), normal(Vec3::UnitY()), texCoord(Vec2::Zero()) {}
    SimpleVertex3D(const Vec3& pos, const Vec3& norm, const Vec2& uv)
        : position(pos), normal(norm), texCoord(uv) {}
};

/// رأس مع عظام للرسوم المتحركة / Skinned vertex for animation
struct SkinnedVertex {
    Vec3 position;     ///< الموقع / Position
    Vec3 normal;       ///< العمودي / Normal
    Vec2 texCoord;     ///< إحداثيات النسيج / Texture coordinates
    Vec4 tangent;      ///< الظل / Tangent
    UInt32 boneIds[4]; ///< معرفات العظام / Bone IDs (up to 4 bones)
    Float32 weights[4]; ///< أوزان العظام / Bone weights
    
    SkinnedVertex() 
        : position(Vec3::Zero()), normal(Vec3::UnitY()), texCoord(Vec2::Zero()),
          tangent(Vec4(1, 0, 0, 1)) {
        for (int i = 0; i < 4; i++) {
            boneIds[i] = 0;
            weights[i] = 0.0f;
        }
    }
    
    /// إضافة تأثير عظم / Add bone influence
    void AddBoneInfluence(UInt32 boneId, Float32 weight) {
        // إيجاد أقل وزن واستبداله إذا كان الوزن الجديد أكبر
        // Find lowest weight and replace if new weight is greater
        int minIndex = 0;
        Float32 minWeight = weights[0];
        for (int i = 1; i < 4; i++) {
            if (weights[i] < minWeight) {
                minIndex = i;
                minWeight = weights[i];
            }
        }
        
        if (weight > minWeight) {
            boneIds[minIndex] = boneId;
            weights[minIndex] = weight;
        }
    }
    
    /// تطبيع الأوزان / Normalize weights
    void NormalizeWeights() {
        Float32 total = weights[0] + weights[1] + weights[2] + weights[3];
        if (total > 0.0f) {
            Float32 inv = 1.0f / total;
            for (int i = 0; i < 4; i++) {
                weights[i] *= inv;
            }
        }
    }
};

// ============================================================================
// البدائيات الهندسية / Geometric Primitives
// ============================================================================

/// نوع البدائية / Primitive type
enum class PrimitiveType {
    Points,         ///< نقاط / Points
    Lines,          ///< خطوط / Lines
    LineStrip,      ///< شريط خطوط / Line strip
    LineLoop,       ///< حلقة خطوط / Line loop
    Triangles,      ///< مثلثات / Triangles
    TriangleStrip,  ///< شريط مثلثات / Triangle strip
    TriangleFan,    ///< مروحة مثلثات / Triangle fan
    Patches         ///< رقع (للتقسيم) / Patches (for tessellation)
};

// ============================================================================
// بيانات الشبكة / Mesh Data
// ============================================================================

/// بيانات شبكة خام / Raw mesh data
struct MeshData {
    std::vector<Vertex3D> vertices;   ///< الرؤوس / Vertices
    std::vector<UInt32> indices;      ///< الفهارس / Indices
    PrimitiveType primitiveType;      ///< نوع البدائية / Primitive type
    AABB boundingBox;                 ///< صندوق الحدود / Bounding box
    Sphere boundingSphere;            ///< كرة الحدود / Bounding sphere
    
    MeshData() : primitiveType(PrimitiveType::Triangles) {}
    
    /// حساب حدود الشبكة / Calculate mesh bounds
    void CalculateBounds() {
        if (vertices.empty()) return;
        
        Vec3 minP = vertices[0].position;
        Vec3 maxP = vertices[0].position;
        
        for (const auto& v : vertices) {
            minP.x = Min(minP.x, v.position.x);
            minP.y = Min(minP.y, v.position.y);
            minP.z = Min(minP.z, v.position.z);
            maxP.x = Max(maxP.x, v.position.x);
            maxP.y = Max(maxP.y, v.position.y);
            maxP.z = Max(maxP.z, v.position.z);
        }
        
        boundingBox = AABB(minP, maxP);
        boundingSphere = Sphere(boundingBox.Center(), boundingBox.Extents().Length());
    }
    
    /// حساب العموديات / Calculate normals
    void CalculateNormals() {
        // إعادة تعيين العموديات / Reset normals
        for (auto& v : vertices) {
            v.normal = Vec3::Zero();
        }
        
        // تجميع عموديات المثلثات / Accumulate triangle normals
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            UInt32 i0 = indices[i];
            UInt32 i1 = indices[i + 1];
            UInt32 i2 = indices[i + 2];
            
            Vec3 v0 = vertices[i0].position;
            Vec3 v1 = vertices[i1].position;
            Vec3 v2 = vertices[i2].position;
            
            Vec3 normal = (v1 - v0).Cross(v2 - v0);
            
            vertices[i0].normal = vertices[i0].normal + normal;
            vertices[i1].normal = vertices[i1].normal + normal;
            vertices[i2].normal = vertices[i2].normal + normal;
        }
        
        // تطبيع العموديات / Normalize normals
        for (auto& v : vertices) {
            v.normal.Normalize();
        }
    }
    
    /// حساب الظلال / Calculate tangents
    void CalculateTangents() {
        std::vector<Vec3> tan1(vertices.size(), Vec3::Zero());
        std::vector<Vec3> tan2(vertices.size(), Vec3::Zero());
        
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            UInt32 i0 = indices[i];
            UInt32 i1 = indices[i + 1];
            UInt32 i2 = indices[i + 2];
            
            const Vec3& v0 = vertices[i0].position;
            const Vec3& v1 = vertices[i1].position;
            const Vec3& v2 = vertices[i2].position;
            
            const Vec2& uv0 = vertices[i0].texCoord;
            const Vec2& uv1 = vertices[i1].texCoord;
            const Vec2& uv2 = vertices[i2].texCoord;
            
            Vec3 edge1 = v1 - v0;
            Vec3 edge2 = v2 - v0;
            
            Vec2 deltaUV1 = uv1 - uv0;
            Vec2 deltaUV2 = uv2 - uv0;
            
            Float32 f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y + EPSILON);
            
            Vec3 tangent(
                f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
            );
            
            Vec3 bitangent(
                f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
                f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
                f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
            );
            
            tan1[i0] = tan1[i0] + tangent;
            tan1[i1] = tan1[i1] + tangent;
            tan1[i2] = tan1[i2] + tangent;
            
            tan2[i0] = tan2[i0] + bitangent;
            tan2[i1] = tan2[i1] + bitangent;
            tan2[i2] = tan2[i2] + bitangent;
        }
        
        for (size_t i = 0; i < vertices.size(); i++) {
            const Vec3& n = vertices[i].normal;
            const Vec3& t = tan1[i];
            
            // Gram-Schmidt orthogonalize
            Vec3 tangent = (t - n * n.Dot(t)).Normalized();
            
            // Calculate handedness
            Float32 w = (n.Cross(t).Dot(tan2[i]) < 0.0f) ? -1.0f : 1.0f;
            
            vertices[i].tangent = Vec4(tangent.x, tangent.y, tangent.z, w);
        }
    }
    
    /// عكس العموديات / Flip normals
    void FlipNormals() {
        for (auto& v : vertices) {
            v.normal = -v.normal;
        }
    }
    
    /// عكس ترتيب المثلثات / Flip triangle winding
    void FlipWinding() {
        for (size_t i = 0; i + 2 < indices.size(); i += 3) {
            std::swap(indices[i + 1], indices[i + 2]);
        }
    }
    
    /// تحويل الشبكة / Transform mesh
    void Transform(const Mat4& matrix) {
        Mat4 normalMatrix = matrix.Transposed().Inverted();
        
        for (auto& v : vertices) {
            v.position = matrix.TransformPoint(v.position);
            v.normal = normalMatrix.TransformDirection(v.normal).Normalized();
        }
        
        CalculateBounds();
    }
    
    /// دمج شبكة أخرى / Merge another mesh
    void Merge(const MeshData& other) {
        UInt32 indexOffset = static_cast<UInt32>(vertices.size());
        
        vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
        
        for (UInt32 idx : other.indices) {
            indices.push_back(idx + indexOffset);
        }
        
        CalculateBounds();
    }
    
    /// إزالة الرؤوس المكررة / Remove duplicate vertices
    void RemoveDuplicateVertices() {
        std::vector<Vertex3D> uniqueVertices;
        std::unordered_map<size_t, UInt32> vertexMap;
        std::vector<UInt32> newIndices;
        
        for (UInt32 idx : indices) {
            const Vertex3D& v = vertices[idx];
            
            // حساب hash بسيط / Simple hash calculation
            size_t hash = 0;
            hash ^= std::hash<Float32>{}(v.position.x) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<Float32>{}(v.position.y) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            hash ^= std::hash<Float32>{}(v.position.z) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            
            auto it = vertexMap.find(hash);
            if (it != vertexMap.end() && uniqueVertices[it->second] == v) {
                newIndices.push_back(it->second);
            } else {
                UInt32 newIndex = static_cast<UInt32>(uniqueVertices.size());
                uniqueVertices.push_back(v);
                vertexMap[hash] = newIndex;
                newIndices.push_back(newIndex);
            }
        }
        
        vertices = std::move(uniqueVertices);
        indices = std::move(newIndices);
    }
};

// ============================================================================
// جزء الشبكة / Mesh Submesh
// ============================================================================

/// جزء من الشبكة (لمواد متعددة) / Submesh (for multiple materials)
struct Submesh {
    UInt32 indexStart;     ///< بداية الفهارس / Index start
    UInt32 indexCount;     ///< عدد الفهارس / Index count
    UInt32 vertexStart;    ///< بداية الرؤوس / Vertex start
    UInt32 vertexCount;    ///< عدد الرؤوس / Vertex count
    UInt32 materialIndex;  ///< فهرس المادة / Material index
    AABB boundingBox;      ///< صندوق الحدود / Bounding box
    
    Submesh()
        : indexStart(0), indexCount(0), vertexStart(0), vertexCount(0), materialIndex(0) {}
    
    Submesh(UInt32 iStart, UInt32 iCount, UInt32 matIdx)
        : indexStart(iStart), indexCount(iCount), vertexStart(0), vertexCount(0), materialIndex(matIdx) {}
};

// ============================================================================
// الشبكة / Mesh Class
// ============================================================================

/// فئة الشبكة ثلاثية الأبعاد / 3D Mesh class
class Mesh {
public:
    /// المنشئ الافتراضي / Default constructor
    Mesh() : m_gpuHandle(0), m_isDirty(true) {}
    
    /// المنشئ من بيانات / Constructor from data
    explicit Mesh(const MeshData& data) : m_data(data), m_gpuHandle(0), m_isDirty(true) {
        if (m_submeshes.empty()) {
            m_submeshes.push_back(Submesh(0, static_cast<UInt32>(data.indices.size()), 0));
        }
    }
    
    /// الحصول على البيانات / Get data
    const MeshData& GetData() const { return m_data; }
    MeshData& GetData() { m_isDirty = true; return m_data; }
    
    /// تعيين البيانات / Set data
    void SetData(const MeshData& data) {
        m_data = data;
        m_isDirty = true;
    }
    
    /// الحصول على الأجزاء / Get submeshes
    const std::vector<Submesh>& GetSubmeshes() const { return m_submeshes; }
    
    /// إضافة جزء / Add submesh
    void AddSubmesh(const Submesh& submesh) { m_submeshes.push_back(submesh); }
    
    /// مسح الأجزاء / Clear submeshes
    void ClearSubmeshes() { m_submeshes.clear(); }
    
    /// عدد الرؤوس / Vertex count
    size_t GetVertexCount() const { return m_data.vertices.size(); }
    
    /// عدد الفهارس / Index count
    size_t GetIndexCount() const { return m_data.indices.size(); }
    
    /// عدد المثلثات / Triangle count
    size_t GetTriangleCount() const { return m_data.indices.size() / 3; }
    
    /// صندوق الحدود / Bounding box
    const AABB& GetBoundingBox() const { return m_data.boundingBox; }
    
    /// كرة الحدود / Bounding sphere
    const Sphere& GetBoundingSphere() const { return m_data.boundingSphere; }
    
    /// حساب الحدود / Calculate bounds
    void CalculateBounds() { m_data.CalculateBounds(); }
    
    /// حساب العموديات / Calculate normals
    void CalculateNormals() { m_data.CalculateNormals(); m_isDirty = true; }
    
    /// حساب الظلال / Calculate tangents
    void CalculateTangents() { m_data.CalculateTangents(); m_isDirty = true; }
    
    /// هل تم التعديل / Is dirty
    bool IsDirty() const { return m_isDirty; }
    
    /// تعيين حالة التعديل / Set dirty state
    void SetDirty(bool dirty) { m_isDirty = dirty; }
    
    /// مؤشر GPU / GPU handle (for renderer use)
    Handle GetGPUHandle() const { return m_gpuHandle; }
    void SetGPUHandle(Handle handle) { m_gpuHandle = handle; }
    
private:
    MeshData m_data;                    ///< بيانات الشبكة / Mesh data
    std::vector<Submesh> m_submeshes;   ///< الأجزاء / Submeshes
    Handle m_gpuHandle;                 ///< مؤشر GPU / GPU handle
    bool m_isDirty;                     ///< حالة التعديل / Dirty flag
};

// ============================================================================
// مولد الأشكال الأساسية / Primitive Shape Generator
// ============================================================================

/// فئة مولد الأشكال / Shape generator class
class MeshGenerator {
public:
    // ============================================
    // الأشكال الأساسية / Basic Shapes
    // ============================================
    
    /// مكعب / Cube
    static MeshData CreateCube(Float32 size = 1.0f) {
        return CreateBox(size, size, size);
    }
    
    /// صندوق / Box
    static MeshData CreateBox(Float32 width, Float32 height, Float32 depth) {
        MeshData mesh;
        Float32 hw = width * 0.5f;
        Float32 hh = height * 0.5f;
        Float32 hd = depth * 0.5f;
        
        // الوجوه الستة / Six faces
        // الأمام / Front (Z+)
        AddQuad(mesh, Vec3(-hw, -hh, hd), Vec3(hw, -hh, hd), Vec3(hw, hh, hd), Vec3(-hw, hh, hd), Vec3::UnitZ());
        // الخلف / Back (Z-)
        AddQuad(mesh, Vec3(hw, -hh, -hd), Vec3(-hw, -hh, -hd), Vec3(-hw, hh, -hd), Vec3(hw, hh, -hd), -Vec3::UnitZ());
        // اليمين / Right (X+)
        AddQuad(mesh, Vec3(hw, -hh, hd), Vec3(hw, -hh, -hd), Vec3(hw, hh, -hd), Vec3(hw, hh, hd), Vec3::UnitX());
        // اليسار / Left (X-)
        AddQuad(mesh, Vec3(-hw, -hh, -hd), Vec3(-hw, -hh, hd), Vec3(-hw, hh, hd), Vec3(-hw, hh, -hd), -Vec3::UnitX());
        // الأعلى / Top (Y+)
        AddQuad(mesh, Vec3(-hw, hh, hd), Vec3(hw, hh, hd), Vec3(hw, hh, -hd), Vec3(-hw, hh, -hd), Vec3::UnitY());
        // الأسفل / Bottom (Y-)
        AddQuad(mesh, Vec3(-hw, -hh, -hd), Vec3(hw, -hh, -hd), Vec3(hw, -hh, hd), Vec3(-hw, -hh, hd), -Vec3::UnitY());
        
        mesh.CalculateBounds();
        return mesh;
    }
    
    /// كرة / Sphere
    static MeshData CreateSphere(Float32 radius = 0.5f, int segments = 32, int rings = 16) {
        MeshData mesh;
        
        for (int ring = 0; ring <= rings; ring++) {
            Float32 phi = PI * ring / rings;
            Float32 y = std::cos(phi) * radius;
            Float32 ringRadius = std::sin(phi) * radius;
            
            for (int seg = 0; seg <= segments; seg++) {
                Float32 theta = TWO_PI * seg / segments;
                Float32 x = std::cos(theta) * ringRadius;
                Float32 z = std::sin(theta) * ringRadius;
                
                Vertex3D v;
                v.position = Vec3(x, y, z);
                v.normal = Vec3(x, y, z).Normalized();
                v.texCoord = Vec2(static_cast<Float32>(seg) / segments, static_cast<Float32>(ring) / rings);
                mesh.vertices.push_back(v);
            }
        }
        
        for (int ring = 0; ring < rings; ring++) {
            for (int seg = 0; seg < segments; seg++) {
                int curr = ring * (segments + 1) + seg;
                int next = curr + segments + 1;
                
                mesh.indices.push_back(curr);
                mesh.indices.push_back(next);
                mesh.indices.push_back(curr + 1);
                
                mesh.indices.push_back(curr + 1);
                mesh.indices.push_back(next);
                mesh.indices.push_back(next + 1);
            }
        }
        
        mesh.CalculateTangents();
        mesh.CalculateBounds();
        return mesh;
    }
    
    /// اسطوانة / Cylinder
    static MeshData CreateCylinder(Float32 radius = 0.5f, Float32 height = 1.0f, int segments = 32) {
        MeshData mesh;
        Float32 halfHeight = height * 0.5f;
        
        // الجسم / Body
        for (int i = 0; i <= segments; i++) {
            Float32 angle = TWO_PI * i / segments;
            Float32 x = std::cos(angle) * radius;
            Float32 z = std::sin(angle) * radius;
            Vec3 normal = Vec3(x, 0, z).Normalized();
            Float32 u = static_cast<Float32>(i) / segments;
            
            mesh.vertices.push_back(Vertex3D(Vec3(x, -halfHeight, z), normal, Vec2(u, 0)));
            mesh.vertices.push_back(Vertex3D(Vec3(x, halfHeight, z), normal, Vec2(u, 1)));
        }
        
        for (int i = 0; i < segments; i++) {
            int base = i * 2;
            mesh.indices.push_back(base);
            mesh.indices.push_back(base + 2);
            mesh.indices.push_back(base + 1);
            
            mesh.indices.push_back(base + 1);
            mesh.indices.push_back(base + 2);
            mesh.indices.push_back(base + 3);
        }
        
        // الغطاء العلوي / Top cap
        UInt32 topCenter = static_cast<UInt32>(mesh.vertices.size());
        mesh.vertices.push_back(Vertex3D(Vec3(0, halfHeight, 0), Vec3::UnitY(), Vec2(0.5f, 0.5f)));
        
        for (int i = 0; i <= segments; i++) {
            Float32 angle = TWO_PI * i / segments;
            Float32 x = std::cos(angle) * radius;
            Float32 z = std::sin(angle) * radius;
            mesh.vertices.push_back(Vertex3D(
                Vec3(x, halfHeight, z), Vec3::UnitY(),
                Vec2(0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle))
            ));
        }
        
        for (int i = 0; i < segments; i++) {
            mesh.indices.push_back(topCenter);
            mesh.indices.push_back(topCenter + 1 + i);
            mesh.indices.push_back(topCenter + 2 + i);
        }
        
        // الغطاء السفلي / Bottom cap
        UInt32 bottomCenter = static_cast<UInt32>(mesh.vertices.size());
        mesh.vertices.push_back(Vertex3D(Vec3(0, -halfHeight, 0), -Vec3::UnitY(), Vec2(0.5f, 0.5f)));
        
        for (int i = 0; i <= segments; i++) {
            Float32 angle = TWO_PI * i / segments;
            Float32 x = std::cos(angle) * radius;
            Float32 z = std::sin(angle) * radius;
            mesh.vertices.push_back(Vertex3D(
                Vec3(x, -halfHeight, z), -Vec3::UnitY(),
                Vec2(0.5f + 0.5f * std::cos(angle), 0.5f - 0.5f * std::sin(angle))
            ));
        }
        
        for (int i = 0; i < segments; i++) {
            mesh.indices.push_back(bottomCenter);
            mesh.indices.push_back(bottomCenter + 2 + i);
            mesh.indices.push_back(bottomCenter + 1 + i);
        }
        
        mesh.CalculateTangents();
        mesh.CalculateBounds();
        return mesh;
    }
    
    /// مخروط / Cone
    static MeshData CreateCone(Float32 radius = 0.5f, Float32 height = 1.0f, int segments = 32) {
        MeshData mesh;
        Float32 halfHeight = height * 0.5f;
        
        // القمة / Apex
        UInt32 apexIndex = static_cast<UInt32>(mesh.vertices.size());
        mesh.vertices.push_back(Vertex3D(Vec3(0, halfHeight, 0), Vec3::UnitY(), Vec2(0.5f, 0)));
        
        // الجسم / Body
        for (int i = 0; i <= segments; i++) {
            Float32 angle = TWO_PI * i / segments;
            Float32 x = std::cos(angle) * radius;
            Float32 z = std::sin(angle) * radius;
            
            // حساب العمودي / Calculate normal
            Vec3 edge = Vec3(x, -height, z).Normalized();
            Vec3 tangent = Vec3(-std::sin(angle), 0, std::cos(angle));
            Vec3 normal = tangent.Cross(edge).Normalized();
            
            mesh.vertices.push_back(Vertex3D(
                Vec3(x, -halfHeight, z), normal,
                Vec2(static_cast<Float32>(i) / segments, 1)
            ));
        }
        
        for (int i = 0; i < segments; i++) {
            mesh.indices.push_back(apexIndex);
            mesh.indices.push_back(apexIndex + 1 + i);
            mesh.indices.push_back(apexIndex + 2 + i);
        }
        
        // القاعدة / Base
        UInt32 baseCenter = static_cast<UInt32>(mesh.vertices.size());
        mesh.vertices.push_back(Vertex3D(Vec3(0, -halfHeight, 0), -Vec3::UnitY(), Vec2(0.5f, 0.5f)));
        
        for (int i = 0; i <= segments; i++) {
            Float32 angle = TWO_PI * i / segments;
            Float32 x = std::cos(angle) * radius;
            Float32 z = std::sin(angle) * radius;
            mesh.vertices.push_back(Vertex3D(
                Vec3(x, -halfHeight, z), -Vec3::UnitY(),
                Vec2(0.5f + 0.5f * std::cos(angle), 0.5f - 0.5f * std::sin(angle))
            ));
        }
        
        for (int i = 0; i < segments; i++) {
            mesh.indices.push_back(baseCenter);
            mesh.indices.push_back(baseCenter + 2 + i);
            mesh.indices.push_back(baseCenter + 1 + i);
        }
        
        mesh.CalculateTangents();
        mesh.CalculateBounds();
        return mesh;
    }
    
    /// طارة / Torus
    static MeshData CreateTorus(Float32 majorRadius = 0.5f, Float32 minorRadius = 0.2f,
                                  int majorSegments = 32, int minorSegments = 16) {
        MeshData mesh;
        
        for (int i = 0; i <= majorSegments; i++) {
            Float32 theta = TWO_PI * i / majorSegments;
            Float32 cosTheta = std::cos(theta);
            Float32 sinTheta = std::sin(theta);
            
            for (int j = 0; j <= minorSegments; j++) {
                Float32 phi = TWO_PI * j / minorSegments;
                Float32 cosPhi = std::cos(phi);
                Float32 sinPhi = std::sin(phi);
                
                Float32 x = (majorRadius + minorRadius * cosPhi) * cosTheta;
                Float32 y = minorRadius * sinPhi;
                Float32 z = (majorRadius + minorRadius * cosPhi) * sinTheta;
                
                Vec3 center = Vec3(majorRadius * cosTheta, 0, majorRadius * sinTheta);
                Vec3 position = Vec3(x, y, z);
                Vec3 normal = (position - center).Normalized();
                
                mesh.vertices.push_back(Vertex3D(
                    position, normal,
                    Vec2(static_cast<Float32>(i) / majorSegments, static_cast<Float32>(j) / minorSegments)
                ));
            }
        }
        
        for (int i = 0; i < majorSegments; i++) {
            for (int j = 0; j < minorSegments; j++) {
                int curr = i * (minorSegments + 1) + j;
                int next = curr + minorSegments + 1;
                
                mesh.indices.push_back(curr);
                mesh.indices.push_back(next);
                mesh.indices.push_back(curr + 1);
                
                mesh.indices.push_back(curr + 1);
                mesh.indices.push_back(next);
                mesh.indices.push_back(next + 1);
            }
        }
        
        mesh.CalculateTangents();
        mesh.CalculateBounds();
        return mesh;
    }
    
    /// مستوى / Plane
    static MeshData CreatePlane(Float32 width = 1.0f, Float32 depth = 1.0f, int segmentsX = 1, int segmentsZ = 1) {
        MeshData mesh;
        Float32 hw = width * 0.5f;
        Float32 hd = depth * 0.5f;
        
        for (int z = 0; z <= segmentsZ; z++) {
            Float32 zPos = -hd + depth * z / segmentsZ;
            Float32 v = static_cast<Float32>(z) / segmentsZ;
            
            for (int x = 0; x <= segmentsX; x++) {
                Float32 xPos = -hw + width * x / segmentsX;
                Float32 u = static_cast<Float32>(x) / segmentsX;
                
                mesh.vertices.push_back(Vertex3D(Vec3(xPos, 0, zPos), Vec3::UnitY(), Vec2(u, v)));
            }
        }
        
        for (int z = 0; z < segmentsZ; z++) {
            for (int x = 0; x < segmentsX; x++) {
                int curr = z * (segmentsX + 1) + x;
                int next = curr + segmentsX + 1;
                
                mesh.indices.push_back(curr);
                mesh.indices.push_back(next);
                mesh.indices.push_back(curr + 1);
                
                mesh.indices.push_back(curr + 1);
                mesh.indices.push_back(next);
                mesh.indices.push_back(next + 1);
            }
        }
        
        mesh.CalculateTangents();
        mesh.CalculateBounds();
        return mesh;
    }
    
    /// كبسولة / Capsule
    static MeshData CreateCapsule(Float32 radius = 0.25f, Float32 height = 1.0f, int segments = 32, int rings = 8) {
        MeshData mesh;
        Float32 cylinderHeight = height - 2.0f * radius;
        Float32 halfCylinder = cylinderHeight * 0.5f;
        
        // النصف العلوي من الكرة / Top hemisphere
        for (int ring = 0; ring <= rings; ring++) {
            Float32 phi = HALF_PI * ring / rings;
            Float32 y = std::cos(phi) * radius + halfCylinder;
            Float32 ringRadius = std::sin(phi) * radius;
            
            for (int seg = 0; seg <= segments; seg++) {
                Float32 theta = TWO_PI * seg / segments;
                Float32 x = std::cos(theta) * ringRadius;
                Float32 z = std::sin(theta) * ringRadius;
                
                Vertex3D v;
                v.position = Vec3(x, y, z);
                v.normal = Vec3(std::cos(theta) * std::sin(phi), std::cos(phi), std::sin(theta) * std::sin(phi));
                v.texCoord = Vec2(static_cast<Float32>(seg) / segments, 0.5f - 0.5f * ring / rings);
                mesh.vertices.push_back(v);
            }
        }
        
        // الاسطوانة / Cylinder body
        int cylinderRings = rings / 2;
        for (int ring = 0; ring <= cylinderRings; ring++) {
            Float32 y = halfCylinder - cylinderHeight * ring / cylinderRings;
            
            for (int seg = 0; seg <= segments; seg++) {
                Float32 theta = TWO_PI * seg / segments;
                Float32 x = std::cos(theta) * radius;
                Float32 z = std::sin(theta) * radius;
                
                mesh.vertices.push_back(Vertex3D(
                    Vec3(x, y, z),
                    Vec3(std::cos(theta), 0, std::sin(theta)),
                    Vec2(static_cast<Float32>(seg) / segments, 0.5f + 0.5f * ring / (cylinderRings + 2 * rings))
                ));
            }
        }
        
        // النصف السفلي من الكرة / Bottom hemisphere
        for (int ring = 0; ring <= rings; ring++) {
            Float32 phi = HALF_PI + HALF_PI * ring / rings;
            Float32 y = std::cos(phi) * radius - halfCylinder;
            Float32 ringRadius = std::sin(phi) * radius;
            
            for (int seg = 0; seg <= segments; seg++) {
                Float32 theta = TWO_PI * seg / segments;
                Float32 x = std::cos(theta) * ringRadius;
                Float32 z = std::sin(theta) * ringRadius;
                
                mesh.vertices.push_back(Vertex3D(
                    Vec3(x, y, z),
                    Vec3(std::cos(theta) * std::sin(phi), std::cos(phi), std::sin(theta) * std::sin(phi)),
                    Vec2(static_cast<Float32>(seg) / segments, 0.5f + 0.5f * (cylinderRings + ring) / (cylinderRings + 2 * rings))
                ));
            }
        }
        
        // إنشاء الفهارس / Create indices
        int totalRings = rings + cylinderRings + rings;
        for (int ring = 0; ring < totalRings; ring++) {
            for (int seg = 0; seg < segments; seg++) {
                int curr = ring * (segments + 1) + seg;
                int next = curr + segments + 1;
                
                mesh.indices.push_back(curr);
                mesh.indices.push_back(next);
                mesh.indices.push_back(curr + 1);
                
                mesh.indices.push_back(curr + 1);
                mesh.indices.push_back(next);
                mesh.indices.push_back(next + 1);
            }
        }
        
        mesh.CalculateTangents();
        mesh.CalculateBounds();
        return mesh;
    }
    
    /// شبكة أرضية / Grid
    static MeshData CreateGrid(int size = 10, Float32 spacing = 1.0f) {
        MeshData mesh;
        mesh.primitiveType = PrimitiveType::Lines;
        
        Float32 halfSize = size * spacing * 0.5f;
        
        for (int i = -size; i <= size; i++) {
            Float32 pos = i * spacing;
            Color color = (i == 0) ? Color::Red : Color::Gray;
            
            // خطوط X / X lines
            Vertex3D v1, v2;
            v1.position = Vec3(-halfSize, 0, pos);
            v2.position = Vec3(halfSize, 0, pos);
            v1.color = v2.color = (i == 0) ? Color::Blue : Color::Gray;
            mesh.vertices.push_back(v1);
            mesh.vertices.push_back(v2);
            
            // خطوط Z / Z lines
            Vertex3D v3, v4;
            v3.position = Vec3(pos, 0, -halfSize);
            v4.position = Vec3(pos, 0, halfSize);
            v3.color = v4.color = (i == 0) ? Color::Red : Color::Gray;
            mesh.vertices.push_back(v3);
            mesh.vertices.push_back(v4);
        }
        
        for (UInt32 i = 0; i < mesh.vertices.size(); i++) {
            mesh.indices.push_back(i);
        }
        
        mesh.CalculateBounds();
        return mesh;
    }
    
    // ============================================
    // أشكال خاصة / Special Shapes
    // ============================================
    
    /// سهم / Arrow
    static MeshData CreateArrow(Float32 length = 1.0f, Float32 shaftRadius = 0.02f,
                                  Float32 headRadius = 0.05f, Float32 headLength = 0.15f) {
        MeshData shaft = CreateCylinder(shaftRadius, length - headLength, 16);
        MeshData head = CreateCone(headRadius, headLength, 16);
        
        // تحريك الجسم / Move shaft
        for (auto& v : shaft.vertices) {
            v.position.y += (length - headLength) * 0.5f;
        }
        
        // تحريك الرأس / Move head
        for (auto& v : head.vertices) {
            v.position.y += length - headLength * 0.5f;
        }
        
        shaft.Merge(head);
        shaft.CalculateBounds();
        return shaft;
    }
    
    /// محاور الإحداثيات / Coordinate axes
    static MeshData CreateAxes(Float32 size = 1.0f) {
        MeshData mesh;
        mesh.primitiveType = PrimitiveType::Lines;
        
        // المحور X (أحمر) / X axis (red)
        Vertex3D x0, x1;
        x0.position = Vec3::Zero(); x0.color = Color::Red;
        x1.position = Vec3(size, 0, 0); x1.color = Color::Red;
        mesh.vertices.push_back(x0);
        mesh.vertices.push_back(x1);
        
        // المحور Y (أخضر) / Y axis (green)
        Vertex3D y0, y1;
        y0.position = Vec3::Zero(); y0.color = Color::Green;
        y1.position = Vec3(0, size, 0); y1.color = Color::Green;
        mesh.vertices.push_back(y0);
        mesh.vertices.push_back(y1);
        
        // المحور Z (أزرق) / Z axis (blue)
        Vertex3D z0, z1;
        z0.position = Vec3::Zero(); z0.color = Color::Blue;
        z1.position = Vec3(0, 0, size); z1.color = Color::Blue;
        mesh.vertices.push_back(z0);
        mesh.vertices.push_back(z1);
        
        for (UInt32 i = 0; i < 6; i++) {
            mesh.indices.push_back(i);
        }
        
        mesh.CalculateBounds();
        return mesh;
    }
    
    /// صندوق حدود سلكي / Wireframe bounding box
    static MeshData CreateWireframeBox(const AABB& box) {
        MeshData mesh;
        mesh.primitiveType = PrimitiveType::Lines;
        
        auto corners = box.GetCorners();
        for (const auto& c : corners) {
            Vertex3D v;
            v.position = c;
            v.color = Color::Green;
            mesh.vertices.push_back(v);
        }
        
        // الحواف الأفقية السفلية / Bottom horizontal edges
        mesh.indices.insert(mesh.indices.end(), {0, 1, 1, 3, 3, 2, 2, 0});
        // الحواف الأفقية العلوية / Top horizontal edges
        mesh.indices.insert(mesh.indices.end(), {4, 5, 5, 7, 7, 6, 6, 4});
        // الحواف العمودية / Vertical edges
        mesh.indices.insert(mesh.indices.end(), {0, 4, 1, 5, 2, 6, 3, 7});
        
        mesh.CalculateBounds();
        return mesh;
    }

private:
    /// إضافة مربع / Add quad helper
    static void AddQuad(MeshData& mesh, const Vec3& bl, const Vec3& br, const Vec3& tr, const Vec3& tl, const Vec3& normal) {
        UInt32 baseIndex = static_cast<UInt32>(mesh.vertices.size());
        
        mesh.vertices.push_back(Vertex3D(bl, normal, Vec2(0, 0)));
        mesh.vertices.push_back(Vertex3D(br, normal, Vec2(1, 0)));
        mesh.vertices.push_back(Vertex3D(tr, normal, Vec2(1, 1)));
        mesh.vertices.push_back(Vertex3D(tl, normal, Vec2(0, 1)));
        
        mesh.indices.push_back(baseIndex);
        mesh.indices.push_back(baseIndex + 1);
        mesh.indices.push_back(baseIndex + 2);
        
        mesh.indices.push_back(baseIndex);
        mesh.indices.push_back(baseIndex + 2);
        mesh.indices.push_back(baseIndex + 3);
    }
};

// ============================================================================
// معرّف الشبكة / Mesh Handle
// ============================================================================

/// مؤشر ذكي للشبكة / Smart pointer to mesh
using MeshPtr = std::shared_ptr<Mesh>;

/// إنشاء شبكة فارغة / Create empty mesh
inline MeshPtr CreateMesh() {
    return std::make_shared<Mesh>();
}

/// إنشاء شبكة من بيانات / Create mesh from data
inline MeshPtr CreateMesh(const MeshData& data) {
    return std::make_shared<Mesh>(data);
}

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_MESH_H
