/**
 * @file builtin_kernel_gpu.cpp
 * @brief (AR) وحدة الرسوميات — إدارة العرض وframebuffer
 * @brief (EN) GPU module — display and framebuffer management
 *
 * @note يتطلب: builtin_common.h أو headers متخصصة
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */

#include "interpreter_core.h"
#include "builtin_registry.h"
namespace Kgpu = Sad::Builtins::Names::KernelGpu;
#include "value.h"
#include "framebuffer.h"

#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Sad {
namespace Interpreter {

// ═══════════════════════════════════════════════════════════════
// SEH wrappers (Windows)
// ═══════════════════════════════════════════════════════════════
#ifdef _WIN32

static int sehFBInitManual(uint64_t addr, uint32_t w, uint32_t h,
                           uint32_t pitch, uint8_t bpp) {
    __try {
        auto& fb = LowLevel::FramebufferManager::getInstance();
        return fb.initManual(addr, w, h, pitch, bpp,
                             LowLevel::PixelFormat::BGRA_8888) ? 0 : -1;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehFBSetMode(uint32_t w, uint32_t h, uint8_t bpp) {
    __try {
        auto& fb = LowLevel::FramebufferManager::getInstance();
        return fb.setVideoMode(w, h, bpp) ? 0 : -1;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehFBInitGPU_inner(LowLevel::GPUInfo* gpu) {
    __try {
        auto& fb = LowLevel::FramebufferManager::getInstance();
        return fb.initGPU(*gpu) ? 0 : -1;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int sehWaitVSync() {
    __try {
        auto& fb = LowLevel::FramebufferManager::getInstance();
        fb.waitVSync();
        return 0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

#endif // _WIN32

// ═══════════════════════════════════════════════════════════════
// Registration
// ═══════════════════════════════════════════════════════════════
void registerBuiltinsKernelGPU(Interpreter& interpreter) {
    auto& fm = interpreter.getFunctionManager();

    // ═══════════════════════════════════════════════════════════════
    // تهيئة الشاشة / Framebuffer Initialization (5 functions)
    // ═══════════════════════════════════════════════════════════════
    // 1. شاشة_تهيئة_يدوي / fb_init_manual
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 5)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            uint64_t addr = static_cast<uint64_t>(args[0]->toDouble());
            uint32_t w = static_cast<uint32_t>(args[1]->toInt());
            uint32_t h = static_cast<uint32_t>(args[2]->toInt());
            uint32_t pitch = static_cast<uint32_t>(args[3]->toInt());
            uint8_t bpp = static_cast<uint8_t>(args[4]->toInt());
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehFBInitManual(addr, w, h, pitch, bpp));
#else
            auto& fb = LowLevel::FramebufferManager::getInstance();
            return std::make_shared<Data::Value>(fb.initManual(addr, w, h, pitch, bpp,
                LowLevel::PixelFormat::BGRA_8888) ? 0 : -1);
#endif
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_0), f);
    }

    // 2. شاشة_تعيين_وضع / fb_set_mode
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            uint32_t w = args.size() > 0 ? static_cast<uint32_t>(args[0]->toInt()) : 1024;
            uint32_t h = args.size() > 1 ? static_cast<uint32_t>(args[1]->toInt()) : 768;
            uint8_t bpp = args.size() > 2 ? static_cast<uint8_t>(args[2]->toInt()) : 32;
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehFBSetMode(w, h, bpp));
#else
            auto& fb = LowLevel::FramebufferManager::getInstance();
            return std::make_shared<Data::Value>(fb.setVideoMode(w, h, bpp) ? 0 : -1);
#endif
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_1), f);
    }

    // 3. شاشة_هل_مهيأ / fb_is_initialized
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            auto& fb = LowLevel::FramebufferManager::getInstance();
            return std::make_shared<Data::Value>(fb.isInitialized() ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_2), f);
    }

    // 4. شاشة_تهيئة_gpu / fb_init_gpu
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            LowLevel::GPUInfo gpu;
#ifdef _WIN32
            return std::make_shared<Data::Value>(sehFBInitGPU_inner(&gpu));
