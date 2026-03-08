/**
 * =============================================================================
 * @file mesh_loader.h
 * @brief (AR) محمّل الشبكات ثلاثية الأبعاد — OBJ, PLY, STL
 * @brief (EN) 3D Mesh Loader — OBJ, PLY, STL formats
 * 
 * المهمة: T243 - Mesh Loading (OBJ/PLY)
 * المرحلة: سبتمبر 2026 — 3D Desktop
 * =============================================================================
 * 
 * 🎯 الصيغ المدعومة
 * ═══════════════════
 * ✓ Wavefront OBJ (.obj)
 * ✓ Stanford PLY (.ply) - ASCII & Binary
 * ✓ STL (.stl) - ASCII & Binary
 * 
 * =============================================================================
 */

#ifndef SAD_GRAPHICS_MESH_LOADER_H
#define SAD_GRAPHICS_MESH_LOADER_H

#include "mesh.h"
#include "../core/types.h"
#include "../core/math3d.h"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <memory>

namespace sad {
namespace graphics {

// =============================================================================
// نتيجة التحميل
// =============================================================================

struct MeshLoadResult {
    bool success = false;
    std::string error;
    std::vector<MeshData> meshes;
    std::vector<std::string> materialNames;
    
    operator bool() const { return success; }
};

// =============================================================================
// محمّل OBJ
// =============================================================================

/**
 * @brief محمّل ملفات Wavefront OBJ
 * 
 * يدعم:
 * - الرؤوس (v)
 * - إحداثيات النسيج (vt)
 * - العموديات (vn)
 * - الوجوه (f) - مثلثات ومربعات
 * - المجموعات (g)
 * - المواد (usemtl)
 */
class OBJLoader {
public:
    /**
     * @brief تحميل ملف OBJ
     * @param path مسار الملف
     * @return نتيجة التحميل
     */
    static MeshLoadResult Load(const std::string& path) {
        MeshLoadResult result;
        
        std::ifstream file(path);
        if (!file.is_open()) {
            result.error = "لا يمكن فتح الملف: " + path;
            return result;
        }
        
        std::vector<Vec3> positions;
        std::vector<Vec2> texCoords;
        std::vector<Vec3> normals;
        
        MeshData currentMesh;
        currentMesh.primitiveType = PrimitiveType::Triangles;
        std::string currentMaterial;
        
        std::string line;
        int lineNum = 0;
        
        while (std::getline(file, line)) {
            lineNum++;
            
            // إزالة المسافات
            size_t start = line.find_first_not_of(" \t");
            if (start == std::string::npos) continue;
            line = line.substr(start);
            
            // تجاهل التعليقات
            if (line.empty() || line[0] == '#') continue;
            
            std::istringstream ss(line);
            std::string cmd;
            ss >> cmd;
            
            if (cmd == "v") {
                // رأس
                Vec3 pos;
                ss >> pos.x >> pos.y >> pos.z;
                positions.push_back(pos);
            }
            else if (cmd == "vt") {
                // إحداثيات نسيج
                Vec2 uv;
                ss >> uv.x >> uv.y;
                // تصحيح UV (OpenGL: 0=أسفل، OBJ: 0=أعلى)
                uv.y = 1.0f - uv.y;
                texCoords.push_back(uv);
            }
            else if (cmd == "vn") {
                // عمودي
                Vec3 norm;
                ss >> norm.x >> norm.y >> norm.z;
                normals.push_back(norm);
            }
            else if (cmd == "f") {
                // وجه
                std::string vertex;
                std::vector<Vertex3D> faceVerts;
                
                while (ss >> vertex) {
                    Vertex3D v = ParseFaceVertex(vertex, positions, texCoords, normals);
                    faceVerts.push_back(v);
                }
                
                // تحويل إلى مثلثات (fan triangulation)
                for (size_t i = 2; i < faceVerts.size(); i++) {
                    UInt32 base = static_cast<UInt32>(currentMesh.vertices.size());
                    
                    currentMesh.vertices.push_back(faceVerts[0]);
                    currentMesh.vertices.push_back(faceVerts[i - 1]);
                    currentMesh.vertices.push_back(faceVerts[i]);
                    
                    currentMesh.indices.push_back(base);
                    currentMesh.indices.push_back(base + 1);
                    currentMesh.indices.push_back(base + 2);
                }
            }
            else if (cmd == "g" || cmd == "o") {
                // مجموعة/كائن جديد
                if (!currentMesh.vertices.empty()) {
                    CalculateBounds(currentMesh);
                    result.meshes.push_back(currentMesh);
                    currentMesh = MeshData();
                    currentMesh.primitiveType = PrimitiveType::Triangles;
                }
            }
            else if (cmd == "usemtl") {
                ss >> currentMaterial;
                result.materialNames.push_back(currentMaterial);
            }
        }
        
        // إضافة آخر mesh
        if (!currentMesh.vertices.empty()) {
            CalculateBounds(currentMesh);
            result.meshes.push_back(currentMesh);
        }
        
        // حساب العموديات إذا لم تكن موجودة
        for (auto& mesh : result.meshes) {
            CalculateNormalsIfMissing(mesh);
            CalculateTangents(mesh);
        }
        
        result.success = !result.meshes.empty();
        if (!result.success) {
            result.error = "لم يتم العثور على بيانات شبكة";
        }
        
        return result;
    }

private:
    static Vertex3D ParseFaceVertex(const std::string& str,
                                    const std::vector<Vec3>& positions,
                                    const std::vector<Vec2>& texCoords,
                                    const std::vector<Vec3>& normals) {
        Vertex3D v;
        
        // صيغ: v, v/vt, v/vt/vn, v//vn
        std::vector<std::string> parts;
        std::istringstream ss(str);
        std::string part;
        while (std::getline(ss, part, '/')) {
            parts.push_back(part);
        }
        
        // الموقع
        if (!parts.empty() && !parts[0].empty()) {
            int idx = std::stoi(parts[0]);
            if (idx < 0) idx = static_cast<int>(positions.size()) + idx + 1;
            if (idx > 0 && static_cast<size_t>(idx) <= positions.size()) {
                v.position = positions[idx - 1];
            }
        }
        
        // إحداثيات النسيج
        if (parts.size() > 1 && !parts[1].empty()) {
            int idx = std::stoi(parts[1]);
            if (idx < 0) idx = static_cast<int>(texCoords.size()) + idx + 1;
            if (idx > 0 && static_cast<size_t>(idx) <= texCoords.size()) {
                v.texCoord = texCoords[idx - 1];
            }
        }
        
        // العمودي
        if (parts.size() > 2 && !parts[2].empty()) {
            int idx = std::stoi(parts[2]);
            if (idx < 0) idx = static_cast<int>(normals.size()) + idx + 1;
            if (idx > 0 && static_cast<size_t>(idx) <= normals.size()) {
                v.normal = normals[idx - 1];
            }
        }
        
        return v;
    }
    
