# التجميع الفوري (Just-In-Time Compilation - JIT)

## ما هو التجميع الفوري؟

التجميع الفوري (JIT - Just-In-Time Compilation) هو تقنية متقدمة تجمع بين مزايا التفسير والترجمة. بدلاً من ترجمة البرنامج بالكامل قبل التشغيل (كالمترجم التقليدي) أو تفسيره سطراً بسطر (كالمفسر)، يقوم JIT بترجمة أجزاء من البرنامج أثناء تشغيله. عندما يتم تنفيذ جزء من الكود عدة مرات (يسمى "المسار الساخن" - Hot Path)، يقوم JIT بترجمته إلى كود آلة أصلي لتسريع التنفيذات التالية.

هذا النهج يعطي أفضل ما في العالمين: سرعة بدء التشغيل مثل المفسر، وأداء تنفيذ قريب من الكود المترجم. بالإضافة إلى ذلك، JIT يمكنه تطبيق تحسينات لا يستطيع المترجم التقليدي تطبيقها لأنه يعرف معلومات وقت التشغيل الفعلية مثل القيم الأكثر شيوعاً وأنماط الاستخدام الحقيقية.

## بنية محرك JIT

يتواجد محرك JIT في المجلد `compiler_new/src/jit/` ويتكون من:

```
compiler_new/src/jit/
├── jit_engine.cpp        # المحرك الرئيسي
├── jit_cache.cpp         # ذاكرة التخزين المؤقت للكود المترجم
├── jit_profiler.cpp      # محلل الأداء
├── jit_metrics.cpp       # جمع الإحصائيات
├── jit_hash.cpp          # حساب التجزئة للتخزين المؤقت
├── jit_bridge.cpp        # الجسر مع الآلة الافتراضية
└── hot_path_detector.cpp # كشف المسارات الساخنة
```

## المكونات الرئيسية

### محرك JIT (JIT Engine)

المحرك الرئيسي يدير عملية التجميع الفوري بالكامل:

```cpp
// من jit_engine.cpp
struct JITEngine::Impl {
#ifdef ENABLE_LLVM_BACKEND
    // مدير سياق LLVM
    std::unique_ptr<LLVMContextManager> llvm_context_mgr_;
    
    // مولد LLVM IR
    std::unique_ptr<LLVMGenerator> llvm_generator_;
    
    // خريطة الدوال المُجمّعة
    std::unordered_map<std::string, void*> compiled_functions_;
#endif
    
    // مكونات Sad
    std::unique_ptr<JITCache> cache_;           // الذاكرة المؤقتة
    std::unique_ptr<HotPathDetector> detector_; // كاشف المسارات الساخنة
    JITConfig config_;                          // الإعدادات
    JITStatistics stats_;                       // الإحصائيات
    
    // معلومات النظام
    std::string target_triple_;                 // معمارية الهدف
    std::string cpu_name_;                      // اسم المعالج
    std::string cpu_features_;                  // خصائص المعالج
};
```

### كاشف المسارات الساخنة (Hot Path Detector)

يراقب تنفيذ البرنامج لاكتشاف الأجزاء التي تُنفَّذ كثيراً:

```cpp
JITEngine::JITEngine(const JITConfig& config)
    : pimpl_(std::make_unique<Impl>())
    , mutex_()
{
    // إنشاء كاشف المسارات الساخنة
    if (config.enable_tiered_compilation) {
        TieringConfig tier_config;
        tier_config.tier1_threshold = 10;    // O0 - بدون تحسين
        tier_config.tier2_threshold = 50;    // O1 - تحسينات بسيطة
        tier_config.tier3_threshold = 200;   // O2 - تحسينات متوسطة
        tier_config.tier4_threshold = 1000;  // O3 + PGO - تحسينات قصوى
        
        pimpl_->detector_ = std::make_unique<HotPathDetector>(tier_config);
        
        // تسجيل callback للمسارات الساخنة
        pimpl_->detector_->onHotPath([this](const HotPathInfo& info) {
            handleHotPath(info);
        });
    }
}
```

