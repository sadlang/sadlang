/**
 * =============================================================================
 * ملف: llvm_graphics_codegen.cpp
 * الوصف: توليد كود LLVM IR لنظام الرسومات - دعم جميع أنظمة التشغيل
 * المرحلة: Phase 30 - Graphics LLVM Backend
 * =============================================================================
 * 
 * 🎨 دليل نظام الرسومات مع LLVM
 * ═════════════════════════════════════════
 * 
 * الهدف:
 * ──────
 * ربط stdlib/graphics بالمترجم لتوليد ملفات تنفيذية أصلية
 * تعمل على Windows, Linux, macOS بدون مفسر.
 * 
 * البنية:
 * ──────
 * ```
 * كود ص مع رسومات
 *       ↓
 * Parser → AST → SIR
 *       ↓
 * SIR + Graphics Intrinsics
 *       ↓
 * LLVM IR
 *       ↓
 * ┌─────────┬──────────┬─────────┐
 * │ Windows │  Linux   │ macOS   │
 * │  PE/EXE │   ELF    │ Mach-O  │
 * └─────────┴──────────┴─────────┘
 * ```
 * 
 * الربط مع SDL2:
 * ──────────────
 * نستخدم SDL2 كطبقة تجريد للرسومات عبر FFI.
 * المترجم يولد استدعاءات مباشرة لـ SDL2 API.
 * 
 * =============================================================================
 */

#ifdef HAS_LLVM

#include "llvm_codegen.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>

namespace Sad {
namespace LLVM {
namespace Graphics {

// =============================================================================
// أنواع الرسومات في LLVM
// Graphics Types in LLVM
// =============================================================================

/**
 * @brief مولد أنواع الرسومات لـ LLVM
 * @brief Graphics types generator for LLVM
 * 
 * يولد الأنواع التالية:
 * - %RGBA = type { i8, i8, i8, i8 }           // لون RGBA
 * - %Widget = type { i32, %RGBA, float, ... } // الويدجت
 * - %Window = type { i8*, i32, i32, ... }     // النافذة
 * - %Event = type { i32, i32, i32, ... }      // الحدث
 */
class GraphicsTypeGenerator {
public:
    explicit GraphicsTypeGenerator(llvm::LLVMContext& ctx, llvm::Module& mod)
        : context_(ctx), module_(mod), builder_(ctx) {}
    
    // ═══════════════════════════════════════════════════════════════════════
    // توليد الأنواع الأساسية
    // ═══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief توليد نوع RGBA
     */
    llvm::StructType* createRGBAType() {
        if (rgbaType_) return rgbaType_;
        
        std::vector<llvm::Type*> fields = {
            llvm::Type::getInt8Ty(context_),   // r
            llvm::Type::getInt8Ty(context_),   // g
            llvm::Type::getInt8Ty(context_),   // b
            llvm::Type::getInt8Ty(context_)    // a
        };
        
        rgbaType_ = llvm::StructType::create(context_, fields, "RGBA");
        return rgbaType_;
    }
    
    /**
     * @brief توليد نوع Widget (بنية الويدجت)
     */
    llvm::StructType* createWidgetType() {
        if (widgetType_) return widgetType_;
        
        std::vector<llvm::Type*> fields = {
            llvm::Type::getInt32Ty(context_),     // id
            llvm::Type::getInt32Ty(context_),     // type (enum)
            llvm::Type::getFloatTy(context_),     // x
            llvm::Type::getFloatTy(context_),     // y
            llvm::Type::getFloatTy(context_),     // width
            llvm::Type::getFloatTy(context_),     // height
            createRGBAType(),                      // backgroundColor
            createRGBAType(),                      // foregroundColor
            llvm::Type::getFloatTy(context_),     // opacity
            llvm::Type::getInt1Ty(context_),      // visible
            llvm::Type::getInt1Ty(context_),      // enabled
            llvm::Type::getFloatTy(context_),     // borderRadius
            llvm::Type::getFloatTy(context_),     // borderWidth
            createRGBAType(),                      // borderColor
            llvm::Type::getInt32Ty(context_),     // parentId
            llvm::Type::getInt8PtrTy(context_),   // text (char*)
            llvm::Type::getFloatTy(context_),     // fontSize
            llvm::Type::getInt32Ty(context_),     // zIndex
            // ... يمكن إضافة المزيد
        };
        
        widgetType_ = llvm::StructType::create(context_, fields, "sad.ui.Widget");
        return widgetType_;
    }
    
