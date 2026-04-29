# Archived: compiler/src/pipeline + compiler/include/pipeline

تم نقل هذا المحتوى من `compiler/{src,include}/pipeline/` إلى الأرشيف بتاريخ النقل لأن جميع الرموز التي يُصدّرها كانت ميتة في وقت التشغيل (0 مستدعين خارج المجلد):

- `LLVMCompilerPipeline` — منسّق LLVM بديل لم يُعتمد قط؛ السائق الفعلي في `tools/compiler/compiler_driver_*.cpp`.
- `sad::compiler::freestanding::FreestandingCodeGen` — مكرّر للنسخة الفعلية في `compiler/src/backend/llvm/builders/core/freestanding_codegen.h`.
- `NoStdModeManager` + `NoStdIntegration` — السائق يعيد كشف `#![no_std]` بنفسه في `compiler_driver_analysis.cpp`.
- `compiler_options.h` — لم يُضمَّن من أي مكان خارج المجلد.

السبب الجذري للأرشفة: انتهاك CW-02 (تسلسل الطبقات) و CW-19 (DRY) و CW-23 (قابلية الاختبار).
