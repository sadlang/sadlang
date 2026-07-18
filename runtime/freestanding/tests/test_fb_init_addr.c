// ============================================================================
// test_fb_init_addr.c — اختبار وحدة للفجوة ف-١ (ربط fb_base بعنوان حقيقيّ)
// Unit test for gap F-1: binding fb_base to a real framebuffer address.
// ============================================================================
// (AR) يثبت أنّ sad_ll_fb_init_addr يوجّه مخزن الإطار إلى عنوان يوفّره النداء
//      (يحاكي عنوان GOP/VESA من المُقلِع) بدل البركة البرمجيّة، فيصل البكسل إلى
//      الذاكرة الحقيقيّة. الإثبات: نكتب بكسلات عبر واجهة ص (sad_ll_fb_*) ثم نقرؤها
//      مباشرةً من العنوان المضبوط ونؤكّد تطابق البايتات. كما يختبر دعم الخطوة
//      (pitch) الأكبر من العرض (حشو عتاديّ) وسلامة مسار البركة.
// (EN) Proves sad_ll_fb_init_addr directs the framebuffer to a caller-provided
//      address (simulating a GOP/VESA base from the bootloader) instead of the
//      software pool, so pixels reach real memory. Proof: write pixels via the
//      sad-facing API (sad_ll_fb_*) then read them straight from the configured
//      address and assert byte equality. Also tests pitch > width (hardware
//      padding) and that the pool path still works.
// ============================================================================
//
// البناء / Build (host, using the bundled LLVM clang):
//   clang runtime/freestanding/tests/test_fb_init_addr.c \
//         runtime/freestanding/sad_lowlevel_rt.c \
//         -I runtime/freestanding -o test_fb_init_addr
//   ./test_fb_init_addr    # RC 0 = كل الفحوص نجحت / all checks passed
// ============================================================================

#include <stdint.h>
#include <stdio.h>
#include "sad_lowlevel_rt.h"

static int g_failures = 0;
static int g_checks   = 0;

static void check(int cond, const char* label) {
    g_checks++;
    if (cond) {
        printf("  PASS: %s\n", label);
    } else {
        printf("  FAIL: %s\n", label);
        g_failures++;
    }
}

// ملاحظة: البيانات الحرفيّة أدناه بيانات اختبار (أبعاد/ألوان)، مستثناة من قاعدة
// «لا نصوص خام منطقيّة». / The literals below are test data (dims/colors), exempt.

#define TW   64      // عرض الاختبار / test width
#define TH   32      // ارتفاع الاختبار / test height
#define BPP  32
#define BPPX 4       // بايتات لكلّ بكسل / bytes per pixel at 32bpp

// عنوان "مخزن الإطار الحقيقيّ" الذي يوفّره المستدعي (يحاكي GOP/VESA).
// Caller-owned buffer standing in for the real framebuffer address.
static uint32_t real_fb[TW * TH];

// مخزن بخطوة محشوّة: الخطوة أكبر من العرض×البايتات (يحاكي عتادًا حقيقيًّا).
// Padded-stride buffer: pitch exceeds width*bytesPerPixel (real-HW pattern).
#define PAD_PIXELS   16
#define PAD_PITCH_B  ((TW + PAD_PIXELS) * BPPX)
static uint8_t padded_fb[PAD_PITCH_B * TH];

