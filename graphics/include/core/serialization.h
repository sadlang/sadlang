// ==============================================================================
// serialization.h - نظام التسلسل والتخزين / Serialization System
// ==============================================================================
// الوصف: نظام لتحويل البيانات لصيغ قابلة للتخزين والنقل
// Description: System for converting data to storable/transferable formats
// ==============================================================================
// المميزات / Features:
//   - تسلسل JSON / JSON serialization
//   - تسلسل ثنائي / Binary serialization
//   - دعم الأنواع الأساسية / Basic type support
//   - دعم الهياكل المخصصة / Custom struct support
//   - ضغط اختياري / Optional compression
//   - تشفير اختياري / Optional encryption
// ==============================================================================
// الاستخدام / Usage:
//   // التسلسل إلى JSON / Serialize to JSON
//   JsonSerializer json;
//   json.BeginObject();
//   json.Write("name", player.name);
//   json.Write("health", player.health);
//   json.Write("position", player.position);
//   json.EndObject();
//   std::string output = json.ToString();
//
//   // القراءة من JSON / Deserialize from JSON
//   JsonDeserializer reader(jsonString);
//   player.name = reader.ReadString("name");
//   player.health = reader.ReadFloat("health");
// ==============================================================================

#ifndef SAD_GRAPHICS_SERIALIZATION_H
#define SAD_GRAPHICS_SERIALIZATION_H

#include "types.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <memory>
#include <functional>
#include <cstring>

namespace sad {
namespace graphics {

// ==============================================================================
// التعدادات / Enumerations
// ==============================================================================

/// نوع القيمة / Value type
enum class ValueType {
    Null,       // فارغ / Null
    Bool,       // منطقي / Boolean
    Int,        // عدد صحيح / Integer
    Float,      // عدد عشري / Float
    String,     // نص / String
    Array,      // مصفوفة / Array
    Object      // كائن / Object
};

// ==============================================================================
// القيمة العامة / Generic Value
// ==============================================================================

class Value;
using ValueMap = std::unordered_map<std::string, Value>;
using ValueArray = std::vector<Value>;

/// قيمة عامة يمكن أن تحتوي على أي نوع
/// Generic value that can hold any type
class Value {
public:
    // ==========================================================================
    // المنشئات / Constructors
    // ==========================================================================
    
    Value() : m_type(ValueType::Null) {}
    Value(bool val) : m_type(ValueType::Bool), m_bool(val) {}
    Value(Int32 val) : m_type(ValueType::Int), m_int(val) {}
    Value(Int64 val) : m_type(ValueType::Int), m_int(val) {}
    Value(Float32 val) : m_type(ValueType::Float), m_float(val) {}
    Value(Float64 val) : m_type(ValueType::Float), m_float(val) {}
    Value(const std::string& val) : m_type(ValueType::String), m_string(val) {}
    Value(const char* val) : m_type(ValueType::String), m_string(val) {}
    
    /// منشئ المصفوفة / Array constructor
    static Value Array() {
        Value v;
        v.m_type = ValueType::Array;
        v.m_array = std::make_shared<ValueArray>();
        return v;
    }
    
    /// منشئ الكائن / Object constructor
    static Value Object() {
        Value v;
        v.m_type = ValueType::Object;
        v.m_object = std::make_shared<ValueMap>();
        return v;
    }
    
    // ==========================================================================
    // الأنواع / Type checking
    // ==========================================================================
    
    ValueType GetType() const { return m_type; }
    bool IsNull() const { return m_type == ValueType::Null; }
    bool IsBool() const { return m_type == ValueType::Bool; }
    bool IsInt() const { return m_type == ValueType::Int; }
    bool IsFloat() const { return m_type == ValueType::Float; }
    bool IsString() const { return m_type == ValueType::String; }
    bool IsArray() const { return m_type == ValueType::Array; }
    bool IsObject() const { return m_type == ValueType::Object; }
    bool IsNumber() const { return m_type == ValueType::Int || m_type == ValueType::Float; }
    
