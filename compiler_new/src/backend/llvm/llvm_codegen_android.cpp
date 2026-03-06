#ifdef ENABLE_ANDROID_CODEGEN
// Android codegen is now enabled - SIR opcodes in Section 19 require these emit functions
/**
 * ==========================================================================
 * llvm_codegen_android.cpp - توليد LLVM IR لدوال Runtime أندرويد
 * LLVM IR Generation for Android Runtime Functions
 * ==========================================================================
 * 
 * الوصف (عربي):
 * --------------
 * هذا الملف يُولّد LLVM IR لاستدعاء دوال Runtime أندرويد الأصلية المُعرّفة في:
 *   runtime_new/android/
 * 
 * كل عملية في لغة ص على أندرويد تُترجم إلى استدعاء دالة C من هذا الملف.
 * مثال:
 *   نص جديد = "مرحبا"  →  sad_string_create_utf8("مرحبا", 10)
 *   مصفوفة أرقام = []  →  sad_array_create(SAD_ARRAY_I64)
 * 
 * الفئات المدعومة:
 * 1. إدارة الذاكرة (Memory) - sad_alloc, sad_free, ...
 * 2. النصوص (String) - sad_string_create, sad_string_concat, ...
 * 3. المصفوفات (Array) - sad_array_create, sad_array_push, ...
 * 4. الخرائط (Map) - sad_map_create, sad_map_put, ...
 * 5. الشبكات (Network) - sad_net_connect, sad_net_send, ...
 * 6. الخيوط (Thread) - sad_thread_create, sad_channel_send, ...
 * 7. واجهة المستخدم (UI) - sad_widget_create, sad_button, ...
 * 
 * Description (English):
 * ----------------------
 * This file generates LLVM IR to call native Android runtime functions
 * defined in runtime_new/android/
 * 
 * Each Sad language operation on Android translates to a C function call.
 * 
 * ==========================================================================
 */

#include "llvm_codegen.h"
#include <llvm/IR/DerivedTypes.h>

namespace Sad {
namespace LLVM {

using SIRInstruction = Compiler::SIR::SIRInstruction;
using SIROperand = Compiler::SIR::SIROperand;
using SIROpcode = Compiler::SIR::SIROpcode;

// ============================================================================
// دالة مساعدة لاستدعاء دوال Runtime
// Helper to emit runtime function calls
// ============================================================================

/**
 * استدعاء دالة Runtime خارجية
 * Call an external runtime function
 * 
 * @param funcName اسم الدالة / Function name
 * @param retType نوع القيمة المُرجعة / Return type
 * @param argTypes أنواع المعاملات / Argument types
 * @param argValues قيم المعاملات / Argument values
 * @return القيمة المُرجعة من الدالة / Return value
 */
llvm::Value* LLVMCodeGen::emitAndroidRuntimeCall(
    const std::string& funcName,
    llvm::Type* retType,
    const std::vector<llvm::Type*>& argTypes,
    const std::vector<llvm::Value*>& argValues)
{
    // البحث عن الدالة أو إنشاؤها / Find or create function
    llvm::Function* fn = module_->getFunction(funcName);
    if (!fn) {
        llvm::FunctionType* ft = llvm::FunctionType::get(retType, argTypes, false);
        fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, funcName, module_.get());
        
        // تحديد سمات الدالة للتحسين / Set function attributes for optimization
        fn->addFnAttr(llvm::Attribute::NoUnwind);  // لا يرمي استثناءات
    }
    
