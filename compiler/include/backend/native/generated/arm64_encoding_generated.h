// ============================================================================
// (AR) مولَّدٌ آليًّا من language-truth/backend/arm64/instructions.yaml — لا تُحرّره.
//      أعِد توليدَه بـ: python x.py gen  (وحارسُ الانجراف: python x.py gen --check).
// (EN) AUTO-GENERATED from the SoT instruction-encoding YAML. DO NOT EDIT.
//      Regenerate with: python x.py gen   (drift guard: python x.py gen --check).
// ============================================================================

#ifndef SAD_NATIVE_ARM64_ENCODING_GENERATED_H
#define SAD_NATIVE_ARM64_ENCODING_GENERATED_H

#include "backend/native/arm64_fixed32_encoder.h"

#include <string>
#include <vector>

namespace sad { namespace native { namespace arm64 {

// (AR) جدول ترميز arm64 (fixed32) المولَّد من SoT (منمنمة، صيغة، EncSpec).
struct GenEncEntry { std::string mnemonic; std::string form; EncSpec spec; };

inline const std::vector<GenEncEntry> &encodingTable()
{
    static const std::vector<GenEncEntry> kTable = {
    { "انقل", "x, imm16", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 29, 2, true, -1 }, { 28, 23, 37, true, -1 }, { 22, 21, 0, true, -1 }, { 20, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "اجمع", "x, x, x", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 21, 88, true, -1 }, { 20, 16, 0, false, 2 }, { 15, 10, 0, true, -1 }, { 9, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "نداء_نظام", "", sad::native::arm64::EncSpec{ 32, { { 31, 21, 1696, true, -1 }, { 20, 5, 0, true, -1 }, { 4, 0, 1, true, -1 } } } },
    { "ارجع", "", sad::native::arm64::EncSpec{ 32, { { 31, 10, 3512256, true, -1 }, { 9, 5, 30, true, -1 }, { 4, 0, 0, true, -1 } } } },
    };
    return kTable;
}

// (AR) بحثٌ عن مواصفة الترميز بالمنمنمة والصيغة؛ يعيد nullptr إن لم تُوجد.
// (EN) look up an encoding spec by mnemonic + form; nullptr if absent.
inline const EncSpec *lookupEncSpec(const std::string &mnemonic, const std::string &form)
{
    for (const auto &e : encodingTable())
        if (e.mnemonic == mnemonic && e.form == form)
            return &e.spec;
    return nullptr;
}

}}} // namespace sad::native::arm64

#endif // SAD_NATIVE_ARM64_ENCODING_GENERATED_H
