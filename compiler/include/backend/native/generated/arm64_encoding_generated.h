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
    { "اجمع", "x, x, imm12", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 23, 34, true, -1 }, { 22, 22, 0, true, -1 }, { 21, 10, 0, false, 2 }, { 9, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "نداء_نظام", "", sad::native::arm64::EncSpec{ 32, { { 31, 21, 1696, true, -1 }, { 20, 5, 0, true, -1 }, { 4, 0, 1, true, -1 } } } },
    { "ارجع", "", sad::native::arm64::EncSpec{ 32, { { 31, 10, 3512256, true, -1 }, { 9, 5, 30, true, -1 }, { 4, 0, 0, true, -1 } } } },
    { "نادِ", "rel26", sad::native::arm64::EncSpec{ 32, { { 31, 26, 37, true, -1 }, { 25, 0, 0, false, 0 } } } },
    { "ثبّت", "x, imm16, lsl", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 29, 3, true, -1 }, { 28, 23, 37, true, -1 }, { 22, 21, 0, false, 2 }, { 20, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "اطرح", "x, x, x", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 21, 600, true, -1 }, { 20, 16, 0, false, 2 }, { 15, 10, 0, true, -1 }, { 9, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "اطرح", "sp, imm12", sad::native::arm64::EncSpec{ 32, { { 31, 23, 418, true, -1 }, { 22, 22, 0, true, -1 }, { 21, 10, 0, false, 0 }, { 9, 5, 31, true, -1 }, { 4, 0, 31, true, -1 } } } },
    { "اطرح", "x, x, imm12", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 23, 162, true, -1 }, { 22, 22, 0, true, -1 }, { 21, 10, 0, false, 2 }, { 9, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "اضرب", "x, x, x", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 21, 216, true, -1 }, { 20, 16, 0, false, 2 }, { 15, 15, 0, true, -1 }, { 14, 10, 31, true, -1 }, { 9, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "اقسم", "x, x, x", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 21, 214, true, -1 }, { 20, 16, 0, false, 2 }, { 15, 10, 3, true, -1 }, { 9, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "اطرح_الضرب", "x, x, x, x", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 21, 216, true, -1 }, { 20, 16, 0, false, 2 }, { 15, 15, 1, true, -1 }, { 14, 10, 0, false, 3 }, { 9, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "انسخ", "x, x", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 21, 336, true, -1 }, { 20, 16, 0, false, 1 }, { 15, 10, 0, true, -1 }, { 9, 5, 31, true, -1 }, { 4, 0, 0, false, 0 } } } },
    { "قارن", "x, x", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 21, 856, true, -1 }, { 20, 16, 0, false, 1 }, { 15, 10, 0, true, -1 }, { 9, 5, 0, false, 0 }, { 4, 0, 31, true, -1 } } } },
    { "اقفز_إذا_ساوى", "rel19", sad::native::arm64::EncSpec{ 32, { { 31, 24, 84, true, -1 }, { 23, 5, 0, false, 0 }, { 4, 0, 0, true, -1 } } } },
    { "اقفز_إذا_لم_يساوِ", "rel19", sad::native::arm64::EncSpec{ 32, { { 31, 24, 84, true, -1 }, { 23, 5, 0, false, 0 }, { 4, 0, 1, true, -1 } } } },
    { "اقفز_إذا_أصغر", "rel19", sad::native::arm64::EncSpec{ 32, { { 31, 24, 84, true, -1 }, { 23, 5, 0, false, 0 }, { 4, 0, 11, true, -1 } } } },
    { "اقفز_إذا_أصغر_أو_ساوى", "rel19", sad::native::arm64::EncSpec{ 32, { { 31, 24, 84, true, -1 }, { 23, 5, 0, false, 0 }, { 4, 0, 13, true, -1 } } } },
    { "اقفز_إذا_أكبر", "rel19", sad::native::arm64::EncSpec{ 32, { { 31, 24, 84, true, -1 }, { 23, 5, 0, false, 0 }, { 4, 0, 12, true, -1 } } } },
    { "اقفز_إذا_أكبر_أو_ساوى", "rel19", sad::native::arm64::EncSpec{ 32, { { 31, 24, 84, true, -1 }, { 23, 5, 0, false, 0 }, { 4, 0, 10, true, -1 } } } },
    { "اقفز", "rel26", sad::native::arm64::EncSpec{ 32, { { 31, 26, 5, true, -1 }, { 25, 0, 0, false, 0 } } } },
    { "اخزن", "x, sp, imm12", sad::native::arm64::EncSpec{ 32, { { 31, 22, 996, true, -1 }, { 21, 10, 0, false, 1 }, { 9, 5, 31, true, -1 }, { 4, 0, 0, false, 0 } } } },
    { "احمل", "x, sp, imm12", sad::native::arm64::EncSpec{ 32, { { 31, 22, 997, true, -1 }, { 21, 10, 0, false, 1 }, { 9, 5, 31, true, -1 }, { 4, 0, 0, false, 0 } } } },
    { "اخزن_بايت", "w, x", sad::native::arm64::EncSpec{ 32, { { 31, 22, 228, true, -1 }, { 21, 10, 0, true, -1 }, { 9, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    { "اقفز_إن_ليس_صفرًا", "x, rel19", sad::native::arm64::EncSpec{ 32, { { 31, 31, 1, true, -1 }, { 30, 24, 53, true, -1 }, { 23, 5, 0, false, 1 }, { 4, 0, 0, false, 0 } } } },
    };
    return kTable;
}

// (AR) ثوابتُ أسماءِ التعليمات (منمنمات) المولَّدة من SoT — تُستهلَك في المُخفِّض
//      بدلَ تأليفِ السلسلةِ العربيّةِ يدويًّا (منعُ انجرافِ الاسم عن مصدر الحقيقة).
// (EN) Instruction-name constants generated from SoT — consumed by the lowering
//      pass instead of hand-authored Arabic strings (prevents name drift).
namespace mnem {
inline const std::string kMovz = "انقل";
inline const std::string kAdd = "اجمع";
inline const std::string kSvc = "نداء_نظام";
inline const std::string kRet = "ارجع";
inline const std::string kBl = "نادِ";
inline const std::string kMovk = "ثبّت";
inline const std::string kSub = "اطرح";
inline const std::string kMul = "اضرب";
inline const std::string kSdiv = "اقسم";
inline const std::string kMsub = "اطرح_الضرب";
inline const std::string kMov = "انسخ";
inline const std::string kCmp = "قارن";
inline const std::string kBeq = "اقفز_إذا_ساوى";
inline const std::string kBne = "اقفز_إذا_لم_يساوِ";
inline const std::string kBlt = "اقفز_إذا_أصغر";
inline const std::string kBle = "اقفز_إذا_أصغر_أو_ساوى";
inline const std::string kBgt = "اقفز_إذا_أكبر";
inline const std::string kBge = "اقفز_إذا_أكبر_أو_ساوى";
inline const std::string kB = "اقفز";
inline const std::string kStr = "اخزن";
inline const std::string kLdr = "احمل";
inline const std::string kStrb = "اخزن_بايت";
inline const std::string kCbnz = "اقفز_إن_ليس_صفرًا";
} // namespace mnem

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