#else
            auto& fb = LowLevel::FramebufferManager::getInstance();
            return std::make_shared<Data::Value>(fb.initGPU(gpu) ? 0 : -1);
#endif
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_3), f);
    }

    // 5. شاشة_الوضع_الحالي / fb_current_mode
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            auto& fb = LowLevel::FramebufferManager::getInstance();
            auto mode = fb.getCurrentMode();
            std::string info = std::to_string(mode.width) + "x" +
                               std::to_string(mode.height) + "x" +
                               std::to_string(mode.bitsPerPixel);
            return std::make_shared<Data::Value>(info);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_4), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // بكسل متقدم / Advanced Pixel (2 functions)
    // ═══════════════════════════════════════════════════════════════
    // 6. شاشة_قراءة_نقطة / fb_get_pixel
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 2)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            int32_t x = args[0]->toInt();
            int32_t y = args[1]->toInt();
            auto& fb = LowLevel::FramebufferManager::getInstance();
            auto c = fb.getPixel(x, y);
            return std::make_shared<Data::Value>(static_cast<int>(c.toUint32()));
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_5), f);
    }

    // 7. شاشة_نقطة_شفافة / fb_pixel_alpha
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 6)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            int32_t x = args[0]->toInt();
            int32_t y = args[1]->toInt();
            uint8_t r = static_cast<uint8_t>(args[2]->toInt());
            uint8_t g = static_cast<uint8_t>(args[3]->toInt());
            uint8_t b = static_cast<uint8_t>(args[4]->toInt());
            uint8_t a = static_cast<uint8_t>(args[5]->toInt());
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.putPixelAlpha(x, y, LowLevel::Color{r, g, b, a});
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_6), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // أشكال إضافية / Additional Shapes (6 functions)
    // ═══════════════════════════════════════════════════════════════
    // 8. شاشة_دائرة_ممتلئة / fb_fill_circle
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 6)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            int32_t cx = args[0]->toInt(), cy = args[1]->toInt();
            int32_t r = args[2]->toInt();
            uint8_t cr = static_cast<uint8_t>(args[3]->toInt());
            uint8_t cg = static_cast<uint8_t>(args[4]->toInt());
            uint8_t cb = static_cast<uint8_t>(args[5]->toInt());
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.fillCircle(cx, cy, r, LowLevel::Color{cr, cg, cb, 255});
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_7), f);
    }

    // 9. شاشة_مثلث / fb_triangle
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 9)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            auto& fb = LowLevel::FramebufferManager::getInstance();
            LowLevel::Point p1{args[0]->toInt(), args[1]->toInt()};
            LowLevel::Point p2{args[2]->toInt(), args[3]->toInt()};
            LowLevel::Point p3{args[4]->toInt(), args[5]->toInt()};
            LowLevel::Color c{static_cast<uint8_t>(args[6]->toInt()),
                              static_cast<uint8_t>(args[7]->toInt()),
                              static_cast<uint8_t>(args[8]->toInt()), 255};
            fb.drawTriangle(p1, p2, p3, c);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_8), f);
    }

    // 10. شاشة_مثلث_ممتلئ / fb_fill_triangle
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 9)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            auto& fb = LowLevel::FramebufferManager::getInstance();
            LowLevel::Point p1{args[0]->toInt(), args[1]->toInt()};
            LowLevel::Point p2{args[2]->toInt(), args[3]->toInt()};
            LowLevel::Point p3{args[4]->toInt(), args[5]->toInt()};
            LowLevel::Color c{static_cast<uint8_t>(args[6]->toInt()),
                              static_cast<uint8_t>(args[7]->toInt()),
                              static_cast<uint8_t>(args[8]->toInt()), 255};
            fb.fillTriangle(p1, p2, p3, c);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_9), f);
    }

    // 11. شاشة_قوس / fb_arc
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 8)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            int32_t cx = args[0]->toInt(), cy = args[1]->toInt();
            int32_t r = args[2]->toInt();
            float start = static_cast<float>(args[3]->toDouble());
            float end = static_cast<float>(args[4]->toDouble());
            LowLevel::Color c{static_cast<uint8_t>(args[5]->toInt()),
                              static_cast<uint8_t>(args[6]->toInt()),
                              static_cast<uint8_t>(args[7]->toInt()), 255};
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.drawArc(cx, cy, r, start, end, c);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_10), f);
    }

    // 12. شاشة_مستطيل_دائري / fb_rounded_rect
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 8)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(),
                                static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
            int32_t radius = args[4]->toInt();
            LowLevel::Color c{static_cast<uint8_t>(args[5]->toInt()),
                              static_cast<uint8_t>(args[6]->toInt()),
                              static_cast<uint8_t>(args[7]->toInt()), 255};
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.drawRoundedRect(rect, radius, c);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_11), f);
    }

    // 13. شاشة_مستطيل_دائري_ممتلئ / fb_fill_rounded_rect
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 8)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(),
                                static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
            int32_t radius = args[4]->toInt();
            LowLevel::Color c{static_cast<uint8_t>(args[5]->toInt()),
                              static_cast<uint8_t>(args[6]->toInt()),
                              static_cast<uint8_t>(args[7]->toInt()), 255};
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.fillRoundedRect(rect, radius, c);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_12), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // نص وخطوط / Text & Fonts (2 functions)
    // ═══════════════════════════════════════════════════════════════
    // 14. شاشة_حرف / fb_char
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 6)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            int32_t x = args[0]->toInt(), y = args[1]->toInt();
            char ch = args[2]->toString()[0];
            LowLevel::Color fg{static_cast<uint8_t>(args[3]->toInt()),
                               static_cast<uint8_t>(args[4]->toInt()),
                               static_cast<uint8_t>(args[5]->toInt()), 255};
            LowLevel::Color bg{0, 0, 0, 0};
            if (args.size() >= 9) {
                bg = LowLevel::Color{static_cast<uint8_t>(args[6]->toInt()),
                                     static_cast<uint8_t>(args[7]->toInt()),
                                     static_cast<uint8_t>(args[8]->toInt()), 255};
            }
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.drawChar(x, y, ch, fg, bg);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_13), f);
    }

    // 15. شاشة_نص_utf8 / fb_text_utf8
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 6)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            int32_t x = args[0]->toInt(), y = args[1]->toInt();
            std::string text = args[2]->toString();
            LowLevel::Color fg{static_cast<uint8_t>(args[3]->toInt()),
                               static_cast<uint8_t>(args[4]->toInt()),
                               static_cast<uint8_t>(args[5]->toInt()), 255};
            LowLevel::Color bg{0, 0, 0, 0};
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.drawStringUTF8(x, y, text.c_str(), fg, bg);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_14), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // شفافية وحجب / Clipping & Buffering (6 functions)
    // ═══════════════════════════════════════════════════════════════
    // 16. شاشة_تعيين_قص / fb_set_clip
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 4)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(),
                                static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.setClipRect(rect);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_15), f);
    }

    // 17. شاشة_إلغاء_قص / fb_reset_clip
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.resetClipRect();
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_16), f);
    }

    // 18. شاشة_تمرير / fb_scroll
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            int32_t dx = args.size() > 0 ? args[0]->toInt() : 0;
            int32_t dy = args.size() > 1 ? args[1]->toInt() : 0;
            uint8_t r = args.size() > 2 ? static_cast<uint8_t>(args[2]->toInt()) : 0;
            uint8_t g = args.size() > 3 ? static_cast<uint8_t>(args[3]->toInt()) : 0;
            uint8_t b = args.size() > 4 ? static_cast<uint8_t>(args[4]->toInt()) : 0;
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.scroll(dx, dy, LowLevel::Color{r, g, b, 255});
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_17), f);
    }

    // 19. شاشة_تخزين_مزدوج / fb_double_buffer
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            bool enable = args.empty() ? true : (args[0]->toInt() != 0);
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.setDoubleBuffering(enable);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_18), f);
    }

    // 20. شاشة_هل_مزدوج / fb_is_double_buffered
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            auto& fb = LowLevel::FramebufferManager::getInstance();
            return std::make_shared<Data::Value>(fb.isDoubleBuffered() ? 1 : 0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_19), f);
    }

    // 21. شاشة_انتظار_متزامن / fb_wait_vsync
    // Note: waitVSync() busy-loops on VGA I/O port 0x3DA.
    // On MSVC desktop builds, IOPorts::inb() returns 0 causing infinite loop.
    // We guard with SEH on Windows and skip if not initialized.
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            (void)args;
            auto& fb = LowLevel::FramebufferManager::getInstance();
            if (!fb.isInitialized()) return std::make_shared<Data::Value>(-1);
