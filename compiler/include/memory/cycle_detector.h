// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file cycle_detector.h
 * @brief كاشف دورات المراجع / Reference Cycle Detector
 * 
 * نظام متطور لاكتشاف دورات المراجع في وقت الترجمة ووقت التشغيل.
 * دورات المراجع تحدث عندما تشير كائنات لبعضها البعض بشكل دائري،
 * مما يمنع جامع القمامة من تحريرها.
 * 
 * مثال على دورة:
 *   أ -> ب -> ج -> أ  (دورة!)
 * 
 * الحل: استخدام مراجع ضعيفة (Weak) لكسر الدورة.
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_MEMORY_CYCLE_DETECTOR_H
#define SAD_MEMORY_CYCLE_DETECTOR_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <functional>

namespace Sad {
namespace Memory {

// =============================================================================
//                    أنواع البيانات
// =============================================================================

/**
 * @brief نوع المرجع
 */
enum class ReferenceType {
    Strong,     // مرجع قوي (يملك)
    Weak,       // مرجع ضعيف (لا يملك)
    Raw,        // مؤشر خام
    Unknown     // غير معروف
};

/**
 * @brief معلومات عن حافة المرجع في رسم الكائنات
 */
struct ReferenceEdge {
    std::string sourceId;       // معرف الكائن المصدر
    std::string targetId;       // معرف الكائن الهدف
    std::string fieldName;      // اسم الحقل
    ReferenceType refType;      // نوع المرجع
    size_t sourceLineNumber;    // رقم السطر (للتحليل الثابت)
    
    bool isStrong() const { return refType == ReferenceType::Strong; }
    bool isWeak() const { return refType == ReferenceType::Weak; }
};

/**
 * @brief معلومات عن دورة مكتشفة
 */
struct DetectedCycle {
    std::vector<std::string> nodeIds;       // الكائنات في الدورة
    std::vector<ReferenceEdge> edges;       // الحواف المشكلة للدورة
    size_t cycleLength;                     // طول الدورة
    bool hasWeakReference;                  // هل تحتوي على مرجع ضعيف؟
    bool isProblematic;                     // هل مشكلة (كل المراجع قوية)؟
    std::string suggestedFix;               // الإصلاح المقترح
    std::string suggestedFixArabic;         // الإصلاح بالعربية
    
    // للتنقيح
    std::string toString() const;
    std::string toStringArabic() const;
};

/**
 * @brief إحصائيات التحليل
 */
struct CycleDetectionStats {
    size_t totalNodes = 0;          // إجمالي الكائنات
    size_t totalEdges = 0;          // إجمالي المراجع
    size_t strongEdges = 0;         // مراجع قوية
    size_t weakEdges = 0;           // مراجع ضعيفة
    size_t cyclesFound = 0;         // دورات مكتشفة
    size_t problematicCycles = 0;   // دورات مشكلة
    size_t safeCycles = 0;          // دورات آمنة (بها Weak)
    double analysisTimeMs = 0.0;    // وقت التحليل
};

// =============================================================================
//                    رسم الكائنات (Object Graph)
// =============================================================================

/**
 * @brief عقدة في رسم الكائنات
 */
struct ObjectNode {
    std::string id;                 // معرف فريد
    std::string typeName;           // اسم النوع
    std::string variableName;       // اسم المتغير
    size_t declarationLine;         // سطر التعريف
    std::string filePath;           // مسار الملف
    std::vector<ReferenceEdge> outgoingEdges;  // المراجع الصادرة
};

/**
 * @brief رسم الكائنات للتحليل
 */
class ObjectGraph {
public:
    ObjectGraph();
    ~ObjectGraph();
    
    // بناء الرسم
    void addNode(const ObjectNode& node);
    void addEdge(const ReferenceEdge& edge);
    void removeNode(const std::string& nodeId);
    void removeEdge(const std::string& sourceId, const std::string& targetId);
    void clear();
    
    // الاستعلام
    const ObjectNode* getNode(const std::string& nodeId) const;
    std::vector<const ObjectNode*> getAllNodes() const;
    std::vector<ReferenceEdge> getIncomingEdges(const std::string& nodeId) const;
    std::vector<ReferenceEdge> getOutgoingEdges(const std::string& nodeId) const;
    size_t getNodeCount() const;
    size_t getEdgeCount() const;
    
    // التنقيح
    void dump() const;
    std::string toGraphviz() const;  // تصدير لـ DOT format

private:
    std::unordered_map<std::string, ObjectNode> nodes_;
    std::vector<ReferenceEdge> edges_;
};

// =============================================================================
//                    كاشف الدورات
// =============================================================================

/**
 * @brief خيارات كشف الدورات
 */
struct CycleDetectorOptions {
    bool detectAtCompileTime = true;    // كشف وقت الترجمة
    bool detectAtRuntime = false;       // كشف وقت التشغيل
    bool reportAllCycles = true;        // الإبلاغ عن كل الدورات
    bool reportOnlyProblematic = false; // الإبلاغ عن المشكلة فقط
    size_t maxCycleLength = 20;         // أقصى طول دورة للبحث
    bool generateSuggestions = true;    // إنشاء اقتراحات الإصلاح
    bool verboseOutput = false;         // مخرجات تفصيلية
};

/**
 * @brief كاشف دورات المراجع
 * 
 * يحلل رسم الكائنات لاكتشاف الدورات المحتملة التي قد تسبب
 * تسرب ذاكرة في نظام عدّ المراجع.
 */
class CycleDetector {
public:
    /**
     * @brief إنشاء كاشف جديد
     */
    CycleDetector();
    
