/**
 * @file vm_opcodes.cpp
 * @brief معالجات تعليمات الآلة الافتراضية / VM Opcode Handlers
 * @brief Implementation of remaining opcode handlers
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "vm/vm.h"
#include <iostream>
#include <cstring>
#include <algorithm>

namespace Sad {
namespace VM {

// ========================================
// Bitwise Operations
// العمليات البتية
// ========================================

void VirtualMachine::op_bit_and() {
    Value b = pop();
    Value a = pop();
    
    if (!a.isInt() || !b.isInt()) {
        runtimeError("Bitwise operations require integers");
        push(Value::Null());
        return;
    }
    
    push(Value::Int(a.asInt() & b.asInt()));
}

void VirtualMachine::op_bit_or() {
    Value b = pop();
    Value a = pop();
    
    if (!a.isInt() || !b.isInt()) {
        runtimeError("Bitwise operations require integers");
        push(Value::Null());
        return;
    }
    
    push(Value::Int(a.asInt() | b.asInt()));
}

void VirtualMachine::op_bit_xor() {
    Value b = pop();
    Value a = pop();
    
    if (!a.isInt() || !b.isInt()) {
        runtimeError("Bitwise operations require integers");
        push(Value::Null());
        return;
    }
    
    push(Value::Int(a.asInt() ^ b.asInt()));
}

void VirtualMachine::op_bit_not() {
    Value v = pop();
    
    if (!v.isInt()) {
        runtimeError("Bitwise NOT requires integer");
        push(Value::Null());
        return;
    }
    
    push(Value::Int(~v.asInt()));
}

void VirtualMachine::op_bit_shl() {
    Value b = pop();
    Value a = pop();
    
    if (!a.isInt() || !b.isInt()) {
        runtimeError("Shift operations require integers");
        push(Value::Null());
        return;
    }
    
    push(Value::Int(a.asInt() << b.asInt()));
}

void VirtualMachine::op_bit_shr() {
    Value b = pop();
    Value a = pop();
    
    if (!a.isInt() || !b.isInt()) {
        runtimeError("Shift operations require integers");
        push(Value::Null());
        return;
    }
    
    push(Value::Int(a.asInt() >> b.asInt()));
}

// ========================================
// Comparison Operations
// عمليات المقارنة
// ========================================

void VirtualMachine::op_cmp_eq() {
    Value b = pop();
    Value a = pop();
    push(Value::Bool(valuesEqual(a, b)));
}

void VirtualMachine::op_cmp_ne() {
    Value b = pop();
    Value a = pop();
    push(Value::Bool(!valuesEqual(a, b)));
}

void VirtualMachine::op_cmp_lt() {
    Value b = pop();
    Value a = pop();
    
    if (a.isInt() && b.isInt()) {
        push(Value::Bool(a.asInt() < b.asInt()));
    } else {
        push(Value::Bool(toFloat(a) < toFloat(b)));
    }
}

void VirtualMachine::op_cmp_le() {
    Value b = pop();
    Value a = pop();
    
    if (a.isInt() && b.isInt()) {
        push(Value::Bool(a.asInt() <= b.asInt()));
    } else {
        push(Value::Bool(toFloat(a) <= toFloat(b)));
    }
}

void VirtualMachine::op_cmp_gt() {
    Value b = pop();
    Value a = pop();
    
    if (a.isInt() && b.isInt()) {
        push(Value::Bool(a.asInt() > b.asInt()));
    } else {
        push(Value::Bool(toFloat(a) > toFloat(b)));
    }
}

void VirtualMachine::op_cmp_ge() {
    Value b = pop();
    Value a = pop();
    
    if (a.isInt() && b.isInt()) {
        push(Value::Bool(a.asInt() >= b.asInt()));
    } else {
        push(Value::Bool(toFloat(a) >= toFloat(b)));
    }
}

// ========================================
// Logical Operations
// العمليات المنطقية
// ========================================

void VirtualMachine::op_log_and() {
    Value b = pop();
    Value a = pop();
    push(Value::Bool(toBool(a) && toBool(b)));
}

void VirtualMachine::op_log_or() {
    Value b = pop();
    Value a = pop();
    push(Value::Bool(toBool(a) || toBool(b)));
}

void VirtualMachine::op_log_not() {
    Value v = pop();
    push(Value::Bool(!toBool(v)));
}

// ========================================
// Control Flow
// تدفق التحكم
// ========================================

void VirtualMachine::op_jmp() {
    uint32_t offset = readU32();
    ip_ = offset;
}

void VirtualMachine::op_jmp_if() {
    uint32_t offset = readU32();
    Value condition = pop();
    
    if (toBool(condition)) {
        ip_ = offset;
    }
}

void VirtualMachine::op_jmp_not() {
    uint32_t offset = readU32();
    Value condition = pop();
    
    if (!toBool(condition)) {
        ip_ = offset;
    }
}

void VirtualMachine::op_call() {
    uint32_t funcIndex = readU32();
    uint8_t argc = readByte();
    
    if (funcIndex >= module_->getFunctions().size()) {
        runtimeError("Invalid function index");
        return;
    }
    
    const Bytecode::FunctionInfo& func = module_->getFunctions()[funcIndex];
    
    // (AR) تحقق من عدد الوسائط
    // (EN) Check argument count
    if (argc != func.arity) {
        runtimeError("Function expects " + std::to_string(func.arity) + 
                    " arguments but got " + std::to_string(argc));
        return;
    }
    
    // (AR) أنشئ إطار استدعاء
    // (EN) Create call frame
    CallFrame frame(ip_, static_cast<uint32_t>(stack_.size() - argc), func.localCount, &func);
    
    if (frames_.size() >= config_.maxCallDepth) {
        runtimeError("Stack overflow - too many nested calls");
        return;
    }
    
    frames_.push_back(frame);
    stats_.functionCalls++;
    
    // (AR) اقفز إلى الدالة
    // (EN) Jump to function
    ip_ = func.codeOffset;
}

/**
 * @brief استدعاء دالة محلية / Call native function
 * @brief Calls a registered C++ function from bytecode
 * 
 * @details
 * (AR) يستدعي دالة C++ مسجلة باستخدام معرفها.
 *      يقرأ المعرف وعدد الوسائط من bytecode،
 *      يجمع الوسائط من المكدس، ويستدعي الدالة.
 * 
 * (EN) Calls a registered C++ function using its ID.
 *      Reads ID and argument count from bytecode,
 *      gathers arguments from stack, and calls the function.
 * 
 * @bytecode OP_CALL_NATIVE <id:u32> <argc:u8>
 * @stack [..., arg1, arg2, ..., argN] => [..., return_value]
 */
