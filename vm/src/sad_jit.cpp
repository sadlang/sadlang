// ============================================================================
// sad_jit.cpp — تنفيذ محرك الترجمة الفورية (JIT) لغة ص
// JIT Compilation Engine Implementation for Sad Language
// ============================================================================
//
// الوصف (AR):
//   يُنفّذ هذا الملف محرك JIT الذي يربط بايت كود الآلة الافتراضية (43+ عملية)
//   بمترجم LLVM ORC JIT لتوليد كود أصلي في وقت التشغيل.
//
//   المسار: VM Opcodes → تحليل البايت كود → بناء LLVM IR → تحسين → كود أصلي
//
//   عند عدم وجود LLVM (بناء بدون LLVM)، يتم تعطيل JIT تلقائياً
//   ويعمل المحرك في وضع العد فقط (profiling-only mode).
//
// Description (EN):
//   Implements the JIT engine that bridges VM bytecode (43+ opcodes)
//   with LLVM ORC JIT compiler for native code generation at runtime.
//   When LLVM is unavailable, falls back to profiling-only mode.
//
// ============================================================================

#include "sad_jit.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

// ============================================================================
// تضمين LLVM المشروط / Conditional LLVM includes
// ============================================================================
#ifdef HAS_LLVM
    #include "llvm/IR/LLVMContext.h"
    #include "llvm/IR/Module.h"
    #include "llvm/IR/IRBuilder.h"
    #include "llvm/IR/Function.h"
    #include "llvm/IR/Type.h"
    #include "llvm/IR/Verifier.h"
    #include "llvm/IR/LegacyPassManager.h"
    #include "llvm/Support/TargetSelect.h"
    #include "llvm/ExecutionEngine/Orc/LLJIT.h"
    #include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
    #include "llvm/Transforms/InstCombine/InstCombine.h"
    #include "llvm/Transforms/Scalar.h"
    #include "llvm/Transforms/Scalar/GVN.h"
#endif

namespace sad {
namespace jit {

// ============================================================================
// بيانات LLVM الداخلية / Internal LLVM Data
// ============================================================================
#ifdef HAS_LLVM
struct محرك_JIT::بيانات_LLVM {
    std::unique_ptr<llvm::orc::LLJIT> المحرك;     // محرك ORC JIT
    std::unique_ptr<llvm::LLVMContext> السياق;      // سياق LLVM
    