    /**
     * @brief إنشاء مع خيارات مخصصة
     */
    explicit CycleDetector(const CycleDetectorOptions& options);
    
    ~CycleDetector();
    
    // ==========================================================================
    //                    التحليل الثابت (وقت الترجمة)
    // ==========================================================================
    
    /**
     * @brief تحليل رسم كائنات للدورات
     * @param graph رسم الكائنات
     * @return قائمة الدورات المكتشفة
     */
    std::vector<DetectedCycle> analyzeGraph(const ObjectGraph& graph);
    
    /**
     * @brief تحليل ملف مصدري
     * @param filePath مسار الملف
     * @return قائمة الدورات
     */
    std::vector<DetectedCycle> analyzeFile(const std::string& filePath);
    
    /**
     * @brief تحليل كود مصدري مباشرة
     * @param sourceCode الكود
     * @param fileName اسم الملف للرسائل
     * @return قائمة الدورات
     */
    std::vector<DetectedCycle> analyzeSource(
        const std::string& sourceCode,
        const std::string& fileName = "<input>"
    );
    
    /**
     * @brief تحليل مشروع كامل
     * @param projectPath مسار المشروع
     * @return قائمة الدورات من كل الملفات
     */
    std::vector<DetectedCycle> analyzeProject(const std::string& projectPath);
    
    // ==========================================================================
    //                    التحليل الديناميكي (وقت التشغيل)
    // ==========================================================================
    
    /**
     * @brief بدء مراقبة الكائنات في وقت التشغيل
     */
    void startRuntimeMonitoring();
    
    /**
     * @brief إيقاف المراقبة
     */
    void stopRuntimeMonitoring();
    
    /**
     * @brief تسجيل إنشاء كائن
     * @param objectId معرف الكائن
     * @param typeName نوع الكائن
     */
    void registerObjectCreation(const std::string& objectId, const std::string& typeName);
    
    /**
     * @brief تسجيل إنشاء مرجع
     */
    void registerReferenceCreation(
        const std::string& sourceId,
        const std::string& targetId,
        const std::string& fieldName,
        ReferenceType refType
    );
    
    /**
     * @brief تسجيل تدمير مرجع
     */
    void registerReferenceDestruction(
        const std::string& sourceId,
        const std::string& targetId
    );
    
    /**
     * @brief فحص فوري للدورات
     * @return الدورات المكتشفة حالياً
     */
    std::vector<DetectedCycle> checkForCyclesNow();
    
    // ==========================================================================
    //                    التقارير
    // ==========================================================================
    
    /**
     * @brief إنشاء تقرير نصي
     */
    std::string generateReport(
        const std::vector<DetectedCycle>& cycles,
        bool arabic = true
    ) const;
    
    /**
     * @brief إنشاء تقرير JSON
     */
    std::string generateJsonReport(const std::vector<DetectedCycle>& cycles) const;
    
    /**
     * @brief الحصول على الإحصائيات
     */
    CycleDetectionStats getStats() const;
    
    // ==========================================================================
    //                    الاقتراحات
    // ==========================================================================
    
    /**
     * @brief توليد اقتراحات لإصلاح دورة
     * @param cycle الدورة المكتشفة
     * @return اقتراح الإصلاح
     */
    std::string generateFixSuggestion(const DetectedCycle& cycle, bool arabic = true) const;
    
    /**
     * @brief اقتراح أفضل حافة لتحويلها لـ Weak
     */
    std::optional<ReferenceEdge> suggestWeakEdge(const DetectedCycle& cycle) const;
    
    // ==========================================================================
    //                    الإعدادات
    // ==========================================================================
    
    void setOptions(const CycleDetectorOptions& options);
    const CycleDetectorOptions& getOptions() const;
    
    // ==========================================================================
    //                    Callbacks
    // ==========================================================================
    
    /**
     * @brief تعيين دالة استدعاء عند اكتشاف دورة
     */
    void setOnCycleDetected(std::function<void(const DetectedCycle&)> callback);

private:
    CycleDetectorOptions options_;
    CycleDetectionStats stats_;
    ObjectGraph runtimeGraph_;
    bool isMonitoring_ = false;
    std::function<void(const DetectedCycle&)> onCycleDetected_;
    
    // خوارزميات الكشف
    std::vector<DetectedCycle> findCyclesDFS(const ObjectGraph& graph);
    std::vector<DetectedCycle> findCyclesTarjan(const ObjectGraph& graph);
    std::vector<DetectedCycle> findCyclesJohnson(const ObjectGraph& graph);
    
    // أدوات مساعدة
    bool isNodeInCycle(const std::string& nodeId, const std::vector<std::string>& cycle) const;
    DetectedCycle buildCycleInfo(
        const std::vector<std::string>& path,
        const ObjectGraph& graph
    ) const;
    void updateStats(const std::vector<DetectedCycle>& cycles);
};

// =============================================================================
//                    دوال مساعدة
// =============================================================================

/**
 * @brief تحويل نوع المرجع لنص عربي
 */
std::string referenceTypeToArabic(ReferenceType type);

/**
 * @brief تحويل نوع المرجع لنص إنجليزي
 */
std::string referenceTypeToEnglish(ReferenceType type);

/**
 * @brief إنشاء رسالة تعليمية عن الدورات
 */
std::string getCycleExplanation(bool arabic = true);

} // namespace Memory
} // namespace Sad

#endif // SAD_MEMORY_CYCLE_DETECTOR_H
