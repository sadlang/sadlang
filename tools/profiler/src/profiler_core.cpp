/**
 * @file profiler_core.cpp
 * @brief (AR) تنفيذ نواة مصحح الأداء — المحرك الأساسي لتحليل أداء برامج لغة ص
 * @brief (EN) Profiler Core Implementation — Main engine for Sad performance analysis
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 * الوصف بالعربية:
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على التنفيذ الكامل لنواة مصحح الأداء، ويشمل:
 * 
 * 1. آلية تسجيل الدخول والخروج من الدوال:
 *    - عند دخول دالة: يُسجل الزمن الحالي ويُخزن في المكدس
 *    - عند الخروج: يحسب الفارق الزمني ويُحدث الإحصائيات
 *    - الزمن الحصري = الزمن الشامل - زمن الدوال الفرعية
 * 
 * 2. آلية بناء شجرة الاستدعاءات:
 *    - كل دخول لدالة يُنشئ عقدة جديدة في الشجرة
 *    - العقدة الحالية تتحرك للأسفل عند الدخول وللأعلى عند الخروج
 *    - إذا كانت دالة مُستدعاة مسبقاً من نفس الأب، تُدمج معها
 * 
 * 3. إنتاج التقارير بأربع صيغ:
 *    - TEXT: تقرير نصي ملون بالعربية (للتيرمنال)
 *    - JSON: لتحليل البيانات بأدوات خارجية
 *    - HTML: تقرير تفاعلي يمكن فتحه في المتصفح
 *    - FlameGraph: بيانات يمكن تحويلها لمخطط لهب
 * 
 * @author فريق تطوير لغة ص
 * @date 2025
 */

#include "profiler_core.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <cassert>

namespace Sad {
namespace Tools {

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) المؤشر العام للمصحح
// (EN) Global profiler pointer
// ═══════════════════════════════════════════════════════════════════════════════
static ProfilerCore* g_globalProfiler = nullptr;

ProfilerCore* getGlobalProfiler() {
    return g_globalProfiler;
}

void setGlobalProfiler(ProfilerCore* profiler) {
    g_globalProfiler = profiler;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) البناء
// (EN) Constructor
// ═══════════════════════════════════════════════════════════════════════════════

ProfilerCore::ProfilerCore(const ProfilerOptions& options)
    : options_(options)
{
    // (AR) تهيئة جذر شجرة الاستدعاءات
    // (EN) Initialize call tree root
    callTreeRoot_ = std::make_unique<CallTreeNode>();
    callTreeRoot_->functionName = "<البرنامج>";  // (AR) الجذر / (EN) Root
    currentTreeNode_ = callTreeRoot_.get();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) التحكم بالمصحح
// (EN) Profiler Control
// ═══════════════════════════════════════════════════════════════════════════════

void ProfilerCore::start() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    active_ = true;
    programStartTime_ = Clock::now();
    
    // (AR) تسجيل دخول البرنامج في شجرة الاستدعاء
    // (EN) Record program entry in call tree
    currentTreeNode_ = callTreeRoot_.get();
}

void ProfilerCore::stop() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    if (!active_) return;
    
    programEndTime_ = Clock::now();
    totalTimeNs_ = std::chrono::duration_cast<Duration>(
        programEndTime_ - programStartTime_).count();
    active_ = false;
    
    // (AR) إفراغ مكدس الاستدعاءات المتبقية (في حالة خروج غير طبيعي)
    // (EN) Flush remaining call stack (in case of abnormal exit)
    while (!callStack_.empty()) {
        auto& top = callStack_.top();
        auto elapsed = std::chrono::duration_cast<Duration>(
            programEndTime_ - top.startTime).count();
        
        auto& data = getOrCreateFunctionData(top.functionName);
        data.totalInclusiveNs += elapsed;
        data.totalExclusiveNs += (elapsed - top.childrenTimeNs);
        
        callStack_.pop();
    }
}

void ProfilerCore::reset() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    active_ = false;
    functionData_.clear();
    while (!callStack_.empty()) callStack_.pop();
    callTreeRoot_ = std::make_unique<CallTreeNode>();
    callTreeRoot_->functionName = "<البرنامج>";
    currentTreeNode_ = callTreeRoot_.get();
    memorySnapshots_.clear();
    totalFunctionCalls_.store(0, std::memory_order_relaxed);
    totalBuiltinCalls_.store(0, std::memory_order_relaxed);
    totalUserCalls_.store(0, std::memory_order_relaxed);
    totalVariablesCreated_.store(0, std::memory_order_relaxed);
    totalObjectsCreated_.store(0, std::memory_order_relaxed);
    totalScopeTransitions_.store(0, std::memory_order_relaxed);
    maxScopeDepth_ = 0;
    maxCallDepth_ = 0;
    stackOverflowWarnings_.store(0, std::memory_order_relaxed);
    sampleCounter_.store(0, std::memory_order_relaxed);
    totalTimeNs_ = 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) خطافات التنفيذ
// (EN) Execution Hooks
// ═══════════════════════════════════════════════════════════════════════════════

void ProfilerCore::onFunctionEnter(const std::string& name, bool isBuiltin,
                                    const std::string& fileName, size_t line) {
    if (!active_.load(std::memory_order_relaxed)) return;
    
    // (AR) حماية من اسم فارغ / (EN) Guard against empty name
    if (name.empty()) return;
    
    // (AR) عدادات ذرية بلا قفل / (EN) Lock-free atomic counters
    totalFunctionCalls_.fetch_add(1, std::memory_order_relaxed);
    if (isBuiltin) {
        totalBuiltinCalls_.fetch_add(1, std::memory_order_relaxed);
    } else {
        totalUserCalls_.fetch_add(1, std::memory_order_relaxed);
    }
    
    // (AR) أخذ العينات — إذا كان معدل أخذ العينات > 1، نتخطى بعض الاستدعاءات
    // (EN) Sampling — if rate > 1, skip some calls
    if (options_.samplingRate > 1) {
        uint64_t counter = sampleCounter_.fetch_add(1, std::memory_order_relaxed);
        if (counter % options_.samplingRate != 0) return;
    }
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // (AR) حماية من فيضان المكدس / (EN) Stack overflow protection
    if (callStack_.size() >= options_.maxCallDepth) {
        stackOverflowWarnings_.fetch_add(1, std::memory_order_relaxed);
        return;  // (AR) تجاهل الاستدعاءات العميقة جداً / (EN) Skip too-deep calls
    }
    
    // (AR) تحديث بيانات الدالة
    // (EN) Update function data
    auto& data = getOrCreateFunctionData(name);
    data.callCount++;
    data.isBuiltin = isBuiltin;
    if (!fileName.empty()) data.fileName = fileName;
    if (line > 0) data.line = line;
    
    // (AR) تتبع المُستدعي (caller)
    // (EN) Track caller
    if (!callStack_.empty()) {
        const auto& caller = callStack_.top();
        data.callers[caller.functionName]++;
        
        auto& callerData = getOrCreateFunctionData(caller.functionName);
        callerData.callees[name]++;
        
        // (AR) فحص الاستدعاء التكراري
        // (EN) Check for recursion
        if (caller.functionName == name) {
            data.recursiveCallCount++;
        }
    }
    
    // (AR) تحديث شجرة الاستدعاءات
    // (EN) Update call tree
    CallTreeNode* treeNode = nullptr;
    if (options_.trackCallTree && currentTreeNode_) {
        // (AR) البحث عن ابن موجود بنفس الاسم لدمج الإحصائيات
        // (EN) Look for existing child with same name to merge stats
        treeNode = currentTreeNode_->findChild(name);
        if (treeNode) {
            treeNode->callCount++;
        } else {
            treeNode = currentTreeNode_->addChild(name);
        }
        currentTreeNode_ = treeNode;
    }
    
    // (AR) تسجيل الدخول في المكدس
    // (EN) Record entry on stack
    ActiveCall call;
    call.functionName = name;
    call.startTime = Clock::now();
    call.childrenTimeNs = 0;
    call.treeNode = treeNode;
    callStack_.push(std::move(call));
    
    // (AR) تحديث أقصى عمق
    // (EN) Update max depth
    if (callStack_.size() > maxCallDepth_) {
        maxCallDepth_ = callStack_.size();
    }
}

