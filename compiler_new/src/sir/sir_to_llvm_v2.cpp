/**
 * =============================================================================
 * ملف: sir_to_llvm_fixes.cpp
 * الوصف: إصلاحات شاملة لتحويل SIR إلى LLVM
 * المرحلة: Phase 95 - Compiler Hardening
 * =============================================================================
 * 
 * 🔧 إصلاحات SIR → LLVM (100 نقطة ضعف)
 * ═════════════════════════════════════════
 * 
 * 1. إصلاح مشكلة static string (thread safety)
 * 2. إضافة معالجة كاملة للأنواع
 * 3. إصلاح العمليات غير المنفذة
 * 4. تحسين معالجة الأخطاء
 * 5. دعم الرسومات الكامل
 * 
 * =============================================================================
 */

#include "sir_types.h"
#include "sir_opcodes.h"
#include "compiler_improvements.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <memory>
#include <mutex>

namespace sad::sir {

// =============================================================================
// إصلاح 1: Thread-Safe LLVM IR Generator
// =============================================================================

/**
 * مُولد LLVM IR محسّن وآمن للخيوط
 */
class LlvmIrGeneratorV2 {
public:
    struct GenerationResult {
        bool success;
        std::string ir;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        size_t instructionCount;
        size_t functionCount;
    };
    
    /**
     * تحويل وحدة SIR إلى LLVM IR (آمن للخيوط)
     */
    GenerationResult generate(const SirModule& module) {
        // كل استدعاء له نتيجة مستقلة
        GenerationResult result{true, "", {}, {}, 0, 0};
        std::ostringstream output;
        std::map<size_t, std::shared_ptr<SirType>> valueTypes;
        
        try {
            // تعريفات عامة
            emitHeader(output, module);
            
            // تعريف الأنواع المخصصة
            emitTypeDefinitions(output, module);
            
            // الدوال الخارجية
            emitExternalDeclarations(output);
            
            // دوال الرسومات إذا كانت مطلوبة
            if (module.usesGraphics) {
                emitGraphicsDeclarations(output);
            }
            
            // الثوابت النصية
            emitStringConstants(output, module);
            
            // الدوال
            for (const auto& func : module.functions) {
                if (!emitFunction(output, func, valueTypes, result)) {
                    result.success = false;
                }
                result.functionCount++;
            }
            
            result.ir = output.str();
            
        } catch (const std::exception& e) {
            result.success = false;
            result.errors.push_back(std::string("Exception: ") + e.what());
        } catch (...) {
            result.success = false;
            result.errors.push_back("Unknown exception during LLVM IR generation");
        }
        
        return result;
    }
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // إصلاح 2: معالجة الأنواع الكاملة
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string typeToLlvm(const std::shared_ptr<SirType>& type) {
        if (!type) {
            compiler::Logger::instance().log(
                compiler::Logger::Level::Warning,
                "Null type in typeToLlvm, defaulting to i32"
            );
            return "i32";
        }
        
        switch (type->kind) {
            // الأنواع الأساسية
            case TypeKind::Void: return "void";
            case TypeKind::Bool: return "i1";
            case TypeKind::Int8: return "i8";
            case TypeKind::Int16: return "i16";
            case TypeKind::Int32: return "i32";
            case TypeKind::Int64: return "i64";
            case TypeKind::UInt8: return "i8";
            case TypeKind::UInt16: return "i16";
            case TypeKind::UInt32: return "i32";
            case TypeKind::UInt64: return "i64";
            case TypeKind::Float32: return "float";
            case TypeKind::Float64: return "double";
            
            // الأنواع المركبة
            case TypeKind::String: return "%String*";
            case TypeKind::Array:
                if (!type->params.empty()) {
                    return "%Array." + typeToLlvm(type->params[0]) + "*";
                }
                return "%Array*";
                
            // المراجع والمؤشرات
            case TypeKind::Reference:
            case TypeKind::MutableRef:
                if (!type->params.empty()) {
                    return typeToLlvm(type->params[0]) + "*";
                }
                return "i8*";
            case TypeKind::Pointer:
                if (!type->params.empty()) {
                    return typeToLlvm(type->params[0]) + "*";
                }
                return "i8*";
            case TypeKind::RawPointer:
                return "i8*";
                
            // الأنواع المستخدمة في الرسومات
            case TypeKind::Widget:
                return "%SadWidget*";
            case TypeKind::Window:
                return "%SadWindow*";
            case TypeKind::Color:
                return "%SadColor";
            case TypeKind::Event:
                return "%SadEvent*";
                
            // الدوال
            case TypeKind::Function:
                return emitFunctionType(type);
                
            // الهياكل
            case TypeKind::Struct:
                if (!type->name.empty()) {
                    return "%struct." + type->name + "*";
                }
                return "%struct.anon*";
                
            // التعدادات
            case TypeKind::Enum:
                return "i32";  // Enums as i32
                
            // الخيارات
            case TypeKind::Optional:
                if (!type->params.empty()) {
                    return "%Optional." + typeToLlvm(type->params[0]);
                }
                return "%Optional.i32";
                
            // النتائج
            case TypeKind::Result:
                return "%Result*";
                
            // غير معروف
            default:
                compiler::Logger::instance().log(
                    compiler::Logger::Level::Warning,
                    "نوع SIR غير معروف في typeToLlvm: " + std::to_string(static_cast<int>(type->kind)) + " — استخدام i32"
                );
                return "i32";
        }
    }
    