#ifdef _WIN32
            // On desktop Windows, inb() is a stub returning 0 → infinite loop.
            // Return -1 (not supported) instead of hanging.
            return std::make_shared<Data::Value>(-1);
#else
            fb.waitVSync();
            return std::make_shared<Data::Value>(0);
#endif
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_20), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // مؤشر الفأرة / Cursor (3 functions)
    // ═══════════════════════════════════════════════════════════════
    // 22. شاشة_تحريك_مؤشر / fb_move_cursor
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 2)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.moveCursor(args[0]->toInt(), args[1]->toInt());
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_21), f);
    }

    // 23. شاشة_إظهار_مؤشر / fb_show_cursor
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            bool show = args.empty() ? true : (args[0]->toInt() != 0);
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.showCursor(show);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_22), f);
    }

    // 24. شاشة_تعيين_vsync / fb_set_vsync
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            bool enable = args.empty() ? true : (args[0]->toInt() != 0);
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.setVSync(enable);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_23), f);
    }

    // ═══════════════════════════════════════════════════════════════
    // تدرج عمودي / Vertical Gradient
    // ═══════════════════════════════════════════════════════════════
    // 25. شاشة_تدرج_عمودي / fb_gradient_v
    {
        auto f = [](Sad::Interpreter::BuiltinContext &ctx) -> std::shared_ptr<Data::Value> {
                const auto &args = ctx.args(); (void)args;
            if (args.size() < 10)
                // (AR) عودةٌ بقيمةٍ زائفةٍ صامتة ⇒ إجابةٌ معقولةُ الشكلِ خاطئةٌ يقينًا.
                ctx.error(::Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
            LowLevel::Rect rect(args[0]->toInt(), args[1]->toInt(),
                                static_cast<uint32_t>(args[2]->toInt()), static_cast<uint32_t>(args[3]->toInt()));
            LowLevel::Color c1{static_cast<uint8_t>(args[4]->toInt()),
                               static_cast<uint8_t>(args[5]->toInt()),
                               static_cast<uint8_t>(args[6]->toInt()), 255};
            LowLevel::Color c2{static_cast<uint8_t>(args[7]->toInt()),
                               static_cast<uint8_t>(args[8]->toInt()),
                               static_cast<uint8_t>(args[9]->toInt()), 255};
            auto& fb = LowLevel::FramebufferManager::getInstance();
            fb.fillGradientV(rect, c1, c2);
            return std::make_shared<Data::Value>(0);
        };
        fm.registerBuiltinFunction(std::string(Kgpu::GPU_24), f);
    }

} // registerBuiltinsKernelGPU

} // namespace Interpreter
} // namespace Sad
