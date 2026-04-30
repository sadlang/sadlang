// ═══════════════════════════════════════════════════════════════════════════════
// (AR) policy_bridge.cpp — تنفيذ جسر تطبيق سياسة الذاكرة الموحَّد.
// (EN) policy_bridge.cpp — Implementation of the unified memory-policy bridge.
//
// (AR) راجع `policy_bridge.h` للسياق المعماري الكامل والمسوّغات.
// (EN) See `policy_bridge.h` for the full architectural rationale.
// ═══════════════════════════════════════════════════════════════════════════════

#include "memory/gc/policy_bridge.h"

#include <iostream>
#include <cstddef>

#include "memory/gc/engine/garbage_collector.h"   // (AR) defaultEngine + GarbageCollector
#include "object_instance.h"                       // (AR) ObjectInstance + hooks API

namespace Sad {
namespace Memory {
namespace GC {

void applyMemoryPolicyGlobal(const ::Sad::Memory::MemoryModeSettings& settings,
                             bool policySet,
                             bool debugMode)
{
    // ─────────────────────────────────────────────────────────────────────────
    // (AR) حماية أساسية: بدون سياسة معتمَدة من CLI لا نلمس الحالة العامة.
    //      هذا يحفظ السلوك القديم لمنفذي التشغيل الذين لا يحلّلون الأعلام بعد
    //      (REPL، LSP، أدوات قديمة). توافق خلفي صريح (BF-15).
    // (EN) Guard: without explicit CLI policy do not mutate global state —
    //      preserves legacy behaviour for callers without flag parsing.
    // ─────────────────────────────────────────────────────────────────────────
    if (!policySet)
    {
        return;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // (AR) (1) تطبيق حالة المحرك العام بحسب gcStrategy.
    //
    //      الدلالات (وفق docs/معمارية_الذاكرة_الموحدة.md):
    //         --dev   → MarkAndSweep → resume()  (المحرك يعمل ويتعقَّب)
    //         --prod  → None         → pause()   (المحرك مُعطَّل، أداء أقصى)
    //         --learn → MarkAndSweep → resume()  (يعمل + تحذيرات ملكية فقط)
    //
    // (EN) (1) Apply engine state according to gcStrategy.
    // ─────────────────────────────────────────────────────────────────────────
    auto& gcEngine = ::Sad::Memory::GC::defaultEngine();
    const bool gcDisabled = (settings.gcStrategy == ::Sad::Memory::GCStrategy::None);

    if (gcDisabled)
    {
        gcEngine.pause();
    }
    else
    {
        gcEngine.resume();
    }

    // ─────────────────────────────────────────────────────────────────────────
    // (AR) (2) الطباعة التشخيصية الموحَّدة — نص ثابت يستخدمه المفسّر والـ VM
    //      وأي منفذ مستقبلي بنفس الشكل، لتسهيل الاختبارات والتشخيص الموحَّد.
    // (EN) (2) Standard diagnostic line — identical across interpreter/VM/etc.
    // ─────────────────────────────────────────────────────────────────────────
    if (debugMode)
    {
        const auto stats = gcEngine.getStats();
        std::cerr << "[memory] GC engine: paused="
                  << (stats.paused ? "true" : "false")
                  << " strategy=" << static_cast<int>(settings.gcStrategy)
                  << std::endl;
    }

    // ─────────────────────────────────────────────────────────────────────────
    // (AR) (3) تركيب/إزالة hooks على ObjectInstance.
    //
    //      عند None → نزيل الـ hooks لاستعادة السلوك الأصلي 100% (لا تعقُّب،
    //      لا overhead). عند MarkAndSweep أو غيرها → نسجّل lambdas تحوّلان
    //      إنشاء/هدم كل ObjectInstance إلى register/unregister على المحرك.
    //
    // (AR) ملاحظات:
    //        - shared/types لا تعتمد على shared/memory_gc وقت الترجمة — الربط
    //          هنا ديناميكي عبر std::function (CW-02: تسلسل الطبقات محفوظ).
    //        - الـ hooks ستاتيكية على مستوى process — تطبيق واحد كافٍ، أي
    //          منفذ ينشئ ObjectInstance بعد ذلك سيرى نفس السلوك.
    //        - حياة ObjectInstance لا تزال shared_ptr — الـ hook إحصائي بحت.
    //
    // (EN) (3) Install or clear ObjectInstance lifecycle hooks. Hooks are
    //      process-global statics; one application is enough for all paths.
    // ─────────────────────────────────────────────────────────────────────────
    if (gcDisabled)
    {
        ::Sad::Data::ObjectInstance::clearHooks();
    }
    else
    {
        ::Sad::Data::ObjectInstance::setAllocHook(
            [](::Sad::Data::ObjectInstance* obj, std::size_t size) {
                ::Sad::Memory::GC::defaultEngine().registerObject(
                    static_cast<void*>(obj), size);
            });

        ::Sad::Data::ObjectInstance::setFreeHook(
            [](::Sad::Data::ObjectInstance* obj) {
                ::Sad::Memory::GC::defaultEngine().unregisterObject(
                    static_cast<void*>(obj));
            });
    }
}

} // namespace GC
} // namespace Memory
} // namespace Sad