    // ==========================================================================
    // القراءة / Getters
    // ==========================================================================
    
    bool AsBool(bool defaultVal = false) const {
        return (m_type == ValueType::Bool) ? m_bool : defaultVal;
    }
    
    Int64 AsInt(Int64 defaultVal = 0) const {
        if (m_type == ValueType::Int) return m_int;
        if (m_type == ValueType::Float) return static_cast<Int64>(m_float);
        return defaultVal;
    }
    
    Float64 AsFloat(Float64 defaultVal = 0.0) const {
        if (m_type == ValueType::Float) return m_float;
        if (m_type == ValueType::Int) return static_cast<Float64>(m_int);
        return defaultVal;
    }
    
    const std::string& AsString(const std::string& defaultVal = "") const {
        static const std::string empty;
        return (m_type == ValueType::String) ? m_string : 
               (defaultVal.empty() ? empty : defaultVal);
    }
    
    // ==========================================================================
    // المصفوفات / Array operations
    // ==========================================================================
    
    /// إضافة عنصر للمصفوفة / Add element to array
    void Push(const Value& item) {
        if (m_type == ValueType::Array && m_array) {
            m_array->push_back(item);
        }
    }
    
    /// حجم المصفوفة / Array size
    size_t Size() const {
        if (m_type == ValueType::Array && m_array) return m_array->size();
        if (m_type == ValueType::Object && m_object) return m_object->size();
        return 0;
    }
    
    /// الوصول للعنصر بالفهرس / Access element by index
    const Value& operator[](size_t index) const {
        static Value null;
        if (m_type == ValueType::Array && m_array && index < m_array->size()) {
            return (*m_array)[index];
        }
        return null;
    }
    
    Value& operator[](size_t index) {
        if (m_type == ValueType::Array && m_array) {
            if (index >= m_array->size()) {
                m_array->resize(index + 1);
            }
            return (*m_array)[index];
        }
        static Value null;
        return null;
    }
    
    // ==========================================================================
    // الكائنات / Object operations
    // ==========================================================================
    
    /// تعيين قيمة في الكائن / Set value in object
    void Set(const std::string& key, const Value& value) {
        if (m_type == ValueType::Object && m_object) {
            (*m_object)[key] = value;
        }
    }
    
    /// الحصول على قيمة من الكائن / Get value from object
    const Value& Get(const std::string& key) const {
        static Value null;
        if (m_type == ValueType::Object && m_object) {
            auto it = m_object->find(key);
            if (it != m_object->end()) return it->second;
        }
        return null;
    }
    
    Value& Get(const std::string& key) {
        if (m_type == ValueType::Object && m_object) {
            return (*m_object)[key];
        }
        static Value null;
        return null;
    }
    
    /// هل المفتاح موجود / Has key
    bool HasKey(const std::string& key) const {
        if (m_type == ValueType::Object && m_object) {
            return m_object->count(key) > 0;
        }
        return false;
    }
    
    /// الوصول بالمفتاح / Access by key
    const Value& operator[](const std::string& key) const {
        return Get(key);
    }
    
    Value& operator[](const std::string& key) {
        if (m_type != ValueType::Object) {
            m_type = ValueType::Object;
            m_object = std::make_shared<ValueMap>();
        }
        return (*m_object)[key];
    }
    
    /// الحصول على جميع المفاتيح / Get all keys
    std::vector<std::string> Keys() const {
        std::vector<std::string> keys;
        if (m_type == ValueType::Object && m_object) {
            for (const auto& pair : *m_object) {
                keys.push_back(pair.first);
            }
        }
        return keys;
    }

private:
    ValueType m_type;
    
    // البيانات / Data storage
    bool m_bool = false;
    Int64 m_int = 0;
    Float64 m_float = 0.0;
    std::string m_string;
    std::shared_ptr<ValueArray> m_array;
    std::shared_ptr<ValueMap> m_object;
};

// ==============================================================================
// JSON المُسلسل / JSON Serializer
// ==============================================================================

/// مُسلسل JSON للكتابة
/// JSON Serializer for writing
class JsonSerializer {
public:
    // ==========================================================================
    // إعدادات / Settings
    // ==========================================================================
    