    /**
     * @brief توليد نوع Window
     */
    llvm::StructType* createWindowType() {
        if (windowType_) return windowType_;
        
        std::vector<llvm::Type*> fields = {
            llvm::Type::getInt8PtrTy(context_),   // SDL_Window*
            llvm::Type::getInt8PtrTy(context_),   // SDL_Renderer*
            llvm::Type::getInt32Ty(context_),     // width
            llvm::Type::getInt32Ty(context_),     // height
            llvm::Type::getInt8PtrTy(context_),   // title
            llvm::Type::getInt1Ty(context_),      // isOpen
            llvm::Type::getInt32Ty(context_),     // flags
        };
        
        windowType_ = llvm::StructType::create(context_, fields, "sad.ui.Window");
        return windowType_;
    }
    
    /**
     * @brief توليد نوع Event
     */
    llvm::StructType* createEventType() {
        if (eventType_) return eventType_;
        
        std::vector<llvm::Type*> fields = {
            llvm::Type::getInt32Ty(context_),     // type
            llvm::Type::getInt32Ty(context_),     // widgetId
            llvm::Type::getInt32Ty(context_),     // x
            llvm::Type::getInt32Ty(context_),     // y
            llvm::Type::getInt32Ty(context_),     // button
            llvm::Type::getInt32Ty(context_),     // key
            llvm::Type::getInt32Ty(context_),     // modifiers
        };
        
        eventType_ = llvm::StructType::create(context_, fields, "sad.ui.Event");
        return eventType_;
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // توليد جميع الأنواع
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateAllTypes() {
        createRGBAType();
        createWidgetType();
        createWindowType();
        createEventType();
    }
    
    // Getters
    llvm::StructType* getRGBAType() { return createRGBAType(); }
    llvm::StructType* getWidgetType() { return createWidgetType(); }
    llvm::StructType* getWindowType() { return createWindowType(); }
    llvm::StructType* getEventType() { return createEventType(); }

private:
    llvm::LLVMContext& context_;
    llvm::Module& module_;
    llvm::IRBuilder<> builder_;
    
    llvm::StructType* rgbaType_ = nullptr;
    llvm::StructType* widgetType_ = nullptr;
    llvm::StructType* windowType_ = nullptr;
    llvm::StructType* eventType_ = nullptr;
};

// =============================================================================
// تعريفات دوال SDL2 الخارجية
// SDL2 External Function Declarations
// =============================================================================

/**
 * @brief مولد تعريفات SDL2
 * @brief SDL2 declarations generator
 * 
 * يولد تعريفات extern لدوال SDL2 المستخدمة:
 * - SDL_Init, SDL_Quit
 * - SDL_CreateWindow, SDL_DestroyWindow
 * - SDL_CreateRenderer, SDL_DestroyRenderer
 * - SDL_RenderClear, SDL_RenderPresent
 * - SDL_SetRenderDrawColor, SDL_RenderFillRect
 * - SDL_PollEvent, SDL_WaitEvent
 * - وغيرها...
 */
class SDL2Declarations {
public:
    explicit SDL2Declarations(llvm::LLVMContext& ctx, llvm::Module& mod)
        : context_(ctx), module_(mod) {}
    
