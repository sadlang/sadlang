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
#include <utility>

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

                    /// (AR) عكس SHAPE_TABLE: أيّ شكل عرضٍ ⇒ الأساس. صفر إن لم يُطابق
                    /// (يشمل تشكيلات FE70–FE7F التي لا يُخرجها shape() — لا تُطوى قصدًا).
                    uint32_t baseOfPresForm(uint32_t cp)
                    {
                        for (const auto &e : SHAPE_TABLE)
                            if (cp == e.isolated || cp == e.final ||
                                cp == e.initial || cp == e.medial)
                                return e.base;
                        // ليغاتورات لام-ألف ⇒ اللام (الخطّ النقطيّ بلا ليغاتورة):
                        for (const auto &e : LAM_ALEF_TABLE)
                            if (cp == e.isolated || cp == e.final)
                                return LAM;
                        return 0;
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

                    /// (AR) علامة تشكيل/حركة عربيّة تُركَّب فوق/تحت الحرف السابق
                    /// (فتحة/ضمّة/كسرة/تنوين/سكون/شدّة/مدّة/ألف خنجريّة …). هذه
                    /// النقاط لا عرض تقدُّميّ لها (advance=0 في التنضيد الصحيح) والخطّ
                    /// النقطيّ لا يموضِعها فوقيًّا — تموضعها دَين FreeType المعلَن.
                    bool isCombiningMark(uint32_t cp)
                    {
                        return (cp >= 0x064B && cp <= 0x065F) || // الحركات + التنوين + علامات
                               (cp == 0x0670) ||                  // ألف خنجريّة فوقيّة
                               (cp >= 0x06D6 && cp <= 0x06DC) ||  // علامات وقف/تجويد
                               (cp >= 0x06DF && cp <= 0x06E4) ||
                               (cp == 0x06E7 || cp == 0x06E8) ||
                               (cp >= 0x06EA && cp <= 0x06ED);
                    }

                    // ═══════════════════════════════════════════════════════════════════
                    // (AR) خوارزميّة الاتّجاه الثنائيّ Unicode (UAX#9) — فقرة واحدة.
                    //   تستبدل «عكس المدى + تموضع الأرقام» بمحرّك مستويات كامل: تُصنَّف كلّ
                    //   نقطة، تُحسَب مستويات التضمين (تضمينات صريحة LRE/RLE/LRO/RLO/PDF +
                    //   عزلات LRI/RLI/FSI/PDI)، ثمّ القواعد الضعيفة (W1–W7)، ثمّ أزواج
                    //   الأقواس (N0/BD16)، ثمّ المحايدة (N1–N2) والضمنيّة (I1–I2)، ثمّ
                    //   إعادة الترتيب البصريّ (L1–L2). تعمل على الغليفات المُشكَّلة (أشكال
                    //   العرض FE70–FEFF ⇒ AL).
                    //   حدود معلَنة: (١) N0 مطبَّقة لأقواس ASCII القياسيّة ( [ { (+ توسعة
                    //   للقوسين المزخرفين العربيّين ﴾﴿ خارج UCD) لا لكامل Bidi_Paired_Bracket؛
                    //   (٢) جدول التصنيف يغطّي نطاق الشكّال (عربيّ/عبريّ/لاتينيّ/أرقام/ترقيم
                    //   شائع) لا كامل قاعدة UCD؛ (٣) العزلات تُعامَل كتضمينات (LRI≈LRE،
                    //   RLI≈RLE، PDI≈PDF) — دقيق للتضمينات، تقريبيّ للعزل العميق المتداخل.
                    // (EN) Unicode Bidirectional Algorithm (UAX#9), single paragraph;
                    //   replaces the run-reversal heuristic with a full level engine.
                    // ═══════════════════════════════════════════════════════════════════
                    enum class Bc : uint8_t {
                        L, R, AL, EN, ES, ET, AN, CS, NSM, BN, B, S, WS, ON,
                        LRE, RLE, LRO, RLO, PDF, LRI, RLI, FSI, PDI
                    };

                    Bc bidiClass(uint32_t cp)
                    {
                        // تنسيقات صريحة (تضمين/عزل)
                        switch (cp)
                        {
                        case 0x202A: return Bc::LRE;
                        case 0x202B: return Bc::RLE;
                        case 0x202D: return Bc::LRO;
                        case 0x202E: return Bc::RLO;
                        case 0x202C: return Bc::PDF;
                        case 0x2066: return Bc::LRI;
                        case 0x2067: return Bc::RLI;
                        case 0x2068: return Bc::FSI;
                        case 0x2069: return Bc::PDI;
                        default: break;
                        }
                        if (cp == 0x0020) return Bc::WS;
                        if (cp == 0x0009 || cp == 0x000B) return Bc::S;
                        if (cp == 0x000A || cp == 0x000D || cp == 0x0085 || cp == 0x2029) return Bc::B;
                        if (cp >= 0x0030 && cp <= 0x0039) return Bc::EN;   // أرقام ASCII
                        if ((cp >= 0x0041 && cp <= 0x005A) || (cp >= 0x0061 && cp <= 0x007A))
                            return Bc::L;                                    // لاتينيّة
                        if (cp == 0x002B || cp == 0x002D) return Bc::ES;    // + −
                        if (cp == 0x002C || cp == 0x002E || cp == 0x002F ||
                            cp == 0x003A || cp == 0x00A0)
                            return Bc::CS;                                   // , . / : NBSP
                        if (cp == 0x0023 || cp == 0x0024 || cp == 0x0025 ||
                            cp == 0x00A2 || cp == 0x00A3 || cp == 0x00A4 || cp == 0x00A5)
                            return Bc::ET;                                   // # $ % عملات
                        if (cp >= 0x0660 && cp <= 0x0669) return Bc::AN;    // أرقام عربيّة-هنديّة
                        if (cp == 0x066B || cp == 0x066C) return Bc::AN;    // فاصلتا العدد العربيّتان
                        if (cp >= 0x06F0 && cp <= 0x06F9) return Bc::EN;    // ممتدّة (فارسيّ/أردو)
                        if (cp == 0x060C) return Bc::CS;                    // الفاصلة العربيّة (فاصل مشترك)
                        if (cp == 0x066A) return Bc::ET;                    // ٪ علامة النسبة العربيّة
                        if (cp == 0xFD3E || cp == 0xFD3F) return Bc::ON;    // قوسان مزخرفان: Bidi_Class=ON قياسيًّا (لا AL)
                        if ((cp >= ARABIC_BLOCK_FIRST && cp <= ARABIC_BLOCK_LAST) ||
                            (cp >= PRESENTATION_FORMS_A_FIRST && cp <= PRESENTATION_FORMS_A_LAST) ||
                            (cp >= PRESENTATION_FORMS_B_FIRST && cp <= PRESENTATION_FORMS_B_LAST))
                            return Bc::AL;                                   // عربيّة (أساس/أشكال عرض)
                        if (cp >= 0x0590 && cp <= 0x05FF) return Bc::R;     // عبريّة
                        if ((cp >= 0x0300 && cp <= 0x036F) ||               // علامات تجميعيّة
                            (cp >= 0x064B && cp <= 0x065F) || cp == 0x0670)
                            return Bc::NSM;
                        return Bc::ON;                                       // محايد افتراضًا
                    }

                    inline bool bidiIsIsolateInitiator(Bc c)
                    {
                        return c == Bc::LRI || c == Bc::RLI || c == Bc::FSI;
                    }
                    inline bool bidiIsExplicit(Bc c)
                    {
                        return c == Bc::LRE || c == Bc::RLE || c == Bc::LRO || c == Bc::RLO ||
                               c == Bc::PDF || bidiIsIsolateInitiator(c) || c == Bc::PDI;
                    }

                    constexpr uint8_t BIDI_MAX_DEPTH = 125;

                    /// (AR) اتّجاه المستوى: زوجيّ ⇒ L، فرديّ ⇒ R.
                    inline Bc bidiLevelDir(uint8_t lvl) { return (lvl & 1u) ? Bc::R : Bc::L; }

                    /// (AR) مستوى الفقرة (P2–P3): أوّل حرفٍ قويّ (L⇒0، R/AL⇒1)، وإلّا 0.
                    ///      يتخطّى ما بين مبتدئ عزلٍ وPDI المطابق (لا يُحتسَب في P2).
                    uint8_t bidiParagraphLevel(const std::vector<Bc> &types)
                    {
                        int isolate = 0;
                        for (Bc t : types)
                        {
                            if (bidiIsIsolateInitiator(t)) { ++isolate; continue; }
                            if (t == Bc::PDI) { if (isolate > 0) --isolate; continue; }
                            if (isolate > 0) continue;
                            if (t == Bc::L) return 0;
                            if (t == Bc::R || t == Bc::AL) return 1;
                        }
                        return 0;
                    }

                    /// (AR) القواعد الصريحة (X1–X8 مبسّطة بمكدّس تضمين/تجاوز): تُنتج مستوى
                    ///      كلّ نقطة وتجاوزها الاتّجاهيّ. العزلات تُعامَل كتضمينات (تقريب
                    ///      معلَن). تنسيقات التضمين/العزل نفسها تُعلَّم BN (تُزال في L).
                    void bidiResolveExplicit(const std::vector<Bc> &types, uint8_t paraLevel,
                                             std::vector<uint8_t> &levels, std::vector<Bc> &resolved)
                    {
                        struct Entry { uint8_t level; Bc override_; };
                        std::vector<Entry> stack;
                        stack.push_back({paraLevel, Bc::ON});
                        auto nextOdd = [](uint8_t l) -> uint8_t { return (l % 2 == 0) ? l + 1 : l + 2; };
                        auto nextEven = [](uint8_t l) -> uint8_t { return (l % 2 == 0) ? l + 2 : l + 1; };
                        for (std::size_t i = 0; i < types.size(); ++i)
                        {
                            Bc t = types[i];
                            const Entry top = stack.back();
                            if (t == Bc::RLE || t == Bc::RLI)
                            {
                                levels[i] = top.level;
                                uint8_t nl = nextOdd(top.level);
                                if (nl <= BIDI_MAX_DEPTH) stack.push_back({nl, Bc::ON});
                                resolved[i] = Bc::BN;
                            }
                            else if (t == Bc::LRE || t == Bc::LRI)
                            {
                                levels[i] = top.level;
                                uint8_t nl = nextEven(top.level);
                                if (nl <= BIDI_MAX_DEPTH) stack.push_back({nl, Bc::ON});
                                resolved[i] = Bc::BN;
                            }
                            else if (t == Bc::RLO)
                            {
                                levels[i] = top.level;
                                uint8_t nl = nextOdd(top.level);
                                if (nl <= BIDI_MAX_DEPTH) stack.push_back({nl, Bc::R});
                                resolved[i] = Bc::BN;
                            }
                            else if (t == Bc::LRO)
                            {
                                levels[i] = top.level;
                                uint8_t nl = nextEven(top.level);
                                if (nl <= BIDI_MAX_DEPTH) stack.push_back({nl, Bc::L});
                                resolved[i] = Bc::BN;
                            }
                            else if (t == Bc::FSI)
                            {
                                // FSI: اتّجاه ذاتيّ من محتوى العزل حتّى PDI المطابق (P2 محلّيّ).
                                levels[i] = top.level;
                                int depth = 0; uint8_t innerPara = 0;
                                for (std::size_t j = i + 1; j < types.size(); ++j)
                                {
                                    Bc u = types[j];
                                    if (bidiIsIsolateInitiator(u)) { ++depth; continue; }
                                    if (u == Bc::PDI) { if (depth == 0) break; --depth; continue; }
                                    if (depth > 0) continue;
                                    if (u == Bc::L) { innerPara = 0; break; }
                                    if (u == Bc::R || u == Bc::AL) { innerPara = 1; break; }
                                }
                                uint8_t nl = innerPara ? nextOdd(top.level) : nextEven(top.level);
                                if (nl <= BIDI_MAX_DEPTH) stack.push_back({nl, Bc::ON});
                                resolved[i] = Bc::BN;
                            }
                            else if (t == Bc::PDF || t == Bc::PDI)
                            {
                                if (stack.size() > 1) stack.pop_back();
                                levels[i] = stack.back().level;
                                resolved[i] = Bc::BN;
                            }
                            else if (t == Bc::B)
                            {
                                levels[i] = paraLevel;
                                resolved[i] = t;
                            }
                            else
                            {
                                levels[i] = top.level;
                                resolved[i] = (top.override_ == Bc::ON) ? t : top.override_;
                            }
                        }
                    }

                    // ─── BD16/N0: أزواج الأقواس المتناظرة ───────────────────────────
                    // مجال الشكّال: أقواس ASCII الثلاثة (Bidi_Paired_Bracket القياسيّة)
                    // + القوسان المُزخرفان العربيّان ﴾﴿ كـ**توسعة عمليّة** (هذان ليسا في
                    // BidiBrackets.txt — bpt=None — لكنّهما قوسان دلاليًّا في النصّ العربيّ،
                    // فنعاملهما حاصرتَي N0). المعرّف الموحَّد = نقطة الفتح (canonical) كي
                    // يطابق كلُّ إغلاقٍ فتحَه. بقيّة مجموعة Bidi_Paired_Bracket خارج الشريحة.
                    struct BracketPair { uint32_t open; uint32_t close; };
                    constexpr BracketPair BRACKET_TABLE[] = {
                        {0x0028, 0x0029}, // ( )
                        {0x005B, 0x005D}, // [ ]
                        {0x007B, 0x007D}, // { }
                        {0xFD3E, 0xFD3F}, // ﴾ ﴿ قوسان مزخرفان عربيّان
                    };
                    /// (AR) نوع القوس: 1=فتح، 2=إغلاق، 0=لا؛ ويضع المعرّف الموحّد (نقطة الفتح).
                    int bracketKind(uint32_t cp, uint32_t &canon)
                    {
                        for (const auto &br : BRACKET_TABLE)
                        {
                            if (cp == br.open) { canon = br.open; return 1; }
                            if (cp == br.close) { canon = br.open; return 2; }
                        }
                        return 0;
                    }
                    /// (AR) حدّ مكدّس BD16 (المواصفة: 63 عنصرًا ثمّ يتوقّف مسح الأزواج).
                    constexpr std::size_t BD16_STACK_MAX = 63;

                    /// (AR) القواعد الضعيفة والمحايدة والضمنيّة (W1–W7، N0، N1–N2، I1–I2)
                    ///      على مدًى مستوًى واحد [b,e) بحدَّي sor/eor المعطَيَين. glyphs/orig
                    ///      للمدى كلّه (فهارس مطلقة): glyphs لكشف الأقواس (N0)، وorig لنوع
                    ///      NSM الأصليّ (قبل W1) في متابعة الحاصرات لأقواس N0.
                    void bidiResolveRun(std::vector<Bc> &t, const std::vector<uint32_t> &glyphs,
                                        const std::vector<Bc> &orig, std::size_t b, std::size_t e,
                                        Bc sor, Bc eor, uint8_t level)
                    {
                        if (b >= e) return;
                        // W1: NSM ⇐ نوع سابقه (أو sor).
                        for (std::size_t i = b; i < e; ++i)
                            if (t[i] == Bc::NSM)
                                t[i] = (i == b) ? sor : t[i - 1];
                        // W2: EN ⇒ AN إن كان آخر قويّ AL.
                        {
                            Bc lastStrong = sor;
                            for (std::size_t i = b; i < e; ++i)
                            {
                                if (t[i] == Bc::R || t[i] == Bc::L || t[i] == Bc::AL) lastStrong = t[i];
                                else if (t[i] == Bc::EN && lastStrong == Bc::AL) t[i] = Bc::AN;
                            }
                        }
                        // W3: AL ⇒ R.
                        for (std::size_t i = b; i < e; ++i)
                            if (t[i] == Bc::AL) t[i] = Bc::R;
                        // W4: ES بين EN⇒EN؛ CS بين رقمين متطابقين ⇒ نوعهما.
                        for (std::size_t i = b + 1; i + 1 < e; ++i)
                        {
                            if (t[i] == Bc::ES && t[i - 1] == Bc::EN && t[i + 1] == Bc::EN)
                                t[i] = Bc::EN;
                            else if (t[i] == Bc::CS && t[i - 1] == t[i + 1] &&
                                     (t[i - 1] == Bc::EN || t[i - 1] == Bc::AN))
                                t[i] = t[i - 1];
                        }
                        // W5: تتابُع ET ملاصق لـEN ⇒ EN.
                        for (std::size_t i = b; i < e; ++i)
                        {
                            if (t[i] != Bc::ET) continue;
                            std::size_t j = i;
                            while (j < e && t[j] == Bc::ET) ++j;
                            bool before = (i > b && t[i - 1] == Bc::EN);
                            bool after = (j < e && t[j] == Bc::EN);
                            if (before || after)
                                for (std::size_t k = i; k < j; ++k) t[k] = Bc::EN;
                            i = j - 1;
                        }
                        // W6: ما بقي من ES/ET/CS ⇒ ON.
                        for (std::size_t i = b; i < e; ++i)
                            if (t[i] == Bc::ES || t[i] == Bc::ET || t[i] == Bc::CS) t[i] = Bc::ON;
                        // W7: EN ⇒ L إن كان آخر قويّ L.
                        {
                            Bc lastStrong = sor;
                            for (std::size_t i = b; i < e; ++i)
                            {
                                if (t[i] == Bc::R || t[i] == Bc::L) lastStrong = t[i];
                                else if (t[i] == Bc::EN && lastStrong == Bc::L) t[i] = Bc::L;
                            }
                        }
                        // N0: أزواج الأقواس (BD16). ضمن هذا النطاق EN/AN كـR. يُطابَق
                        //     كلُّ زوجٍ ثمّ تُسنَد كلتا حاصرتيه لاتّجاهٍ قويّ وفق محتواه
                        //     والسياق السابق، فتصير حاصرةٌ يمينيّةٌ ويسريّةٌ متطابقتين
                        //     (وإلّا تُترَك للمحايدات N1/N2 حين لا قويّ داخلها).
                        {
                            auto strongN0 = [](Bc c) -> Bc {
                                if (c == Bc::L) return Bc::L;
                                if (c == Bc::R || c == Bc::EN || c == Bc::AN) return Bc::R;
                                return Bc::ON;
                            };
                            // BD16: اكتشاف الأزواج بمكدّس فتحٍ داخل المدى [b,e).
                            struct StackEl { uint32_t canon; std::size_t pos; };
                            std::vector<StackEl> stk;
                            std::vector<std::pair<std::size_t, std::size_t>> pairs;
                            for (std::size_t k = b; k < e; ++k)
                            {
                                uint32_t canon = 0;
                                int kind = bracketKind(glyphs[k], canon);
                                if (kind == 1)
                                {
                                    if (stk.size() >= BD16_STACK_MAX) break; // مكدّس ممتلئ ⇒ توقّف
                                    stk.push_back({canon, k});
                                }
                                else if (kind == 2)
                                {
                                    for (std::size_t s = stk.size(); s-- > 0;)
                                        if (stk[s].canon == canon)
                                        {
                                            pairs.push_back({stk[s].pos, k});
                                            stk.resize(s); // انزع حتّى المطابق ضمنًا
                                            break;
                                        }
                                }
                            }
                            std::sort(pairs.begin(), pairs.end()); // بترتيب موضع الفتح
                            const Bc eDir = bidiLevelDir(level);
                            const Bc oppDir = (eDir == Bc::L) ? Bc::R : Bc::L;
                            for (const auto &pr : pairs)
                            {
                                const std::size_t o = pr.first, c = pr.second;
                                bool hasE = false, hasOpp = false;
                                for (std::size_t k = o + 1; k < c; ++k)
                                {
                                    Bc s = strongN0(t[k]);
                                    if (s == eDir) { hasE = true; break; }
                                    if (s == oppDir) hasOpp = true;
                                }
                                Bc chosen;
                                if (hasE) chosen = eDir;               // (b) قويّ باتّجاه التضمين
                                else if (hasOpp)                        // (c) قويّ معاكس فقط داخلها
                                {
                                    Bc prevStrong = sor;
                                    for (std::size_t k = o; k-- > b;)
                                    {
                                        Bc s = strongN0(t[k]);
                                        if (s != Bc::ON) { prevStrong = s; break; }
                                    }
                                    chosen = (prevStrong == oppDir) ? oppDir : eDir;
                                }
                                else continue;                          // (d) لا قويّ ⇒ اترك للـN1/N2
                                t[o] = chosen;
                                t[c] = chosen;
                                // متابعة NSM: حاصرات نوعها الأصليّ (قبل W1) NSM وتلي القوس
                                //             مباشرةً تتبع اتّجاهه (فقرة N0 الأخيرة).
                                for (std::size_t k = o + 1; k < e && orig[k] == Bc::NSM; ++k) t[k] = chosen;
                                for (std::size_t k = c + 1; k < e && orig[k] == Bc::NSM; ++k) t[k] = chosen;
                            }
                        }
                        // N1–N2: المحايدات (B/S/WS/ON + BN) — EN/AN تُعامَل R.
                        auto asStrong = [](Bc c) -> Bc {
                            if (c == Bc::L) return Bc::L;
                            if (c == Bc::R || c == Bc::EN || c == Bc::AN) return Bc::R;
                            return Bc::ON;
                        };
                        auto isNeutral = [](Bc c) -> bool {
                            return c == Bc::B || c == Bc::S || c == Bc::WS || c == Bc::ON || c == Bc::BN;
                        };
                        for (std::size_t i = b; i < e; ++i)
                        {
                            if (!isNeutral(t[i])) continue;
                            std::size_t j = i;
                            while (j < e && isNeutral(t[j])) ++j;
                            Bc left = (i == b) ? sor : asStrong(t[i - 1]);
                            Bc right = (j == e) ? eor : asStrong(t[j]);
                            Bc fill = (left == right && left != Bc::ON) ? left : bidiLevelDir(level); // N1 وإلّا N2
                            for (std::size_t k = i; k < j; ++k) t[k] = fill;
                            i = j - 1;
                        }
                    }

                    /// (AR) يُنتج ترتيب الغليفات البصريّ (فهارس منطقيّة) من المستويات بعد
                    ///      تطبيق الضمنيّة I1–I2 وإعادة الترتيب L2 (عكس المدايات من الأعلى).
                    std::vector<uint8_t> bidiImplicitLevels(const std::vector<Bc> &t,
                                                            const std::vector<uint8_t> &lvlIn)
                    {
                        std::vector<uint8_t> lvl = lvlIn;
                        for (std::size_t i = 0; i < t.size(); ++i)
                        {
                            if (lvl[i] % 2 == 0)
                            {
                                if (t[i] == Bc::R) lvl[i] += 1;
                                else if (t[i] == Bc::AN || t[i] == Bc::EN) lvl[i] += 2;
                            }
                            else
                            {
                                if (t[i] == Bc::L || t[i] == Bc::EN || t[i] == Bc::AN) lvl[i] += 1;
                            }
                        }
                        return lvl;
                    }

                    /// (AR) المحرّك الكامل: يعيد ترتيب الغليفات بصريًّا (أوّل عنصر = أقصى
                    ///      اليسار) وفق UAX#9، ويزيل تنسيقات التضمين/العزل الصفريّة العرض.
                    std::vector<uint32_t> bidiReorder(const std::vector<uint32_t> &glyphs)
                    {
                        const std::size_t n = glyphs.size();
                        if (n == 0) return {};
                        std::vector<Bc> orig(n);
                        for (std::size_t i = 0; i < n; ++i) orig[i] = bidiClass(glyphs[i]);

                        const uint8_t paraLevel = bidiParagraphLevel(orig);
                        std::vector<uint8_t> levels(n, paraLevel);
                        std::vector<Bc> types(n);
                        bidiResolveExplicit(orig, paraLevel, levels, types);

                        // معالجة W/N على كلّ مدًى مستوًى متساوٍ (تسلسل مدى العزل = مدى المستوى
                        // لنصّ الفقرة الواحدة). sor/eor = اتّجاه الأعلى بين المدى وجاره.
                        std::size_t i = 0;
                        while (i < n)
                        {
                            std::size_t j = i + 1;
                            while (j < n && levels[j] == levels[i]) ++j;
                            uint8_t runLvl = levels[i];
                            uint8_t prevLvl = (i == 0) ? paraLevel : levels[i - 1];
                            uint8_t nextLvl = (j == n) ? paraLevel : levels[j];
                            Bc sor = bidiLevelDir(std::max(runLvl, prevLvl));
                            Bc eor = bidiLevelDir(std::max(runLvl, nextLvl));
                            bidiResolveRun(types, glyphs, orig, i, j, sor, eor, runLvl);
                            i = j;
                        }

                        std::vector<uint8_t> lvl = bidiImplicitLevels(types, levels);

                        // L1: أعِد S وB (والمسافات السابقة لهما أو الطرفيّة) إلى مستوى الفقرة.
                        for (std::size_t k = 0; k < n; ++k)
                        {
                            if (orig[k] == Bc::B || orig[k] == Bc::S)
                            {
                                lvl[k] = paraLevel;
                                for (std::size_t m = k; m-- > 0;)
                                {
                                    if (orig[m] == Bc::WS || bidiIsExplicit(orig[m]))
                                        lvl[m] = paraLevel;
                                    else break;
                                }
                            }
                        }
                        for (std::size_t m = n; m-- > 0;)
                        {
                            if (orig[m] == Bc::WS || bidiIsExplicit(orig[m])) lvl[m] = paraLevel;
                            else break;
                        }

                        // بناء ترتيب الفهارس (استبعاد تنسيقات التضمين/العزل الصفريّة).
                        std::vector<std::size_t> order;
                        order.reserve(n);
                        for (std::size_t k = 0; k < n; ++k)
                            if (!bidiIsExplicit(orig[k])) order.push_back(k);
                        if (order.empty()) return {};

                        // L2: من أعلى مستوًى نزولًا حتّى أدنى مستوًى فرديّ، اعكس المدايات
                        //     المتّصلة ذات المستوى ≥ الحدّ (على تسلسل الفهارس المرئيّة).
                        uint8_t maxLvl = 0, minOdd = BIDI_MAX_DEPTH + 1;
                        for (std::size_t k : order)
                        {
                            if (lvl[k] > maxLvl) maxLvl = lvl[k];
                            if ((lvl[k] & 1u) && lvl[k] < minOdd) minOdd = lvl[k];
                        }
                        for (uint8_t l = maxLvl; l >= minOdd && l > 0; --l)
                        {
                            std::size_t a = 0;
                            while (a < order.size())
                            {
                                if (lvl[order[a]] < l) { ++a; continue; }
                                std::size_t bEnd = a;
                                while (bEnd < order.size() && lvl[order[bEnd]] >= l) ++bEnd;
                                std::reverse(order.begin() + static_cast<std::ptrdiff_t>(a),
                                             order.begin() + static_cast<std::ptrdiff_t>(bEnd));
                                a = bEnd;
                            }
                            if (l == minOdd) break; // تفادي التفاف uint8_t تحت الصفر
                        }

                        std::vector<uint32_t> out;
                        out.reserve(order.size());
                        for (std::size_t k : order) out.push_back(glyphs[k]);
                        return out;
                    }

                } // namespace

                std::vector<uint32_t> shape(const std::vector<uint32_t> &codepointsIn)
                {
                    // ── المرحلة 0: شفافيّة الحركات (تحسين التشكيل) ──
                    // تُسقَط علامات التشكيل قبل المرحلة 1 كي لا تكسر اتّصال الحرفين
                    // اللذين تفصل بينهما: قبل هذا كانت الحركة (مثلًا شدّة «ـّ» بين ي وة
                    // في «الطرفيّة») تُرى «سابقًا/لاحقًا» لا يتّصل فتنكسر السلسلة ويرتدّ
                    // الحرفان إلى شكلٍ منفصل. الخطّ النقطيّ لا يموضِع الحركات فوقيًّا
                    // (advance لها 0 في التنضيد الصحيح) فإسقاطها هو السلوك الأمين لهذا
                    // المسار؛ تموضعها الفوقيّ دَين FreeType المعلَن (المسار المتّجه).
                    std::vector<uint32_t> codepoints;
                    codepoints.reserve(codepointsIn.size());
                    for (uint32_t cp : codepointsIn)
                        if (!isCombiningMark(cp))
                            codepoints.push_back(cp);

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

                    // ── المرحلة 2: إعادة الترتيب البصريّ بخوارزميّة UAX#9 الكاملة ──
                    // تحلّ محلَّ «عكس المدى + تموضع الأرقام»: محرّك مستويات كامل يعالج
                    // الفقرات المختلطة L/R، التضمينات الصريحة، حلّ المحايدات بين المدايات،
                    // والأرقام أينما وقعت (بما فيها ما يسبق المدى العربيّ). النتيجة بترتيبٍ
                    // بصريّ (أوّل عنصر = أقصى اليسار) جاهزةٍ لحلقة رسمٍ تتقدّم يسارًا.
                    return bidiReorder(out);
                }

                uint32_t presentationFormToBase(uint32_t cp)
                {
                    if (!isPresentationFormB(cp))
                        return cp;
                    uint32_t base = baseOfPresForm(cp);
                    return base != 0 ? base : cp;
                }

            } // namespace arabic
        } // namespace freestanding
    } // namespace ui
} // namespace sad