void ProfilerCore::onFunctionExit(const std::string& name) {
    if (!active_.load(std::memory_order_relaxed)) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (callStack_.empty()) return;
    
    auto& top = callStack_.top();
    
    // (AR) فحص تطابق الاسم (حماية ضد عدم التوازن)
    // (EN) Check name match (protection against imbalance)
    if (top.functionName != name) {
        // (AR) البحث في المكدس عن الدالة (قد تكون هناك دوال فقدت خروجها)
        // (EN) Search stack for the function (some exits may have been missed)
        // (AR) لا نُعدّل المكدس — نتجاهل مطابقة خاطئة
        // (EN) Don't modify stack — ignore bad match
        return;
    }
    
    auto now = Clock::now();
    auto elapsedNs = std::chrono::duration_cast<Duration>(now - top.startTime).count();
    
    // (AR) تحديث بيانات الدالة
    // (EN) Update function data
    auto& data = getOrCreateFunctionData(name);
    data.totalInclusiveNs += elapsedNs;
    int64_t exclusiveNs = std::max(int64_t(0), elapsedNs - top.childrenTimeNs);
    data.totalExclusiveNs += exclusiveNs;
    
    // (AR) تحديث الحد الأدنى والأقصى
    // (EN) Update min/max
    if (elapsedNs < data.minInclusiveNs) data.minInclusiveNs = elapsedNs;
    if (elapsedNs > data.maxInclusiveNs) data.maxInclusiveNs = elapsedNs;
    
    // (AR) تحديث شجرة الاستدعاءات
    // (EN) Update call tree
    if (top.treeNode) {
        top.treeNode->inclusiveNs += elapsedNs;
        top.treeNode->exclusiveNs += exclusiveNs;
    }
    
    callStack_.pop();
    
    // (AR) إضافة الزمن لأب المكدس (لحساب childrenTime)
    // (EN) Add time to stack parent (for childrenTime calculation)
    if (!callStack_.empty()) {
        callStack_.top().childrenTimeNs += elapsedNs;
    }
    
    // (AR) العودة للعقدة الأب في شجرة الاستدعاءات
    // (EN) Return to parent node in call tree
    if (options_.trackCallTree && currentTreeNode_ && currentTreeNode_->parent) {
        currentTreeNode_ = currentTreeNode_->parent;
    }
}

void ProfilerCore::onVariableCreated(const std::string& name, const std::string& scope) {
    if (!active_.load(std::memory_order_relaxed) || !options_.trackMemory) return;
    totalVariablesCreated_.fetch_add(1, std::memory_order_relaxed);
}

void ProfilerCore::onScopeEnter(const std::string& name, size_t depth) {
    if (!active_.load(std::memory_order_relaxed)) return;
    totalScopeTransitions_.fetch_add(1, std::memory_order_relaxed);
    // (AR) maxScopeDepth_ يحتاج قفل لأنه ليس ذري / (EN) maxScopeDepth_ needs lock (non-atomic)
    if (depth > maxScopeDepth_) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        if (depth > maxScopeDepth_) maxScopeDepth_ = depth;
    }
}

void ProfilerCore::onScopeExit(const std::string& name, size_t depth) {
    if (!active_.load(std::memory_order_relaxed)) return;
    totalScopeTransitions_.fetch_add(1, std::memory_order_relaxed);
}

void ProfilerCore::onObjectCreated(const std::string& className) {
    if (!active_.load(std::memory_order_relaxed) || !options_.trackMemory) return;
    totalObjectsCreated_.fetch_add(1, std::memory_order_relaxed);
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) دوال مساعدة
// (EN) Helper Functions
// ═══════════════════════════════════════════════════════════════════════════════

FunctionProfileData& ProfilerCore::getOrCreateFunctionData(const std::string& name) {
    auto it = functionData_.find(name);
    if (it == functionData_.end()) {
        // (AR) حماية من تسرب الذاكرة / (EN) Memory leak guard
        if (functionData_.size() >= options_.maxTrackedFunctions) {
            // (AR) ارجع أول عنصر بدلاً من إنشاء جديد / (EN) Return first element instead of creating new
            return functionData_.begin()->second;
        }
        FunctionProfileData data;
        data.name = name;
        auto result = functionData_.emplace(name, std::move(data));
        return result.first->second;
    }
    return it->second;
}

std::string ProfilerCore::formatTime(int64_t ns) {
    if (ns < 1'000) {
        return std::to_string(ns) + " نانو";
    } else if (ns < 1'000'000) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << (ns / 1'000.0) << " مكرو";
        return oss.str();
    } else if (ns < 1'000'000'000) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << (ns / 1'000'000.0) << " مللي";
        return oss.str();
    } else {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << (ns / 1'000'000'000.0) << " ثانية";
        return oss.str();
    }
}

std::string ProfilerCore::formatPercent(double value) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << value << "%";
    return oss.str();
}

std::string ProfilerCore::drawBar(double percent, int width) {
    int filled = static_cast<int>(std::round(percent / 100.0 * width));
    if (filled < 0) filled = 0;
    if (filled > width) filled = width;
    std::string bar;
    bar.reserve(width + 2);
    bar += "[";
    for (int i = 0; i < width; ++i) {
        bar += (i < filled) ? "█" : "░";
    }
    bar += "]";
    return bar;
}

std::string ProfilerCore::indent(int level) {
    return std::string(level * 2, ' ');
}

std::string ProfilerCore::escapeJSON(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 10);
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                    result += buf;
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string ProfilerCore::escapeHTML(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 20);
    for (char c : str) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) استعلامات البيانات
// (EN) Data Queries
// ═══════════════════════════════════════════════════════════════════════════════

const FunctionProfileData* ProfilerCore::getFunctionData(const std::string& name) const {
    auto it = functionData_.find(name);
    return (it != functionData_.end()) ? &it->second : nullptr;
}

std::vector<const FunctionProfileData*> ProfilerCore::getTopFunctions(
    size_t count, bool byExclusive) const
{
    std::vector<const FunctionProfileData*> result;
    result.reserve(functionData_.size());
    
    for (const auto& [name, data] : functionData_) {
        // (AR) استبعاد الدوال المدمجة حسب الخيارات / (EN) Exclude builtins if configured
        if (options_.excludeBuiltins && data.isBuiltin) continue;
        
        // (AR) تصفية بنمط الاسم / (EN) Filter by name pattern
        if (!options_.filterPattern.empty()) {
            if (name.find(options_.filterPattern) == std::string::npos) continue;
        }
        
        double timeMs = FunctionProfileData::nsToMs(
            byExclusive ? data.totalExclusiveNs : data.totalInclusiveNs);
        if (timeMs >= options_.thresholdMs) {
            result.push_back(&data);
        }
    }
    
    // (AR) ترتيب تنازلي حسب الزمن / (EN) Sort descending by time
    std::sort(result.begin(), result.end(),
        [byExclusive](const FunctionProfileData* a, const FunctionProfileData* b) {
            return byExclusive 
                ? (a->totalExclusiveNs > b->totalExclusiveNs)
                : (a->totalInclusiveNs > b->totalInclusiveNs);
        });
    
    if (result.size() > count) {
        result.resize(count);
    }
    
    return result;
}

std::vector<HotSpotInfo> ProfilerCore::getHotSpots() const {
    std::vector<HotSpotInfo> spots;
    
    if (totalTimeNs_ <= 0) return spots;
    
    for (const auto& [name, data] : functionData_) {
        double percent = (static_cast<double>(data.totalExclusiveNs) / totalTimeNs_) * 100.0;
        
        HotSpotInfo spot;
        spot.functionName = name;
        spot.percentOfTotal = percent;
        
        // (AR) تصنيف الشدة / (EN) Classify severity
        if (percent > 50.0) {
            spot.severity = 5;
            spot.reason = "تستهلك أكثر من 50% من الزمن الكلي!";
        } else if (percent > 25.0) {
            spot.severity = 4;
            spot.reason = "تستهلك أكثر من 25% من الزمن الكلي";
        } else if (percent > 10.0) {
            spot.severity = 3;
            spot.reason = "تستهلك أكثر من 10% من الزمن الكلي";
        } else if (data.callCount > 10000) {
            spot.severity = 3;
            spot.reason = "عدد استدعاءات مرتفع جداً (" + std::to_string(data.callCount) + ")";
        } else if (data.recursiveCallCount > 100) {
            spot.severity = 2;
            spot.reason = "استدعاء تكراري مكثف (" + std::to_string(data.recursiveCallCount) + " مرة)";
        } else if (data.varianceRatio() > 100.0 && data.callCount > 10) {
            spot.severity = 2;
            spot.reason = "تباين زمني عالٍ (النسبة " + std::to_string(static_cast<int>(data.varianceRatio())) + ":1)";
        } else {
            continue;  // (AR) ليست نقطة اختناق / (EN) Not a hot spot
        }
        
        spots.push_back(std::move(spot));
    }
    
    // (AR) ترتيب حسب الشدة / (EN) Sort by severity
    std::sort(spots.begin(), spots.end(),
        [](const HotSpotInfo& a, const HotSpotInfo& b) {
            return a.severity > b.severity || 
                   (a.severity == b.severity && a.percentOfTotal > b.percentOfTotal);
        });
    
    return spots;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) إنتاج التقرير النصي
