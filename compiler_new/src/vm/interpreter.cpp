/**
 * =============================================================================
 * ملف: interpreter.cpp
 * الوصف: مفسّر الـ Bytecode VM
 * المهمة: T209 - Implement bytecode VM interpreter
 * المرحلة: Phase 21 - User Story 18 (JIT & Tiered Compilation)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو الـ VM (Virtual Machine)؟
 * ─────────────────────────────────
 * آلة افتراضية تنفذ bytecode بدلاً من تعليمات المعالج مباشرة.
 * 
 * لماذا نحتاج VM؟
 * ────────────────
 * 1. **السرعة**: أسرع من التفسير المباشر للكود
 * 2. **المرونة**: يمكن التشغيل على أي منصة
 * 3. **التدرج**: قاعدة للترقية إلى JIT
 * 
 * كيف يعمل؟
 * ──────────
 * ```
 * الكود المصدري → المترجم → Bytecode → VM → النتيجة
 * ```
 * 
 * مثال على تعليمات Bytecode:
 * ──────────────────────────
 * ```
 * PUSH 5        // دفع 5 على المكدس
 * PUSH 3        // دفع 3 على المكدس
 * ADD           // جمع: 5 + 3 = 8
 * PRINT         // طباعة: 8
 * ```
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <stack>
#include <map>
#include <memory>
#include <variant>
#include <optional>
#include <sstream>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <chrono>

namespace sad::vm {

// =============================================================================
// تعريف القيم
// =============================================================================

/**
 * أنواع القيم في VM
 */
enum class ValueType {
    Nil,        // لا شيء
    Bool,       // منطقي
    Int,        // صحيح
    Float,      // عشري
    String,     // نص
    Array,      // مصفوفة
    Object,     // كائن
    Function,   // دالة
};

// إعلان مسبق
struct Value;
using ValuePtr = std::shared_ptr<Value>;

/**
 * قيمة في VM
 */
struct Value {
    using Variant = std::variant<
        std::monostate,         // Nil
        bool,                   // Bool
        int64_t,               // Int
        double,                // Float
        std::string,           // String
        std::vector<ValuePtr>, // Array
        std::map<std::string, ValuePtr>  // Object
    >;
    
    ValueType type;
    Variant data;
    
    static Value nil() { return {ValueType::Nil, std::monostate{}}; }
    static Value boolean(bool b) { return {ValueType::Bool, b}; }
    static Value integer(int64_t i) { return {ValueType::Int, i}; }
    static Value floating(double f) { return {ValueType::Float, f}; }
    static Value string(const std::string& s) { return {ValueType::String, s}; }
    
    bool isNil() const { return type == ValueType::Nil; }
    bool isBool() const { return type == ValueType::Bool; }
    bool isInt() const { return type == ValueType::Int; }
    bool isFloat() const { return type == ValueType::Float; }
    bool isString() const { return type == ValueType::String; }
    bool isNumeric() const { return isInt() || isFloat(); }
    
    int64_t asInt() const { return std::get<int64_t>(data); }
    double asFloat() const { return std::get<double>(data); }
    bool asBool() const { return std::get<bool>(data); }
    const std::string& asString() const { return std::get<std::string>(data); }
    
    double asNumber() const {
        if (isInt()) return static_cast<double>(asInt());
        return asFloat();
    }
    
    std::string toString() const {
        switch (type) {
            case ValueType::Nil: return "لا_شيء";
            case ValueType::Bool: return asBool() ? "صحيح" : "خطأ";
            case ValueType::Int: return std::to_string(asInt());
            case ValueType::Float: return std::to_string(asFloat());
            case ValueType::String: return "\"" + asString() + "\"";
            case ValueType::Array: return "[مصفوفة]";
            case ValueType::Object: return "{كائن}";
            case ValueType::Function: return "<دالة>";
        }
        return "?";
    }
};

// =============================================================================
// تعليمات Bytecode
// =============================================================================

/**
 * رموز التعليمات (Opcodes)
 */
