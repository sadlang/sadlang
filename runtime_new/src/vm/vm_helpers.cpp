/**
 * @file vm_helpers.cpp
 * @brief الدوال المساعدة للآلة الافتراضية / VM Helper Functions
 * @brief Utility functions, type conversions, and memory management
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "vm/vm.h"
#include <iostream>
#include <sstream>
#include <iomanip>

namespace Sad {
namespace VM {

// ========================================
// Object Operations (Remaining)
// ========================================

/**
 * @brief إنشاء كائن جديد / Create new object
 * @brief Creates an empty MapObject for storing properties
 * 
 * @details
 * (AR) ينشئ كائن قاموس جديد (MapObject) فارغ ويدفعه على المكدس.
 *      الكائن يُخصص في الذاكرة heap ويُسجل في قائمة الكائنات للـ GC.
 * 
 * (EN) Creates a new empty dictionary object (MapObject) and pushes it to stack.
 *      Object is heap-allocated and registered with GC.
 * 
 * @stack [...] => [..., object]
 */
void VirtualMachine::op_object_new() {
    // تخصيص كائن جديد / Allocate new object
    Object* obj = allocateObject(ObjectType::OBJ_MAP, sizeof(MapObject));
    
    if (!obj) {
        runtimeError("Failed to allocate object");
        push(Value::Null());
        return;
    }
    
    // تهيئة MapObject / Initialize MapObject
    MapObject* mapObj = static_cast<MapObject*>(obj);
    new (&mapObj->fields) std::unordered_map<std::string, Value>();
    
    // دفع الكائن على المكدس / Push object to stack
    push(Value::Object(mapObj));
}

/**
 * @brief الحصول على خاصية كائن / Get object property
 * @brief Reads a property from an object
 * 
 * @details
 * (AR) يقرأ خاصية من كائن باستخدام مفتاح نصي.
 *      إذا كانت الخاصية موجودة، يُرجع قيمتها، وإلا يُرجع null.
 * 
 * (EN) Reads a property from an object using a string key.
 *      Returns the value if property exists, otherwise returns null.
 * 
 * @stack [..., object, key_string] => [..., value]
 * @error Runtime error if object is not an object type or key is not a string
 */
void VirtualMachine::op_object_get() {
    // قراءة المفتاح والكائن من المكدس / Pop key and object from stack
    Value keyVal = pop();
    Value objectVal = pop();
    
    // التحقق من الأنواع / Type checking
    if (!objectVal.isObject()) {
        runtimeError("Cannot get property of non-object");
        push(Value::Null());
        return;
    }
    
    if (!keyVal.isString()) {
        runtimeError("Object property key must be a string");
        push(Value::Null());
        return;
    }
    
    // الحصول على الكائن والمفتاح / Get object and key
    MapObject* obj = objectVal.asMap();
    std::string key = keyVal.asString()->toString();
    
    // البحث عن الخاصية / Search for property
    auto it = obj->fields.find(key);
    if (it != obj->fields.end()) {
        // الخاصية موجودة / Property exists
        push(it->second);
    } else {
        // الخاصية غير موجودة / Property doesn't exist
        push(Value::Null());
    }
}

/**
 * @brief تعيين خاصية كائن / Set object property
 * @brief Writes a value to an object property
 * 
 * @details
 * (AR) يعين قيمة لخاصية في كائن باستخدام مفتاح نصي.
 *      إذا كانت الخاصية موجودة، يتم تحديثها، وإلا يتم إنشاؤها.
 * 
 * (EN) Sets a value to an object property using a string key.
 *      Updates existing property or creates new one.
 * 
 * @stack [..., object, key_string, value] => [...]
 * @error Runtime error if object is not an object type or key is not a string
 */
void VirtualMachine::op_object_set() {
    // قراءة القيمة والمفتاح والكائن من المكدس / Pop value, key, and object from stack
    Value value = pop();
    Value keyVal = pop();
    Value objectVal = pop();
    
    // التحقق من الأنواع / Type checking
    if (!objectVal.isObject()) {
        runtimeError("Cannot set property of non-object");
        return;
    }
    
    if (!keyVal.isString()) {
        runtimeError("Object property key must be a string");
        return;
    }
    
    // الحصول على الكائن والمفتاح / Get object and key
    MapObject* obj = objectVal.asMap();
    std::string key = keyVal.asString()->toString();
    
    // تعيين الخاصية / Set property
    obj->fields[key] = value;
}