void VirtualMachine::op_call_native() {
    uint32_t nativeId = readU32();
    uint8_t argc = readByte();
    
    // البحث عن الدالة / Find function
    auto it = nativesById_.find(nativeId);
    if (it == nativesById_.end()) {
        runtimeError("Undefined native function with ID: " + std::to_string(nativeId));
        push(Value::Null());
        return;
    }
    
    // جمع الوسائط من المكدس / Gather arguments from stack
    std::vector<Value> args;
    args.reserve(argc);
    for (uint8_t i = 0; i < argc; ++i) {
        args.push_back(pop());
    }
    
    // عكس الترتيب (المكدس LIFO) / Reverse order (stack is LIFO)
    std::reverse(args.begin(), args.end());
    
    // استدعاء الدالة / Call function
    stats_.nativeCalls++;
    Value result = it->second(this, argc, args.data());
    
    // دفع النتيجة / Push result
    push(result);
}

void VirtualMachine::op_loop() {
    int32_t offset = static_cast<int32_t>(readU32());
    ip_ = static_cast<uint32_t>(static_cast<int32_t>(ip_) + offset);
}

// ========================================
// Variable Access
// الوصول للمتغيرات
// ========================================

void VirtualMachine::op_get_local() {
    uint32_t index = readU32();
    
    if (frames_.empty()) {
        runtimeError("No active call frame");
        push(Value::Null());
        return;
    }
    
    const CallFrame& frame = frames_.back();
    uint32_t stackIndex = frame.basePointer + index;
    
    if (stackIndex >= stack_.size()) {
        runtimeError("Invalid local variable index");
        push(Value::Null());
        return;
    }
    
    push(stack_[stackIndex]);
}