enum class OpCode : uint8_t {
    // ثوابت ومتغيرات
    NOP,            // لا عملية
    CONST,          // تحميل ثابت
    LOAD,           // تحميل متغير
    STORE,          // تخزين متغير
    LOAD_GLOBAL,    // تحميل متغير عام
    STORE_GLOBAL,   // تخزين متغير عام
    
    // المكدس
    POP,            // إزالة من المكدس
    DUP,            // تكرار قمة المكدس
    SWAP,           // تبديل العنصرين العلويين
    
    // العمليات الحسابية
    ADD,            // جمع
    SUB,            // طرح
    MUL,            // ضرب
    DIV,            // قسمة
    MOD,            // باقي القسمة
    NEG,            // سالب
    
    // المقارنة
    EQ,             // يساوي
    NE,             // لا يساوي
    LT,             // أصغر من
    LE,             // أصغر أو يساوي
    GT,             // أكبر من
    GE,             // أكبر أو يساوي
    
    // المنطقية
    NOT,            // نفي
    AND,            // و
    OR,             // أو
    
    // التحكم
    JMP,            // قفز
    JMP_IF,         // قفز إذا صحيح
    JMP_IF_NOT,     // قفز إذا خطأ
    CALL,           // استدعاء دالة
    RET,            // إرجاع
    
    // أخرى
    PRINT,          // طباعة
    HALT,           // إيقاف
};

/**
 * تعليمة Bytecode
 */
struct Instruction {
    OpCode op;
    int32_t operand;
    
    Instruction(OpCode o, int32_t opr = 0) : op(o), operand(opr) {}
};

// =============================================================================
// الوحدة (Module)
// =============================================================================

/**
 * وحدة Bytecode
 * تحتوي على الثوابت والتعليمات
 */
class BytecodeModule {
public:
    /**
     * إضافة ثابت
     */
    int addConstant(const Value& val) {
        constants_.push_back(val);
        return static_cast<int>(constants_.size() - 1);
    }
    
    /**
     * إضافة تعليمة
     */
    int addInstruction(const Instruction& instr) {
        code_.push_back(instr);
        return static_cast<int>(code_.size() - 1);
    }
    
    /**
     * إضافة تعليمة بمعامل
     */
    int emit(OpCode op, int32_t operand = 0) {
        return addInstruction(Instruction(op, operand));
    }
    
    const Value& getConstant(int index) const {
        return constants_.at(index);
    }
    
    const Instruction& getInstruction(int index) const {
        return code_.at(index);
    }
    
    size_t codeSize() const { return code_.size(); }
    
    /**
     * تصحيح قفزة
     */
    void patchJump(int jumpIndex, int target) {
        code_[jumpIndex].operand = target;
    }
    
    /**
     * طباعة disassembly
     */
    void disassemble() const {
        std::cout << "═══ Bytecode Disassembly ═══\n";
        for (size_t i = 0; i < code_.size(); i++) {
            std::cout << std::setw(4) << i << ": ";
            printInstruction(code_[i]);
        }
    }
    
private:
    std::vector<Value> constants_;
    std::vector<Instruction> code_;
    
    void printInstruction(const Instruction& instr) const {
        switch (instr.op) {
            case OpCode::NOP: std::cout << "NOP"; break;
            case OpCode::CONST: 
                std::cout << "CONST " << instr.operand;
                std::cout << " (" << constants_[instr.operand].toString() << ")";
                break;
            case OpCode::LOAD: std::cout << "LOAD " << instr.operand; break;
            case OpCode::STORE: std::cout << "STORE " << instr.operand; break;
            case OpCode::POP: std::cout << "POP"; break;
            case OpCode::ADD: std::cout << "ADD"; break;
            case OpCode::SUB: std::cout << "SUB"; break;
            case OpCode::MUL: std::cout << "MUL"; break;
            case OpCode::DIV: std::cout << "DIV"; break;
            case OpCode::EQ: std::cout << "EQ"; break;
            case OpCode::LT: std::cout << "LT"; break;
            case OpCode::GT: std::cout << "GT"; break;
            case OpCode::NOT: std::cout << "NOT"; break;
            case OpCode::JMP: std::cout << "JMP " << instr.operand; break;
            case OpCode::JMP_IF: std::cout << "JMP_IF " << instr.operand; break;
            case OpCode::JMP_IF_NOT: std::cout << "JMP_IF_NOT " << instr.operand; break;
            case OpCode::CALL: std::cout << "CALL " << instr.operand; break;
            case OpCode::RET: std::cout << "RET"; break;
            case OpCode::PRINT: std::cout << "PRINT"; break;
            case OpCode::HALT: std::cout << "HALT"; break;
            default: std::cout << "UNKNOWN"; break;
        }
        std::cout << "\n";
    }
};

