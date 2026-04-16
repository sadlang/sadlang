#pragma once

/**
 * ═══════════════════════════════════════════════════════════════════════════
 * ملف: analyzer_engine.h
 * الوصف: الواجهة العامة لمحرك المحلل المتقدم للغة ص
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * هذا المحرك هو نقطة الدخول الرسمية لأي مستهلك يريد تحليل ملفات لغة ص.
 * تم إبقاؤه كواجهة نحيفة وواضحة حتى تكون مسؤوليته محددة بدقة:
 *
 * - قراءة المصدر
 * - تشغيل Lexer و Parser
 * - تحويل أخطاء التحليل النحوي إلى نتائج موحدة
 * - تشغيل طبقة التحليل الساكن على AST عند نجاح البناء النحوي
 * - إعادة نتيجة موحدة قابلة للاستهلاك من CLI أو LSP
 *
 * الهدف هنا ليس تكديس كل المنطق في هذا الصف، بل جعله منسقاً بين المراحل.
 */

#include <string>

#include "analysis_types.h"

namespace Sad::Analizer {

class AnalyzerEngine {
public:
    AnalyzerEngine() = default;

    AnalysisResult analyzeFile(const std::string& filePath,
                               const AnalyzerOptions& options = AnalyzerOptions()) const;

    AnalysisResult analyzeSource(const std::string& source,
                                 const std::string& filePath,
                                 const AnalyzerOptions& options = AnalyzerOptions()) const;
};

} // namespace Sad::Analizer
