// ==============================================================================
// shader.cpp - تطبيق نظام التظليل / Shader System Implementation
// ==============================================================================
// الوصف: تطبيق إدارة برامج التظليل
// Description: Implementation of shader program management
// ==============================================================================

#include "../../include/rendering/shader.h"  // ملف الرأس / Header file
#include <glad/glad.h>                       // GLAD OpenGL loader
#include <fstream>                           // لقراءة الملفات / For file reading
#include <sstream>                           // لبناء النصوص / For string building
#include <iostream>                          // للطباعة / For printing
#include <vector>                            // للمصفوفات الديناميكية / For dynamic arrays

namespace SadGraphics {                      // مساحة الأسماء / Namespace

// ==============================================================================
// الإنشاء والتدمير / Construction and Destruction
// ==============================================================================

/// المنشئ الافتراضي / Default constructor
ShaderProgram::ShaderProgram()
    : m_programID(0)                         // تهيئة المعرف بصفر / Initialize ID to zero
    , m_isValid(false)                       // البداية بحالة غير صالحة / Start as invalid
    , m_errorLog("")                         // سجل أخطاء فارغ / Empty error log
{
    // لا حاجة لإنشاء البرنامج الآن / No need to create program now
}

/// منشئ من نصوص / Constructor from source
ShaderProgram::ShaderProgram(const std::string& vertexSource, const std::string& fragmentSource)
    : m_programID(0)                         // تهيئة المعرف / Initialize ID
    , m_isValid(false)                       // بداية غير صالحة / Start invalid
    , m_errorLog("")                         // سجل فارغ / Empty log
{
    // ترجمة وربط البرنامج / Compile and link program
    Compile(vertexSource, fragmentSource);   // محاولة الترجمة / Try compilation
}

/// المدمر / Destructor
ShaderProgram::~ShaderProgram() {
    // حذف البرنامج إذا كان موجوداً / Delete program if exists
    if (m_programID != 0) {                  // التحقق من وجود المعرف / Check ID exists
        glDeleteProgram(m_programID);        // حذف البرنامج / Delete program
        m_programID = 0;                     // إعادة تعيين المعرف / Reset ID
    }
}

/// منشئ النقل / Move constructor
ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    : m_programID(other.m_programID)         // نقل المعرف / Move ID
    , m_isValid(other.m_isValid)             // نقل الصلاحية / Move validity
    , m_errorLog(std::move(other.m_errorLog)) // نقل سجل الأخطاء / Move error log
    , m_uniformLocationCache(std::move(other.m_uniformLocationCache))  // نقل الخريطة / Move cache
{
    // إبطال الكائن المنقول منه / Invalidate moved-from object
    other.m_programID = 0;                   // إزالة المعرف / Remove ID
    other.m_isValid = false;                 // جعله غير صالح / Make invalid
}

/// إسناد النقل / Move assignment
ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
    // التحقق من عدم الإسناد الذاتي / Check for self-assignment
    if (this != &other) {                    // إذا لم يكن نفس الكائن / If not same object
        // حذف البرنامج الحالي / Delete current program
        if (m_programID != 0) {              // إذا كان موجود / If exists
            glDeleteProgram(m_programID);    // حذفه / Delete it
        }
        
        // نقل البيانات / Move data
        m_programID = other.m_programID;     // نقل المعرف / Move ID
        m_isValid = other.m_isValid;         // نقل الصلاحية / Move validity
        m_errorLog = std::move(other.m_errorLog);  // نقل السجل / Move log
        m_uniformLocationCache = std::move(other.m_uniformLocationCache);  // نقل الخريطة / Move cache
        
        // إبطال الكائن المنقول منه / Invalidate moved-from object
        other.m_programID = 0;               // إزالة المعرف / Remove ID
        other.m_isValid = false;             // جعله غير صالح / Make invalid
    }
    return *this;                            // إرجاع المرجع / Return reference
}

// ==============================================================================
// إدارة البرنامج / Program Management
// ==============================================================================