// =============================================================================
// إطار الاستدعاء (Call Frame)
// =============================================================================

/**
 * إطار استدعاء دالة
 */
struct CallFrame {
    int returnAddress;
    int basePointer;
    std::string functionName;
};

// =============================================================================
// المفسّر (Interpreter)
// =============================================================================

/**
 * مفسّر Bytecode VM
 */
class VMInterpreter {
public:
    /**
     * تنفيذ وحدة bytecode
     */
    Value execute(BytecodeModule& module) {
        module_ = &module;
        ip_ = 0;  // مؤشر التعليمة
        running_ = true;
        
        // إعادة تعيين الحالة
        stack_.clear();
        locals_.clear();
        globals_.clear();
        frames_.clear();
        instructionCount_ = 0;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        while (running_ && ip_ < static_cast<int>(module_->codeSize())) {
            const auto& instr = module_->getInstruction(ip_);
            executeInstruction(instr);
            instructionCount_++;
            ip_++;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        executionTime_ = std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - startTime
        ).count();
        
        return stack_.empty() ? Value::nil() : stack_.back();
    }
    
    /**
     * الحصول على إحصائيات التنفيذ
     */
    uint64_t instructionCount() const { return instructionCount_; }
    int64_t executionTime() const { return executionTime_; }
    
    /**
     * تعيين متغير عام
     */
    void setGlobal(const std::string& name, const Value& val) {
        globals_[name] = val;
    }
    
    /**
     * الحصول على متغير عام
     */
    std::optional<Value> getGlobal(const std::string& name) const {
        auto it = globals_.find(name);
        if (it == globals_.end()) return std::nullopt;
        return it->second;
    }
    
private:
    BytecodeModule* module_ = nullptr;
    int ip_ = 0;
    bool running_ = false;
    
    std::vector<Value> stack_;
    std::vector<Value> locals_;
    std::map<std::string, Value> globals_;
    std::vector<CallFrame> frames_;
    
    uint64_t instructionCount_ = 0;
    int64_t executionTime_ = 0;
    
    void push(const Value& val) {
        stack_.push_back(val);
    }
    
    Value pop() {
        if (stack_.empty()) {
            throw std::runtime_error("Stack underflow");
        }
        Value val = stack_.back();
        stack_.pop_back();
        return val;
    }
    
    Value& top() {
        if (stack_.empty()) {
            throw std::runtime_error("Stack empty");
        }
        return stack_.back();
    }
    
