/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: interpreter/include/ui/sad_graphics_bridge.h
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * (AR) الواجهة العامّة لطبقة جسر الواجهات (sad_graphics_bridge) — م2-أ من sadlang-rfcs#10.
 *      نقطة الدخول الوحيدة التي يحتاجها مضيف مفسّر يربط sad_graphics_bridge وينفّذ كود
 *      واجهات (sad-run، profiler): استدعِ installSadGraphicsBridge() مرّةً على الخيط
 *      الرئيسيّ قبل إنشاء أيّ مفسّر قد ينفّذ كود واجهات. عديمة التكرار. المضيفون بلا
 *      رسومات (مثل wasm/sad-build) لا يستدعونها، والمسار البديل يجعل الرسومات بلا
 *      أثر بلا انهيار. لا تضمّن الترويسة الخاصّة ui_eval_bridge_impl.h من خارج الجسر.
 *
 * (EN) Public surface of the UI bridge layer (sad_graphics_bridge), Phase 2-A. The single
 *      entry point a host that links sad_graphics_bridge and runs UI code needs (sad-run,
 *      profiler): call installSadGraphicsBridge() once on the main thread before
 *      constructing any interpreter that may run UI code. Idempotent. Headless hosts
 *      (wasm, sad-build) do not call it; the null-bridge fallback makes UI a no-op.
 *      Do not include the private ui_eval_bridge_impl.h from outside the bridge.
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_INTERPRETER_SAD_GRAPHICS_BRIDGE_H
#define SAD_INTERPRETER_SAD_GRAPHICS_BRIDGE_H

namespace Sad
{
    namespace Interpreter
    {
        // (AR) يثبّت بذرة تقييم الواجهات ويسجّل مزوّد وحدات الواجهات. عديمة التكرار:
        //      استدعاؤها أكثر من مرّة لا أثر له. استدعِها قبل إنشاء المفسّر.
        // (EN) Installs the UI evaluation seam and registers the UI module provider.
        //      Idempotent; safe to call more than once. Call before interpreter creation.
        void installSadGraphicsBridge();
    }
}

#endif // SAD_INTERPRETER_SAD_GRAPHICS_BRIDGE_H