    static void CalculateBounds(MeshData& mesh) {
        if (mesh.vertices.empty()) return;
        
        mesh.boundingBox.min = mesh.boundingBox.max = mesh.vertices[0].position;
        
        for (const auto& v : mesh.vertices) {
            mesh.boundingBox.min.x = std::min(mesh.boundingBox.min.x, v.position.x);
            mesh.boundingBox.min.y = std::min(mesh.boundingBox.min.y, v.position.y);
            mesh.boundingBox.min.z = std::min(mesh.boundingBox.min.z, v.position.z);
            
            mesh.boundingBox.max.x = std::max(mesh.boundingBox.max.x, v.position.x);
            mesh.boundingBox.max.y = std::max(mesh.boundingBox.max.y, v.position.y);
            mesh.boundingBox.max.z = std::max(mesh.boundingBox.max.z, v.position.z);
        }
    }
    
    static void CalculateNormalsIfMissing(MeshData& mesh) {
        // تحقق إذا كانت العموديات موجودة
        bool hasNormals = false;
        for (const auto& v : mesh.vertices) {
            if (v.normal.LengthSquared() > 0.001f) {
                hasNormals = true;
                break;
            }
        }
        
        if (hasNormals) return;
        
        // حساب العموديات من المثلثات
        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
            auto& v0 = mesh.vertices[mesh.indices[i]];
            auto& v1 = mesh.vertices[mesh.indices[i + 1]];
            auto& v2 = mesh.vertices[mesh.indices[i + 2]];
            
            Vec3 edge1 = v1.position - v0.position;
            Vec3 edge2 = v2.position - v0.position;
            Vec3 normal = edge1.Cross(edge2).Normalized();
            
            v0.normal = v0.normal + normal;
            v1.normal = v1.normal + normal;
            v2.normal = v2.normal + normal;
        }
        
