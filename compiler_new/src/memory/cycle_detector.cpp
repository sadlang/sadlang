// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file cycle_detector.cpp
 * @brief تنفيذ كاشف دورات المراجع / Reference Cycle Detector Implementation
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "memory/cycle_detector.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stack>
#include <chrono>
#include <iomanip>

namespace Sad {
namespace Memory {

// =============================================================================
//                    DetectedCycle Methods
// =============================================================================

std::string DetectedCycle::toString() const {
    std::ostringstream ss;
    ss << "Cycle [length=" << cycleLength << ", problematic=" << (isProblematic ? "yes" : "no") << "]: ";
    for (size_t i = 0; i < nodeIds.size(); i++) {
        ss << nodeIds[i];
        if (i < nodeIds.size() - 1) ss << " -> ";
    }
    ss << " -> " << nodeIds[0];  // Back to start
    return ss.str();
}

std::string DetectedCycle::toStringArabic() const {
    std::ostringstream ss;
    ss << "دورة [الطول=" << cycleLength << "، مشكلة=" << (isProblematic ? "نعم" : "لا") << "]: ";
    for (size_t i = 0; i < nodeIds.size(); i++) {
        ss << nodeIds[i];
        if (i < nodeIds.size() - 1) ss << " ← ";
    }
    ss << " ← " << nodeIds[0];  // العودة للبداية
    return ss.str();
}

// =============================================================================
//                    ObjectGraph Implementation
// =============================================================================

ObjectGraph::ObjectGraph() = default;
ObjectGraph::~ObjectGraph() = default;

void ObjectGraph::addNode(const ObjectNode& node) {
    nodes_[node.id] = node;
}

void ObjectGraph::addEdge(const ReferenceEdge& edge) {
    edges_.push_back(edge);
    
    // تحديث الحواف الصادرة للعقدة المصدر
    auto it = nodes_.find(edge.sourceId);
    if (it != nodes_.end()) {
        it->second.outgoingEdges.push_back(edge);
    }
}

void ObjectGraph::removeNode(const std::string& nodeId) {
    nodes_.erase(nodeId);
    
    // إزالة الحواف المتعلقة
    edges_.erase(
        std::remove_if(edges_.begin(), edges_.end(),
            [&nodeId](const ReferenceEdge& e) {
                return e.sourceId == nodeId || e.targetId == nodeId;
            }
        ),
        edges_.end()
    );
}

void ObjectGraph::removeEdge(const std::string& sourceId, const std::string& targetId) {
    edges_.erase(
        std::remove_if(edges_.begin(), edges_.end(),
            [&sourceId, &targetId](const ReferenceEdge& e) {
                return e.sourceId == sourceId && e.targetId == targetId;
            }
        ),
        edges_.end()
    );
    
    // تحديث العقدة المصدر
    auto it = nodes_.find(sourceId);
    if (it != nodes_.end()) {
        auto& outgoing = it->second.outgoingEdges;
        outgoing.erase(
            std::remove_if(outgoing.begin(), outgoing.end(),
                [&targetId](const ReferenceEdge& e) {
                    return e.targetId == targetId;
                }
            ),
            outgoing.end()
        );
    }
}

void ObjectGraph::clear() {
    nodes_.clear();
    edges_.clear();
}

const ObjectNode* ObjectGraph::getNode(const std::string& nodeId) const {
    auto it = nodes_.find(nodeId);
    return it != nodes_.end() ? &it->second : nullptr;
}

std::vector<const ObjectNode*> ObjectGraph::getAllNodes() const {
    std::vector<const ObjectNode*> result;
    result.reserve(nodes_.size());
    for (const auto& pair : nodes_) {
        result.push_back(&pair.second);
    }
    return result;
}

std::vector<ReferenceEdge> ObjectGraph::getIncomingEdges(const std::string& nodeId) const {
    std::vector<ReferenceEdge> result;
    for (const auto& edge : edges_) {
        if (edge.targetId == nodeId) {
            result.push_back(edge);
        }
    }
    return result;
}

std::vector<ReferenceEdge> ObjectGraph::getOutgoingEdges(const std::string& nodeId) const {
    auto it = nodes_.find(nodeId);
    if (it != nodes_.end()) {
        return it->second.outgoingEdges;
    }
    return {};
}

size_t ObjectGraph::getNodeCount() const {
    return nodes_.size();
}

size_t ObjectGraph::getEdgeCount() const {
    return edges_.size();
}

void ObjectGraph::dump() const {
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════╗\n";
    std::cout << "║         رسم الكائنات (Object Graph)  ║\n";
    std::cout << "╚═══════════════════════════════════════╝\n";
    std::cout << "\n";
    std::cout << "  العقد (" << nodes_.size() << "):\n";
    for (const auto& pair : nodes_) {
        const auto& node = pair.second;
        std::cout << "    • " << node.id << " : " << node.typeName;
        if (!node.variableName.empty()) {
            std::cout << " (" << node.variableName << ")";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
    std::cout << "  الحواف (" << edges_.size() << "):\n";
    for (const auto& edge : edges_) {
        std::cout << "    " << edge.sourceId << " --[" 
                  << edge.fieldName << "]--> " << edge.targetId;
        std::cout << " (" << referenceTypeToArabic(edge.refType) << ")\n";
    }
    std::cout << "\n";
}

std::string ObjectGraph::toGraphviz() const {
    std::ostringstream dot;
    dot << "digraph ObjectGraph {\n";
    dot << "  rankdir=LR;\n";
    dot << "  node [shape=box];\n";
    dot << "\n";
    
    // العقد
    for (const auto& pair : nodes_) {
        const auto& node = pair.second;
        dot << "  \"" << node.id << "\" [label=\"" << node.typeName;
        if (!node.variableName.empty()) {
            dot << "\\n" << node.variableName;
        }
        dot << "\"];\n";
    }
    
    dot << "\n";
    
    // الحواف
    for (const auto& edge : edges_) {
        dot << "  \"" << edge.sourceId << "\" -> \"" << edge.targetId << "\"";
        dot << " [label=\"" << edge.fieldName << "\"";
        if (edge.refType == ReferenceType::Weak) {
            dot << ", style=dashed, color=blue";
        } else if (edge.refType == ReferenceType::Strong) {
            dot << ", style=bold, color=red";
        }
        dot << "];\n";
    }
    
    dot << "}\n";
    return dot.str();
}

// =============================================================================
//                    CycleDetector Implementation
// =============================================================================

CycleDetector::CycleDetector()
    : options_(CycleDetectorOptions{}) {
}

CycleDetector::CycleDetector(const CycleDetectorOptions& options)
    : options_(options) {
}

CycleDetector::~CycleDetector() = default;

// =============================================================================
//                    التحليل الثابت
// =============================================================================

std::vector<DetectedCycle> CycleDetector::analyzeGraph(const ObjectGraph& graph) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // تحديث الإحصائيات
    stats_.totalNodes = graph.getNodeCount();
    stats_.totalEdges = graph.getEdgeCount();
    
    // استخدام خوارزمية DFS للبحث عن الدورات
    auto cycles = findCyclesDFS(graph);
    
    auto end = std::chrono::high_resolution_clock::now();
    stats_.analysisTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    
    // تحديث الإحصائيات النهائية
    updateStats(cycles);
    
    // استدعاء callback لكل دورة
    if (onCycleDetected_) {
        for (const auto& cycle : cycles) {
            onCycleDetected_(cycle);
        }
    }
    
    return cycles;
}

std::vector<DetectedCycle> CycleDetector::analyzeFile(const std::string& filePath) {
    // TODO: قراءة الملف وبناء الرسم من AST
    // حالياً نرجع قائمة فارغة
    return {};
}

std::vector<DetectedCycle> CycleDetector::analyzeSource(
    const std::string& sourceCode,
    const std::string& fileName
) {
    // TODO: تحليل الكود وبناء الرسم
    return {};
}

std::vector<DetectedCycle> CycleDetector::analyzeProject(const std::string& projectPath) {
    // TODO: تحليل كل الملفات في المشروع
    return {};
}

// =============================================================================
//                    خوارزمية DFS للبحث عن الدورات
// =============================================================================

std::vector<DetectedCycle> CycleDetector::findCyclesDFS(const ObjectGraph& graph) {
    std::vector<DetectedCycle> cycles;
    
    // مجموعات للتتبع
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> inStack;
    std::vector<std::string> currentPath;
    
    // دالة DFS متداخلة
    std::function<void(const std::string&)> dfs = [&](const std::string& nodeId) {
        if (currentPath.size() > options_.maxCycleLength) {
            return;
        }
        
        // إذا كانت العقدة في المسار الحالي، وجدنا دورة
        if (inStack.count(nodeId)) {
            // بناء الدورة من نقطة التكرار
            auto it = std::find(currentPath.begin(), currentPath.end(), nodeId);
            if (it != currentPath.end()) {
                std::vector<std::string> cyclePath(it, currentPath.end());
                auto cycle = buildCycleInfo(cyclePath, graph);
                
                // فحص إذا كانت مشكلة أو آمنة
                if (options_.reportAllCycles || 
                    (options_.reportOnlyProblematic && cycle.isProblematic)) {
                    cycles.push_back(cycle);
                }
            }
            return;
        }
        
        // إذا زُرت مسبقاً (ليس في المسار الحالي)، تخطي
        if (visited.count(nodeId)) {
            return;
        }
        
        visited.insert(nodeId);
        inStack.insert(nodeId);
        currentPath.push_back(nodeId);
        
        // زيارة الجيران (المراجع القوية فقط للدورات المشكلة)
        auto outgoing = graph.getOutgoingEdges(nodeId);
        for (const auto& edge : outgoing) {
            // نتابع فقط المراجع القوية
            if (edge.isStrong() || !options_.reportOnlyProblematic) {
                dfs(edge.targetId);
            }
        }
        
        currentPath.pop_back();
        inStack.erase(nodeId);
    };
    
    // بدء DFS من كل عقدة
    for (const auto* node : graph.getAllNodes()) {
        if (!visited.count(node->id)) {
            visited.clear();  // إعادة تعيين للبحث من عقدة جديدة
            inStack.clear();
            currentPath.clear();
            dfs(node->id);
        }
    }
    
    return cycles;
}

std::vector<DetectedCycle> CycleDetector::findCyclesTarjan(const ObjectGraph& graph) {
    // TODO: تنفيذ خوارزمية Tarjan للمكونات المتصلة بقوة
    return {};
}

std::vector<DetectedCycle> CycleDetector::findCyclesJohnson(const ObjectGraph& graph) {
    // TODO: تنفيذ خوارزمية Johnson للدورات الأولية
    return {};
}

// =============================================================================
//                    التحليل الديناميكي
// =============================================================================

void CycleDetector::startRuntimeMonitoring() {
    isMonitoring_ = true;
    runtimeGraph_.clear();
}

void CycleDetector::stopRuntimeMonitoring() {
    isMonitoring_ = false;
}

void CycleDetector::registerObjectCreation(
    const std::string& objectId,
    const std::string& typeName
) {
    if (!isMonitoring_) return;
    
    ObjectNode node;
    node.id = objectId;
    node.typeName = typeName;
    runtimeGraph_.addNode(node);
}

void CycleDetector::registerReferenceCreation(
    const std::string& sourceId,
    const std::string& targetId,
    const std::string& fieldName,
    ReferenceType refType
) {
    if (!isMonitoring_) return;
    
    ReferenceEdge edge;
    edge.sourceId = sourceId;
    edge.targetId = targetId;
    edge.fieldName = fieldName;
    edge.refType = refType;
    runtimeGraph_.addEdge(edge);
}

void CycleDetector::registerReferenceDestruction(
    const std::string& sourceId,
    const std::string& targetId
) {
    if (!isMonitoring_) return;
    runtimeGraph_.removeEdge(sourceId, targetId);
}

std::vector<DetectedCycle> CycleDetector::checkForCyclesNow() {
    return analyzeGraph(runtimeGraph_);
}

// =============================================================================
//                    التقارير
// =============================================================================

std::string CycleDetector::generateReport(
    const std::vector<DetectedCycle>& cycles,
    bool arabic
) const {
    std::ostringstream report;
    
    if (arabic) {
        report << "\n";
        report << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
        report << "║                        تقرير كشف دورات المراجع                               ║\n";
        report << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
        report << "\n";
        report << "  📊 الإحصائيات:\n";
        report << "  ─────────────\n";
        report << "    • الكائنات المحللة:    " << stats_.totalNodes << "\n";
        report << "    • المراجع المحللة:     " << stats_.totalEdges << "\n";
        report << "    • الدورات المكتشفة:    " << cycles.size() << "\n";
        report << "    • دورات مشكلة:        " << stats_.problematicCycles << "\n";
        report << "    • دورات آمنة:         " << stats_.safeCycles << "\n";
        report << "    • وقت التحليل:        " << std::fixed << std::setprecision(2) 
               << stats_.analysisTimeMs << " مللي ثانية\n";
        report << "\n";
        
        if (cycles.empty()) {
            report << "  ✅ لم يتم اكتشاف أي دورات مراجع!\n";
        } else {
            report << "  ⚠️ الدورات المكتشفة:\n";
            report << "  ─────────────────────\n";
            
            for (size_t i = 0; i < cycles.size(); i++) {
                const auto& cycle = cycles[i];
                
                report << "\n";
                report << "  ┌─── دورة #" << (i + 1) << " ";
                if (cycle.isProblematic) {
                    report << "🔴 مشكلة";
                } else {
                    report << "🟢 آمنة";
                }
                report << " ───\n";
                report << "  │\n";
                report << "  │  المسار: ";
                for (size_t j = 0; j < cycle.nodeIds.size(); j++) {
                    report << cycle.nodeIds[j];
                    if (j < cycle.nodeIds.size() - 1) {
                        report << " → ";
                    }
                }
                report << " → " << cycle.nodeIds[0] << "\n";
                report << "  │\n";
                report << "  │  الطول: " << cycle.cycleLength << " عقد\n";
                report << "  │  مراجع ضعيفة: " << (cycle.hasWeakReference ? "نعم" : "لا") << "\n";
                
                if (cycle.isProblematic && options_.generateSuggestions) {
                    report << "  │\n";
                    report << "  │  💡 الإصلاح المقترح:\n";
                    report << "  │     " << cycle.suggestedFixArabic << "\n";
                }
                
                report << "  └────────────────────────\n";
            }
        }
        
        if (stats_.problematicCycles > 0) {
            report << "\n";
            report << getCycleExplanation(true);
        }
    } else {
        report << "\n";
        report << "╔═══════════════════════════════════════════════════════════════════════════════╗\n";
        report << "║                        Reference Cycle Detection Report                       ║\n";
        report << "╚═══════════════════════════════════════════════════════════════════════════════╝\n";
        report << "\n";
        report << "  Statistics:\n";
        report << "    Total nodes:        " << stats_.totalNodes << "\n";
        report << "    Total edges:        " << stats_.totalEdges << "\n";
        report << "    Cycles found:       " << cycles.size() << "\n";
        report << "    Problematic cycles: " << stats_.problematicCycles << "\n";
        report << "    Safe cycles:        " << stats_.safeCycles << "\n";
        report << "\n";
        
        for (size_t i = 0; i < cycles.size(); i++) {
            report << "  Cycle #" << (i + 1) << ": " << cycles[i].toString() << "\n";
        }
    }
    
    return report.str();
}

std::string CycleDetector::generateJsonReport(const std::vector<DetectedCycle>& cycles) const {
    std::ostringstream json;
    
    json << "{\n";
    json << "  \"stats\": {\n";
    json << "    \"totalNodes\": " << stats_.totalNodes << ",\n";
    json << "    \"totalEdges\": " << stats_.totalEdges << ",\n";
    json << "    \"cyclesFound\": " << cycles.size() << ",\n";
    json << "    \"problematicCycles\": " << stats_.problematicCycles << ",\n";
    json << "    \"safeCycles\": " << stats_.safeCycles << ",\n";
    json << "    \"analysisTimeMs\": " << stats_.analysisTimeMs << "\n";
    json << "  },\n";
    json << "  \"cycles\": [\n";
    
    for (size_t i = 0; i < cycles.size(); i++) {
        const auto& cycle = cycles[i];
        json << "    {\n";
        json << "      \"length\": " << cycle.cycleLength << ",\n";
        json << "      \"isProblematic\": " << (cycle.isProblematic ? "true" : "false") << ",\n";
        json << "      \"hasWeakReference\": " << (cycle.hasWeakReference ? "true" : "false") << ",\n";
        json << "      \"nodes\": [";
        for (size_t j = 0; j < cycle.nodeIds.size(); j++) {
            json << "\"" << cycle.nodeIds[j] << "\"";
            if (j < cycle.nodeIds.size() - 1) json << ", ";
        }
        json << "],\n";
        json << "      \"suggestedFix\": \"" << cycle.suggestedFix << "\"\n";
        json << "    }";
        if (i < cycles.size() - 1) json << ",";
        json << "\n";
    }
    
    json << "  ]\n";
    json << "}\n";
    
    return json.str();
}

CycleDetectionStats CycleDetector::getStats() const {
    return stats_;
}

// =============================================================================
//                    الاقتراحات
// =============================================================================

std::string CycleDetector::generateFixSuggestion(const DetectedCycle& cycle, bool arabic) const {
    if (!cycle.isProblematic) {
        return arabic 
            ? "الدورة آمنة - تحتوي على مرجع ضعيف يكسر الدورة"
            : "Cycle is safe - contains a weak reference breaking the cycle";
    }
    
    // البحث عن أفضل حافة لتحويلها
    auto suggestedEdge = suggestWeakEdge(cycle);
    
    if (suggestedEdge) {
        if (arabic) {
            return "حوّل المرجع '" + suggestedEdge->fieldName + 
                   "' من عم<> إلى مرجع_ضعيف<> لكسر الدورة.\n"
                   "المرجع الضعيف لا يمنع تحرير الكائن الهدف.";
        } else {
            return "Convert reference '" + suggestedEdge->fieldName + 
                   "' from Rc<> to Weak<> to break the cycle.\n"
                   "Weak references don't prevent target object cleanup.";
        }
    }
    
    return arabic
        ? "حوّل أحد المراجع في الدورة إلى مرجع_ضعيف<>"
        : "Convert one of the references in the cycle to Weak<>";
}

std::optional<ReferenceEdge> CycleDetector::suggestWeakEdge(const DetectedCycle& cycle) const {
    // نبحث عن أفضل حافة لتحويلها
    // عادةً نختار المرجع "العكسي" (مثل: ابن -> أب)
    
    for (const auto& edge : cycle.edges) {
        // إذا كان اسم الحقل يدل على علاقة عكسية
        if (edge.fieldName.find("أب") != std::string::npos ||
            edge.fieldName.find("parent") != std::string::npos ||
            edge.fieldName.find("owner") != std::string::npos ||
            edge.fieldName.find("مالك") != std::string::npos) {
            return edge;
        }
    }
    
    // إذا لم نجد، نقترح الحافة الأخيرة (التي تغلق الدورة)
    if (!cycle.edges.empty()) {
        return cycle.edges.back();
    }
    
    return std::nullopt;
}

// =============================================================================
//                    الإعدادات
// =============================================================================

void CycleDetector::setOptions(const CycleDetectorOptions& options) {
    options_ = options;
}

const CycleDetectorOptions& CycleDetector::getOptions() const {
    return options_;
}

void CycleDetector::setOnCycleDetected(std::function<void(const DetectedCycle&)> callback) {
    onCycleDetected_ = callback;
}

// =============================================================================
//                    أدوات مساعدة داخلية
// =============================================================================

bool CycleDetector::isNodeInCycle(
    const std::string& nodeId,
    const std::vector<std::string>& cycle
) const {
    return std::find(cycle.begin(), cycle.end(), nodeId) != cycle.end();
}

DetectedCycle CycleDetector::buildCycleInfo(
    const std::vector<std::string>& path,
    const ObjectGraph& graph
) const {
    DetectedCycle cycle;
    cycle.nodeIds = path;
    cycle.cycleLength = path.size();
    cycle.hasWeakReference = false;
    cycle.isProblematic = true;
    
    // بناء الحواف
    for (size_t i = 0; i < path.size(); i++) {
        size_t nextIdx = (i + 1) % path.size();
        auto outgoing = graph.getOutgoingEdges(path[i]);
        
        for (const auto& edge : outgoing) {
            if (edge.targetId == path[nextIdx]) {
                cycle.edges.push_back(edge);
                
                if (edge.isWeak()) {
                    cycle.hasWeakReference = true;
                    cycle.isProblematic = false;
                }
                break;
            }
        }
    }
    
    // إنشاء اقتراح الإصلاح
    if (cycle.isProblematic && options_.generateSuggestions) {
        cycle.suggestedFix = generateFixSuggestion(cycle, false);
        cycle.suggestedFixArabic = generateFixSuggestion(cycle, true);
    }
    
    return cycle;
}

void CycleDetector::updateStats(const std::vector<DetectedCycle>& cycles) {
    stats_.cyclesFound = cycles.size();
    stats_.problematicCycles = 0;
    stats_.safeCycles = 0;
    stats_.strongEdges = 0;
    stats_.weakEdges = 0;
    
    for (const auto& cycle : cycles) {
        if (cycle.isProblematic) {
            stats_.problematicCycles++;
        } else {
            stats_.safeCycles++;
        }
        
        for (const auto& edge : cycle.edges) {
            if (edge.isStrong()) stats_.strongEdges++;
            if (edge.isWeak()) stats_.weakEdges++;
        }
    }
}

// =============================================================================
//                    الدوال المساعدة العامة
// =============================================================================

std::string referenceTypeToArabic(ReferenceType type) {
    switch (type) {
        case ReferenceType::Strong:  return "قوي";
        case ReferenceType::Weak:    return "ضعيف";
        case ReferenceType::Raw:     return "خام";
        case ReferenceType::Unknown: return "غير معروف";
    }
    return "غير معروف";
}

std::string referenceTypeToEnglish(ReferenceType type) {
    switch (type) {
        case ReferenceType::Strong:  return "Strong";
        case ReferenceType::Weak:    return "Weak";
        case ReferenceType::Raw:     return "Raw";
        case ReferenceType::Unknown: return "Unknown";
    }
    return "Unknown";
}

std::string getCycleExplanation(bool arabic) {
    if (arabic) {
        return R"(
  ┌─────────────────────────────────────────────────────────────────────────────┐
  │                         📚 شرح دورات المراجع                               │
  ├─────────────────────────────────────────────────────────────────────────────┤
  │                                                                             │
  │  ما هي دورة المراجع؟                                                       │
  │  ───────────────────                                                        │
  │  تحدث عندما تشير مجموعة كائنات لبعضها البعض بشكل دائري:                    │
  │                                                                             │
  │      أ ──→ ب ──→ ج                                                          │
  │      ↑           │                                                          │
  │      └───────────┘                                                          │
  │                                                                             │
  │  لماذا هي مشكلة؟                                                            │
  │  ────────────────                                                           │
  │  في نظام عدّ المراجع (Reference Counting)، لن يصل عدّاد أي                  │
  │  كائن في الدورة للصفر، لأن كل كائن يُشير إليه كائن آخر.                    │
  │  هذا يسبب تسرب ذاكرة - الكائنات لا تُحرَّر أبداً!                           │
  │                                                                             │
  │  كيف نحلها؟                                                                 │
  │  ──────────                                                                 │
  │  استخدم مرجع_ضعيف<> (Weak) لأحد المراجع في الدورة:                         │
  │                                                                             │
  │      أ ══→ ب ══→ ج    (══ = قوي، ─ ─ = ضعيف)                              │
  │      ↑           ┊                                                          │
  │      └ ─ ─ ─ ─ ─ ┘                                                          │
  │                                                                             │
  │  المرجع الضعيف لا يُحتسب في العدّاد، مما يسمح بكسر الدورة.                 │
  │                                                                             │
  │  مثال عملي:                                                                 │
  │  ─────────────                                                              │
  │    صنف عقدة                                                                 │
  │      أبناء: مصفوفة<عم<عقدة>>   // الأب يملك الأبناء                         │
  │      أب: مرجع_ضعيف<عقدة>       // الابن يشير للأب بدون ملكية               │
  │    نهاية                                                                    │
  │                                                                             │
  └─────────────────────────────────────────────────────────────────────────────┘
)";
    } else {
        return R"(
  ┌─────────────────────────────────────────────────────────────────────────────┐
  │                      Reference Cycles Explained                             │
  ├─────────────────────────────────────────────────────────────────────────────┤
  │                                                                             │
  │  What is a reference cycle?                                                 │
  │  When objects reference each other in a circular pattern:                   │
  │      A --> B --> C --> A                                                    │
  │                                                                             │
  │  Why is it a problem?                                                       │
  │  In reference counting, no object's count reaches zero.                     │
  │  Memory leak - objects are never freed!                                     │
  │                                                                             │
  │  How to fix?                                                                │
  │  Use Weak<> for one reference in the cycle.                                 │
  │  Weak references don't count, breaking the cycle.                           │
  │                                                                             │
  └─────────────────────────────────────────────────────────────────────────────┘
)";
    }
}

} // namespace Memory
} // namespace Sad