    return builder_->CreateCall(fn, argValues, funcName + "_result");
}


// ============================================================================
// 1. إدارة الذاكرة / Memory Management
// ============================================================================

/**
 * تخصيص ذاكرة
 * Allocate memory: sad_alloc(size) -> ptr
 */
llvm::Value* LLVMCodeGen::emitAndroidAlloc(std::shared_ptr<SIRInstruction> inst) {
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    
    // الحصول على معامل الحجم / Get size operand
    llvm::Value* size = resolveOperand(inst->operands[0]);
    size = builder_->CreateIntCast(size, i64Ty, false);
    
    return emitAndroidRuntimeCall("sad_alloc", ptrTy, {i64Ty}, {size});
}

/**
 * تحرير ذاكرة
 * Free memory: sad_free(ptr)
 */
llvm::Value* LLVMCodeGen::emitAndroidFree(std::shared_ptr<SIRInstruction> inst) {
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    
    llvm::Value* ptr = resolveOperand(inst->operands[0]);
    
    return emitAndroidRuntimeCall("sad_free", voidTy, {ptrTy}, {ptr});
}


// ============================================================================
// 2. النصوص / Strings
// ============================================================================

/**
 * إنشاء نص من UTF-8
 * Create string from UTF-8: sad_string_create_utf8(data, len) -> SadString*
 */
llvm::Value* LLVMCodeGen::emitAndroidStringCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* data = resolveOperand(inst->operands[0]);  // const char*
    llvm::Value* len = resolveOperand(inst->operands[1]);   // size_t
    len = builder_->CreateIntCast(len, i64Ty, false);
    
    return emitAndroidRuntimeCall("sad_string_create_utf8", ptrTy, {ptrTy, i64Ty}, {data, len});
}

/**
 * دمج نصين
 * Concatenate strings: sad_string_concat(a, b) -> SadString*
 */
llvm::Value* LLVMCodeGen::emitAndroidStringConcat(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    
    llvm::Value* a = resolveOperand(inst->operands[0]);
    llvm::Value* b = resolveOperand(inst->operands[1]);
    
    return emitAndroidRuntimeCall("sad_string_concat", ptrTy, {ptrTy, ptrTy}, {a, b});
}

/**
 * طول النص
 * String length: sad_string_length(str) -> i64
 */
llvm::Value* LLVMCodeGen::emitAndroidStringLength(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* str = resolveOperand(inst->operands[0]);
    
    return emitAndroidRuntimeCall("sad_string_length", i64Ty, {ptrTy}, {str});
}

/**
 * مقارنة نصين
 * Compare strings: sad_string_compare(a, b) -> i32
 */
llvm::Value* LLVMCodeGen::emitAndroidStringCompare(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    
    llvm::Value* a = resolveOperand(inst->operands[0]);
    llvm::Value* b = resolveOperand(inst->operands[1]);
    
    return emitAndroidRuntimeCall("sad_string_equals", i1Ty, {ptrTy, ptrTy}, {a, b});
}

/**
 * استخراج جزء من نص
 * Substring: sad_string_substr(str, start, len) -> SadString*
 */
llvm::Value* LLVMCodeGen::emitAndroidStringSubstr(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* str = resolveOperand(inst->operands[0]);
    llvm::Value* start = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i64Ty, false);
    llvm::Value* len = builder_->CreateIntCast(resolveOperand(inst->operands[2]), i64Ty, false);
    
    return emitAndroidRuntimeCall("sad_string_substring", ptrTy, {ptrTy, i64Ty, i64Ty}, {str, start, len});
}


// ============================================================================
// 3. المصفوفات / Arrays
// ============================================================================

/**
 * إنشاء مصفوفة
 * Create array: sad_array_create(element_type) -> SadArray*
 */
llvm::Value* LLVMCodeGen::emitAndroidArrayCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    
    llvm::Value* elemType = resolveOperand(inst->operands[0]);
    elemType = builder_->CreateIntCast(elemType, i32Ty, false);
    
    return emitAndroidRuntimeCall("sad_array_create", ptrTy, {i32Ty}, {elemType});
}

/**
 * إضافة عنصر للمصفوفة
 * Push to array: sad_array_push_i64(arr, value)
 */
llvm::Value* LLVMCodeGen::emitAndroidArrayPush(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    
    llvm::Value* arr = resolveOperand(inst->operands[0]);
    llvm::Value* val = resolveOperand(inst->operands[1]);
    val = builder_->CreateIntCast(val, i64Ty, false);
    
    return emitAndroidRuntimeCall("sad_array_push_i64", i1Ty, {ptrTy, i64Ty}, {arr, val});
}

/**
 * الحصول على عنصر من المصفوفة
 * Get from array: sad_array_get_i64(arr, index) -> i64
 */