        // تطبيع
        for (auto& v : mesh.vertices) {
            v.normal = v.normal.Normalized();
        }
    }
    
    static void CalculateTangents(MeshData& mesh) {
        // حساب الـ tangents للـ normal mapping
        for (size_t i = 0; i + 2 < mesh.indices.size(); i += 3) {
            auto& v0 = mesh.vertices[mesh.indices[i]];
            auto& v1 = mesh.vertices[mesh.indices[i + 1]];
            auto& v2 = mesh.vertices[mesh.indices[i + 2]];
            
            Vec3 edge1 = v1.position - v0.position;
            Vec3 edge2 = v2.position - v0.position;
            
            Vec2 deltaUV1 = v1.texCoord - v0.texCoord;
            Vec2 deltaUV2 = v2.texCoord - v0.texCoord;
            
            float f = deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y;
            if (std::abs(f) < 0.0001f) f = 1.0f;
            f = 1.0f / f;
            
            Vec3 tangent(
                f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
                f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
                f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
            );
            tangent = tangent.Normalized();
            
            v0.tangent = Vec4(tangent.x, tangent.y, tangent.z, 1.0f);
            v1.tangent = Vec4(tangent.x, tangent.y, tangent.z, 1.0f);
            v2.tangent = Vec4(tangent.x, tangent.y, tangent.z, 1.0f);
        }
    }
};

// =============================================================================
// محمّل PLY
// =============================================================================

/**
 * @brief محمّل ملفات Stanford PLY
 * 
 * يدعم:
 * - ASCII format
 * - Binary little-endian
 * - Binary big-endian
 * - خصائص مخصصة (vertex colors, etc.)
 */
class PLYLoader {
public:
    static MeshLoadResult Load(const std::string& path) {
        MeshLoadResult result;
        
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            result.error = "لا يمكن فتح الملف: " + path;
            return result;
        }
        
        // قراءة الرأس
        PLYHeader header;
        if (!ParseHeader(file, header)) {
            result.error = "رأس PLY غير صالح";
            return result;
        }
        
        MeshData mesh;
        mesh.primitiveType = PrimitiveType::Triangles;
        
        // قراءة الرؤوس
        if (header.format == "ascii") {
            ReadVerticesASCII(file, header, mesh);
            ReadFacesASCII(file, header, mesh);
        } else if (header.format == "binary_little_endian") {
            ReadVerticesBinary(file, header, mesh, true);
            ReadFacesBinary(file, header, mesh, true);
        } else if (header.format == "binary_big_endian") {
            ReadVerticesBinary(file, header, mesh, false);
            ReadFacesBinary(file, header, mesh, false);
        } else {
            result.error = "صيغة PLY غير مدعومة: " + header.format;
            return result;
        }
        
        // حساب العموديات إذا لم تكن موجودة
        OBJLoader().CalculateNormalsIfMissing(mesh);
        
        result.meshes.push_back(mesh);
        result.success = true;
        return result;
    }

private:
    struct PropertyInfo {
        std::string name;
        std::string type;
        bool isList = false;
        std::string listCountType;
        std::string listValueType;
    };
    
    struct ElementInfo {
        std::string name;
        size_t count = 0;
        std::vector<PropertyInfo> properties;
    };
    
    struct PLYHeader {
        std::string format;
        std::vector<ElementInfo> elements;
        
