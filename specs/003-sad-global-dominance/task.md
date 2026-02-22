# مهام تجهيز مترجم ص لدعم نظام النحلة (BeeOS) — الـ 30% المتبقية

**فرع الميزة**: `003-sad-global-dominance`  
**تاريخ الإنشاء**: 10 فبراير 2026  
**الحالة**: مسودة  
**الهدف**: سد كل الثغرات في المترجم التي تمنع ترجمة نواة نظام تشغيل (BeeOS) بلغة ص

---

## ملخص التحليل

| البند | القيمة |
|-------|--------|
| جاهزية المترجم الحالية | ~70% |
| المطلوب للوصول لـ 100% | 48 مهمة |
| ملفات C++ جديدة | 31 ملف |
| ملفات C++ تعديل | 22 ملف |
| المدة التقديرية | 8-12 أسبوع |
| المطورون المطلوبون | 2-3 مطورين C++ |

---

## جدول المحتويات

1. [المرحلة أ: إصلاح ربط الأنواع (الأعلى أولوية)](#المرحلة-أ)
2. [المرحلة ب: تفعيل البناء وتكامل خط الأنابيب](#المرحلة-ب)
3. [المرحلة ج: بيئة التشغيل بدون نظام (Freestanding Runtime)](#المرحلة-ج)
4. [المرحلة د: مولد كود Bare-Metal](#المرحلة-د)
5. [المرحلة هـ: إدارة الذاكرة للنواة](#المرحلة-هـ)
6. [المرحلة و: نظام الأنواع المركبة](#المرحلة-و)
7. [المرحلة ز: الربط والإخراج الثنائي](#المرحلة-ز)
8. [المرحلة ح: الاختبار والتحقق](#المرحلة-ح)

---

## المرحلة أ: إصلاح ربط الأنواع (الأعلى أولوية — حاجز حرج)

> **السبب**: `LLVMCodeGen::convertType()` في `llvm_codegen.cpp` سطر 243 يُرجع `i64` لكل الأنواع.
> `LLVMTypeMapper` موجود ويعمل لكنه **غير مربوط** بـ `convertType()`.
> بدون هذا الإصلاح لا يمكن ترجمة أي بنية بيانات للنواة (جداول صفحات، IDT، GDT، إلخ).

---

### م-أ01: ~~ربط LLVMTypeMapper بـ LLVMCodeGen::convertType()~~ ✅ مكتمل

**الأولوية**: حرجة — حاجز رقم 1  
**التقدير**: 3 أيام  
**التبعيات**: لا يوجد

**الوصف**: تعديل `convertType()` ليستدعي `LLVMTypeMapper::mapSadType()` بدلاً من إرجاع `i64` دائماً. يتطلب إنشاء مثيل `LLVMTypeMapper` داخل `LLVMCodeGen` أو حقنه كاعتمادية.

**ملفات التعديل (C++)**:
| الملف | الإجراء | الوصف |
|-------|---------|-------|
| `src/backend/llvm/llvm_codegen.cpp` | تعديل | تبديل `convertType()` لاستدعاء `LLVMTypeMapper` |
| `src/backend/llvm/llvm_codegen.h` | تعديل | إضافة عضو `LLVMTypeMapper* type_mapper_` |
| `src/backend/llvm/llvm_type_mapper.cpp` | تعديل | إضافة دوال مساعدة للأنواع المفقودة |
| `src/backend/llvm/llvm_type_mapper.h` | تعديل | تحديث الواجهة |

**معايير القبول**:
- `convertType(نوع_صحيح_32)` يُرجع `i32` وليس `i64`
- `convertType(نوع_بنية)` يُرجع `StructType` صحيح
- `convertType(نوع_مؤشر)` يُرجع `PointerType`
- كل الاختبارات الحالية تمر بنجاح

---

### م-أ02: ~~إضافة ربط الأنواع المركبة في TypeMapper~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 4 أيام  
**التبعيات**: م-أ01

**الوصف**: `LLVMTypeMapper` يدعم الأنواع البدائية والمؤشرات والأصناف، لكن ينقصه دعم المصفوفات، الصفوف (tuples)، التعدادات بقيم (tagged unions)، ونوع الدالة (function pointers). هذه ضرورية لهياكل بيانات النواة.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_type_mapper_composite.cpp` | ربط المصفوفات، الصفوف، الأنواع الجبرية → LLVM types |
| `src/backend/llvm/llvm_type_mapper_composite.h` | إعلانات ربط الأنواع المركبة |

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/llvm/llvm_type_mapper.cpp` | إضافة استدعاءات للمُنسّق الجديد |
| `src/types/type_checker.cpp` | إصلاح إرجاع `AnyType` للأنواع المركبة |
| `src/types/type_checker.h` | تحديث تواقيع الدوال |

**معايير القبول**:
- `مصفوفة<ع32, 256>` → `[256 x i32]`
- `(ع8, ع16, ع64)` → `{i8, i16, i64}`
- `تعداد نتيجة { نجاح(ع32), فشل(ع8) }` → `{i8, [8 x i8]}` (tagged union)
- مؤشر دالة `دالة(ع32) -> ع64` → `i64 (i32)*`

---

### م-أ03: ~~ربط أنواع volatile والذاكرة المعيّنة~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 2 أيام  
**التبعيات**: م-أ01

**الوصف**: نواة BeeOS تحتاج الوصول لسجلات الأجهزة (MMIO) عبر `متطاير<ن>`. يجب أن يُولّد المترجم تعليمات `load volatile`/`store volatile` في LLVM IR.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_volatile_ops.cpp` | توليد عمليات volatile load/store |
| `src/backend/llvm/llvm_volatile_ops.h` | إعلانات |

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/llvm/llvm_codegen.cpp` | إضافة فحص `isVolatile` عند توليد load/store |
| `src/types/type_checker.cpp` | التعرف على `متطاير<ن>` كنوع مُغلّف |

**معايير القبول**:
- `*متطاير_مؤشر = قيمة` ← `store volatile i32 %val, i32* %ptr`
- `متغير س = *متطاير_مؤشر` ← `%x = load volatile i32, i32* %ptr`

---

## المرحلة ب: تفعيل البناء وتكامل خط الأنابيب

> **السبب**: ملفات البنية التحتية للنواة (`entry_point.cpp`, `linker_script.cpp`, `interrupt_handlers.cpp`, `panic_handler.cpp`, `inline_asm.cpp`) موجودة في `src/backend/` لكنها **لا تُترجم** لأن `CMakeLists.txt` يعمل glob فقط على `src/backend/llvm/*.cpp`. كذلك `no_std_mode.cpp` في `src/pipeline/` غير مدمج في خط الأنابيب الرئيسي.

---

### م-ب01: ~~إضافة ملفات backend للبناء في CMakeLists.txt~~ ✅ مكتمل

**الأولوية**: حرجة — حاجز رقم 2  
**التقدير**: 1 يوم  
**التبعيات**: لا يوجد

**الوصف**: إضافة `src/backend/*.cpp` و `src/pipeline/no_std_mode.cpp` لأهداف البناء في CMake.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `CMakeLists.txt` | إضافة glob لـ `src/backend/*.cpp` و `src/pipeline/*.cpp` |

**معايير القبول**:
- `cmake --build .` ينجح بدون أخطاء
- الملفات التالية تُترجم: `entry_point.cpp`, `linker_script.cpp`, `interrupt_handlers.cpp`, `panic_handler.cpp`, `inline_asm.cpp`, `c_abi.cpp`, `no_std_mode.cpp`

---

### م-ب02: ~~دمج NoStdModeManager في خط الأنابيب الرئيسي~~ ✅ مكتمل

**الأولوية**: حرجة — حاجز رقم 3  
**التقدير**: 3 أيام  
**التبعيات**: م-ب01

**الوصف**: `NoStdModeManager` في `no_std_mode.cpp` يحلل سمة `#![بلا_مكتبة_قياسية]` ويُولد كود بدء تشغيل، لكنه **لا يُستدعى من أي مكان** في خط الترجمة. يجب ربطه بالـ pipeline الرئيسي بحيث عند وجود السمة ← تتفعل وضعية bare-metal تلقائياً.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/pipeline/no_std_integration.cpp` | نقطة التكامل بين NoStdModeManager وخط الترجمة الرئيسي |
| `src/pipeline/no_std_integration.h` | إعلانات |

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/pipeline/no_std_mode.cpp` | تحويل توليد IR من نصوص string إلى استدعاءات IRBuilder |
| `src/pipeline/pipeline.cpp` أو ما يعادله | إضافة خطوة فحص `#![بلا_مكتبة_قياسية]` |
| `src/frontend/parser.cpp` | التأكد من تحليل سمة الملف `#![...]` |

**معايير القبول**:
- ملف ص بسمة `#![بلا_مكتبة_قياسية]` ← لا تُربط المكتبة القياسية
- يُولد كود بدء التشغيل (`_start`, BSS clear, stack init) عبر IRBuilder
- يفشل الترجمة بخطأ واضح إذا لم تتوفر `#[معالج_ذعر]`

---

### م-ب03: ~~تفعيل مكتبة المُحسّن (Optimizer)~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 2 أيام  
**التبعيات**: لا يوجد

**الوصف**: مكتبة `sad_new_optimizer` معطلة في CMakeLists بسبب "PDB locking issue". النواة تحتاج تحسينات LLVM (خاصة inlining للدوال الصغيرة في ISR). يجب إصلاح مشكلة القفل وتفعيل المكتبة.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `CMakeLists.txt` | إزالة تعليق `sad_new_optimizer` وإصلاح PDB locking |
| `src/optimizer/pass_manager.cpp` | إصلاح مشكلة الـ concurrent PDB access |

**معايير القبول**:
- `--تحسين 2` يُفعّل تحسينات LLVM O2
- الدوال المُعلّمة بـ `#[مضمنة_دائماً]` تُدمج

---

### م-ب04: ~~تفعيل مكتبة الأمان (Security)~~ ✅ مكتمل

**الأولوية**: متوسطة  
**التقدير**: 2 أيام  
**التبعيات**: لا يوجد

**الوصف**: مكتبة الأمان (Triple Guard) معطلة بسبب مشاكل ترميز Unicode. النواة تحتاج تحليل التلويث (taint analysis) لمنع ثغرات تصعيد الصلاحيات.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `CMakeLists.txt` | إزالة تعليق مكتبة الأمان |
| `src/security/taint_tracker.cpp` | إصلاح ترميز Unicode في أسماء المتغيرات |
| `src/security/taint_tracker.h` | تحديث لدعم UTF-8 |

**معايير القبول**:
- تحليل التلويث يعمل مع أسماء متغيرات عربية
- `صد ترجم --تحليل-أمان` لا يتعطل

---

## المرحلة ج: بيئة التشغيل بدون نظام (Freestanding Runtime)

> **السبب**: بيئة التشغيل الوحيدة (`sad_runtime.h`) تعتمد على `<string>`, `<vector>`, `<filesystem>`, `malloc()` — كلها غير متاحة في البيئة المعدنية. يجب إنشاء مكتبة `أساسي` (core library) لا تعتمد على أي نظام تشغيل.

---

### م-ج01: ~~إنشاء بيئة تشغيل Freestanding أساسية~~ ✅ مكتمل

**الأولوية**: حرجة — حاجز رقم 4  
**التقدير**: 5 أيام  
**التبعيات**: م-أ01

**الوصف**: إنشاء بيئة تشغيل بدون أي اعتمادية على libc/libstdc++. توفر: `memcpy`, `memset`, `memcmp`, `memmove`, واجهة ذعر، أنواع بدائية.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `runtime/freestanding/sad_core.h` | الرأس الرئيسي — لا يستورد أي شيء من libc |
| `runtime/freestanding/sad_core.cpp` | تنفيذ `memcpy`, `memset`, `memcmp`, `memmove` بالتجميع |
| `runtime/freestanding/sad_panic.h` | واجهة معالج الذعر — يُوفّرها مطور النواة |
| `runtime/freestanding/sad_panic.cpp` | معالج ذعر افتراضي (حلقة لا نهائية + `hlt`) |
| `runtime/freestanding/sad_types.h` | تعريفات الأنواع البدائية (`ع8`-`ع64`, `ح8`-`ح64`, `حجم`, `منطق`) بدون STL |

**معايير القبول**:
- يُترجم بـ `-ffreestanding -nostdlib -nostdinc`
- لا يحتوي على أي `#include` من مكتبة C/C++ القياسية
- `memcpy` يعمل بشكل صحيح (اختبار على QEMU)
- حجم الملف الناتج < 4KB

---

### م-ج02: ~~إنشاء واجهة المُخصّص العام (Global Allocator Interface)~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 3 أيام  
**التبعيات**: م-ج01

**الوصف**: لاستخدام `قائمة<ن>` أو `نص` في النواة يجب توفير مُخصّص ذاكرة. ننشئ واجهة `مُخصّص_عام` يُنفّذها مطور النواة (مثلاً: bump allocator, buddy allocator).

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `runtime/freestanding/sad_allocator.h` | واجهة `SadGlobalAllocator` — `allocate()`, `deallocate()`, `reallocate()` |
| `runtime/freestanding/sad_allocator.cpp` | مُخصّص افتراضي فارغ (يُذعر عند الاستدعاء حتى يُوفّر بديل) |
| `runtime/freestanding/sad_bump_allocator.h` | مُخصّص bump بسيط كمثال/اختبار |
| `runtime/freestanding/sad_bump_allocator.cpp` | تنفيذ bump allocator |

**معايير القبول**:
- `#[مُخصّص_عام] ثابت مُخصّص: مُخصّصي = مُخصّصي::جديد()` ← يربط المُخصّص
- بدون مُخصّص ← خطأ ترجمة واضح عند استخدام تخصيص ديناميكي
- bump allocator يمر باختبارات الوحدة

---

### م-ج03: ~~إنشاء واجهة أدخال/إخراج المنافذ (Port I/O)~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 2 أيام  
**التبعيات**: م-ج01

**الوصف**: نواة BeeOS تحتاج `in`/`out` لـ x86 للتعامل مع: المنفذ التسلسلي (UART 0x3F8)، PIC (0x20/0xA0)، PIT (0x40)، PS/2 (0x60/0x64). ننشئ intrinsics تُترجم مباشرة لتعليمات تجميع.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `runtime/freestanding/sad_port_io.h` | إعلانات: `port_in_u8()`, `port_out_u8()`, `port_in_u16()`, إلخ |
| `runtime/freestanding/sad_port_io.cpp` | تنفيذ بتجميع مُضمّن x86 (`in al, dx` / `out dx, al`) |
| `src/backend/llvm/llvm_port_io_intrinsics.cpp` | توليد LLVM IR لاستدعاءات I/O بدون استدعاء دالي (inline always) |
| `src/backend/llvm/llvm_port_io_intrinsics.h` | إعلانات |

**معايير القبول**:
- `منفذ_إدخال<ع8>(0x3F8)` ← `in al, 0x3F8` في التجميع الناتج
- `منفذ_إخراج<ع8>(0x3F8, بايت)` ← `out 0x3F8, al`
- العمليات مُضمّنة دائماً (لا overhead لاستدعاء دالة)

---

## المرحلة د: مولد كود Bare-Metal

> **السبب**: الملفات الموجودة (`entry_point.cpp`, `interrupt_handlers.cpp`, `linker_script.cpp`, `inline_asm.cpp`) تُولّد LLVM IR كنصوص (string concatenation) وليس عبر LLVM IRBuilder API. هذا يعني أنها **معزولة** عن خط توليد الكود الرئيسي ولا يمكن استخدامها فعلياً.

---

### م-د01: ~~تحويل EntryPointCodeGenerator لاستخدام IRBuilder~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 4 أيام  
**التبعيات**: م-أ01, م-ب02

**الوصف**: `entry_point.cpp` يُولد LLVM IR كنصوص. يجب تحويله ليستخدم `llvm::IRBuilder` مباشرة، ودمجه مع `LLVMCodeGen` بحيث سمة `#[نقطة_دخول]` تُفعّل توليد `_start` تلقائياً.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_entry_point.cpp` | نسخة IRBuilder من مولد نقاط الدخول |
| `src/backend/llvm/llvm_entry_point.h` | إعلانات |

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/entry_point.cpp` | الاحتفاظ به كمرجع — النسخة الجديدة تحل محله |
| `src/backend/llvm/llvm_codegen.cpp` | إضافة استدعاء `LLVMEntryPoint::generate()` عند كشف `#[نقطة_دخول]` |

**معايير القبول**:
- `#[نقطة_دخول] دالة نواة_رئيسية()` ← يُولد `_start` بـ BSS clear + data init + stack setup + call
- `_start` مُعلّم بـ `naked` (بدون prologue/epilogue)
- اختبار: QEMU يبدأ ويصل لدالة النواة

---

### م-د02: ~~تحويل InterruptCodeGenerator لاستخدام IRBuilder~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 5 أيام  
**التبعيات**: م-د01

**الوصف**: `interrupt_handlers.cpp` يُولد جداول IDT ومعالجات المقاطعات كنصوص IR. يجب تحويله لـ IRBuilder مع دعم x86_64 (IDT 256 مُدخل) و ARM (NVIC).

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_interrupt_handlers.cpp` | توليد ISR wrappers + IDT بـ IRBuilder |
| `src/backend/llvm/llvm_interrupt_handlers.h` | إعلانات |

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/interrupt_handlers.cpp` | الاحتفاظ كمرجع |
| `src/backend/llvm/llvm_codegen.cpp` | ربط سمة `#[معالج_مقاطعة(رقم)]` بمولد المقاطعات |

**معايير القبول**:
- `#[معالج_مقاطعة(14)]` ← ISR wrapper مع حفظ/استعادة السجلات + `iretq`
- جدول IDT يُولد بشكل صحيح (256 مُدخل)
- `ماكرو جدول_مقاطعات!` يعمل

---

### م-د03: ~~تحويل InlineAsmCodeGenerator لاستخدام IRBuilder~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 3 أيام  
**التبعيات**: م-أ01

**الوصف**: `inline_asm.cpp` يحوّل كلمات عربية (مثل `توقف`→`hlt`) ويُولد IR كنصوص. يجب أن يُولد `llvm::InlineAsm` objects مباشرة عبر API.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/inline_asm.cpp` | تحويل من string-based إلى IRBuilder-based |
| `src/backend/llvm/llvm_codegen.cpp` | ربط `تجميع! { ... }` بالمولد الجديد |

**معايير القبول**:
- `تجميع! { "cli" }` ← `call void asm sideeffect "cli", ""()`
- المدخلات/المخرجات تُربط بالسجلات عبر LLVM inline asm constraints
- `#[معمارية("x86_64")]` يُفلتر التجميع حسب الهدف

---

### م-د04: ~~دمج LLVMKernelSyncCodegen في خط الأنابيب~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 2 أيام  
**التبعيات**: م-أ01

**الوصف**: `llvm_kernel_sync.cpp` يحتوي على spinlock, ticket lock, memory barriers — كلها صحيحة لكنها تُولد IR كنصوص ولا يمكن استدعاؤها من الكود العادي. يجب ربطها بالمكتبة القياسية الأساسية.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/llvm/llvm_kernel_sync.cpp` | تحويل من string-based إلى IRBuilder |
| `src/backend/llvm/llvm_codegen.cpp` | إضافة توليد atomics عند استدعاء `ذري<ن>.قارن_وبدّل()` إلخ |

**معايير القبول**:
- `قفل_دوراني.اقفل()` ← CAS loop مع `pause` hint
- `حاجز_ذاكرة(متسلسل)` ← `fence seq_cst`
- `عطّل_المقاطعات()` / `فعّل_المقاطعات()` ← `cli`/`sti`

---

### م-د05: ~~إضافة دعم ترويسة Multiboot2 / UEFI~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 3 أيام  
**التبعيات**: م-د01

**الوصف**: `entry_point.cpp` يُولد `_start` لكن بدون ترويسة Multiboot2 أو UEFI. بدون هذا لا يمكن لـ GRUB أو أي bootloader تحميل النواة.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_multiboot.cpp` | توليد ترويسة Multiboot2 كبيانات ثابتة في `.multiboot` section |
| `src/backend/llvm/llvm_multiboot.h` | إعلانات |
| `src/backend/llvm/llvm_uefi_entry.cpp` | توليد نقطة دخول UEFI `efi_main(ImageHandle, SystemTable)` |
| `src/backend/llvm/llvm_uefi_entry.h` | إعلانات |

**معايير القبول**:
- `#[إقلاع(multiboot2)]` ← يُضاف magic `0xE85250D6` + header في `.multiboot`
- `#[إقلاع(uefi)]` ← يُولد `efi_main` بتوقيع UEFI صحيح
- GRUB يتعرف على الملف الناتج باستخدام `grub-file --is-x86-multiboot2`

---

### م-د06: ~~إضافة دعم إدارة سجلات المعالج (CR0-CR4, MSRs)~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 3 أيام  
**التبعيات**: م-د03

**الوصف**: النواة تحتاج قراءة/كتابة سجلات التحكم (CR0, CR3, CR4) وسجلات MSR لتفعيل الترحيل (paging)، الوضع الطويل (long mode)، APIC إلخ.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_cpu_regs.cpp` | توليد inline asm لقراءة/كتابة CR0-CR4, EFLAGS, MSRs |
| `src/backend/llvm/llvm_cpu_regs.h` | إعلانات |

**معايير القبول**:
- `سجلات::cr3::اقرأ()` ← `mov rax, cr3`
- `سجلات::cr3::اكتب(قيمة)` ← `mov cr3, rax`
- `سجلات::msr::اقرأ(0xC0000080)` ← `rdmsr` مع ECX = 0xC0000080
- `سجلات::msr::اكتب(0xC0000080, قيمة)` ← `wrmsr`

---

### م-د07: ~~إضافة دعم جداول الصفحات (Paging Support)~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 4 أيام  
**التبعيات**: م-د06, م-أ02

**الوصف**: BeeOS يحتاج 4-level paging (PML4→PDPT→PD→PT). يجب توفير أنواع آمنة لجداول الصفحات مع تضمين `invlpg` لتحديث TLB.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_paging.cpp` | توليد كود لبنى جداول الصفحات وتعليمات `invlpg`, `mov cr3` |
| `src/backend/llvm/llvm_paging.h` | إعلانات |

**معايير القبول**:
- بنية `مُدخل_صفحة` (64-bit) مع حقول بت: present, writable, user, write-through, cache-disable, accessed, dirty, huge, global, NX
- `أبطل_صفحة(عنوان)` ← `invlpg [rax]`
- أنواع `جدول_صفحات<المستوى>` آمنة النوع

---

## المرحلة هـ: إدارة الذاكرة للنواة

> **السبب**: `llvm_memory_manager.cpp` يحتوي على stubs لـ mark-and-sweep و cycle detection (كلها TODO). في وضع النواة يجب أن يعمل نظام الملكية (بدون GC) وأن يُنفّذ borrow checker ref/deref.

---

### م-هـ01: ~~إصلاح borrow checker — دعم المراجع والتحرير~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 4 أيام  
**التبعيات**: لا يوجد

**الوصف**: `borrow_checker.cpp` فيه `visitUnaryExpr` بـ TODO لعمليات reference (`&`) و dereference (`*`). بدون هذا لا يمكن استخدام المراجع (borrowing) في كود النواة.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/borrow/borrow_checker.cpp` | تنفيذ `visitUnaryExpr` لـ `&` و `&متغير` و `*` |
| `src/borrow/borrow_checker.h` | إضافة تواقيع الدوال الجديدة |
| `src/borrow/lifetime_analyzer.cpp` | ربط lifetimes للمراجع المنشأة |

**معايير القبول**:
- `&قيمة` ← إنشاء استعارة قراءة، borrow checker يتتبعها
- `&متغير قيمة` ← إنشاء استعارة كتابة حصرية
- `*مؤشر` ← فحص أن المؤشر مستعار بشكل صحيح
- `&قيمة` و `&متغير قيمة` في نفس النطاق ← خطأ ترجمة

---

### م-هـ02: ~~تفعيل وضع الملكية الصارم بدون GC~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 3 أيام  
**التبعيات**: م-هـ01

**الوصف**: `gc_mode.cpp` يحدد 4 أوضاع ذاكرة. في وضع النواة يجب أن يكون الوضع `ملكية_صارمة` (مثل Rust) بدون أي GC. يجب أن يكون هذا الوضع الافتراضي عند `#![بلا_مكتبة_قياسية]`.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/memory/gc_mode.cpp` | فرض `OwnershipStrict` في وضع no_std |
| `src/memory/memory_mode_flag.cpp` | إضافة فحص: إذا no_std ← منع GC/RefCounting |
| `src/pipeline/no_std_mode.cpp` | ضبط وضع الذاكرة تلقائياً عند تفعيل no_std |

**معايير القبول**:
- `#![بلا_مكتبة_قياسية]` ← وضع الملكية الصارم تلقائياً
- محاولة استخدام `اجمع_قمامة()` في no_std ← خطأ ترجمة
- `--ذاكرة gc` مع `--بلا-مكتبة-قياسية` ← خطأ: "لا يمكن استخدام جامع القمامة في وضع بلا مكتبة قياسية"

---

### م-هـ03: ~~توليد كود Drop/Destructor بدون GC~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 3 أيام  
**التبعيات**: م-هـ02, م-أ01

**الوصف**: في وضع الملكية، عند خروج متغير من النطاق يجب استدعاء `هدم()` (destructor). يجب أن يُولد `LLVMCodeGen` كود الـ drop تلقائياً.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_drop_glue.cpp` | توليد drop glue لكل نوع يحتاج تنظيف |
| `src/backend/llvm/llvm_drop_glue.h` | إعلانات |

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/llvm/llvm_codegen.cpp` | إضافة drop calls عند نهاية النطاقات |

**معايير القبول**:
- `{ متغير س = مورد::جديد(); }` ← يستدعي `س.هدم()` عند `}`
- التنظيف يحدث بترتيب عكسي (LIFO)
- الأنواع البدائية (ع32, منطق) لا تحتاج drop
- `*نقل* قيمة` ← لا يُستدعى drop للمصدر

---

## المرحلة و: نظام الأنواع المركبة

> **السبب**: `type_checker.cpp` يُرجع `AnyType` للأنواع المركبة (Array, Map, Function, Object, Enum, Tuple). هذا يعني أن فحص الأنواع لا يعمل لأي بنية بيانات معقدة.

---

### م-و01: ~~تنفيذ فحص أنواع المصفوفات والشرائح~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 3 أيام  
**التبعيات**: لا يوجد

**الوصف**: `checkArrayType()` في `type_checker.cpp` يُرجع `AnyType`. يجب تنفيذه ليفحص نوع العنصر وحجم المصفوفة الثابت.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/types/type_checker.cpp` | تنفيذ `checkArrayType()` — فحص نوع العنصر + الحجم |
| `src/types/type_checker.cpp` | تنفيذ `checkSliceType()` — شريحة بدون حجم ثابت |

**معايير القبول**:
- `مصفوفة<ع32, 256>` ← نوع صالح
- `متغير م: مصفوفة<نص, 10> = [1, 2, 3]` ← خطأ: نوع العنصر غير متطابق
- `م[257]` ← خطأ: فهرس خارج الحدود (إذا كان ثابتاً)

---

### م-و02: ~~تنفيذ فحص أنواع التعدادات (Tagged Unions)~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 3 أيام  
**التبعيات**: لا يوجد

**الوصف**: `checkEnumType()` يُرجع `AnyType`. النواة تحتاج تعدادات مثل `نتيجة<ن, خ>` و `اختياري<ن>` لمعالجة الأخطاء بدون استثناءات.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/types/type_checker.cpp` | تنفيذ `checkEnumType()` — فحص الحالات والقيم المرتبطة |
| `src/types/type_checker.cpp` | تنفيذ فحص مطابقة `قابل` الشاملة |

**معايير القبول**:
- `تعداد نتيجة<ن, خ> { نجاح(ن), فشل(خ) }` ← نوع معمم صالح
- `قابل ن { نتيجة::نجاح(ق) => ... }` بدون `فشل` ← خطأ: مطابقة غير شاملة
- `نتيجة<ع32, نص>` نوع مختلف عن `نتيجة<ع64, نص>`

---

### م-و03: ~~تنفيذ فحص أنواع الصفوف والبنى~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 2 أيام  
**التبعيات**: لا يوجد

**الوصف**: تنفيذ `checkTupleType()` و `checkObjectType()` (بنى) في type_checker.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/types/type_checker.cpp` | تنفيذ `checkTupleType()` و `checkObjectType()` |
| `src/types/type_inferencer.cpp` | إصلاح "In full implementation" TODOs |

**معايير القبول**:
- `(ع8, ع16)` نوع مختلف عن `(ع16, ع8)`
- `بنية نقطة { س: ع32, ص: ع32 }` ← نوع بحقول مسماة
- `متغير ن: نقطة = نقطة { س: 1, ص: "خطأ" }` ← خطأ نوع

---

### م-و04: ~~تنفيذ فحص أنواع مؤشرات الدوال~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 2 أيام  
**التبعيات**: لا يوجد

**الوصف**: نواة BeeOS تستخدم مؤشرات دوال لجداول المقاطعات وsyscall table. `checkFunctionType()` يُرجع `AnyType`.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/types/type_checker.cpp` | تنفيذ `checkFunctionType()` — فحص المعاملات ونوع الإرجاع |

**معايير القبول**:
- `نوع معالج = دالة(ع64) يرجع فراغ` ← نوع مؤشر دالة صالح
- `متغير م: معالج = دالة_خاطئة_التوقيع` ← خطأ نوع

---

### م-و05: ~~إصلاح مُحلّل القيود (Constraint Solver)~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 3 أيام  
**التبعيات**: م-و01, م-و02, م-و03

**الوصف**: مُحلّل القيود فيه عدة stubs "In full implementation". يجب تنفيذ حل القيود للأنواع المعممة مع الأنواع المركبة الجديدة.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/types/constraint_solver.cpp` | تنفيذ TODOs: حل قيود generics مع composite types |
| `src/types/type_inferencer.cpp` | تنفيذ استنتاج أنواع للأنواع المركبة |

**معايير القبول**:
- `دالة أول<ن>(قائمة: مصفوفة<ن, _>) -> ن` ← يُستنتج `ن` من نوع العنصر
- `أول([1, 2, 3])` ← `ن = ع32` (أو حجم العدد الافتراضي)

---

## المرحلة ز: الربط والإخراج الثنائي

> **السبب**: `LLVMLinker` يستهدف فقط ملفات تنفيذية مُستضافة (MSVC, Darwin, Linux-GNU). لا يوجد مسار لإنتاج ملفات ELF/flat binary بدون نظام تشغيل.

---

### م-ز01: ~~إضافة مسار ربط bare-metal في LLVMLinker~~ ✅ مكتمل

**الأولوية**: حرجة — حاجز رقم 5  
**التقدير**: 4 أيام  
**التبعيات**: م-ب02

**الوصف**: إضافة مسار ربط جديد في `LLVMLinker` يدعم: `-nostdlib`, `-T linker.ld`, `--oformat binary`/ELF, دون ربط أي مكتبة نظام.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_bare_metal_linker.cpp` | مسار الربط لـ bare-metal: -nostdlib, -T script, custom sections |
| `src/backend/llvm/llvm_bare_metal_linker.h` | إعلانات |

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/llvm/llvm_linker.cpp` | إضافة dispatch لـ bare-metal target |
| `src/backend/linker_script.cpp` | ربطه بـ `LLVMBareMetalLinker` |

**معايير القبول**:
- `صد بناء --هدف x86_64-bare` ← يستخدم linker script ولا يربط libc
- ملف ELF الناتج لا يحتوي على `NEEDED` entries
- `--صيغة-إخراج ثنائي-خام` ← flat binary (مثل `objcopy -O binary`)

---

### م-ز02: ~~ربط LinkerScriptGenerator بمسار البناء~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 2 أيام  
**التبعيات**: م-ز01

**الوصف**: `linker_script.cpp` يُولد نصوص linker script (يتضمن templates لـ `x86_kernel`, STM32, RP2040) لكن لا يُستدعى من أي مكان.

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/linker_script.cpp` | إضافة واجهة `generateForTarget(target_triple)` |
| `src/backend/llvm/llvm_bare_metal_linker.cpp` | استدعاء LinkerScriptGenerator إذا لم يوفّر المستخدم سكربت |

**معايير القبول**:
- `صد بناء --هدف x86_64-bare` بدون سكربت مخصص ← يُولّد سكربت تلقائي
- `صد بناء --هدف x86_64-bare --سكربت-ربط نواتي.ld` ← يستخدم المخصص
- السكربت المُولد يحتوي على `.text`, `.rodata`, `.data`, `.bss`, `KERNEL_STACK`

---

### م-ز03: ~~إضافة تعريف الأقسام المخصصة (Custom Sections)~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 2 أيام  
**التبعيات**: م-أ01

**الوصف**: النواة تحتاج وضع بيانات في أقسام محددة: `.multiboot` للترويسة, `.page_tables` لجداول الصفحات, `.interrupt_table` لـ IDT.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `src/backend/llvm/llvm_custom_sections.cpp` | توليد `section` attributes على globals/functions |
| `src/backend/llvm/llvm_custom_sections.h` | إعلانات |

**ملفات التعديل**:
| الملف | الإجراء |
|-------|---------|
| `src/backend/llvm/llvm_codegen.cpp` | ربط سمة `#[قسم("اسم")]` بتوليد section attribute |

**معايير القبول**:
- `#[قسم(".multiboot")] ثابت ترويسة = ...` ← البيانات في `.multiboot` section
- `#[قسم(".interrupt_table")] ثابت جدول_idt = ...` ← في `.interrupt_table`

---

## المرحلة ح: الاختبار والتحقق

> **السبب**: لا توجد اختبارات تكامل لوضع bare-metal. يجب التأكد من أن كل ما سبق يعمل فعلياً بإقلاع نواة على QEMU.

---

### ~~م-ح01: إنشاء إطار اختبار bare-metal مع QEMU~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 3 أيام  
**التبعيات**: م-ز01

**الوصف**: إنشاء harness اختبار يترجم ملف `.ص` في وضع bare-metal، يُشغله على QEMU، ويتحقق من الإخراج عبر المنفذ التسلسلي.

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `tests/bare_metal/test_harness.cpp` | إطار اختبار: ترجمة ← QEMU ← فحص الإخراج التسلسلي |
| `tests/bare_metal/test_harness.h` | إعلانات |

**ملفات اختبار ص جديدة**:
| الملف | الوصف |
|-------|-------|
| `tests/bare_metal/test_boot.sad` | اختبار: يبدأ ويكتب "BeeOS" على المنفذ التسلسلي |
| `tests/bare_metal/test_interrupts.sad` | اختبار: يُسجل معالج timer ويتحقق من استدعائه |
| `tests/bare_metal/test_paging.sad` | اختبار: يُفعّل ترحيل الصفحات ويصل لعنوان افتراضي |
| `tests/bare_metal/test_allocator.sad` | اختبار: bump allocator يخصص ويُحرر |

**معايير القبول**:
- `ctest --test-dir build -R bare_metal` ← كل الاختبارات تمر
- اختبار الإقلاع يطبع "BeeOS" عبر UART
- QEMU يخرج بكود 0 (نجاح) أو 1 (فشل) تلقائياً

---

### ~~م-ح02: اختبارات وحدة لربط الأنواع~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 2 أيام  
**التبعيات**: م-أ01, م-أ02

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `tests/unit/test_type_mapper_integration.cpp` | اختبارات: convertType لكل نوع ← LLVM type صحيح |

**معايير القبول**:
- 50+ اختبار يغطي كل الأنواع البدائية والمركبة
- اختبار regression: لا يُرجع `i64` لأي نوع غير `ع64`

---

### ~~م-ح03: اختبارات وحدة لـ borrow checker مع المراجع~~ ✅ مكتمل

**الأولوية**: عالية  
**التقدير**: 2 أيام  
**التبعيات**: م-هـ01

**ملفات C++ جديدة**:
| الملف | الوصف |
|-------|-------|
| `tests/unit/test_borrow_ref_deref.cpp` | اختبارات: `&`, `&متغير`, `*`, lifetime rules |

**معايير القبول**:
- 30+ اختبار يغطي سيناريوهات الاستعارة
- اختبار: `&x` و `&mut x` في نفس النطاق ← خطأ

---

### ~~م-ح04: اختبار تكاملي شامل — نواة Hello World~~ ✅ مكتمل

**الأولوية**: حرجة  
**التقدير**: 3 أيام  
**التبعيات**: كل المراحل السابقة

**الوصف**: كتابة نواة BeeOS مُصغّرة بلغة ص تمر بكل مراحل الترجمة وتُقلع على QEMU.

**ملفات اختبار ص**:
| الملف | الوصف |
|-------|-------|
| `tests/integration/beeos_hello/نواة.ص` | نواة مُصغّرة: إقلاع → ترحيل صفحات → طباعة "نظام النحلة" عبر VGA + UART |
| `tests/integration/beeos_hello/حزمة.ص.toml` | إعدادات المشروع: هدف bare-metal، بلا مكتبة قياسية |
| `tests/integration/beeos_hello/ربط.ld` | سكربت ربط مخصص |

**معايير القبول**:
- `صد بناء --هدف x86_64-bare` ← ينتج `نواة.elf`
- `qemu-system-x86_64 -kernel نواة.elf` ← يطبع "نظام النحلة 🐝" على الشاشة
- الملف الناتج < 64KB
- تحليل الأمان يمر بدون تحذيرات

---

## ملخص الملفات

### ملفات C++ جديدة (31 ملف)

| # | المسار | المرحلة |
|---|--------|---------|
| 1 | `src/backend/llvm/llvm_type_mapper_composite.cpp` | أ |
| 2 | `src/backend/llvm/llvm_type_mapper_composite.h` | أ |
| 3 | `src/backend/llvm/llvm_volatile_ops.cpp` | أ |
| 4 | `src/backend/llvm/llvm_volatile_ops.h` | أ |
| 5 | `src/pipeline/no_std_integration.cpp` | ب |
| 6 | `src/pipeline/no_std_integration.h` | ب |
| 7 | `runtime/freestanding/sad_core.h` | ج |
| 8 | `runtime/freestanding/sad_core.cpp` | ج |
| 9 | `runtime/freestanding/sad_panic.h` | ج |
| 10 | `runtime/freestanding/sad_panic.cpp` | ج |
| 11 | `runtime/freestanding/sad_types.h` | ج |
| 12 | `runtime/freestanding/sad_allocator.h` | ج |
| 13 | `runtime/freestanding/sad_allocator.cpp` | ج |
| 14 | `runtime/freestanding/sad_bump_allocator.h` | ج |
| 15 | `runtime/freestanding/sad_bump_allocator.cpp` | ج |
| 16 | `runtime/freestanding/sad_port_io.h` | ج |
| 17 | `runtime/freestanding/sad_port_io.cpp` | ج |
| 18 | `src/backend/llvm/llvm_port_io_intrinsics.cpp` | ج |
| 19 | `src/backend/llvm/llvm_port_io_intrinsics.h` | ج |
| 20 | `src/backend/llvm/llvm_entry_point.cpp` | د |
| 21 | `src/backend/llvm/llvm_entry_point.h` | د |
| 22 | `src/backend/llvm/llvm_interrupt_handlers.cpp` | د |
| 23 | `src/backend/llvm/llvm_interrupt_handlers.h` | د |
| 24 | `src/backend/llvm/llvm_multiboot.cpp` | د |
| 25 | `src/backend/llvm/llvm_multiboot.h` | د |
| 26 | `src/backend/llvm/llvm_uefi_entry.cpp` | د |
| 27 | `src/backend/llvm/llvm_uefi_entry.h` | د |
| 28 | `src/backend/llvm/llvm_cpu_regs.cpp` | د |
| 29 | `src/backend/llvm/llvm_cpu_regs.h` | د |
| 30 | `src/backend/llvm/llvm_paging.cpp` | د |
| 31 | `src/backend/llvm/llvm_paging.h` | د |
| 32 | `src/backend/llvm/llvm_drop_glue.cpp` | هـ |
| 33 | `src/backend/llvm/llvm_drop_glue.h` | هـ |
| 34 | `src/backend/llvm/llvm_bare_metal_linker.cpp` | ز |
| 35 | `src/backend/llvm/llvm_bare_metal_linker.h` | ز |
| 36 | `src/backend/llvm/llvm_custom_sections.cpp` | ز |
| 37 | `src/backend/llvm/llvm_custom_sections.h` | ز |
| 38 | `tests/bare_metal/test_harness.cpp` | ح |
| 39 | `tests/bare_metal/test_harness.h` | ح |
| 40 | `tests/unit/test_type_mapper_integration.cpp` | ح |
| 41 | `tests/unit/test_borrow_ref_deref.cpp` | ح |

### ملفات C++ للتعديل (22 ملف)

| # | المسار | المهام |
|---|--------|--------|
| 1 | `src/backend/llvm/llvm_codegen.cpp` | م-أ01, م-أ03, م-د01, م-د02, م-د03, م-د04, م-هـ03, م-ز03 |
| 2 | `src/backend/llvm/llvm_codegen.h` | م-أ01 |
| 3 | `src/backend/llvm/llvm_type_mapper.cpp` | م-أ01, م-أ02 |
| 4 | `src/backend/llvm/llvm_type_mapper.h` | م-أ01 |
| 5 | `src/types/type_checker.cpp` | م-أ02, م-أ03, م-و01 → م-و04 |
| 6 | `src/types/type_checker.h` | م-أ02 |
| 7 | `CMakeLists.txt` | م-ب01, م-ب03, م-ب04 |
| 8 | `src/pipeline/no_std_mode.cpp` | م-ب02, م-هـ02 |
| 9 | `src/pipeline/pipeline.cpp` | م-ب02 |
| 10 | `src/frontend/parser.cpp` | م-ب02 |
| 11 | `src/optimizer/pass_manager.cpp` | م-ب03 |
| 12 | `src/security/taint_tracker.cpp` | م-ب04 |
| 13 | `src/security/taint_tracker.h` | م-ب04 |
| 14 | `src/backend/inline_asm.cpp` | م-د03 |
| 15 | `src/backend/entry_point.cpp` | م-د01 (مرجع) |
| 16 | `src/backend/interrupt_handlers.cpp` | م-د02 (مرجع) |
| 17 | `src/backend/llvm/llvm_kernel_sync.cpp` | م-د04 |
| 18 | `src/borrow/borrow_checker.cpp` | م-هـ01 |
| 19 | `src/borrow/borrow_checker.h` | م-هـ01 |
| 20 | `src/borrow/lifetime_analyzer.cpp` | م-هـ01 |
| 21 | `src/memory/gc_mode.cpp` | م-هـ02 |
| 22 | `src/memory/memory_mode_flag.cpp` | م-هـ02 |
| 23 | `src/types/constraint_solver.cpp` | م-و05 |
| 24 | `src/types/type_inferencer.cpp` | م-و03, م-و05 |
| 25 | `src/backend/llvm/llvm_linker.cpp` | م-ز01 |
| 26 | `src/backend/linker_script.cpp` | م-ز02 |

---

## مصفوفة التبعيات

```
م-أ01 ─────┬──► م-أ02
            ├──► م-أ03
            ├──► م-د01 ──► م-د02
            │           └──► م-د05
            ├──► م-د03 ──► م-د06 ──► م-د07
            ├──► م-د04
            ├──► م-ج01 ──► م-ج02
            │           └──► م-ج03
            └──► م-هـ03

م-ب01 ──► م-ب02 ──► م-ز01 ──► م-ز02
                           └──► م-ح01

م-هـ01 ──► م-هـ02 ──► م-هـ03

م-و01 ─┐
م-و02 ─┼──► م-و05
م-و03 ─┘
م-و04

م-ح04 ← (كل المهام السابقة)
```

---

## جدول التنفيذ المقترح

| الأسبوع | المهام | المطورون |
|---------|--------|----------|
| 1 | م-أ01, م-ب01 | 2 |
| 2 | م-أ02, م-أ03, م-ب02 | 2 |
| 3 | م-ب03, م-ب04, م-ج01 | 2 |
| 4 | م-ج02, م-ج03, م-و01 | 2 |
| 5 | م-هـ01, م-و02, م-و03 | 2 |
| 6 | م-هـ02, م-و04, م-و05 | 2 |
| 7 | م-د01, م-د03, م-هـ03 | 2 |
| 8 | م-د02, م-د04, م-ز01 | 2 |
| 9 | م-د05, م-د06, م-ز02 | 2 |
| 10 | م-د07, م-ز03, م-ح01 | 2 |
| 11 | م-ح02, م-ح03 | 1 |
| 12 | م-ح04 (اختبار تكاملي شامل) | 2 |

---

## إحصائيات

| البند | القيمة |
|-------|--------|
| إجمالي المهام | 30 |
| المهام الحرجة (حاجزة) | 12 |
| المهام العالية | 12 |
| المهام المتوسطة | 6 |
| ملفات C++ جديدة | 41 |
| ملفات C++ للتعديل | 26 |
| أيام عمل تقديرية | ~88 يوم |
| معايير القبول | 90+ |
| اختبارات bare-metal | 4 |
| اختبارات وحدة | 80+ |

---

*تاريخ الإنشاء: 10 فبراير 2026*  
*مبني على تحليل عميق لـ 40,000+ سطر C++ في `compiler_new/`*