void VirtualMachine::op_set_local() {
    uint32_t index = readU32();
    Value value = peek(0);
    
    if (frames_.empty()) {
        runtimeError("No active call frame");
        return;
    }
    
    const CallFrame& frame = frames_.back();
    uint32_t stackIndex = frame.basePointer + index;
    
    if (stackIndex >= stack_.size()) {
        runtimeError("Invalid local variable index");
        return;
    }
    
    stack_[stackIndex] = value;
}

void VirtualMachine::op_get_global() {
    uint32_t index = readU32();
    
    if (index >= globals_.size()) {
        runtimeError("Invalid global variable index");
        push(Value::Null());
        return;
    }
    
    push(globals_[index]);
}

void VirtualMachine::op_set_global() {
    uint32_t index = readU32();
    Value value = peek(0);
    
    if (index >= globals_.size()) {
        runtimeError("Invalid global variable index");
        return;
    }
    
    globals_[index] = value;
}

// ========================================
// Memory Operations
// عمليات الذاكرة
// ========================================

void VirtualMachine::op_malloc() {
    Value sizeVal = pop();
    
    if (!sizeVal.isInt()) {
        runtimeError("malloc requires integer size");
        push(Value::Null());
        return;
    }
    
    size_t size = static_cast<size_t>(sizeVal.asInt());
    void* ptr = std::malloc(size);
    
    if (!ptr) {
        runtimeError("malloc failed");
        push(Value::Null());
        return;
    }
    
    stats_.bytesAllocated += size;
    push(Value::Pointer(ptr));
}

void VirtualMachine::op_free() {
    Value ptrVal = pop();
    
    if (!ptrVal.isPointer()) {
        runtimeError("free requires pointer");
        return;
    }
    
    std::free(ptrVal.asPointer());
}

/**
 * @brief تخطيط ذاكرة / Memory mapping
 * @brief Maps memory region (Stage 1 compatibility)
 * 
 * @details
 * (AR) يحجز منطقة ذاكرة بحجم محدد ويرجع مؤشرها.
 *      يشبه malloc لكن قد يستخدم لاحقاً لـ memory-mapped I/O.
 * 
 * (EN) Allocates a memory region of specified size and returns pointer.
 *      Similar to malloc but may later support memory-mapped I/O.
 * 
 * @stack [..., size:int] => [..., ptr:pointer]
 */
void VirtualMachine::op_mmap() {
    Value sizeVal = pop();
    
    // التحقق من النوع / Type check
    if (!sizeVal.isInt()) {
        runtimeError("mmap: size must be integer");
        push(Value::Null());
        return;
    }
    
    int64_t size = sizeVal.asInt();
    
    // التحقق من الحجم / Size check
    if (size <= 0) {
        runtimeError("mmap: size must be positive");
        push(Value::Null());
        return;
    }
    
    // تحديد حد أقصى (16MB) / Set maximum (16MB)
    const int64_t MAX_MMAP_SIZE = 16 * 1024 * 1024;
    if (size > MAX_MMAP_SIZE) {
        runtimeError("mmap: size exceeds maximum (16MB)");
        push(Value::Null());
        return;
    }
    
    // تخصيص الذاكرة / Allocate memory
    void* ptr = std::calloc(1, static_cast<size_t>(size));
    if (!ptr) {
        runtimeError("mmap: failed to allocate " + std::to_string(size) + " bytes");
        push(Value::Null());
        return;
    }
    
    // إرجاع مؤشر / Return pointer
    push(Value::Pointer(ptr));
}

