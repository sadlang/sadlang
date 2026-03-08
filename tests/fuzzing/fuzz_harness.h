/**
 * @file fuzz_harness.cpp
 * @brief إطار Fuzzing للمفسر والمترجم
 * @brief (EN) Fuzzing harness for interpreter and compiler
 * 
 * @author أحمد
 * @date نوفمبر 2026
 */

#include &lt;cstdint&gt;
#include &lt;cstddef&gt;
#include &lt;string&gt;
#include &lt;vector&gt;
#include &lt;random&gt;
#include &lt;chrono&gt;
#include &lt;fstream&gt;
#include &lt;filesystem&gt;
#include &lt;functional&gt;

namespace Sad {
namespace Fuzzing {

//==============================================================================
// أنواع الـ Fuzzing
//==============================================================================

/**
 * @brief نوع الهدف
 */
enum class FuzzTarget {
    LEXER,          // المحلل المعجمي
    PARSER,         // المحلل النحوي
    INTERPRETER,    // المفسر
    COMPILER,       // المترجم
    FULL_PIPELINE   // المسار الكامل
};

/**
 * @brief استراتيجية الطفرات
 */
enum class MutationStrategy {
    BIT_FLIP,           // قلب بتات
    BYTE_FLIP,          // قلب بايتات
    ARITHMETIC,         // عمليات حسابية
    INTERESTING_VALUES, // قيم مثيرة
    HAVOC,              // فوضى عشوائية
    SPLICE,             // دمج مدخلات
    DICTIONARY,         // كلمات من قاموس
    GRAMMAR_AWARE       // واعي بالقواعد
};

//==============================================================================
// مولد الطفرات
//==============================================================================

/**
 * @brief مولد الطفرات للمدخلات
 */
class Mutator {
public:
    Mutator(uint64_t seed = 0) 
        : rng_(seed == 0 ? std::random_device{}() : seed) {}
    
    /**
     * @brief تطبيق طفرة على المدخل
     */
    std::string mutate(const std::string&amp; input, MutationStrategy strategy) {
        switch (strategy) {
            case MutationStrategy::BIT_FLIP:
                return bitFlip(input);
            case MutationStrategy::BYTE_FLIP:
                return byteFlip(input);
            case MutationStrategy::ARITHMETIC:
                return arithmetic(input);
            case MutationStrategy::INTERESTING_VALUES:
                return insertInteresting(input);
            case MutationStrategy::HAVOC:
                return havoc(input);
            case MutationStrategy::SPLICE:
                return splice(input);
            case MutationStrategy::DICTIONARY:
                return insertFromDictionary(input);
            case MutationStrategy::GRAMMAR_AWARE:
                return grammarAwareMutation(input);
            default:
                return input;
        }
    }
    
    /**
     * @brief إضافة كلمات للقاموس
     */
    void addToDictionary(const std::vector&lt;std::string&gt;&amp; words) {
        dictionary_.insert(dictionary_.end(), words.begin(), words.end());
    }
    
    /**
     * @brief تعيين مكتبة من المدخلات للدمج
     */
    void setSpliceCorpus(const std::vector&lt;std::string&gt;&amp; corpus) {
        spliceCorpus_ = corpus;
    }

private:
    std::mt19937_64 rng_;
    std::vector&lt;std::string&gt; dictionary_;
    std::vector&lt;std::string&gt; spliceCorpus_;
    
    // القيم المثيرة
    static constexpr int64_t interestingInts[] = {
        -1, 0, 1, 2, 127, 128, 255, 256, 32767, 32768, 65535, 65536,
        INT32_MAX, INT32_MIN, INT64_MAX, INT64_MIN
    };
    
    std::string bitFlip(const std::string&amp; input) {
        if (input.empty()) return input;
        std::string result = input;
        size_t pos = rng_() % (result.size() * 8);
        result[pos / 8] ^= (1 &lt;&lt; (pos % 8));
        return result;
    }
    