// (EN) Text Report Generation
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateTextReport() const {
    std::ostringstream out;
    bool color = options_.colorOutput;
    
    // (AR) ترميز الألوان ANSI / (EN) ANSI color codes
    auto bold   = [&]() -> std::string { return color ? "\033[1m" : ""; };
    auto reset  = [&]() -> std::string { return color ? "\033[0m" : ""; };
    auto red    = [&]() -> std::string { return color ? "\033[91m" : ""; };
    auto green  = [&]() -> std::string { return color ? "\033[92m" : ""; };
    auto yellow = [&]() -> std::string { return color ? "\033[93m" : ""; };
    auto blue   = [&]() -> std::string { return color ? "\033[94m" : ""; };
    auto cyan   = [&]() -> std::string { return color ? "\033[96m" : ""; };
    auto dim    = [&]() -> std::string { return color ? "\033[2m" : ""; };
    
    // ─── العنوان / Header ───
    out << "\n";
    out << bold() << cyan()
        << "═══════════════════════════════════════════════════════════════════\n"
        << "   📊 تقرير مصحح الأداء — لغة ص (Sad Profiler Report)\n"
        << "═══════════════════════════════════════════════════════════════════"
        << reset() << "\n";
    
    // (AR) طابع زمني / (EN) Timestamp
    if (options_.addTimestamp) {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        struct tm timeinfo;
#ifdef _WIN32
        localtime_s(&timeinfo, &time_t);
#else
        localtime_r(&time_t, &timeinfo);
#endif
        char timeBuf[64];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        out << dim() << "   🕐 " << timeBuf << reset() << "\n";
    }
    out << "\n";
    
    // ─── الملخص العام / Summary ───
    out << bold() << "📋 الملخص العام:" << reset() << "\n";
    out << "   ⏱ الزمن الكلي للتنفيذ:     " << green() << formatTime(totalTimeNs_) << reset() << "\n";
    out << "   📞 إجمالي استدعاءات الدوال: " << std::to_string(totalFunctionCalls_) << "\n";
    out << "   │  ├─ دوال مستخدم:         " << std::to_string(totalUserCalls_) << "\n";
    out << "   │  └─ دوال مدمجة:          " << std::to_string(totalBuiltinCalls_) << "\n";
    out << "   📦 عدد الدوال المُراقبة:    " << std::to_string(functionData_.size()) << "\n";
    out << "   📊 أقصى عمق استدعاء:        " << std::to_string(maxCallDepth_) << "\n";
    
    if (stackOverflowWarnings_ > 0) {
        out << "   " << red() << "⚠ تحذيرات فيضان المكدس:  " << std::to_string(stackOverflowWarnings_) << reset() << "\n";
    }
    
    // (AR) نسبة الزمن المدمجة/المستخدم / (EN) Builtin/User time ratio
    double builtinPct = getBuiltinTimePercent();
    double userPct = getUserTimePercent();
    if (totalFunctionCalls_ > 0) {
        out << "   📈 زمن المستخدم:            " << formatPercent(userPct) << "\n";
        out << "   📈 زمن المدمجة:             " << formatPercent(builtinPct) << "\n";
    }
    
    if (options_.trackMemory) {
        out << "   💾 متغيرات مُنشأة:          " << std::to_string(totalVariablesCreated_) << "\n";
        out << "   🧱 كائنات مُنشأة:           " << std::to_string(totalObjectsCreated_) << "\n";
        out << "   🔄 انتقالات النطاق:         " << std::to_string(totalScopeTransitions_) << "\n";
        out << "   📏 أقصى عمق نطاق:           " << std::to_string(maxScopeDepth_) << "\n";
    }
    out << "\n";
    
    // ─── الدوال الأبطأ (حصري) / Slowest Functions (Exclusive) ───
    auto topExclusive = getTopFunctions(options_.topFunctionsCount, true);
    if (!topExclusive.empty()) {
        out << bold() << "🔥 الدوال الأبطأ (حسب الزمن الحصري):" << reset() << "\n";
        out << dim() << "   الزمن الحصري = زمن الدالة بدون الدوال الفرعية" << reset() << "\n\n";
        
        // (AR) رأس الجدول / (EN) Table header
        out << dim()
            << "   # │ اسم الدالة                    │ استدعاءات │ حصري        │ شامل        │ %    │ شريط"
            << reset() << "\n";
        out << dim()
            << "   ──┼───────────────────────────────┼──────────┼─────────────┼─────────────┼──────┼──────────────────────────────"
            << reset() << "\n";
        
        for (size_t i = 0; i < topExclusive.size(); ++i) {
            const auto* f = topExclusive[i];
            double percent = (totalTimeNs_ > 0)
                ? (static_cast<double>(f->totalExclusiveNs) / totalTimeNs_) * 100.0 : 0.0;
            
            // (AR) تلوين حسب الشدة / (EN) Color by severity
            std::string nameColor;
            if (percent > 25.0) nameColor = red();
            else if (percent > 10.0) nameColor = yellow();
            else nameColor = "";
            
            // (AR) قص الاسم إذا كان طويلاً / (EN) Truncate long names
            std::string displayName = f->name;
            if (displayName.length() > 28) {
                displayName = displayName.substr(0, 25) + "...";
            }
            
            out << "   " << std::setw(2) << (i + 1) << "│ "
                << nameColor << std::left << std::setw(30) << displayName << reset() << "│ "
                << std::right << std::setw(8) << f->callCount << " │ "
                << std::setw(11) << formatTime(f->totalExclusiveNs) << " │ "
                << std::setw(11) << formatTime(f->totalInclusiveNs) << " │ "
                << std::setw(5) << formatPercent(percent) << "│ ";
            if (options_.showPercentBar) {
                out << drawBar(percent, 20);
            }
            out << "\n";
        }
        out << "\n";
    }
    
    // ─── نقاط الاختناق / Hot Spots ───
    auto hotSpots = getHotSpots();
    if (!hotSpots.empty()) {
        out << bold() << red() << "⚠️  نقاط الاختناق المكتشفة:" << reset() << "\n\n";
        
        for (const auto& spot : hotSpots) {
            std::string severityIcon;
            std::string severityColor;
            switch (spot.severity) {
                case 5: severityIcon = "🔴"; severityColor = red(); break;
                case 4: severityIcon = "🟠"; severityColor = yellow(); break;
                case 3: severityIcon = "🟡"; severityColor = yellow(); break;
                default: severityIcon = "🟢"; severityColor = green(); break;
            }
            
            out << "   " << severityIcon << " " << severityColor << bold()
                << spot.functionName << reset() << " — "
                << formatPercent(spot.percentOfTotal) << " — " 
                << spot.reason << "\n";
        }
        out << "\n";
    }
    
    // ─── شجرة الاستدعاءات / Call Tree ───
    if (options_.trackCallTree && callTreeRoot_ && !callTreeRoot_->children.empty()) {
        out << bold() << "🌳 شجرة الاستدعاءات:" << reset() << "\n\n";
        buildCallTreeText(callTreeRoot_.get(), out, "   ");
        out << "\n";
    }
    
    // ─── التذييل / Footer ───
    out << dim() 
        << "───────────────────────────────────────────────────────────────────\n"
        << "   تم إنتاج هذا التقرير بواسطة مصحح أداء لغة ص (sad-profiler)\n"
        << "───────────────────────────────────────────────────────────────────"
        << reset() << "\n";
    
    return out.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) بناء شجرة الاستدعاءات كنص
// (EN) Build call tree as text
// ═══════════════════════════════════════════════════════════════════════════════

