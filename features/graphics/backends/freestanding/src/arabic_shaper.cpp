/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: arabic_shaper.cpp — تنفيذ مُشكِّل النصّ العربيّ للوضع المستقلّ
 * المسار: features/graphics/backends/freestanding/src/arabic_shaper.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * جدول الأشكال أدناه مبنيّ **بعكس** الجدول المرجعيّ الموثوق في المترجم:
 * compiler/src/backend/llvm/arabic_normalization.cpp — decomposePresBForm()
 * (أشكال العرض-B ⇒ الأساس). هناك تُسرَد شيفرات كلّ أساس بترتيب مقطع
 * Unicode Presentation Forms-B نفسه: منفصل، نهائيّ، [ابتدائيّ، وسطيّ] —
 * الحروف ثنائيّة الاتّصال لها 4 أشكال، واليمينيّة الاتّصال شكلان (منفصل/نهائيّ)،
 * والهمزة شكل واحد. لا نقطة هنا غير مقابَلة بذلك الجدول.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص — رخصة MIT
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/freestanding/arabic_shaper.h"

#include <algorithm>
#include <cstddef>

namespace sad
{
    namespace ui
    {
        namespace freestanding
        {
            namespace arabic
            {
                namespace
                {

                    /// (AR) صنف اتّصال الحرف (المذكّرة التصميميّة — ثلاثة أصناف فقط)
                    enum class Joining : uint8_t
                    {
                        None,  ///< لا يتّصل (ء)
                        Right, ///< يتّصل يمينًا فقط: شكلان (منفصل/نهائيّ)
                        Dual   ///< يتّصل بالجهتين: أربعة أشكال
                    };

                    /// (AR) سطر جدول الأشكال: أساس ⇒ (منفصل، نهائيّ، ابتدائيّ، وسطيّ)
                    /// الشكلان الأخيران صفر لغير ثنائيّ الاتّصال.
                    struct ShapeEntry
                    {
                        uint32_t base;
                        Joining joining;
                        uint32_t isolated;
                        uint32_t final;
                        uint32_t initial;
                        uint32_t medial;
                    };