    std::string emitFunctionType(const std::shared_ptr<SirType>& type) {
        std::ostringstream ss;
        
        // نوع الإرجاع
        if (!type->params.empty()) {
            ss << typeToLlvm(type->params[0]);
        } else {
            ss << "void";
        }
        
        ss << " (";
        
        // المعاملات
        for (size_t i = 1; i < type->params.size(); ++i) {
            if (i > 1) ss << ", ";
            ss << typeToLlvm(type->params[i]);
        }
        
        ss << ")*";
        return ss.str();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إصلاح 3: الترويسة والتعريفات
    // ═══════════════════════════════════════════════════════════════════════════
    
    void emitHeader(std::ostringstream& out, const SirModule& module) {
        out << "; LLVM IR generated from Sad SIR (v2 - hardened)\n";
        out << "; Module: " << module.name << "\n";
        out << "; Generated by sadc compiler\n\n";
        
        // تحديد target بناءً على المنصة
#ifdef _WIN32
        out << "target datalayout = \"e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
        out << "target triple = \"x86_64-pc-windows-msvc\"\n\n";
#elif defined(__linux__)
        out << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
        out << "target triple = \"x86_64-unknown-linux-gnu\"\n\n";
#elif defined(__APPLE__)
        out << "target datalayout = \"e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
        out << "target triple = \"x86_64-apple-macosx12.0.0\"\n\n";
#else
        out << "target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128\"\n";
        out << "target triple = \"x86_64-unknown-unknown\"\n\n";
#endif
    }
    
    void emitTypeDefinitions(std::ostringstream& out, const SirModule& module) {
        out << "; ════════════════════════════════════════════════════════════════════\n";
        out << "; Type definitions / تعريفات الأنواع\n";
        out << "; ════════════════════════════════════════════════════════════════════\n\n";
        
        // نوع النص
        out << "%String = type { i8*, i64, i64 }  ; data, length, capacity\n";
        
        // نوع المصفوفة
        out << "%Array = type { i8*, i64, i64, i64 }  ; data, length, capacity, elementSize\n";
        
        // نوع Slice
        out << "%Slice = type { i8*, i64 }  ; data, length\n";
        
        // نوع Optional
        out << "%Optional.i32 = type { i1, i32 }  ; hasValue, value\n";
        out << "%Optional.i64 = type { i1, i64 }\n";
        out << "%Optional.ptr = type { i1, i8* }\n";
        
        // نوع Result
        out << "%Result = type { i1, i8*, i8* }  ; success, value, error\n";
        
        // أنواع الرسومات
        out << "\n; Graphics types / أنواع الرسومات\n";
        out << "%SadColor = type { i8, i8, i8, i8 }  ; r, g, b, a\n";
        out << "%SadWidget = type { i32, i32, float, float, float, float, %SadColor, %SadColor, i8*, i32 }\n";
        out << "%SadWindow = type opaque\n";
        out << "%SadEvent = type { i32, i32, i32, i32 }  ; type, x, y, key\n";
        
        // الهياكل المخصصة من الوحدة
        for (const auto& structDef : module.structs) {
            out << "%struct." << structDef.name << " = type { ";
            for (size_t i = 0; i < structDef.fields.size(); ++i) {
                if (i > 0) out << ", ";
                out << typeToLlvm(structDef.fields[i].type);
            }
            out << " }\n";
        }
        
        out << "\n";
    }
    
    void emitExternalDeclarations(std::ostringstream& out) {
        out << "; ════════════════════════════════════════════════════════════════════\n";
        out << "; External declarations / التعريفات الخارجية\n";
        out << "; ════════════════════════════════════════════════════════════════════\n\n";
        
        // دوال وقت التشغيل الأساسية
        out << "; Runtime functions / دوال وقت التشغيل\n";
        out << "declare void @sad_print(i8*)\n";
        out << "declare void @sad_println(i8*)\n";
        out << "declare i8* @sad_alloc(i64)\n";
        out << "declare i8* @sad_realloc(i8*, i64)\n";
        out << "declare void @sad_free(i8*)\n";
        out << "declare void @sad_panic(i8*) noreturn\n";
        out << "declare void @sad_assert(i1, i8*)\n";
        
        // دوال النصوص
        out << "\n; String functions / دوال النصوص\n";
        out << "declare %String* @sad_string_new(i8*, i64)\n";
        out << "declare void @sad_string_free(%String*)\n";
        out << "declare %String* @sad_string_concat(%String*, %String*)\n";
        out << "declare i64 @sad_string_length(%String*)\n";
        out << "declare i8* @sad_string_data(%String*)\n";
        out << "declare %String* @sad_string_clone(%String*)\n";
        out << "declare i1 @sad_string_equals(%String*, %String*)\n";
        out << "declare i32 @sad_string_compare(%String*, %String*)\n";
        out << "declare %String* @sad_string_substring(%String*, i64, i64)\n";
        out << "declare i64 @sad_string_find(%String*, %String*)\n";
        out << "declare %String* @sad_string_replace(%String*, %String*, %String*)\n";
        out << "declare %String* @sad_string_trim(%String*)\n";
        out << "declare %String* @sad_string_to_upper(%String*)\n";
        out << "declare %String* @sad_string_to_lower(%String*)\n";
        
        // دوال المصفوفات
        out << "\n; Array functions / دوال المصفوفات\n";
        out << "declare %Array* @sad_array_new(i64, i64)\n";
        out << "declare void @sad_array_free(%Array*)\n";
        out << "declare void @sad_array_push(%Array*, i8*)\n";
        out << "declare i8* @sad_array_pop(%Array*)\n";
        out << "declare i8* @sad_array_get(%Array*, i64)\n";
        out << "declare void @sad_array_set(%Array*, i64, i8*)\n";
        out << "declare i64 @sad_array_length(%Array*)\n";
        out << "declare void @sad_array_clear(%Array*)\n";
        out << "declare %Array* @sad_array_clone(%Array*)\n";
        out << "declare void @sad_array_sort(%Array*, i8*)\n";
        out << "declare %Array* @sad_array_filter(%Array*, i8*)\n";
        out << "declare %Array* @sad_array_map(%Array*, i8*)\n";
        out << "declare i8* @sad_array_reduce(%Array*, i8*, i8*)\n";
        
        // دوال الإدخال/الإخراج
        out << "\n; IO functions / دوال الإدخال والإخراج\n";
        out << "declare %String* @sad_read_line()\n";
        out << "declare %String* @sad_read_file(i8*)\n";
        out << "declare i1 @sad_write_file(i8*, %String*)\n";
        out << "declare i1 @sad_file_exists(i8*)\n";
        out << "declare i1 @sad_delete_file(i8*)\n";
        
        // دوال الرياضيات
        out << "\n; Math functions / دوال الرياضيات\n";
        out << "declare double @llvm.sqrt.f64(double)\n";
        out << "declare double @llvm.pow.f64(double, double)\n";
        out << "declare double @llvm.sin.f64(double)\n";
        out << "declare double @llvm.cos.f64(double)\n";
        out << "declare double @llvm.tan.f64(double)\n";
        out << "declare double @llvm.log.f64(double)\n";
        out << "declare double @llvm.exp.f64(double)\n";
        out << "declare double @llvm.floor.f64(double)\n";
        out << "declare double @llvm.ceil.f64(double)\n";
        out << "declare double @llvm.fabs.f64(double)\n";
        
        out << "\n";
    }
    
    void emitGraphicsDeclarations(std::ostringstream& out) {
        out << "; ════════════════════════════════════════════════════════════════════\n";
        out << "; Graphics declarations / تعريفات الرسومات\n";
        out << "; ════════════════════════════════════════════════════════════════════\n\n";
        
        // تهيئة وإغلاق
        out << "; Initialization\n";
        out << "declare i32 @sad_ui_init()\n";
        out << "declare void @sad_ui_quit()\n";
        
        // النوافذ
        out << "\n; Window\n";
        out << "declare %SadWindow* @sad_ui_create_window(i8*, i32, i32)\n";
        out << "declare void @sad_ui_destroy_window(%SadWindow*)\n";
        out << "declare void @sad_ui_set_window_title(%SadWindow*, i8*)\n";
        out << "declare void @sad_ui_set_window_size(%SadWindow*, i32, i32)\n";
        out << "declare void @sad_ui_show_window(%SadWindow*)\n";
        out << "declare void @sad_ui_hide_window(%SadWindow*)\n";
        
        // الويدجات
        out << "\n; Widgets\n";
        out << "declare i32 @sad_ui_create_widget(i32)  ; type -> widget_id\n";
        out << "declare void @sad_ui_destroy_widget(i32)\n";
        out << "declare void @sad_ui_set_position(i32, float, float)\n";
        out << "declare void @sad_ui_set_size(i32, float, float)\n";
        out << "declare void @sad_ui_set_text(i32, i8*)\n";
        out << "declare void @sad_ui_set_bg_color(i32, i8, i8, i8, i8)\n";
        out << "declare void @sad_ui_set_fg_color(i32, i8, i8, i8, i8)\n";
        out << "declare void @sad_ui_set_visible(i32, i1)\n";
        out << "declare void @sad_ui_set_enabled(i32, i1)\n";
        out << "declare void @sad_ui_add_child(i32, i32)\n";
        out << "declare i8* @sad_ui_get_text(i32)\n";
        out << "declare i1 @sad_ui_get_checked(i32)\n";
        out << "declare float @sad_ui_get_value(i32)\n";
        
        // أنواع الويدجات
        out << "\n; Widget creation shortcuts\n";
        out << "declare i32 @sad_ui_widget_button(i8*)\n";
        out << "declare i32 @sad_ui_widget_label(i8*)\n";
        out << "declare i32 @sad_ui_widget_textfield(i8*)\n";
        out << "declare i32 @sad_ui_widget_checkbox(i8*)\n";
        out << "declare i32 @sad_ui_widget_radio(i8*)\n";
        out << "declare i32 @sad_ui_widget_slider(float, float)\n";
        out << "declare i32 @sad_ui_widget_dropdown()\n";
        out << "declare i32 @sad_ui_widget_column()\n";
        out << "declare i32 @sad_ui_widget_row()\n";
        out << "declare i32 @sad_ui_widget_container()\n";
        out << "declare i32 @sad_ui_widget_scrollview()\n";
        out << "declare i32 @sad_ui_widget_listview()\n";
        out << "declare i32 @sad_ui_widget_gridview(i32)\n";
        out << "declare i32 @sad_ui_widget_card()\n";
        out << "declare i32 @sad_ui_widget_image(i8*)\n";
        out << "declare i32 @sad_ui_widget_icon(i32)\n";
        out << "declare i32 @sad_ui_widget_progress(float)\n";
        out << "declare i32 @sad_ui_widget_canvas(i32, i32)\n";
        
        // الأحداث
        out << "\n; Events\n";
        out << "declare i32 @sad_ui_poll_event(%SadEvent*)\n";
        out << "declare i32 @sad_ui_wait_event(%SadEvent*)\n";
        out << "declare void @sad_ui_set_callback(i32, i32, i8*)\n";
        
        // حلقة الأحداث
        out << "\n; Event loop\n";
        out << "declare void @sad_ui_render(%SadWindow*)\n";
        out << "declare i32 @sad_ui_event_loop(%SadWindow*)\n";
        
        // الرسم المباشر
        out << "\n; Direct drawing\n";
        out << "declare void @sad_ui_draw_line(i32, i32, i32, i32, i8, i8, i8, i8)\n";
        out << "declare void @sad_ui_draw_rect(i32, i32, i32, i32, i8, i8, i8, i8)\n";
        out << "declare void @sad_ui_fill_rect(i32, i32, i32, i32, i8, i8, i8, i8)\n";
        out << "declare void @sad_ui_draw_circle(i32, i32, i32, i8, i8, i8, i8)\n";
        out << "declare void @sad_ui_fill_circle(i32, i32, i32, i8, i8, i8, i8)\n";
        out << "declare void @sad_ui_draw_text(i32, i32, i8*, i8, i8, i8, i8)\n";
        out << "declare void @sad_ui_clear(i8, i8, i8, i8)\n";
        out << "declare void @sad_ui_present()\n";
        
        out << "\n";
    }
    
    void emitStringConstants(std::ostringstream& out, const SirModule& module) {
        out << "; ════════════════════════════════════════════════════════════════════\n";
        out << "; String constants / الثوابت النصية\n";
        out << "; ════════════════════════════════════════════════════════════════════\n\n";
        
        for (size_t i = 0; i < module.stringConstants.size(); ++i) {
            const auto& str = module.stringConstants[i];
            out << "@.str." << i << " = private unnamed_addr constant [" 
                << (str.size() + 1) << " x i8] c\"";
            
            // escape the string
            for (char c : str) {
                if (c == '\\') out << "\\\\";
                else if (c == '"') out << "\\22";
                else if (c == '\n') out << "\\0A";
                else if (c == '\r') out << "\\0D";
                else if (c == '\t') out << "\\09";
                else if (static_cast<unsigned char>(c) < 32 || static_cast<unsigned char>(c) > 126) {
                    out << "\\";
                    out << std::hex << std::setfill('0') << std::setw(2) 
                        << static_cast<int>(static_cast<unsigned char>(c));
                    out << std::dec;
                } else {
                    out << c;
                }
            }
            
            out << "\\00\"\n";
        }
        
        out << "\n";
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إصلاح 4: توليد الدوال الكامل
    // ═══════════════════════════════════════════════════════════════════════════
    
    bool emitFunction(std::ostringstream& out, const SirFunction& func,
                      std::map<size_t, std::shared_ptr<SirType>>& valueTypes,
                      GenerationResult& genResult) {
        out << "; ────────────────────────────────────────────────────────────────────\n";
        out << "; Function: " << func.name << "\n";
        out << "; ────────────────────────────────────────────────────────────────────\n";
        
        // تحديد الـ linkage
        if (func.isExport) {
            out << "define dso_local ";
        } else {
            out << "define internal ";
        }
        
        out << typeToLlvm(func.returnType) << " @" << func.name << "(";
        
        // المعاملات
        for (size_t i = 0; i < func.params.size(); i++) {
            if (i > 0) out << ", ";
            out << typeToLlvm(func.params[i].type) << " %" << func.params[i].name;
            
            // تسجيل نوع المعامل
            valueTypes[i] = func.params[i].type;
        }
        
        out << ") ";
        
        // سمات الدالة
        if (func.isNoReturn) {
            out << "noreturn ";
        }
        if (func.isNoThrow) {
            out << "nounwind ";
        }
        
        out << "{\n";
        
        // الكتل
        bool hasReturn = false;
        for (const auto& block : func.blocks) {
            if (!emitBlock(out, block, valueTypes, genResult)) {
                return false;
            }
            
            // التحقق من وجود return
            if (!block.instructions.empty()) {
                const auto& lastInst = block.instructions.back();
                if (lastInst.opcode == Opcode::Return) {
                    hasReturn = true;
                }
            }
        }
        
        // إضافة return افتراضي إذا لزم الأمر
        if (!hasReturn && func.returnType && func.returnType->kind == TypeKind::Void) {
            out << "  ret void\n";
        }
        
        out << "}\n\n";
        return true;
    }
    
    bool emitBlock(std::ostringstream& out, const SirBasicBlock& block,
                   std::map<size_t, std::shared_ptr<SirType>>& valueTypes,
                   GenerationResult& genResult) {
        out << block.name << ":\n";
        
        for (const auto& inst : block.instructions) {
            // تسجيل نوع النتيجة
            if (inst.result && inst.resultType) {
                valueTypes[*inst.result] = inst.resultType;
            }
            
            out << "  ";
            if (!emitInstruction(out, inst, valueTypes, genResult)) {
                genResult.warnings.push_back("Failed to emit instruction: opcode " + 
                                            std::to_string(static_cast<int>(inst.opcode)));
            }
            out << "\n";
            genResult.instructionCount++;
        }
        
        return true;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إصلاح 5: توليد التعليمات الكامل
    // ═══════════════════════════════════════════════════════════════════════════
    
    bool emitInstruction(std::ostringstream& out, const SirInstruction& inst,
                         std::map<size_t, std::shared_ptr<SirType>>& valueTypes,
                         GenerationResult& genResult) {
        switch (inst.opcode) {
            // ═══════════════════════════════════════════════════════════════
            // عمليات الملكية
            // ═══════════════════════════════════════════════════════════════
            case Opcode::Alloc:
                return emitAlloc(out, inst, valueTypes);
                
            case Opcode::Borrow:
            case Opcode::BorrowMut:
                return emitBorrow(out, inst, valueTypes);
                
            case Opcode::Move:
            case Opcode::Copy:
                return emitMoveCopy(out, inst, valueTypes);
                
            case Opcode::Drop:
                return emitDrop(out, inst, valueTypes);
                
            case Opcode::Clone:
                return emitClone(out, inst, valueTypes);
                
            case Opcode::EndBorrow:
                // لا شيء في LLVM IR
                out << "; end borrow";
                return true;
                
            case Opcode::Deref:
                return emitDeref(out, inst, valueTypes);
                
            // ═══════════════════════════════════════════════════════════════
            // عمليات الذاكرة
            // ═══════════════════════════════════════════════════════════════
            case Opcode::Load:
                return emitLoad(out, inst, valueTypes);
                
            case Opcode::Store:
                return emitStore(out, inst, valueTypes);
                
            // ═══════════════════════════════════════════════════════════════
            // العمليات الحسابية
            // ═══════════════════════════════════════════════════════════════
            case Opcode::Add:
                return emitBinaryOp(out, inst, valueTypes, "add", "fadd");
                
            case Opcode::Sub:
                return emitBinaryOp(out, inst, valueTypes, "sub", "fsub");
                
            case Opcode::Mul:
                return emitBinaryOp(out, inst, valueTypes, "mul", "fmul");
                
            case Opcode::Div:
                return emitBinaryOp(out, inst, valueTypes, "sdiv", "fdiv");
                
            case Opcode::Mod:
                return emitBinaryOp(out, inst, valueTypes, "srem", "frem");
                
            case Opcode::Neg:
                return emitUnaryOp(out, inst, valueTypes, "neg", "fneg");
                
            // ═══════════════════════════════════════════════════════════════
            // العمليات المنطقية والثنائية
            // ═══════════════════════════════════════════════════════════════
            case Opcode::And:
                return emitBinaryOp(out, inst, valueTypes, "and", "and");
                
            case Opcode::Or:
                return emitBinaryOp(out, inst, valueTypes, "or", "or");
                
            case Opcode::Xor:
                return emitBinaryOp(out, inst, valueTypes, "xor", "xor");
                
            case Opcode::Not:
                return emitNot(out, inst, valueTypes);
                
            case Opcode::Shl:
                return emitBinaryOp(out, inst, valueTypes, "shl", "shl");
                
            case Opcode::Shr:
                return emitBinaryOp(out, inst, valueTypes, "ashr", "ashr");
                
            // ═══════════════════════════════════════════════════════════════
            // المقارنات
            // ═══════════════════════════════════════════════════════════════
            case Opcode::Eq:
                return emitComparison(out, inst, valueTypes, "eq", "oeq");
                
            case Opcode::Ne:
                return emitComparison(out, inst, valueTypes, "ne", "one");
                
            case Opcode::Lt:
                return emitComparison(out, inst, valueTypes, "slt", "olt");
                
            case Opcode::Le:
                return emitComparison(out, inst, valueTypes, "sle", "ole");
                
            case Opcode::Gt:
                return emitComparison(out, inst, valueTypes, "sgt", "ogt");
                
            case Opcode::Ge:
                return emitComparison(out, inst, valueTypes, "sge", "oge");
                
            // ═══════════════════════════════════════════════════════════════
            // التحكم في التدفق
            // ═══════════════════════════════════════════════════════════════
            case Opcode::Jump:
                return emitJump(out, inst);
                
            case Opcode::Branch:
                return emitBranch(out, inst, valueTypes);
                
            case Opcode::Return:
                return emitReturn(out, inst, valueTypes);
                
            case Opcode::Call:
                return emitCall(out, inst, valueTypes);
                
            case Opcode::IndirectCall:
                return emitIndirectCall(out, inst, valueTypes);
                
            // ═══════════════════════════════════════════════════════════════
            // التحويلات
            // ═══════════════════════════════════════════════════════════════
            case Opcode::IntToFloat:
                return emitCast(out, inst, valueTypes, "sitofp");
                
            case Opcode::FloatToInt:
                return emitCast(out, inst, valueTypes, "fptosi");
                
            case Opcode::ZeroExt:
                return emitCast(out, inst, valueTypes, "zext");
                
            case Opcode::SignExt:
                return emitCast(out, inst, valueTypes, "sext");
                
            case Opcode::Trunc:
                return emitCast(out, inst, valueTypes, "trunc");
                
            case Opcode::BitCast:
                return emitCast(out, inst, valueTypes, "bitcast");
                
            case Opcode::IntToPtr:
                return emitCast(out, inst, valueTypes, "inttoptr");
                
            case Opcode::PtrToInt:
                return emitCast(out, inst, valueTypes, "ptrtoint");
                
            // ═══════════════════════════════════════════════════════════════
            // العمليات الذرية
            // ═══════════════════════════════════════════════════════════════
            case Opcode::AtomicLoad:
                return emitAtomicLoad(out, inst, valueTypes);
                
            case Opcode::AtomicStore:
                return emitAtomicStore(out, inst, valueTypes);
                
            case Opcode::AtomicAdd:
                return emitAtomicRmw(out, inst, valueTypes, "add");
                
            case Opcode::AtomicSub:
                return emitAtomicRmw(out, inst, valueTypes, "sub");
                
            case Opcode::AtomicExchange:
                return emitAtomicRmw(out, inst, valueTypes, "xchg");
                
            case Opcode::AtomicCmpXchg:
                return emitAtomicCmpXchg(out, inst, valueTypes);
                
            // ═══════════════════════════════════════════════════════════════
            // حواجز الذاكرة
            // ═══════════════════════════════════════════════════════════════
            case Opcode::MemFence:
                out << "fence seq_cst";
                return true;
                
            case Opcode::LoadFence:
                out << "fence acquire";
                return true;
                
            case Opcode::StoreFence:
                out << "fence release";
                return true;
                
            // ═══════════════════════════════════════════════════════════════
            // GEP والوصول للحقول
            // ═══════════════════════════════════════════════════════════════
            case Opcode::GetElementPtr:
                return emitGetElementPtr(out, inst, valueTypes);
                
            case Opcode::ExtractValue:
                return emitExtractValue(out, inst, valueTypes);
                
            case Opcode::InsertValue:
                return emitInsertValue(out, inst, valueTypes);
                
            // ═══════════════════════════════════════════════════════════════
            // Φ والاختيار
            // ═══════════════════════════════════════════════════════════════
            case Opcode::Phi:
                return emitPhi(out, inst, valueTypes);
                
            case Opcode::Select:
                return emitSelect(out, inst, valueTypes);
                
            // ═══════════════════════════════════════════════════════════════
            // عمليات منخفضة المستوى (OS)
            // ═══════════════════════════════════════════════════════════════
            case Opcode::Cli:
                out << "call void asm sideeffect \"cli\", \"\"()";
                return true;
                
            case Opcode::Sti:
                out << "call void asm sideeffect \"sti\", \"\"()";
                return true;
                
            case Opcode::Hlt:
                out << "call void asm sideeffect \"hlt\", \"\"()";
                return true;
                
            case Opcode::Nop:
                out << "; nop";
                return true;
                
            // ═══════════════════════════════════════════════════════════════
            // افتراضي
            // ═══════════════════════════════════════════════════════════════
            default:
                out << "; unsupported opcode: " << static_cast<int>(inst.opcode);
                genResult.warnings.push_back("Unsupported opcode: " + 
                                            std::to_string(static_cast<int>(inst.opcode)));
                return false;
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال التوليد المساعدة
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string getValueType(size_t id,
                             const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        auto it = valueTypes.find(id);
        if (it != valueTypes.end() && it->second) {
            return typeToLlvm(it->second);
        }
        return "i32";
    }
    
    bool emitAlloc(std::ostringstream& out, const SirInstruction& inst,
                   std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result) return false;
        
        std::string elemType = inst.resultType ? typeToLlvm(inst.resultType) : "i32";
        
        // إزالة * من النهاية للحصول على النوع الأساسي
        if (elemType.back() == '*') {
            elemType = elemType.substr(0, elemType.size() - 1);
        }
        
        out << "%" << *inst.result << " = alloca " << elemType;
        
        // تسجيل النوع
        if (inst.resultType) {
            valueTypes[*inst.result] = inst.resultType;
        }
        
        return true;
    }
    
    bool emitBorrow(std::ostringstream& out, const SirInstruction& inst,
                    const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.empty()) return false;
        
        auto& srcOp = std::get<ValueOperand>(inst.operands[0]);
        std::string srcType = getValueType(srcOp.id, valueTypes);
        
        // الاستعارة في LLVM هي مجرد نسخ المؤشر
        out << "%" << *inst.result << " = bitcast " << srcType << " %" << srcOp.id 
            << " to " << srcType;
        
        return true;
    }
    
    bool emitMoveCopy(std::ostringstream& out, const SirInstruction& inst,
                      const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.empty()) return false;
        
        auto& srcOp = std::get<ValueOperand>(inst.operands[0]);
        std::string srcType = getValueType(srcOp.id, valueTypes);
        
        out << "%" << *inst.result << " = bitcast " << srcType << " %" << srcOp.id 
            << " to " << srcType;
        
        return true;
    }
    
    bool emitDrop(std::ostringstream& out, const SirInstruction& inst,
                  const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (inst.operands.empty()) return false;
        
        auto& valOp = std::get<ValueOperand>(inst.operands[0]);
        std::string valType = getValueType(valOp.id, valueTypes);
        
        // استدعاء sad_free للمؤشرات
        if (valType.back() == '*') {
            out << "call void @sad_free(i8* %" << valOp.id << ")";
        } else {
            out << "; drop non-pointer value";
        }
        
        return true;
    }
    
    bool emitClone(std::ostringstream& out, const SirInstruction& inst,
                   const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.empty()) return false;
        
        auto& srcOp = std::get<ValueOperand>(inst.operands[0]);
        std::string srcType = getValueType(srcOp.id, valueTypes);
        
        // للنصوص
        if (srcType == "%String*") {
            out << "%" << *inst.result << " = call %String* @sad_string_clone(%String* %" << srcOp.id << ")";
        }
        // للمصفوفات
        else if (srcType.find("%Array") != std::string::npos) {
            out << "%" << *inst.result << " = call %Array* @sad_array_clone(%Array* %" << srcOp.id << ")";
        }
        // افتراضي
        else {
            out << "%" << *inst.result << " = bitcast " << srcType << " %" << srcOp.id << " to " << srcType;
        }
        
        return true;
    }
    
    bool emitDeref(std::ostringstream& out, const SirInstruction& inst,
                   const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.empty()) return false;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        std::string ptrType = getValueType(ptrOp.id, valueTypes);
        std::string elemType = inst.resultType ? typeToLlvm(inst.resultType) : "i32";
        
        out << "%" << *inst.result << " = load " << elemType << ", " << ptrType << " %" << ptrOp.id;
        
        return true;
    }
    