    /**
     * @brief توليد جميع تعريفات SDL2
     */
    void generateAllDeclarations() {
        // SDL_Init(uint32) -> int32
        declareFunction("SDL_Init", 
            llvm::Type::getInt32Ty(context_),
            {llvm::Type::getInt32Ty(context_)});
        
        // SDL_Quit() -> void
        declareFunction("SDL_Quit",
            llvm::Type::getVoidTy(context_),
            {});
        
        // SDL_CreateWindow(char*, int, int, int, int, uint32) -> void*
        declareFunction("SDL_CreateWindow",
            llvm::Type::getInt8PtrTy(context_),
            {
                llvm::Type::getInt8PtrTy(context_),  // title
                llvm::Type::getInt32Ty(context_),    // x
                llvm::Type::getInt32Ty(context_),    // y
                llvm::Type::getInt32Ty(context_),    // w
                llvm::Type::getInt32Ty(context_),    // h
                llvm::Type::getInt32Ty(context_)     // flags
            });
        
        // SDL_DestroyWindow(void*) -> void
        declareFunction("SDL_DestroyWindow",
            llvm::Type::getVoidTy(context_),
            {llvm::Type::getInt8PtrTy(context_)});
        
        // SDL_CreateRenderer(void*, int, uint32) -> void*
        declareFunction("SDL_CreateRenderer",
            llvm::Type::getInt8PtrTy(context_),
            {
                llvm::Type::getInt8PtrTy(context_),  // window
                llvm::Type::getInt32Ty(context_),    // index
                llvm::Type::getInt32Ty(context_)     // flags
            });
        
        // SDL_DestroyRenderer(void*) -> void
        declareFunction("SDL_DestroyRenderer",
            llvm::Type::getVoidTy(context_),
            {llvm::Type::getInt8PtrTy(context_)});
        
        // SDL_SetRenderDrawColor(void*, uint8, uint8, uint8, uint8) -> int
        declareFunction("SDL_SetRenderDrawColor",
            llvm::Type::getInt32Ty(context_),
            {
                llvm::Type::getInt8PtrTy(context_),  // renderer
                llvm::Type::getInt8Ty(context_),     // r
                llvm::Type::getInt8Ty(context_),     // g
                llvm::Type::getInt8Ty(context_),     // b
                llvm::Type::getInt8Ty(context_)      // a
            });
        
        // SDL_RenderClear(void*) -> int
        declareFunction("SDL_RenderClear",
            llvm::Type::getInt32Ty(context_),
            {llvm::Type::getInt8PtrTy(context_)});
        
        // SDL_RenderPresent(void*) -> void
        declareFunction("SDL_RenderPresent",
            llvm::Type::getVoidTy(context_),
            {llvm::Type::getInt8PtrTy(context_)});
        
        // SDL_RenderFillRect(void*, void*) -> int
        declareFunction("SDL_RenderFillRect",
            llvm::Type::getInt32Ty(context_),
            {
                llvm::Type::getInt8PtrTy(context_),  // renderer
                llvm::Type::getInt8PtrTy(context_)   // rect
            });
        
        // SDL_RenderDrawRect(void*, void*) -> int
        declareFunction("SDL_RenderDrawRect",
            llvm::Type::getInt32Ty(context_),
            {
                llvm::Type::getInt8PtrTy(context_),
                llvm::Type::getInt8PtrTy(context_)
            });
        
        // SDL_PollEvent(void*) -> int
        declareFunction("SDL_PollEvent",
            llvm::Type::getInt32Ty(context_),
            {llvm::Type::getInt8PtrTy(context_)});
        
        // SDL_WaitEvent(void*) -> int
        declareFunction("SDL_WaitEvent",
            llvm::Type::getInt32Ty(context_),
            {llvm::Type::getInt8PtrTy(context_)});
        
        // SDL_GetMouseState(int*, int*) -> uint32
        declareFunction("SDL_GetMouseState",
            llvm::Type::getInt32Ty(context_),
            {
                llvm::PointerType::get(llvm::Type::getInt32Ty(context_), 0),
                llvm::PointerType::get(llvm::Type::getInt32Ty(context_), 0)
            });
        
        // SDL_Delay(uint32) -> void
        declareFunction("SDL_Delay",
            llvm::Type::getVoidTy(context_),
            {llvm::Type::getInt32Ty(context_)});
        
        // SDL_GetTicks() -> uint32
        declareFunction("SDL_GetTicks",
            llvm::Type::getInt32Ty(context_),
            {});
        
        // TTF functions for text rendering
        declareFunction("TTF_Init",
            llvm::Type::getInt32Ty(context_),
            {});
        
        declareFunction("TTF_Quit",
            llvm::Type::getVoidTy(context_),
            {});
        
        declareFunction("TTF_OpenFont",
            llvm::Type::getInt8PtrTy(context_),
            {
                llvm::Type::getInt8PtrTy(context_),  // file
                llvm::Type::getInt32Ty(context_)     // ptsize
            });
        
        declareFunction("TTF_CloseFont",
            llvm::Type::getVoidTy(context_),
            {llvm::Type::getInt8PtrTy(context_)});
        
        // Standard C library
        declareFunction("printf",
            llvm::Type::getInt32Ty(context_),
            {llvm::Type::getInt8PtrTy(context_)},
            true);  // variadic
        
        declareFunction("malloc",
            llvm::Type::getInt8PtrTy(context_),
            {llvm::Type::getInt64Ty(context_)});
        
        declareFunction("free",
            llvm::Type::getVoidTy(context_),
            {llvm::Type::getInt8PtrTy(context_)});
        
        declareFunction("memcpy",
            llvm::Type::getInt8PtrTy(context_),
            {
                llvm::Type::getInt8PtrTy(context_),
                llvm::Type::getInt8PtrTy(context_),
                llvm::Type::getInt64Ty(context_)
            });
        
        declareFunction("memset",
            llvm::Type::getInt8PtrTy(context_),
            {
                llvm::Type::getInt8PtrTy(context_),
                llvm::Type::getInt32Ty(context_),
                llvm::Type::getInt64Ty(context_)
            });
    }
    