/// ترجمة وربط البرنامج / Compile and link program
Result ShaderProgram::Compile(const std::string& vertexSource, const std::string& fragmentSource) {
    // مسح سجل الأخطاء السابق / Clear previous error log
    m_errorLog.clear();                      // تنظيف السجل / Clean log
    
    // ترجمة vertex shader / Compile vertex shader
    UInt32 vertexShaderID = CompileShader(ShaderType::Vertex, vertexSource);
    if (vertexShaderID == 0) {               // إذا فشلت الترجمة / If compilation failed
        return Result::Failure;                // إرجاع خطأ / Return error
    }
    
    // ترجمة fragment shader / Compile fragment shader
    UInt32 fragmentShaderID = CompileShader(ShaderType::Fragment, fragmentSource);
    if (fragmentShaderID == 0) {             // إذا فشلت الترجمة / If compilation failed
        glDeleteShader(vertexShaderID);      // حذف vertex shader / Delete vertex shader
        return Result::Failure;                // إرجاع خطأ / Return error
    }
    
    // ربط البرنامج / Link program
    bool linked = LinkProgram(vertexShaderID, fragmentShaderID);
    
    // حذف shaders (لم نعد نحتاجها بعد الربط) / Delete shaders (no longer needed after linking)
    glDeleteShader(vertexShaderID);          // حذف vertex shader / Delete vertex shader
    glDeleteShader(fragmentShaderID);        // حذف fragment shader / Delete fragment shader
    
    // تحديث حالة الصلاحية / Update validity state
    m_isValid = linked;                      // حفظ النتيجة / Store result
    
    return linked ? Result::Success : Result::Failure;  // إرجاع النتيجة / Return result
}

/// استخدام البرنامج / Use program
void ShaderProgram::Use() const {
    // التحقق من صلاحية البرنامج / Check program validity
    if (m_isValid && m_programID != 0) {     // إذا كان صالح / If valid
        glUseProgram(m_programID);           // تفعيل البرنامج / Activate program
    }
}

/// إيقاف استخدام البرنامج / Stop using program
void ShaderProgram::Unbind() {
    glUseProgram(0);                         // إيقاف أي برنامج / Deactivate any program
}

/// تحميل من ملفات / Load from files
ShaderProgram ShaderProgram::LoadFromFiles(const std::string& vertexPath, const std::string& fragmentPath) {
    // قراءة محتوى الملفات / Read file contents
    std::string vertexSource = ReadFile(vertexPath);      // قراءة vertex shader / Read vertex shader
    std::string fragmentSource = ReadFile(fragmentPath);  // قراءة fragment shader / Read fragment shader
    
    // إنشاء البرنامج / Create program
    return ShaderProgram(vertexSource, fragmentSource);   // إرجاع البرنامج / Return program
}

// ==============================================================================
// ضبط المتغيرات / Setting Uniforms
// ==============================================================================

/// ضبط int / Set int
void ShaderProgram::SetInt(const std::string& name, Int32 value) {
    Int32 location = GetUniformLocation(name);  // الحصول على الموقع / Get location
    if (location != -1) {                    // إذا وُجد / If found
        glUniform1i(location, value);        // ضبط القيمة / Set value
    }
}

/// ضبط float / Set float
void ShaderProgram::SetFloat(const std::string& name, Float32 value) {
    Int32 location = GetUniformLocation(name);  // الحصول على الموقع / Get location
    if (location != -1) {                    // إذا وُجد / If found
        glUniform1f(location, value);        // ضبط القيمة / Set value
    }
}

/// ضبط float2 / Set float2
void ShaderProgram::SetFloat2(const std::string& name, Float32 v0, Float32 v1) {
    Int32 location = GetUniformLocation(name);  // الحصول على الموقع / Get location
    if (location != -1) {                    // إذا وُجد / If found
        glUniform2f(location, v0, v1);       // ضبط القيمتين / Set values
    }
}

