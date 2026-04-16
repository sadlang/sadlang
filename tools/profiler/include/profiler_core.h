/**
 * @file profiler_core.h
 * @brief (AR) نواة مصحح الأداء — المحرك الأساسي لتحليل أداء برامج لغة ص
 * @brief (EN) Profiler Core — Main engine for performance analysis of Sad programs
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * الوصف بالعربية:
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يُعرّف النواة الأساسية لمصحح الأداء (Profiler) الخاص بلغة ص.
 * مصحح الأداء أداة حيوية تُساعد المطورين على:
 * 
 * 1. قياس زمن تنفيذ كل دالة (شامل / حصري)
 *    - الزمن الشامل: يشمل زمن الدوال المُستدعاة من داخل الدالة
 *    - الزمن الحصري: زمن الدالة فقط بدون الدوال الفرعية
 * 
 * 2. تتبع عدد مرات استدعاء كل دالة
 *    - كم مرة استُدعيت كل دالة
 *    - معدل الاستدعاء (مرات/ثانية)
 * 
 * 3. تحليل شجرة الاستدعاءات (Call Tree)
 *    - أي دالة تستدعي أي دالة أخرى
 *    - عمق سلسلة الاستدعاء
 * 
 * 4. اكتشاف نقاط الاختناق (Bottlenecks)
 *    - الدوال الأبطأ في البرنامج
 *    - الدوال الأكثر استدعاءً
 *    - نقاط تسرب الأداء (Hot spots)
 * 
 * 5. تحليل استخدام الذاكرة
 *    - عدد الكائنات المُنشأة
 *    - عدد المتغيرات المُعرّفة
 *    - عمليات النطاق (دخول/خروج)
 * 
 * 6. إنتاج تقارير متعددة الصيغ
 *    - تقرير نصي ملون بالعربية
 *    - تقرير JSON للأدوات الخارجية
 *    - تقرير HTML تفاعلي
 *    - تقرير FlameGraph (مخطط اللهب)
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * البنية المعمارية:
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 *   ProfilerCore ──► FunctionProfileData    (بيانات كل دالة)
 *        │       ──► CallTreeNode           (شجرة الاستدعاءات)
 *        │       ──► MemorySnapshot         (لقطات الذاكرة)
 *        │       ──► HotSpotInfo            (نقاط الاختناق)
 *        ▼
 *   ReportGenerator ──► TextReport          (تقرير نصي)
 *                   ──► JSONReport          (تقرير JSON)
 *                   ──► HTMLReport          (تقرير HTML)
 *                   ──► FlameGraphReport    (مخطط اللهب)
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * آلية العمل:
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * 1. يتم تفعيل المصحح عبر خيار --profile في سطر الأوامر
 * 2. عند دخول كل دالة، يُسجل المصحح الزمن الحالي (onFunctionEnter)
 * 3. عند خروج كل دالة، يحسب المصحح الزمن المنقضي (onFunctionExit)
 * 4. يبني المصحح شجرة الاستدعاءات أثناء التنفيذ
 * 5. بعد انتهاء البرنامج، يُنتج المصحح التقارير المطلوبة
 * 
 * @author فريق تطوير لغة ص
 * @date 2025
 * @version 1.0.0
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <stack>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <functional>
#include <cmath>

namespace Sad {
namespace Tools {

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) أنواع الساعة عالية الدقة
// (EN) High-resolution clock types
// ═══════════════════════════════════════════════════════════════════════════════
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;
using Duration = std::chrono::nanoseconds;

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) صيغ التقارير المدعومة
// (EN) Supported report formats
// ═══════════════════════════════════════════════════════════════════════════════
enum class ReportFormat {
    TEXT,           ///< (AR) تقرير نصي ملون / (EN) Colored text report
    JSON,           ///< (AR) تقرير JSON / (EN) JSON report
    HTML,           ///< (AR) تقرير HTML تفاعلي / (EN) Interactive HTML report
    FLAMEGRAPH,     ///< (AR) مخطط اللهب / (EN) Flame graph
    CSV             ///< (AR) جدول CSV / (EN) CSV spreadsheet
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) خيارات مصحح الأداء
// (EN) Profiler options
// ═══════════════════════════════════════════════════════════════════════════════
struct ProfilerOptions {
    bool enabled = false;               ///< (AR) تفعيل المصحح / (EN) Enable profiler
    bool trackMemory = true;            ///< (AR) تتبع الذاكرة / (EN) Track memory
    bool trackCallTree = true;          ///< (AR) بناء شجرة الاستدعاءات / (EN) Build call tree
    bool trackLineLevel = false;        ///< (AR) تتبع على مستوى السطر / (EN) Line-level tracking
    bool colorOutput = true;            ///< (AR) إخراج ملون / (EN) Colored output
    bool arabicOutput = true;           ///< (AR) إخراج بالعربية / (EN) Arabic output
    bool excludeBuiltins = false;       ///< (AR) استبعاد الدوال المدمجة من التقرير / (EN) Exclude builtins from report
    bool showPercentBar = true;         ///< (AR) عرض شريط النسبة / (EN) Show percentage bar
    bool addTimestamp = true;           ///< (AR) إضافة طابع زمني / (EN) Add timestamp
    bool utfBom = false;                ///< (AR) إضافة BOM لملفات UTF-8 / (EN) Add UTF-8 BOM
    size_t maxCallDepth = 1000;         ///< (AR) أقصى عمق للاستدعاء / (EN) Maximum call depth
    size_t topFunctionsCount = 20;      ///< (AR) عدد الدوال الأعلى في التقرير / (EN) Top functions in report
    size_t maxTreeDepth = 30;           ///< (AR) أقصى عمق لعرض الشجرة / (EN) Max call tree display depth
    size_t maxTrackedFunctions = 10000;  ///< (AR) أقصى عدد دوال مُتتبعة / (EN) Max tracked functions (memory guard)
    double thresholdMs = 0.0;           ///< (AR) عتبة العرض (مللي ثانية) / (EN) Display threshold (ms)
    std::string outputFile;             ///< (AR) ملف الإخراج / (EN) Output file
    std::string filterPattern;          ///< (AR) نمط تصفية أسماء الدوال / (EN) Function name filter pattern
    size_t samplingRate = 1;             ///< (AR) معدل أخذ العينات (1=كل استدعاء) / (EN) Sampling rate (1=every call)
    ReportFormat format = ReportFormat::TEXT;  ///< (AR) صيغة التقرير / (EN) Report format
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) بيانات أداء دالة واحدة
// (EN) Performance data for a single function
// ═══════════════════════════════════════════════════════════════════════════════
/**
 * @brief (AR) بيانات أداء دالة واحدة — تُجمع إحصائيات التنفيذ لكل دالة
 * @brief (EN) Single function performance data — collects execution statistics
 * 
 * يحتوي على:
 * - عدد مرات الاستدعاء
 * - الزمن الشامل (inclusive): يشمل الدوال الفرعية
 * - الزمن الحصري (exclusive): بدون الدوال الفرعية
 * - أطول وأقصر استدعاء
 * - السطر والملف
 */