    llvm::Function* getFunction(const std::string& name) {
        return module_.getFunction(name);
    }

private:
    void declareFunction(const std::string& name,
                        llvm::Type* returnType,
                        std::vector<llvm::Type*> paramTypes,
                        bool isVarArg = false) {
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            returnType, paramTypes, isVarArg);
        
        module_.getOrInsertFunction(name, funcType);
    }
    
    llvm::LLVMContext& context_;
    llvm::Module& module_;
};

// =============================================================================
// دوال Runtime للرسومات
// Graphics Runtime Functions
// =============================================================================

/**
 * @brief مولد دوال الرسومات في Runtime
 * @brief Graphics runtime functions generator
 * 
 * يولد الدوال التالية:
 * - sad_ui_init(): تهيئة نظام الرسومات
 * - sad_ui_quit(): إغلاق نظام الرسومات
 * - sad_ui_create_window(): إنشاء نافذة
 * - sad_ui_create_widget(): إنشاء ويدجت
 * - sad_ui_render(): رسم الواجهة
 * - sad_ui_event_loop(): حلقة الأحداث
 */
class GraphicsRuntimeGenerator {
public:
    GraphicsRuntimeGenerator(llvm::LLVMContext& ctx, 
                            llvm::Module& mod,
                            GraphicsTypeGenerator& types,
                            SDL2Declarations& sdl)
        : context_(ctx), module_(mod), builder_(ctx), 
          types_(types), sdl_(sdl) {}
    
    /**
     * @brief توليد جميع دوال Runtime
     */
    void generateAllFunctions() {
        generateUIInit();
        generateUIQuit();
        generateCreateWindow();
        generateDestroyWindow();
        generateCreateWidget();
        generateDestroyWidget();
        generateSetWidgetProperty();
        generateRenderWidget();
        generateRenderAll();
        generateEventLoop();
        generateProcessEvent();
    }
    
private:
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_init() — تهيئة نظام الرسومات
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateUIInit() {
        // int sad_ui_init()
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context_), {}, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_init", module_);
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)
        llvm::Value* initFlags = llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(context_), 0x00000020 | 0x00004000);
        
        llvm::Function* sdlInit = sdl_.getFunction("SDL_Init");
        llvm::Value* result = builder_.CreateCall(sdlInit, {initFlags});
        
        // TTF_Init()
        llvm::Function* ttfInit = sdl_.getFunction("TTF_Init");
        if (ttfInit) {
            builder_.CreateCall(ttfInit, {});
        }
        