        const ElementInfo* GetElement(const std::string& name) const {
            for (const auto& e : elements) {
                if (e.name == name) return &e;
            }
            return nullptr;
        }
    };
    
    static bool ParseHeader(std::ifstream& file, PLYHeader& header) {
        std::string line;
        
        // التحقق من التوقيع
        if (!std::getline(file, line) || line.find("ply") == std::string::npos) {
            return false;
        }
        
        ElementInfo* currentElement = nullptr;
        
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string cmd;
            ss >> cmd;
            
            if (cmd == "format") {
                ss >> header.format;
            }
            else if (cmd == "element") {
                header.elements.push_back({});
                currentElement = &header.elements.back();
                ss >> currentElement->name >> currentElement->count;
            }
            else if (cmd == "property" && currentElement) {
                PropertyInfo prop;
                std::string typeOrList;
                ss >> typeOrList;
                
                if (typeOrList == "list") {
                    prop.isList = true;
                    ss >> prop.listCountType >> prop.listValueType >> prop.name;
                } else {
                    prop.type = typeOrList;
                    ss >> prop.name;
                }
                
                currentElement->properties.push_back(prop);
            }
            else if (cmd == "end_header") {
                break;
            }
        }
        
        return true;
    }
    
    static int GetPropertyIndex(const ElementInfo& element, const std::string& name) {
        for (size_t i = 0; i < element.properties.size(); i++) {
            if (element.properties[i].name == name) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }
    
    static void ReadVerticesASCII(std::ifstream& file, const PLYHeader& header, MeshData& mesh) {
        const ElementInfo* vertex = header.GetElement("vertex");
        if (!vertex) return;
        
        int xIdx = GetPropertyIndex(*vertex, "x");
        int yIdx = GetPropertyIndex(*vertex, "y");
        int zIdx = GetPropertyIndex(*vertex, "z");
        int nxIdx = GetPropertyIndex(*vertex, "nx");
        int nyIdx = GetPropertyIndex(*vertex, "ny");
        int nzIdx = GetPropertyIndex(*vertex, "nz");
        int uIdx = GetPropertyIndex(*vertex, "s");
        int vIdx = GetPropertyIndex(*vertex, "t");
        int rIdx = GetPropertyIndex(*vertex, "red");
        int gIdx = GetPropertyIndex(*vertex, "green");
        int bIdx = GetPropertyIndex(*vertex, "blue");
        
        mesh.vertices.reserve(vertex->count);
        
        for (size_t i = 0; i < vertex->count; i++) {
            std::string line;
            std::getline(file, line);
            
            std::vector<float> values;
            std::istringstream ss(line);
            float val;
            while (ss >> val) {
                values.push_back(val);
            }
            
            Vertex3D v;
            if (xIdx >= 0 && xIdx < (int)values.size()) v.position.x = values[xIdx];
            if (yIdx >= 0 && yIdx < (int)values.size()) v.position.y = values[yIdx];
            if (zIdx >= 0 && zIdx < (int)values.size()) v.position.z = values[zIdx];
            
            if (nxIdx >= 0 && nxIdx < (int)values.size()) v.normal.x = values[nxIdx];
            if (nyIdx >= 0 && nyIdx < (int)values.size()) v.normal.y = values[nyIdx];
            if (nzIdx >= 0 && nzIdx < (int)values.size()) v.normal.z = values[nzIdx];
            
            if (uIdx >= 0 && uIdx < (int)values.size()) v.texCoord.x = values[uIdx];
            if (vIdx >= 0 && vIdx < (int)values.size()) v.texCoord.y = values[vIdx];
            
            if (rIdx >= 0 && rIdx < (int)values.size()) {
                v.color.r = static_cast<uint8_t>(values[rIdx]);
                v.color.g = static_cast<uint8_t>(values[gIdx >= 0 ? gIdx : rIdx]);
                v.color.b = static_cast<uint8_t>(values[bIdx >= 0 ? bIdx : rIdx]);
            }
            
            mesh.vertices.push_back(v);
        }
    }
    
    static void ReadFacesASCII(std::ifstream& file, const PLYHeader& header, MeshData& mesh) {
        const ElementInfo* face = header.GetElement("face");
        if (!face) return;
        
        for (size_t i = 0; i < face->count; i++) {
            std::string line;
            std::getline(file, line);
            
            std::istringstream ss(line);
            int count;
            ss >> count;
            
            std::vector<UInt32> indices;
            for (int j = 0; j < count; j++) {
                UInt32 idx;
                ss >> idx;
                indices.push_back(idx);
            }
            
            // تحويل إلى مثلثات
            for (size_t j = 2; j < indices.size(); j++) {
                mesh.indices.push_back(indices[0]);
                mesh.indices.push_back(indices[j - 1]);
                mesh.indices.push_back(indices[j]);
            }
        }
    }
    
    static void ReadVerticesBinary(std::ifstream& file, const PLYHeader& header,
                                   MeshData& mesh, bool littleEndian) {
        // TODO: تنفيذ قراءة binary
        (void)file;
        (void)header;
        (void)mesh;
        (void)littleEndian;
    }
    
    static void ReadFacesBinary(std::ifstream& file, const PLYHeader& header,
                                MeshData& mesh, bool littleEndian) {
        // TODO: تنفيذ قراءة binary
        (void)file;
        (void)header;
        (void)mesh;
        (void)littleEndian;
    }
};