llvm::Value* LLVMCodeGen::emitAndroidArrayGet(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* arr = resolveOperand(inst->operands[0]);
    llvm::Value* idx = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i64Ty, false);
    
    return emitAndroidRuntimeCall("sad_array_get_i64", i64Ty, {ptrTy, i64Ty}, {arr, idx});
}

/**
 * طول المصفوفة
 * Array length: sad_array_length(arr) -> i64
 */
llvm::Value* LLVMCodeGen::emitAndroidArrayLength(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* arr = resolveOperand(inst->operands[0]);
    
    return emitAndroidRuntimeCall("sad_array_length", i64Ty, {ptrTy}, {arr});
}


// ============================================================================
// 4. الخرائط / Maps
// ============================================================================

/**
 * إنشاء خريطة
 * Create map: sad_map_create(key_type, value_type) -> SadMap*
 */
llvm::Value* LLVMCodeGen::emitAndroidMapCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    
    llvm::Value* keyType = builder_->CreateIntCast(resolveOperand(inst->operands[0]), i32Ty, false);
    llvm::Value* valType = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i32Ty, false);
    
    return emitAndroidRuntimeCall("sad_map_create", ptrTy, {i32Ty, i32Ty}, {keyType, valType});
}

/**
 * إضافة/تحديث قيمة في الخريطة
 * Set in map: sad_map_put_str_i64(map, key, value)
 */
llvm::Value* LLVMCodeGen::emitAndroidMapSet(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    
    llvm::Value* map = resolveOperand(inst->operands[0]);
    llvm::Value* key = resolveOperand(inst->operands[1]);  // SadString*
    llvm::Value* val = builder_->CreateIntCast(resolveOperand(inst->operands[2]), i64Ty, false);
    
    return emitAndroidRuntimeCall("sad_map_put_str_i64", i1Ty, {ptrTy, ptrTy, i64Ty}, {map, key, val});
}

/**
 * الحصول على قيمة من الخريطة
 * Get from map: sad_map_get_str_i64(map, key) -> i64
 */
llvm::Value* LLVMCodeGen::emitAndroidMapGet(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* map = resolveOperand(inst->operands[0]);
    llvm::Value* key = resolveOperand(inst->operands[1]);
    
    return emitAndroidRuntimeCall("sad_map_get_str_i64", i64Ty, {ptrTy, ptrTy}, {map, key});
}


// ============================================================================
// 5. الشبكات / Network
// ============================================================================

/**
 * إنشاء اتصال TCP
 * Create TCP connection: sad_net_tcp_connect(host, port) -> SadSocket*
 */
llvm::Value* LLVMCodeGen::emitAndroidNetConnect(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    
    llvm::Value* host = resolveOperand(inst->operands[0]);  // const char*
    llvm::Value* port = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i32Ty, false);
    
    return emitAndroidRuntimeCall("sad_net_tcp_connect", ptrTy, {ptrTy, i32Ty}, {host, port});
}

/**
 * إرسال بيانات
 * Send data: sad_net_send(socket, data, len) -> i64
 */
llvm::Value* LLVMCodeGen::emitAndroidNetSend(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* socket = resolveOperand(inst->operands[0]);
    llvm::Value* data = resolveOperand(inst->operands[1]);
    llvm::Value* len = builder_->CreateIntCast(resolveOperand(inst->operands[2]), i64Ty, false);
    
    return emitAndroidRuntimeCall("sad_net_send", i64Ty, {ptrTy, ptrTy, i64Ty}, {socket, data, len});
}

/**
 * استقبال بيانات
 * Receive data: sad_net_recv(socket, buffer, max_len) -> i64
 */
llvm::Value* LLVMCodeGen::emitAndroidNetRecv(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* socket = resolveOperand(inst->operands[0]);
    llvm::Value* buffer = resolveOperand(inst->operands[1]);
    llvm::Value* maxLen = builder_->CreateIntCast(resolveOperand(inst->operands[2]), i64Ty, false);
    
    return emitAndroidRuntimeCall("sad_net_recv", i64Ty, {ptrTy, ptrTy, i64Ty}, {socket, buffer, maxLen});
}

/**
 * إغلاق اتصال
 * Close connection: sad_net_close(socket)
 */