    /// تفعيل التنسيق الجميل / Enable pretty formatting
    void SetPrettyPrint(bool enabled, Int32 indent = 2) {
        m_prettyPrint = enabled;
        m_indentSize = indent;
    }
    
    // ==========================================================================
    // الكتابة / Writing
    // ==========================================================================
    
    /// بدء كائن / Begin object
    void BeginObject() {
        WriteIndent();
        m_output << "{";
        if (m_prettyPrint) m_output << "\n";
        m_indent++;
        m_isFirst.push(true);
    }
    
    /// إنهاء كائن / End object
    void EndObject() {
        m_indent--;
        if (m_prettyPrint) m_output << "\n";
        WriteIndent();
        m_output << "}";
        if (!m_isFirst.empty()) m_isFirst.pop();
    }
    
    /// بدء مصفوفة / Begin array
    void BeginArray() {
        WriteIndent();
        m_output << "[";
        if (m_prettyPrint) m_output << "\n";
        m_indent++;
        m_isFirst.push(true);
    }
    
    /// إنهاء مصفوفة / End array
    void EndArray() {
        m_indent--;
        if (m_prettyPrint) m_output << "\n";
        WriteIndent();
        m_output << "]";
        if (!m_isFirst.empty()) m_isFirst.pop();
    }
    
    /// كتابة مفتاح / Write key
    void WriteKey(const std::string& key) {
        WriteSeparator();
        WriteIndent();
        m_output << "\"" << EscapeString(key) << "\":";
        if (m_prettyPrint) m_output << " ";
    }
    
    /// كتابة قيمة null / Write null value
    void WriteNull(const std::string& key) {
        WriteKey(key);
        m_output << "null";
    }
    
    /// كتابة قيمة منطقية / Write boolean value
    void Write(const std::string& key, bool value) {
        WriteKey(key);
        m_output << (value ? "true" : "false");
    }
    
    /// كتابة عدد صحيح / Write integer
    void Write(const std::string& key, Int32 value) {
        WriteKey(key);
        m_output << value;
    }
    
    void Write(const std::string& key, Int64 value) {
        WriteKey(key);
        m_output << value;
    }
    
    void Write(const std::string& key, UInt32 value) {
        WriteKey(key);
        m_output << value;
    }
    
    /// كتابة عدد عشري / Write float
    void Write(const std::string& key, Float32 value) {
        WriteKey(key);
        m_output << value;
    }
    
    void Write(const std::string& key, Float64 value) {
        WriteKey(key);
        m_output << value;
    }
    
    /// كتابة نص / Write string
    void Write(const std::string& key, const std::string& value) {
        WriteKey(key);
        m_output << "\"" << EscapeString(value) << "\"";
    }
    
    void Write(const std::string& key, const char* value) {
        Write(key, std::string(value));
    }
    
    /// كتابة Vec2 / Write Vec2
    void Write(const std::string& key, const Vec2& value) {
        WriteKey(key);
        m_output << "{\"x\":" << value.x << ",\"y\":" << value.y << "}";
    }
    
    /// كتابة Vec3 / Write Vec3
    void Write(const std::string& key, const Vec3& value) {
        WriteKey(key);
        m_output << "{\"x\":" << value.x << ",\"y\":" << value.y 
                 << ",\"z\":" << value.z << "}";
    }
    
    /// كتابة Vec4 / Write Vec4
    void Write(const std::string& key, const Vec4& value) {
        WriteKey(key);
        m_output << "{\"x\":" << value.x << ",\"y\":" << value.y 
                 << ",\"z\":" << value.z << ",\"w\":" << value.w << "}";
    }
    
    /// كتابة لون / Write color
    void Write(const std::string& key, const Color& value) {
        WriteKey(key);
        m_output << "{\"r\":" << value.r << ",\"g\":" << value.g 
                 << ",\"b\":" << value.b << ",\"a\":" << value.a << "}";
    }
    
