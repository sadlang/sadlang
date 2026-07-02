// بسم الله الرحمن الرحيم
// ======================================================================
// recording_renderer.h — مُصيّر مسجِّل حتميّ لاختبار طبقة الرسم (headless)
// ======================================================================
// الوصف:
//   RecordingRenderer يُطبّق واجهة PlatformRenderer المجرّدة، لكنّه بدل
//   الرسم على شاشة/بكسل يُسجّل كل نداء رسم أساسيّ كسطر نصّيّ حتميّ في
//   "قائمة رسم" (display list). هذا يتيح اختبار طبقتَي التخطيط (layout)
//   والرسم (renderNode) — وهما المنطق المشترك أسفل شجرة IR الذي لا
//   تغطّيه طباعة_شجرة — بلا SDL2 ولا خطوط ولا بكسل، وبحتميّة تامّة.
//
//   قياس النص تركيبيّ (synthetic) ومستقلّ عن الخطّ: تقدّم كل محرف =
//   حجم_الخط × kSyntheticAdvanceRatio، والارتفاع = حجم_الخط ×
//   kSyntheticLineHeightRatio. هذا يجعل حسابات المحاذاة في renderNode
//   حتميّة على كل منصّة دون الاعتماد على SDL2_ttf.
//
//   قاعدة تصميميّة: لا سلاسل نصّيّة مباشرة في المنطق — كل رمز/شظيّة في
//   بروتوكول القائمة مُعرَّف كثابت مسمّى (kOp*/kFrag*/المحارف) في مكان
//   واحد، فتغيير البروتوكول نقطةٌ واحدة لا بحثٌ متناثر.
//
// English:
//   A deterministic PlatformRenderer that records each primitive draw
//   call as a text line ("display list") instead of rasterizing pixels.
//   Lets us golden-test the shared layout() + renderNode() logic below
//   the IR tree — the layer طباعة_شجرة does not cover — with zero SDL2,
//   fonts, or pixels, fully deterministically.
// ======================================================================

#ifndef SAD_UI_TESTING_RECORDING_RENDERER_H
#define SAD_UI_TESTING_RECORDING_RENDERER_H

#include "sad_ui/platform_renderer.h"
#include "sad_ui/node.h"

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <sstream>
#include <string>
#include <utility>

namespace sad
{
    namespace ui
    {
        namespace testing
        {

            /**
             * @brief (AR) مُصيّر يسجّل نداءات الرسم كنصّ حتميّ بدل البكسل.
             * @details (EN) Renderer that records draw calls as deterministic text.
             *
             * الاستخدام:
             *   RecordingRenderer r;
             *   r.render(irRoot, layoutResult);
             *   std::string displayList = r.str();
             *
             * تحذير حتميّة: لا تختبر حقل نصّ (TextField) وهو **مركَّز**
             * (setInteractionState بـfocused) — فمسار المؤشّر الوامض في renderNode
             * يقرأ getCurrentTimeMs() (وقتًا فعليًّا) فيجعل القائمة متذبذبة. اختبر
             * حقول النصّ غير مركَّزة فقط (كما في حالة الاختبار المخصّصة).
             * الاختبار آمن افتراضيًّا لأنّ render() لا يعيّن focusedNode_ (يبقى
             * nullptr)؛ الخطر يظهر فقط إن استدعيتَ setInteractionState(...,focused,...)
             * يدويًّا قبل render().
             */
            class RecordingRenderer : public PlatformRenderer
            {
            public:
                RecordingRenderer() = default;
                ~RecordingRenderer() override = default;

                /// قائمة الرسم المسجّلة حتى الآن
                [[nodiscard]] std::string str() const { return out_.str(); }

                /// تفريغ القائمة (لإعادة الاستخدام بين الحالات)
                void clear()
                {
                    out_.str(std::string{});
                    out_.clear();
                }

                // ── دورة الحياة (لا تفعل شيئًا — لا سطح فعليّ) ──────────────

                bool initialize(void * /*nativeSurface*/,
                                const PlatformRenderOptions &options = {}) override
                {
                    options_ = options; // options_ عضو موروث من PlatformRenderer
                    ready_ = true;
                    return true;
                }
                void shutdown() override { ready_ = false; }
                [[nodiscard]] bool isReady() const override { return ready_; }

                bool loadFont(const std::string & /*fontPath*/, float /*fontSize*/ = 16.0f) override
                {
                    return true; // القياس تركيبيّ — لا حاجة لخطّ فعليّ
                }

                // ملاحظة: PlatformRenderer::render() لا يستدعي beginFrame/endFrame
                // (انظر platform_renderer.cpp) — لذا لن يظهر سطرا begin-frame/end-frame
                // في أيّ قائمة ذهبيّة ناتجة عن render() (تؤكّده الحالات الـ13: لا قائمة
                // تحوي begin-frame). موجودان لاكتمال الواجهة، ويُختبَران بنداء يدويّ.
                void beginFrame(const Color &clearColor = {}) override
                {
                    out_ << kOpBeginFrame << kSpace << col(clearColor) << kEol;
                }
                void endFrame() override { out_ << kOpEndFrame << kEol; }