llvm::Value* LLVMCodeGen::emitAndroidNetClose(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    
    llvm::Value* socket = resolveOperand(inst->operands[0]);
    
    return emitAndroidRuntimeCall("sad_net_close", voidTy, {ptrTy}, {socket});
}


// ============================================================================
// 6. الخيوط / Threading
// ============================================================================

/**
 * إنشاء خيط
 * Create thread: sad_thread_create(func, arg) -> SadThread*
 */
llvm::Value* LLVMCodeGen::emitAndroidThreadCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    
    llvm::Value* func = resolveOperand(inst->operands[0]);  // function pointer
    llvm::Value* arg = resolveOperand(inst->operands[1]);   // void*
    
    return emitAndroidRuntimeCall("sad_thread_create", ptrTy, {ptrTy, ptrTy}, {func, arg});
}

/**
 * انتظار خيط
 * Join thread: sad_thread_join(thread) -> void*
 */
llvm::Value* LLVMCodeGen::emitAndroidThreadJoin(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    
    llvm::Value* thread = resolveOperand(inst->operands[0]);
    
    return emitAndroidRuntimeCall("sad_thread_join", ptrTy, {ptrTy}, {thread});
}

/**
 * إنشاء قناة
 * Create channel: sad_channel_create(capacity) -> SadChannel*
 */
llvm::Value* LLVMCodeGen::emitAndroidChannelCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    
    llvm::Value* capacity = builder_->CreateIntCast(resolveOperand(inst->operands[0]), i64Ty, false);
    
    return emitAndroidRuntimeCall("sad_channel_create", ptrTy, {i64Ty}, {capacity});
}

/**
 * إرسال في قناة
 * Send to channel: sad_channel_send(channel, value) -> bool
 */
llvm::Value* LLVMCodeGen::emitAndroidChannelSend(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    
    llvm::Value* channel = resolveOperand(inst->operands[0]);
    llvm::Value* value = resolveOperand(inst->operands[1]);
    
    return emitAndroidRuntimeCall("sad_channel_send", i1Ty, {ptrTy, ptrTy}, {channel, value});
}

/**
 * استقبال من قناة
 * Receive from channel: sad_channel_recv(channel) -> void*
 */
llvm::Value* LLVMCodeGen::emitAndroidChannelRecv(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    
    llvm::Value* channel = resolveOperand(inst->operands[0]);
    
    return emitAndroidRuntimeCall("sad_channel_recv", ptrTy, {ptrTy}, {channel});
}


// ============================================================================
// 7. واجهة المستخدم / UI
// ============================================================================

/**
 * إنشاء تطبيق
 * Create app: sad_app_create() -> SadApp*
 */
llvm::Value* LLVMCodeGen::emitAndroidAppCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    return emitAndroidRuntimeCall("sad_app_create", ptrTy, {}, {});
}

/**
 * إنشاء واجهة
 * Create widget: sad_widget_create(type) -> SadWidget*
 */
llvm::Value* LLVMCodeGen::emitAndroidWidgetCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    
    llvm::Value* type = builder_->CreateIntCast(resolveOperand(inst->operands[0]), i32Ty, false);
    
    return emitAndroidRuntimeCall("sad_widget_create", ptrTy, {i32Ty}, {type});
}

/**
 * إنشاء نص
 * Create text: sad_text(text, font_size, color) -> SadWidget*
 */
llvm::Value* LLVMCodeGen::emitAndroidText(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* floatTy = llvm::Type::getFloatTy(*context_);
    
    llvm::Value* text = resolveOperand(inst->operands[0]);
    llvm::Value* fontSize = resolveOperand(inst->operands[1]);
    llvm::Value* color = resolveOperand(inst->operands[2]);  // SadColor*
    
    return emitAndroidRuntimeCall("sad_text", ptrTy, {ptrTy, floatTy, ptrTy}, {text, fontSize, color});
}

/**
 * إنشاء زر
 * Create button: sad_button(text, callback, user_data) -> SadWidget*
 */