/**
 * @brief إلغاء تخطيط ذاكرة / Memory unmapping
 * @brief Unmaps/frees memory region (Stage 1 compatibility)
 * 
 * @details
 * (AR) يحرر منطقة ذاكرة محجوزة بواسطة mmap.
 * (EN) Frees memory region allocated by mmap.
 * 
 * @stack [..., ptr:pointer] => [...]
 */
void VirtualMachine::op_munmap() {
    Value ptrVal = pop();
    
    // التحقق من النوع / Type check
    if (!ptrVal.isPointer()) {
        runtimeError("munmap: argument must be pointer");
        return;
    }
    
    void* ptr = ptrVal.asPointer();
    
    // التحقق من null / Check for null
    if (!ptr) {
        // لا شيء للتحرير / Nothing to free
        return;
    }
    
    // تحرير الذاكرة / Free memory
    std::free(ptr);
}

// ========================================
// Array Operations
// عمليات المصفوفات
// ========================================

void VirtualMachine::op_array_new() {
    Value sizeVal = pop();
    
    if (!sizeVal.isInt()) {
        runtimeError("Array size must be integer");
        push(Value::Null());
        return;
    }
    
    size_t size = static_cast<size_t>(sizeVal.asInt());
    
    // (AR) أنشئ كائن مصفوفة
    // (EN) Create array object
    ArrayObject* array = static_cast<ArrayObject*>(
        allocateObject(ObjectType::OBJ_ARRAY, sizeof(ArrayObject))
    );
    
    // (AR) تهيئة المصفوفة باستخدام placement new للـ vector ثم ملئها بـ null
    // (EN) Initialize array using placement new for vector then fill with null
    new (&array->elements) std::vector<Value>(size, Value::Null());
    
    push(Value::Array(array));
}

void VirtualMachine::op_array_get() {
    Value indexVal = pop();
    Value arrayVal = pop();
    
    if (!arrayVal.isArray()) {
        runtimeError("Array access requires array");
        push(Value::Null());
        return;
    }
    
    if (!indexVal.isInt()) {
        runtimeError("Array index must be integer");
        push(Value::Null());
        return;
    }
    
    ArrayObject* array = arrayVal.asArray();
    int64_t index = indexVal.asInt();
    
    if (index < 0 || static_cast<size_t>(index) >= array->elements.size()) {
        runtimeError("Array index out of bounds");
        push(Value::Null());
        return;
    }
    
    push(array->elements[index]);
}

void VirtualMachine::op_array_set() {
    Value value = pop();
    Value indexVal = pop();
    Value arrayVal = pop();
    
    if (!arrayVal.isArray()) {
        runtimeError("Array access requires array");
        return;
    }
    
    if (!indexVal.isInt()) {
        runtimeError("Array index must be integer");
        return;
    }
    
    ArrayObject* array = arrayVal.asArray();
    int64_t index = indexVal.asInt();
    
    if (index < 0 || static_cast<size_t>(index) >= array->elements.size()) {
        runtimeError("Array index out of bounds");
        return;
    }
    
    array->elements[index] = value;
}

void VirtualMachine::op_array_len() {
    Value arrayVal = pop();
    
    if (!arrayVal.isArray()) {
        runtimeError("Array length requires array");
        push(Value::Null());
        return;
    }
    
    ArrayObject* array = arrayVal.asArray();
    push(Value::Int(static_cast<int64_t>(array->elements.size())));
}

void VirtualMachine::op_array_push() {
    Value value = pop();
    Value arrayVal = pop();
    
    if (!arrayVal.isArray()) {
        runtimeError("Array push requires array");
        return;
    }
    
    ArrayObject* array = arrayVal.asArray();
    
    // (AR) أضف العنصر باستخدام push_back — الـ vector يدير السعة تلقائياً
    // (EN) Use push_back — vector manages capacity automatically
    array->elements.push_back(value);
}

void VirtualMachine::op_array_pop() {
    Value arrayVal = pop();
    
    if (!arrayVal.isArray()) {
        runtimeError("Array pop requires array");
        push(Value::Null());
        return;
    }
    
    ArrayObject* array = arrayVal.asArray();
    
    if (array->elements.empty()) {
        runtimeError("Cannot pop from empty array");
        push(Value::Null());
        return;
    }
    
    // (AR) أخذ آخر عنصر ثم حذفه / (EN) Get last element then remove it
    push(array->elements.back());
    array->elements.pop_back();
}