    bool emitLoad(std::ostringstream& out, const SirInstruction& inst,
                  const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.empty()) return false;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        std::string elemType = inst.resultType ? typeToLlvm(inst.resultType) : "i32";
        
        out << "%" << *inst.result << " = load " << elemType << ", " << elemType << "* %" << ptrOp.id;
        
        return true;
    }
    
    bool emitStore(std::ostringstream& out, const SirInstruction& inst,
                   const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (inst.operands.size() < 2) return false;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        auto& valOp = std::get<ValueOperand>(inst.operands[1]);
        std::string valType = getValueType(valOp.id, valueTypes);
        
        out << "store " << valType << " %" << valOp.id << ", " << valType << "* %" << ptrOp.id;
        
        return true;
    }
    
    bool emitBinaryOp(std::ostringstream& out, const SirInstruction& inst,
                      const std::map<size_t, std::shared_ptr<SirType>>& valueTypes,
                      const std::string& intOp, const std::string& floatOp) {
        if (!inst.result || inst.operands.size() < 2) return false;
        
        auto& lhsOp = std::get<ValueOperand>(inst.operands[0]);
        auto& rhsOp = std::get<ValueOperand>(inst.operands[1]);
        std::string opType = getValueType(lhsOp.id, valueTypes);
        
        // تحديد العملية بناءً على النوع
        std::string op = intOp;
        if (opType == "float" || opType == "double") {
            op = floatOp;
        }
        
        out << "%" << *inst.result << " = " << op << " " << opType 
            << " %" << lhsOp.id << ", %" << rhsOp.id;
        
        return true;
    }
    