llvm::Value* LLVMCodeGen::emitAndroidButton(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    
    llvm::Value* text = resolveOperand(inst->operands[0]);
    llvm::Value* callback = resolveOperand(inst->operands[1]);
    llvm::Value* userData = resolveOperand(inst->operands[2]);
    
    return emitAndroidRuntimeCall("sad_button", ptrTy, {ptrTy, ptrTy, ptrTy}, {text, callback, userData});
}

/**
 * إنشاء حقل إدخال
 * Create input: sad_input(placeholder, callback, user_data) -> SadWidget*
 */
llvm::Value* LLVMCodeGen::emitAndroidInput(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    
    llvm::Value* placeholder = resolveOperand(inst->operands[0]);
    llvm::Value* callback = resolveOperand(inst->operands[1]);
    llvm::Value* userData = resolveOperand(inst->operands[2]);
    
    return emitAndroidRuntimeCall("sad_input", ptrTy, {ptrTy, ptrTy, ptrTy}, {placeholder, callback, userData});
}

/**
 * إنشاء عمود
 * Create column: sad_column(main_align, cross_align, spacing) -> SadWidget*
 */
llvm::Value* LLVMCodeGen::emitAndroidColumn(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* floatTy = llvm::Type::getFloatTy(*context_);
    
    llvm::Value* mainAlign = builder_->CreateIntCast(resolveOperand(inst->operands[0]), i32Ty, false);
    llvm::Value* crossAlign = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i32Ty, false);
    llvm::Value* spacing = resolveOperand(inst->operands[2]);
    
    return emitAndroidRuntimeCall("sad_column", ptrTy, {i32Ty, i32Ty, floatTy}, {mainAlign, crossAlign, spacing});
}

/**
 * إنشاء صف
 * Create row: sad_row(main_align, cross_align, spacing) -> SadWidget*
 */
llvm::Value* LLVMCodeGen::emitAndroidRow(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* floatTy = llvm::Type::getFloatTy(*context_);
    
    llvm::Value* mainAlign = builder_->CreateIntCast(resolveOperand(inst->operands[0]), i32Ty, false);
    llvm::Value* crossAlign = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i32Ty, false);
    llvm::Value* spacing = resolveOperand(inst->operands[2]);
    
    return emitAndroidRuntimeCall("sad_row", ptrTy, {i32Ty, i32Ty, floatTy}, {mainAlign, crossAlign, spacing});
}

/**
 * إضافة طفل لواجهة
 * Add child: sad_widget_add_child(parent, child) -> bool
 */
llvm::Value* LLVMCodeGen::emitAndroidWidgetAddChild(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    
    llvm::Value* parent = resolveOperand(inst->operands[0]);
    llvm::Value* child = resolveOperand(inst->operands[1]);
    
    return emitAndroidRuntimeCall("sad_widget_add_child", i1Ty, {ptrTy, ptrTy}, {parent, child});
}

/**
 * تعيين نص الواجهة
 * Set widget text: sad_widget_set_text(widget, text)
 */
llvm::Value* LLVMCodeGen::emitAndroidWidgetSetText(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* text = resolveOperand(inst->operands[1]);
    
    return emitAndroidRuntimeCall("sad_widget_set_text", voidTy, {ptrTy, ptrTy}, {widget, text});
}

/**
 * إنشاء شاشة
 * Create screen: sad_screen_create(name) -> SadScreen*
 */
llvm::Value* LLVMCodeGen::emitAndroidScreenCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    
    llvm::Value* name = resolveOperand(inst->operands[0]);
    
    return emitAndroidRuntimeCall("sad_screen_create", ptrTy, {ptrTy}, {name});
}

/**
 * تعيين جذر الشاشة
 * Set screen root: sad_screen_set_root(screen, root)
 */
llvm::Value* LLVMCodeGen::emitAndroidScreenSetRoot(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    
    llvm::Value* screen = resolveOperand(inst->operands[0]);
    llvm::Value* root = resolveOperand(inst->operands[1]);
    
    return emitAndroidRuntimeCall("sad_screen_set_root", voidTy, {ptrTy, ptrTy}, {screen, root});
}

/**
 * إضافة شاشة للتطبيق
 * Add screen to app: sad_app_add_screen(app, screen) -> bool
 */