                    // الجدول الأماميّ المعكوس عن decomposePresBForm (المرجع أعلاه).
                    // أصناف الاتّصال من المذكّرة: يمينيّ = ا د ذ ر ز و أ إ آ ؤ ة ى؛
                    // لا يتّصل = ء؛ والبقيّة ثنائيّ.
                    constexpr ShapeEntry SHAPE_TABLE[] = {
                        {0x0621, Joining::None, 0xFE80, 0, 0, 0},           // ء
                        {0x0622, Joining::Right, 0xFE81, 0xFE82, 0, 0},      // آ
                        {0x0623, Joining::Right, 0xFE83, 0xFE84, 0, 0},      // أ
                        {0x0624, Joining::Right, 0xFE85, 0xFE86, 0, 0},      // ؤ
                        {0x0625, Joining::Right, 0xFE87, 0xFE88, 0, 0},      // إ
                        {0x0626, Joining::Dual, 0xFE89, 0xFE8A, 0xFE8B, 0xFE8C}, // ئ
                        {0x0627, Joining::Right, 0xFE8D, 0xFE8E, 0, 0},      // ا
                        {0x0628, Joining::Dual, 0xFE8F, 0xFE90, 0xFE91, 0xFE92}, // ب
                        {0x0629, Joining::Right, 0xFE93, 0xFE94, 0, 0},      // ة
                        {0x062A, Joining::Dual, 0xFE95, 0xFE96, 0xFE97, 0xFE98}, // ت
                        {0x062B, Joining::Dual, 0xFE99, 0xFE9A, 0xFE9B, 0xFE9C}, // ث
                        {0x062C, Joining::Dual, 0xFE9D, 0xFE9E, 0xFE9F, 0xFEA0}, // ج
                        {0x062D, Joining::Dual, 0xFEA1, 0xFEA2, 0xFEA3, 0xFEA4}, // ح
                        {0x062E, Joining::Dual, 0xFEA5, 0xFEA6, 0xFEA7, 0xFEA8}, // خ
                        {0x062F, Joining::Right, 0xFEA9, 0xFEAA, 0, 0},      // د
                        {0x0630, Joining::Right, 0xFEAB, 0xFEAC, 0, 0},      // ذ
                        {0x0631, Joining::Right, 0xFEAD, 0xFEAE, 0, 0},      // ر
                        {0x0632, Joining::Right, 0xFEAF, 0xFEB0, 0, 0},      // ز
                        {0x0633, Joining::Dual, 0xFEB1, 0xFEB2, 0xFEB3, 0xFEB4}, // س
                        {0x0634, Joining::Dual, 0xFEB5, 0xFEB6, 0xFEB7, 0xFEB8}, // ش
                        {0x0635, Joining::Dual, 0xFEB9, 0xFEBA, 0xFEBB, 0xFEBC}, // ص
                        {0x0636, Joining::Dual, 0xFEBD, 0xFEBE, 0xFEBF, 0xFEC0}, // ض
                        {0x0637, Joining::Dual, 0xFEC1, 0xFEC2, 0xFEC3, 0xFEC4}, // ط
                        {0x0638, Joining::Dual, 0xFEC5, 0xFEC6, 0xFEC7, 0xFEC8}, // ظ
                        {0x0639, Joining::Dual, 0xFEC9, 0xFECA, 0xFECB, 0xFECC}, // ع
                        {0x063A, Joining::Dual, 0xFECD, 0xFECE, 0xFECF, 0xFED0}, // غ
                        // U+063B–U+0640 خارج الجدول (فجوة Unicode + التطويل — حدّ معلَن)
                        {0x0641, Joining::Dual, 0xFED1, 0xFED2, 0xFED3, 0xFED4}, // ف
                        {0x0642, Joining::Dual, 0xFED5, 0xFED6, 0xFED7, 0xFED8}, // ق
                        {0x0643, Joining::Dual, 0xFED9, 0xFEDA, 0xFEDB, 0xFEDC}, // ك
                        {0x0644, Joining::Dual, 0xFEDD, 0xFEDE, 0xFEDF, 0xFEE0}, // ل
                        {0x0645, Joining::Dual, 0xFEE1, 0xFEE2, 0xFEE3, 0xFEE4}, // م
                        {0x0646, Joining::Dual, 0xFEE5, 0xFEE6, 0xFEE7, 0xFEE8}, // ن
                        {0x0647, Joining::Dual, 0xFEE9, 0xFEEA, 0xFEEB, 0xFEEC}, // ه
                        {0x0648, Joining::Right, 0xFEED, 0xFEEE, 0, 0},      // و
                        {0x0649, Joining::Right, 0xFEEF, 0xFEF0, 0, 0},      // ى
                        {0x064A, Joining::Dual, 0xFEF1, 0xFEF2, 0xFEF3, 0xFEF4}, // ي
                    };

                    /// (AR) لام — بادئة ليغاتورة لام-ألف
                    constexpr uint32_t LAM = 0x0644;

                    /// (AR) ليغاتورات لام-ألف (منفصلة/نهائيّة) — من نفس الجدول المرجعيّ:
                    /// FEF5/FEF6 = لآ، FEF7/FEF8 = لأ، FEF9/FEFA = لإ، FEFB/FEFC = لا
                    struct LamAlefEntry
                    {
                        uint32_t alef;
                        uint32_t isolated;
                        uint32_t final;
                    };
                    constexpr LamAlefEntry LAM_ALEF_TABLE[] = {
                        {0x0622, 0xFEF5, 0xFEF6}, // لآ
                        {0x0623, 0xFEF7, 0xFEF8}, // لأ
                        {0x0625, 0xFEF9, 0xFEFA}, // لإ
                        {0x0627, 0xFEFB, 0xFEFC}, // لا
                    };

                    // بحث خطّيّ مقصود: الجدول 36 مُدخَلًا ثابتًا والنداء لكلّ حرف عرضٍ —
                    // لا يُذكر أداءً هنا. لا تُحسِّنه (خريطة/فرز) بلا قياس، ولا تنسخه
                    // لنطاق أثقل (نصوص طويلة/جداول أكبر) بلا مراجعة البنية.
                    const ShapeEntry *findEntry(uint32_t cp)
                    {
                        for (const auto &e : SHAPE_TABLE)
                            if (e.base == cp)
                                return &e;
                        return nullptr;
                    }

                    const LamAlefEntry *findLamAlef(uint32_t alef)
                    {
                        for (const auto &e : LAM_ALEF_TABLE)
                            if (e.alef == alef)
                                return &e;
                        return nullptr;
                    }

                    /// (AR) هل يتّصل هذا الحرف بما بعده (يسارًا)؟ — ثنائيّ الاتّصال فقط
                    bool joinsForward(uint32_t cp)
                    {
                        const ShapeEntry *e = findEntry(cp);
                        return e != nullptr && e->joining == Joining::Dual;
                    }