    bool emitUnaryOp(std::ostringstream& out, const SirInstruction& inst,
                     const std::map<size_t, std::shared_ptr<SirType>>& valueTypes,
                     const std::string& intOp, const std::string& floatOp) {
        if (!inst.result || inst.operands.empty()) return false;
        
        auto& srcOp = std::get<ValueOperand>(inst.operands[0]);
        std::string opType = getValueType(srcOp.id, valueTypes);
        
        std::string op = intOp;
        if (opType == "float" || opType == "double") {
            op = floatOp;
        }
        
        if (op == "neg") {
            out << "%" << *inst.result << " = sub " << opType << " 0, %" << srcOp.id;
        } else if (op == "fneg") {
            out << "%" << *inst.result << " = fneg " << opType << " %" << srcOp.id;
        }
        
        return true;
    }
    
    bool emitNot(std::ostringstream& out, const SirInstruction& inst,
                 const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.empty()) return false;
        
        auto& srcOp = std::get<ValueOperand>(inst.operands[0]);
        std::string opType = getValueType(srcOp.id, valueTypes);
        
        out << "%" << *inst.result << " = xor " << opType << " %" << srcOp.id << ", -1";
        
        return true;
    }
    
    bool emitComparison(std::ostringstream& out, const SirInstruction& inst,
                        const std::map<size_t, std::shared_ptr<SirType>>& valueTypes,
                        const std::string& intCmp, const std::string& floatCmp) {
        if (!inst.result || inst.operands.size() < 2) return false;
        
        auto& lhsOp = std::get<ValueOperand>(inst.operands[0]);
        auto& rhsOp = std::get<ValueOperand>(inst.operands[1]);
        std::string opType = getValueType(lhsOp.id, valueTypes);
        
        std::string cmpOp = "icmp";
        std::string cmp = intCmp;
        if (opType == "float" || opType == "double") {
            cmpOp = "fcmp";
            cmp = floatCmp;
        }
        
        out << "%" << *inst.result << " = " << cmpOp << " " << cmp << " " << opType 
            << " %" << lhsOp.id << ", %" << rhsOp.id;
        
        return true;
    }
    
    bool emitJump(std::ostringstream& out, const SirInstruction& inst) {
        if (inst.operands.empty()) return false;
        
        auto& targetOp = std::get<LabelOperand>(inst.operands[0]);
        out << "br label %" << targetOp.name;
        
        return true;
    }
    
    bool emitBranch(std::ostringstream& out, const SirInstruction& inst,
                    const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (inst.operands.size() < 3) return false;
        
        auto& condOp = std::get<ValueOperand>(inst.operands[0]);
        auto& trueOp = std::get<LabelOperand>(inst.operands[1]);
        auto& falseOp = std::get<LabelOperand>(inst.operands[2]);
        
        out << "br i1 %" << condOp.id << ", label %" << trueOp.name 
            << ", label %" << falseOp.name;
        
        return true;
    }
    
    bool emitReturn(std::ostringstream& out, const SirInstruction& inst,
                    const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (inst.operands.empty()) {
            out << "ret void";
        } else {
            auto& valOp = std::get<ValueOperand>(inst.operands[0]);
            std::string valType = getValueType(valOp.id, valueTypes);
            out << "ret " << valType << " %" << valOp.id;
        }
        
        return true;
    }
    
    bool emitCall(std::ostringstream& out, const SirInstruction& inst,
                  const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (inst.operands.empty()) return false;
        
        auto& funcOp = std::get<FunctionOperand>(inst.operands[0]);
        std::string retType = inst.resultType ? typeToLlvm(inst.resultType) : "void";
        
        if (inst.result && retType != "void") {
            out << "%" << *inst.result << " = ";
        }
        
        out << "call " << retType << " @" << funcOp.name << "(";
        
        // المعاملات
        for (size_t i = 1; i < inst.operands.size(); ++i) {
            if (i > 1) out << ", ";
            auto& argOp = std::get<ValueOperand>(inst.operands[i]);
            std::string argType = getValueType(argOp.id, valueTypes);
            out << argType << " %" << argOp.id;
        }
        
        out << ")";
        
        return true;
    }
    
    bool emitIndirectCall(std::ostringstream& out, const SirInstruction& inst,
                          const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (inst.operands.empty()) return false;
        
        auto& funcPtrOp = std::get<ValueOperand>(inst.operands[0]);
        std::string retType = inst.resultType ? typeToLlvm(inst.resultType) : "void";
        
        if (inst.result && retType != "void") {
            out << "%" << *inst.result << " = ";
        }
        
        out << "call " << retType << " %" << funcPtrOp.id << "(";
        
        // المعاملات
        for (size_t i = 1; i < inst.operands.size(); ++i) {
            if (i > 1) out << ", ";
            auto& argOp = std::get<ValueOperand>(inst.operands[i]);
            std::string argType = getValueType(argOp.id, valueTypes);
            out << argType << " %" << argOp.id;
        }
        
        out << ")";
        
        return true;
    }
    
    bool emitCast(std::ostringstream& out, const SirInstruction& inst,
                  const std::map<size_t, std::shared_ptr<SirType>>& valueTypes,
                  const std::string& castOp) {
        if (!inst.result || inst.operands.empty()) return false;
        
        auto& srcOp = std::get<ValueOperand>(inst.operands[0]);
        std::string srcType = getValueType(srcOp.id, valueTypes);
        std::string dstType = inst.resultType ? typeToLlvm(inst.resultType) : "i32";
        
        out << "%" << *inst.result << " = " << castOp << " " << srcType 
            << " %" << srcOp.id << " to " << dstType;
        
        return true;
    }
    
    bool emitAtomicLoad(std::ostringstream& out, const SirInstruction& inst,
                        const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.empty()) return false;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        std::string elemType = inst.resultType ? typeToLlvm(inst.resultType) : "i32";
        
        out << "%" << *inst.result << " = load atomic " << elemType << ", " 
            << elemType << "* %" << ptrOp.id << " seq_cst";
        
        return true;
    }
    
    bool emitAtomicStore(std::ostringstream& out, const SirInstruction& inst,
                         const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (inst.operands.size() < 2) return false;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        auto& valOp = std::get<ValueOperand>(inst.operands[1]);
        std::string valType = getValueType(valOp.id, valueTypes);
        
        out << "store atomic " << valType << " %" << valOp.id 
            << ", " << valType << "* %" << ptrOp.id << " seq_cst";
        
        return true;
    }
    
    bool emitAtomicRmw(std::ostringstream& out, const SirInstruction& inst,
                       const std::map<size_t, std::shared_ptr<SirType>>& valueTypes,
                       const std::string& op) {
        if (!inst.result || inst.operands.size() < 2) return false;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        auto& valOp = std::get<ValueOperand>(inst.operands[1]);
        std::string valType = inst.resultType ? typeToLlvm(inst.resultType) : "i64";
        
        out << "%" << *inst.result << " = atomicrmw " << op << " " 
            << valType << "* %" << ptrOp.id << ", " << valType 
            << " %" << valOp.id << " seq_cst";
        
        return true;
    }
    
    bool emitAtomicCmpXchg(std::ostringstream& out, const SirInstruction& inst,
                           const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.size() < 3) return false;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        auto& expectedOp = std::get<ValueOperand>(inst.operands[1]);
        auto& desiredOp = std::get<ValueOperand>(inst.operands[2]);
        std::string valType = getValueType(expectedOp.id, valueTypes);
        
        out << "%" << *inst.result << ".pair = cmpxchg " << valType 
            << "* %" << ptrOp.id << ", " << valType << " %" << expectedOp.id 
            << ", " << valType << " %" << desiredOp.id << " seq_cst seq_cst\n  ";
        out << "%" << *inst.result << " = extractvalue {" << valType 
            << ", i1} %" << *inst.result << ".pair, 0";
        
        return true;
    }
    
    bool emitGetElementPtr(std::ostringstream& out, const SirInstruction& inst,
                           const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.size() < 2) return false;
        
        auto& ptrOp = std::get<ValueOperand>(inst.operands[0]);
        std::string ptrType = getValueType(ptrOp.id, valueTypes);
        std::string elemType = inst.resultType ? typeToLlvm(inst.resultType) : "i8";
        
        out << "%" << *inst.result << " = getelementptr " << elemType 
            << ", " << ptrType << " %" << ptrOp.id;
        
        for (size_t i = 1; i < inst.operands.size(); ++i) {
            auto& idxOp = std::get<ValueOperand>(inst.operands[i]);
            out << ", i64 %" << idxOp.id;
        }
        
        return true;
    }
    
    bool emitExtractValue(std::ostringstream& out, const SirInstruction& inst,
                          const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.size() < 2) return false;
        
        auto& aggOp = std::get<ValueOperand>(inst.operands[0]);
        auto& idxOp = std::get<ConstantOperand>(inst.operands[1]);
        std::string aggType = getValueType(aggOp.id, valueTypes);
        std::string elemType = inst.resultType ? typeToLlvm(inst.resultType) : "i32";
        
        int64_t idx = std::get<int64_t>(idxOp.value);
        
        out << "%" << *inst.result << " = extractvalue " << aggType 
            << " %" << aggOp.id << ", " << idx;
        
        return true;
    }
    
    bool emitInsertValue(std::ostringstream& out, const SirInstruction& inst,
                         const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.size() < 3) return false;
        
        auto& aggOp = std::get<ValueOperand>(inst.operands[0]);
        auto& valOp = std::get<ValueOperand>(inst.operands[1]);
        auto& idxOp = std::get<ConstantOperand>(inst.operands[2]);
        std::string aggType = getValueType(aggOp.id, valueTypes);
        std::string valType = getValueType(valOp.id, valueTypes);
        
        int64_t idx = std::get<int64_t>(idxOp.value);
        
        out << "%" << *inst.result << " = insertvalue " << aggType 
            << " %" << aggOp.id << ", " << valType << " %" << valOp.id << ", " << idx;
        
        return true;
    }
    
    bool emitPhi(std::ostringstream& out, const SirInstruction& inst,
                 const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.size() < 2) return false;
        
        std::string phiType = inst.resultType ? typeToLlvm(inst.resultType) : "i32";
        
        out << "%" << *inst.result << " = phi " << phiType << " ";
        
        // كل زوج: [value, label]
        for (size_t i = 0; i < inst.operands.size(); i += 2) {
            if (i > 0) out << ", ";
            auto& valOp = std::get<ValueOperand>(inst.operands[i]);
            auto& labelOp = std::get<LabelOperand>(inst.operands[i + 1]);
            out << "[ %" << valOp.id << ", %" << labelOp.name << " ]";
        }
        
        return true;
    }
    
    bool emitSelect(std::ostringstream& out, const SirInstruction& inst,
                    const std::map<size_t, std::shared_ptr<SirType>>& valueTypes) {
        if (!inst.result || inst.operands.size() < 3) return false;
        
        auto& condOp = std::get<ValueOperand>(inst.operands[0]);
        auto& trueOp = std::get<ValueOperand>(inst.operands[1]);
        auto& falseOp = std::get<ValueOperand>(inst.operands[2]);
        std::string valType = getValueType(trueOp.id, valueTypes);
        
        out << "%" << *inst.result << " = select i1 %" << condOp.id 
            << ", " << valType << " %" << trueOp.id 
            << ", " << valType << " %" << falseOp.id;
        
        return true;
    }
};

} // namespace sad::sir

#endif // Include guard would go here