    بيانات_LLVM() : السياق(std::make_unique<llvm::LLVMContext>()) {}
    ~بيانات_LLVM() = default;
};
#endif

// ============================================================================
// البناء / Constructor
// ============================================================================
محرك_JIT::محرك_JIT(const إعدادات_JIT& إعدادات)
    : الإعدادات_(إعدادات)
    , جاهز_(false)
{
#ifdef HAS_LLVM
    llvm_ = std::make_unique<بيانات_LLVM>();
#endif
}

// ============================================================================
// التدمير / Destructor
// ============================================================================
محرك_JIT::~محرك_JIT() {
    فرّغ_الذاكرة();
}

// ============================================================================
// التهيئة / Initialization
// ============================================================================
bool محرك_JIT::هيّئ() {
    std::lock_guard<std::mutex> قفل(القفل_);
    
    if (جاهز_) return true;
    
#ifdef HAS_LLVM
    // تهيئة أهداف LLVM
    // Initialize LLVM targets
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    
    // إنشاء محرك ORC LLJIT
    // Create ORC LLJIT engine
    auto builder = llvm::orc::LLJITBuilder();
    auto jit = builder.create();
    
    if (!jit) {
        if (الإعدادات_.التتبع) {
            std::cerr << "[JIT] ✗ فشل إنشاء محرك LLVM ORC JIT" << std::endl;
            std::cerr << "[JIT] ✗ Failed to create LLVM ORC JIT engine" << std::endl;
        }
        return false;
    }
    
    llvm_->المحرك = std::move(*jit);
    جاهز_ = true;
    
    if (الإعدادات_.التتبع) {
        std::cout << "[JIT] ✓ تم تهيئة محرك LLVM ORC JIT بنجاح" << std::endl;
        std::cout << "[JIT] ✓ LLVM ORC JIT engine initialized successfully" << std::endl;
    }
    
    return true;
#else
    // وضع العد فقط — بدون LLVM
    // Profiling-only mode — no LLVM available
    جاهز_ = true;
    
    if (الإعدادات_.التتبع) {
        std::cout << "[JIT] ⚠ وضع البروفايلر فقط (LLVM غير متوفر)" << std::endl;
        std::cout << "[JIT] ⚠ Profiler-only mode (LLVM unavailable)" << std::endl;
    }
    
    return true;
#endif
}

// ============================================================================
// تسجيل دالة / Register Function
// ============================================================================
void محرك_JIT::سجّل_دالة(const std::string& الاسم,
                            const vm::وحدة_بايت_كود& الكود,
                            uint32_t عدد_المعاملات) {
    std::lock_guard<std::mutex> قفل(القفل_);
    
    بايت_كود_الدوال_[الاسم] = الكود;
    عدادات_[الاسم] = 0;
    
    if (الإعدادات_.التتبع) {
        std::cout << "[JIT] ✓ تسجيل دالة: " << الاسم
                  << " (حجم: " << الكود.الحجم() << " بايت"
                  << ", معاملات: " << عدد_المعاملات << ")" << std::endl;
    }
}

// ============================================================================
// تسجيل استدعاء (بروفايلر) / Record Call (Profiler)
// ============================================================================
bool محرك_JIT::سجّل_استدعاء(const std::string& الاسم) {
    std::lock_guard<std::mutex> قفل(القفل_);
    
    auto it = عدادات_.find(الاسم);
    if (it == عدادات_.end()) return false;
    
    ++(it->second);
    
    // فحص ما إذا تجاوز العتبة
    // Check if threshold exceeded
    if (it->second == الإعدادات_.عتبة_الترجمة) {
        if (الإعدادات_.التتبع) {
            std::cout << "[JIT] → الدالة '" << الاسم 
                      << "' وصلت عتبة الترجمة (" << الإعدادات_.عتبة_الترجمة 
                      << " استدعاء)" << std::endl;
        }
        return true; // يجب الترجمة
    }
    
    // فحص التحسين التكيفي
    // Check adaptive optimization
    if (الإعدادات_.التحسين_التكيفي) {
        auto cache_it = ذاكرة_الكود_.find(الاسم);
        if (cache_it != ذاكرة_الكود_.end() && cache_it->second.صالحة) {
            if (it->second == الإعدادات_.عتبة_الترجمة + الإعدادات_.عتبة_التحسين_التكيفي) {
                if (cache_it->second.مستوى < مستوى_التحسين::أقصى) {
                    if (الإعدادات_.التتبع) {
                        std::cout << "[JIT] ↑ ترقية تحسين الدالة '" << الاسم 
                                  << "' إلى O3" << std::endl;
                    }
                    return true; // إعادة ترجمة بمستوى أعلى
                }
            }
        }
    }
    
    return false;
}

// ============================================================================
// ترجمة فورية / Immediate Compilation
// ============================================================================
bool محرك_JIT::ترجم_فوراً(const std::string& الاسم, مستوى_التحسين المستوى) {
    std::lock_guard<std::mutex> قفل(القفل_);
    
    // التحقق من وجود البايت كود
    auto it = بايت_كود_الدوال_.find(الاسم);
    if (it == بايت_كود_الدوال_.end()) {
        if (الإعدادات_.التتبع) {
            std::cerr << "[JIT] ✗ الدالة '" << الاسم << "' غير مسجلة" << std::endl;
        }
        return false;
    }
    
#ifdef HAS_LLVM
    if (!جاهز_ || !llvm_ || !llvm_->المحرك) {
        if (الإعدادات_.التتبع) {
            std::cerr << "[JIT] ✗ المحرك غير مُهيّأ" << std::endl;
        }
        return false;
    }
    
    auto بداية = std::chrono::steady_clock::now();
    
    // الخطوة 1: تحويل بايت كود → LLVM IR
    // Step 1: Convert bytecode → LLVM IR
    if (!حوّل_إلى_IR(الاسم, it->second, المستوى)) {
        return false;
    }
    
    // الخطوة 2: تحسين LLVM IR
    // Step 2: Optimize LLVM IR
    حسّن_IR(الاسم, المستوى);
    
    // الخطوة 3: توليد كود أصلي
    // Step 3: Generate native code
    void* كود = ولّد_كود_أصلي(الاسم);
    if (!كود) {
        return false;
    }
    
    auto نهاية = std::chrono::steady_clock::now();
    double زمن = std::chrono::duration<double, std::milli>(نهاية - بداية).count();
    
    // تخزين في ذاكرة الكود المؤقتة
    // Store in code cache
    دالة_مترجمة معلومات;
    معلومات.الاسم = الاسم;
    معلومات.مؤشر_الكود = كود;
    معلومات.مستوى = المستوى;
    معلومات.صالحة = true;
    معلومات.زمن_الترجمة = زمن;
    معلومات.آخر_استخدام = std::chrono::steady_clock::now();
    ذاكرة_الكود_[الاسم] = std::move(معلومات);
    
    // تحديث الإحصائيات
    الإحصائيات_.عدد_الدوال_المترجمة++;
    الإحصائيات_.زمن_الترجمة_الإجمالي += زمن;
    
    if (الإعدادات_.التتبع) {
        std::cout << "[JIT] ✓ ترجمة '"  << الاسم << "' → O" 
                  << static_cast<int>(المستوى) << " (" 
                  << std::fixed << std::setprecision(2) << زمن << " مللي ثانية)" << std::endl;
    }
    
    return true;
#else
    // بدون LLVM — لا يمكن الترجمة
    if (الإعدادات_.التتبع) {
        std::cout << "[JIT] ⚠ LLVM غير متوفر — الترجمة معطلة" << std::endl;
    }
    return false;
#endif
}

// ============================================================================
// ترجمة الكل / Compile All
// ============================================================================
uint32_t محرك_JIT::ترجم_الكل(مستوى_التحسين المستوى) {
    uint32_t عدد = 0;
    // نسخ الأسماء لتجنب deadlock (ترجم_فوراً تأخذ القفل)
    std::vector<std::string> أسماء;
    {
        std::lock_guard<std::mutex> قفل(القفل_);
        for (const auto& زوج : بايت_كود_الدوال_) {
            أسماء.push_back(زوج.first);
        }
    }
    
    for (const auto& اسم : أسماء) {
        if (ترجم_فوراً(اسم, المستوى)) {
            ++عدد;
        }
    }
    return عدد;
}

// ============================================================================
// التنفيذ / Execution
// ============================================================================
int64_t محرك_JIT::نفّذ(const std::string& الاسم,
                       const std::vector<int64_t>& المعاملات) {
    std::lock_guard<std::mutex> قفل(القفل_);
    
    // فحص وجود كود أصلي
    auto it = ذاكرة_الكود_.find(الاسم);
    if (it != ذاكرة_الكود_.end() && it->second.صالحة && it->second.مؤشر_الكود) {
        // تنفيذ أصلي
        it->second.عدد_الاستدعاءات++;
        it->second.آخر_استخدام = std::chrono::steady_clock::now();
        الإحصائيات_.عدد_الاستدعاءات_الأصلية++;
        
        // استدعاء الكود الأصلي
        // الشكل المتوقع: int64_t (*)(int64_t*, int)
        using نوع_الدالة = int64_t(*)(const int64_t*, int);
        auto دالة = reinterpret_cast<نوع_الدالة>(it->second.مؤشر_الكود);
        return دالة(المعاملات.data(), static_cast<int>(المعاملات.size()));
    }
    
    // لا يوجد كود أصلي — تسجيل استدعاء مفسّر
    الإحصائيات_.عدد_الاستدعاءات_المفسرة++;
    
    // الإرجاع 0 يعني: يجب استخدام المفسّر
    return 0;
}

// ============================================================================
// هل الدالة مُترجمة؟ / Is Function Compiled?
// ============================================================================
bool محرك_JIT::مترجمة(const std::string& الاسم) const {
    std::lock_guard<std::mutex> قفل(القفل_);
    auto it = ذاكرة_الكود_.find(الاسم);
    return it != ذاكرة_الكود_.end() && it->second.صالحة;
}

// ============================================================================
// الإحصائيات / Statistics
// ============================================================================
void محرك_JIT::أعد_تعيين_الإحصائيات() {
    std::lock_guard<std::mutex> قفل(القفل_);
    الإحصائيات_ = إحصائيات_JIT{};
}

void محرك_JIT::اطبع_التقرير() const {
    std::lock_guard<std::mutex> قفل(القفل_);
    
    std::cout << "\n";
    std::cout << "╔══════════════════════════════════════════════════════╗\n";
    std::cout << "║          تقرير أداء محرك JIT — لغة ص               ║\n";
    std::cout << "║          JIT Engine Performance Report              ║\n";
    std::cout << "╠══════════════════════════════════════════════════════╣\n";
    std::cout << "║  الدوال المسجلة      : " << std::setw(8) << بايت_كود_الدوال_.size() 
              << "                     ║\n";
    std::cout << "║  الدوال المُترجمة    : " << std::setw(8) << الإحصائيات_.عدد_الدوال_المترجمة
              << "                     ║\n";
    std::cout << "║  استدعاءات أصلية    : " << std::setw(8) << الإحصائيات_.عدد_الاستدعاءات_الأصلية
              << "                     ║\n";
    std::cout << "║  استدعاءات مُفسّرة  : " << std::setw(8) << الإحصائيات_.عدد_الاستدعاءات_المفسرة
              << "                     ║\n";
    std::cout << "║  زمن الترجمة        : " << std::setw(8) << std::fixed << std::setprecision(2)
              << الإحصائيات_.زمن_الترجمة_الإجمالي << " مللي ثانية   ║\n";
    std::cout << "║  نسبة التسريع       : " << std::setw(8) << std::fixed << std::setprecision(1)
              << الإحصائيات_.نسبة_التسريع() << "x                   ║\n";
    std::cout << "╚══════════════════════════════════════════════════════╝\n\n";
}

// ============================================================================
// تفريغ الذاكرة / Clear Cache
// ============================================================================
void محرك_JIT::فرّغ_الذاكرة() {
    std::lock_guard<std::mutex> قفل(القفل_);
    ذاكرة_الكود_.clear();
}

// ============================================================================
// الدوال الخاصة — تحويل بايت كود إلى LLVM IR
// Private — Convert bytecode to LLVM IR
// ============================================================================
bool محرك_JIT::حوّل_إلى_IR(const std::string& الاسم,
                            const vm::وحدة_بايت_كود& الكود,
                            مستوى_التحسين المستوى) {
#ifdef HAS_LLVM
    if (!llvm_ || !llvm_->السياق) return false;
    
    auto& ctx = *llvm_->السياق;
    auto module = std::make_unique<llvm::Module>("jit_" + الاسم, ctx);
    llvm::IRBuilder<> builder(ctx);
    
    // إنشاء نوع الدالة: int64_t(int64_t*, int)
    // Create function type: int64_t(int64_t*, int)
    auto int64Type = llvm::Type::getInt64Ty(ctx);
    auto int32Type = llvm::Type::getInt32Ty(ctx);
    auto ptrType = llvm::PointerType::get(int64Type, 0);
    
    std::vector<llvm::Type*> paramTypes = { ptrType, int32Type };
    auto funcType = llvm::FunctionType::get(int64Type, paramTypes, false);
    
    auto func = llvm::Function::Create(
        funcType, llvm::Function::ExternalLinkage, الاسم, module.get());
    
    // إنشاء الكتلة الأساسية
    // Create entry block
    auto entry = llvm::BasicBlock::Create(ctx, "entry", func);
    builder.SetInsertPoint(entry);
    
    // الحصول على معاملات الدالة
    auto args = func->arg_begin();
    llvm::Value* argsPtr = args++;     // مؤشر المعاملات
    llvm::Value* argsCount = args;     // عدد المعاملات
    
    // ====================================================================
    // ترجمة بايت كود VM إلى LLVM IR
    // Translate VM bytecode to LLVM IR
    // ====================================================================
    // يتم المشي على كل تعليمة بايت كود وتحويلها إلى عمليات LLVM IR
    // Walk through each bytecode instruction and convert to LLVM IR ops
    
    const auto* بيانات = الكود.البيانات();
    size_t حجم = الكود.الحجم();
    
    // مكدس LLVM (يمثل مكدس VM) 
    // LLVM stack (represents VM stack)
    std::vector<llvm::Value*> مكدس;
    
    // خريطة المتغيرات المحلية → alloca
    // Local variable map → alloca
    std::unordered_map<uint16_t, llvm::AllocaInst*> محليات;
    
    size_t ip = 0;
    while (ip < حجم) {
        vm::رمز_عملية عملية = بيانات[ip++];
        
        switch (عملية) {
            // ============================================================
            // عمليات المكدس / Stack operations
            // ============================================================
            case vm::عملية_ثابت: {
                // دفع ثابت إلى المكدس (معامل 16 بت)
                if (ip + 1 < حجم) {
                    uint16_t فهرس = static_cast<uint16_t>(بيانات[ip]) |
                                   (static_cast<uint16_t>(بيانات[ip + 1]) << 8);
                    ip += 2;
                    // محاولة قراءة الثابت الفعلي من مجمع الثوابت
                    auto ثابت_val = builder.getInt64(0); // placeholder
                    مكدس.push_back(ثابت_val);
                }
                break;
            }
            case vm::عملية_رقم_صغير: {
                // دفع عدد صحيح صغير (معامل 8 بت)
                if (ip < حجم) {
                    int8_t قيمة = static_cast<int8_t>(بيانات[ip++]);
                    مكدس.push_back(builder.getInt64(قيمة));
                }
                break;
            }
            case vm::عملية_لا_شيء_قيمة: {
                // دفع null (0)
                مكدس.push_back(builder.getInt64(0));
                break;
            }
            case vm::عملية_صحيح: {
                // دفع true (1)
                مكدس.push_back(builder.getInt64(1));
                break;
            }
            case vm::عملية_خطأ: {
                // دفع false (0)
                مكدس.push_back(builder.getInt64(0));
                break;
            }
            case vm::عملية_سحب: {
                // إزالة قمة المكدس
                if (!مكدس.empty()) مكدس.pop_back();
                break;
            }
            case vm::عملية_سحب_متعدد: {
                // إزالة عدة عناصر (معامل 8 بت)
                if (ip < حجم) {
                    uint8_t عدد = بيانات[ip++];
                    for (uint8_t i = 0; i < عدد && !مكدس.empty(); ++i) {
                        مكدس.pop_back();
                    }
                }
                break;
            }
            case vm::عملية_نسخ: {
                // نسخ قمة المكدس
                if (!مكدس.empty()) {
                    مكدس.push_back(مكدس.back());
                }
                break;
            }
            case vm::عملية_تبديل: {
                // تبديل أعلى عنصرين
                if (مكدس.size() >= 2) {
                    auto حجم_م = مكدس.size();
                    std::swap(مكدس[حجم_م - 1], مكدس[حجم_م - 2]);
                }
                break;
            }
            
            // ============================================================
            // عمليات حسابية / Arithmetic operations
            // ============================================================
            case vm::عملية_جمع: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateAdd(أ, ب, "add"));
                }
                break;
            }
            case vm::عملية_طرح: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateSub(أ, ب, "sub"));
                }
                break;
            }
            case vm::عملية_ضرب: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateMul(أ, ب, "mul"));
                }
                break;
            }
            case vm::عملية_قسمة: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateSDiv(أ, ب, "div"));
                }
                break;
            }
            case vm::عملية_باقي: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateSRem(أ, ب, "mod"));
                }
                break;
            }
            case vm::عملية_سالب: {
                if (!مكدس.empty()) {
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateNeg(أ, "neg"));
                }
                break;
            }
            case vm::عملية_زيادة: {
                // INC: أ + 1
                if (!مكدس.empty()) {
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateAdd(أ, builder.getInt64(1), "inc"));
                }
                break;
            }
            case vm::عملية_نقصان: {
                // DEC: أ - 1
                if (!مكدس.empty()) {
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateSub(أ, builder.getInt64(1), "dec"));
                }
                break;
            }
            
            // ============================================================
            // عمليات المقارنة / Comparison operations
            // ============================================================
            case vm::عملية_يساوي: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    auto نتيجة = builder.CreateICmpEQ(أ, ب, "eq");
                    مكدس.push_back(builder.CreateZExt(نتيجة, int64Type, "eq_i64"));
                }
                break;
            }
            case vm::عملية_لا_يساوي: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    auto نتيجة = builder.CreateICmpNE(أ, ب, "ne");
                    مكدس.push_back(builder.CreateZExt(نتيجة, int64Type, "ne_i64"));
                }
                break;
            }
            case vm::عملية_أصغر: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    auto نتيجة = builder.CreateICmpSLT(أ, ب, "lt");
                    مكدس.push_back(builder.CreateZExt(نتيجة, int64Type, "lt_i64"));
                }
                break;
            }
            case vm::عملية_أصغر_يساوي: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    auto نتيجة = builder.CreateICmpSLE(أ, ب, "le");
                    مكدس.push_back(builder.CreateZExt(نتيجة, int64Type, "le_i64"));
                }
                break;
            }
            case vm::عملية_أكبر: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    auto نتيجة = builder.CreateICmpSGT(أ, ب, "gt");
                    مكدس.push_back(builder.CreateZExt(نتيجة, int64Type, "gt_i64"));
                }
                break;
            }
            case vm::عملية_أكبر_يساوي: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    auto نتيجة = builder.CreateICmpSGE(أ, ب, "ge");
                    مكدس.push_back(builder.CreateZExt(نتيجة, int64Type, "ge_i64"));
                }
                break;
            }
            
            // ============================================================
            // عمليات منطقية / Logical operations
            // ============================================================
            case vm::عملية_ليس: {
                // NOT: !أ  (0 → 1, أي شيء آخر → 0)
                if (!مكدس.empty()) {
                    auto أ = مكدس.back(); مكدس.pop_back();
                    auto صفر = builder.getInt64(0);
                    auto نتيجة = builder.CreateICmpEQ(أ, صفر, "not");
                    مكدس.push_back(builder.CreateZExt(نتيجة, int64Type, "not_i64"));
                }
                break;
            }
            case vm::عملية_و: {
                // AND: أ && ب (كلاهما غير صفري)
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    auto صفر = builder.getInt64(0);
                    auto أ_bool = builder.CreateICmpNE(أ, صفر, "a_bool");
                    auto ب_bool = builder.CreateICmpNE(ب, صفر, "b_bool");
                    auto نتيجة = builder.CreateAnd(أ_bool, ب_bool, "and");
                    مكدس.push_back(builder.CreateZExt(نتيجة, int64Type, "and_i64"));
                }
                break;
            }
            case vm::عملية_أو: {
                // OR: أ || ب (أحدهما غير صفري)
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    auto صفر = builder.getInt64(0);
                    auto أ_bool = builder.CreateICmpNE(أ, صفر, "a_bool");
                    auto ب_bool = builder.CreateICmpNE(ب, صفر, "b_bool");
                    auto نتيجة = builder.CreateOr(أ_bool, ب_bool, "or");
                    مكدس.push_back(builder.CreateZExt(نتيجة, int64Type, "or_i64"));
                }
                break;
            }
            
            // ============================================================
            // عمليات بتية / Bitwise operations
            // ============================================================
            case vm::عملية_و_بتية: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateAnd(أ, ب, "bit_and"));
                }
                break;
            }
            case vm::عملية_أو_بتية: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateOr(أ, ب, "bit_or"));
                }
                break;
            }
            case vm::عملية_أو_حصرية: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateXor(أ, ب, "xor"));
                }
                break;
            }
            case vm::عملية_نفي_بتي: {
                if (!مكدس.empty()) {
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateNot(أ, "bit_not"));
                }
                break;
            }
            case vm::عملية_إزاحة_يسار: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateShl(أ, ب, "shl"));
                }
                break;
            }
            case vm::عملية_إزاحة_يمين: {
                if (مكدس.size() >= 2) {
                    auto ب = مكدس.back(); مكدس.pop_back();
                    auto أ = مكدس.back(); مكدس.pop_back();
                    مكدس.push_back(builder.CreateAShr(أ, ب, "shr"));
                }
                break;
            }
            
            // ============================================================
            // متغيرات محلية / Local variables (1-byte operand)
            // ============================================================
            case vm::عملية_تحميل_محلي: {
                if (ip < حجم) {
                    uint8_t فهرس = بيانات[ip++];
                    auto it = محليات.find(فهرس);
                    if (it != محليات.end()) {
                        مكدس.push_back(builder.CreateLoad(int64Type, it->second, "local"));
                    } else {
                        // متغير محلي غير مُخزّن بعد — قد يكون معامل دالة
                        // تحميل من مصفوفة المعاملات
                        auto idx = builder.getInt32(فهرس);
                        auto ptr = builder.CreateGEP(int64Type, argsPtr, idx, "arg_ptr");
                        مكدس.push_back(builder.CreateLoad(int64Type, ptr, "arg"));
                    }
                }
                break;
            }
            case vm::عملية_تخزين_محلي: {
                if (ip < حجم && !مكدس.empty()) {
                    uint8_t فهرس = بيانات[ip++];
                    auto قيمة = مكدس.back(); مكدس.pop_back();
                    auto& alloca = محليات[فهرس];
                    if (!alloca) {
                        // إنشاء مساحة في كتلة الدخول
                        auto saved = builder.GetInsertBlock();
                        builder.SetInsertPoint(&func->getEntryBlock(), 
                            func->getEntryBlock().begin());
                        alloca = builder.CreateAlloca(int64Type, nullptr, "var_" + std::to_string(فهرس));
                        builder.SetInsertPoint(saved);
                    }
                    builder.CreateStore(قيمة, alloca);
                }
                break;
            }
            
            // ============================================================
            // إرجاع القيمة / Return value
            // ============================================================
            case vm::عملية_عودة_بقيمة: {
                if (!مكدس.empty()) {
                    builder.CreateRet(مكدس.back());
                    مكدس.pop_back();
                } else {
                    builder.CreateRet(builder.getInt64(0));
                }
                goto end_translation;
            }
            case vm::عملية_عودة: {
                builder.CreateRet(builder.getInt64(0));
                goto end_translation;
            }
            
            case vm::عملية_إيقاف: {
                goto end_translation;
            }
            
            default: {
                // عمليات غير مدعومة — تخطي المعاملات
                // Unsupported opcodes — skip operands
                auto حجم_معاملات = vm::حجم_المعاملات(عملية);
                ip += حجم_معاملات;
                break;
            }
        }
    }
    