llvm::Value* LLVMCodeGen::emitAndroidAppAddScreen(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    
    llvm::Value* app = resolveOperand(inst->operands[0]);
    llvm::Value* screen = resolveOperand(inst->operands[1]);
    
    return emitAndroidRuntimeCall("sad_app_add_screen", i1Ty, {ptrTy, ptrTy}, {app, screen});
}

/**
 * الانتقال إلى شاشة
 * Navigate to screen: sad_app_navigate_to(app, name) -> bool
 */
llvm::Value* LLVMCodeGen::emitAndroidAppNavigate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    
    llvm::Value* app = resolveOperand(inst->operands[0]);
    llvm::Value* name = resolveOperand(inst->operands[1]);
    
    return emitAndroidRuntimeCall("sad_app_navigate_to", i1Ty, {ptrTy, ptrTy}, {app, name});
}


// ============================================================================
// 8. الطباعة والتسجيل / Print and Logging
// ============================================================================

/**
 * طباعة نص (للتسجيل في logcat)
 * Print text (for logcat): sad_print(text)
 */
llvm::Value* LLVMCodeGen::emitAndroidPrint(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    
    llvm::Value* text = resolveOperand(inst->operands[0]);
    
    return emitAndroidRuntimeCall("sad_print", voidTy, {ptrTy}, {text});
}

/**
 * طباعة سطر
 * Print line: sad_println(text)
 */
llvm::Value* LLVMCodeGen::emitAndroidPrintln(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    
    llvm::Value* text = resolveOperand(inst->operands[0]);
    
    return emitAndroidRuntimeCall("sad_println", voidTy, {ptrTy}, {text});
}


// ============================================================================
// الدوال الإضافية المطلوبة للتوافق مع SIR Opcodes
// Additional functions required for SIR Opcode compatibility
// ============================================================================

// -- String --
llvm::Value* LLVMCodeGen::emitAndroidStringFree(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* str = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_string_free", voidTy, {ptrTy}, {str});
}

// -- Array --
llvm::Value* LLVMCodeGen::emitAndroidArraySet(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* arr = resolveOperand(inst->operands[0]);
    llvm::Value* idx = builder_->CreateIntCast(resolveOperand(inst->operands[1]), i64Ty, false);
    llvm::Value* val = resolveOperand(inst->operands[2]);
    return emitAndroidRuntimeCall("sad_array_set", voidTy, {ptrTy, i64Ty, ptrTy}, {arr, idx, val});
}

llvm::Value* LLVMCodeGen::emitAndroidArrayPop(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* arr = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_array_pop", ptrTy, {ptrTy}, {arr});
}

llvm::Value* LLVMCodeGen::emitAndroidArrayFree(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* arr = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_array_free", voidTy, {ptrTy}, {arr});
}

// -- Map --
llvm::Value* LLVMCodeGen::emitAndroidMapHas(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    llvm::Value* map = resolveOperand(inst->operands[0]);
    llvm::Value* key = resolveOperand(inst->operands[1]);
    return emitAndroidRuntimeCall("sad_map_has", i1Ty, {ptrTy, ptrTy}, {map, key});
}

llvm::Value* LLVMCodeGen::emitAndroidMapDelete(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    llvm::Value* map = resolveOperand(inst->operands[0]);
    llvm::Value* key = resolveOperand(inst->operands[1]);
    return emitAndroidRuntimeCall("sad_map_delete", i1Ty, {ptrTy, ptrTy}, {map, key});
}

llvm::Value* LLVMCodeGen::emitAndroidMapSize(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* map = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_map_size", i64Ty, {ptrTy}, {map});
}

llvm::Value* LLVMCodeGen::emitAndroidMapFree(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* map = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_map_free", voidTy, {ptrTy}, {map});
}

// -- Network extras --
llvm::Value* LLVMCodeGen::emitAndroidHttpRequest(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* method = resolveOperand(inst->operands[0]);
    llvm::Value* url = resolveOperand(inst->operands[1]);
    llvm::Value* body = (inst->operands.size() > 2) ? resolveOperand(inst->operands[2]) : 
                        llvm::ConstantPointerNull::get(llvm::PointerType::getUnqual(*context_));
    return emitAndroidRuntimeCall("sad_http_request", ptrTy, {ptrTy, ptrTy, ptrTy}, {method, url, body});
}