    /// كتابة مستطيل / Write rect
    void Write(const std::string& key, const Rect& value) {
        WriteKey(key);
        m_output << "{\"x\":" << value.x << ",\"y\":" << value.y 
                 << ",\"width\":" << value.width << ",\"height\":" << value.height << "}";
    }
    
    /// كتابة مصفوفة أعداد / Write number array
    template<typename T>
    void WriteArray(const std::string& key, const std::vector<T>& values) {
        WriteKey(key);
        m_output << "[";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) m_output << ",";
            m_output << values[i];
        }
        m_output << "]";
    }
    
    /// كتابة مصفوفة نصوص / Write string array
    void WriteArray(const std::string& key, const std::vector<std::string>& values) {
        WriteKey(key);
        m_output << "[";
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) m_output << ",";
            m_output << "\"" << EscapeString(values[i]) << "\"";
        }
        m_output << "]";
    }
    
    // ==========================================================================
    // الإخراج / Output
    // ==========================================================================
    
    /// تحويل إلى نص / Convert to string
    std::string ToString() const {
        return m_output.str();
    }
    
    /// حفظ في ملف / Save to file
    bool SaveToFile(const std::string& path) const {
        std::ofstream file(path);
        if (!file.is_open()) return false;
        file << m_output.str();
        return true;
    }
    
    /// مسح / Clear
    void Clear() {
        m_output.str("");
        m_output.clear();
        m_indent = 0;
        while (!m_isFirst.empty()) m_isFirst.pop();
    }

private:
    /// كتابة المسافة البادئة / Write indent
    void WriteIndent() {
        if (!m_prettyPrint) return;
        for (Int32 i = 0; i < m_indent * m_indentSize; ++i) {
            m_output << " ";
        }
    }
    
    /// كتابة الفاصل / Write separator
    void WriteSeparator() {
        if (!m_isFirst.empty()) {
            if (!m_isFirst.top()) {
                m_output << ",";
                if (m_prettyPrint) m_output << "\n";
            }
            m_isFirst.top() = false;
        }
    }
    
    /// تهريب النص / Escape string
    std::string EscapeString(const std::string& str) const {
        std::string result;
        result.reserve(str.size());
        
        for (char c : str) {
            switch (c) {
                case '"':  result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:   result += c; break;
            }
        }
        
        return result;
    }
    
    std::ostringstream m_output;
    Int32 m_indent = 0;
    Int32 m_indentSize = 2;
    bool m_prettyPrint = false;
    std::stack<bool> m_isFirst;
};

// ==============================================================================
// JSON المُحلل / JSON Parser
// ==============================================================================

/// محلل JSON للقراءة
/// JSON Parser for reading
class JsonParser {
public:
    /// تحليل نص JSON / Parse JSON string
    static Value Parse(const std::string& json) {
        JsonParser parser(json);
        return parser.ParseValue();
    }
    
    /// تحميل من ملف / Load from file
    static Value LoadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return Value();
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return Parse(buffer.str());
    }