    void executeInstruction(const Instruction& instr) {
        switch (instr.op) {
            case OpCode::NOP:
                break;
                
            case OpCode::CONST:
                push(module_->getConstant(instr.operand));
                break;
                
            case OpCode::LOAD:
                if (instr.operand >= 0 && instr.operand < static_cast<int>(locals_.size())) {
                    push(locals_[instr.operand]);
                } else {
                    push(Value::nil());
                }
                break;
                
            case OpCode::STORE:
                while (locals_.size() <= static_cast<size_t>(instr.operand)) {
                    locals_.push_back(Value::nil());
                }
                locals_[instr.operand] = pop();
                break;
                
            case OpCode::POP:
                pop();
                break;
                
            case OpCode::DUP:
                push(top());
                break;
                
            case OpCode::SWAP: {
                if (stack_.size() >= 2) {
                    std::swap(stack_[stack_.size()-1], stack_[stack_.size()-2]);
                }
                break;
            }
                
            case OpCode::ADD: {
                Value b = pop();
                Value a = pop();
                if (a.isNumeric() && b.isNumeric()) {
                    if (a.isInt() && b.isInt()) {
                        push(Value::integer(a.asInt() + b.asInt()));
                    } else {
                        push(Value::floating(a.asNumber() + b.asNumber()));
                    }
                } else if (a.isString() && b.isString()) {
                    push(Value::string(a.asString() + b.asString()));
                }
                break;
            }
                
            case OpCode::SUB: {
                Value b = pop();
                Value a = pop();
                if (a.isInt() && b.isInt()) {
                    push(Value::integer(a.asInt() - b.asInt()));
                } else {
                    push(Value::floating(a.asNumber() - b.asNumber()));
                }
                break;
            }
                
            case OpCode::MUL: {
                Value b = pop();
                Value a = pop();
                if (a.isInt() && b.isInt()) {
                    push(Value::integer(a.asInt() * b.asInt()));
                } else {
                    push(Value::floating(a.asNumber() * b.asNumber()));
                }
                break;
            }
                
            case OpCode::DIV: {
                Value b = pop();
                Value a = pop();
                double divisor = b.asNumber();
                if (divisor == 0) {
                    throw std::runtime_error("قسمة على صفر");
                }
                push(Value::floating(a.asNumber() / divisor));
                break;
            }
                
            case OpCode::MOD: {
                Value b = pop();
                Value a = pop();
                if (a.isInt() && b.isInt() && b.asInt() != 0) {
                    push(Value::integer(a.asInt() % b.asInt()));
                }
                break;
            }
                
            case OpCode::NEG: {
                Value a = pop();
                if (a.isInt()) {
                    push(Value::integer(-a.asInt()));
                } else {
                    push(Value::floating(-a.asNumber()));
                }
                break;
            }
                
            case OpCode::EQ: {
                Value b = pop();
                Value a = pop();
                if (a.type == b.type) {
                    if (a.isInt()) push(Value::boolean(a.asInt() == b.asInt()));
                    else if (a.isFloat()) push(Value::boolean(a.asFloat() == b.asFloat()));
                    else if (a.isBool()) push(Value::boolean(a.asBool() == b.asBool()));
                    else if (a.isString()) push(Value::boolean(a.asString() == b.asString()));
                    else push(Value::boolean(false));
                } else {
                    push(Value::boolean(false));
                }
                break;
            }
                
            case OpCode::NE: {
                Value b = pop();
                Value a = pop();
                push(Value::boolean(a.data != b.data));
                break;
            }
                
            case OpCode::LT: {
                Value b = pop();
                Value a = pop();
                push(Value::boolean(a.asNumber() < b.asNumber()));
                break;
            }
                
            case OpCode::LE: {
                Value b = pop();
                Value a = pop();
                push(Value::boolean(a.asNumber() <= b.asNumber()));
                break;
            }
                
            case OpCode::GT: {
                Value b = pop();
                Value a = pop();
                push(Value::boolean(a.asNumber() > b.asNumber()));
                break;
            }
                
            case OpCode::GE: {
                Value b = pop();
                Value a = pop();
                push(Value::boolean(a.asNumber() >= b.asNumber()));
                break;
            }
                
            case OpCode::NOT: {
                Value a = pop();
                push(Value::boolean(!a.asBool()));
                break;
            }
                
            case OpCode::AND: {
                Value b = pop();
                Value a = pop();
                push(Value::boolean(a.asBool() && b.asBool()));
                break;
            }
                
            case OpCode::OR: {
                Value b = pop();
                Value a = pop();
                push(Value::boolean(a.asBool() || b.asBool()));
                break;
            }
                
            case OpCode::JMP:
                ip_ = instr.operand - 1;  // -1 لأن ip_++ سيُضاف
                break;
                
            case OpCode::JMP_IF: {
                Value cond = pop();
                if (cond.asBool()) {
                    ip_ = instr.operand - 1;
                }
                break;
            }
                
            case OpCode::JMP_IF_NOT: {
                Value cond = pop();
                if (!cond.asBool()) {
                    ip_ = instr.operand - 1;
                }
                break;
            }
                
            case OpCode::CALL: {
                // (AR) تنفيذ استدعاء الدوال - operand يحدد عنوان الدالة أو عدد الوسائط
                // (EN) Function call execution - operand specifies function address
                int targetAddress = instr.operand;
                
                // (AR) حفظ إطار الاستدعاء الحالي
                // (EN) Save current call frame
                CallFrame frame;
                frame.returnAddress = ip_ + 1;  // العودة إلى التعليمة التالية / Return to next instruction
                frame.basePointer = static_cast<int>(locals_.size());
                frame.functionName = "";  // (AR) يمكن إضافة اسم لاحقاً / Can add name later
                frames_.push_back(frame);
                
                // (AR) القفز لعنوان الدالة (-1 لأن ip_++ سيُضاف)
                // (EN) Jump to function address (-1 because ip_++ will be added)
                ip_ = targetAddress - 1;
                break;
            }
                
            case OpCode::RET:
                if (!frames_.empty()) {
                    CallFrame frame = frames_.back();
                    frames_.pop_back();

                    // (AR) استرجاع مساحة المتغيرات المحلية الخاصة بالدالة المُستدعاة
                    // (EN) Restore caller local area by dropping callee locals
                    if (frame.basePointer >= 0 &&
                        static_cast<size_t>(frame.basePointer) <= locals_.size()) {
                        locals_.resize(static_cast<size_t>(frame.basePointer));
                    }

                    ip_ = frame.returnAddress - 1;
                } else {
                    running_ = false;
                }
                break;
                
            case OpCode::PRINT: {
                Value val = pop();
                std::cout << val.toString() << "\n";
                break;
            }
                
            case OpCode::HALT:
                running_ = false;
                break;
        }
    }
};