// ========================================
// I/O Operations
// ========================================

/**
 * @brief قراءة من ملف / Read from file
 * @brief Reads data from an open file
 * 
 * @details
 * (AR) يقرأ عدداً محدداً من البايتات من ملف مفتوح.
 *      إذا كان الحجم -1، يقرأ الملف بالكامل.
 *      يرجع البيانات كنص أو null عند الفشل.
 * 
 * (EN) Reads specified number of bytes from an open file.
 *      If size is -1, reads entire file.
 *      Returns data as string or null on failure.
 * 
 * @stack [..., fd_int, size_int] => [..., data_string]
 */
void VirtualMachine::op_io_read() {
    Value sizeVal = pop();
    Value fdVal = pop();
    
    // التحقق من الأنواع / Type checking
    if (!fdVal.isInt() || !sizeVal.isInt()) {
        runtimeError("File descriptor and size must be integers");
        push(Value::Null());
        return;
    }
    
    int32_t fd = static_cast<int32_t>(fdVal.asInt());
    int64_t size = sizeVal.asInt();
    
    // البحث عن الملف / Find file
    auto it = fileHandles_.find(fd);
    if (it == fileHandles_.end()) {
        runtimeError("Invalid file descriptor");
        push(Value::Null());
        return;
    }
    
    FileHandle& handle = it->second;
    if (!handle.stream || !handle.isOpen) {
        runtimeError("File is not open");
        push(Value::Null());
        return;
    }
    
    std::string data;
    
    if (size == -1) {
        // قراءة الملف بالكامل / Read entire file
        std::stringstream buffer;
        buffer << handle.stream->rdbuf();
        data = buffer.str();
    } else {
        // قراءة عدد محدد من البايتات / Read specific number of bytes
        data.resize(static_cast<size_t>(size));
        handle.stream->read(&data[0], size);
        
        // ضبط الحجم حسب ما تم قراءته فعلياً / Adjust size based on actual read
        data.resize(handle.stream->gcount());
    }
    
    // إنشاء string object / Create string object
    Object* strObj = allocateObject(ObjectType::OBJ_STRING, sizeof(StringObject));
    if (!strObj) {
        runtimeError("Failed to allocate string");
        push(Value::Null());
        return;
    }
    
    StringObject* str = static_cast<StringObject*>(strObj);
    str->length = data.length();
    str->chars = new char[str->length + 1];
    std::memcpy(str->chars, data.c_str(), str->length);
    str->chars[str->length] = '\0';
    
    // حساب hash / Calculate hash
    str->hash = 0;
    for (size_t i = 0; i < str->length; i++) {
        str->hash = str->hash * 31 + static_cast<uint32_t>(str->chars[i]);
    }
    
    push(Value::String(str));
}

/**
 * @brief الكتابة إلى ملف أو مخرج / Write to file or output
 * @brief Writes data to a file or stdout/stderr
 * 
 * @details
 * (AR) يكتب بيانات إلى ملف مفتوح أو مخرج قياسي.
 *      fd=0: stdin (غير مدعوم)
 *      fd=1: stdout
 *      fd=2: stderr
 *      fd>2: ملف مفتوح
 * 
 * (EN) Writes data to an open file or standard output.
 *      fd=0: stdin (not supported)
 *      fd=1: stdout
 *      fd=2: stderr
 *      fd>2: open file
 * 
 * @stack [..., fd_int, data] => [..., bytes_written_int]
 */
void VirtualMachine::op_io_write() {
    Value data = pop();
    Value fdVal = pop();
    
    if (!fdVal.isInt()) {
        runtimeError("File descriptor must be an integer");
        push(Value::Int(-1));
        return;
    }
    
    int32_t fd = static_cast<int32_t>(fdVal.asInt());
    
    // معالجة stdout/stderr / Handle stdout/stderr
    if (fd == 1 || fd == 2) {
        std::ostream& out = (fd == 1) ? std::cout : std::cerr;
        printValue(data);
        out << std::flush;
        
        // حساب عدد البايتات / Calculate bytes written
        int64_t bytesWritten = 0;
        if (data.isString()) {
            bytesWritten = data.asString()->length;
        } else {
            bytesWritten = data.toString().length();
        }
        push(Value::Int(bytesWritten));
        return;
    }
    
    // معالجة الملفات / Handle files
    auto it = fileHandles_.find(fd);
    if (it == fileHandles_.end()) {
        runtimeError("Invalid file descriptor");
        push(Value::Int(-1));
        return;
    }
    
    FileHandle& handle = it->second;
    if (!handle.stream || !handle.isOpen) {
        runtimeError("File is not open");
        push(Value::Int(-1));
        return;
    }
    
    // تحويل البيانات إلى نص / Convert data to string
    std::string str;
    if (data.isString()) {
        str = data.asString()->toString();
    } else {
        str = data.toString();
    }
    
    // الكتابة إلى الملف / Write to file
    (*handle.stream) << str;
    handle.stream->flush();
    
    if (handle.stream->fail()) {
        runtimeError("Failed to write to file");
        push(Value::Int(-1));
        return;
    }
    
    push(Value::Int(static_cast<int64_t>(str.length())));
}