int main(void) {
    printf("=== test_fb_init_addr (F-1) ===\n");

    // ---------------------------------------------------------------------
    // 1) المسار الأساسيّ: توجيه fb_base إلى العنوان الحقيقيّ + قراءة البكسل منه.
    //    Core path: direct fb_base to the real address and read the pixel back.
    // ---------------------------------------------------------------------
    for (int i = 0; i < TW * TH; i++) real_fb[i] = 0xDEADBEEF; // قيمة حرّاسة
    sad_ll_fb_init_addr((int64_t)(uintptr_t)real_fb, TW, TH, TW * BPPX, BPP);

    // clear يجب أن يملأ العنوان الحقيقيّ بالكامل.
    sad_ll_fb_clear(0x00112233);
    int clear_ok = 1;
    for (int i = 0; i < TW * TH; i++)
        if (real_fb[i] != 0x00112233u) { clear_ok = 0; break; }
    check(clear_ok, "clear writes the caller-provided address (every pixel)");

    // بكسل مفرد عبر واجهة ص يُقرأ من العنوان المضبوط بموضعه الصحيح.
    sad_ll_fb_set_pixel(10, 5, 0xAABBCCDD);
    check(real_fb[5 * TW + 10] == 0xAABBCCDDu,
          "set_pixel lands at real_fb[y*width + x] at the configured address");
    check(real_fb[5 * TW + 11] == 0x00112233u,
          "neighbouring pixel is untouched (no overspill)");

    // fill_rect منطقة داخليّة.
    sad_ll_fb_fill_rect(20, 8, 4, 3, 0x01020304);
    int fill_ok = 1;
    for (int yy = 8; yy < 11; yy++)
        for (int xx = 20; xx < 24; xx++)
            if (real_fb[yy * TW + xx] != 0x01020304u) fill_ok = 0;
    check(fill_ok, "fill_rect region reaches the real address");

    // ---------------------------------------------------------------------
    // 2) حدود: كتابة خارج الشاشة تُتجاهَل (لا فيضان على العنوان الحقيقيّ).
    // ---------------------------------------------------------------------
    uint32_t before = real_fb[0];
    sad_ll_fb_set_pixel(-1, -1, 0x99999999);
    sad_ll_fb_set_pixel(TW, TH, 0x99999999);
    check(real_fb[0] == before, "out-of-bounds set_pixel is ignored");

    // ---------------------------------------------------------------------
    // 3) دعم الخطوة (pitch): خطوة أكبر من العرض×البايتات (حشو عتاديّ حقيقيّ).
    //    A real GOP framebuffer often has pitch > width*4. Pixel (x,y) must land
    //    at byte offset y*pitch + x*4 — NOT y*width*4.
    // ---------------------------------------------------------------------
    for (uint32_t i = 0; i < sizeof(padded_fb); i++) padded_fb[i] = 0x77;
    sad_ll_fb_init_addr((int64_t)(uintptr_t)padded_fb, TW, TH, PAD_PITCH_B, BPP);
    sad_ll_fb_set_pixel(3, 4, 0x0A0B0C0D);
    uint32_t* at_pitch = (uint32_t*)(padded_fb + 4 * PAD_PITCH_B + 3 * BPPX);
    uint32_t* at_wrong = (uint32_t*)(padded_fb + 4 * (TW * BPPX) + 3 * BPPX);
    check(*at_pitch == 0x0A0B0C0Du,
          "set_pixel honours pitch (byte offset y*pitch + x*4)");
    check(*at_wrong != 0x0A0B0C0Du,
          "set_pixel does NOT use width as stride when pitch is larger");

    // ---------------------------------------------------------------------
    // 4) مسار البركة البرمجيّة ما زال صالحًا (اختبار مضيفيّ بلا عنوان حقيقيّ).
    //    Software-pool path still works (host testing without a real address).
    // ---------------------------------------------------------------------
    sad_ll_fb_init(TW, TH, BPP);
    const char* rpt = sad_ll_fb_report();
    // لا نعرف عنوان البركة من الخارج؛ يكفي أن التهيئة والرسم لا يتعطّلان
    // وأنّ التقرير يظهر "Framebuffer:". Pool base is internal; assert no crash.
    sad_ll_fb_clear(0x0);
    sad_ll_fb_set_pixel(1, 1, 0xFFFFFFFF);
    int has_prefix = (rpt[0] == 'F'); // "Framebuffer: ..."
    check(has_prefix, "pool path: fb_init + report + draw succeed");

    // ---------------------------------------------------------------------
    printf("=== %d/%d checks passed ===\n", g_checks - g_failures, g_checks);
    return g_failures == 0 ? 0 : 1;
}