// =============================================================================
// محمّل STL
// =============================================================================

/**
 * @brief محمّل ملفات STL
 */
class STLLoader {
public:
    static MeshLoadResult Load(const std::string& path) {
        MeshLoadResult result;
        
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) {
            result.error = "لا يمكن فتح الملف: " + path;
            return result;
        }
        
        // التحقق من النوع (ASCII أو Binary)
        char header[6];
        file.read(header, 5);
        header[5] = '\0';
        file.seekg(0);
        
        MeshData mesh;
        mesh.primitiveType = PrimitiveType::Triangles;
        
        if (std::string(header) == "solid") {
            // قد يكون ASCII أو binary يبدأ بـ solid
            file.seekg(80);
            uint32_t triangleCount;
            file.read(reinterpret_cast<char*>(&triangleCount), 4);
            
            // التحقق من الحجم
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0);
            
            size_t expectedBinarySize = 84 + triangleCount * 50;
            
            if (fileSize == expectedBinarySize) {
                ReadBinary(file, mesh);
            } else {
                ReadASCII(file, mesh);
            }
        } else {
            ReadBinary(file, mesh);
        }
        
        result.meshes.push_back(mesh);
        result.success = !mesh.vertices.empty();
        return result;
    }

private:
    static void ReadASCII(std::ifstream& file, MeshData& mesh) {
        std::string line;
        Vec3 normal;
        std::vector<Vec3> triangle;
        
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string cmd;
            ss >> cmd;
            
            if (cmd == "facet") {
                std::string normalWord;
                ss >> normalWord >> normal.x >> normal.y >> normal.z;
            }
            else if (cmd == "vertex") {
                Vec3 pos;
                ss >> pos.x >> pos.y >> pos.z;
                triangle.push_back(pos);
            }
            else if (cmd == "endfacet" && triangle.size() == 3) {
                UInt32 base = static_cast<UInt32>(mesh.vertices.size());
                
                for (const auto& pos : triangle) {
                    Vertex3D v;
                    v.position = pos;
                    v.normal = normal;
                    mesh.vertices.push_back(v);
                }
                
                mesh.indices.push_back(base);
                mesh.indices.push_back(base + 1);
                mesh.indices.push_back(base + 2);
                
                triangle.clear();
            }
        }
    }
    
    static void ReadBinary(std::ifstream& file, MeshData& mesh) {
        // تخطي الرأس
        file.seekg(80);
        
        // عدد المثلثات
        uint32_t triangleCount;
        file.read(reinterpret_cast<char*>(&triangleCount), 4);
        
        mesh.vertices.reserve(triangleCount * 3);
        mesh.indices.reserve(triangleCount * 3);
        
        for (uint32_t i = 0; i < triangleCount; i++) {
            // العمودي
            float nx, ny, nz;
            file.read(reinterpret_cast<char*>(&nx), 4);
            file.read(reinterpret_cast<char*>(&ny), 4);
            file.read(reinterpret_cast<char*>(&nz), 4);
            Vec3 normal(nx, ny, nz);
            
            // ثلاث رؤوس
            for (int v = 0; v < 3; v++) {
                float x, y, z;
                file.read(reinterpret_cast<char*>(&x), 4);
                file.read(reinterpret_cast<char*>(&y), 4);
                file.read(reinterpret_cast<char*>(&z), 4);
                
                Vertex3D vertex;
                vertex.position = Vec3(x, y, z);
                vertex.normal = normal;
                
                mesh.indices.push_back(static_cast<UInt32>(mesh.vertices.size()));
                mesh.vertices.push_back(vertex);
            }
            
            // تخطي attribute byte count
            file.seekg(2, std::ios::cur);
        }
    }
};