/// ضبط float3 / Set float3
void ShaderProgram::SetFloat3(const std::string& name, Float32 v0, Float32 v1, Float32 v2) {
    Int32 location = GetUniformLocation(name);  // الحصول على الموقع / Get location
    if (location != -1) {                    // إذا وُجد / If found
        glUniform3f(location, v0, v1, v2);   // ضبط القيم / Set values
    }
}

/// ضبط float4 / Set float4
void ShaderProgram::SetFloat4(const std::string& name, Float32 v0, Float32 v1, Float32 v2, Float32 v3) {
    Int32 location = GetUniformLocation(name);  // الحصول على الموقع / Get location
    if (location != -1) {                    // إذا وُجد / If found
        glUniform4f(location, v0, v1, v2, v3);  // ضبط القيم / Set values
    }
}

/// ضبط لون / Set color
void ShaderProgram::SetColor(const std::string& name, const Color& color) {
    // تحويل اللون من 0-255 إلى 0.0-1.0 / Convert color from 0-255 to 0.0-1.0
    Float32 r = color.r / 255.0f;            // الأحمر / Red
    Float32 g = color.g / 255.0f;            // الأخضر / Green
    Float32 b = color.b / 255.0f;            // الأزرق / Blue
    Float32 a = color.a / 255.0f;            // الشفافية / Alpha
    
    // ضبط القيم / Set values
    SetFloat4(name, r, g, b, a);             // إرسال القيم / Send values
}

/// ضبط مصفوفة / Set matrix
void ShaderProgram::SetMatrix4(const std::string& name, const Float32* matrix) {
    Int32 location = GetUniformLocation(name);  // الحصول على الموقع / Get location
    if (location != -1) {                    // إذا وُجد / If found
        glUniformMatrix4fv(location, 1, GL_FALSE, matrix);  // ضبط المصفوفة / Set matrix
    }
}

/// ضبط texture / Set texture
void ShaderProgram::SetTexture(const std::string& name, Int32 slot) {
    SetInt(name, slot);                      // ضبط رقم الفتحة / Set slot number
}

// ==============================================================================
// معلومات البرنامج / Program Information
// ==============================================================================

/// طباعة معلومات البرنامج / Print program info
void ShaderProgram::PrintInfo() const {
    std::cout << "Shader Program Info:\n";   // العنوان / Title
    std::cout << "  ID: " << m_programID << "\n";            // المعرف / ID
    std::cout << "  Valid: " << (m_isValid ? "Yes" : "No") << "\n";  // الصلاحية / Validity
    
    // إذا كان هناك أخطاء / If there are errors
    if (!m_errorLog.empty()) {               // التحقق من السجل / Check log
        std::cout << "  Errors:\n" << m_errorLog;  // طباعة الأخطاء / Print errors
    }
}

// ==============================================================================
// الدوال الخاصة / Private Functions
// ==============================================================================

/// ترجمة shader / Compile shader
UInt32 ShaderProgram::CompileShader(ShaderType type, const std::string& source) {
    // تحديد نوع OpenGL / Determine OpenGL type
    GLenum glType = (type == ShaderType::Vertex) ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
    
    // إنشاء shader / Create shader
    UInt32 shaderID = glCreateShader(glType); // إنشاء الـ shader / Create shader
    if (shaderID == 0) {                     // إذا فشل الإنشاء / If creation failed
        m_errorLog += "Failed to create shader\n";  // إضافة خطأ / Add error
        return 0;                            // إرجاع فشل / Return failure
    }
    
    // ربط الكود المصدري / Attach source code
    const char* sourceCStr = source.c_str(); // تحويل إلى C string / Convert to C string
    glShaderSource(shaderID, 1, &sourceCStr, nullptr);  // ربط الكود / Attach code
    
    // ترجمة الـ shader / Compile shader
    glCompileShader(shaderID);               // تنفيذ الترجمة / Execute compilation
    
    // التحقق من نجاح الترجمة / Check compilation success
    Int32 success = 0;                       // متغير النجاح / Success variable
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);  // الحصول على الحالة / Get status
    
    // إذا فشلت الترجمة / If compilation failed
    if (!success) {                          // التحقق من الفشل / Check failure
        // الحصول على طول رسالة الخطأ / Get error message length
        Int32 logLength = 0;                 // طول الرسالة / Message length
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);  // طلب الطول / Request length
        
        // قراءة رسالة الخطأ / Read error message
        std::vector<char> errorMessage(logLength + 1);  // مخزن الرسالة / Message buffer
        glGetShaderInfoLog(shaderID, logLength, nullptr, errorMessage.data());  // قراءة الرسالة / Read message
        
        // إضافة الخطأ للسجل / Add error to log
        m_errorLog += "Shader compilation error:\n";    // عنوان الخطأ / Error title
        m_errorLog += errorMessage.data();   // محتوى الخطأ / Error content
        m_errorLog += "\n";                  // سطر جديد / New line
        
        // حذف الـ shader الفاشل / Delete failed shader
        glDeleteShader(shaderID);            // الحذف / Delete
        return 0;                            // إرجاع فشل / Return failure
    }
    
    return shaderID;                         // إرجاع المعرف / Return ID
}