        builder_.CreateRet(result);
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_quit() — إغلاق نظام الرسومات
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateUIQuit() {
        // void sad_ui_quit()
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(context_), {}, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_quit", module_);
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // TTF_Quit()
        llvm::Function* ttfQuit = sdl_.getFunction("TTF_Quit");
        if (ttfQuit) {
            builder_.CreateCall(ttfQuit, {});
        }
        
        // SDL_Quit()
        llvm::Function* sdlQuit = sdl_.getFunction("SDL_Quit");
        builder_.CreateCall(sdlQuit, {});
        
        builder_.CreateRetVoid();
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_create_window() — إنشاء نافذة
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateCreateWindow() {
        // void* sad_ui_create_window(char* title, int w, int h)
        std::vector<llvm::Type*> params = {
            llvm::Type::getInt8PtrTy(context_),  // title
            llvm::Type::getInt32Ty(context_),    // width
            llvm::Type::getInt32Ty(context_)     // height
        };
        
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getInt8PtrTy(context_), params, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_create_window", module_);
        
        auto args = func->arg_begin();
        llvm::Value* title = args++;
        llvm::Value* width = args++;
        llvm::Value* height = args;
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // SDL_CreateWindow(title, CENTERED, CENTERED, w, h, SHOWN)
        llvm::Value* centered = llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(context_), 0x2FFF0000); // SDL_WINDOWPOS_CENTERED
        llvm::Value* flags = llvm::ConstantInt::get(
            llvm::Type::getInt32Ty(context_), 4); // SDL_WINDOW_SHOWN
        
        llvm::Function* createWindow = sdl_.getFunction("SDL_CreateWindow");
        llvm::Value* window = builder_.CreateCall(
            createWindow, {title, centered, centered, width, height, flags});
        
        builder_.CreateRet(window);
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_destroy_window() — تدمير نافذة
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateDestroyWindow() {
        // void sad_ui_destroy_window(void* window)
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(context_),
            {llvm::Type::getInt8PtrTy(context_)}, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_destroy_window", module_);
        
        llvm::Value* window = func->arg_begin();
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        llvm::Function* destroyWindow = sdl_.getFunction("SDL_DestroyWindow");
        builder_.CreateCall(destroyWindow, {window});
        
        builder_.CreateRetVoid();
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_create_widget() — إنشاء ويدجت
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateCreateWidget() {
        // int sad_ui_create_widget(int type)
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context_),
            {llvm::Type::getInt32Ty(context_)}, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_create_widget", module_);
        
        llvm::Value* widgetType = func->arg_begin();
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // تخصيص ذاكرة للويدجت
        llvm::Function* mallocFn = sdl_.getFunction("malloc");
        llvm::Value* size = llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(context_), 256); // حجم Widget struct
        
        llvm::Value* ptr = builder_.CreateCall(mallocFn, {size});
        
        // تصفير الذاكرة
        llvm::Function* memsetFn = sdl_.getFunction("memset");
        builder_.CreateCall(memsetFn, {
            ptr,
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 0),
            size
        });
        
        // إرجاع معرف الويدجت (مؤشر محول لـ int)
        llvm::Value* id = builder_.CreatePtrToInt(
            ptr, llvm::Type::getInt32Ty(context_));
        
        builder_.CreateRet(id);
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_destroy_widget() — تدمير ويدجت
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateDestroyWidget() {
        // void sad_ui_destroy_widget(int id)
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(context_),
            {llvm::Type::getInt32Ty(context_)}, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_destroy_widget", module_);
        
        llvm::Value* id = func->arg_begin();
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // تحويل المعرف لمؤشر وتحريره
        llvm::Value* ptr = builder_.CreateIntToPtr(
            id, llvm::Type::getInt8PtrTy(context_));
        
        llvm::Function* freeFn = sdl_.getFunction("free");
        builder_.CreateCall(freeFn, {ptr});
        
        builder_.CreateRetVoid();
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_set_property() — تعيين خاصية ويدجت
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateSetWidgetProperty() {
        // void sad_ui_set_property(int id, char* name, char* value)
        std::vector<llvm::Type*> params = {
            llvm::Type::getInt32Ty(context_),    // widget id
            llvm::Type::getInt8PtrTy(context_),  // property name
            llvm::Type::getInt8PtrTy(context_)   // value
        };
        
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(context_), params, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_set_property", module_);
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // TODO: تنفيذ منطق تعيين الخصائص
        builder_.CreateRetVoid();
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_render_widget() — رسم ويدجت واحد
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateRenderWidget() {
        // void sad_ui_render_widget(void* renderer, int id)
        std::vector<llvm::Type*> params = {
            llvm::Type::getInt8PtrTy(context_),  // renderer
            llvm::Type::getInt32Ty(context_)     // widget id
        };
        
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(context_), params, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_render_widget", module_);
        
        auto args = func->arg_begin();
        llvm::Value* renderer = args++;
        llvm::Value* widgetId = args;
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // تعيين لون الرسم الافتراضي (أبيض)
        llvm::Function* setColor = sdl_.getFunction("SDL_SetRenderDrawColor");
        builder_.CreateCall(setColor, {
            renderer,
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 255),
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 255),
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 255),
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 255)
        });
        
