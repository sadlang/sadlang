// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable : 4819)
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
#include <fstream>
#include <filesystem>

namespace Sad
{
    namespace Memory
    {

        // =============================================================================
        //                    DetectedCycle Methods
        // =============================================================================

        std::string DetectedCycle::toString() const
        {
            std::ostringstream ss;
            ss << "Cycle [length=" << cycleLength << ", problematic=" << (isProblematic ? "yes" : "no") << "]: ";
            for (size_t i = 0; i < nodeIds.size(); i++)
            {
                ss << nodeIds[i];
                if (i < nodeIds.size() - 1)
                    ss << " -> ";
            }
            ss << " -> " << nodeIds[0]; // Back to start
            return ss.str();
        }

        std::string DetectedCycle::toStringArabic() const
        {
            std::ostringstream ss;
            ss << "دورة [الطول=" << cycleLength << "، مشكلة=" << (isProblematic ? "نعم" : "لا") << "]: ";
            for (size_t i = 0; i < nodeIds.size(); i++)
            {
                ss << nodeIds[i];
                if (i < nodeIds.size() - 1)
                    ss << " ← ";
            }
            ss << " ← " << nodeIds[0]; // العودة للبداية
            return ss.str();
        }

        // =============================================================================
        //                    ObjectGraph Implementation
        // =============================================================================

        ObjectGraph::ObjectGraph() = default;
        ObjectGraph::~ObjectGraph() = default;

        void ObjectGraph::addNode(const ObjectNode &node)
        {
            nodes_[node.id] = node;
        }

        void ObjectGraph::addEdge(const ReferenceEdge &edge)
        {
            edges_.push_back(edge);

            // تحديث الحواف الصادرة للعقدة المصدر
            auto it = nodes_.find(edge.sourceId);
            if (it != nodes_.end())
            {
                it->second.outgoingEdges.push_back(edge);
            }
        }

        void ObjectGraph::removeNode(const std::string &nodeId)
        {
            nodes_.erase(nodeId);

            // إزالة الحواف المتعلقة
            edges_.erase(
                std::remove_if(edges_.begin(), edges_.end(),
                               [&nodeId](const ReferenceEdge &e)
                               {
                                   return e.sourceId == nodeId || e.targetId == nodeId;
                               }),
                edges_.end());
        }

        void ObjectGraph::removeEdge(const std::string &sourceId, const std::string &targetId)
        {
            edges_.erase(
                std::remove_if(edges_.begin(), edges_.end(),
                               [&sourceId, &targetId](const ReferenceEdge &e)
                               {
                                   return e.sourceId == sourceId && e.targetId == targetId;
                               }),
                edges_.end());

            // تحديث العقدة المصدر
            auto it = nodes_.find(sourceId);
            if (it != nodes_.end())
            {
                auto &outgoing = it->second.outgoingEdges;
                outgoing.erase(
                    std::remove_if(outgoing.begin(), outgoing.end(),
                                   [&targetId](const ReferenceEdge &e)
                                   {
                                       return e.targetId == targetId;
                                   }),
                    outgoing.end());
            }
        }

        void ObjectGraph::clear()
        {
            nodes_.clear();
            edges_.clear();
        }

        const ObjectNode *ObjectGraph::getNode(const std::string &nodeId) const
        {
            auto it = nodes_.find(nodeId);
            return it != nodes_.end() ? &it->second : nullptr;
        }

        std::vector<const ObjectNode *> ObjectGraph::getAllNodes() const
        {
            std::vector<const ObjectNode *> result;
            result.reserve(nodes_.size());
            for (const auto &pair : nodes_)
            {
                result.push_back(&pair.second);
            }
            return result;
        }

        std::vector<ReferenceEdge> ObjectGraph::getIncomingEdges(const std::string &nodeId) const
        {
            std::vector<ReferenceEdge> result;
            for (const auto &edge : edges_)
            {
                if (edge.targetId == nodeId)
                {
                    result.push_back(edge);
                }
            }
            return result;
        }

        std::vector<ReferenceEdge> ObjectGraph::getOutgoingEdges(const std::string &nodeId) const
        {
            auto it = nodes_.find(nodeId);
            if (it != nodes_.end())
            {
                return it->second.outgoingEdges;
            }
            return {};
        }

        size_t ObjectGraph::getNodeCount() const
        {
            return nodes_.size();
        }

        size_t ObjectGraph::getEdgeCount() const
        {
            return edges_.size();
        }

        void ObjectGraph::dump() const
        {
            std::cout << "\n";
            std::cout << "╔═══════════════════════════════════════╗\n";
            std::cout << "║         رسم الكائنات (Object Graph)  ║\n";
            std::cout << "╚═══════════════════════════════════════╝\n";
            std::cout << "\n";
            std::cout << "  العقد (" << nodes_.size() << "):\n";
            for (const auto &pair : nodes_)
            {
                const auto &node = pair.second;
                std::cout << "    • " << node.id << " : " << node.typeName;
                if (!node.variableName.empty())
                {
                    std::cout << " (" << node.variableName << ")";
                }
                std::cout << "\n";
            }
            std::cout << "\n";
            std::cout << "  الحواف (" << edges_.size() << "):\n";
            for (const auto &edge : edges_)
            {
                std::cout << "    " << edge.sourceId << " --["
                          << edge.fieldName << "]--> " << edge.targetId;
                std::cout << " (" << referenceTypeToArabic(edge.refType) << ")\n";
            }
            std::cout << "\n";
        }

        std::string ObjectGraph::toGraphviz() const
        {
            std::ostringstream dot;
            dot << "digraph ObjectGraph {\n";
            dot << "  rankdir=LR;\n";
            dot << "  node [shape=box];\n";
            dot << "\n";

            // العقد
            for (const auto &pair : nodes_)
            {
                const auto &node = pair.second;
                dot << "  \"" << node.id << "\" [label=\"" << node.typeName;
                if (!node.variableName.empty())
                {
                    dot << "\\n"
                        << node.variableName;
                }
                dot << "\"];\n";
            }

            dot << "\n";

            // الحواف
            for (const auto &edge : edges_)
            {
                dot << "  \"" << edge.sourceId << "\" -> \"" << edge.targetId << "\"";
                dot << " [label=\"" << edge.fieldName << "\"";
                if (edge.refType == ReferenceType::Weak)
                {
                    dot << ", style=dashed, color=blue";
                }
                else if (edge.refType == ReferenceType::Strong)
                {
                    dot << ", style=bold, color=red";
                }
                dot << "];\n";
            }

            dot << "}\n";
            return dot.str();
        }

    } // namespace Memory
} // namespace Sad
