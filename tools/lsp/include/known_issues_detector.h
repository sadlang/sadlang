// بسم الله الرحمن الرحيم
// ══════════════════════════════════════════════════════════════════════════════
// ملف: known_issues_detector.h
// الوصف: واجهة كاشف المشاكل المعروفة
// ══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "lsp_protocol_types.h"  // Diagnostic, DocumentUri

#include <string>
#include <vector>

namespace sad {
namespace lsp {

/// كاشف المشاكل المعروفة والموثقة في مشاكل.md
/// يُستدعى من AnalysisPipeline لإضافة تحذيرات للأنماط الإشكالية
///
/// المشاكل المكتشفة:
/// - P1:  array[i]["key"] داخل دالة
/// - P2:  متغير x = x + 1 داخل بينما
/// - P3:  .الطول() في عمق ≥ 2
/// - P9:  كلمات محجوزة غير موثقة (نوع، حجم)
/// - P12: القسمة كفهرس مصفوفة
/// - P17: كلمات محجوزة (جديد، سجل)
/// - P18: هذا.method() داخل method
/// - P19: hex literals
/// - P20: أساس كمعامل
/// - P21: قالب كمعامل
std::vector<Diagnostic> detect_known_issues(const std::string& content, const DocumentUri& uri);

} // namespace lsp
} // namespace sad