        // TODO: رسم الويدجت بناءً على نوعه
        
        builder_.CreateRetVoid();
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_render_all() — رسم كل الويدجات
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateRenderAll() {
        // void sad_ui_render_all(void* renderer)
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(context_),
            {llvm::Type::getInt8PtrTy(context_)}, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_render_all", module_);
        
        llvm::Value* renderer = func->arg_begin();
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // مسح الشاشة
        llvm::Function* setColor = sdl_.getFunction("SDL_SetRenderDrawColor");
        builder_.CreateCall(setColor, {
            renderer,
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 30),
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 30),
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 30),
            llvm::ConstantInt::get(llvm::Type::getInt8Ty(context_), 255)
        });
        
        llvm::Function* clear = sdl_.getFunction("SDL_RenderClear");
        builder_.CreateCall(clear, {renderer});
        
        // TODO: رسم جميع الويدجات المسجلة
        
        // عرض الإطار
        llvm::Function* present = sdl_.getFunction("SDL_RenderPresent");
        builder_.CreateCall(present, {renderer});
        
        builder_.CreateRetVoid();
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_event_loop() — حلقة الأحداث
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateEventLoop() {
        // int sad_ui_event_loop(void* window, void* renderer)
        std::vector<llvm::Type*> params = {
            llvm::Type::getInt8PtrTy(context_),  // window
            llvm::Type::getInt8PtrTy(context_)   // renderer
        };
        
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context_), params, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_event_loop", module_);
        
        auto args = func->arg_begin();
        llvm::Value* window = args++;
        llvm::Value* renderer = args;
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(context_, "entry", func);
        llvm::BasicBlock* loopHead = llvm::BasicBlock::Create(context_, "loop_head", func);
        llvm::BasicBlock* loopBody = llvm::BasicBlock::Create(context_, "loop_body", func);
        llvm::BasicBlock* loopEnd = llvm::BasicBlock::Create(context_, "loop_end", func);
        llvm::BasicBlock* exit = llvm::BasicBlock::Create(context_, "exit", func);
        
        // Entry
        builder_.SetInsertPoint(entry);
        
        // تخصيص SDL_Event على المكدس
        llvm::Value* eventSize = llvm::ConstantInt::get(
            llvm::Type::getInt64Ty(context_), 56); // sizeof(SDL_Event)
        llvm::Function* mallocFn = sdl_.getFunction("malloc");
        llvm::Value* eventPtr = builder_.CreateCall(mallocFn, {eventSize});
        
        // متغير التشغيل
        llvm::AllocaInst* running = builder_.CreateAlloca(
            llvm::Type::getInt1Ty(context_), nullptr, "running");
        builder_.CreateStore(
            llvm::ConstantInt::getTrue(context_), running);
        
        builder_.CreateBr(loopHead);
        
        // Loop head - فحص الشرط
        builder_.SetInsertPoint(loopHead);
        llvm::Value* runningVal = builder_.CreateLoad(
            llvm::Type::getInt1Ty(context_), running);
        builder_.CreateCondBr(runningVal, loopBody, exit);
        
        // Loop body
        builder_.SetInsertPoint(loopBody);
        
        // معالجة الأحداث
        llvm::BasicBlock* eventLoop = llvm::BasicBlock::Create(
            context_, "event_loop", func);
        llvm::BasicBlock* eventDone = llvm::BasicBlock::Create(
            context_, "event_done", func);
        
        builder_.CreateBr(eventLoop);
        
        // Event loop
        builder_.SetInsertPoint(eventLoop);
        llvm::Function* pollEvent = sdl_.getFunction("SDL_PollEvent");
        llvm::Value* hasEvent = builder_.CreateCall(pollEvent, {eventPtr});
        llvm::Value* hasEventBool = builder_.CreateICmpNE(
            hasEvent, llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 0));
        builder_.CreateCondBr(hasEventBool, eventLoop, eventDone);
        
        // Event done - render
        builder_.SetInsertPoint(eventDone);
        llvm::Function* renderAll = module_.getFunction("sad_ui_render_all");
        if (renderAll) {
            builder_.CreateCall(renderAll, {renderer});
        }
        
        // تأخير صغير
        llvm::Function* delay = sdl_.getFunction("SDL_Delay");
        builder_.CreateCall(delay, {
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 16)
        });
        
        builder_.CreateBr(loopHead);
        
        // Exit
        builder_.SetInsertPoint(exit);
        llvm::Function* freeFn = sdl_.getFunction("free");
        builder_.CreateCall(freeFn, {eventPtr});
        builder_.CreateRet(
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 0));
    }
    
    // ═══════════════════════════════════════════════════════════════════════
    // sad_ui_process_event() — معالجة حدث واحد
    // ═══════════════════════════════════════════════════════════════════════
    
    void generateProcessEvent() {
        // int sad_ui_process_event(void* event)
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context_),
            {llvm::Type::getInt8PtrTy(context_)}, false);
        
        llvm::Function* func = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "sad_ui_process_event", module_);
        
        llvm::Value* eventPtr = func->arg_begin();
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", func);
        builder_.SetInsertPoint(entry);
        
        // قراءة نوع الحدث (أول 4 بايت من SDL_Event)
        llvm::Value* typePtr = builder_.CreateBitCast(
            eventPtr,
            llvm::PointerType::get(llvm::Type::getInt32Ty(context_), 0));
        llvm::Value* eventType = builder_.CreateLoad(
            llvm::Type::getInt32Ty(context_), typePtr);
        
        // فحص إذا كان حدث إغلاق (SDL_QUIT = 0x100)
        llvm::Value* isQuit = builder_.CreateICmpEQ(
            eventType,
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 0x100));
        
        // إرجاع 1 إذا كان إغلاق، 0 إذا ليس
        llvm::Value* result = builder_.CreateSelect(
            isQuit,
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 1),
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 0));
        
        builder_.CreateRet(result);
    }
    
    llvm::LLVMContext& context_;
    llvm::Module& module_;
    llvm::IRBuilder<> builder_;
    GraphicsTypeGenerator& types_;
    SDL2Declarations& sdl_;
};