                    /// (AR) هل يقبل هذا الحرف اتّصالًا ممّا قبله (يمينًا)؟
                    bool joinsBackward(uint32_t cp)
                    {
                        const ShapeEntry *e = findEntry(cp);
                        return e != nullptr && e->joining != Joining::None;
                    }

                    /// (AR) نقطة عربيّة عرضيًّا؟ (الكتلة الأساس أو مقطعا أشكال العرض)
                    bool isArabicDisplay(uint32_t cp)
                    {
                        return (cp >= ARABIC_BLOCK_FIRST && cp <= ARABIC_BLOCK_LAST) ||
                               (cp >= PRESENTATION_FORMS_A_FIRST && cp <= PRESENTATION_FORMS_A_LAST) ||
                               (cp >= PRESENTATION_FORMS_B_FIRST && cp <= PRESENTATION_FORMS_B_LAST);
                    }

                } // namespace

                std::vector<uint32_t> shape(const std::vector<uint32_t> &codepoints)
                {
                    const std::size_t n = codepoints.size();
                    std::vector<uint32_t> out;
                    out.reserve(n);

                    // ── المرحلة 1: الأشكال السياقيّة + ليغاتورة لام-ألف (بترتيب المنطق) ──
                    for (std::size_t i = 0; i < n; ++i)
                    {
                        const uint32_t cp = codepoints[i];
                        const ShapeEntry *entry = findEntry(cp);
                        if (entry == nullptr)
                        {
                            out.push_back(cp); // خارج الجدول: يمرّ كما هو (حدّ معلَن)
                            continue;
                        }

                        // الاتّصال ممّا قبل: السابق المنطقيّ يتّصل يسارًا وهذا يقبل يمينًا.
                        // (لو استُهلك السابقان في ليغاتورة، فالسابق المباشر هنا هو الألف
                        // نفسها — يمينيّة الاتّصال فلا تتّصل يسارًا: الفحص صحيح تلقائيًّا.)
                        const bool linkBefore =
                            i > 0 && joinsForward(codepoints[i - 1]) && joinsBackward(cp);

                        // ليغاتورة لام-ألف: تُستهلك النقطتان وتُنتَج نقطة واحدة.
                        if (cp == LAM && i + 1 < n)
                        {
                            const LamAlefEntry *lig = findLamAlef(codepoints[i + 1]);
                            if (lig != nullptr)
                            {
                                out.push_back(linkBefore ? lig->final : lig->isolated);
                                ++i; // استهلاك الألف
                                continue;
                            }
                        }

                        const bool linkAfter =
                            entry->joining == Joining::Dual &&
                            i + 1 < n && joinsBackward(codepoints[i + 1]);

                        uint32_t shaped = entry->isolated;
                        if (linkBefore && linkAfter && entry->medial != 0)
                            shaped = entry->medial;
                        else if (linkBefore && entry->final != 0)
                            shaped = entry->final;
                        else if (linkAfter && entry->initial != 0)
                            shaped = entry->initial;
                        out.push_back(shaped);
                    }

                    // ── المرحلة 2: عكس المدى العربيّ البسيط (لا UAX#9 — حدّ معلَن) ──
                    // المدى = تتابُع نقاط عربيّة، والمسافات تُضمّ فقط إن تلاها عربيّ
                    // (مسافات الحافّة بين عربيّ ولاتينيّ تبقى خارج المدى بترتيبها).
                    const std::size_t m = out.size();
                    std::size_t i = 0;
                    while (i < m)
                    {
                        if (!isArabicDisplay(out[i]))
                        {
                            ++i;
                            continue;
                        }
                        std::size_t runEnd = i + 1; // ما بعد آخر عربيّ مضموم
                        std::size_t k = runEnd;
                        while (k < m)
                        {
                            if (isArabicDisplay(out[k]))
                            {
                                ++k;
                                runEnd = k;
                            }
                            else if (out[k] == SPACE_CODEPOINT)
                            {
                                ++k; // تُضمّ فقط إذا جاء عربيّ بعدها (runEnd لا يتقدّم الآن)
                            }
                            else
                            {
                                break;
                            }
                        }
                        std::reverse(out.begin() + static_cast<std::ptrdiff_t>(i),
                                     out.begin() + static_cast<std::ptrdiff_t>(runEnd));
                        i = runEnd;
                    }

                    return out;
                }

            } // namespace arabic
        } // namespace freestanding
    } // namespace ui
} // namespace sad