private:
    JsonParser(const std::string& json) : m_json(json), m_pos(0) {}
    
    Value ParseValue() {
        SkipWhitespace();
        
        if (m_pos >= m_json.size()) return Value();
        
        char c = m_json[m_pos];
        
        if (c == '{') return ParseObject();
        if (c == '[') return ParseArray();
        if (c == '"') return ParseString();
        if (c == 't' || c == 'f') return ParseBool();
        if (c == 'n') return ParseNull();
        if (c == '-' || (c >= '0' && c <= '9')) return ParseNumber();
        
        return Value();
    }
    
    Value ParseObject() {
        Value obj = Value::Object();
        
        Expect('{');
        SkipWhitespace();
        
        if (m_json[m_pos] == '}') {
            m_pos++;
            return obj;
        }
        
        while (true) {
            SkipWhitespace();
            
            // قراءة المفتاح / Read key
            std::string key = ParseStringValue();
            
            SkipWhitespace();
            Expect(':');
            
            // قراءة القيمة / Read value
            Value value = ParseValue();
            obj.Set(key, value);
            
            SkipWhitespace();
            
            if (m_json[m_pos] == ',') {
                m_pos++;
                continue;
            }
            
            if (m_json[m_pos] == '}') {
                m_pos++;
                break;
            }
            
            break; // خطأ
        }
        
        return obj;
    }
    
    Value ParseArray() {
        Value arr = Value::Array();
        
        Expect('[');
        SkipWhitespace();
        
        if (m_json[m_pos] == ']') {
            m_pos++;
            return arr;
        }
        
        while (true) {
            Value value = ParseValue();
            arr.Push(value);
            
            SkipWhitespace();
            
            if (m_json[m_pos] == ',') {
                m_pos++;
                continue;
            }
            
            if (m_json[m_pos] == ']') {
                m_pos++;
                break;
            }
            
            break;
        }
        
        return arr;
    }
    
    Value ParseString() {
        return Value(ParseStringValue());
    }
    
    std::string ParseStringValue() {
        Expect('"');
        
        std::string result;
        
        while (m_pos < m_json.size() && m_json[m_pos] != '"') {
            if (m_json[m_pos] == '\\') {
                m_pos++;
                if (m_pos < m_json.size()) {
                    switch (m_json[m_pos]) {
                        case '"':  result += '"'; break;
                        case '\\': result += '\\'; break;
                        case 'b':  result += '\b'; break;
                        case 'f':  result += '\f'; break;
                        case 'n':  result += '\n'; break;
                        case 'r':  result += '\r'; break;
                        case 't':  result += '\t'; break;
                        default:   result += m_json[m_pos]; break;
                    }
                }
            } else {
                result += m_json[m_pos];
            }
            m_pos++;
        }
        
        if (m_pos < m_json.size()) m_pos++; // تخطي علامة الاقتباس النهائية
        
        return result;
    }
    
    Value ParseNumber() {
        size_t start = m_pos;
        bool isFloat = false;
        
        if (m_json[m_pos] == '-') m_pos++;
        
        while (m_pos < m_json.size()) {
            char c = m_json[m_pos];
            if (c >= '0' && c <= '9') {
                m_pos++;
            } else if (c == '.' || c == 'e' || c == 'E') {
                isFloat = true;
                m_pos++;
            } else if (c == '+' || c == '-') {
                m_pos++;
            } else {
                break;
            }
        }
        
        std::string numStr = m_json.substr(start, m_pos - start);
        
        if (isFloat) {
            return Value(std::stod(numStr));
        } else {
            return Value(static_cast<Int64>(std::stoll(numStr)));
        }
    }
    
    Value ParseBool() {
        if (m_json.substr(m_pos, 4) == "true") {
            m_pos += 4;
            return Value(true);
        }
        if (m_json.substr(m_pos, 5) == "false") {
            m_pos += 5;
            return Value(false);
        }
        return Value();
    }
    
    Value ParseNull() {
        if (m_json.substr(m_pos, 4) == "null") {
            m_pos += 4;
            return Value();
        }
        return Value();
    }
    
    void SkipWhitespace() {
        while (m_pos < m_json.size()) {
            char c = m_json[m_pos];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                m_pos++;
            } else {
                break;
            }
        }
    }
    
    void Expect(char c) {
        if (m_pos < m_json.size() && m_json[m_pos] == c) {
            m_pos++;
        }
    }
    
    std::string m_json;
    size_t m_pos;
};

// ==============================================================================
// المُسلسل الثنائي / Binary Serializer
// ==============================================================================

/// مُسلسل ثنائي للكتابة
/// Binary serializer for writing
class BinaryWriter {
public:
    // ==========================================================================
    // الكتابة / Writing
    // ==========================================================================
    
    /// كتابة بايت / Write byte
    void WriteByte(UInt8 value) {
        m_data.push_back(value);
    }
    
    /// كتابة منطقي / Write bool
    void WriteBool(bool value) {
        WriteByte(value ? 1 : 0);
    }
    