// =============================================================================
// مولد البرنامج الرئيسي للرسومات
// Main Graphics Program Generator
// =============================================================================

/**
 * @brief مولد البرنامج الرئيسي
 * @brief Main program generator
 */
class GraphicsProgramGenerator {
public:
    GraphicsProgramGenerator(llvm::LLVMContext& ctx, llvm::Module& mod)
        : context_(ctx), module_(mod), builder_(ctx) {}
    
    /**
     * @brief توليد دالة main قياسية
     */
    void generateMain(const std::string& windowTitle, int width, int height) {
        // int main(int argc, char** argv)
        std::vector<llvm::Type*> params = {
            llvm::Type::getInt32Ty(context_),
            llvm::PointerType::get(llvm::Type::getInt8PtrTy(context_), 0)
        };
        
        llvm::FunctionType* funcType = llvm::FunctionType::get(
            llvm::Type::getInt32Ty(context_), params, false);
        
        llvm::Function* mainFunc = llvm::Function::Create(
            funcType, llvm::Function::ExternalLinkage,
            "main", module_);
        
        llvm::BasicBlock* entry = llvm::BasicBlock::Create(
            context_, "entry", mainFunc);
        builder_.SetInsertPoint(entry);
        
        // sad_ui_init()
        llvm::Function* initFn = module_.getFunction("sad_ui_init");
        if (initFn) {
            builder_.CreateCall(initFn, {});
        }
        
        // إنشاء النافذة
        llvm::Function* createWin = module_.getFunction("sad_ui_create_window");
        if (createWin) {
            // إنشاء سلسلة العنوان
            llvm::Value* title = builder_.CreateGlobalStringPtr(windowTitle);
            llvm::Value* w = llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(context_), width);
            llvm::Value* h = llvm::ConstantInt::get(
                llvm::Type::getInt32Ty(context_), height);
            
            llvm::Value* window = builder_.CreateCall(createWin, {title, w, h});
            
            // إنشاء الـ renderer
            llvm::Function* createRenderer = module_.getFunction("SDL_CreateRenderer");
            if (createRenderer) {
                llvm::Value* renderer = builder_.CreateCall(createRenderer, {
                    window,
                    llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), -1),
                    llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 
                        0x00000002 | 0x00000004) // ACCELERATED | PRESENTVSYNC
                });
                
                // حلقة الأحداث
                llvm::Function* eventLoop = module_.getFunction("sad_ui_event_loop");
                if (eventLoop) {
                    builder_.CreateCall(eventLoop, {window, renderer});
                }
                
                // تنظيف
                llvm::Function* destroyRenderer = module_.getFunction("SDL_DestroyRenderer");
                if (destroyRenderer) {
                    builder_.CreateCall(destroyRenderer, {renderer});
                }
            }
            
            // تدمير النافذة
            llvm::Function* destroyWin = module_.getFunction("sad_ui_destroy_window");
            if (destroyWin) {
                builder_.CreateCall(destroyWin, {window});
            }
        }
        
        // sad_ui_quit()
        llvm::Function* quitFn = module_.getFunction("sad_ui_quit");
        if (quitFn) {
            builder_.CreateCall(quitFn, {});
        }
        
        builder_.CreateRet(
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), 0));
    }