void ProfilerCore::buildCallTreeText(const CallTreeNode* node, std::ostringstream& out,
                                      const std::string& prefix, bool isLast) const {
    if (!node) return;
    
    // (AR) تحديد عمق العرض / (EN) Limit display depth
    if (node->depth > options_.maxTreeDepth) {
        if (node->depth == options_.maxTreeDepth + 1) {
            out << prefix << "└── ... (" << node->children.size() << " فرع مخفي)\n";
        }
        return;
    }
    
    // (AR) رسم الشجرة / (EN) Draw tree
    std::string connector = isLast ? "└── " : "├── ";
    std::string childPrefix = prefix + (isLast ? "    " : "│   ");
    
    if (node != callTreeRoot_.get()) {
        out << prefix << connector;
    }
    
    // (AR) اسم الدالة مع الإحصائيات / (EN) Function name with stats
    out << node->functionName;
    if (node->callCount > 1) {
        out << " (×" << node->callCount << ")";
    }
    if (node->inclusiveNs > 0) {
        out << " [" << formatTime(node->inclusiveNs) << "]";
    }
    out << "\n";
    
    // (AR) الأبناء / (EN) Children
    for (size_t i = 0; i < node->children.size(); ++i) {
        bool last = (i == node->children.size() - 1);
        buildCallTreeText(node->children[i].get(), out,
                          node == callTreeRoot_.get() ? prefix : childPrefix, last);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) إنتاج تقرير JSON
// (EN) JSON Report Generation
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateJSONReport() const {
    std::ostringstream out;
    
    out << "{\n";
    out << "  \"profiler\": \"sad-profiler\",\n";
    out << "  \"version\": \"1.2.0\",\n";
    
    // (AR) طابع زمني / (EN) Timestamp
    {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        struct tm timeinfo;
#ifdef _WIN32
        localtime_s(&timeinfo, &time_t);
#else
        localtime_r(&time_t, &timeinfo);
#endif
        char timeBuf[64];
        std::strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%dT%H:%M:%S", &timeinfo);
        out << "  \"timestamp\": \"" << timeBuf << "\",\n";
    }
    
    // (AR) الملخص / (EN) Summary
    out << "  \"summary\": {\n";
    out << "    \"totalTimeNs\": " << totalTimeNs_ << ",\n";
    out << "    \"totalTimeMs\": " << std::fixed << std::setprecision(3) 
        << FunctionProfileData::nsToMs(totalTimeNs_) << ",\n";
    out << "    \"totalFunctionCalls\": " << totalFunctionCalls_ << ",\n";
    out << "    \"totalBuiltinCalls\": " << totalBuiltinCalls_ << ",\n";
    out << "    \"totalUserCalls\": " << totalUserCalls_ << ",\n";
    out << "    \"profiledFunctions\": " << functionData_.size() << ",\n";
    out << "    \"maxCallDepth\": " << maxCallDepth_ << ",\n";
    out << "    \"stackOverflowWarnings\": " << stackOverflowWarnings_ << ",\n";
    out << "    \"totalVariablesCreated\": " << totalVariablesCreated_ << ",\n";
    out << "    \"totalObjectsCreated\": " << totalObjectsCreated_ << ",\n";
    out << "    \"maxScopeDepth\": " << maxScopeDepth_ << "\n";
    out << "  },\n";
    
    // (AR) بيانات الدوال / (EN) Function data
    out << "  \"functions\": [\n";
    
    auto sorted = getTopFunctions(functionData_.size(), true);
    for (size_t i = 0; i < sorted.size(); ++i) {
        const auto* f = sorted[i];
        out << "    {\n";
        out << "      \"name\": \"" << escapeJSON(f->name) << "\",\n";
        out << "      \"fileName\": \"" << escapeJSON(f->fileName) << "\",\n";
        out << "      \"line\": " << f->line << ",\n";
        out << "      \"isBuiltin\": " << (f->isBuiltin ? "true" : "false") << ",\n";
        out << "      \"callCount\": " << f->callCount << ",\n";
        out << "      \"recursiveCallCount\": " << f->recursiveCallCount << ",\n";
        out << "      \"totalInclusiveNs\": " << f->totalInclusiveNs << ",\n";
        out << "      \"totalExclusiveNs\": " << f->totalExclusiveNs << ",\n";
        out << "      \"avgInclusiveNs\": " << std::fixed << std::setprecision(1) << f->avgInclusiveNs() << ",\n";
        out << "      \"avgExclusiveNs\": " << std::fixed << std::setprecision(1) << f->avgExclusiveNs() << ",\n";
        out << "      \"minInclusiveNs\": " << (f->minInclusiveNs == INT64_MAX ? 0 : f->minInclusiveNs) << ",\n";
        out << "      \"maxInclusiveNs\": " << f->maxInclusiveNs << ",\n";
        out << "      \"callRatePerSec\": " << std::fixed << std::setprecision(1) << f->callRatePerSecond(totalTimeNs_) << ",\n";
        out << "      \"recursivePercent\": " << std::fixed << std::setprecision(1) << f->recursivePercent() << ",\n";
        out << "      \"varianceRatio\": " << std::fixed << std::setprecision(1) << f->varianceRatio() << ",\n";
        
        double percent = (totalTimeNs_ > 0)
            ? (static_cast<double>(f->totalExclusiveNs) / totalTimeNs_) * 100.0 : 0.0;
        out << "      \"percentOfTotal\": " << std::fixed << std::setprecision(2) << percent << ",\n";
        
        // (AR) المُستدعون / (EN) Callers
        out << "      \"callers\": {";
        {
            bool first = true;
            for (const auto& [cname, count] : f->callers) {
                if (!first) out << ", ";
                out << "\"" << escapeJSON(cname) << "\": " << count;
                first = false;
            }
        }
        out << "},\n";
        
        // (AR) المُستدعين / (EN) Callees
        out << "      \"callees\": {";
        {
            bool first = true;
            for (const auto& [cname, count] : f->callees) {
                if (!first) out << ", ";
                out << "\"" << escapeJSON(cname) << "\": " << count;
                first = false;
            }
        }
        out << "}\n";
        
        out << "    }" << (i < sorted.size() - 1 ? "," : "") << "\n";
    }
    out << "  ],\n";
    
    // (AR) شجرة الاستدعاءات / (EN) Call tree
    out << "  \"callTree\": ";
    buildCallTreeJSON(callTreeRoot_.get(), out, 2);
    out << ",\n";
    
    // (AR) نقاط الاختناق / (EN) Hot spots
    out << "  \"hotSpots\": [\n";
    auto hotSpots = getHotSpots();
    for (size_t i = 0; i < hotSpots.size(); ++i) {
        const auto& hs = hotSpots[i];
        out << "    {\n";
        out << "      \"functionName\": \"" << escapeJSON(hs.functionName) << "\",\n";
        out << "      \"percentOfTotal\": " << std::fixed << std::setprecision(2) << hs.percentOfTotal << ",\n";
        out << "      \"severity\": " << hs.severity << ",\n";
        out << "      \"reason\": \"" << escapeJSON(hs.reason) << "\"\n";
        out << "    }" << (i < hotSpots.size() - 1 ? "," : "") << "\n";
    }
    out << "  ]\n";
    
    out << "}\n";
    
    return out.str();
}

void ProfilerCore::buildCallTreeJSON(const CallTreeNode* node, std::ostringstream& out,
                                      int ind) const {
    if (!node) {
        out << "null";
        return;
    }
    
    out << "{\n";
    out << indent(ind+1) << "\"name\": \"" << escapeJSON(node->functionName) << "\",\n";
    out << indent(ind+1) << "\"callCount\": " << node->callCount << ",\n";
    out << indent(ind+1) << "\"inclusiveNs\": " << node->inclusiveNs << ",\n";
    out << indent(ind+1) << "\"exclusiveNs\": " << node->exclusiveNs << ",\n";
    out << indent(ind+1) << "\"depth\": " << node->depth << ",\n";
    out << indent(ind+1) << "\"children\": [\n";
    
    for (size_t i = 0; i < node->children.size(); ++i) {
        out << indent(ind+2);
        buildCallTreeJSON(node->children[i].get(), out, ind+2);
        if (i < node->children.size() - 1) out << ",";
        out << "\n";
    }
    
    out << indent(ind+1) << "]\n";
    out << indent(ind) << "}";
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) إنتاج تقرير HTML
// (EN) HTML Report Generation
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateHTMLReport() const {
    std::ostringstream out;
    
    out << R"PROF(<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>تقرير مصحح الأداء — لغة ص</title>
    <style>
        :root { --bg: #0d1117; --card-bg: #161b22; --border: #30363d; --text: #c9d1d9; --accent: #58a6ff; --accent2: #79c0ff; --dim: #484f58; --muted: #8b949e; }
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { 
            font-family: 'Segoe UI', Tahoma, Arial, sans-serif;
            background: var(--bg); color: var(--text);
            padding: 20px; direction: rtl; transition: all 0.3s;
        }
        body.light { --bg: #ffffff; --card-bg: #f6f8fa; --border: #d0d7de; --text: #1f2328; --accent: #0969da; --accent2: #0550ae; --dim: #656d76; --muted: #57606a; }
        .container { max-width: 1200px; margin: 0 auto; }
        h1 { color: var(--accent); margin-bottom: 20px; font-size: 1.8em; }
        h2 { color: var(--accent2); margin: 20px 0 10px; font-size: 1.3em; border-bottom: 1px solid var(--border); padding-bottom: 8px; }
        .summary { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 12px; margin-bottom: 20px; }
        .stat-card {
            background: var(--card-bg); border: 1px solid var(--border); border-radius: 8px;
            padding: 16px; text-align: center;
        }
        .stat-card .value { font-size: 1.8em; font-weight: bold; color: var(--accent); }
        .stat-card .label { font-size: 0.9em; color: var(--muted); margin-top: 4px; }
        table { width: 100%; border-collapse: collapse; margin: 10px 0; }
        th { background: var(--card-bg); color: var(--accent2); padding: 10px 8px; text-align: right; font-size: 0.9em; }
        td { padding: 8px; border-bottom: 1px solid var(--border); font-size: 0.85em; }
        tr:hover { background: var(--card-bg); }
        .bar { height: 12px; border-radius: 3px; background: var(--border); overflow: hidden; }
        .bar-fill { height: 100%; border-radius: 3px; }
        .bar-red { background: linear-gradient(90deg, #f85149, #da3633); }
        .bar-yellow { background: linear-gradient(90deg, #d29922, #e3b341); }
        .bar-green { background: linear-gradient(90deg, #3fb950, #2ea043); }
        .bar-blue { background: linear-gradient(90deg, #58a6ff, #388bfd); }
        .hotspot { background: #1c1410; border: 1px solid #f8514933; border-radius: 6px; padding: 12px; margin: 8px 0; }
        .hotspot .name { color: #f85149; font-weight: bold; }
        .severity-5 { border-color: #f85149; }
        .severity-4 { border-color: #d29922; }
        .severity-3 { border-color: #e3b341; }
        .tree { font-family: 'Courier New', monospace; font-size: 0.85em; background: var(--card-bg); padding: 16px; border-radius: 8px; overflow-x: auto; direction: ltr; text-align: left; }
        .builtin { color: var(--muted); font-style: italic; }
        .search-box { width: 100%; padding: 10px; margin: 10px 0; background: var(--card-bg); border: 1px solid var(--border); border-radius: 6px; color: var(--text); font-size: 1em; direction: rtl; }
        .search-box::placeholder { color: var(--dim); }
        th.sortable { cursor: pointer; user-select: none; }
        th.sortable:hover { color: var(--accent); }
        th.sortable::after { content: ' \21C5'; font-size: 0.8em; color: var(--dim); }
        th.sort-asc::after { content: ' \25B2'; color: var(--accent); }
        th.sort-desc::after { content: ' \25BC'; color: var(--accent); }
        .hidden { display: none; }
        .toolbar { display: flex; gap: 10px; margin-bottom: 15px; flex-wrap: wrap; align-items: center; }
        .toolbar button { background: var(--card-bg); border: 1px solid var(--border); color: var(--text); padding: 6px 14px; border-radius: 6px; cursor: pointer; font-size: 0.9em; }
        .toolbar button:hover { border-color: var(--accent); color: var(--accent); }
        .tabs { display: flex; gap: 5px; margin-bottom: 15px; border-bottom: 1px solid var(--border); }
        .tab { padding: 8px 16px; cursor: pointer; border: none; background: none; color: var(--muted); border-bottom: 2px solid transparent; }
        .tab:hover { color: var(--accent); }
        .tab.active { color: var(--accent); border-bottom-color: var(--accent); }
        .tab-content { display: none; }
        .tab-content.active { display: block; }
        .chart-container { background: var(--card-bg); border: 1px solid var(--border); border-radius: 8px; padding: 16px; margin: 15px 0; }
        .chart-row { display: flex; gap: 20px; flex-wrap: wrap; }
        .chart-box { flex: 1; min-width: 300px; }
        .bar-chart { width: 100%; }
        .bar-chart-item { display: flex; align-items: center; margin: 8px 0; }
        .bar-chart-label { width: 150px; font-size: 0.85em; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }
        .bar-chart-bar { flex: 1; height: 20px; background: var(--border); border-radius: 4px; overflow: hidden; position: relative; }
        .bar-chart-fill { height: 100%; border-radius: 4px; transition: width 0.3s; }
        .bar-chart-value { position: absolute; left: 8px; top: 50%; transform: translateY(-50%); font-size: 0.75em; color: #fff; text-shadow: 0 0 2px rgba(0,0,0,0.5); }
        .collapsible { cursor: pointer; user-select: none; }
        .collapsible::before { content: '\\25BC '; font-size: 0.8em; }
        .collapsible.collapsed::before { content: '\\25B6 '; }
        .collapsible-content { padding-right: 20px; }
        .collapsible.collapsed + .collapsible-content { display: none; }
        .tooltip { position: relative; }
        .tooltip:hover::after { content: attr(data-tip); position: absolute; bottom: 100%; right: 0; background: var(--card-bg); border: 1px solid var(--border); padding: 4px 8px; border-radius: 4px; font-size: 0.8em; white-space: nowrap; z-index: 100; }
        @media print { body { background: #fff; color: #000; } .toolbar { display: none; } }
    </style>
</head>
<body>
    <div class="container">
        <div class="toolbar">
            <button id="themeBtn">&#127763; تبديل المظهر</button>
            <button id="printBtn">&#128424; طباعة</button>
            <button id="csvBtn">&#128229; تصدير CSV</button>
        </div>
        <h1>&#128202; تقرير مصحح الأداء — لغة ص</h1>
        
        <h2>&#128203; الملخص العام</h2>
        <div class="summary">
            <div class="stat-card">
                <div class="value">)PROF" << formatTime(totalTimeNs_) << R"PROF(</div>
                <div class="label">الزمن الكلي</div>
            </div>
            <div class="stat-card">
                <div class="value">)PROF" << totalFunctionCalls_ << R"PROF(</div>
                <div class="label">استدعاءات الدوال</div>
            </div>
            <div class="stat-card">
                <div class="value">)PROF" << functionData_.size() << R"PROF(</div>
                <div class="label">دوال مُراقبة</div>
            </div>
            <div class="stat-card">
                <div class="value">)PROF" << maxCallDepth_ << R"PROF(</div>
                <div class="label">أقصى عمق استدعاء</div>
            </div>)PROF";
    
    if (options_.trackMemory) {
        out << R"PROF(
            <div class="stat-card">
                <div class="value">)PROF" << totalVariablesCreated_ << R"PROF(</div>
                <div class="label">متغيرات مُنشأة</div>
            </div>
            <div class="stat-card">
                <div class="value">)PROF" << totalObjectsCreated_ << R"PROF(</div>
                <div class="label">كائنات مُنشأة</div>
            </div>)PROF";
    }
    
    out << R"PROF(
        </div>
        
        <div class="tabs">
            <button class="tab active" data-tab="funcs">&#128293; الدوال</button>
            <button class="tab" data-tab="chart">&#128202; الرسم البياني</button>
            <button class="tab" data-tab="tree">&#127795; الشجرة</button>
        </div>
        
        <div id="tab-funcs" class="tab-content active">
        <h2>&#128293; الدوال الأبطأ (الزمن الحصري)</h2>
        <input type="text" id="funcSearch" class="search-box" placeholder="&#128269; بحث في الدوال...">
        <table>
            <thead>
                <tr>
                    <th>#</th>
                    <th class="sortable">الدالة</th>
                    <th class="sortable" data-type="num">استدعاءات</th>
                    <th class="sortable">حصري</th>
                    <th class="sortable">شامل</th>
                    <th class="sortable" data-type="num">%</th>
                    <th style="width:200px">النسبة</th>
                </tr>
            </thead>
            <tbody>)PROF";
    
    auto topFuncs = getTopFunctions(options_.topFunctionsCount, true);
    for (size_t i = 0; i < topFuncs.size(); ++i) {
        const auto* f = topFuncs[i];
        double percent = (totalTimeNs_ > 0)
            ? (static_cast<double>(f->totalExclusiveNs) / totalTimeNs_) * 100.0 : 0.0;
        
        std::string barClass = "bar-blue";
        if (percent > 25.0) barClass = "bar-red";
        else if (percent > 10.0) barClass = "bar-yellow";
        else if (percent > 5.0) barClass = "bar-green";
        
        out << "\n                <tr"
            << (f->isBuiltin ? " class=\"builtin\"" : "") << ">"
            << "<td>" << (i+1) << "</td>"
            << "<td>" << escapeHTML(f->name) << "</td>"
            << "<td>" << f->callCount << "</td>"
            << "<td>" << formatTime(f->totalExclusiveNs) << "</td>"
            << "<td>" << formatTime(f->totalInclusiveNs) << "</td>"
            << "<td>" << formatPercent(percent) << "</td>"
            << "<td><div class=\"bar\"><div class=\"bar-fill " << barClass
            << "\" style=\"width:" << std::min(percent, 100.0) << "%\"></div></div></td>"
            << "</tr>";
    }
    
    out << R"PROF(
            </tbody>
        </table>
        </div>
        
        <div id="tab-chart" class="tab-content">
        <h2>&#128202; الرسم البياني للأداء</h2>
        <div class="chart-container">
            <div class="chart-row">
                <div class="chart-box">
                    <h3>&#128293; توزيع الزمن (أعلى 10 دوال)</h3>
                    <div class="bar-chart" id="timeChart">)PROF";
    
    // (AR) إضافة الرسم البياني الشريطي / (EN) Add bar chart
    auto chartFuncs = getTopFunctions(10, true);
    for (size_t i = 0; i < chartFuncs.size(); ++i) {
        const auto* f = chartFuncs[i];
        double percent = (totalTimeNs_ > 0)
            ? (static_cast<double>(f->totalExclusiveNs) / totalTimeNs_) * 100.0 : 0.0;
        std::string color = "#58a6ff";
        if (percent > 25.0) color = "#f85149";
        else if (percent > 10.0) color = "#d29922";
        else if (percent > 5.0) color = "#3fb950";
        
        out << "\n                        <div class=\"bar-chart-item\">"
            << "<span class=\"bar-chart-label\" title=\"" << escapeHTML(f->name) << "\">" 
            << escapeHTML(f->name.length() > 18 ? f->name.substr(0, 15) + "..." : f->name) << "</span>"
            << "<div class=\"bar-chart-bar\">"
            << "<div class=\"bar-chart-fill\" style=\"width:" << std::min(percent * 2, 100.0) << "%;background:" << color << ";\"></div>"
            << "<span class=\"bar-chart-value\">" << formatPercent(percent) << "</span>"
            << "</div></div>";
    }
    
    out << R"PROF(
                    </div>
                </div>
                <div class="chart-box">
                    <h3>&#128200; توزيع الاستدعاءات (أعلى 10 دوال)</h3>
                    <div class="bar-chart" id="callChart">)PROF";
    
    // (AR) الرسم البياني لعدد الاستدعاءات / (EN) Call count bar chart
    auto callFuncs = getTopFunctions(functionData_.size(), true);
    std::sort(callFuncs.begin(), callFuncs.end(), 
        [](const FunctionProfileData* a, const FunctionProfileData* b) {
            return a->callCount > b->callCount;
        });
    callFuncs.resize(std::min(callFuncs.size(), size_t(10)));
    
    uint64_t maxCalls = callFuncs.empty() ? 1 : callFuncs[0]->callCount;
    for (const auto* f : callFuncs) {
        double pct = 100.0 * f->callCount / maxCalls;
        out << "\n                        <div class=\"bar-chart-item\">"
            << "<span class=\"bar-chart-label\" title=\"" << escapeHTML(f->name) << "\">" 
            << escapeHTML(f->name.length() > 18 ? f->name.substr(0, 15) + "..." : f->name) << "</span>"
            << "<div class=\"bar-chart-bar\">"
            << "<div class=\"bar-chart-fill\" style=\"width:" << pct << "%;background:#79c0ff;\"></div>"
            << "<span class=\"bar-chart-value\">" << f->callCount << "</span>"
            << "</div></div>";
    }
    
    out << R"PROF(
                    </div>
                </div>
            </div>
        </div>
        </div>
        
        <div id="tab-tree" class="tab-content">)PROF";
    
    // (AR) نقاط الاختناق / (EN) Hot spots
    auto hotSpots = getHotSpots();
    if (!hotSpots.empty()) {
        out << R"PROF(
        
        <h2>&#9888;&#65039; نقاط الاختناق</h2>)PROF";
        
        for (const auto& hs : hotSpots) {
            out << "\n        <div class=\"hotspot severity-" << hs.severity << "\">"
                << "<span class=\"name\">" << escapeHTML(hs.functionName) << "</span>"
                << " — " << formatPercent(hs.percentOfTotal) << " — "
                << escapeHTML(hs.reason) << "</div>";
        }
    }
    
    // (AR) شجرة الاستدعاءات / (EN) Call tree
    if (options_.trackCallTree && callTreeRoot_ && !callTreeRoot_->children.empty()) {
        out << R"PROF(
        
        <h2>&#127795; شجرة الاستدعاءات</h2>
        <div class="tree"><pre>)PROF";
        buildCallTreeText(callTreeRoot_.get(), out, "");
        out << "</pre></div>";
    }
    
    out << "\n        </div>"; // close tab-tree
    
    out << R"PROF(
        
        <div style="margin-top:20px;color:#484f58;font-size:0.8em;text-align:center">
            تم إنتاج هذا التقرير بواسطة مصحح أداء لغة ص (sad-profiler v1.2.0)
        </div>
    </div>
    <script>
    // (AR) أزرار شريط الأدوات / (EN) Toolbar buttons
    document.getElementById('themeBtn').addEventListener('click', function(){ document.body.classList.toggle('light'); });
    document.getElementById('printBtn').addEventListener('click', function(){ window.print(); });
    document.getElementById('csvBtn').addEventListener('click', function(){ exportCSV(); });
    
    // (AR) فرز الجدول / (EN) Table sorting
    document.querySelectorAll('th.sortable').forEach(function(th) {
        th.addEventListener('click', function() {
            var table = th.closest('table');
            var tbody = table.querySelector('tbody');
            var rows = Array.from(tbody.querySelectorAll('tr'));
            var idx = Array.from(th.parentNode.children).indexOf(th);
            var isNum = th.dataset.type === 'num';
            var asc = !th.classList.contains('sort-asc');
            
            table.querySelectorAll('th').forEach(function(h) { h.classList.remove('sort-asc', 'sort-desc'); });
            th.classList.add(asc ? 'sort-asc' : 'sort-desc');
            
            rows.sort(function(a, b) {
                var va = a.children[idx].textContent.trim();
                var vb = b.children[idx].textContent.trim();
                if (isNum) { va = parseFloat(va) || 0; vb = parseFloat(vb) || 0; }
                return asc ? (va > vb ? 1 : -1) : (va < vb ? 1 : -1);
            });
            rows.forEach(function(r) { tbody.appendChild(r); });
        });
    });
    
    // (AR) بحث / (EN) Search
    var searchBox = document.getElementById('funcSearch');
    if (searchBox) {
        searchBox.addEventListener('input', function() {
            var q = searchBox.value.toLowerCase();
            document.querySelectorAll('tbody tr').forEach(function(row) {
                var name = row.children[1] ? row.children[1].textContent.toLowerCase() : '';
                row.classList.toggle('hidden', q && name.indexOf(q) === -1);
            });
        });
    }
    
    // (AR) تصدير CSV / (EN) Export CSV
    function exportCSV() {
        var table = document.querySelector('table');
        if (!table) return;
        var csv = '\uFEFF';
        table.querySelectorAll('tr').forEach(function(row) {
            var cols = Array.from(row.querySelectorAll('th, td')).map(function(c) { return c.textContent; });
            csv += cols.join(',') + '\n';
        });
        var blob = new Blob([csv], {type: 'text/csv;charset=utf-8'});
        var a = document.createElement('a');
        a.href = URL.createObjectURL(blob);
        a.download = 'profiler_report.csv';
        a.click();
    }
    
    // (AR) تبديل التبويبات / (EN) Tab switching
    document.querySelectorAll('.tab').forEach(function(tab) {
        tab.addEventListener('click', function() {
            var target = tab.getAttribute('data-tab');
            document.querySelectorAll('.tab').forEach(function(t) { t.classList.remove('active'); });
            document.querySelectorAll('.tab-content').forEach(function(c) { c.classList.remove('active'); });
            tab.classList.add('active');
            var el = document.getElementById(target);
            if (el) el.classList.add('active');
        });
    });
    
    // (AR) عناصر قابلة للطي / (EN) Collapsible elements
    document.querySelectorAll('.collapsible-header').forEach(function(header) {
        header.addEventListener('click', function() {
            var content = header.nextElementSibling;
            if (content && content.classList.contains('collapsible-content')) {
                content.classList.toggle('open');
                header.classList.toggle('open');
            }
        });
    });
    </script>
</body>
</html>)PROF";
    
    return out.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) إنتاج بيانات مخطط اللهب (FlameGraph)
// (EN) FlameGraph Data Generation
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateFlameGraphData() const {
    std::ostringstream out;
    std::vector<std::string> stack;
    buildFlameGraphData(callTreeRoot_.get(), out, stack);
    return out.str();
}

void ProfilerCore::buildFlameGraphData(const CallTreeNode* node, std::ostringstream& out,
                                        std::vector<std::string>& stack) const {
    if (!node) return;
    
    if (node != callTreeRoot_.get()) {
        stack.push_back(node->functionName);
    }
    
    // (AR) إذا كانت عقدة ورقية، أو لها زمن حصري، اكتب سطراً
    // (EN) If leaf node or has exclusive time, write a line
    if (node->children.empty() || node->exclusiveNs > 0) {
        if (!stack.empty()) {
            for (size_t i = 0; i < stack.size(); ++i) {
                if (i > 0) out << ";";
                out << stack[i];
            }
            // (AR) القيمة بالميكروثانية / (EN) Value in microseconds
            out << " " << std::max(static_cast<int64_t>(1), node->inclusiveNs / 1000) << "\n";
        }
    }
    
    for (const auto& child : node->children) {
        buildFlameGraphData(child.get(), out, stack);
    }
    
    if (node != callTreeRoot_.get()) {
        stack.pop_back();
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) إنتاج تقرير CSV
// (EN) CSV Report Generation
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateCSVReport() const {
    std::ostringstream out;
    
    // (AR) رأس الجدول / (EN) Header
    out << "الدالة,الملف,السطر,استدعاءات,تكرارية,حصري_نانو,شامل_نانو,حصري_مللي,شامل_مللي,متوسط_حصري,متوسط_شامل,أدنى,أقصى,نسبة,معدل_استدعاء,تباين,تكرارية%,مدمجة\n";
    
    auto sorted = getTopFunctions(functionData_.size(), true);
    for (const auto* f : sorted) {
        double percent = (totalTimeNs_ > 0)
            ? (static_cast<double>(f->totalExclusiveNs) / totalTimeNs_) * 100.0 : 0.0;
        
        // (AR) هروب الفاصلة والاقتباس / (EN) Escape comma and quotes in CSV
        auto csvEscape = [](const std::string& s) -> std::string {
            if (s.find(',') != std::string::npos || s.find('"') != std::string::npos || s.find('\n') != std::string::npos) {
                std::string escaped = "\"";
                for (char c : s) {
                    if (c == '"') escaped += "\"\"";
                    else escaped += c;
                }
                escaped += "\"";
                return escaped;
            }
            return "\"" + s + "\"";
        };
        
        out << csvEscape(f->name) << ","
            << csvEscape(f->fileName) << ","
            << f->line << ","
            << f->callCount << ","
            << f->recursiveCallCount << ","
            << f->totalExclusiveNs << ","
            << f->totalInclusiveNs << ","
            << std::fixed << std::setprecision(3) << FunctionProfileData::nsToMs(f->totalExclusiveNs) << ","
            << std::fixed << std::setprecision(3) << FunctionProfileData::nsToMs(f->totalInclusiveNs) << ","
            << std::fixed << std::setprecision(1) << f->avgExclusiveNs() << ","
            << std::fixed << std::setprecision(1) << f->avgInclusiveNs() << ","
            << (f->minInclusiveNs == INT64_MAX ? 0 : f->minInclusiveNs) << ","
            << f->maxInclusiveNs << ","
            << std::fixed << std::setprecision(2) << percent << ","
            << std::fixed << std::setprecision(1) << f->callRatePerSecond(totalTimeNs_) << ","
            << std::fixed << std::setprecision(1) << f->varianceRatio() << ","
            << std::fixed << std::setprecision(1) << f->recursivePercent() << ","
            << (f->isBuiltin ? "نعم" : "لا") << "\n";
    }
    
    return out.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) ملخص مختصر (سطر واحد)
// (EN) One-liner summary
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateOneLinerSummary() const {
    std::ostringstream out;
    out << "📊 " << formatTime(totalTimeNs_) 
        << " | " << totalFunctionCalls_ << " استدعاء"
        << " | " << functionData_.size() << " دالة"
        << " | عمق " << maxCallDepth_;
    
    auto hotSpots = getHotSpots();
    if (!hotSpots.empty()) {
        out << " | 🔥 " << hotSpots[0].functionName 
            << " (" << formatPercent(hotSpots[0].percentOfTotal) << ")";
    }
    return out.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) تقرير تفصيلي لدالة محددة
// (EN) Detailed function report
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateFunctionDetail(const std::string& funcName) const {
    const auto* f = getFunctionData(funcName);
    if (!f) return "❌ الدالة غير موجودة: " + funcName;
    
    std::ostringstream out;
    double percent = (totalTimeNs_ > 0)
        ? (static_cast<double>(f->totalExclusiveNs) / totalTimeNs_) * 100.0 : 0.0;
    
    out << "═══ تفاصيل الدالة: " << f->name << " ═══\n"
        << "  📁 الملف:            " << (f->fileName.empty() ? "غير محدد" : f->fileName) << "\n"
        << "  📍 السطر:            " << f->line << "\n"
        << "  🏷 نوع:              " << (f->isBuiltin ? "مدمجة" : "مستخدم") << "\n"
        << "  📞 استدعاءات:        " << f->callCount << "\n"
        << "  🔄 تكرارية:          " << f->recursiveCallCount 
        << " (" << formatPercent(f->recursivePercent()) << ")\n"
        << "  ⏱ حصري إجمالي:     " << formatTime(f->totalExclusiveNs) << "\n"
        << "  ⏱ شامل إجمالي:     " << formatTime(f->totalInclusiveNs) << "\n"
        << "  ⏱ متوسط حصري:      " << formatTime(static_cast<int64_t>(f->avgExclusiveNs())) << "\n"
        << "  ⏱ متوسط شامل:      " << formatTime(static_cast<int64_t>(f->avgInclusiveNs())) << "\n"
        << "  ⏱ أقصر استدعاء:     " << formatTime(f->minInclusiveNs == INT64_MAX ? 0 : f->minInclusiveNs) << "\n"
        << "  ⏱ أطول استدعاء:     " << formatTime(f->maxInclusiveNs) << "\n"
        << "  📊 النسبة من الكل:   " << formatPercent(percent) << "\n"
        << "  📊 معدل الاستدعاء:   " << std::fixed << std::setprecision(1) 
        << f->callRatePerSecond(totalTimeNs_) << " مرة/ثانية\n"
        << "  📊 تباين:            " << std::fixed << std::setprecision(1) << f->varianceRatio() << ":1\n";
    
    if (!f->callers.empty()) {
        out << "\n  📥 مُستدعى من:\n";
        for (const auto& [caller, count] : f->callers) {
            out << "    - " << caller << " (" << count << " مرة)\n";
        }
    }
    
    if (!f->callees.empty()) {
        out << "\n  📤 يستدعي:\n";
        for (const auto& [callee, count] : f->callees) {
            out << "    - " << callee << " (" << count << " مرة)\n";
        }
    }
    
    return out.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) نسبة زمن الدوال المدمجة/المستخدم
// (EN) Builtin/User time percentages
// ═══════════════════════════════════════════════════════════════════════════════

double ProfilerCore::getBuiltinTimePercent() const {
    if (totalTimeNs_ <= 0) return 0.0;
    int64_t builtinTime = 0;
    for (const auto& [name, data] : functionData_) {
        if (data.isBuiltin) builtinTime += data.totalExclusiveNs;
    }
    return (static_cast<double>(builtinTime) / totalTimeNs_) * 100.0;
}

double ProfilerCore::getUserTimePercent() const {
    if (totalTimeNs_ <= 0) return 0.0;
    int64_t userTime = 0;
    for (const auto& [name, data] : functionData_) {
        if (!data.isBuiltin) userTime += data.totalExclusiveNs;
    }
    return (static_cast<double>(userTime) / totalTimeNs_) * 100.0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) إنتاج تقرير بالصيغة المحددة
// (EN) Generate report in specified format
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateReport(ReportFormat format) const {
    switch (format) {
        case ReportFormat::TEXT:       return generateTextReport();
        case ReportFormat::JSON:       return generateJSONReport();
        case ReportFormat::HTML:       return generateHTMLReport();
        case ReportFormat::FLAMEGRAPH: return generateFlameGraphData();
        case ReportFormat::CSV:        return generateCSVReport();
        default:                       return generateTextReport();
    }
}

bool ProfilerCore::saveReport(const std::string& filePath, ReportFormat format) const {
    // (AR) حماية من مسار فارغ / (EN) Guard against empty path
    if (filePath.empty()) return false;
    
    std::string report = generateReport(format);
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) return false;
    
    // (AR) إضافة BOM لملفات UTF-8 إذا كان مطلوباً / (EN) Add UTF-8 BOM if requested
    if (options_.utfBom) {
        file << "\xEF\xBB\xBF";
    }
    
    file << report;
    return file.good();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) استعلامات إضافية للبيانات
// (EN) Additional Data Queries
// ═══════════════════════════════════════════════════════════════════════════════

size_t ProfilerCore::getHotFunctionCount() const {
    size_t count = 0;
    for (const auto& [name, data] : functionData_) {
        if (data.isHot()) count++;
    }
    return count;
}

size_t ProfilerCore::getColdFunctionCount() const {
    size_t count = 0;
    for (const auto& [name, data] : functionData_) {
        if (data.isCold()) count++;
    }
    return count;
}

double ProfilerCore::getAverageCallTimeNs() const {
    if (totalFunctionCalls_ == 0) return 0.0;
    return static_cast<double>(totalTimeNs_) / totalFunctionCalls_;
}

std::vector<const FunctionProfileData*> ProfilerCore::getAllFunctionsSorted(bool byExclusive) const {
    return getTopFunctions(functionData_.size(), byExclusive);
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) تقرير المقارنة بين دالتين
// (EN) Comparison Report Between Two Functions
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateComparisonReport(const std::string& func1, const std::string& func2) const {
    const auto* f1 = getFunctionData(func1);
    const auto* f2 = getFunctionData(func2);
    
    std::ostringstream out;
    out << "═══════════════════════════════════════════════════════════════════\n";
    out << "           📊 تقرير المقارنة بين دالتين\n";
    out << "═══════════════════════════════════════════════════════════════════\n\n";
    
    if (!f1 && !f2) {
        out << "❌ كلتا الدالتين غير موجودتين\n";
        return out.str();
    }
    if (!f1) {
        out << "❌ الدالة الأولى غير موجودة: " << func1 << "\n";
        return out.str();
    }
    if (!f2) {
        out << "❌ الدالة الثانية غير موجودة: " << func2 << "\n";
        return out.str();
    }
    
    // (AR) جدول المقارنة / (EN) Comparison table
    out << std::left << std::setw(25) << "المعيار" 
        << std::setw(25) << f1->name 
        << std::setw(25) << f2->name 
        << "النسبة\n";
    out << std::string(75, '-') << "\n";
    
    auto compare = [&](const std::string& label, int64_t v1, int64_t v2) {
        double ratio = (v2 > 0) ? static_cast<double>(v1) / v2 : 0.0;
        std::string winner = (v1 < v2) ? " ←" : (v1 > v2) ? " →" : " =";
        out << std::left << std::setw(25) << label
            << std::setw(25) << v1
            << std::setw(25) << v2
            << std::fixed << std::setprecision(2) << ratio << "x" << winner << "\n";
    };
    
    compare("استدعاءات", f1->callCount, f2->callCount);
    compare("زمن حصري (نانو)", f1->totalExclusiveNs, f2->totalExclusiveNs);
    compare("زمن شامل (نانو)", f1->totalInclusiveNs, f2->totalInclusiveNs);
    compare("متوسط حصري", static_cast<int64_t>(f1->avgExclusiveNs()), static_cast<int64_t>(f2->avgExclusiveNs()));
    compare("أقصى زمن", f1->maxInclusiveNs, f2->maxInclusiveNs);
    compare("أدنى زمن", f1->minInclusiveNs == INT64_MAX ? 0 : f1->minInclusiveNs, 
                       f2->minInclusiveNs == INT64_MAX ? 0 : f2->minInclusiveNs);
    
    out << "\n📈 الخلاصة: ";
    if (f1->totalExclusiveNs < f2->totalExclusiveNs) {
        double pct = 100.0 * (1.0 - static_cast<double>(f1->totalExclusiveNs) / f2->totalExclusiveNs);
        out << f1->name << " أسرع بـ " << std::fixed << std::setprecision(1) << pct << "%\n";
    } else if (f1->totalExclusiveNs > f2->totalExclusiveNs) {
        double pct = 100.0 * (1.0 - static_cast<double>(f2->totalExclusiveNs) / f1->totalExclusiveNs);
        out << f2->name << " أسرع بـ " << std::fixed << std::setprecision(1) << pct << "%\n";
    } else {
        out << "الدالتان متساويتان في السرعة\n";
    }
    
    return out.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) تقرير التوزيع الزمني
// (EN) Time Distribution Report
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateTimeDistributionReport() const {
    std::ostringstream out;
    
    out << "═══════════════════════════════════════════════════════════════════\n";
    out << "              📊 تقرير التوزيع الزمني\n";
    out << "═══════════════════════════════════════════════════════════════════\n\n";
    
    if (functionData_.empty()) {
        out << "لا توجد بيانات\n";
        return out.str();
    }
    
    // (AR) تصنيف الدوال حسب الزمن / (EN) Classify functions by time
    struct TimeRange {
        std::string label;
        int64_t minNs, maxNs;
        size_t count = 0;
        int64_t totalTime = 0;
    };
    
    std::vector<TimeRange> ranges = {
        {"< 1 مكرو",       0, 1'000, 0, 0},
        {"1-10 مكرو",      1'000, 10'000, 0, 0},
        {"10-100 مكرو",    10'000, 100'000, 0, 0},
        {"100 مكرو - 1 مللي", 100'000, 1'000'000, 0, 0},
        {"1-10 مللي",      1'000'000, 10'000'000, 0, 0},
        {"10-100 مللي",    10'000'000, 100'000'000, 0, 0},
        {"100 مللي - 1 ث", 100'000'000, 1'000'000'000, 0, 0},
        {"> 1 ثانية",      1'000'000'000, INT64_MAX, 0, 0}
    };
    
    for (const auto& [name, data] : functionData_) {
        int64_t avgTime = static_cast<int64_t>(data.avgExclusiveNs());
        for (auto& range : ranges) {
            if (avgTime >= range.minNs && avgTime < range.maxNs) {
                range.count++;
                range.totalTime += data.totalExclusiveNs;
                break;
            }
        }
    }
    
    // (AR) عرض التوزيع / (EN) Display distribution
    out << std::left << std::setw(20) << "النطاق الزمني" 
        << std::setw(10) << "الدوال"
        << std::setw(15) << "الزمن الكلي"
        << "توزيع\n";
    out << std::string(60, '-') << "\n";
    
    size_t maxCount = 0;
    for (const auto& r : ranges) if (r.count > maxCount) maxCount = r.count;
    
    for (const auto& range : ranges) {
        if (range.count == 0) continue;
        int barLen = maxCount > 0 ? static_cast<int>(20.0 * range.count / maxCount) : 0;
        out << std::left << std::setw(20) << range.label
            << std::setw(10) << range.count
            << std::setw(15) << formatTime(range.totalTime)
            << std::string(barLen, '#') << "\n";
    }
    
    out << std::string(60, '-') << "\n";
    out << "إجمالي الدوال: " << functionData_.size() << "\n";
    
    return out.str();
}

// ═══════════════════════════════════════════════════════════════════════════════
// (AR) تقرير الدوال الساخنة/الباردة
// (EN) Hot/Cold Functions Report
// ═══════════════════════════════════════════════════════════════════════════════

std::string ProfilerCore::generateHotColdReport() const {
    std::ostringstream out;
    
    out << "═══════════════════════════════════════════════════════════════════\n";
    out << "          🔥❄️ تقرير الدوال الساخنة والباردة\n";
    out << "═══════════════════════════════════════════════════════════════════\n\n";
    
    std::vector<const FunctionProfileData*> hotFuncs, coldFuncs, normalFuncs;
    
    for (const auto& [name, data] : functionData_) {
        if (data.isHot()) hotFuncs.push_back(&data);
        else if (data.isCold()) coldFuncs.push_back(&data);
        else normalFuncs.push_back(&data);
    }
    
    // (AR) ترتيب حسب عدد الاستدعاءات / (EN) Sort by call count
    auto sortByCallCount = [](const FunctionProfileData* a, const FunctionProfileData* b) {
        return a->callCount > b->callCount;
    };
    std::sort(hotFuncs.begin(), hotFuncs.end(), sortByCallCount);
    
    // (AR) الدوال الساخنة / (EN) Hot functions
    out << "🔥 الدوال الساخنة (>1000 استدعاء): " << hotFuncs.size() << " دالة\n";
    out << std::string(50, '-') << "\n";
    for (size_t i = 0; i < std::min(hotFuncs.size(), size_t(10)); ++i) {
        const auto* f = hotFuncs[i];
        out << "  " << (i+1) << ". " << f->name 
            << " — " << f->callCount << " استدعاء"
            << " — " << formatTime(f->totalExclusiveNs) << "\n";
    }
    if (hotFuncs.size() > 10) out << "  ... و " << (hotFuncs.size() - 10) << " دالة أخرى\n";
    
    // (AR) الدوال الباردة / (EN) Cold functions
    out << "\n❄️ الدوال الباردة (استدعاء واحد): " << coldFuncs.size() << " دالة\n";
    out << std::string(50, '-') << "\n";
    
    // (AR) ترتيب حسب الزمن تنازلياً / (EN) Sort by time descending
    std::sort(coldFuncs.begin(), coldFuncs.end(), 
        [](const FunctionProfileData* a, const FunctionProfileData* b) {
            return a->totalExclusiveNs > b->totalExclusiveNs;
        });
    
    int64_t coldTotalTime = 0;
    for (const auto* f : coldFuncs) coldTotalTime += f->totalExclusiveNs;
    
    for (size_t i = 0; i < std::min(coldFuncs.size(), size_t(5)); ++i) {
        const auto* f = coldFuncs[i];
        out << "  " << (i+1) << ". " << f->name 
            << " — " << formatTime(f->totalExclusiveNs) << "\n";
    }
    if (coldFuncs.size() > 5) out << "  ... و " << (coldFuncs.size() - 5) << " دالة أخرى\n";
    out << "  📊 زمن الدوال الباردة الكلي: " << formatTime(coldTotalTime) << "\n";
    
    // (AR) ملخص / (EN) Summary
    out << "\n📊 الملخص:\n";
    out << "  - ساخنة:   " << hotFuncs.size() << " (" 
        << formatPercent(100.0 * hotFuncs.size() / functionData_.size()) << ")\n";
    out << "  - عادية:   " << normalFuncs.size() << " (" 
        << formatPercent(100.0 * normalFuncs.size() / functionData_.size()) << ")\n";
    out << "  - باردة:   " << coldFuncs.size() << " (" 
        << formatPercent(100.0 * coldFuncs.size() / functionData_.size()) << ")\n";
    
    return out.str();
}

} // namespace Tools
} // namespace Sad