    /// كتابة عدد صحيح / Write integers
    void WriteInt8(Int8 value) {
        WriteByte(static_cast<UInt8>(value));
    }
    
    void WriteInt16(Int16 value) {
        WriteByte(static_cast<UInt8>(value & 0xFF));
        WriteByte(static_cast<UInt8>((value >> 8) & 0xFF));
    }
    
    void WriteInt32(Int32 value) {
        WriteByte(static_cast<UInt8>(value & 0xFF));
        WriteByte(static_cast<UInt8>((value >> 8) & 0xFF));
        WriteByte(static_cast<UInt8>((value >> 16) & 0xFF));
        WriteByte(static_cast<UInt8>((value >> 24) & 0xFF));
    }
    
    void WriteInt64(Int64 value) {
        for (int i = 0; i < 8; ++i) {
            WriteByte(static_cast<UInt8>((value >> (i * 8)) & 0xFF));
        }
    }
    
    void WriteUInt8(UInt8 value) { WriteByte(value); }
    void WriteUInt16(UInt16 value) { WriteInt16(static_cast<Int16>(value)); }
    void WriteUInt32(UInt32 value) { WriteInt32(static_cast<Int32>(value)); }
    void WriteUInt64(UInt64 value) { WriteInt64(static_cast<Int64>(value)); }
    
    /// كتابة عدد عشري / Write floats
    void WriteFloat32(Float32 value) {
        UInt32 bits;
        std::memcpy(&bits, &value, sizeof(Float32));
        WriteUInt32(bits);
    }
    
    void WriteFloat64(Float64 value) {
        UInt64 bits;
        std::memcpy(&bits, &value, sizeof(Float64));
        WriteUInt64(bits);
    }
    
    /// كتابة نص / Write string
    void WriteString(const std::string& value) {
        WriteUInt32(static_cast<UInt32>(value.size()));
        for (char c : value) {
            WriteByte(static_cast<UInt8>(c));
        }
    }
    
    /// كتابة Vec2 / Write Vec2
    void WriteVec2(const Vec2& value) {
        WriteFloat32(value.x);
        WriteFloat32(value.y);
    }
    
    /// كتابة Vec3 / Write Vec3
    void WriteVec3(const Vec3& value) {
        WriteFloat32(value.x);
        WriteFloat32(value.y);
        WriteFloat32(value.z);
    }
    
    /// كتابة بيانات خام / Write raw data
    void WriteBytes(const void* data, size_t size) {
        const UInt8* bytes = static_cast<const UInt8*>(data);
        for (size_t i = 0; i < size; ++i) {
            WriteByte(bytes[i]);
        }
    }
    
    // ==========================================================================
    // الإخراج / Output
    // ==========================================================================
    
    /// الحصول على البيانات / Get data
    const std::vector<UInt8>& GetData() const { return m_data; }
    
    /// الحجم / Size
    size_t Size() const { return m_data.size(); }
    
    /// حفظ في ملف / Save to file
    bool SaveToFile(const std::string& path) const {
        std::ofstream file(path, std::ios::binary);
        if (!file.is_open()) return false;
        file.write(reinterpret_cast<const char*>(m_data.data()), m_data.size());
        return true;
    }
    
    /// مسح / Clear
    void Clear() { m_data.clear(); }

private:
    std::vector<UInt8> m_data;
};

/// قارئ ثنائي للقراءة
/// Binary reader for reading
class BinaryReader {
public:
    BinaryReader(const std::vector<UInt8>& data) : m_data(data), m_pos(0) {}
    BinaryReader(const void* data, size_t size) 
        : m_data(static_cast<const UInt8*>(data), static_cast<const UInt8*>(data) + size)
        , m_pos(0) {}
    