/**
 * @brief فتح ملف / Open file
 * @brief Opens a file and returns a file descriptor
 * 
 * @details
 * (AR) يفتح ملفاً باسم معين ووضع قراءة/كتابة.
 *      يرجع معرف ملف (file descriptor) موجب عند النجاح، أو -1 عند الفشل.
 * 
 * (EN) Opens a file with given name and mode (read/write).
 *      Returns positive file descriptor on success, -1 on failure.
 * 
 * @stack [..., filename_string, mode_int] => [..., fd_int]
 * @modes 0=read, 1=write, 2=append, 3=read+write
 */
void VirtualMachine::op_io_open() {
    Value modeVal = pop();
    Value filenameVal = pop();
    
    // التحقق من الأنواع / Type checking
    if (!filenameVal.isString()) {
        runtimeError("File name must be a string");
        push(Value::Int(-1));
        return;
    }
    
    if (!modeVal.isInt()) {
        runtimeError("File mode must be an integer");
        push(Value::Int(-1));
        return;
    }
    
    std::string filename = filenameVal.asString()->toString();
    int64_t mode = modeVal.asInt();
    
    // تحديد وضع الفتح / Determine open mode
    std::ios_base::openmode openMode;
    switch (mode) {
        case 0: // قراءة / read
            openMode = std::ios::in;
            break;
        case 1: // كتابة / write
            openMode = std::ios::out | std::ios::trunc;
            break;
        case 2: // إلحاق / append
            openMode = std::ios::out | std::ios::app;
            break;
        case 3: // قراءة+كتابة / read+write
            openMode = std::ios::in | std::ios::out;
            break;
        default:
            runtimeError("Invalid file mode");
            push(Value::Int(-1));
            return;
    }
    
    // فتح الملف / Open file
    std::fstream* file = new std::fstream(filename, openMode);
    
    if (!file->is_open()) {
        delete file;
        push(Value::Int(-1)); // فشل / Failure
        return;
    }
    
    // تسجيل الملف / Register file
    int32_t fd = nextFileHandle_++;
    fileHandles_[fd] = FileHandle(file, filename);
    
    push(Value::Int(fd)); // نجاح / Success
}

/**
 * @brief إغلاق ملف / Close file
 * @brief Closes an open file and releases resources
 * 
 * @details
 * (AR) يغلق ملفاً مفتوحاً ويحرر موارده.
 *      يرجع 0 عند النجاح، -1 عند الفشل.
 * 
 * (EN) Closes an open file and releases its resources.
 *      Returns 0 on success, -1 on failure.
 * 
 * @stack [..., fd_int] => [..., result_int]
 */
void VirtualMachine::op_io_close() {
    Value fdVal = pop();
    
    if (!fdVal.isInt()) {
        runtimeError("File descriptor must be an integer");
        push(Value::Int(-1));
        return;
    }
    
    int32_t fd = static_cast<int32_t>(fdVal.asInt());
    
    // البحث عن الملف / Find file
    auto it = fileHandles_.find(fd);
    if (it == fileHandles_.end()) {
        runtimeError("Invalid file descriptor");
        push(Value::Int(-1));
        return;
    }
    
    // إغلاق الملف / Close file
    FileHandle& handle = it->second;
    if (handle.stream && handle.isOpen) {
        handle.stream->close();
        delete handle.stream;
        handle.stream = nullptr;
        handle.isOpen = false;
    }
    
    // إزالة من الخريطة / Remove from map
    fileHandles_.erase(it);
    
    push(Value::Int(0)); // نجاح / Success
}

// ========================================
// Type Operations
// ========================================