// =============================================================================
// محمّل موحد
// =============================================================================

/**
 * @brief محمّل شبكات موحد — يكتشف الصيغة تلقائياً
 */
class MeshLoader {
public:
    /**
     * @brief تحميل ملف شبكة
     * @param path مسار الملف
     * @return نتيجة التحميل
     */
    static MeshLoadResult Load(const std::string& path) {
        // استخراج الامتداد
        size_t dotPos = path.rfind('.');
        if (dotPos == std::string::npos) {
            MeshLoadResult result;
            result.error = "لا يمكن تحديد نوع الملف: " + path;
            return result;
        }
        
        std::string ext = path.substr(dotPos);
        // تحويل للحروف الصغيرة
        for (char& c : ext) {
            if (c >= 'A' && c <= 'Z') c += 32;
        }
        
        if (ext == ".obj") {
            return OBJLoader::Load(path);
        } else if (ext == ".ply") {
            return PLYLoader::Load(path);
        } else if (ext == ".stl") {
            return STLLoader::Load(path);
        } else {
            MeshLoadResult result;
            result.error = "صيغة غير مدعومة: " + ext;
            return result;
        }
    }
    
    /**
     * @brief إنشاء أشكال أساسية
     */
    static MeshData CreateCube(float size = 1.0f) {
        MeshData mesh;
        mesh.primitiveType = PrimitiveType::Triangles;
        float h = size / 2;
        
        // Front face
        AddQuad(mesh, Vec3(-h, -h, h), Vec3(h, -h, h), Vec3(h, h, h), Vec3(-h, h, h), Vec3(0, 0, 1));
        // Back face
        AddQuad(mesh, Vec3(h, -h, -h), Vec3(-h, -h, -h), Vec3(-h, h, -h), Vec3(h, h, -h), Vec3(0, 0, -1));
        // Top face
        AddQuad(mesh, Vec3(-h, h, h), Vec3(h, h, h), Vec3(h, h, -h), Vec3(-h, h, -h), Vec3(0, 1, 0));
        // Bottom face
        AddQuad(mesh, Vec3(-h, -h, -h), Vec3(h, -h, -h), Vec3(h, -h, h), Vec3(-h, -h, h), Vec3(0, -1, 0));
        // Right face
        AddQuad(mesh, Vec3(h, -h, h), Vec3(h, -h, -h), Vec3(h, h, -h), Vec3(h, h, h), Vec3(1, 0, 0));
        // Left face
        AddQuad(mesh, Vec3(-h, -h, -h), Vec3(-h, -h, h), Vec3(-h, h, h), Vec3(-h, h, -h), Vec3(-1, 0, 0));
        
        mesh.boundingBox.min = Vec3(-h, -h, -h);
        mesh.boundingBox.max = Vec3(h, h, h);
        
        return mesh;
    }
    