// =============================================================================
// مجمّع بسيط (Simple Assembler)
// =============================================================================

/**
 * مجمّع بسيط لـ bytecode
 * يحوّل تعليمات نصية إلى bytecode
 */
class SimpleAssembler {
public:
    BytecodeModule assemble(const std::vector<std::string>& lines) {
        BytecodeModule module;
        
        for (const auto& line : lines) {
            if (line.empty() || line[0] == '#') continue;
            
            auto parts = split(line);
            if (parts.empty()) continue;
            
            std::string op = parts[0];
            int operand = parts.size() > 1 ? std::stoi(parts[1]) : 0;
            
            if (op == "PUSH" || op == "CONST") {
                int idx = module.addConstant(Value::integer(operand));
                module.emit(OpCode::CONST, idx);
            }
            else if (op == "LOAD") module.emit(OpCode::LOAD, operand);
            else if (op == "STORE") module.emit(OpCode::STORE, operand);
            else if (op == "POP") module.emit(OpCode::POP);
            else if (op == "ADD") module.emit(OpCode::ADD);
            else if (op == "SUB") module.emit(OpCode::SUB);
            else if (op == "MUL") module.emit(OpCode::MUL);
            else if (op == "DIV") module.emit(OpCode::DIV);
            else if (op == "EQ") module.emit(OpCode::EQ);
            else if (op == "LT") module.emit(OpCode::LT);
            else if (op == "GT") module.emit(OpCode::GT);
            else if (op == "JMP") module.emit(OpCode::JMP, operand);
            else if (op == "JMP_IF") module.emit(OpCode::JMP_IF, operand);
            else if (op == "JMP_IF_NOT") module.emit(OpCode::JMP_IF_NOT, operand);
            else if (op == "PRINT") module.emit(OpCode::PRINT);
            else if (op == "HALT") module.emit(OpCode::HALT);
        }
        
        return module;
    }
    
private:
    std::vector<std::string> split(const std::string& s) {
        std::vector<std::string> parts;
        std::istringstream iss(s);
        std::string part;
        while (iss >> part) {
            parts.push_back(part);
        }
        return parts;
    }
};

} // namespace sad::vm

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::vm;