struct FunctionProfileData {
    std::string name;                   ///< (AR) اسم الدالة / (EN) Function name
    std::string fileName;               ///< (AR) اسم الملف / (EN) File name
    size_t line = 0;                    ///< (AR) رقم السطر / (EN) Line number
    
    // (AR) إحصائيات الاستدعاء / (EN) Call statistics
    uint64_t callCount = 0;             ///< (AR) عدد مرات الاستدعاء / (EN) Call count
    uint64_t recursiveCallCount = 0;    ///< (AR) عدد الاستدعاءات التكرارية / (EN) Recursive call count
    
    // (AR) إحصائيات الزمن (بالنانو ثانية) / (EN) Time statistics (nanoseconds)
    int64_t totalInclusiveNs = 0;       ///< (AR) الزمن الشامل الكلي / (EN) Total inclusive time
    int64_t totalExclusiveNs = 0;       ///< (AR) الزمن الحصري الكلي / (EN) Total exclusive time
    int64_t minInclusiveNs = INT64_MAX; ///< (AR) أقصر استدعاء / (EN) Minimum call time
    int64_t maxInclusiveNs = 0;         ///< (AR) أطول استدعاء / (EN) Maximum call time
    
    // (AR) هل الدالة مضمنة؟ / (EN) Is it a built-in function?
    bool isBuiltin = false;
    