    std::string byteFlip(const std::string&amp; input) {
        if (input.empty()) return input;
        std::string result = input;
        size_t pos = rng_() % result.size();
        result[pos] ^= 0xFF;
        return result;
    }
    
    std::string arithmetic(const std::string&amp; input) {
        if (input.empty()) return input;
        std::string result = input;
        size_t pos = rng_() % result.size();
        int delta = (rng_() % 35) - 17; // -17 to +17
        result[pos] = static_cast&lt;char&gt;(result[pos] + delta);
        return result;
    }
    
    std::string insertInteresting(const std::string&amp; input) {
        std::string result = input;
        size_t pos = rng_() % (result.size() + 1);
        int64_t val = interestingInts[rng_() % (sizeof(interestingInts)/sizeof(int64_t))];
        result.insert(pos, std::to_string(val));
        return result;
    }
    
    std::string havoc(const std::string&amp; input) {
        std::string result = input;
        int numMutations = (rng_() % 8) + 1;
        
        for (int i = 0; i &lt; numMutations; ++i) {
            int op = rng_() % 5;
            switch (op) {
                case 0: // حذف
                    if (!result.empty()) {
                        size_t pos = rng_() % result.size();
                        result.erase(pos, 1);
                    }
                    break;
                case 1: // إدراج
                    {
                        size_t pos = rng_() % (result.size() + 1);
                        result.insert(pos, 1, static_cast&lt;char&gt;(rng_() % 256));
                    }
                    break;
                case 2: // استبدال
                    if (!result.empty()) {
                        size_t pos = rng_() % result.size();
                        result[pos] = static_cast&lt;char&gt;(rng_() % 256);
                    }
                    break;
                case 3: // تكرار
                    if (!result.empty()) {
                        size_t pos = rng_() % result.size();
                        size_t len = std::min(rng_() % 8 + 1, result.size() - pos);
                        result.insert(pos, result.substr(pos, len));
                    }
                    break;
                case 4: // قلب
                    result = bitFlip(result);
                    break;
            }
        }
        return result;
    }
    
    std::string splice(const std::string&amp; input) {
        if (spliceCorpus_.empty()) return input;
        const std::string&amp; other = spliceCorpus_[rng_() % spliceCorpus_.size()];
        
        size_t splitA = rng_() % (input.size() + 1);
        size_t splitB = rng_() % (other.size() + 1);
        
        return input.substr(0, splitA) + other.substr(splitB);
    }
    
    std::string insertFromDictionary(const std::string&amp; input) {
        if (dictionary_.empty()) return input;
        const std::string&amp; word = dictionary_[rng_() % dictionary_.size()];
        size_t pos = rng_() % (input.size() + 1);
        
        std::string result = input;
        result.insert(pos, word);
        return result;
    }
    
    std::string grammarAwareMutation(const std::string&amp; input);
};

//==============================================================================
// قاموس لغة ص
//==============================================================================

/**
 * @brief قاموس الكلمات المفتاحية للغة ص
 */
class SadDictionary {
public:
    static std::vector&lt;std::string&gt; getKeywords() {
        return {
            // الكلمات المحجوزة
            "دالة", "ارجع", "صنف", "بنية", "تعداد", "يرث", "نهاية",
            "جديد", "هذا", "باني", "الأساس",
            "إذا", "وإلا", "بينما", "لكل", "في", "توقف", "استمر",
            "طابق", "عندما", "افتراضي",
            "حاول", "امسك", "ارمي", "أخيراً",
            "عام", "خاص", "محمي", "مجرد",
            "استورد", "من", "كـ", "صدّر",
            "متغير", "ثابت", "ساكن", "خارجي",
            "صحيح", "خطأ", "لاشيء",
            "و", "أو", "ليس",
            
            // أسماء الأنواع
            "رقم", "عشري", "نص", "منطقي", "فراغ", "عدم", "مصفوفة", "خريطة", "أي",
            
            // عوامل
            "+", "-", "*", "/", "%", "**", "==", "!=", "&lt;", "&gt;", "&lt;=", "&gt;=",
            "=", "+=", "-=", "*=", "/=", ".", "[", "]", "(", ")", "{", "}",
            
            // حالات حدية
            "", " ", "\n", "\t", "\r\n",
            "0", "1", "-1", "999999999999999999",
            "\"\"", "\"نص\"", "\"\\n\"", "\"\\t\"",
        };
    }
    
