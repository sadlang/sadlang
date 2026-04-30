// ═══════════════════════════════════════════════════════════════════════════════
// (AR) policy_bridge.h — جسر تطبيق سياسة الذاكرة على المحرك الموحَّد + الكائنات.
//
// (AR) هذه الواجهة هي **النقطة الواحدة الموحَّدة** لتطبيق `MemoryModeSettings`
//      على نظام التشغيل الفعلي:
//        1. تشغيل/إيقاف محرك الـ GC (`Sad::Memory::GC::defaultEngine()`).
//        2. تركيب/إزالة hooks على `Sad::Data::ObjectInstance` (إنشاء/هدم).
//        3. الطباعة التشخيصية الموحَّدة عند `debugMode = true`.
//
// (AR) الهدف من هذا الجسر — حفظ مبدأ **DRY (CW-19)** و**One Consistent Fix
//      (BF-11)**: قبل هذا الملف كان منطق التطبيق منسوخاً داخل المفسّر فقط
//      (B-step3 + B-step4). DEF-002 طلب نفس التطبيق من الـ VM launcher
//      وربما لاحقاً من REPL/LSP/WASM. كل launcher جديد كان سيكرّر النمط.
//      الآن: نقطة استدعاء واحدة، تنفيذ متماثل، تطوير مستقبلي بمكان واحد.
//
// (EN) policy_bridge.h — Single unified entry point for applying
//      `MemoryModeSettings` to the runtime: pauses/resumes the GC engine,
//      installs/clears `ObjectInstance` lifecycle hooks, and emits the
//      standard diagnostic line under `debugMode`.
//
// (EN) Honours CW-19 (DRY) and BF-11 (consistent fix). Replaces the duplicated
//      block inside `interpreter_core.cpp` and unblocks identical wiring from
//      the VM launcher and any future execution front-end.
//
// @phase Architecture Refactor: Memory Layer Unification — Phase B / DEF-002
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "memory/policy/gc_mode.h" // (AR) MemoryModeSettings + GCStrategy
                                   // (EN) MemoryModeSettings + GCStrategy

namespace Sad
{
    namespace Memory
    {
        namespace GC
        {

            // ─────────────────────────────────────────────────────────────────────────────
            // (AR) applyMemoryPolicyGlobal — تطبيق سياسة وضع الذاكرة على المحرك العام.
            //
            // (AR) المعاملات:
            //        settings   — إعدادات الوضع (--dev/--prod/--learn) المُحلَّلة من CLI.
            //        policySet  — هل وُجدت أعلام CLI فعلاً؟ إذا false → الدالة no-op
            //                     (تحفظ التوافق مع المنفذين بدون أعلام: REPL، LSP).
            //        debugMode  — هل نطبع سطر التشخيص الموحَّد؟ يُمرَّر من
            //                     `enableDebugMode` لكل منفذ.
            //
            // (AR) الأثر الجانبي:
            //        - عند gcStrategy == None : pause() على defaultEngine + clearHooks().
            //        - وإلا                  : resume() على defaultEngine + setAllocHook +
            //                                  setFreeHook (يحوّلان إلى register/unregister).
            //        - عند debugMode         : سطر `[memory] GC engine: ...` على std::cerr.
            //
            // (AR) ملاحظات أمان الخيوط:
            //        - defaultEngine() يستخدم Meyer's singleton (آمن للخيوط في C++17).
            //        - hooks على ObjectInstance محمية بـ mutex داخلي (B-step4).
            //        - الدالة نفسها idempotent: استدعاؤها مرتين بنفس الإعدادات يعطي نفس
            //          الحالة بدون آثار جانبية إضافية.
            //
            // (EN) Applies the resolved memory policy to the global GC engine and the
            //      ObjectInstance lifecycle hooks. No-op when `policySet == false` to
            //      preserve legacy behaviour for callers that don't yet parse CLI flags.
            // ─────────────────────────────────────────────────────────────────────────────
            void applyMemoryPolicyGlobal(const ::Sad::Memory::MemoryModeSettings &settings,
                                         bool policySet,
                                         bool debugMode);

        } // namespace GC
    } // namespace Memory
} // namespace Sad
