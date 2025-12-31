// ==============================================================================
// shader.h - نظام التظليل Shader / Shader System
// ==============================================================================
// الوصف: إدارة برامج التظليل (Vertex و Fragment Shaders)
// Description: Management of shader programs (Vertex and Fragment Shaders)
// ==============================================================================

#ifndef SAD_GRAPHICS_RENDERING_SHADER_H
#define SAD_GRAPHICS_RENDERING_SHADER_H

#include "../core/types.h"      // الأنواع الأساسية / Basic types
#include <string>                // للنصوص / For strings
#include <unordered_map>         // لخريطة المتغيرات / For uniform map

namespace SadGraphics {          // مساحة الأسماء / Namespace

// ==============================================================================
// أنواع التظليل / Shader Types
// ==============================================================================
enum class ShaderType {
    Vertex,                      // تظليل الرؤوس / Vertex shader
    Fragment,                    // تظليل الأجزاء / Fragment shader
    Geometry,                    // تظليل الهندسة / Geometry shader (للمستقبل / Future)
};

// ==============================================================================
// فئة برنامج التظليل / Shader Program Class
// ==============================================================================
class ShaderProgram {
public:
    // ==============================================================================
    // الإنشاء والتدمير / Construction and Destruction
    // ==============================================================================
    
    /// إنشاء برنامج تظليل فارغ / Create empty shader program
    ShaderProgram();
    
    /// إنشاء برنامج تظليل من نصوص / Create shader program from source code
    /// vertexSource: كود تظليل الرؤوس / Vertex shader source
    /// fragmentSource: كود تظليل الأجزاء / Fragment shader source
    ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource);
    
    /// إنشاء من ملفات / Create from files
    /// vertexPath: مسار ملف تظليل الرؤوس / Vertex shader file path
    /// fragmentPath: مسار ملف تظليل الأجزاء / Fragment shader file path
    static ShaderProgram LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);
    
    /// المدمر / Destructor
    ~ShaderProgram();
    
    // منع النسخ / Prevent copying
    ShaderProgram(const ShaderProgram&) = delete;              // منع النسخ / Disable copy
    ShaderProgram& operator=(const ShaderProgram&) = delete;   // منع الإسناد / Disable assignment
    
    // السماح بالنقل / Allow moving
    ShaderProgram(ShaderProgram&& other) noexcept;             // منشئ النقل / Move constructor
    ShaderProgram& operator=(ShaderProgram&& other) noexcept;  // إسناد النقل / Move assignment
    
    // ==============================================================================
    // إدارة البرنامج / Program Management
    // ==============================================================================
    
    /// ترجمة وربط البرنامج / Compile and link program
    /// vertexSource: كود الرؤوس / Vertex source
    /// fragmentSource: كود الأجزاء / Fragment source
    /// Returns: نجح أم لا / Success or failure
    Result Compile(const std::string& vertexSource, const std::string& fragmentSource);
    
    /// استخدام هذا البرنامج للرسم / Use this program for rendering
    void Use() const;
    
    /// إيقاف استخدام أي برنامج / Stop using any program
    static void Unbind();
    
    /// التحقق من صلاحية البرنامج / Check if program is valid
    /// Returns: صالح أم لا / Valid or not
    bool IsValid() const { return m_isValid; }
    
    /// الحصول على معرف البرنامج / Get program ID
    /// Returns: معرف OpenGL / OpenGL ID
    UInt32 GetID() const { return m_programID; }
    
    // ==============================================================================
    // ضبط المتغيرات Uniforms / Setting Uniforms
    // ==============================================================================
    
    /// ضبط قيمة int / Set int value
    void SetInt(const std::string& name, Int32 value);
    
    /// ضبط قيمة float / Set float value
    void SetFloat(const std::string& name, Float32 value);
    
    /// ضبط قيمتين float / Set 2 float values
    void SetFloat2(const std::string& name, Float32 v0, Float32 v1);
    
    /// ضبط 3 قيم float / Set 3 float values
    void SetFloat3(const std::string& name, Float32 v0, Float32 v1, Float32 v2);
    
    /// ضبط 4 قيم float / Set 4 float values
    void SetFloat4(const std::string& name, Float32 v0, Float32 v1, Float32 v2, Float32 v3);
    
    /// ضبط لون / Set color
    void SetColor(const std::string& name, const Color& color);
    
    /// ضبط مصفوفة 4x4 / Set 4x4 matrix
    void SetMatrix4(const std::string& name, const Float32* matrix);
    
    /// ضبط texture / Set texture
    void SetTexture(const std::string& name, Int32 slot);
    
    // ==============================================================================
    // الحصول على المعلومات / Getting Information
    // ==============================================================================
    
    /// الحصول على سجل الأخطاء / Get error log
    /// Returns: نص الأخطاء / Error text
    std::string GetErrorLog() const { return m_errorLog; }
    
    /// طباعة معلومات البرنامج / Print program info
    void PrintInfo() const;

private:
    // ==============================================================================
    // البيانات الخاصة / Private Data
    // ==============================================================================
    
    UInt32 m_programID;          // معرف برنامج OpenGL / OpenGL program ID
    bool m_isValid;              // هل البرنامج صالح / Is program valid
    std::string m_errorLog;      // سجل الأخطاء / Error log
    
    // خريطة مواقع المتغيرات (للأداء) / Uniform location cache (for performance)
    mutable std::unordered_map<std::string, Int32> m_uniformLocationCache;
    
    // ==============================================================================
    // الدوال الخاصة / Private Functions
    // ==============================================================================
    
    /// ترجمة shader واحد / Compile single shader
    /// type: نوع التظليل / Shader type
    /// source: الكود المصدري / Source code
    /// Returns: معرف الـ shader / Shader ID (0 = فشل / failed)
    UInt32 CompileShader(ShaderType type, const std::string& source);
    
    /// ربط البرنامج / Link program
    /// vertexShaderID: معرف تظليل الرؤوس / Vertex shader ID
    /// fragmentShaderID: معرف تظليل الأجزاء / Fragment shader ID
    /// Returns: نجح أم لا / Success or failure
    bool LinkProgram(UInt32 vertexShaderID, UInt32 fragmentShaderID);
    
    /// الحصول على موقع uniform / Get uniform location
    /// name: اسم المتغير / Variable name
    /// Returns: الموقع (-1 = غير موجود / not found)
    Int32 GetUniformLocation(const std::string& name) const;
    
    /// قراءة محتوى ملف / Read file content
    /// path: مسار الملف / File path
    /// Returns: محتوى الملف / File content
    static std::string ReadFile(const std::string& path);
};

// ==============================================================================
// Shaders افتراضية / Default Shaders
// ==============================================================================

/// الحصول على shader افتراضي بسيط (لون صلب) / Get simple default shader (solid color)
/// Returns: برنامج shader / Shader program
ShaderProgram CreateDefaultShader();

/// الحصول على shader للنصوص / Get shader for text rendering
/// Returns: برنامج shader / Shader program
ShaderProgram CreateTextShader();

/// الحصول على shader للصور / Get shader for texture rendering
/// Returns: برنامج shader / Shader program
ShaderProgram CreateTextureShader();

} // namespace SadGraphics

#endif // SAD_GRAPHICS_RENDERING_SHADER_H