// ========================================
// String Operations
// عمليات النصوص
// ========================================

void VirtualMachine::op_string_concat() {
    Value b = pop();
    Value a = pop();
    
    if (!a.isString() || !b.isString()) {
        runtimeError("String concatenation requires strings");
        push(Value::Null());
        return;
    }
    
    StringObject* strA = a.asString();
    StringObject* strB = b.asString();
    
    size_t newLength = strA->length + strB->length;
    StringObject* result = static_cast<StringObject*>(
        allocateObject(ObjectType::OBJ_STRING, sizeof(StringObject))
    );
    
    result->length = newLength;
    result->chars = new char[newLength + 1];
    
    std::memcpy(result->chars, strA->chars, strA->length);
    std::memcpy(result->chars + strA->length, strB->chars, strB->length);
    result->chars[newLength] = '\0';
    
    // (AR) احسب hash
    // (EN) Calculate hash
    result->hash = std::hash<std::string>{}(std::string(result->chars, newLength));
    
    push(Value::String(result));
}

void VirtualMachine::op_string_len() {
    Value strVal = pop();
    
    if (!strVal.isString()) {
        runtimeError("String length requires string");
        push(Value::Null());
        return;
    }
    
    StringObject* str = strVal.asString();
    push(Value::Int(static_cast<int64_t>(str->length)));
}

void VirtualMachine::op_string_get() {
    Value indexVal = pop();
    Value strVal = pop();
    
    if (!strVal.isString()) {
        runtimeError("String indexing requires string");
        push(Value::Null());
        return;
    }
    
    if (!indexVal.isInt()) {
        runtimeError("String index must be integer");
        push(Value::Null());
        return;
    }
    
    StringObject* str = strVal.asString();
    int64_t index = indexVal.asInt();
    
    if (index < 0 || static_cast<size_t>(index) >= str->length) {
        runtimeError("String index out of bounds");
        push(Value::Null());
        return;
    }
    
    // (AR) أنشئ نص من حرف واحد
    // (EN) Create single-character string
    StringObject* result = static_cast<StringObject*>(
        allocateObject(ObjectType::OBJ_STRING, sizeof(StringObject))
    );
    
    result->length = 1;
    result->chars = new char[2];
    result->chars[0] = str->chars[index];
    result->chars[1] = '\0';
    result->hash = std::hash<std::string>{}(std::string(result->chars, 1));
    
    push(Value::String(result));
}

void VirtualMachine::op_string_substr() {
    Value lengthVal = pop();
    Value startVal = pop();
    Value strVal = pop();
    
    if (!strVal.isString()) {
        runtimeError("Substring requires string");
        push(Value::Null());
        return;
    }
    
    if (!startVal.isInt() || !lengthVal.isInt()) {
        runtimeError("Substring indices must be integers");
        push(Value::Null());
        return;
    }
    
    StringObject* str = strVal.asString();
    int64_t start = startVal.asInt();
    int64_t length = lengthVal.asInt();
    
    if (start < 0 || static_cast<size_t>(start) >= str->length) {
        runtimeError("Substring start index out of bounds");
        push(Value::Null());
        return;
    }
    
    if (length < 0 || static_cast<size_t>(start + length) > str->length) {
        length = static_cast<int64_t>(str->length - start);
    }
    
    StringObject* result = static_cast<StringObject*>(
        allocateObject(ObjectType::OBJ_STRING, sizeof(StringObject))
    );
    
    result->length = static_cast<size_t>(length);
    result->chars = new char[length + 1];
    std::memcpy(result->chars, str->chars + start, length);
    result->chars[length] = '\0';
    result->hash = std::hash<std::string>{}(std::string(result->chars, length));
    
    push(Value::String(result));
}

// يتبع في الجزء التالي...
// (Continued in next part...)

} // namespace VM
} // namespace Sad