    static std::vector&lt;std::string&gt; getTemplates() {
        return {
            "دالة ف()\nنهاية",
            "دالة ف(أ)\n  ارجع أ\nنهاية",
            "إذا (صحيح)\n  اطبع(1)\nنهاية",
            "بينما (صحيح)\n  توقف\nنهاية",
            "لكل س في [1,2,3]\n  اطبع(س)\nنهاية",
            "صنف أ\n  باني()\n  نهاية\nنهاية",
            "متغير س = 0",
            "ثابت ص = \"نص\"",
            "حاول\n  ارمي \"خطأ\"\nامسك (خ)\n  اطبع(خ)\nنهاية",
        };
    }
};

//==============================================================================
// تتبع التغطية
//==============================================================================

/**
 * @brief تتبع تغطية الكود
 */
class CoverageTracker {
public:
    void recordBranch(uint64_t branchId) {
        branches_.insert(branchId);
    }
    
    void recordEdge(uint64_t from, uint64_t to) {
        edges_.insert((from &lt;&lt; 32) | to);
    }
    
    size_t getBranchCount() const { return branches_.size(); }
    size_t getEdgeCount() const { return edges_.size(); }
    
    bool hasNewCoverage() const {
        size_t total = branches_.size() + edges_.size();
        bool isNew = total &gt; lastTotal_;
        lastTotal_ = total;
        return isNew;
    }
    
    void exportReport(const std::string&amp; path) const {
        std::ofstream out(path);
        out &lt;&lt; "# Coverage Report\n";
        out &lt;&lt; "Branches: " &lt;&lt; branches_.size() &lt;&lt; "\n";
        out &lt;&lt; "Edges: " &lt;&lt; edges_.size() &lt;&lt; "\n";
    }
    
private:
    std::set&lt;uint64_t&gt; branches_;
    std::set&lt;uint64_t&gt; edges_;
    mutable size_t lastTotal_ = 0;
};

//==============================================================================
// محرك Fuzzing
//==============================================================================

/**
 * @brief نتيجة تشغيل واحد
 */
struct FuzzResult {
    bool crashed = false;
    bool timeout = false;
    bool newCoverage = false;
    std::string errorMessage;
    std::chrono::microseconds duration;
};

/**
 * @brief إحصائيات الـ Fuzzing
 */
struct FuzzStats {
    uint64_t totalRuns = 0;
    uint64_t crashes = 0;
    uint64_t timeouts = 0;
    uint64_t newCoverageFound = 0;
    double avgDurationUs = 0;
    std::chrono::steady_clock::time_point startTime;
};

/**
 * @brief محرك Fuzzing الرئيسي
 */
class FuzzEngine {
public:
    using TargetFunction = std::function&lt;void(const uint8_t*, size_t)&gt;;
    
    FuzzEngine(FuzzTarget target, uint64_t seed = 0)
        : target_(target)
        , mutator_(seed)
        , maxInputSize_(1024 * 1024)  // 1 MB
        , timeoutMs_(5000)
    {
        // إعداد قاموس لغة ص
        mutator_.addToDictionary(SadDictionary::getKeywords());
        
        stats_.startTime = std::chrono::steady_clock::now();
    }
    
    /**
     * @brief تعيين المدخلات الأولية
     */
    void setSeedCorpus(const std::vector&lt;std::string&gt;&amp; seeds) {
        corpus_ = seeds;
        mutator_.setSpliceCorpus(seeds);
    }
    