    // (AR) الدوال التي تستدعيها هذه الدالة / (EN) Functions called by this function
    std::unordered_map<std::string, uint64_t> callees;
    
    // (AR) الدوال التي تستدعي هذه الدالة / (EN) Functions that call this function
    std::unordered_map<std::string, uint64_t> callers;
    
    // ─── (AR) دوال مساعدة / (EN) Helper functions ───
    
    /// (AR) متوسط الزمن الشامل / (EN) Average inclusive time
    double avgInclusiveNs() const {
        return callCount > 0 ? static_cast<double>(totalInclusiveNs) / callCount : 0.0;
    }
    
    /// (AR) متوسط الزمن الحصري / (EN) Average exclusive time
    double avgExclusiveNs() const {
        return callCount > 0 ? static_cast<double>(totalExclusiveNs) / callCount : 0.0;
    }
    
    /// (AR) معدل الاستدعاء (مرات/ثانية) / (EN) Call rate (calls/second)
    double callRatePerSecond(int64_t totalTimeNs) const {
        if (totalTimeNs <= 0) return 0.0;
        return static_cast<double>(callCount) / (totalTimeNs / 1.0e9);
    }
    
    /// (AR) نسبة الاستدعاءات التكرارية / (EN) Recursive call percentage
    double recursivePercent() const {
        return callCount > 0 ? (static_cast<double>(recursiveCallCount) / callCount) * 100.0 : 0.0;
    }
    
    /// (AR) نسبة التباين (أقصى/أدنى) / (EN) Variance ratio (max/min)
    double varianceRatio() const {
        if (minInclusiveNs <= 0 || minInclusiveNs == INT64_MAX) return 0.0;
        return static_cast<double>(maxInclusiveNs) / minInclusiveNs;
    }
    
    /// (AR) الانحراف المعياري المقدّر / (EN) Estimated standard deviation
    double estimatedStdDevNs() const {
        if (callCount < 2) return 0.0;
        double mean = avgInclusiveNs();
        // (AR) تقدير تقريبي: (max - min) / 4 
        double range = static_cast<double>(maxInclusiveNs - (minInclusiveNs == INT64_MAX ? 0 : minInclusiveNs));
        return range / 4.0;
    }
    
    /// (AR) الكفاءة (استدعاءات/مللي ثانية) / (EN) Efficiency (calls/ms)
    double efficiency() const {
        if (totalInclusiveNs <= 0) return 0.0;
        return static_cast<double>(callCount) / (totalInclusiveNs / 1'000'000.0);
    }
    
    /// (AR) هل الدالة "باردة"؟ (استدعاء واحد) / (EN) Is it a "cold" function? (single call)
    bool isCold() const { return callCount == 1; }
    
    /// (AR) هل الدالة "ساخنة"؟ (أكثر من 1000 استدعاء) / (EN) Is it "hot"? (>1000 calls)
    bool isHot() const { return callCount > 1000; }
    
    /// (AR) تحويل نانو ثانية إلى مللي ثانية / (EN) Nanoseconds to milliseconds
    static double nsToMs(int64_t ns) {
        return static_cast<double>(ns) / 1'000'000.0;
    }
    
