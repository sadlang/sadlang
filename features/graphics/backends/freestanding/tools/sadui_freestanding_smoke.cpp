// sadui_freestanding_smoke.cpp
// المسار: features/graphics/backends/freestanding/tools/sadui_freestanding_smoke.cpp
//
// (AR) سائق اثبات: يبني شجرة «سطح_مكتب.ص» عبر واجهة C ABI لوقت التشغيل — وهي
//      عين النداءات التي يصدرها المترجم من «تشغيل_تطبيق» — ثم يستدعي sad_app_run.
//      مبنيا بـSAD_UI_FREESTANDING يسلك sad_app_run فرع fb0/evdev بلا SDL2.
//      اداة اثبات ربط وعرض حي للجسر الحر (نظير sad_desktop عبر جسر وقت التشغيل).
// ليس جزءا من اللغة او الـSoT.

#include "sad_ui_runtime.h"

#include <cstdio>

static void onDemo(void *) { std::printf("SMOKE_TAP demo\n"); std::fflush(stdout); }
static void onShell(void *) { std::printf("SMOKE_TAP shell\n"); std::fflush(stdout); }
static void onShutdown(void *) { std::printf("SMOKE_TAP shutdown\n"); std::fflush(stdout); }

int main(void)
{
    // الوان/قياسات/نصوص مطابقة لـسطح_مكتب.ص:
    const char *COLOR_BG = "#123A6E";
    const char *COLOR_TEXT_LIGHT = "#FFFFFF";
    const char *COLOR_TEXT_DIM = "#B8C6DB";
    const char *COLOR_BTN_DEMO = "#1E824C";
    const char *COLOR_BTN_TERM = "#C0392B";
    const char *COLOR_BTN_OFF = "#7A1F1F";

    const char *TITLE = "سطح مكتب نظام ص";
    const char *HINT = "اضغط زرّ الطرفيّة أو F2 للصدَفة النصّيّة";
    const char *T_DEMO = "العرض التجريبيّ";
    const char *T_TERM = "الطرفيّة";
    const char *T_OFF = "إطفاء";

    // الشجرة التصريحيّة (عين ما يبنيه المترجم من الـIR):
    SadWidget root = sad_column();
    sad_set_prop_str(root, "لون_خلفية", COLOR_BG);
    sad_set_prop_num(root, "تباعد", 48.0);
    sad_set_prop_num(root, "حشو", 64.0);

    SadWidget bar = sad_app_bar(TITLE);
    sad_add_child(root, bar);

    SadWidget title = sad_text(TITLE);
    sad_set_prop_num(title, "حجم_خط", 48.0);
    sad_set_prop_str(title, "لون", COLOR_TEXT_LIGHT);
    sad_add_child(root, title);

    SadWidget hint = sad_text(HINT);
    sad_set_prop_num(hint, "حجم_خط", 24.0);
    sad_set_prop_str(hint, "لون", COLOR_TEXT_DIM);
    sad_add_child(root, hint);

    SadWidget bDemo = sad_button(T_DEMO, onDemo, nullptr);
    sad_set_prop_str(bDemo, "لون_خلفية", COLOR_BTN_DEMO);
    sad_set_prop_num(bDemo, "حجم_خط", 32.0);
    sad_add_child(root, bDemo);

    SadWidget bTerm = sad_button(T_TERM, onShell, nullptr);
    sad_set_prop_str(bTerm, "لون_خلفية", COLOR_BTN_TERM);
    sad_set_prop_num(bTerm, "حجم_خط", 32.0);
    sad_add_child(root, bTerm);

    SadWidget bOff = sad_button(T_OFF, onShutdown, nullptr);
    sad_set_prop_str(bOff, "لون_خلفية", COLOR_BTN_OFF);
    sad_set_prop_num(bOff, "حجم_خط", 32.0);
    sad_add_child(root, bOff);

    std::printf("SMOKE_BUILD_OK\n");
    std::fflush(stdout);

    // تشغيل التطبيق: يسلك فرع fb0/evdev الحر (SAD_UI_FREESTANDING):
    sad_app_run(root);

    std::printf("SMOKE_EXIT\n");
    std::fflush(stdout);
    return 0;
}