    /**
     * @brief تحميل المدخلات من مجلد
     */
    void loadCorpusFromDir(const std::string&amp; dir) {
        for (const auto&amp; entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::ifstream file(entry.path());
                std::string content((std::istreambuf_iterator&lt;char&gt;(file)),
                                     std::istreambuf_iterator&lt;char&gt;());
                corpus_.push_back(content);
            }
        }
        mutator_.setSpliceCorpus(corpus_);
    }
    
    /**
     * @brief تشغيل الـ Fuzzing
     */
    void run(TargetFunction targetFn, uint64_t maxIterations = 0) {
        targetFn_ = targetFn;
        
        // إذا لم يكن هناك مدخلات، استخدم القوالب
        if (corpus_.empty()) {
            corpus_ = SadDictionary::getTemplates();
        }
        
        uint64_t iteration = 0;
        while (maxIterations == 0 || iteration &lt; maxIterations) {
            // اختيار مدخل من المكتبة
            const std::string&amp; base = corpus_[iteration % corpus_.size()];
            
            // اختيار استراتيجية طفرة
            MutationStrategy strategy = selectStrategy();
            
            // تطبيق الطفرة
            std::string mutated = mutator_.mutate(base, strategy);
            
            // تنفيذ المدخل
            FuzzResult result = executeOne(mutated);
            
            // تحديث الإحصائيات
            updateStats(result);
            
            // حفظ المدخلات المثيرة
            if (result.crashed) {
                saveCrash(mutated, result);
            } else if (result.newCoverage) {
                corpus_.push_back(mutated);
            }
            
            ++iteration;
            
            // طباعة التقدم كل 1000 تكرار
            if (iteration % 1000 == 0) {
                printProgress();
            }
        }
    }
    
    /**
     * @brief الحصول على الإحصائيات
     */
    const FuzzStats&amp; getStats() const { return stats_; }
    
    /**
     * @brief تصدير تقرير
     */
    void exportReport(const std::string&amp; path) const {
        std::ofstream out(path);
        auto elapsed = std::chrono::steady_clock::now() - stats_.startTime;
        auto elapsedSec = std::chrono::duration_cast&lt;std::chrono::seconds&gt;(elapsed).count();
        
        out &lt;&lt; "# Fuzzing Report - Sad Language\n\n";
        out &lt;&lt; "## Summary\n";
        out &lt;&lt; "- Target: " &lt;&lt; static_cast&lt;int&gt;(target_) &lt;&lt; "\n";
        out &lt;&lt; "- Duration: " &lt;&lt; elapsedSec &lt;&lt; "s\n";
        out &lt;&lt; "- Total Runs: " &lt;&lt; stats_.totalRuns &lt;&lt; "\n";
        out &lt;&lt; "- Exec/sec: " &lt;&lt; (elapsedSec &gt; 0 ? stats_.totalRuns / elapsedSec : 0) &lt;&lt; "\n";
        out &lt;&lt; "- Crashes: " &lt;&lt; stats_.crashes &lt;&lt; "\n";
        out &lt;&lt; "- Timeouts: " &lt;&lt; stats_.timeouts &lt;&lt; "\n";
        out &lt;&lt; "- New Coverage: " &lt;&lt; stats_.newCoverageFound &lt;&lt; "\n";
        out &lt;&lt; "- Corpus Size: " &lt;&lt; corpus_.size() &lt;&lt; "\n";
        
        coverage_.exportReport(path + ".coverage");
    }

private:
    FuzzTarget target_;
    Mutator mutator_;
    CoverageTracker coverage_;
    FuzzStats stats_;
    std::vector&lt;std::string&gt; corpus_;
    TargetFunction targetFn_;
    
    size_t maxInputSize_;
    uint64_t timeoutMs_;
    std::string crashDir_ = "crashes/";
    