void VirtualMachine::op_typeof() {
    Value v = pop();
    
    const char* typeName = nullptr;
    switch (v.type) {
        case VAL_NULL:     typeName = "null"; break;
        case VAL_BOOL:     typeName = "bool"; break;
        case VAL_INT:      typeName = "int"; break;
        case VAL_FLOAT:    typeName = "float"; break;
        case VAL_STRING:   typeName = "string"; break;
        case VAL_ARRAY:    typeName = "array"; break;
        case VAL_OBJECT:   typeName = "object"; break;
        case VAL_FUNCTION: typeName = "function"; break;
        case VAL_CLOSURE:  typeName = "closure"; break;
        case VAL_NATIVE:   typeName = "native"; break;
        case VAL_POINTER:  typeName = "pointer"; break;
        default:           typeName = "unknown"; break;
    }
    
    // (AR) أنشئ نص للنوع
    // (EN) Create type string
    StringObject* typeStr = static_cast<StringObject*>(
        allocateObject(OBJ_STRING, sizeof(StringObject))
    );
    
    size_t len = std::strlen(typeName);
    typeStr->length = len;
    typeStr->chars = new char[len + 1];
    std::strcpy(typeStr->chars, typeName);
    typeStr->hash = std::hash<std::string>{}(typeName);
    
    push(Value::String(typeStr));
}

void VirtualMachine::op_cast_int() {
    Value v = pop();
    push(Value::Int(toInteger(v)));
}

void VirtualMachine::op_cast_float() {
    Value v = pop();
    push(Value::Float(toFloat(v)));
}

void VirtualMachine::op_cast_string() {
    Value v = pop();
    
    std::ostringstream oss;
    
    switch (v.type) {
        case VAL_NULL:
            oss << "null";
            break;
        case VAL_BOOL:
            oss << (v.asBool() ? "true" : "false");
            break;
        case VAL_INT:
            oss << v.asInt();
            break;
        case VAL_FLOAT:
            oss << v.asFloat();
            break;
        case VAL_STRING:
            push(v); // Already a string
            return;
        default:
            oss << "<object>";
            break;
    }
    
    std::string str = oss.str();
    StringObject* strObj = static_cast<StringObject*>(
        allocateObject(OBJ_STRING, sizeof(StringObject))
    );
    
    strObj->length = str.length();
    strObj->chars = new char[str.length() + 1];
    std::strcpy(strObj->chars, str.c_str());
    strObj->hash = std::hash<std::string>{}(str);
    
    push(Value::String(strObj));
}

void VirtualMachine::op_cast_bool() {
    Value v = pop();
    push(Value::Bool(toBool(v)));
}

// ========================================
// Native Functions
// الدوال المحلية
// ========================================

/**
 * @brief تسجيل دالة محلية / Register native function
 * @brief Registers a C++ function callable from bytecode
 * 
 * @details
 * (AR) يسجل دالة C++ بحيث يمكن استدعاؤها من bytecode.
 *      يعطيها معرفاً فريداً تلقائياً.
 * 
 * (EN) Registers a C++ function for bytecode calls.
 *      Assigns it a unique auto-incrementing ID.
 * 
 * @param name اسم الدالة / Function name
 * @param function مؤشر الدالة / Function pointer
 */
void VirtualMachine::registerNative(const std::string& name, NativeFunction function) {
    // تسجيل بالاسم / Register by name
    natives_[name] = function;
    
    // تسجيل بالمعرف / Register by ID
    uint32_t id = nextNativeId_++;
    nativesById_[id] = function;
    nativeNameToId_[name] = id;
}

/**
 * @brief تسجيل دالة محلية بمعرف محدد / Register native with specific ID
 * @brief Registers a C++ function with a predetermined ID
 * 
 * @details
 * (AR) مفيد عند تحميل bytecode مع معرفات ثابتة.
 * (EN) Useful when loading bytecode with fixed IDs.
 * 
 * @param id المعرف المحدد / Specific ID
 * @param name اسم الدالة / Function name
 * @param function مؤشر الدالة / Function pointer
 */
void VirtualMachine::registerNativeById(uint32_t id, const std::string& name, NativeFunction function) {
    natives_[name] = function;
    nativesById_[id] = function;
    nativeNameToId_[name] = id;
    
    // تحديث nextNativeId_ إذا لزم / Update nextNativeId_ if needed
    if (id >= nextNativeId_) {
        nextNativeId_ = id + 1;
    }
}