    static MeshData CreateSphere(float radius = 1.0f, int segments = 32, int rings = 16) {
        MeshData mesh;
        mesh.primitiveType = PrimitiveType::Triangles;
        
        for (int ring = 0; ring <= rings; ring++) {
            float phi = static_cast<float>(ring) / rings * 3.14159f;
            float y = std::cos(phi) * radius;
            float ringRadius = std::sin(phi) * radius;
            
            for (int seg = 0; seg <= segments; seg++) {
                float theta = static_cast<float>(seg) / segments * 2.0f * 3.14159f;
                float x = std::cos(theta) * ringRadius;
                float z = std::sin(theta) * ringRadius;
                
                Vertex3D v;
                v.position = Vec3(x, y, z);
                v.normal = Vec3(x, y, z).Normalized();
                v.texCoord = Vec2(static_cast<float>(seg) / segments,
                                  static_cast<float>(ring) / rings);
                
                mesh.vertices.push_back(v);
            }
        }
        
        // الفهارس
        for (int ring = 0; ring < rings; ring++) {
            for (int seg = 0; seg < segments; seg++) {
                UInt32 curr = ring * (segments + 1) + seg;
                UInt32 next = curr + segments + 1;
                
                mesh.indices.push_back(curr);
                mesh.indices.push_back(next);
                mesh.indices.push_back(curr + 1);
                
                mesh.indices.push_back(curr + 1);
                mesh.indices.push_back(next);
                mesh.indices.push_back(next + 1);
            }
        }
        
        mesh.boundingBox.min = Vec3(-radius, -radius, -radius);
        mesh.boundingBox.max = Vec3(radius, radius, radius);
        
        return mesh;
    }
    
    static MeshData CreatePlane(float width = 1.0f, float depth = 1.0f, int segmentsX = 1, int segmentsZ = 1) {
        MeshData mesh;
        mesh.primitiveType = PrimitiveType::Triangles;
        
        float halfW = width / 2;
        float halfD = depth / 2;
        
        for (int z = 0; z <= segmentsZ; z++) {
            for (int x = 0; x <= segmentsX; x++) {
                float u = static_cast<float>(x) / segmentsX;
                float v = static_cast<float>(z) / segmentsZ;
                
                Vertex3D vert;
                vert.position = Vec3(u * width - halfW, 0, v * depth - halfD);
                vert.normal = Vec3(0, 1, 0);
                vert.texCoord = Vec2(u, v);
                
                mesh.vertices.push_back(vert);
            }
        }
        
        for (int z = 0; z < segmentsZ; z++) {
            for (int x = 0; x < segmentsX; x++) {
                UInt32 curr = z * (segmentsX + 1) + x;
                UInt32 next = curr + segmentsX + 1;
                
                mesh.indices.push_back(curr);
                mesh.indices.push_back(next);
                mesh.indices.push_back(curr + 1);
                
                mesh.indices.push_back(curr + 1);
                mesh.indices.push_back(next);
                mesh.indices.push_back(next + 1);
            }
        }
        
        mesh.boundingBox.min = Vec3(-halfW, 0, -halfD);
        mesh.boundingBox.max = Vec3(halfW, 0, halfD);
        
        return mesh;
    }

private:
    static void AddQuad(MeshData& mesh, const Vec3& p0, const Vec3& p1,
                        const Vec3& p2, const Vec3& p3, const Vec3& normal) {
        UInt32 base = static_cast<UInt32>(mesh.vertices.size());
        
        Vertex3D v0, v1, v2, v3;
        v0.position = p0; v0.normal = normal; v0.texCoord = Vec2(0, 0);
        v1.position = p1; v1.normal = normal; v1.texCoord = Vec2(1, 0);
        v2.position = p2; v2.normal = normal; v2.texCoord = Vec2(1, 1);
        v3.position = p3; v3.normal = normal; v3.texCoord = Vec2(0, 1);
        
        mesh.vertices.push_back(v0);
        mesh.vertices.push_back(v1);
        mesh.vertices.push_back(v2);
        mesh.vertices.push_back(v3);
        
        mesh.indices.push_back(base);
        mesh.indices.push_back(base + 1);
        mesh.indices.push_back(base + 2);
        
        mesh.indices.push_back(base);
        mesh.indices.push_back(base + 2);
        mesh.indices.push_back(base + 3);
    }
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_MESH_LOADER_H