    /// تحميل من ملف / Load from file
    static BinaryReader FromFile(const std::string& path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) return BinaryReader({});
        
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);
        
        std::vector<UInt8> data(size);
        file.read(reinterpret_cast<char*>(data.data()), size);
        
        return BinaryReader(data);
    }
    
    // ==========================================================================
    // القراءة / Reading
    // ==========================================================================
    
    UInt8 ReadByte() {
        if (m_pos >= m_data.size()) return 0;
        return m_data[m_pos++];
    }
    
    bool ReadBool() { return ReadByte() != 0; }
    
    Int8 ReadInt8() { return static_cast<Int8>(ReadByte()); }
    
    Int16 ReadInt16() {
        UInt8 b0 = ReadByte();
        UInt8 b1 = ReadByte();
        return static_cast<Int16>(b0 | (b1 << 8));
    }
    
    Int32 ReadInt32() {
        UInt8 b0 = ReadByte();
        UInt8 b1 = ReadByte();
        UInt8 b2 = ReadByte();
        UInt8 b3 = ReadByte();
        return static_cast<Int32>(b0 | (b1 << 8) | (b2 << 16) | (b3 << 24));
    }
    
    Int64 ReadInt64() {
        Int64 result = 0;
        for (int i = 0; i < 8; ++i) {
            result |= static_cast<Int64>(ReadByte()) << (i * 8);
        }
        return result;
    }
    
    UInt8 ReadUInt8() { return ReadByte(); }
    UInt16 ReadUInt16() { return static_cast<UInt16>(ReadInt16()); }
    UInt32 ReadUInt32() { return static_cast<UInt32>(ReadInt32()); }
    UInt64 ReadUInt64() { return static_cast<UInt64>(ReadInt64()); }
    
    Float32 ReadFloat32() {
        UInt32 bits = ReadUInt32();
        Float32 value;
        std::memcpy(&value, &bits, sizeof(Float32));
        return value;
    }
    
    Float64 ReadFloat64() {
        UInt64 bits = ReadUInt64();
        Float64 value;
        std::memcpy(&value, &bits, sizeof(Float64));
        return value;
    }
    
    std::string ReadString() {
        UInt32 length = ReadUInt32();
        std::string result;
        result.reserve(length);
        for (UInt32 i = 0; i < length; ++i) {
            result += static_cast<char>(ReadByte());
        }
        return result;
    }
    
    Vec2 ReadVec2() {
        Float32 x = ReadFloat32();
        Float32 y = ReadFloat32();
        return Vec2(x, y);
    }
    
    Vec3 ReadVec3() {
        Float32 x = ReadFloat32();
        Float32 y = ReadFloat32();
        Float32 z = ReadFloat32();
        return Vec3(x, y, z);
    }
    
    void ReadBytes(void* data, size_t size) {
        UInt8* bytes = static_cast<UInt8*>(data);
        for (size_t i = 0; i < size; ++i) {
            bytes[i] = ReadByte();
        }
    }
    
    // ==========================================================================
    // الحالة / State
    // ==========================================================================
    
    size_t GetPosition() const { return m_pos; }
    void SetPosition(size_t pos) { m_pos = pos; }
    size_t GetSize() const { return m_data.size(); }
    bool IsEnd() const { return m_pos >= m_data.size(); }
    size_t Remaining() const { return m_data.size() - m_pos; }

private:
    std::vector<UInt8> m_data;
    size_t m_pos;
};

// ==============================================================================
// واجهة التسلسل / Serializable Interface
// ==============================================================================

/// واجهة للكائنات القابلة للتسلسل
/// Interface for serializable objects
class ISerializable {
public:
    virtual ~ISerializable() = default;
    
    /// التسلسل إلى JSON / Serialize to JSON
    virtual void Serialize(JsonSerializer& serializer) const = 0;
    
    /// القراءة من JSON / Deserialize from JSON
    virtual void Deserialize(const Value& value) = 0;
    
    /// التسلسل الثنائي / Binary serialize
    virtual void Serialize(BinaryWriter& writer) const = 0;
    
    /// القراءة الثنائية / Binary deserialize
    virtual void Deserialize(BinaryReader& reader) = 0;
};

} // namespace graphics
} // namespace sad

#endif // SAD_GRAPHICS_SERIALIZATION_H