void* sad_vm_new() {
    return new VMInterpreter();
}

void sad_vm_free(void* vm) {
    delete static_cast<VMInterpreter*>(vm);
}

void* sad_bytecode_module_new() {
    return new BytecodeModule();
}

void sad_bytecode_module_free(void* module) {
    delete static_cast<BytecodeModule*>(module);
}

int sad_bytecode_emit(void* module, int opcode, int operand) {
    if (!module) return -1;
    auto* m = static_cast<BytecodeModule*>(module);
    return m->emit(static_cast<OpCode>(opcode), operand);
}

int sad_bytecode_add_int_constant(void* module, int64_t value) {
    if (!module) return -1;
    auto* m = static_cast<BytecodeModule*>(module);
    return m->addConstant(Value::integer(value));
}

int sad_vm_execute(void* vm, void* module) {
    if (!vm || !module) return -1;
    auto* v = static_cast<VMInterpreter*>(vm);
    auto* m = static_cast<BytecodeModule*>(module);
    v->execute(*m);
    return 0;
}

uint64_t sad_vm_instruction_count(void* vm) {
    if (!vm) return 0;
    return static_cast<VMInterpreter*>(vm)->instructionCount();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef VM_INTERPRETER_TEST

#include <cassert>

void testBasicArithmetic() {
    sad::vm::BytecodeModule module;
    sad::vm::VMInterpreter vm;
    
    // 5 + 3 = 8
    int c1 = module.addConstant(sad::vm::Value::integer(5));
    int c2 = module.addConstant(sad::vm::Value::integer(3));
    
    module.emit(sad::vm::OpCode::CONST, c1);
    module.emit(sad::vm::OpCode::CONST, c2);
    module.emit(sad::vm::OpCode::ADD);
    module.emit(sad::vm::OpCode::HALT);
    
    auto result = vm.execute(module);
    
    assert(result.isInt());
    assert(result.asInt() == 8);
    
    std::cout << "✅ اختبار الحساب الأساسي نجح!\n";
}

void testConditionalJump() {
    sad::vm::BytecodeModule module;
    sad::vm::VMInterpreter vm;
    
    // إذا 5 > 3 اطبع 1 وإلا اطبع 0
    int c5 = module.addConstant(sad::vm::Value::integer(5));
    int c3 = module.addConstant(sad::vm::Value::integer(3));
    int c1 = module.addConstant(sad::vm::Value::integer(1));
    int c0 = module.addConstant(sad::vm::Value::integer(0));
    
    module.emit(sad::vm::OpCode::CONST, c5);  // 0
    module.emit(sad::vm::OpCode::CONST, c3);  // 1
    module.emit(sad::vm::OpCode::GT);         // 2: 5 > 3 = true
    module.emit(sad::vm::OpCode::JMP_IF_NOT, 7); // 3: إذا خطأ اقفز لـ 7
    module.emit(sad::vm::OpCode::CONST, c1);  // 4: الفرع الصحيح
    module.emit(sad::vm::OpCode::JMP, 8);     // 5: اقفز للنهاية
    module.emit(sad::vm::OpCode::CONST, c0);  // 6: الفرع الخطأ (لن يُنفذ)
    module.emit(sad::vm::OpCode::HALT);       // 7
    
    auto result = vm.execute(module);
    
    assert(result.isInt());
    assert(result.asInt() == 1);
    
    std::cout << "✅ اختبار القفز الشرطي نجح!\n";
}

void testLocalVariables() {
    sad::vm::BytecodeModule module;
    sad::vm::VMInterpreter vm;
    
    // x = 10; y = 20; z = x + y
    int c10 = module.addConstant(sad::vm::Value::integer(10));
    int c20 = module.addConstant(sad::vm::Value::integer(20));
    
    module.emit(sad::vm::OpCode::CONST, c10);
    module.emit(sad::vm::OpCode::STORE, 0);   // x = 10
    module.emit(sad::vm::OpCode::CONST, c20);
    module.emit(sad::vm::OpCode::STORE, 1);   // y = 20
    module.emit(sad::vm::OpCode::LOAD, 0);    // تحميل x
    module.emit(sad::vm::OpCode::LOAD, 1);    // تحميل y
    module.emit(sad::vm::OpCode::ADD);        // x + y
    module.emit(sad::vm::OpCode::HALT);
    
    auto result = vm.execute(module);
    
    assert(result.isInt());
    assert(result.asInt() == 30);
    
    std::cout << "✅ اختبار المتغيرات المحلية نجح!\n";
}

void testLoop() {
    sad::vm::BytecodeModule module;
    sad::vm::VMInterpreter vm;
    
    // sum = 0; i = 1; while i <= 5 { sum += i; i++ }
    int c0 = module.addConstant(sad::vm::Value::integer(0));
    int c1 = module.addConstant(sad::vm::Value::integer(1));
    int c5 = module.addConstant(sad::vm::Value::integer(5));
    
    module.emit(sad::vm::OpCode::CONST, c0);
    module.emit(sad::vm::OpCode::STORE, 0);   // sum = 0
    module.emit(sad::vm::OpCode::CONST, c1);
    module.emit(sad::vm::OpCode::STORE, 1);   // i = 1
    
    // بداية الحلقة (4)
    module.emit(sad::vm::OpCode::LOAD, 1);    // 4: تحميل i
    module.emit(sad::vm::OpCode::CONST, c5);  // 5: تحميل 5
    module.emit(sad::vm::OpCode::LE);         // 6: i <= 5
    module.emit(sad::vm::OpCode::JMP_IF_NOT, 16); // 7: إذا خطأ اخرج
    
    // جسم الحلقة
    module.emit(sad::vm::OpCode::LOAD, 0);    // 8: تحميل sum
    module.emit(sad::vm::OpCode::LOAD, 1);    // 9: تحميل i
    module.emit(sad::vm::OpCode::ADD);        // 10: sum + i
    module.emit(sad::vm::OpCode::STORE, 0);   // 11: sum = sum + i
    
    module.emit(sad::vm::OpCode::LOAD, 1);    // 12: تحميل i
    module.emit(sad::vm::OpCode::CONST, c1);  // 13: تحميل 1
    module.emit(sad::vm::OpCode::ADD);        // 14: i + 1
    module.emit(sad::vm::OpCode::STORE, 1);   // 15: i = i + 1
    
    module.emit(sad::vm::OpCode::JMP, 4);     // 16: عد للبداية
    
    module.emit(sad::vm::OpCode::LOAD, 0);    // 17: تحميل sum
    module.emit(sad::vm::OpCode::HALT);       // 18
    
    auto result = vm.execute(module);
    
    assert(result.isInt());
    assert(result.asInt() == 15);  // 1+2+3+4+5 = 15
    
    std::cout << "✅ اختبار الحلقة نجح! (1+2+3+4+5 = " << result.asInt() << ")\n";
}

void testDisassemble() {
    sad::vm::BytecodeModule module;
    
    int c5 = module.addConstant(sad::vm::Value::integer(5));
    int c3 = module.addConstant(sad::vm::Value::integer(3));
    
    module.emit(sad::vm::OpCode::CONST, c5);
    module.emit(sad::vm::OpCode::CONST, c3);
    module.emit(sad::vm::OpCode::ADD);
    module.emit(sad::vm::OpCode::PRINT);
    module.emit(sad::vm::OpCode::HALT);
    
    std::cout << "\n";
    module.disassemble();
    
    std::cout << "✅ اختبار التفكيك نجح!\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات مفسّر Bytecode VM\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicArithmetic();
    testConditionalJump();
    testLocalVariables();
    testLoop();
    testDisassemble();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // VM_INTERPRETER_TEST