### التجميع المتدرج (Tiered Compilation)

JIT يستخدم نظام تجميع متدرج يرفع مستوى التحسين تدريجياً:

**المستوى 0 (التفسير):** الكود يُفسَّر مباشرة، أسرع في البدء لكن أبطأ في التنفيذ.

**المستوى 1 (O0):** ترجمة سريعة بدون تحسينات، للكود الذي يُنفَّذ قليلاً.

**المستوى 2 (O1):** تحسينات أساسية مثل constant folding، للكود متوسط الاستخدام.

**المستوى 3 (O2):** تحسينات متوسطة مثل inlining، للكود الساخن.

**المستوى 4 (O3 + PGO):** تحسينات قصوى مع Profile-Guided Optimization، للكود شديد الحرارة.

```
عدد التنفيذات    |    مستوى التحسين
─────────────────┼────────────────────
0-9              |    تفسير (Tier 0)
10-49            |    O0 (Tier 1)
50-199           |    O1 (Tier 2)
200-999          |    O2 (Tier 3)
1000+            |    O3 + PGO (Tier 4)
```

### ذاكرة التخزين المؤقت (JIT Cache)

الكود المترجم يُخزَّن مؤقتاً لتجنب إعادة الترجمة:

```cpp
// إنشاء الذاكرة المؤقتة
if (config.enable_cache) {
    pimpl_->cache_ = std::make_unique<JITCache>(
        config.max_cache_size_mb,
        CacheEvictionPolicy::LRU  // إزالة الأقل استخداماً
    );
}
```

سياسات الإزالة المدعومة:
- **LRU (Least Recently Used):** إزالة الكود الأقل استخداماً مؤخراً
- **LFU (Least Frequently Used):** إزالة الكود الأقل تكراراً
- **FIFO (First In First Out):** إزالة الكود الأقدم

## عملية التجميع الفوري

### الخطوة 1: المراقبة

أثناء تنفيذ البرنامج، يراقب JIT عدد مرات تنفيذ كل دالة:

```cpp
void JITEngine::recordFunctionCall(const std::string& funcName) {
    auto& count = pimpl_->call_counts_[funcName];
    count++;
    
    // التحقق من العتبة
    if (count == pimpl_->config_.hot_threshold) {
        scheduleCompilation(funcName, OptLevel::O1);
    }
}
```

### الخطوة 2: الكشف عن المسارات الساخنة

عندما يتجاوز عداد الدالة العتبة المحددة:

```cpp
void HotPathDetector::onCall(const std::string& funcName) {
    auto& info = paths_[funcName];
    info.call_count++;
    info.last_call_time = now();
    
    // التحقق من المستويات المختلفة
    if (info.call_count >= config_.tier4_threshold && info.tier < 4) {
        info.tier = 4;
        callback_(info);
    } else if (info.call_count >= config_.tier3_threshold && info.tier < 3) {
        // ...
    }
}
```

### الخطوة 3: الترجمة

JIT يستخدم LLVM ORC لترجمة الكود:

```cpp
bool JITEngine::initialize(const std::string& target_triple) {
    // تهيئة LLVM ORC JIT
    pimpl_->llvm_context_mgr_ = std::make_unique<LLVMContextManager>();
    
    if (!pimpl_->llvm_context_mgr_->initialize(pimpl_->target_triple_)) {
        std::cerr << "❌ فشل تهيئة LLVM Context Manager\n";
        return false;
    }
    
    // إنشاء مولد LLVM IR
    pimpl_->llvm_generator_ = std::make_unique<LLVMGenerator>();
    
    pimpl_->is_initialized_ = true;
    return true;
}
```

### الخطوة 4: التنفيذ

بعد الترجمة، الكود الأصلي يحل محل الكود المفسر:

```cpp
void* JITEngine::getCompiledFunction(const std::string& funcName) {
    auto it = pimpl_->compiled_functions_.find(funcName);
    if (it != pimpl_->compiled_functions_.end()) {
        return it->second;  // إرجاع المؤشر للدالة المترجمة
    }
    return nullptr;  // الدالة غير مترجمة بعد
}
```

