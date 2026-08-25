// ============================================================================
// (AR) error_messages_generated.h — كتالوج رسائل الأخطاء المولَّد آلياً
// (EN) error_messages_generated.h — Auto-generated error messages catalog
// ----------------------------------------------------------------------------
// (AR) ⚠️ لا تُعدِّل هذا الملف يدوياً — عدِّل language-truth/errors/*.yaml
//          ثم أعد البناء (CMake يُشغّل gen_error_messages.py تلقائياً).
// (EN) ⚠️ DO NOT EDIT — modify language-truth/errors/*.yaml then rebuild.
// ----------------------------------------------------------------------------
// (AR) عدد الإدخالات: 181 | إصدار YAML: 5.0
// ============================================================================

#pragma once

#include "error_codes.h"
#include <cstddef>
#include <string_view>

namespace Sad::Errors::Generated {

/**
 * @brief (AR) إدخال مولَّد لكل ErrorCode — نصوص ثنائية اللغة + معاجل ميتاداتا.
 *        (EN) Generated entry per ErrorCode — bilingual text + metadata.
 *
 * @note (AR) نص فارغ يُمَثَّل بـ nullptr (راجع PRD D-3).
 *       (EN) Empty text is represented as nullptr (see PRD D-3).
 */
struct GeneratedErrorEntry {
    ErrorCode    code;
    const char*  id;          ///< مثل "SEM002"
    const char*  category;    ///< مثل "semantic"
    const char*  titleAr;     const char* titleEn;
    const char*  briefAr;     const char* briefEn;
    const char*  detailedAr;  const char* detailedEn;
    const char*  teacherAr;   const char* teacherEn;
    const char*  fixHintAr;   const char* fixHintEn;
    const char*  codeExample;
};

/// (AR) مصفوفة كل الأخطاء — مرتبة بـ (category بالترتيب الرسمي ثم id تصاعدياً).
extern const GeneratedErrorEntry kErrorMessages[];

/// (AR) عدد العناصر في kErrorMessages.
extern const std::size_t kErrorMessagesCount;

/// (AR) بحث O(1) عن إدخال بـErrorCode — يُرجع nullptr إذا غير موجود.
/// (EN) O(1) lookup by ErrorCode — returns nullptr if not found.
const GeneratedErrorEntry* findByCode(ErrorCode code);

/// (AR) بحث بـid نصي (مثال: "SEM002") — يُرجع nullptr إذا غير موجود.
const GeneratedErrorEntry* findById(std::string_view id);

} // namespace Sad::Errors::Generated