/// ربط البرنامج / Link program
bool ShaderProgram::LinkProgram(UInt32 vertexShaderID, UInt32 fragmentShaderID) {
    // إنشاء البرنامج / Create program
    m_programID = glCreateProgram();         // إنشاء برنامج OpenGL / Create OpenGL program
    if (m_programID == 0) {                  // إذا فشل الإنشاء / If creation failed
        m_errorLog += "Failed to create shader program\n";  // إضافة خطأ / Add error
        return false;                        // إرجاع فشل / Return failure
    }
    
    // ربط الـ shaders بالبرنامج / Attach shaders to program
    glAttachShader(m_programID, vertexShaderID);      // ربط vertex shader / Attach vertex shader
    glAttachShader(m_programID, fragmentShaderID);    // ربط fragment shader / Attach fragment shader
    
    // ربط البرنامج / Link program
    glLinkProgram(m_programID);              // تنفيذ الربط / Execute linking
    
    // التحقق من نجاح الربط / Check linking success
    Int32 success = 0;                       // متغير النجاح / Success variable
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);  // الحصول على الحالة / Get status
    
    // إذا فشل الربط / If linking failed
    if (!success) {                          // التحقق من الفشل / Check failure
        // الحصول على طول رسالة الخطأ / Get error message length
        Int32 logLength = 0;                 // طول الرسالة / Message length
        glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &logLength);  // طلب الطول / Request length
        
        // قراءة رسالة الخطأ / Read error message
        std::vector<char> errorMessage(logLength + 1);  // مخزن الرسالة / Message buffer
        glGetProgramInfoLog(m_programID, logLength, nullptr, errorMessage.data());  // قراءة الرسالة / Read message
        
        // إضافة الخطأ للسجل / Add error to log
        m_errorLog += "Shader linking error:\n";  // عنوان الخطأ / Error title
        m_errorLog += errorMessage.data();   // محتوى الخطأ / Error content
        m_errorLog += "\n";                  // سطر جديد / New line
        
        // حذف البرنامج الفاشل / Delete failed program
        glDeleteProgram(m_programID);        // الحذف / Delete
        m_programID = 0;                     // إعادة تعيين المعرف / Reset ID
        return false;                        // إرجاع فشل / Return failure
    }
    
    // فك ارتباط الـ shaders (لم نعد نحتاجها) / Detach shaders (no longer needed)
    glDetachShader(m_programID, vertexShaderID);      // فك vertex shader / Detach vertex shader
    glDetachShader(m_programID, fragmentShaderID);    // فك fragment shader / Detach fragment shader
    
    return true;                             // نجح الربط / Linking succeeded
}

/// الحصول على موقع uniform / Get uniform location
Int32 ShaderProgram::GetUniformLocation(const std::string& name) const {
    // البحث في الخريطة أولاً / Search cache first
    auto it = m_uniformLocationCache.find(name);  // البحث / Search
    if (it != m_uniformLocationCache.end()) {     // إذا وُجد / If found
        return it->second;                   // إرجاع الموقع المحفوظ / Return cached location
    }
    
    // الحصول على الموقع من OpenGL / Get location from OpenGL
    Int32 location = glGetUniformLocation(m_programID, name.c_str());  // طلب الموقع / Request location
    
    // حفظ في الخريطة / Cache the location
    m_uniformLocationCache[name] = location; // الحفظ / Save
    
    return location;                         // إرجاع الموقع / Return location
}