## تحسينات وقت التشغيل

JIT يمكنه تطبيق تحسينات لا يستطيع المترجم التقليدي تطبيقها:

### Deoptimization

إذا تغيرت الافتراضات التي بُني عليها التحسين:

```cpp
// مثال: إذا افترضنا أن متغير دائماً رقم صحيح
// لكنه أصبح نصاً، نحتاج للعودة للتفسير
void JITEngine::deoptimize(const std::string& funcName) {
    pimpl_->compiled_functions_.erase(funcName);
    // العودة للتفسير
}
```

### Profile-Guided Optimization (PGO)

JIT يجمع معلومات عن سلوك البرنامج الفعلي:

- أي الفروع تُأخذ أكثر
- ما هي الأنواع الفعلية للمتغيرات
- أنماط الوصول للذاكرة

### Inline Caching

للعمليات متعددة الأشكال (Polymorphic):

```cpp
// بدلاً من البحث عن الطريقة في كل مرة
// نخزن مؤقتاً موقعها إذا كان النوع ثابتاً
struct InlineCache {
    TypeId expected_type;
    void* cached_method;
};
```

## الإحصائيات والمقاييس

JIT يجمع إحصائيات مفصلة:

```cpp
struct JITStatistics {
    uint64_t total_compilations;      // عدد الترجمات
    uint64_t compilation_time_us;     // وقت الترجمة
    uint64_t cache_hits;              // إصابات الذاكرة المؤقتة
    uint64_t cache_misses;            // أخطاء الذاكرة المؤقتة
    uint64_t deoptimizations;         // عدد التراجعات
    uint64_t hot_functions;           // عدد الدوال الساخنة
    size_t   code_size_bytes;         // حجم الكود المترجم
};
```

## إعدادات JIT

يمكن تخصيص سلوك JIT:

```cpp
struct JITConfig {
    bool enable_jit = true;              // تفعيل JIT
    bool enable_cache = true;            // تفعيل التخزين المؤقت
    bool enable_tiered_compilation = true; // التجميع المتدرج
    uint32_t hot_threshold = 100;        // عتبة المسار الساخن
    uint32_t max_cache_size_mb = 64;     // حجم الذاكرة المؤقتة
    OptLevel default_opt_level = O1;     // مستوى التحسين الافتراضي
};
```

## متى يفيد JIT؟

- **الحلقات الطويلة:** كود يُنفَّذ آلاف المرات
- **الدوال المتكررة:** دوال تُستدعى كثيراً
- **الخوارزميات الحسابية:** عمليات رياضية مكثفة
- **معالجة البيانات:** عمليات على مصفوفات كبيرة

## مقارنة الأداء

```
النهج              | سرعة البدء | سرعة التنفيذ | استخدام الذاكرة
───────────────────┼────────────┼──────────────┼────────────────
المفسر             | ⭐⭐⭐⭐⭐  | ⭐            | ⭐⭐⭐⭐⭐
الآلة الافتراضية  | ⭐⭐⭐⭐   | ⭐⭐          | ⭐⭐⭐⭐
JIT               | ⭐⭐⭐      | ⭐⭐⭐⭐      | ⭐⭐⭐
المترجم التقليدي  | ⭐          | ⭐⭐⭐⭐⭐    | ⭐⭐⭐⭐
```

## الخلاصة

التجميع الفوري في لغة ص يمثل تقنية متطورة تجمع بين مرونة التفسير وسرعة الترجمة. من خلال المراقبة الذكية للكود وترجمة الأجزاء الساخنة فقط، يحقق JIT توازناً ممتازاً بين سرعة البدء وأداء التنفيذ. التكامل مع LLVM يضمن أن الكود المترجم يحصل على تحسينات على مستوى عالمي. نظام التجميع المتدرج يسمح بتخصيص مستوى التحسين حسب أهمية الكود، مما يوفر موارد النظام مع الحفاظ على الأداء العالي للأجزاء الحرجة.