/**
 * @brief الحصول على معرف دالة / Get function ID
 * @brief Returns the ID of a registered native function
 * 
 * @param name اسم الدالة / Function name
 * @return المعرف أو -1 إذا لم توجد / ID or -1 if not found
 */
int32_t VirtualMachine::getNativeId(const std::string& name) const {
    auto it = nativeNameToId_.find(name);
    if (it == nativeNameToId_.end()) {
        return -1;
    }
    return static_cast<int32_t>(it->second);
}

Value VirtualMachine::callNative(const std::string& name, int argc) {
    auto it = natives_.find(name);
    if (it == natives_.end()) {
        runtimeError("Undefined native function: " + name);
        return Value::Null();
    }
    
    // (AR) اجمع الوسائط
    // (EN) Gather arguments
    std::vector<Value> args;
    args.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        args.push_back(pop());
    }
    std::reverse(args.begin(), args.end());
    
    stats_.nativeCalls++;
    return it->second(this, argc, args.data());
}

// ========================================
// Memory Management
// ========================================

Object* VirtualMachine::allocateObject(ObjectType type, size_t size) {
    Object* object = static_cast<Object*>(std::malloc(size));
    
    if (!object) {
        runtimeError("Out of memory");
        return nullptr;
    }
    
    object->type = type;
    object->isMarked = false;
    object->gcColor = GC_WHITE;
    object->next = objects_;
    objects_ = object;
    
    bytesAllocated_ += size;
    stats_.bytesAllocated += size;
    
    return object;
}

void VirtualMachine::freeObject(Object* object) {
    if (!object) return;
    
    switch (object->type) {
        case OBJ_STRING: {
            StringObject* str = static_cast<StringObject*>(object);
            delete[] str->chars;
            stats_.bytesFreed += str->length + sizeof(StringObject);
            break;
        }
        case OBJ_ARRAY: {
            ArrayObject* arr = static_cast<ArrayObject*>(object);
            delete[] arr->elements;
            stats_.bytesFreed += arr->capacity * sizeof(Value) + sizeof(ArrayObject);
            break;
        }
        case OBJ_MAP: {
            // تحرير خصائص الكائن / Free object properties
            MapObject* obj = static_cast<MapObject*>(object);
            
            // تدمير unordered_map بشكل صحيح / Properly destroy unordered_map
            obj->fields.~unordered_map<std::string, Value>();
            
            stats_.bytesFreed += sizeof(MapObject);
            break;
        }
        case OBJ_OBJECT: {
            // OBJ_OBJECT deprecated - use OBJ_MAP instead
            stats_.bytesFreed += sizeof(MapObject);
            break;
        }
        default:
            break;
    }
    
    std::free(object);
}

void VirtualMachine::collectGarbage() {
    if (!config_.enableGC) return;
    
    if (config_.enableDebug) {
        std::cout << "[GC] Starting collection (allocated: " 
                  << bytesAllocated_ << " bytes)\n";
    }
    
    size_t before = bytesAllocated_;
    
    // (AR) المرحلة 1: ضع علامة على الجذور
    // (EN) Phase 1: Mark roots
    for (const auto& value : stack_) {
        markValue(value);
    }
    
    for (const auto& value : globals_) {
        markValue(value);
    }
    
    // (AR) المرحلة 2: امسح غير المعلَّم
    // (EN) Phase 2: Sweep unmarked
    Object** obj = &objects_;
    while (*obj) {
        if (!(*obj)->isMarked) {
            Object* unreached = *obj;
            *obj = unreached->next;
            freeObject(unreached);
        } else {
            (*obj)->isMarked = false;
            obj = &(*obj)->next;
        }
    }
    
    nextGC_ = bytesAllocated_ * 2;
    stats_.gcCollections++;
    
    if (config_.enableDebug) {
        std::cout << "[GC] Collected " << (before - bytesAllocated_) 
                  << " bytes (now: " << bytesAllocated_ << ")\n";
    }
}

void VirtualMachine::markValue(const Value& value) {
    if (!value.isObject()) return;
    
    Object* object = value.asObject();
    if (!object || object->isMarked) return;
    
    object->isMarked = true;
    
    // (AR) علِّم الكائنات المشار إليها
    // (EN) Mark referenced objects
    switch (object->type) {
        case OBJ_ARRAY: {
            ArrayObject* arr = static_cast<ArrayObject*>(object);
            for (size_t i = 0; i < arr->length; ++i) {
                markValue(arr->elements[i]);
            }
            break;
        }
        case OBJ_MAP: {
            // تعليم خصائص الكائن / Mark object properties
            MapObject* obj = static_cast<MapObject*>(object);
            
            // تعليم جميع القيم في الخريطة / Mark all values in map
            for (auto& [key, value] : obj->fields) {
                markValue(value);
            }
            break;
        }
        case OBJ_OBJECT: {
            // OBJ_OBJECT deprecated - use OBJ_MAP instead
            break;
        }
        default:
            break;
    }
}