                // ── دوال الرسم الأساسية — كل واحدة تُسجّل سطرًا ──────────────

                void drawFilledRect(float x, float y, float w, float h,
                                    const Color &color) override
                {
                    out_ << kOpRect << kSpace << rectPart(x, y, w, h)
                         << kSpace << col(applyOpacity(color)) << kEol;
                }

                void drawRoundedRect(float x, float y, float w, float h,
                                     const Color &color, float radius) override
                {
                    out_ << kOpRoundedRect << kSpace << rectPart(x, y, w, h)
                         << kFragRadius << f(radius)
                         << kSpace << col(applyOpacity(color)) << kEol;
                }

                void drawRectOutline(float x, float y, float w, float h,
                                     const Color &color, float thickness) override
                {
                    out_ << kOpRectOutline << kSpace << rectPart(x, y, w, h)
                         << kFragThickness << f(thickness)
                         << kSpace << col(applyOpacity(color)) << kEol;
                }

                std::pair<float, float> drawText(const std::string &text,
                                                 float x, float y,
                                                 const Color &color,
                                                 float fontSize) override
                {
                    auto sz = measureText(text, fontSize);
                    out_ << kOpText << kSpace << quoted(text) << kSpace << pt(x, y)
                         << kFragSize << f(fontSize)
                         << kSpace << col(applyOpacity(color)) << kEol;
                    return sz;
                }

                std::pair<float, float> measureText(const std::string &text,
                                                    float fontSize) override
                {
                    // قياس تركيبيّ حتميّ مستقلّ عن الخطّ.
                    //
                    // حدّ مهمّ: renderNode يستخدم measureText نفسه لحساب المحاذاة
                    // (يمين RTL / توسيط)، فالاختبار يتحقّق من **اتّساق منطق المحاذاة
                    // داخليًّا** لا من **مطابقة بكسل desktop** — فالخلفيّة الحقيقيّة
                    // (SDL_ttf) قد تقيس النصّ بعرض خطّ مختلف. تلك مطابقة بكسل تُغطَّى
                    // بطبقة لقطة منفصلة لاحقًا، خارج هذا الاختبار الحتميّ.
                    float w = static_cast<float>(utf8Length(text)) * fontSize * kSyntheticAdvanceRatio;
                    float h = fontSize * kSyntheticLineHeightRatio;
                    return {w, h};
                }

                void drawImage(const std::string &path,
                               float x, float y, float w, float h) override
                {
                    out_ << kOpImage << kSpace << quoted(path)
                         << kSpace << rectPart(x, y, w, h) << kEol;
                }

                void drawCircle(float cx, float cy, float radius,
                                const Color &color) override
                {
                    out_ << kOpCircle << kSpace << pt(cx, cy) << kFragRadius << f(radius)
                         << kSpace << col(applyOpacity(color)) << kEol;
                }

                void drawCircleOutline(float cx, float cy, float radius,
                                       const Color &color, float thickness) override
                {
                    out_ << kOpCircleOutline << kSpace << pt(cx, cy)
                         << kFragRadius << f(radius) << kFragThickness << f(thickness)
                         << kSpace << col(applyOpacity(color)) << kEol;
                }

                void drawLine(float x1, float y1, float x2, float y2,
                              const Color &color, float thickness) override
                {
                    out_ << kOpLine << kSpace << pt(x1, y1) << kSpace << pt(x2, y2)
                         << kFragThickness << f(thickness)
                         << kSpace << col(applyOpacity(color)) << kEol;
                }

                void drawLinearGradient(float x, float y, float w, float h,
                                        const Color &startColor, const Color &endColor,
                                        bool vertical, float radius) override
                {
                    out_ << kOpGradient << kSpace << rectPart(x, y, w, h)
                         << kSpace << col(applyOpacity(startColor)) << kFragArrow
                         << col(applyOpacity(endColor))
                         << (vertical ? kFragVertical : kFragHorizontal)
                         << kFragRadius << f(radius) << kEol;
                }

                void setClipRect(float x, float y, float w, float h) override
                {
                    out_ << kOpClip << kSpace << rectPart(x, y, w, h) << kEol;
                }
                void clearClipRect() override { out_ << kOpClipClear << kEol; }

            private:
                // ── بروتوكول القائمة: الرموز (opcodes) ─────────────────────
                // (كلّها static constexpr ⇒ inline ضمنيًّا في C++17، بلا ODR)
                static constexpr const char *kOpRect = "rect";
                static constexpr const char *kOpRoundedRect = "rrect";
                static constexpr const char *kOpRectOutline = "routline";
                static constexpr const char *kOpText = "text";
                static constexpr const char *kOpImage = "image";
                static constexpr const char *kOpCircle = "circle";
                static constexpr const char *kOpCircleOutline = "circleoutline";
                static constexpr const char *kOpLine = "line";
                static constexpr const char *kOpGradient = "gradient";
                static constexpr const char *kOpClip = "clip";
                static constexpr const char *kOpClipClear = "clip-clear";
                static constexpr const char *kOpBeginFrame = "begin-frame";
                static constexpr const char *kOpEndFrame = "end-frame";

