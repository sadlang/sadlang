// بسم الله الرحمن الرحيم
// ============================================================================
// Compiler Driver — Middle-end (SIR optimization + SIR printing)
// مشغّل المترجم — المرحلةُ الوسطى (تحسينُ SIR وطباعتُه)
// ============================================================================
// (AR) فُصلَ هذا الملفُّ عن `compiler_driver_backend.cpp` لأنّ المرحلةَ الوسطى
//      **لا تمسُّ LLVM البتّة**: مدخلُها وحدةُ SIR ومخرجُها وحدةُ SIR، والمحسّنُ
//      (`Sad::Compiler::Optimizer`) مكتبةُ المستودعِ نفسِها. وبقاؤها في ملفٍّ
//      يُضمِّنُ ثمانيَ ترويساتِ LLVM كانَ يجعلُ استبعادَ الخلفيّةِ استبعادًا
//      للتحسينِ معها — فيختلفُ الناتجُ بينَ المترجمَين لا لسببٍ دلاليّ.
//
//      ⚠️ **هذا هو المعنى:** `sad-build` و`sad-build-native` يشتركانِ في هذه
//      الوحدةِ بعينِها، فوحدةُ SIR التي تبلغُ الخلفيّةَ الأصليّةَ واحدةٌ في
//      الاثنين، وبصمةُ ELF التي يقيسُها الحارسُ قابلةٌ للمقارنة.
//
//   - run_middleend()         : تحسينُ SIR الوسيط (O0-O3)
//   - print_ir_if_requested() : طباعةُ SIR عند الطلب
//
// (EN) Split out of compiler_driver_backend.cpp because the middle end touches
//      no LLVM at all: SIR in, SIR out, optimized by this repo's own optimizer.
//      Leaving it in a file that includes eight LLVM headers meant excluding the
//      backend also excluded optimization — so the two compilers would produce
//      different code for a non-semantic reason. Both `sad-build` and
//      `sad-build-native` compile this exact TU, so the SIR module reaching the
//      native backend is identical in both and the ELF fingerprint is comparable.
// ============================================================================

#include "compiler_driver.h"
#include "../../compiler/include/frontend/sir_module.h"
#include "../../compiler/include/sir_optimizer/optimizer.h"

#include <iostream>

namespace sad
{
    namespace driver
    {

        bool CompilerDriver::run_middleend()
        {
            if (options_.verbose)
            {
                std::cout << "  [5/5] Optimizing... / التحسين...\n";
            }

            // ========================================================================
            // (AR) مرحلة التحسين الوسيطة — SIR Optimizer
            //      هذه المرحلة تطبّق ممرات التحسين على تمثيل SIR الوسيط قبل
            //      التحويل إلى LLVM IR. الممرات المدعومة حسب مستوى التحسين:
            //
            //      O0: بلا تحسين — تُتخطى بالكامل (للتطوير والتنقيح)
            //      O1: طي الثوابت + إزالة الكود الميت
            //           - طي الثوابت: حساب التعبيرات الثابتة أثناء الترجمة بدل التنفيذ
            //             مثال: 2 + 3 → 5 مباشرة في وقت الترجمة بدل تعليمة ADD
            //           - إزالة الكود الميت: حذف التعليمات التي لا تُستخدم نتائجها
            //      O2: O1 + نشر النسخ + حذف التعبيرات الجزئية المشتركة (CSE)
            //           - نشر النسخ: تعقّب %b = %a واستبدال الاستخدامات مباشرة بـ %a
            //           - CSE: اكتشاف التعبيرات المتكررة وحساب قيمتها مرة واحدة فقط
            //      O3: O2 + دمج السجلات
            //           - دمج السجلات: تقليل عدد السجلات الافتراضية المستخدمة
            //
            // (EN) Middle-end optimization phase — SIR Optimizer
            //      Applies optimization passes on SIR intermediate representation before
            //      converting to LLVM IR. Supported passes per optimization level:
            //
            //      O0: No optimization — skipped entirely (for development/debugging)
            //      O1: Constant folding + Dead code elimination
            //      O2: O1 + Copy propagation + CSE (Common Subexpression Elimination)
            //      O3: O2 + Register coalescing
            // ========================================================================

            // (AR) تخطي التحسين في المستوى O0
            // (EN) Skip optimization at O0 level
            if (options_.opt_level == OptimizationLevel::O0)
            {
                if (options_.verbose)
                {
                    std::cout << "  ✓ Optimization skipped (O0) / تمّ تخطي التحسين (O0)\n";
                }
                return true;
            }

            // (AR) التحقق من وجود وحدة SIR صالحة
            // (EN) Verify valid SIR module exists
            if (!sir_module_)
            {
                if (options_.verbose)
                {
                    std::cout << "  ⚠ No SIR module to optimize / لا توجد وحدة SIR للتحسين\n";
                }
                return true;
            }

            // (AR) تحويل مستوى التحسين من صيغة المشغّل إلى صيغة المحسِّن
            //      Os و Oz تُعامَلان كـ O2 (تحسينات قياسية، والتركيز على الحجم يتم في مستوى LLVM لاحقاً)
            // (EN) Convert optimization level from driver format to optimizer format
            //      Os and Oz are treated as O2 (standard optimizations, size focus at LLVM level later)
            Sad::Compiler::Optimizer::OptimizationLevel optLevel;
            switch (options_.opt_level)
            {
            case OptimizationLevel::O1:
                optLevel = Sad::Compiler::Optimizer::OptimizationLevel::O1;
                break;
            case OptimizationLevel::O3:
                optLevel = Sad::Compiler::Optimizer::OptimizationLevel::O3;
                break;
            case OptimizationLevel::O2:
            case OptimizationLevel::Os:
            case OptimizationLevel::Oz:
            default:
                optLevel = Sad::Compiler::Optimizer::OptimizationLevel::O2;
                break;
            }

            // (AR) إنشاء المحسِّن وتشغيله على وحدة SIR
            // (EN) Create optimizer and run on SIR module
            try
            {
                auto optimizer = Sad::Compiler::Optimizer::createOptimizer(optLevel);

                if (options_.verbose)
                {
                    optimizer->setDebugMode(true);
                }

                bool changed = optimizer->optimize(sir_module_);

                if (options_.verbose)
                {
                    optimizer->printStats();
                    if (changed)
                    {
                        std::cout << "  ✓ Optimization applied / تمّ تطبيق التحسينات\n";
                    }
                    else
                    {
                        std::cout << "  ✓ No optimizations needed / لا توجد تحسينات لازمة\n";
                    }
                }
            }
            catch (const std::exception &e)
            {
                // (AR) خطأ في التحسين — نتابع بدون تحسين بدلاً من الفشل الكامل
                //      هذا أفضل من إيقاف عملية الترجمة بأكملها
                // (EN) Optimization error — continue without optimization instead of full failure
                //      This is better than stopping the entire compilation process
                if (options_.verbose)
                {
                    std::cerr << "  ? Optimization error (continuing): " << e.what() << "\n";
                    std::cerr << "  ⚠ خطأ في التحسين (متابعة): " << e.what() << "\n";
                }
            }

            return true;
        }

        void CompilerDriver::print_ir_if_requested()
        {
            if (options_.emit_sir && sir_module_)
            {
                std::cout << "\n========== SIR ==========\n";
                sir_module_->print(); // print() doesn't take ostream parameter
                std::cout << "=========================\n\n";
            }
        }

    } // namespace driver
} // namespace sad
// بسم الله الرحمن الرحيم