    MutationStrategy selectStrategy() {
        static std::mt19937_64 rng(42);
        int choice = rng() % 100;
        
        if (choice &lt; 20) return MutationStrategy::GRAMMAR_AWARE;
        if (choice &lt; 40) return MutationStrategy::DICTIONARY;
        if (choice &lt; 55) return MutationStrategy::HAVOC;
        if (choice &lt; 70) return MutationStrategy::SPLICE;
        if (choice &lt; 80) return MutationStrategy::BIT_FLIP;
        if (choice &lt; 90) return MutationStrategy::BYTE_FLIP;
        return MutationStrategy::ARITHMETIC;
    }
    
    FuzzResult executeOne(const std::string&amp; input) {
        FuzzResult result;
        auto start = std::chrono::steady_clock::now();
        
        try {
            targetFn_(reinterpret_cast&lt;const uint8_t*&gt;(input.data()), input.size());
        } catch (const std::exception&amp; e) {
            result.crashed = true;
            result.errorMessage = e.what();
        } catch (...) {
            result.crashed = true;
            result.errorMessage = "Unknown exception";
        }
        
        auto end = std::chrono::steady_clock::now();
        result.duration = std::chrono::duration_cast&lt;std::chrono::microseconds&gt;(end - start);
        result.newCoverage = coverage_.hasNewCoverage();
        
        return result;
    }
    
    void updateStats(const FuzzResult&amp; result) {
        stats_.totalRuns++;
        if (result.crashed) stats_.crashes++;
        if (result.timeout) stats_.timeouts++;
        if (result.newCoverage) stats_.newCoverageFound++;
        
        // حساب المتوسط المتحرك
        stats_.avgDurationUs = (stats_.avgDurationUs * (stats_.totalRuns - 1) + 
                                result.duration.count()) / stats_.totalRuns;
    }
    
    void saveCrash(const std::string&amp; input, const FuzzResult&amp; result) {
        std::filesystem::create_directories(crashDir_);
        std::string filename = crashDir_ + "crash_" + std::to_string(stats_.crashes) + ".txt";
        std::ofstream out(filename);
        out &lt;&lt; "# Crash #" &lt;&lt; stats_.crashes &lt;&lt; "\n";
        out &lt;&lt; "# Error: " &lt;&lt; result.errorMessage &lt;&lt; "\n\n";
        out &lt;&lt; input;
    }
    
    void printProgress() {
        auto elapsed = std::chrono::steady_clock::now() - stats_.startTime;
        auto elapsedSec = std::chrono::duration_cast&lt;std::chrono::seconds&gt;(elapsed).count();
        
        std::cout &lt;&lt; "\r[" &lt;&lt; elapsedSec &lt;&lt; "s] "
                  &lt;&lt; "runs: " &lt;&lt; stats_.totalRuns
                  &lt;&lt; " | crashes: " &lt;&lt; stats_.crashes
                  &lt;&lt; " | coverage: " &lt;&lt; stats_.newCoverageFound
                  &lt;&lt; " | corpus: " &lt;&lt; corpus_.size()
                  &lt;&lt; " | exec/s: " &lt;&lt; (elapsedSec &gt; 0 ? stats_.totalRuns / elapsedSec : 0)
                  &lt;&lt; std::flush;
    }
};

//==============================================================================
// أهداف Fuzzing جاهزة
//==============================================================================

/**
 * @brief Fuzzing للمحلل المعجمي
 */
extern "C" int LLVMFuzzerTestOneInput_Lexer(const uint8_t* data, size_t size);

/**
 * @brief Fuzzing للمحلل النحوي
 */
extern "C" int LLVMFuzzerTestOneInput_Parser(const uint8_t* data, size_t size);

/**
 * @brief Fuzzing للمفسر
 */
extern "C" int LLVMFuzzerTestOneInput_Interpreter(const uint8_t* data, size_t size);

/**
 * @brief Fuzzing للمترجم
 */
extern "C" int LLVMFuzzerTestOneInput_Compiler(const uint8_t* data, size_t size);

} // namespace Fuzzing
} // namespace Sad

#endif // SAD_FUZZ_HARNESS_H