                // ── بروتوكول القائمة: الشظايا (وصفيّات داخل السطر) ──────────
                static constexpr const char *kFragRadius = " r=";
                static constexpr const char *kFragThickness = " t=";
                static constexpr const char *kFragSize = " size=";
                static constexpr const char *kFragArrow = "->";
                static constexpr const char *kFragVertical = " vertical";
                static constexpr const char *kFragHorizontal = " horizontal";

                // ── بروتوكول القائمة: المحارف المفردة ──────────────────────
                static constexpr char kEol = '\n';
                static constexpr char kSpace = ' ';
                static constexpr char kQuote = '"';
                static constexpr char kLParen = '(';
                static constexpr char kRParen = ')';
                static constexpr char kComma = ',';
                static constexpr char kDimSep = 'x';

                // ── ثوابت التنسيق والقياس ──────────────────────────────────
                static constexpr const char *kFloatFmt = "%.2f"; ///< يحكم دقّة القائمة الذهبيّة
                static constexpr std::size_t kFloatBufSize = 32;
                static constexpr float kZeroSnapEpsilon = 0.005f;         ///< نصف المنزلة الأخيرة (%.2f)
                static constexpr float kSyntheticAdvanceRatio = 0.5f;     ///< تقدّم المحرف = حجم × هذا
                static constexpr float kSyntheticLineHeightRatio = 1.2f;  ///< ارتفاع السطر = حجم × هذا

                // ── مساعدات نقيّة (بلا حالة؛ تبني std::string فقد ترمي bad_alloc) ──

                // تنسيق عائم حتميّ (منزلتان عشريّتان) — يُثبّت القائمة الذهبيّة.
                // افتراض حتميّة: القِيَم الهندسيّة في الحالات مُصمَّمة لتكون «نظيفة»
                // (مضاعفات نظيفة لـfontSize/إزاحات) تتجنّب أرقام «…5» في المنزلة
                // الثالثة، حتى لا يختلف تقريب %.2f بين libc (MSVC/glibc).
                static std::string f(float v)
                {
                    // تطبيع أيّ قيمة دون نصف المنزلة الأخيرة إلى +0.00 — يحصّن ضدّ
                    // "-0.00" (فرق إشارة الصفر) وضدّ اختلاف اتّجاه التقريب حول الصفر.
                    if (std::fabs(v) < kZeroSnapEpsilon)
                        v = 0.0f;
                    char buf[kFloatBufSize];
                    std::snprintf(buf, sizeof(buf), kFloatFmt, v);
                    return buf;
                }

                static std::string pt(float x, float y)
                {
                    std::string s;
                    s += kLParen;
                    s += f(x);
                    s += kComma;
                    s += f(y);
                    s += kRParen;
                    return s;
                }

                static std::string rectPart(float x, float y, float w, float h)
                {
                    std::string s = pt(x, y);
                    s += kSpace;
                    s += f(w);
                    s += kDimSep;
                    s += f(h);
                    return s;
                }

                // اللون كنصّ سداسيّ حتميّ (#RRGGBB أو #RRGGBBAA).
                // مصدر التنسيق هو Color::toHexString (node.cpp) الذي يحذف الألفا عند
                // a ≥ 0.999f؛ إن تغيّرت عتبته مستقبلًا تتغيّر القوائم الذهبيّة — فابحث هناك.
                // تنبيه: toHexString **يبتر** المكوّنات (static_cast<int>(v*255)) لا
                // يقرّبها؛ فاختر قيم شفافيّة يبعد ناتج v×255 فيها عن حدود الأعداد
                // الصحيحة كي لا يتذبذب البتر بخطأ float بين المنصّات.
                static std::string col(const Color &c) { return c.toHexString(); }

                // النصّ محاطًا باقتباس مع تهريب — يوحّد الاقتباس+التهريب في موضع واحد.
                static std::string quoted(const std::string &s)
                {
                    std::string out;
                    out += kQuote;
                    out += esc(s);
                    out += kQuote;
                    return out;
                }

                // تهريب محارف النصّ الخاصّة حتى تبقى القائمة سطرًا واحدًا لا لبس فيه.
                // بروتوكول التهريب المعتمَد للقائمة: خطّ مائل/اقتباس/سطر/إرجاع/جدولة.
                static std::string esc(const std::string &s)
                {
                    std::string out;
                    out.reserve(s.size());
                    for (char ch : s)
                    {
                        switch (ch)
                        {
                        case '\\': out += "\\\\"; break;
                        case '"':  out += "\\\""; break;
                        case '\n': out += "\\n"; break;
                        case '\r': out += "\\r"; break;
                        case '\t': out += "\\t"; break;
                        default:   out += ch; break;
                        }
                    }
                    return out;
                }

                std::ostringstream out_;
                bool ready_ = false;
            };

        } // namespace testing
    } // namespace ui
} // namespace sad

#endif // SAD_UI_TESTING_RECORDING_RENDERER_H