    /// (AR) تحويل نانو ثانية إلى ميكرو ثانية / (EN) Nanoseconds to microseconds
    static double nsToUs(int64_t ns) {
        return static_cast<double>(ns) / 1'000.0;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) عقدة في شجرة الاستدعاءات
// (EN) Node in the call tree
// ═══════════════════════════════════════════════════════════════════════════════
/**
 * @brief (AR) عقدة في شجرة الاستدعاءات — تمثل استدعاء دالة واحد
 * @brief (EN) Call tree node — represents a single function call
 * 
 * شجرة الاستدعاءات تُبنى أثناء التنفيذ وتُظهر علاقات
 * الاستدعاء بين الدوال (أي دالة تستدعي أي دالة أخرى).
 */
struct CallTreeNode {
    std::string functionName;           ///< (AR) اسم الدالة / (EN) Function name
    int64_t inclusiveNs = 0;            ///< (AR) الزمن الشامل / (EN) Inclusive time
    int64_t exclusiveNs = 0;            ///< (AR) الزمن الحصري / (EN) Exclusive time
    uint64_t callCount = 1;             ///< (AR) عدد الاستدعاءات / (EN) Call count
    size_t depth = 0;                   ///< (AR) العمق في الشجرة / (EN) Depth in tree
    CallTreeNode* parent = nullptr;     ///< (AR) العقدة الأب / (EN) Parent node
    std::vector<std::unique_ptr<CallTreeNode>> children;  ///< (AR) العقد الأبناء / (EN) Child nodes
    
    /// (AR) البحث عن ابن بنفس الاسم / (EN) Find child with same name
    CallTreeNode* findChild(const std::string& name) {
        for (auto& child : children) {
            if (child->functionName == name) return child.get();
        }
        return nullptr;
    }
    
    /// (AR) إضافة ابن جديد / (EN) Add new child
    CallTreeNode* addChild(const std::string& name) {
        auto child = std::make_unique<CallTreeNode>();
        child->functionName = name;
        child->depth = depth + 1;
        child->parent = this;
        auto* ptr = child.get();
        children.push_back(std::move(child));
        return ptr;
    }
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) لقطة ذاكرة في لحظة معينة
// (EN) Memory snapshot at a specific moment
// ═══════════════════════════════════════════════════════════════════════════════
struct MemorySnapshot {
    int64_t timestampNs = 0;            ///< (AR) الطابع الزمني / (EN) Timestamp
    size_t variableCount = 0;           ///< (AR) عدد المتغيرات / (EN) Variable count
    size_t scopeDepth = 0;             ///< (AR) عمق النطاق / (EN) Scope depth
    size_t objectCount = 0;            ///< (AR) عدد الكائنات / (EN) Object count
    size_t arrayCount = 0;             ///< (AR) عدد المصفوفات / (EN) Array count
    std::string context;               ///< (AR) السياق (اسم الدالة) / (EN) Context (function name)
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) معلومات نقطة اختناق
// (EN) Hot spot information
// ═══════════════════════════════════════════════════════════════════════════════
struct HotSpotInfo {
    std::string functionName;           ///< (AR) اسم الدالة / (EN) Function name
    double percentOfTotal = 0.0;        ///< (AR) النسبة من الإجمالي / (EN) Percentage of total
    std::string reason;                 ///< (AR) سبب التصنيف / (EN) Classification reason
    int severity = 0;                   ///< (AR) الشدة (1-5) / (EN) Severity (1-5)
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) سجل دخول دالة في المكدس
// (EN) Function entry record on stack
// ═══════════════════════════════════════════════════════════════════════════════
struct ActiveCall {
    std::string functionName;           ///< (AR) اسم الدالة / (EN) Function name
    TimePoint startTime;                ///< (AR) وقت البدء / (EN) Start time
    int64_t childrenTimeNs = 0;         ///< (AR) زمن الدوال الفرعية / (EN) Children time
    CallTreeNode* treeNode = nullptr;   ///< (AR) عقدة الشجرة / (EN) Tree node
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) نواة مصحح الأداء — المحرك الرئيسي
// (EN) Profiler Core — Main Engine
// ═══════════════════════════════════════════════════════════════════════════════
/**
 * @brief (AR) نواة مصحح الأداء — يجمع ويحلل بيانات الأداء أثناء التنفيذ
 * @brief (EN) Profiler Core — collects and analyzes performance data at runtime
 * 
 * الاستخدام:
 * @code{.cpp}
 * ProfilerCore profiler;
 * profiler.start();
 * 
 * // أثناء التنفيذ:
 * profiler.onFunctionEnter("جمع");
 * // ... تنفيذ الدالة ...
 * profiler.onFunctionExit("جمع");
 * 
 * profiler.stop();
 * profiler.generateReport(ReportFormat::TEXT);
 * @endcode
 */
class ProfilerCore {
public:
    // ═══════════════════════════════════════════════════════
    // (AR) البناء والتدمير / (EN) Construction & Destruction
    // ═══════════════════════════════════════════════════════
    
    explicit ProfilerCore(const ProfilerOptions& options = ProfilerOptions());
    ~ProfilerCore() = default;
    
    // (AR) منع النسخ / (EN) Prevent copying
    ProfilerCore(const ProfilerCore&) = delete;
    ProfilerCore& operator=(const ProfilerCore&) = delete;
    
    // ═══════════════════════════════════════════════════════
    // (AR) التحكم بالمصحح / (EN) Profiler Control
    // ═══════════════════════════════════════════════════════
    
    /// (AR) بدء جمع البيانات / (EN) Start data collection
    void start();
    
    /// (AR) إيقاف جمع البيانات / (EN) Stop data collection
    void stop();
    
    /// (AR) إعادة تعيين البيانات / (EN) Reset data
    void reset();
    
    /// (AR) هل المصحح نشط؟ / (EN) Is profiler active?
    bool isActive() const { return active_; }
    
    // ═══════════════════════════════════════════════════════
    // (AR) خطافات التنفيذ — تُستدعى من المفسر
    // (EN) Execution Hooks — called from interpreter
    // ═══════════════════════════════════════════════════════
    
    /**
     * @brief (AR) يُستدعى عند دخول دالة
     * @brief (EN) Called when entering a function
     * @param name (AR) اسم الدالة / (EN) Function name
     * @param isBuiltin (AR) هل هي مدمجة؟ / (EN) Is it built-in?
     * @param fileName (AR) اسم الملف / (EN) File name
     * @param line (AR) رقم السطر / (EN) Line number
     */
    void onFunctionEnter(const std::string& name, bool isBuiltin = false,
                         const std::string& fileName = "", size_t line = 0);
    
    /**
     * @brief (AR) يُستدعى عند خروج من دالة
     * @brief (EN) Called when exiting a function
     * @param name (AR) اسم الدالة / (EN) Function name
     */
    void onFunctionExit(const std::string& name);
    
    /**
     * @brief (AR) يُستدعى عند إنشاء متغير
     * @brief (EN) Called when a variable is created
     */
    void onVariableCreated(const std::string& name, const std::string& scope);
    
    /**
     * @brief (AR) يُستدعى عند دخول/خروج نطاق
     * @brief (EN) Called on scope enter/exit
     */
    void onScopeEnter(const std::string& name, size_t depth);
    void onScopeExit(const std::string& name, size_t depth);
    
    /**
     * @brief (AR) يُستدعى عند إنشاء كائن
     * @brief (EN) Called when an object is created
     */
    void onObjectCreated(const std::string& className);
    
    // ═══════════════════════════════════════════════════════
    // (AR) إنتاج التقارير / (EN) Report Generation
    // ═══════════════════════════════════════════════════════
    
    /// (AR) إنتاج تقرير نصي / (EN) Generate text report
    std::string generateTextReport() const;
    
    /// (AR) إنتاج تقرير JSON / (EN) Generate JSON report
    std::string generateJSONReport() const;
    
    /// (AR) إنتاج تقرير HTML / (EN) Generate HTML report
    std::string generateHTMLReport() const;
    
    /// (AR) إنتاج بيانات مخطط اللهب / (EN) Generate flame graph data
    std::string generateFlameGraphData() const;
    
    /// (AR) إنتاج تقرير CSV / (EN) Generate CSV report
    std::string generateCSVReport() const;
    
    /// (AR) إنتاج تقرير بالصيغة المحددة / (EN) Generate report in specified format
    std::string generateReport(ReportFormat format) const;
    
    /// (AR) حفظ التقرير في ملف / (EN) Save report to file
    bool saveReport(const std::string& filePath, ReportFormat format) const;
    
    /// (AR) إنتاج ملخص مختصر (سطر واحد) / (EN) Generate one-line summary
    std::string generateOneLinerSummary() const;
    
    /// (AR) إنتاج تقرير تفصيلي لدالة محددة / (EN) Generate detailed function report
    std::string generateFunctionDetail(const std::string& funcName) const;
    
    /// (AR) إنتاج تقرير مقارنة بين دالتين / (EN) Generate comparison report between two functions
    std::string generateComparisonReport(const std::string& func1, const std::string& func2) const;
    
    /// (AR) إنتاج تقرير التوزيع الزمني / (EN) Generate time distribution report
    std::string generateTimeDistributionReport() const;
    
    /// (AR) إنتاج تقرير دوال ساخنة/باردة / (EN) Generate hot/cold functions report
    std::string generateHotColdReport() const;
    
    /// (AR) الحصول على نسبة زمن الدوال المدمجة / (EN) Get builtin time percentage
    double getBuiltinTimePercent() const;
    
    /// (AR) الحصول على نسبة زمن دوال المستخدم / (EN) Get user time percentage
    double getUserTimePercent() const;
    
    // ═══════════════════════════════════════════════════════
    // (AR) استعلامات البيانات / (EN) Data Queries
    // ═══════════════════════════════════════════════════════
    
    /// (AR) الحصول على بيانات دالة محددة / (EN) Get specific function data
    const FunctionProfileData* getFunctionData(const std::string& name) const;
    
    /// (AR) الحصول على الدوال الأبطأ / (EN) Get slowest functions
    std::vector<const FunctionProfileData*> getTopFunctions(size_t count, bool byExclusive = true) const;
    
    /// (AR) الحصول على نقاط الاختناق / (EN) Get hot spots
    std::vector<HotSpotInfo> getHotSpots() const;
    
    /// (AR) الحصول على شجرة الاستدعاءات / (EN) Get call tree
    const CallTreeNode* getCallTree() const { return callTreeRoot_.get(); }
    
    /// (AR) الحصول على لقطات الذاكرة / (EN) Get memory snapshots
    const std::vector<MemorySnapshot>& getMemorySnapshots() const { return memorySnapshots_; }
    
    /// (AR) الزمن الكلي / (EN) Total time
    int64_t getTotalTimeNs() const { return totalTimeNs_; }
    
    /// (AR) إجمالي استدعاءات الدوال / (EN) Total function calls
    uint64_t getTotalFunctionCalls() const { return totalFunctionCalls_; }
    
    /// (AR) أقصى عمق استدعاء / (EN) Maximum call depth
    size_t getMaxCallDepth() const { return maxCallDepth_; }
    
    /// (AR) عدد الدوال المُراقبة / (EN) Number of profiled functions
    size_t getProfiledFunctionCount() const { return functionData_.size(); }
    
    /// (AR) عدد الدوال الساخنة / (EN) Hot function count
    size_t getHotFunctionCount() const;
    
    /// (AR) عدد الدوال الباردة / (EN) Cold function count
    size_t getColdFunctionCount() const;
    
    /// (AR) متوسط زمن الاستدعاء للبرنامج / (EN) Average call time for program
    double getAverageCallTimeNs() const;
    
    /// (AR) جلب جميع الدوال المرتبة / (EN) Get all sorted functions
    std::vector<const FunctionProfileData*> getAllFunctionsSorted(bool byExclusive = true) const;
    
    /// (AR) الحصول على الخيارات / (EN) Get options
    const ProfilerOptions& getOptions() const { return options_; }

private:
    // ─── (AR) الحالة / (EN) State ───
    ProfilerOptions options_;
    std::atomic<bool> active_{false};
    TimePoint programStartTime_;
    TimePoint programEndTime_;
    int64_t totalTimeNs_ = 0;
    
    // ─── (AR) بيانات الدوال / (EN) Function data ───
    std::unordered_map<std::string, FunctionProfileData> functionData_;
    
    // ─── (AR) مكدس الاستدعاءات النشطة / (EN) Active call stack ───
    std::stack<ActiveCall> callStack_;
    
    // ─── (AR) شجرة الاستدعاءات / (EN) Call tree ───
    std::unique_ptr<CallTreeNode> callTreeRoot_;
    CallTreeNode* currentTreeNode_ = nullptr;
    
    // ─── (AR) لقطات الذاكرة / (EN) Memory snapshots ───
    std::vector<MemorySnapshot> memorySnapshots_;
    
    // ─── (AR) إحصائيات عامة / (EN) General statistics ───
    std::atomic<uint64_t> totalFunctionCalls_{0};
    std::atomic<uint64_t> totalBuiltinCalls_{0};   ///< (AR) عدد استدعاءات الدوال المدمجة / (EN) Builtin call count
    std::atomic<uint64_t> totalUserCalls_{0};      ///< (AR) عدد استدعاءات الدوال المستخدم / (EN) User call count
    std::atomic<uint64_t> totalVariablesCreated_{0};
    std::atomic<uint64_t> totalObjectsCreated_{0};
    std::atomic<uint64_t> totalScopeTransitions_{0};
    std::atomic<uint64_t> stackOverflowWarnings_{0};   ///< (AR) تحذيرات فيضان المكدس / (EN) Stack overflow warnings
    size_t maxScopeDepth_ = 0;
    size_t maxCallDepth_ = 0;
    
    // ─── (AR) أخذ العينات / (EN) Sampling ───
    std::atomic<uint64_t> sampleCounter_{0};       ///< (AR) عداد العينات / (EN) Sample counter
    
    // ─── (AR) مزامنة / (EN) Synchronization ───
    mutable std::mutex dataMutex_;
    
    // ─── (AR) دوال مساعدة / (EN) Helper functions ───
    
    /// (AR) الحصول على أو إنشاء بيانات دالة / (EN) Get or create function data
    FunctionProfileData& getOrCreateFunctionData(const std::string& name);
    
    /// (AR) تنسيق الزمن للعرض / (EN) Format time for display
    static std::string formatTime(int64_t ns);
    
    /// (AR) تنسيق النسبة المئوية / (EN) Format percentage
    static std::string formatPercent(double value);
    
    /// (AR) رسم شريط تقدم نصي / (EN) Draw text progress bar
    static std::string drawBar(double percent, int width = 30);
    
    /// (AR) بناء شجرة الاستدعاءات كنص / (EN) Build call tree as text
    void buildCallTreeText(const CallTreeNode* node, std::ostringstream& out, 
                           const std::string& prefix = "", bool isLast = true) const;
    
    /// (AR) بناء شجرة الاستدعاءات كـ JSON / (EN) Build call tree as JSON
    void buildCallTreeJSON(const CallTreeNode* node, std::ostringstream& out,
                           int indent = 0) const;
    
    /// (AR) بناء بيانات FlameGraph / (EN) Build FlameGraph data
    void buildFlameGraphData(const CallTreeNode* node, std::ostringstream& out,
                             std::vector<std::string>& stack) const;
    
    /// (AR) تكرار مسافة بادئة / (EN) Repeat indent string
    static std::string indent(int level);
    
    /// (AR) هروب نص JSON / (EN) Escape JSON string
    static std::string escapeJSON(const std::string& str);
    
    /// (AR) هروب نص HTML / (EN) Escape HTML string
    static std::string escapeHTML(const std::string& str);
};

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) نقطة وصول عامة للمصحح (Singleton اختياري)
// (EN) Global profiler access point (optional Singleton)
// ═══════════════════════════════════════════════════════════════════════════════

/// (AR) الحصول على المصحح العام / (EN) Get global profiler
ProfilerCore* getGlobalProfiler();

/// (AR) تعيين المصحح العام / (EN) Set global profiler
void setGlobalProfiler(ProfilerCore* profiler);

} // namespace Tools
} // namespace Sad