/// قراءة ملف / Read file
std::string ShaderProgram::ReadFile(const std::string& path) {
    // فتح الملف / Open file
    std::ifstream file(path);                // محاولة الفتح / Try opening
    if (!file.is_open()) {                   // إذا فشل الفتح / If opening failed
        return "";                           // إرجاع نص فارغ / Return empty string
    }
    
    // قراءة المحتوى / Read content
    std::stringstream buffer;                // مخزن مؤقت / Buffer
    buffer << file.rdbuf();                  // قراءة كل المحتوى / Read all content
    
    return buffer.str();                     // إرجاع النص / Return string
}

// ==============================================================================
// Shaders افتراضية / Default Shaders
// ==============================================================================

/// shader افتراضي بسيط / Simple default shader
ShaderProgram CreateDefaultShader() {
    // كود vertex shader / Vertex shader code
    const char* vertexSource = R"(
        #version 330 core
        layout(location = 0) in vec2 aPosition;  // موقع الرأس / Vertex position
        
        uniform mat4 uProjection;                // مصفوفة الإسقاط / Projection matrix
        uniform mat4 uTransform;                 // مصفوفة التحويل / Transform matrix
        
        void main() {
            gl_Position = uProjection * uTransform * vec4(aPosition, 0.0, 1.0);  // حساب الموقع النهائي / Calculate final position
        }
    )";
    
    // كود fragment shader / Fragment shader code
    const char* fragmentSource = R"(
        #version 330 core
        out vec4 FragColor;                      // اللون النهائي / Final color
        
        uniform vec4 uColor;                     // اللون الموحد / Uniform color
        
        void main() {
            FragColor = uColor;                  // استخدام اللون الموحد / Use uniform color
        }
    )";
    
    // إنشاء وإرجاع البرنامج / Create and return program
    return ShaderProgram(vertexSource, fragmentSource);
}

/// shader للنصوص / Shader for text
ShaderProgram CreateTextShader() {
    // مشابه للـ default لكن مع texture / Similar to default but with texture
    const char* vertexSource = R"(
        #version 330 core
        layout(location = 0) in vec2 aPosition;  // الموقع / Position
        layout(location = 1) in vec2 aTexCoord;  // إحداثيات texture / Texture coordinates
        
        out vec2 vTexCoord;                      // إحداثيات texture للـ fragment shader / Texture coords to fragment shader
        
        uniform mat4 uProjection;                // مصفوفة الإسقاط / Projection matrix
        uniform mat4 uTransform;                 // مصفوفة التحويل / Transform matrix
        
        void main() {
            vTexCoord = aTexCoord;               // تمرير الإحداثيات / Pass coordinates
            gl_Position = uProjection * uTransform * vec4(aPosition, 0.0, 1.0);
        }
    )";
    
    const char* fragmentSource = R"(
        #version 330 core
        in vec2 vTexCoord;                       // إحداثيات texture / Texture coordinates
        out vec4 FragColor;                      // اللون النهائي / Final color
        
        uniform sampler2D uTexture;              // texture النص / Text texture
        uniform vec4 uColor;                     // لون النص / Text color
        
        void main() {
            vec4 sampled = texture(uTexture, vTexCoord);  // أخذ عينة من الـ texture / Sample texture
            FragColor = uColor * sampled;        // دمج مع اللون / Combine with color
        }
    )";
    
    return ShaderProgram(vertexSource, fragmentSource);
}

/// shader للصور / Shader for textures
ShaderProgram CreateTextureShader() {
    // مشابه للنصوص / Similar to text shader
    return CreateTextShader();               // استخدام نفس الـ shader / Use same shader
}

} // namespace SadGraphics
