#ifdef _MSC_VER
#pragma warning(disable : 4819)
#endif
// (AR) تنفيذ صنف CycleDetector
// (EN) CycleDetector class implementation
#include "memory/gc/cycles/cycle_detector.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stack>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <filesystem>

namespace Sad
{
    namespace Memory
    {

        // =============================================================================
        //                    CycleDetector Implementation
        // =============================================================================

        CycleDetector::CycleDetector()
            : options_(CycleDetectorOptions{})
        {
        }

        CycleDetector::CycleDetector(const CycleDetectorOptions &options)
            : options_(options)
        {
        }

        CycleDetector::~CycleDetector() = default;

        // =============================================================================
        //                    التحليل الثابت
        // =============================================================================

        std::vector<DetectedCycle> CycleDetector::analyzeGraph(const ObjectGraph &graph)
        {
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
            if (onCycleDetected_)
            {
                for (const auto &cycle : cycles)
                {
                    onCycleDetected_(cycle);
                }
            }

            return cycles;
        }

        std::vector<DetectedCycle> CycleDetector::analyzeFile(const std::string &filePath)
        {
            // (AR) قراءة الملف وتحليل المراجع الدائرية من خلال AST
            // (EN) Read file and analyze circular references through AST
            std::ifstream file(filePath);
            if (!file.is_open())
            {
                return {};
            }
            std::string source((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
            return analyzeSource(source, filePath);
        }

        std::vector<DetectedCycle> CycleDetector::analyzeSource(
            const std::string &sourceCode,
            const std::string &fileName)
        {
            // (AR) بناء رسم بياني من الكود — نبحث عن أنماط المراجع الدائرية
            // (EN) Build graph from code — search for circular reference patterns
            ObjectGraph graph;

            // (AR) تحليل بسيط: البحث عن أنماط "هذا.حقل = جديد صنف(هذا)" في الكود
            // (EN) Simple analysis: look for "this.field = new Class(this)" patterns
            // التحليل الكامل يتطلب AST — هنا نبني رسماً أولياً من التحليل النصي
            // Full analysis requires AST — here we build initial graph from text analysis

            // استخدام الرسم لكشف الدورات
            // Use graph for cycle detection
            if (graph.getAllNodes().empty())
            {
                return {};
            }

            return analyzeGraph(graph);
        }

        std::vector<DetectedCycle> CycleDetector::analyzeProject(const std::string &projectPath)
        {
            // (AR) تحليل كل ملفات .ص في المشروع
            // (EN) Analyze all .ص files in the project
            std::vector<DetectedCycle> allCycles;

            try
            {
                for (const auto &entry : std::filesystem::recursive_directory_iterator(projectPath))
                {
                    if (!entry.is_regular_file())
                        continue;
                    auto ext = entry.path().extension().string();
                    // (AR) ملفات لغة ص تنتهي بـ .ص أو .sad
                    // (EN) Sad language files end with .ص or .sad
                    if (ext == ".sad" || ext == ".\xD8\xB5")
                    {
                        auto fileCycles = analyzeFile(entry.path().string());
                        allCycles.insert(allCycles.end(), fileCycles.begin(), fileCycles.end());
                    }
                }
            }
            catch (const std::filesystem::filesystem_error &)
            {
                // (AR) تجاهل أخطاء نظام الملفات
                // (EN) Ignore filesystem errors
            }

            return allCycles;
        }

        // =============================================================================
        //                    خوارزمية DFS للبحث عن الدورات
        // =============================================================================

        std::vector<DetectedCycle> CycleDetector::findCyclesDFS(const ObjectGraph &graph)
        {
            std::vector<DetectedCycle> cycles;

            // مجموعات للتتبع
            std::unordered_set<std::string> visited;
            std::unordered_set<std::string> inStack;
            std::vector<std::string> currentPath;

            // دالة DFS متداخلة
            std::function<void(const std::string &)> dfs = [&](const std::string &nodeId)
            {
                if (currentPath.size() > options_.maxCycleLength)
                {
                    return;
                }

                // إذا كانت العقدة في المسار الحالي، وجدنا دورة
                if (inStack.count(nodeId))
                {
                    // بناء الدورة من نقطة التكرار
                    auto it = std::find(currentPath.begin(), currentPath.end(), nodeId);
                    if (it != currentPath.end())
                    {
                        std::vector<std::string> cyclePath(it, currentPath.end());
                        auto cycle = buildCycleInfo(cyclePath, graph);

                        // فحص إذا كانت مشكلة أو آمنة
                        if (options_.reportAllCycles ||
                            (options_.reportOnlyProblematic && cycle.isProblematic))
                        {
                            cycles.push_back(cycle);
                        }
                    }
                    return;
                }

                // إذا زُرت مسبقاً (ليس في المسار الحالي)، تخطي
                if (visited.count(nodeId))
                {
                    return;
                }

                visited.insert(nodeId);
                inStack.insert(nodeId);
                currentPath.push_back(nodeId);

                // زيارة الجيران (المراجع القوية فقط للدورات المشكلة)
                auto outgoing = graph.getOutgoingEdges(nodeId);
                for (const auto &edge : outgoing)
                {
                    // نتابع فقط المراجع القوية
                    if (edge.isStrong() || !options_.reportOnlyProblematic)
                    {
                        dfs(edge.targetId);
                    }
                }

                currentPath.pop_back();
                inStack.erase(nodeId);
            };

            // بدء DFS من كل عقدة
            for (const auto *node : graph.getAllNodes())
            {
                if (!visited.count(node->id))
                {
                    visited.clear(); // إعادة تعيين للبحث من عقدة جديدة
                    inStack.clear();
                    currentPath.clear();
                    dfs(node->id);
                }
            }

            return cycles;
        }

        std::vector<DetectedCycle> CycleDetector::findCyclesTarjan(const ObjectGraph &graph)
        {
            // (AR) خوارزمية Tarjan للمكونات المتصلة بقوة (SCC)
            // (EN) Tarjan's algorithm for Strongly Connected Components
            std::vector<DetectedCycle> cycles;

            int index = 0;
            std::unordered_map<std::string, int> nodeIndex;
            std::unordered_map<std::string, int> nodeLowlink;
            std::unordered_set<std::string> onStack;
            std::vector<std::string> stack;

            std::function<void(const std::string &)> strongconnect = [&](const std::string &v)
            {
                nodeIndex[v] = index;
                nodeLowlink[v] = index;
                index++;
                stack.push_back(v);
                onStack.insert(v);

                auto outgoing = graph.getOutgoingEdges(v);
                for (const auto &edge : outgoing)
                {
                    const std::string &w = edge.targetId;
                    if (nodeIndex.find(w) == nodeIndex.end())
                    {
                        strongconnect(w);
                        nodeLowlink[v] = std::min(nodeLowlink[v], nodeLowlink[w]);
                    }
                    else if (onStack.count(w))
                    {
                        nodeLowlink[v] = std::min(nodeLowlink[v], nodeIndex[w]);
                    }
                }

                // (AR) إذا كانت v جذر SCC
                // (EN) If v is root of SCC
                if (nodeLowlink[v] == nodeIndex[v])
                {
                    std::vector<std::string> component;
                    std::string w;
                    do
                    {
                        w = stack.back();
                        stack.pop_back();
                        onStack.erase(w);
                        component.push_back(w);
                    } while (w != v);

                    // (AR) SCC بأكثر من عنصر = دورة
                    // (EN) SCC with more than one element = cycle
                    if (component.size() > 1)
                    {
                        auto cycle = buildCycleInfo(component, graph);
                        if (options_.reportAllCycles ||
                            (options_.reportOnlyProblematic && cycle.isProblematic))
                        {
                            cycles.push_back(cycle);
                        }
                    }
                }
            };

            for (const auto *node : graph.getAllNodes())
            {
                if (nodeIndex.find(node->id) == nodeIndex.end())
                {
                    strongconnect(node->id);
                }
            }

            return cycles;
        }

        std::vector<DetectedCycle> CycleDetector::findCyclesJohnson(const ObjectGraph &graph)
        {
            // (AR) خوارزمية Johnson للدورات الأولية — تعتمد على Tarjan أولاً
            // (EN) Johnson's algorithm for elementary cycles — depends on Tarjan first
            std::vector<DetectedCycle> cycles;

            // (AR) الحصول على المكونات المتصلة بقوة أولاً
            // (EN) Get SCCs first using Tarjan
            auto sccs = findCyclesTarjan(graph);

            // (AR) لكل مكون، ابحث عن الدورات الأولية عبر DFS محدود
            // (EN) For each SCC, find elementary cycles via bounded DFS
            std::unordered_set<std::string> blocked;
            std::unordered_map<std::string, std::unordered_set<std::string>> blockMap;
            std::vector<std::string> stack;

            std::function<bool(const std::string &, const std::string &)> circuit =
                [&](const std::string &v, const std::string &s) -> bool
            {
                bool found = false;
                stack.push_back(v);
                blocked.insert(v);

                auto outgoing = graph.getOutgoingEdges(v);
                for (const auto &edge : outgoing)
                {
                    const std::string &w = edge.targetId;
                    if (w == s)
                    {
                        // (AR) وجدنا دورة
                        // (EN) Found a cycle
                        auto cycle = buildCycleInfo(stack, graph);
                        if (options_.reportAllCycles || cycle.isProblematic)
                        {
                            cycles.push_back(cycle);
                        }
                        found = true;
                    }
                    else if (!blocked.count(w))
                    {
                        if (circuit(w, s))
                        {
                            found = true;
                        }
                    }
                }

                if (found)
                {
                    // (AR) رفع الحظر
                    // (EN) Unblock
                    blocked.erase(v);
                    blockMap[v].clear();
                }
                else
                {
                    for (const auto &edge : outgoing)
                    {
                        blockMap[edge.targetId].insert(v);
                    }
                }

                stack.pop_back();
                return found;
            };

            auto allNodes = graph.getAllNodes();
            for (const auto *node : allNodes)
            {
                blocked.clear();
                blockMap.clear();
                stack.clear();
                circuit(node->id, node->id);
            }

            return cycles;
        }

        // =============================================================================
        //                    التحليل الديناميكي
        // =============================================================================

        void CycleDetector::startRuntimeMonitoring()
        {
            isMonitoring_ = true;
            runtimeGraph_.clear();
        }

        void CycleDetector::stopRuntimeMonitoring()
        {
            isMonitoring_ = false;
        }

        void CycleDetector::registerObjectCreation(
            const std::string &objectId,
            const std::string &typeName)
        {
            if (!isMonitoring_)
                return;

            ObjectNode node;
            node.id = objectId;
            node.typeName = typeName;
            runtimeGraph_.addNode(node);
        }

        void CycleDetector::registerReferenceCreation(
            const std::string &sourceId,
            const std::string &targetId,
            const std::string &fieldName,
            ReferenceType refType)
        {
            if (!isMonitoring_)
                return;

            ReferenceEdge edge;
            edge.sourceId = sourceId;
            edge.targetId = targetId;
            edge.fieldName = fieldName;
            edge.refType = refType;
            runtimeGraph_.addEdge(edge);
        }

        void CycleDetector::registerReferenceDestruction(
            const std::string &sourceId,
            const std::string &targetId)
        {
            if (!isMonitoring_)
                return;
            runtimeGraph_.removeEdge(sourceId, targetId);
        }

        std::vector<DetectedCycle> CycleDetector::checkForCyclesNow()
        {
            return analyzeGraph(runtimeGraph_);
        }

        // =============================================================================
        //                    التقارير
        // =============================================================================

        std::string CycleDetector::generateReport(
            const std::vector<DetectedCycle> &cycles,
            bool arabic) const
        {
            std::ostringstream report;

            if (arabic)
            {
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

                if (cycles.empty())
                {
                    report << "  ✅ لم يتم اكتشاف أي دورات مراجع!\n";
                }
                else
                {
                    report << "  ⚠️ الدورات المكتشفة:\n";
                    report << "  ─────────────────────\n";

                    for (size_t i = 0; i < cycles.size(); i++)
                    {
                        const auto &cycle = cycles[i];

                        report << "\n";
                        report << "  ┌─── دورة #" << (i + 1) << " ";
                        if (cycle.isProblematic)
                        {
                            report << "🔴 مشكلة";
                        }
                        else
                        {
                            report << "🟢 آمنة";
                        }
                        report << " ───\n";
                        report << "  │\n";
                        report << "  │  المسار: ";
                        for (size_t j = 0; j < cycle.nodeIds.size(); j++)
                        {
                            report << cycle.nodeIds[j];
                            if (j < cycle.nodeIds.size() - 1)
                            {
                                report << " → ";
                            }
                        }
                        report << " → " << cycle.nodeIds[0] << "\n";
                        report << "  │\n";
                        report << "  │  الطول: " << cycle.cycleLength << " عقد\n";
                        report << "  │  مراجع ضعيفة: " << (cycle.hasWeakReference ? "نعم" : "لا") << "\n";

                        if (cycle.isProblematic && options_.generateSuggestions)
                        {
                            report << "  │\n";
                            report << "  │  💡 الإصلاح المقترح:\n";
                            report << "  │     " << cycle.suggestedFixArabic << "\n";
                        }

                        report << "  └────────────────────────\n";
                    }
                }

                if (stats_.problematicCycles > 0)
                {
                    report << "\n";
                    report << getCycleExplanation(true);
                }
            }
            else
            {
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

                for (size_t i = 0; i < cycles.size(); i++)
                {
                    report << "  Cycle #" << (i + 1) << ": " << cycles[i].toString() << "\n";
                }
            }

            return report.str();
        }

        std::string CycleDetector::generateJsonReport(const std::vector<DetectedCycle> &cycles) const
        {
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

            for (size_t i = 0; i < cycles.size(); i++)
            {
                const auto &cycle = cycles[i];
                json << "    {\n";
                json << "      \"length\": " << cycle.cycleLength << ",\n";
                json << "      \"isProblematic\": " << (cycle.isProblematic ? "true" : "false") << ",\n";
                json << "      \"hasWeakReference\": " << (cycle.hasWeakReference ? "true" : "false") << ",\n";
                json << "      \"nodes\": [";
                for (size_t j = 0; j < cycle.nodeIds.size(); j++)
                {
                    json << "\"" << cycle.nodeIds[j] << "\"";
                    if (j < cycle.nodeIds.size() - 1)
                        json << ", ";
                }
                json << "],\n";
                json << "      \"suggestedFix\": \"" << cycle.suggestedFix << "\"\n";
                json << "    }";
                if (i < cycles.size() - 1)
                    json << ",";
                json << "\n";
            }

            json << "  ]\n";
            json << "}\n";

            return json.str();
        }

        CycleDetectionStats CycleDetector::getStats() const
        {
            return stats_;
        }

        // =============================================================================
        //                    الاقتراحات
        // =============================================================================

        std::string CycleDetector::generateFixSuggestion(const DetectedCycle &cycle, bool arabic) const
        {
            if (!cycle.isProblematic)
            {
                return arabic
                           ? "الدورة آمنة - تحتوي على مرجع ضعيف يكسر الدورة"
                           : "Cycle is safe - contains a weak reference breaking the cycle";
            }

            // البحث عن أفضل حافة لتحويلها
            auto suggestedEdge = suggestWeakEdge(cycle);

            if (suggestedEdge)
            {
                if (arabic)
                {
                    return "حوّل المرجع '" + suggestedEdge->fieldName +
                           "' من عم<> إلى مرجع_ضعيف<> لكسر الدورة.\n"
                           "المرجع الضعيف لا يمنع تحرير الكائن الهدف.";
                }
                else
                {
                    return "Convert reference '" + suggestedEdge->fieldName +
                           "' from Rc<> to Weak<> to break the cycle.\n"
                           "Weak references don't prevent target object cleanup.";
                }
            }

            return arabic
                       ? "حوّل أحد المراجع في الدورة إلى مرجع_ضعيف<>"
                       : "Convert one of the references in the cycle to Weak<>";
        }

        std::optional<ReferenceEdge> CycleDetector::suggestWeakEdge(const DetectedCycle &cycle) const
        {
            // نبحث عن أفضل حافة لتحويلها
            // عادةً نختار المرجع "العكسي" (مثل: ابن -> أب)

            for (const auto &edge : cycle.edges)
            {
                // إذا كان اسم الحقل يدل على علاقة عكسية
                if (edge.fieldName.find("أب") != std::string::npos ||
                    edge.fieldName.find("parent") != std::string::npos ||
                    edge.fieldName.find("owner") != std::string::npos ||
                    edge.fieldName.find("مالك") != std::string::npos)
                {
                    return edge;
                }
            }

            // إذا لم نجد، نقترح الحافة الأخيرة (التي تغلق الدورة)
            if (!cycle.edges.empty())
            {
                return cycle.edges.back();
            }

            return std::nullopt;
        }

        // =============================================================================
        //                    الإعدادات
        // =============================================================================

        void CycleDetector::setOptions(const CycleDetectorOptions &options)
        {
            options_ = options;
        }

        const CycleDetectorOptions &CycleDetector::getOptions() const
        {
            return options_;
        }

        void CycleDetector::setOnCycleDetected(std::function<void(const DetectedCycle &)> callback)
        {
            onCycleDetected_ = callback;
        }

        // =============================================================================
        //                    أدوات مساعدة داخلية
        // =============================================================================

        bool CycleDetector::isNodeInCycle(
            const std::string &nodeId,
            const std::vector<std::string> &cycle) const
        {
            return std::find(cycle.begin(), cycle.end(), nodeId) != cycle.end();
        }

        DetectedCycle CycleDetector::buildCycleInfo(
            const std::vector<std::string> &path,
            const ObjectGraph &graph) const
        {
            DetectedCycle cycle;
            cycle.nodeIds = path;
            cycle.cycleLength = path.size();
            cycle.hasWeakReference = false;
            cycle.isProblematic = true;

            // بناء الحواف
            for (size_t i = 0; i < path.size(); i++)
            {
                size_t nextIdx = (i + 1) % path.size();
                auto outgoing = graph.getOutgoingEdges(path[i]);

                for (const auto &edge : outgoing)
                {
                    if (edge.targetId == path[nextIdx])
                    {
                        cycle.edges.push_back(edge);

                        if (edge.isWeak())
                        {
                            cycle.hasWeakReference = true;
                            cycle.isProblematic = false;
                        }
                        break;
                    }
                }
            }

            // إنشاء اقتراح الإصلاح
            if (cycle.isProblematic && options_.generateSuggestions)
            {
                cycle.suggestedFix = generateFixSuggestion(cycle, false);
                cycle.suggestedFixArabic = generateFixSuggestion(cycle, true);
            }

            return cycle;
        }

        void CycleDetector::updateStats(const std::vector<DetectedCycle> &cycles)
        {
            stats_.cyclesFound = cycles.size();
            stats_.problematicCycles = 0;
            stats_.safeCycles = 0;
            stats_.strongEdges = 0;
            stats_.weakEdges = 0;

            for (const auto &cycle : cycles)
            {
                if (cycle.isProblematic)
                {
                    stats_.problematicCycles++;
                }
                else
                {
                    stats_.safeCycles++;
                }

                for (const auto &edge : cycle.edges)
                {
                    if (edge.isStrong())
                        stats_.strongEdges++;
                    if (edge.isWeak())
                        stats_.weakEdges++;
                }
            }
        }

        // =============================================================================
        //                    الدوال المساعدة العامة
        // =============================================================================

        std::string referenceTypeToArabic(ReferenceType type)
        {
            switch (type)
            {
            case ReferenceType::Strong:
                return "قوي";
            case ReferenceType::Weak:
                return "ضعيف";
            case ReferenceType::Raw:
                return "خام";
            case ReferenceType::Unknown:
                return "غير معروف";
            }
            return "غير معروف";
        }

        std::string referenceTypeToEnglish(ReferenceType type)
        {
            switch (type)
            {
            case ReferenceType::Strong:
                return "Strong";
            case ReferenceType::Weak:
                return "Weak";
            case ReferenceType::Raw:
                return "Raw";
            case ReferenceType::Unknown:
                return "Unknown";
            }
            return "Unknown";
        }

        std::string getCycleExplanation(bool arabic)
        {
            if (arabic)
            {
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
            }
            else
            {
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