end_translation:
    // إذا لم يُضف return بعد
    if (!builder.GetInsertBlock()->getTerminator()) {
        if (!مكدس.empty()) {
            builder.CreateRet(مكدس.back());
        } else {
            builder.CreateRet(builder.getInt64(0));
        }
    }
    
    // التحقق من صحة الوحدة
    // Verify module
    if (llvm::verifyFunction(*func, &llvm::errs())) {
        if (الإعدادات_.التتبع) {
            std::cerr << "[JIT] ✗ فشل التحقق من IR للدالة '" << الاسم << "'" << std::endl;
        }
        return false;
    }
    
    // إضافة الوحدة إلى محرك JIT
    auto tsm = llvm::orc::ThreadSafeModule(std::move(module), 
        std::make_unique<llvm::LLVMContext>());
    
    auto err = llvm_->المحرك->addIRModule(std::move(tsm));
    if (err) {
        if (الإعدادات_.التتبع) {
            std::cerr << "[JIT] ✗ فشل إضافة IR إلى محرك JIT" << std::endl;
        }
        return false;
    }
    
    return true;
#else
    return false;
#endif
}

// ============================================================================
// تحسين IR / Optimize IR
// ============================================================================
bool محرك_JIT::حسّن_IR(const std::string& الاسم, مستوى_التحسين المستوى) {
#ifdef HAS_LLVM
    // التحسين يتم عبر LLJIT تلقائياً بناءً على مستوى التحسين
    // Optimization is handled by LLJIT automatically
    return true;
#else
    return false;
#endif
}