private:
    llvm::LLVMContext& context_;
    llvm::Module& module_;
    llvm::IRBuilder<> builder_;
};

// =============================================================================
// الفئة الرئيسية لتوليد كود الرسومات
// Main Graphics Code Generator Class
// =============================================================================

/**
 * @brief مولد كود الرسومات الرئيسي
 * @brief Main graphics code generator
 * 
 * يجمع كل المولدات ويوفر واجهة واحدة.
 */
class LLVMGraphicsCodeGen {
public:
    LLVMGraphicsCodeGen() = default;
    
    /**
     * @brief تهيئة المولد
     * @param moduleName اسم الوحدة
     * @param targetTriple المنصة المستهدفة
     */
    void initialize(const std::string& moduleName,
                   const std::string& targetTriple = "") {
        context_ = std::make_unique<llvm::LLVMContext>();
        module_ = std::make_unique<llvm::Module>(moduleName, *context_);
        
        if (!targetTriple.empty()) {
            module_->setTargetTriple(targetTriple);
        }
        
        // إنشاء المولدات
        types_ = std::make_unique<GraphicsTypeGenerator>(*context_, *module_);
        sdl_ = std::make_unique<SDL2Declarations>(*context_, *module_);
        runtime_ = std::make_unique<GraphicsRuntimeGenerator>(
            *context_, *module_, *types_, *sdl_);
        program_ = std::make_unique<GraphicsProgramGenerator>(*context_, *module_);
    }
    
    /**
     * @brief توليد كل كود الرسومات
     */
    void generateAll(const std::string& windowTitle = "Sad Application",
                    int width = 800, int height = 600) {
        // توليد الأنواع
        types_->generateAllTypes();
        
        // توليد تعريفات SDL2
        sdl_->generateAllDeclarations();
        
        // توليد دوال Runtime
        runtime_->generateAllFunctions();
        
        // توليد main
        program_->generateMain(windowTitle, width, height);
    }
    
    /**
     * @brief التحقق من صحة الوحدة
     */
    bool verify() {
        std::string error;
        llvm::raw_string_ostream errorStream(error);
        
        if (llvm::verifyModule(*module_, &errorStream)) {
            std::cerr << "خطأ في التحقق من الوحدة:\n" << error << std::endl;
            return false;
        }
        return true;
    }
    
    /**
     * @brief حفظ LLVM IR إلى ملف
     */
    bool emitToFile(const std::string& filename) {
        std::error_code ec;
        llvm::raw_fd_ostream out(filename, ec);
        
        if (ec) {
            std::cerr << "خطأ في فتح الملف: " << ec.message() << std::endl;
            return false;
        }
        
        module_->print(out, nullptr);
        return true;
    }
    
    /**
     * @brief الحصول على الوحدة
     */
    llvm::Module* getModule() { return module_.get(); }
    
    /**
     * @brief طباعة LLVM IR إلى stdout
     */
    void dump() {
        module_->print(llvm::outs(), nullptr);
    }

private:
    std::unique_ptr<llvm::LLVMContext> context_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<GraphicsTypeGenerator> types_;
    std::unique_ptr<SDL2Declarations> sdl_;
    std::unique_ptr<GraphicsRuntimeGenerator> runtime_;
    std::unique_ptr<GraphicsProgramGenerator> program_;
};

} // namespace Graphics
} // namespace LLVM
} // namespace Sad

#endif // HAS_LLVM
