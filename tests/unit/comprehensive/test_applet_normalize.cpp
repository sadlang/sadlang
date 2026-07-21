// بسم الله الرحمن الرحيم
// ============================================================================
// (AR) اختبار وحدة للمطابقة المتسامحة في موزِّع آبلتات sad-repl:
//        - normalizeArabic (المولَّدة) تجرّد التشكيل/التطويل وتطوي همزات الألف.
//        - appletExec ثنائيّ الطبقة: تطابقٌ بايتيّ تامّ أوّلًا، ثمّ مطبَّع متسامح.
//      يحرس أنّ دالّة C++ المولَّدة تطابق التطبيع المتوقَّع (نظير normalize_arabic
//      في _lib/emit.py)، وأنّ الدَّينين الموثَّقَين (نسيان الشدّة، همزة الألف) مُغلقان.
//      نقيّ (لا execvp، لا busybox) ⇒ يعمل على كلّ منصّات CI.
// (EN) Unit test for the sad-repl applet dispatcher's tolerant matching:
//      normalizeArabic + two-tier appletExec (exact then normalized). Pure
//      string logic (no execvp/busybox) so it runs on every CI platform.
// ============================================================================
#include "repl_sot_generated.h"

#include <cstdio>
#include <string>
#include <string_view>

using namespace Sad::REPL::SoT;

static int g_failures = 0;

static void expectExec(std::string_view input, const char *wantExec, const char *what) {
    const char *got = appletExec(input);
    bool ok = (wantExec == nullptr) ? (got == nullptr)
                                    : (got != nullptr && std::string(got) == wantExec);
    if (!ok) {
        std::printf("  FAIL: %s — appletExec(\"%.*s\") = %s (توقّعنا %s)\n", what,
                    static_cast<int>(input.size()), input.data(), got ? got : "nullptr",
                    wantExec ? wantExec : "nullptr");
        ++g_failures;
    }
}

static void expectNorm(std::string_view input, std::string_view want, const char *what) {
    std::string got = normalizeArabic(input);
    if (got != want) {
        std::printf("  FAIL: %s — normalizeArabic mismatch\n", what);
        ++g_failures;
    }
}

int main() {
    // ── ١) التطابق البايتيّ التامّ محفوظ (لا انحدار سلوكيّ) ──
    // «اطبع» = echo، «اعرض» = cat، «صحيح» = true — أسماء قانونيّة كما هي.
    expectExec("\xd8\xa7\xd8\xb7\xd8\xa8\xd8\xb9", "echo", "exact: اطبع⇒echo");
    expectExec("\xd8\xa7\xd8\xb9\xd8\xb1\xd8\xb6", "cat", "exact: اعرض⇒cat");
    expectExec("\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad", "true", "exact: صحيح⇒true");

    // ── ٢) اسم غير معرَّف يبقى بلا ترجمة (توافق خلفيّ: يمرّ للإنجليزيّة) ──
    expectExec("ls", nullptr, "undefined: ls⇒nullptr");
    expectExec("\xd9\x84\xd8\xa7_\xd9\x8a\xd9\x88\xd8\xac\xd8\xaf", nullptr, "undefined: لا_يوجد⇒nullptr");

    // ── ٣) الدَّين الموثَّق: همزة الألف (mesg = «إذن_الرسائل»، القانونيّ بهمزة تحت) ──
    // إدخال بهمزة فوق «أذن_الرسائل» يجب أن يُطبَّع ويطابق ⇒ mesg.
    // إذن_الرسائل (canonical): 0xd8 0xa5 ... | أذن_الرسائل (variant): 0xd8 0xa3 ...
    expectExec("\xd8\xa5\xd8\xb0\xd9\x86_\xd8\xa7\xd9\x84\xd8\xb1\xd8\xb3\xd8\xa7\xd8\xa6\xd9\x84",
               "mesg", "exact: إذن_الرسائل⇒mesg");
    expectExec("\xd8\xa3\xd8\xb0\xd9\x86_\xd8\xa7\xd9\x84\xd8\xb1\xd8\xb3\xd8\xa7\xd8\xa6\xd9\x84",
               "mesg", "tolerant: أذن_الرسائل (همزة فوق)⇒mesg");

    // ── ٤) الدَّين الموثَّق: نسيان الشدّة («بدّل_محارف» = tr، الشدّة U+0651 على الدال) ──
    // بدون شدّة «بدل_محارف» يجب أن يطابق ⇒ tr.
    expectExec("\xd8\xa8\xd8\xaf\xd9\x91\xd9\x84_\xd9\x85\xd8\xad\xd8\xa7\xd8\xb1\xd9\x81",
               "tr", "exact: بدّل_محارف (بالشدّة)⇒tr");
    expectExec("\xd8\xa8\xd8\xaf\xd9\x84_\xd9\x85\xd8\xad\xd8\xa7\xd8\xb1\xd9\x81",
               "tr", "tolerant: بدل_محارف (بلا شدّة)⇒tr");

    // ── ٥) تكافؤ الجدول كاملًا: normalizeArabic(arabic) == arabicNorm لكلّ مدخل ──
    //     يحرس تطابق دالّة C++ المولَّدة مع normalize_arabic (Python) الّتي حسبت
    //     arabicNorm — إذ لا يقابل gen --check قالب الدالّة بمصدر Python (مصدران).
    for (std::size_t i = 0; i < kAppletsCount; ++i) {
        std::string got = normalizeArabic(kApplets[i].arabic);
        if (got != std::string(kApplets[i].arabicNorm)) {
            std::printf("  FAIL: table[%zu] arabic=\"%s\" normalizeArabic=\"%s\" لكن arabicNorm=\"%s\"\n",
                        i, kApplets[i].arabic, got.c_str(), kApplets[i].arabicNorm);
            ++g_failures;
        }
    }

    // ── ٦) صحّة normalizeArabic مباشرةً ──
    // تجريد الشدّة: بدّل⇒بدل | طيّ الهمزة: أذن⇒اذن، إذن⇒اذن | ASCII يمرّ ──
    expectNorm("\xd8\xa8\xd8\xaf\xd9\x91\xd9\x84", "\xd8\xa8\xd8\xaf\xd9\x84", "norm: strip shadda");
    expectNorm("\xd8\xa3\xd8\xb0\xd9\x86", "\xd8\xa7\xd8\xb0\xd9\x86", "norm: fold أ⇒ا");
    expectNorm("\xd8\xa5\xd8\xb0\xd9\x86", "\xd8\xa7\xd8\xb0\xd9\x86", "norm: fold إ⇒ا");
    expectNorm("ls -la", "ls -la", "norm: ASCII passthrough");

    if (g_failures == 0) {
        std::printf("✓ test_applet_normalize: كلّ الحالات نجحت (تطابق تامّ + متسامح + تطبيع).\n");
        return 0;
    }
    std::printf("✗ test_applet_normalize: %d حالة فشلت.\n", g_failures);
    return 1;
}