// ========================================
// Globals
// ========================================

void VirtualMachine::setGlobal(uint32_t index, const Value& value) {
    if (index >= globals_.size()) {
        globals_.resize(index + 1, Value::Null());
    }
    globals_[index] = value;
}

Value VirtualMachine::getGlobal(uint32_t index) const {
    if (index >= globals_.size()) {
        return Value::Null();
    }
    return globals_[index];
}

// ========================================
// Helper Functions
// ========================================

/**
 * @brief خطأ في وقت التشغيل / Runtime error
 * @brief Reports a runtime error and sets error state
 * 
 * @details
 * (AR) يسجل خطأ runtime ويضبط علم الخطأ.
 *      يطبع stack trace للمساعدة في التصحيح.
 * 
 * (EN) Records runtime error and sets error flag.
 *      Prints stack trace for debugging help.
 * 
 * @param message رسالة الخطأ / Error message
 */
void VirtualMachine::runtimeError(const std::string& message) {
    // ضبط حالة الخطأ / Set error state
    hasError_ = true;
    lastError_ = message;
    
    // طباعة الخطأ / Print error
    std::cerr << "[Runtime Error] " << message << "\n";
    
    // (AR) اطبع stack trace
    // (EN) Print stack trace
    if (!frames_.empty()) {
        std::cerr << "Call stack:\n";
        for (auto it = frames_.rbegin(); it != frames_.rend(); ++it) {
            std::cerr << "  at " << (it->function ? it->function->name : "<unknown>") 
                      << "()\n";
        }
    }
}

bool VirtualMachine::checkValueType(const Value& value, ValueType expected) {
    return value.type == expected;
}

int64_t VirtualMachine::toInteger(const Value& value) {
    if (value.isInt()) {
        return value.asInt();
    } else if (value.isFloat()) {
        return static_cast<int64_t>(value.asFloat());
    } else if (value.isBool()) {
        return value.asBool() ? 1 : 0;
    } else if (value.isString()) {
        try {
            return std::stoll(value.asString()->chars);
        } catch (...) {
            return 0;
        }
    }
    return 0;
}

double VirtualMachine::toFloat(const Value& value) {
    if (value.isFloat()) {
        return value.asFloat();
    } else if (value.isInt()) {
        return static_cast<double>(value.asInt());
    } else if (value.isBool()) {
        return value.asBool() ? 1.0 : 0.0;
    } else if (value.isString()) {
        try {
            return std::stod(value.asString()->chars);
        } catch (...) {
            return 0.0;
        }
    }
    return 0.0;
}

bool VirtualMachine::toBool(const Value& value) {
    switch (value.type) {
        case VAL_NULL:
            return false;
        case VAL_BOOL:
            return value.asBool();
        case VAL_INT:
            return value.asInt() != 0;
        case VAL_FLOAT:
            return value.asFloat() != 0.0;
        case VAL_STRING:
            return value.asString()->length > 0;
        default:
            return true; // Objects are truthy
    }
}

const Bytecode::FunctionInfo* VirtualMachine::findFunction(const std::string& name) const {
    if (!module_) return nullptr;
    
    for (const auto& func : module_->getFunctions()) {
        if (func.name == name) {
            return &func;
        }
    }
    
    return nullptr;
}

// ========================================
// Debug Functions
// ========================================

void VirtualMachine::dumpStack() const {
    std::cout << "Stack [" << stack_.size() << "]: ";
    for (const auto& value : stack_) {
        printValue(value);
        std::cout << " ";
    }
    std::cout << "\n";
}

void VirtualMachine::dumpCallStack() const {
    std::cout << "Call Stack [" << frames_.size() << "]:\n";
    for (size_t i = 0; i < frames_.size(); ++i) {
        const auto& frame = frames_[i];
        std::cout << "  [" << i << "] " 
                  << (frame.function ? frame.function->name : "<unknown>")
                  << " (locals: " << frame.localCount 
                  << ", bp: " << frame.basePointer << ")\n";
    }
}

} // namespace VM
} // namespace Sad