llvm::Value* LLVMCodeGen::emitAndroidWsConnect(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* url = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_ws_connect", ptrTy, {ptrTy}, {url});
}

llvm::Value* LLVMCodeGen::emitAndroidWsSend(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i1Ty = llvm::Type::getInt1Ty(*context_);
    llvm::Value* ws = resolveOperand(inst->operands[0]);
    llvm::Value* msg = resolveOperand(inst->operands[1]);
    return emitAndroidRuntimeCall("sad_ws_send", i1Ty, {ptrTy, ptrTy}, {ws, msg});
}

llvm::Value* LLVMCodeGen::emitAndroidWsRecv(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    llvm::Value* ws = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_ws_recv", ptrTy, {ptrTy}, {ws});
}

// -- Threading extras --
llvm::Value* LLVMCodeGen::emitAndroidMutexCreate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    return emitAndroidRuntimeCall("sad_mutex_create", ptrTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitAndroidMutexLock(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* mtx = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_mutex_lock", voidTy, {ptrTy}, {mtx});
}

llvm::Value* LLVMCodeGen::emitAndroidMutexUnlock(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* mtx = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_mutex_unlock", voidTy, {ptrTy}, {mtx});
}

// -- UI functions (using unified widget approach) --
llvm::Value* LLVMCodeGen::emitAndroidUiInit(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    return emitAndroidRuntimeCall("sad_ui_init", ptrTy, {}, {});
}

llvm::Value* LLVMCodeGen::emitAndroidUiCreateWidget(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    llvm::Value* type = builder_->CreateIntCast(resolveOperand(inst->operands[0]), i32Ty, false);
    return emitAndroidRuntimeCall("sad_widget_create", ptrTy, {i32Ty}, {type});
}

llvm::Value* LLVMCodeGen::emitAndroidUiSetText(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* text = resolveOperand(inst->operands[1]);
    return emitAndroidRuntimeCall("sad_widget_set_text", voidTy, {ptrTy, ptrTy}, {widget, text});
}

llvm::Value* LLVMCodeGen::emitAndroidUiSetCallback(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    llvm::Value* callback = resolveOperand(inst->operands[1]);
    return emitAndroidRuntimeCall("sad_widget_set_callback", voidTy, {ptrTy, ptrTy}, {widget, callback});
}

llvm::Value* LLVMCodeGen::emitAndroidUiShow(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_widget_show", voidTy, {ptrTy}, {widget});
}

llvm::Value* LLVMCodeGen::emitAndroidUiHide(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* widget = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_widget_hide", voidTy, {ptrTy}, {widget});
}

llvm::Value* LLVMCodeGen::emitAndroidUiUpdate(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* app = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_ui_update", voidTy, {ptrTy}, {app});
}

llvm::Value* LLVMCodeGen::emitAndroidUiRun(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    llvm::Value* app = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_ui_run", i32Ty, {ptrTy}, {app});
}

// -- Logging --
llvm::Value* LLVMCodeGen::emitAndroidLog(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* i32Ty = llvm::Type::getInt32Ty(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* level = builder_->CreateIntCast(resolveOperand(inst->operands[0]), i32Ty, false);
    llvm::Value* tag = resolveOperand(inst->operands[1]);
    llvm::Value* msg = resolveOperand(inst->operands[2]);
    return emitAndroidRuntimeCall("sad_log", voidTy, {i32Ty, ptrTy, ptrTy}, {level, tag, msg});
}

llvm::Value* LLVMCodeGen::emitAndroidPrint(std::shared_ptr<SIRInstruction> inst) {
    auto* ptrTy = llvm::PointerType::getUnqual(*context_);
    auto* voidTy = llvm::Type::getVoidTy(*context_);
    llvm::Value* msg = resolveOperand(inst->operands[0]);
    return emitAndroidRuntimeCall("sad_print", voidTy, {ptrTy}, {msg});
}

// NOTE: Unified UI functions moved to llvm_codegen_ui.cpp (not conditional on ENABLE_ANDROID_CODEGEN)

} // namespace LLVM
} // namespace Sad
#endif // ENABLE_ANDROID_CODEGEN