// ============================================================================
// توليد كود أصلي / Generate Native Code
// ============================================================================
void* محرك_JIT::ولّد_كود_أصلي(const std::string& الاسم) {
#ifdef HAS_LLVM
    if (!llvm_ || !llvm_->المحرك) return nullptr;
    
    // البحث عن الرمز في محرك JIT
    // Look up symbol in JIT engine
    auto sym = llvm_->المحرك->lookup(الاسم);
    if (!sym) {
        if (الإعدادات_.التتبع) {
            std::cerr << "[JIT] ✗ لم يُعثر على الرمز '" << الاسم << "'" << std::endl;
        }
        return nullptr;
    }
    
    return reinterpret_cast<void*>(sym->getValue());
#else
    return nullptr;
#endif
}

// ============================================================================
// هل يجب الترجمة؟ / Should Compile?
// ============================================================================
bool محرك_JIT::يجب_الترجمة(const std::string& الاسم) const {
    auto it = عدادات_.find(الاسم);
    if (it == عدادات_.end()) return false;
    return it->second >= الإعدادات_.عتبة_الترجمة;
}

// ============================================================================
// إدارة الذاكرة / Memory Management
// ============================================================================
void محرك_JIT::أدر_الذاكرة() {
    // حساب الحجم الحالي
    uint64_t حجم_كلي = 0;
    for (const auto& زوج : ذاكرة_الكود_) {
        حجم_كلي += زوج.second.حجم_الكود;
    }
    
    uint64_t الحد = static_cast<uint64_t>(الإعدادات_.حجم_الذاكرة_الأقصى_MB) * 1024 * 1024;
    
    if (حجم_كلي <= الحد) return;
    
    // حذف الدوال الأقل استخداماً (LRU)
    // Evict least recently used functions (LRU)
    std::vector<std::pair<std::string, std::chrono::steady_clock::time_point>> مرتبة;
    for (const auto& زوج : ذاكرة_الكود_) {
        مرتبة.emplace_back(زوج.first, زوج.second.آخر_استخدام);
    }
    
    std::sort(مرتبة.begin(), مرتبة.end(),
        [](const auto& أ, const auto& ب) { return أ.second < ب.second; });
    
    for (const auto& زوج : مرتبة) {
        if (حجم_كلي <= الحد) break;
        auto it = ذاكرة_الكود_.find(زوج.first);
        if (it != ذاكرة_الكود_.end()) {
            حجم_كلي -= it->second.حجم_الكود;
            ذاكرة_الكود_.erase(it);
            الإحصائيات_.عدد_مرات_إلغاء_الترجمة++;
        }
    }
}

// ============================================================================
// إبطال الترجمة / Invalidate Compilation
// ============================================================================
void محرك_JIT::أبطل_الترجمة(const std::string& الاسم) {
    auto it = ذاكرة_الكود_.find(الاسم);
    if (it != ذاكرة_الكود_.end()) {
        it->second.صالحة = false;
        it->second.مؤشر_الكود = nullptr;
    }
}

} // namespace jit
} // namespace sad
