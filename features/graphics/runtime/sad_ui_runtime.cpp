/* ============================================================================
 * sad_ui_runtime.cpp — تنفيذ مكتبة وقت التشغيل لواجهات ص
 * Sad UI Runtime Implementation — C API bridging to sad_ui C++ library
 * ============================================================================
 *
 * (AR) هذا الملف يُنفّذ دوال C التي يستدعيها الكود المُترجم بـ LLVM.
 *      يربط بين الدوال البسيطة (sad_column, sad_text, ...) ومكتبة sad_ui
 *      التي تحتوي على IRNode و PlatformRenderer و Layout.
 *
 * (EN) This file implements the C functions called by LLVM-compiled code.
 *      It bridges simple C functions to the sad_ui C++ library containing
 *      IRNode, PlatformRenderer, and Layout systems.
 *
 * المسار: runtime/sad_ui_runtime.cpp
 * يتطابق مع: compiler/src/backend/llvm/llvm_codegen_ui.cpp
 *
 * ============================================================================
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ============================================================================
 */

#include "sad_ui_runtime.h"

#include <sad_ui/ir.h>
#include <sad_ui/ir_builder.h>
#include "sad_ui/event_dispatch.h" // (rfcs#51) stopActiveEventPropagation
#include <sad_ui/prop_keys.h> // مفاتيح الخصائص القانونيّة (SoT) — لا literals خام
#include <sad_ui/print_tree.h> // (AR) م-تحكّم: طباعة_شجرة عبر منطق المكتبة المشترك
#include <sad_ui/layout.h>
#include <sad_ui/platform_renderer.h>
#include <sad_ui/node.h>
#include <sad_ui/nav.h> // (AR) م-تحكّم: مكدّس التنقّل المشترك (مصدر الحقيقة)
#include <sad_ui/window_control.h> // (م-تحكّم) عنوان/إغلاق النافذة عبر المتحكّم المشترك
#include <sad_ui/web/html_codegen.h> // (م-تحكّم) توليد_ويب عبر مولّد HTML المكتبيّ
#include <sad_ui/types.h>
// (AR) SoT مفاتيح الخصائص (props::PADDING…) — هيدر مولَّد ذاتيّ الاكتفاء بلا اعتماد
//      خلفيّة، يُضمَّن دون قيد كي تراه كتّاب المفاتيح المستضافون (sad_set_padding)
//      لا الفرع الحرّ وحده — نظير تضمينه غير المشروط في core/src/types.cpp.
// (EN) SoT property keys (props::PADDING…) — self-contained generated header, included
//      unconditionally so hosted key writers (sad_set_padding) see it too, not only the
//      freestanding branch — mirroring the unconditional include in core/src/types.cpp.
#include <sad_ui/prop_keys.h>

#ifdef SAD_UI_USE_SDL2
#include <sad_ui/desktop/renderer.h>
#include <sad_ui/desktop/window.h>
#endif

// (AR) الفرع الحرّ (fb0/evdev): جسر تشغيل التطبيق المستقلّ — بديل SDL2 على لينكس
//      بلا X11. آليّة الحلقة كاملةً في المكتبة (app_runner)؛ هذا الجسر يربط فقط.
// (EN) Freestanding branch (fb0/evdev): the SDL2 alternative on Linux without X11.
#if defined(SAD_UI_FREESTANDING) && defined(__linux__)
#include <sad_ui/freestanding/app_runner.h>
#endif

#include <string>
#include <vector>
#include <memory>
#include <algorithm> // (م1-ب) remove_if لحصاد أجيال البانِي (تفادي تسريب النبضة)
#include <chrono>  // (م1-ب) نبضة الساعة الحيّة: system_clock لبوّابة الدقيقة الجداريّة
#include <cstdint>
#include <cstring>
#include <cstdio>  // (AR) fprintf لإعلان فشل تهيئة الوضع الحرّ على stderr
#include <cstdlib> // (م-تحكّم) malloc لنسخة نصّ توليد_ويب المملوكة للمستدعي
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unordered_map>

using namespace sad::ui;

/* ============================================================================
 * بنية العنصر الداخلية / Internal Widget Structure
 * ============================================================================
 * كل SadWidget هو مؤشر إلى هذه البنية. تحتوي على عقدة IR + خصائص إضافية.
 */
struct SadWidgetImpl {
    std::shared_ptr<IRNode> irNode;
    std::vector<SadWidgetImpl*> children;
    SadWidgetImpl* parent = nullptr;
    SadCallback onTap = nullptr;
    void* userData = nullptr;

    // (م1-ب) جيل الإنشاء: يُختَم من g_widgetGeneration وقت createWidget. نموذج البانِي
    //        (تشغيل_تطبيق(دالّة)) يبني شجرةً بجيلٍ جديد كلّ نبضة، ويحصد جيلَ الشجرة
    //        المُستبدَلة بعد عرض الجديدة ⇒ لا يتراكم g_widgets بلا حدّ. الجيل 0 =
    //        عناصر مستوى الوحدة/اللقطة (لا تُحصَد أبدًا). انظر reapWidgetGeneration.
    uint64_t generation = 0;

    // (AR) ربط الأحداث الانسيابيّة عند_* (م-أ3ر، L2): اسم الحدث ⇒ ردّ نداء + بيانات.
    //      الحدث نفسه يُسجَّل على irNode (تطابق getEvents مع المفسّر)؛ هذا الجدول
    //      يحمل ردّ النداء المُترجَم للإرسال (لا يحمله IREvent الذي يخزّن تعبيرًا نصّيًّا).
    struct EventBinding {
        std::string name;
        IREventType type = IREventType::Custom; // (AR) مُشتقٌّ مرّةً عند الربط (لا يُعاد اشتقاقه لكلّ إرسال)
        SadEventCallback cb = nullptr;          // (② rfcs#46) ردّ نداءٍ حامل POD الحدث
        void* data = nullptr;
        // (AR) مُعرِّفٌ فريدٌ يطابق IREvent::expression على العقدة. المُرسِل المشترك
        //      (dispatchEvent) يختار المعالِج ويمرّر تعبيره؛ فنُطلق الرابطَ المطابق
        //      وحده لا كلَّ روابط النوع — وإلّا تعطّلت تصفية الطور واختلطت البيانات.
        std::string expression;
    };
    std::vector<EventBinding> eventBindings;

    // (AR) حالة سلسلة التحريك (م-أ3ر، L3): تحاكي WidgetBuilder في المفسّر.
    //      animActive: تحريكٌ نشطٌ قابلٌ للتعديل (نظير hasActiveAnimation_)؛
    //      animCompound: وضع المجموعة المركّبة؛
    //      animCompoundStart: فهرس بداية المجموعة (نظير compoundStartIndex_ في
    //      widget_builder.h) — size_t غير موقَّع؛ حارسُ animApplyToActive يعالج
    //      أيّ التفافٍ محتمل. يُزامَن يدويًّا مع نظيره في المفسّر.
    bool   animActive = false;
    bool   animCompound = false;
    size_t animCompoundStart = 0;

    // (AR) مُجمِّع الخاصّيّة متعدّدة الوسائط (م-أ3ر): قيمٌ مُنسَّقة تُدمَج بفواصل عند commit.
    std::vector<std::string> joinAccum;

    SadWidgetImpl(UINodeType type) {
        irNode = std::make_shared<IRNode>(type);
    }

    ~SadWidgetImpl() = default;
};

/* ============================================================================
 * بنية التطبيق الداخلية / Internal App Structure
 * ============================================================================
 */
struct SadAppImpl {
    SadWidgetImpl* root = nullptr;
    std::unique_ptr<PlatformRenderer> renderer;
    std::shared_ptr<LayoutResult> layoutResult;
    bool initialized = false;
    float width = 800.0f;
    float height = 600.0f;
    
    SadAppImpl() = default;
    ~SadAppImpl() {
        if (renderer) {
            renderer->shutdown();
        }
    }
};

/* ── سجل العناصر المُنشأة لإدارة الذاكرة / Created widgets registry ── */
static std::vector<std::unique_ptr<SadWidgetImpl>> g_widgets;
static std::vector<std::unique_ptr<SadAppImpl>> g_apps;

/* ── (م1-ب) جيل الإنشاء الحاليّ: يبدأ 0 (مستوى الوحدة/اللقطة)؛ نموذج البانِي يزيده
 *    قبل كلّ بناء شجرةٍ ليختم عناصرها بجيلٍ متفرّد، فيُحصَد جيل الشجرة المُستبدَلة
 *    وحده دون مساس بالجيل الحاليّ أو الجيل 0. ── */
static uint64_t g_widgetGeneration = 0;

/* ── دوال مساعدة / Helper functions ── */

static SadWidgetImpl* createWidget(UINodeType type) {
    auto widget = std::make_unique<SadWidgetImpl>(type);
    SadWidgetImpl* ptr = widget.get();
    ptr->generation = g_widgetGeneration; // (م1-ب) ختمُ الجيل للحصاد اللاحق
    g_widgets.push_back(std::move(widget));
    return ptr;
}

/* ── (م1-ب) حصاد جيلٍ من العناصر: يحرّر كلّ SadWidgetImpl مختومٍ بـgen. يُستدعى بعد
 *    عرض شجرةٍ جديدة (setContent) لتحرير الشجرة المُستبدَلة في نموذج البانِي، فلا
 *    يتراكم g_widgets مع النبضة الدوريّة. الجيل 0 (مستوى الوحدة/اللقطة) لا يُحصَد.
 *    آمنٌ بعد setContent: الشجرة المُستبدَلة لم تعُد معروضة ولا يشير إليها إلّا ما
 *    نحرّره (أشجار IR جديدة كاملة بلا مشاركةٍ مع الجديدة). المؤشّرات الباقية تظلّ
 *    صالحة (erase على unique_ptr لا يزحزح الكائنات الباقية). ── */
static void reapWidgetGeneration(uint64_t gen) {
    if (gen == 0) return; // الجيل 0 = عناصر دائمة (مستوى الوحدة/اللقطة)
    g_widgets.erase(
        std::remove_if(g_widgets.begin(), g_widgets.end(),
            [gen](const std::unique_ptr<SadWidgetImpl>& up) {
                return up && up->generation == gen;
            }),
        g_widgets.end());
}

static SadWidgetImpl* toWidget(SadWidget w) {
    return static_cast<SadWidgetImpl*>(w);
}

/* ── تحصين فشل-مُغلق (تحصين أميليا): هل المؤشّر عنصرٌ مُسجَّل فعلًا؟ ──
 * (AR) باني الحاويات في المترجم يُصدر sad_add_child لكلّ وسيطٍ نوعه Pointer، وحارس
 *      النوع Pointer أضعف من isWidgetBuilder في المفسّر: قد يمرّر مؤشّرًا غير-عنصر
 *      (كائن صنف / &x). دوال إدارة الشجرة تمرّره عبر toWidget (static_cast أعمى)
 *      فتقرأ حقول عنصرٍ من مؤشّرٍ ليس عنصرًا ⇒ UB/إفساد ذاكرة. نتحقّق أنّ المؤشّر
 *      مُسجَّل في g_widgets (نظير createWidget) قبل أيّ استعمال — وإلّا تجاهل آمن
 *      (نظير تجاهل المفسّر لغير-العنصر). O(n) مقبولٌ لأحجام UI الصغيرة، ويبطل الـUB
 *      نهائيًّا للمحرّكين (المُستضاف والحرّ) دون حاجة لنوع «عنصر» في SIR.
 * (EN) Fail-closed guard: verify the pointer is a registered widget in g_widgets
 *      before the blind static_cast in toWidget — else ignore safely. */
static bool isRegisteredWidget(const void* p) {
    if (!p) return false;
    for (const auto& up : g_widgets) {
        if (up.get() == p) return true;
    }
    return false;
}

static SadAppImpl* toApp(SadApp a) {
    return static_cast<SadAppImpl*>(a);
}

static void setStringProperty(SadWidgetImpl* w, const std::string& name,
                              const std::string& value) {
    if (!w || !w->irNode) return;
    w->irNode->setProperty(name, value);
}

static void setFloatProperty(SadWidgetImpl* w, const std::string& name,
                             double value) {
    if (!w || !w->irNode) return;
    w->irNode->setProperty(name, value);
}

static void setIntProperty(SadWidgetImpl* w, const std::string& name,
                           int64_t value) {
    if (!w || !w->irNode) return;
    w->irNode->setProperty(name, value);
}

/* ── سلسلة التحريك (م-أ3ر، L3): محاكاة آلة حالة WidgetBuilder ──
 * نظائر مباشرة لـwidget_builder.cpp؛ تعمل على نفس IRAnimation/IRNode المشترك
 * فيتطابق getAnimations() بين المحرّكين. */

// (AR) نظير commitAnimation: التحريكات التالية تبدأ مجموعة جديدة.
static void animCommit(SadWidgetImpl* w) {
    w->animActive = false;
    w->animCompound = false;
}

// (AR) نظير WidgetBuilder::applyToActiveAnimations (widget_builder.cpp:~226): يطبّق
//      على [animCompoundStart .. النهاية). يُزامَن يدويًّا مع نظيره في المفسّر.
template <typename F>
static void animApplyToActive(SadWidgetImpl* w, F modifier) {
    if (!w || !w->irNode) return;
    auto& anims = w->irNode->getMutableAnimations();
    size_t start = w->animCompoundStart;
    if (start > anims.size())
        start = anims.empty() ? 0 : anims.size() - 1;
    for (size_t i = start; i < anims.size(); ++i)
        modifier(anims[i]);
}

// (AR) تنسيق عددٍ عشريّ نظير Value::toString (value.cpp:~454): fixed بدقّة 6 ثمّ حذف
//      الأصفار الزائدة مع إبقاء رقمٍ واحدٍ بعد النقطة (10.0 لا 10) — لتطابق المفسّر
//      في الخاصّيّة متعدّدة الوسائط.
//      ⚠ نسخةٌ يدويّة من منطق value.cpp: **يجب مزامنتها يدويًّا** عند أيّ تغييرٍ
//        لتنسيق Value::toString (مثل setprecision) وإلّا انجرف مخرَج المترجم صمتًا
//        عن المفسّر في القيم العشريّة متعدّدة الوسائط.
static std::string formatNumLikeValueToString(double v) {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(6) << v;
    std::string r = oss.str();
    size_t dot = r.find('.');
    if (dot != std::string::npos) {
        size_t last = r.find_last_not_of('0');
        if (last <= dot) last = dot + 1;
        r.erase(last + 1);
    }
    return r;
}

// (AR) نظير startAnimationChain: ينهي تحريكًا مفردًا سابقًا (لا مركّبًا)،
//      يضبط بداية المجموعة، ثم يُنشئ تحريكًا بالنوع المعطى ويُضيفه.
static void animStartChain(SadWidgetImpl* w, const std::string& typeName) {
    if (!w || !w->irNode) return;
    if (w->animActive && !w->animCompound)
        animCommit(w);
    if (!w->animCompound)
        w->animCompoundStart = w->irNode->getAnimations().size();
    IRAnimation anim{};
    anim.type = stringToAnimationType(typeName);
    w->irNode->addAnimation(anim);
    w->animActive = true;
}

// (② rfcs#46) POD تخطيط الحدث + دالّة تعبئته من EventData — مصدر حقيقة مُولَّد.
//   يجب أن يُضمَّن بربط C++ الافتراضيّ (الرأس يضمّ <array> وقوالب) خارج أيّ كتلة
//   extern "C"، وبعد types.h (السطر 34) كي تتوفّر sad::ui::EventData لـsadFillEventPod.
#define SAD_EVENT_POD_WITH_EVENTDATA
#include "sad_event_layout_generated.h"

/* ═══════════════════════════════════════════════════════════════════════════
 * 1. مصانع العناصر / Widget Factories
 * ═══════════════════════════════════════════════════════════════════════════ */

extern "C" {

// (② rfcs#46، إصلاح Amelia ع-2) نسخةٌ مملوكة من نصّ حدثٍ: يستدعيها ثانكُ المترجم لكلّ
//   حقلٍ نصّيّ فيؤمّن عمرَه بعد رجوع الإرسال (POD يحمل c_str() لـstd::string داخل EventData
//   المحلّيّة فتتدلّى بالعودة). نصوص ص تُدار بلا مُحرِّر في المترجم (تُسرَّب كنموذج اللغة
//   الحاليّ) فالنسخة متّسقة معه. الفارغ ⇒ "" ساكن مشترك (لا تخصيص) تفاديًا لتسرّبٍ لكلّ
//   إطار سحب. ⚠ غير مشروطة بـSAD_UI_USE_SDL2/الحرّ (خلافًا لـdispatchCompiledEvent): المترجم
//   يُصدر نداءها لأيّ برنامجٍ فيه معالِج حدثٍ يقرأ نصًّا، فيلزم تعريفها في كلّ بناءٍ لمكتبة
//   الرسومات (نظير مصانع العناصر). صفرُ اعتمادٍ على SDL2/الحرّ (<cstdlib>/<cstring> فقط).
const char* sad_event_dup_str(const char* s) {
    if (!s || !*s) { static const char kEmpty[] = ""; return kEmpty; }
    const std::size_t n = std::strlen(s);
    char* p = static_cast<char*>(std::malloc(n + 1));
    if (!p) { static const char kEmpty[] = ""; return kEmpty; } // فشل تخصيص ⇒ فارغ آمن
    std::memcpy(p, s, n + 1);
    return p;
}

SadWidget sad_column(void) {
    return createWidget(UINodeType::Column);
}

SadWidget sad_row(void) {
    return createWidget(UINodeType::Row);
}

SadWidget sad_stack(void) {
    return createWidget(UINodeType::Stack);
}

SadWidget sad_container(void) {
    return createWidget(UINodeType::Container);
}

SadWidget sad_text(const char* text) {
    auto* w = createWidget(UINodeType::Text);
    if (text) {
        // (AR) مفتاح واحد «محتوى» مطابقةً للمفسّر (widget_builtins.cpp:109)؛
        //      المُرسِّم يقرأه احتياطيًّا (text←محتوى←نص) فالرسم يعمل بلا aliases.
        setStringProperty(w, "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", text); // محتوى
    }
    return w;
}

SadWidget sad_text_styled(const char* text, float fontSize,
                          float r, float g, float b, float a) {
    auto* w = createWidget(UINodeType::Text);
    if (text) {
        setStringProperty(w, "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", text); // محتوى (كالمفسّر)
    }
    setFloatProperty(w, "\xd8\xad\xd8\xac\xd9\x85_\xd8\xae\xd8\xb7", fontSize);  // حجم_خط
    
    // تحويل اللون إلى hex string #RRGGBBAA
    char colorHex[10];
    snprintf(colorHex, sizeof(colorHex), "#%02X%02X%02X%02X",
             (int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
    setStringProperty(w, "\xd9\x84\xd9\x88\xd9\x86_\xd8\xa7\xd9\x84\xd9\x86\xd8\xb5", colorHex);  // لون_النص
    return w;
}

SadWidget sad_button(const char* label, SadCallback onTap, void* userData) {
    auto* w = createWidget(UINodeType::Button);
    if (label) {
        // (AR) مفتاح واحد «عنوان» مطابقةً للمفسّر (widget_builtins.cpp:127)؛
        //      المُرسِّم يقرأه احتياطيًّا (text←نص←عنوان) فالرسم يعمل بلا aliases.
        setStringProperty(w, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", label);    // عنوان
    }
    w->onTap = onTap;
    w->userData = userData;
    return w;
}

SadWidget sad_button_variant(const char* label, int32_t variant,
                             float r, float g, float b, float a,
                             SadCallback onTap, void* userData) {
    auto* w = createWidget(UINodeType::Button);
    if (label) {
        setStringProperty(w, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", label);  // عنوان (كالمفسّر)
    }
    setIntProperty(w, "\xd9\x86\xd9\x88\xd8\xb9", variant);  // نوع
    
    char colorHex[10];
    snprintf(colorHex, sizeof(colorHex), "#%02X%02X%02X%02X",
             (int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
    setStringProperty(w, "\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9", colorHex);  // لون_خلفية
    
    w->onTap = onTap;
    w->userData = userData;
    return w;
}

SadWidget sad_icon_button(const char* icon, SadCallback onTap, void* userData) {
    auto* w = createWidget(UINodeType::Button);
    if (icon) {
        setStringProperty(w, "\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", icon);  // أيقونة
    }
    w->onTap = onTap;
    w->userData = userData;
    return w;
}

SadWidget sad_fab(const char* icon,
                  float r, float g, float b, float a,
                  SadCallback onTap, void* userData) {
    // (AR) FAB (لا Button) + مفتاح «أيقونة» وحده — يطابق المفسّر
    //      (widget_builtins.cpp:131 MAKE_WIDGET_WITH_PROP_FN(FAB,"أيقونة") ⇒
    //      UINodeType::FAB، اسم «زر_عائم»). اللون/الظلّ/الزوايا من اختصاص المُرسِّم
    //      لا التخزين (تفادي تباعد طباعة_شجرة؛ نظير sad_card). r/g/b/a يبقيان في
    //      التوقيع للـABI لكن يُهمَلان هنا.
    (void)r; (void)g; (void)b; (void)a;
    auto* w = createWidget(UINodeType::FAB);
    if (icon) {
        setStringProperty(w, "\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", icon);
    }
    w->onTap = onTap;
    w->userData = userData;
    return w;
}

SadWidget sad_text_field(const char* hint, SadCallback onChange, void* userData) {
    auto* w = createWidget(UINodeType::TextField);
    if (hint) {
        setStringProperty(w, "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad", hint);  // تلميح
    }
    w->onTap = onChange;  // reuse callback slot
    w->userData = userData;
    return w;
}

SadWidget sad_checkbox(SadCallback onChanged, void* userData) {
    auto* w = createWidget(UINodeType::Checkbox);
    w->onTap = onChanged;
    w->userData = userData;
    return w;
}

SadWidget sad_switch_toggle(SadCallback onChanged, void* userData) {
    auto* w = createWidget(UINodeType::Toggle);
    w->onTap = onChanged;
    w->userData = userData;
    return w;
}

SadWidget sad_slider(float minVal, float maxVal,
                     SadCallback onChanged, void* userData) {
    // (AR) عقدة Slider فارغة — يطابق المفسّر (منزلق() ⇒ بلا خصائص). مفتاح «قيمة»
    //      يُضبَط لاحقًا عبر SET_PROP من الخافض (لا «أدنى/أقصى»). minVal/maxVal
    //      يبقيان في التوقيع للـABI لكن يُهمَلان (المدى من اختصاص المُرسِّم).
    (void)minVal; (void)maxVal;
    auto* w = createWidget(UINodeType::Slider);
    w->onTap = onChanged;
    w->userData = userData;
    return w;
}

SadWidget sad_card(void) {
    // (AR) بلا خصائص افتراضيّة — يطابق المفسّر (بطاقة() ⇒ عقدةٌ بلا خصائص).
    //      الظلّ/الزوايا الافتراضيّان من اختصاص المُرسِّم لا التخزين (تفادي تباعد
    //      «طباعة_شجرة»؛ نظير إزالة aliases المصانع الأخرى).
    return createWidget(UINodeType::Card);
}

SadWidget sad_scaffold(void) {
    return createWidget(UINodeType::Scaffold);
}

SadWidget sad_app_bar(const char* title) {
    auto* w = createWidget(UINodeType::AppBar);
    if (title) {
        setStringProperty(w, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", title);  // عنوان (كالمفسّر؛ المُرسِّم يقرأه احتياطيًّا)
    }
    return w;
}

SadWidget sad_spacer(void) {
    return createWidget(UINodeType::Spacer);
}

SadWidget sad_divider(void) {
    return createWidget(UINodeType::Divider);
}

SadWidget sad_dialog(const char* title, const char* message) {
    auto* w = createWidget(UINodeType::Dialog);
    if (title) {
        setStringProperty(w, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", title);
    }
    // (AR) نضبط «رسالة» فقط إن كانت غير فارغة — يطابق المفسّر (حوار(عنوان) يضبط
    //      «عنوان» وحده). الوسيط الغائب يصل كسلسلةٍ فارغة لا null، فنحرس *message.
    if (message && *message) {
        setStringProperty(w, "\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9", message);  // رسالة
    }
    return w;
}

SadWidget sad_image(const char* source) {
    auto* w = createWidget(UINodeType::Image);
    if (source) {
        setStringProperty(w, "\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1", source);  // مصدر (كالمفسّر؛ المُرسِّم يقرأه أوّلًا)
    }
    return w;
}

/* ─── مصانع إضافيّة (م-مصانع) — تطابق تسجيل المفسّر في widget_builtins.cpp ───
 * (AR) كانت هذه العناصر مدعومة في المفسّر فقط؛ نضيف نظيرها في وقت تشغيل المترجم.
 * (EN) Interpreter-only widgets; we add their compiler-runtime counterparts. */

SadWidget sad_icon(const char* name) {
    auto* w = createWidget(UINodeType::Icon);
    if (name) {
        setStringProperty(w, "\xd8\xa7\xd8\xb3\xd9\x85", name);  // اسم
    }
    return w;
}

SadWidget sad_text_button(const char* label) {
    auto* w = createWidget(UINodeType::Button);
    if (label) {
        setStringProperty(w, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", label);  // عنوان (كالمفسّر)
    }
    return w;
}

SadWidget sad_snackbar(const char* message) {
    auto* w = createWidget(UINodeType::SnackBar);
    if (message) {
        setStringProperty(w, "\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9", message);  // رسالة
    }
    return w;
}

SadWidget sad_tooltip(const char* text) {
    auto* w = createWidget(UINodeType::Tooltip);
    if (text) {
        setStringProperty(w, "\xd9\x86\xd8\xb5", text);  // نص
    }
    return w;
}

SadWidget sad_text_area(const char* hint) {
    auto* w = createWidget(UINodeType::TextArea);
    if (hint) {
        setStringProperty(w, "\xd8\xaa\xd9\x84\xd9\x85\xd9\x8a\xd8\xad", hint);  // تلميح
    }
    return w;
}

SadWidget sad_progress(float value) {
    auto* w = createWidget(UINodeType::ProgressBar);
    setFloatProperty(w, "\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9", value);  // قيمة
    return w;
}

SadWidget sad_sized_box(float width, float height) {
    auto* w = createWidget(UINodeType::SizedBox);
    setFloatProperty(w, "\xd8\xb9\xd8\xb1\xd8\xb6", width);                   // عرض
    setFloatProperty(w, "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", height); // ارتفاع
    return w;
}

SadWidget sad_grid(void)        { return createWidget(UINodeType::Grid); }
SadWidget sad_center(void)      { return createWidget(UINodeType::Center); }
SadWidget sad_padding(void)     { return createWidget(UINodeType::Padding); }
SadWidget sad_align(void)       { return createWidget(UINodeType::Align); }
SadWidget sad_expanded(void)    { return createWidget(UINodeType::Expanded); }
SadWidget sad_flexible(void)    { return createWidget(UINodeType::Flexible); }
SadWidget sad_wrap(void)        { return createWidget(UINodeType::Wrap); }
SadWidget sad_box(void)         { return createWidget(UINodeType::Box); }
SadWidget sad_scroll_view(void) { return createWidget(UINodeType::ScrollView); }
SadWidget sad_bottom_nav(void)  { return createWidget(UINodeType::BottomNav); }
SadWidget sad_lazy_column(void) { return createWidget(UINodeType::LazyColumn); }
SadWidget sad_lazy_row(void)    { return createWidget(UINodeType::LazyRow); }
SadWidget sad_list_view(void)   { return createWidget(UINodeType::List); }
SadWidget sad_drawer(void)      { return createWidget(UINodeType::Drawer); }
SadWidget sad_safe_area(void)   { return createWidget(UINodeType::SafeArea); }
SadWidget sad_surface(void)     { return createWidget(UINodeType::Surface); }

/* ─── أثر المعدّلات الانسيابيّة (م-أ3ر، L1): خاصّيّة عامّة بالاسم ───
 * (AR) نظير setIRProperty في المفسّر: اسم الطريقة = مفتاح الخاصيّة على IRNode
 *      المشترك نفسه، فالأثر في شجرة العناصر مطابق للمحرّكين. */
// (AR) معدّل خاصّيّة عامّ يُنهي سلسلة التحريك النشطة قبل الضبط — نظير المفسّر
//      (ui_widget_method_call.cpp:220: commitAnimation قبل أيّ معدّل غير تحريكيّ).
//      الأحداث لا تُنهيها (فرع عند_* يعود مبكّرًا) فلا commit في sad_add_event.
void sad_set_prop_str(SadWidget w, const char* name, const char* value) {
    auto* impl = toWidget(w);
    if (impl) animCommit(impl);
    if (impl && impl->irNode && name)
        impl->irNode->setProperty(name, std::string(value ? value : ""));
}
void sad_set_prop_int(SadWidget w, const char* name, int64_t value) {
    auto* impl = toWidget(w);
    if (impl) animCommit(impl);
    if (impl && impl->irNode && name)
        impl->irNode->setProperty(name, static_cast<int64_t>(value)); // متغاير int64_t كالمفسّر
}
void sad_set_prop_num(SadWidget w, const char* name, double value) {
    auto* impl = toWidget(w);
    if (impl) animCommit(impl);
    if (impl && impl->irNode && name)
        impl->irNode->setProperty(name, value);
}
void sad_set_prop_bool(SadWidget w, const char* name, int32_t value) {
    auto* impl = toWidget(w);
    if (impl) animCommit(impl);
    if (impl && impl->irNode && name)
        impl->irNode->setProperty(name, value != 0);
}

/* ─── ربط حدث انسيابيّ عند_* (م-أ3ر، L2) ───
 * (AR) نظير addIREvent في المفسّر: نُسجّل IREvent على العقدة المشتركة (النوع
 *      يُشتقّ بـstringToIREventType من القلب فالتطابق دقيق)، فيتطابق getEvents()
 *      بين المحرّكين. ونحفظ ردّ النداء المُترجَم في جدول العنصر لإرسالٍ مستقبليّ
 *      — لا مُرسِل في وقت تشغيل المترجم بعدُ يقرأ onTap/eventBindings (شريحة لاحقة). */
void sad_add_event(SadWidget w, const char* name, SadEventCallback cb, void* data) {
    auto* impl = toWidget(w);
    if (!impl || !impl->irNode || !name) return;
    const std::string evName(name);
    IREvent ev;
    ev.type = stringToIREventType(evName);
    if (ev.type == IREventType::Custom)
        ev.customEventName = evName;
    // (AR) تعبيرٌ **فريدٌ لكلّ رابط** (لا ثابت "__compiled_cb"): المُرسِل المشترك
    //      dispatchEvent يختار المعالِج (بالطور) ويمرّر تعبيره للمُصرِف، فيُطلق
    //      dispatchCompiledEvent الرابطَ المطابق وحده. الثابتُ القديم كان يجعل
    //      المُصرِف يُعيد الاختيار بالنوع فيُطلق كلّ روابط النوع — مبطلًا تصفية
    //      الطور وخالطًا بيانات المعالِجات ومضاعِفًا الإطلاق (N²). التعبير ليس
    //      جزءًا من مخرَج التكافؤ (طباعة_شجرة تقرأ getEventName لا expression).
    static uint64_t s_cbCounter = 0;
    ev.expression = "__cb_" + std::to_string(s_cbCounter++);
    impl->irNode->addEvent(ev);
    impl->eventBindings.push_back({evName, ev.type, cb, data, ev.expression}); // النوع + المُعرِّف الفريد
    // (② rfcs#46) أُزيل شيم onTap القديم: eventBindings صار مصدر الحقيقة الوحيد لأحداث
    //   عند_* في مسار المترجم (dispatchCompiledEvent يمرّ عليها ثمّ onTap كاحتياطٍ عند
    //   !fired فقط)، ولأنّ ردّ النداء الآن SadEventCallback ثنائيّ الوسيط لا يُسند لخانة
    //   onTap أحاديّة الوسيط (SadCallback المصنعيّة). هذا هو ما أوصى به TODO الحظر سابقًا.
}

/* ─── سلسلة التحريك (م-أ3ر، L3) ───
 * (AR) نظائر معدّلات WidgetBuilder في المفسّر (ui_widget_method_call.cpp). */

/** .حرّك(أنواع) — يبدأ سلسلة تحريك. الأنواع مفصولة بفواصل (مركّب). */
void sad_anim_begin(SadWidget w, const char* typesCsv) {
    auto* impl = toWidget(w);
    if (!impl || !impl->irNode) return;
    // (AR) فكّ السلسلة المفصولة بفواصل (تنظيف المسافات) — نظير جمع المفسّر للأنواع.
    std::vector<std::string> types;
    if (typesCsv) {
        std::string cur;
        for (const char* p = typesCsv; ; ++p) {
            if (*p == ',' || *p == '\0') {
                std::string part;
                for (char c : cur) if (c != ' ' && c != '\t') part += c;
                if (!part.empty()) types.push_back(part);
                cur.clear();
                if (*p == '\0') break;
            } else {
                cur += *p;
            }
        }
    }
    // (AR) احتياطيّ كالمفسّر: النوع الافتراضيّ (FadeIn) مشتقٌّ من مصدر الحقيقة
    //   عبر animationTypeToString (لا ليترال إنجليزيّ؛ يعيد الاسم القانونيّ «ظهور»).
    if (types.empty()) types.push_back(sad::ui::animationTypeToString(sad::ui::AnimationType::FadeIn));
    // (AR) أكثر من نوع ⇒ مجموعة مركّبة (المعدّلات تُطبَّق على الكلّ).
    const bool isCompound = types.size() > 1;
    if (isCompound) {
        if (impl->animActive) animCommit(impl);
        impl->animCompound = true;
        impl->animCompoundStart = impl->irNode->getAnimations().size();
    }
    for (const auto& t : types) animStartChain(impl, t);
    if (isCompound) impl->animCompound = false; // endCompoundBatch
}

/** .مدة(ث) — مدة التحريك بالثواني (على المجموعة النشطة). */
void sad_anim_duration(SadWidget w, double seconds) {
    auto* impl = toWidget(w);
    if (impl && impl->animActive)
        animApplyToActive(impl, [seconds](IRAnimation& a){ a.duration = static_cast<float>(seconds); });
}

/** .منحنى(اسم) — منحنى التحريك الزمنيّ. */
void sad_anim_easing(SadWidget w, const char* name) {
    auto* impl = toWidget(w);
    if (impl && impl->animActive && name) {
        auto curve = stringToEasingCurve(name);
        animApplyToActive(impl, [curve](IRAnimation& a){ a.easing = curve; });
    }
}

/** .تفرع(طور) — طور انتشار **آخر** معالِجٍ سُجِّل على العقدة (rfcs#51).
 *  نظيرٌ دقيق لـWidgetBuilder::setLastEventPropagation في المفسّر: بلا معالِجٍ
 *  سابق لا أثر (معدّلٌ بلا هدف)، واسمٌ مجهول ⇒ لا_انتشار (فشل-آمن). */
void sad_set_event_phase(SadWidget w, const char* phase) {
    auto* impl = toWidget(w);
    if (!impl || !impl->irNode || !phase) return;
    auto& events = impl->irNode->getMutableEvents();
    if (events.empty()) return;
    events.back().propagation = stringToEventPropagation(std::string(phase));
}

/** بيانات المستخدم لآخر معالِجٍ سُجِّل — تصل الحقلَ «بيانات» في بنية «حدث». */
void sad_set_event_data(SadWidget w, const char* text) {
    auto* impl = toWidget(w);
    if (!impl || !impl->irNode || !text) return;
    auto& events = impl->irNode->getMutableEvents();
    if (events.empty()) return;
    events.back().userData = std::string(text);
}

/** .تأخير(ث) — التأخير قبل البدء. */
void sad_anim_delay(SadWidget w, double seconds) {
    auto* impl = toWidget(w);
    if (impl && impl->animActive)
        animApplyToActive(impl, [seconds](IRAnimation& a){ a.delay = static_cast<float>(seconds); });
}

/** .تكرار(ن) — عدد التكرارات. */
void sad_anim_repeat(SadWidget w, int32_t count) {
    auto* impl = toWidget(w);
    if (impl && impl->animActive)
        animApplyToActive(impl, [count](IRAnimation& a){ a.repeatCount = static_cast<int>(count); });
}

/** .عكس_تلقائي([منطقي]) — عكس الحركة تلقائيًّا. */
void sad_anim_auto_reverse(SadWidget w, int32_t reverse) {
    auto* impl = toWidget(w);
    if (impl && impl->animActive) {
        const bool rev = reverse != 0;
        animApplyToActive(impl, [rev](IRAnimation& a){ a.autoReverse = rev; });
    }
}

/* ─── خاصّيّة متعدّدة الوسائط (م-أ3ر): دمج بفواصل ───
 * (AR) كلّ add يُنسّق وسيطًا (نظير Value::toString) ويُضيفه لمجمِّع العنصر؛ commit يدمج
 *      بفواصل ⇒ خاصّيّة نصّيّة على نفس IRNode (تطابق ui_widget_method_call.cpp:239-249). */
void sad_prop_join_add_str(SadWidget w, const char* value) {
    auto* impl = toWidget(w);
    if (impl) impl->joinAccum.push_back(value ? value : "");
}
void sad_prop_join_add_int(SadWidget w, int64_t value) {
    auto* impl = toWidget(w);
    if (impl) impl->joinAccum.push_back(std::to_string(value));
}
void sad_prop_join_add_num(SadWidget w, double value) {
    auto* impl = toWidget(w);
    if (impl) impl->joinAccum.push_back(formatNumLikeValueToString(value));
}
void sad_prop_join_add_bool(SadWidget w, int32_t value) {
    auto* impl = toWidget(w);
    // (AR) نظير Value::toString: منطقيّ ⇒ «صحيح»/«خطأ» (لا 1/0).
    if (impl) impl->joinAccum.push_back(value != 0 ? "\xd8\xb5\xd8\xad\xd9\x8a\xd8\xad"
                                                   : "\xd8\xae\xd8\xb7\xd8\xa3");
}
// (AR) يدمج المُجمِّع بفواصل ⇒ خاصّيّة نصّيّة، ثمّ يمسحه. افتراضٌ يحفظه الخافض:
//      كلّ سلسلة PROP_JOIN_ADD يتبعها COMMIT حتمًا (call_method_dispatch.cpp) فلا
//      يتسرّب المُجمِّع للخاصّيّة التالية؛ والمسح هنا يضمن نظافةَ أيّ استعمالٍ لاحق.
void sad_prop_join_commit(SadWidget w, const char* name) {
    auto* impl = toWidget(w);
    if (!impl) return;
    if (impl->animActive) animCommit(impl); // معدّل غير تحريكيّ يُنهي السلسلة (كالمفسّر)
    if (impl->irNode && name) {
        std::string combined;
        for (size_t i = 0; i < impl->joinAccum.size(); ++i) {
            if (i > 0) combined += ",";
            combined += impl->joinAccum[i];
        }
        impl->irNode->setProperty(name, combined);
    }
    impl->joinAccum.clear();
}

/* ═══════════════════════════════════════════════════════════════════════════
 * 2. إدارة الشجرة / Tree Management
 * ═══════════════════════════════════════════════════════════════════════════ */

void sad_add_child(SadWidget parent, SadWidget child) {
    // (AR) تحصين فشل-مُغلق: تجاهل آمن إن لم يكن الأب أو الابن عنصرًا مُسجَّلًا
    //      (مؤشّر غير-عنصر من باني الحاويات) — قبل static_cast الأعمى.
    if (!isRegisteredWidget(parent) || !isRegisteredWidget(child)) return;
    auto* p = toWidget(parent);
    auto* c = toWidget(child);
    if (!p || !c) return;

    c->parent = p;
    p->children.push_back(c);
    
    if (p->irNode && c->irNode) {
        p->irNode->addChild(c->irNode);
    }
}

void sad_remove_child(SadWidget parent, SadWidget child) {
    // (AR) تحصين فشل-مُغلق (نظير sad_add_child): تجاهل آمن لغير-العنصر المُسجَّل.
    if (!isRegisteredWidget(parent) || !isRegisteredWidget(child)) return;
    auto* p = toWidget(parent);
    auto* c = toWidget(child);
    if (!p || !c) return;
    
    c->parent = nullptr;
    auto& kids = p->children;
    kids.erase(std::remove(kids.begin(), kids.end(), c), kids.end());
    
    // IRNode لا تدعم removeChild مباشرة — يتم التعامل على مستوى SadWidgetImpl فقط
}

void sad_clear_children(SadWidget widget) {
    // (AR) تحصين فشل-مُغلق: تجاهل آمن إن لم يكن مؤشّرًا لعنصرٍ مُسجَّل.
    if (!isRegisteredWidget(widget)) return;
    auto* w = toWidget(widget);
    if (!w) return;
    
    for (auto* child : w->children) {
        child->parent = nullptr;
    }
    w->children.clear();
    
    // IRNode لا تدعم clearChildren — يتم إعادة إنشاء العقدة عند الحاجة
}

/* ═══════════════════════════════════════════════════════════════════════════
 * 3. ضبط الخصائص / Property Setters
 * ═══════════════════════════════════════════════════════════════════════════ */

void sad_set_text(SadWidget widget, const char* text) {
    auto* w = toWidget(widget);
    if (!w || !text) return;
    // (AR) مُدمَجة عامّة (عيّن_النص) قد تُطبَّق على أيّ عنصر ذي نصّ. نكتب مفتاحًا
    //      واحدًا «text» لأنّه المفتاح الذي يقرؤه المُرسِّم أوّلًا لكلّ الأنواع
    //      (Text/Button/AppBar) فيُرسَم بلا استثناء؛ وتجنّبًا لتلويث الـIR بمفاتيح
    //      متعدّدة (نص/محتوى) لا يضعها أيّ مصنع — يحفظ اتّساق «طباعة_شجرة».
    setStringProperty(w, "text", text);
}

void sad_set_size(SadWidget widget, float width, float height) {
    auto* w = toWidget(widget);
    if (!w) return;
    setFloatProperty(w, "\xd8\xb9\xd8\xb1\xd8\xb6", width);    // عرض
    setFloatProperty(w, "\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9", height);  // ارتفاع
}

void sad_set_flex(SadWidget widget, float flex) {
    auto* w = toWidget(widget);
    if (!w) return;
    setFloatProperty(w, "\xd9\x85\xd8\xb1\xd9\x88\xd9\x86\xd8\xa9", flex);  // مرونة
}

void sad_set_background(SadWidget widget, float r, float g, float b, float a) {
    auto* w = toWidget(widget);
    if (!w) return;
    char colorHex[10];
    snprintf(colorHex, sizeof(colorHex), "#%02X%02X%02X%02X",
             (int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
    setStringProperty(w, "\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9", colorHex);  // لون_خلفية
}

void sad_set_foreground(SadWidget widget, float r, float g, float b, float a) {
    auto* w = toWidget(widget);
    if (!w) return;
    char colorHex[10];
    snprintf(colorHex, sizeof(colorHex), "#%02X%02X%02X%02X",
             (int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
    setStringProperty(w, "\xd9\x84\xd9\x88\xd9\x86_\xd8\xa7\xd9\x84\xd9\x86\xd8\xb5", colorHex);  // لون_النص
}

void sad_set_spacing(SadWidget widget, float spacing) {
    auto* w = toWidget(widget);
    if (!w) return;
    setFloatProperty(w, "\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb9\xd8\xaf", spacing);  // تباعد
}

// (AR) عين_الحشوة(عنصر, فوق, يمين, تحت, يسار) — الوسائط فيزيائيّة (top/right/bottom/left).
//   يُكتب المفتاح القانونيّ من SoT (props::PADDING_*) لا سلاسل خام — إغلاقًا لتباعُد
//   قديم: كان يكتب «حشو_يمين/يسار» (بلا تاء، فيزيائيّ) بينما منتِج DSL يكتب
//   «حشوة_بداية/نهاية» (بالتاء، منطقيّ). التحويل فيزيائيّ⇒منطقيّ يتبع اتّفاقيّة
//   layout.h الموثَّقة (start = يمين، end = يسار): right⇒START، left⇒END. عموديًّا
//   top/bottom منطقيّ ثابت. تنبيه دَين: هذا المسار ميّت حاليًّا — layout.cpp يقرأ
//   props::PADDING الأساس فقط ولا يقرأ أيّ مفتاح اتّجاهيّ؛ التوحيد يزيل التباعُد
//   النصّيّ لا يفعّل سلوكًا.
// (EN) sad_set_padding physical params (top/right/bottom/left). Writes SoT canonical
//   keys (props::PADDING_*) — closes an old drift where it wrote «حشو_يمين/يسار»
//   (no taa, physical) while the DSL producer writes «حشوة_بداية/نهاية» (logical).
//   Physical⇒logical per layout.h (start=right, end=left): right⇒START, left⇒END.
//   Debt: this path is dead — layout.cpp reads only base props::PADDING, never a
//   directional key; unifying removes textual drift, it does not activate behaviour.
void sad_set_padding(SadWidget widget, float top, float right,
                     float bottom, float left) {
    auto* w = toWidget(widget);
    if (!w) return;
    setFloatProperty(w, sad::ui::props::PADDING_TOP, top);       // حشوة_أعلى
    setFloatProperty(w, sad::ui::props::PADDING_START, right);   // حشوة_بداية (start=يمين، layout.h)
    setFloatProperty(w, sad::ui::props::PADDING_BOTTOM, bottom); // حشوة_أسفل
    setFloatProperty(w, sad::ui::props::PADDING_END, left);      // حشوة_نهاية (end=يسار، layout.h)
}

void sad_set_alignment(SadWidget widget, int32_t mainAxis, int32_t crossAxis) {
    auto* w = toWidget(widget);
    if (!w) return;
    setIntProperty(w, "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9_\xd8\xb1\xd8\xa6\xd9\x8a\xd8\xb3\xd9\x8a\xd8\xa9", mainAxis);   // محاذاة_رئيسية
    setIntProperty(w, "\xd9\x85\xd8\xad\xd8\xa7\xd8\xb0\xd8\xa7\xd8\xa9_\xd8\xab\xd8\xa7\xd9\x86\xd9\x88\xd9\x8a\xd8\xa9", crossAxis);  // محاذاة_ثانوية
}

void sad_set_border(SadWidget widget, float width) {
    auto* w = toWidget(widget);
    if (!w) return;
    setFloatProperty(w, "\xd8\xad\xd8\xaf\xd9\x88\xd8\xaf", width);  // حدود
}

void sad_set_elevation(SadWidget widget, float elevation) {
    auto* w = toWidget(widget);
    if (!w) return;
    setFloatProperty(w, "\xd8\xb8\xd9\x84", elevation);  // ظل
}

void sad_set_opacity(SadWidget widget, float opacity) {
    auto* w = toWidget(widget);
    if (!w) return;
    setFloatProperty(w, "\xd8\xb4\xd9\x81\xd8\xa7\xd9\x81\xd9\x8a\xd8\xa9", opacity);  // شفافية
}

void sad_set_visibility(SadWidget widget, int32_t visible) {
    auto* w = toWidget(widget);
    if (!w) return;
    setIntProperty(w, "\xd9\x85\xd8\xb1\xd8\xa6\xd9\x8a", visible);  // مرئي
}

/* ═══════════════════════════════════════════════════════════════════════════
 * 4. إدارة التطبيق / App Management
 * ═══════════════════════════════════════════════════════════════════════════ */

SadApp sad_app_create(void) {
    auto app = std::make_unique<SadAppImpl>();
    
#ifdef SAD_UI_USE_SDL2
    app->renderer = std::make_unique<desktop::DesktopRenderer>();
    PlatformRenderOptions opts;
    opts.antialiasing = true;
    opts.vsync = true;
    opts.scaleFactor = 1.0f;
    if (app->renderer->initialize(nullptr, opts)) {
        app->initialized = true;
    }
#endif
    
    SadAppImpl* ptr = app.get();
    g_apps.push_back(std::move(app));
    return ptr;
}

void sad_app_set_root(SadApp app, SadWidget root) {
    auto* a = toApp(app);
    auto* w = toWidget(root);
    if (!a || !w) return;
    a->root = w;
}

void sad_app_layout(SadApp app, float width, float height) {
    auto* a = toApp(app);
    if (!a || !a->root || !a->root->irNode) return;
    
    a->width = width;
    a->height = height;
    
    // تنفيذ التخطيط على شجرة IR
    LayoutConstraints constraints;
    constraints.maxWidth = width;
    constraints.maxHeight = height;
    constraints.minWidth = 0;
    constraints.minHeight = 0;
    
    LayoutEngine engine;
    engine.setDirection(LayoutDirection::RTL);  // العربية — من اليمين لليسار
    engine.setViewportSize(width, height);
    a->layoutResult = engine.layout(a->root->irNode, constraints);
}

void sad_app_render(SadApp app) {
    auto* a = toApp(app);
    if (!a || !a->initialized || !a->renderer || !a->root) return;

    a->renderer->beginFrame();
    
    // رسم شجرة العناصر عبر render() العامة
    if (a->root->irNode && a->layoutResult) {
        a->renderer->render(a->root->irNode, a->layoutResult);
    }
    
    a->renderer->endFrame();
}

#if defined(SAD_UI_USE_SDL2) || (defined(SAD_UI_FREESTANDING) && defined(__linux__))
/* ─── إرسال حدثٍ من النافذة إلى ردود النداء المُترجَمة (م-أ3ر/الإرسال) ───
 * (AR) النافذة (المكتبة) تكتشف العنصر المُصاب (hit-test داخليّ) وتستدعي هذا الجسر
 *      بنوع الحدث والعقدة؛ نبحث عن SadWidgetImpl صاحب العقدة ونستدعي ربط عند_*
 *      المطابق للنوع (stringToIREventType من القلب فالمطابقة دقيقة)، وإلّا onTap
 *      المصنعيّ لحدث النقر. (لا نُكرّر الإرسال: الربط له الأولويّة على onTap.)
 *      مشترك بين الجسر المستضاف (SDL2) والجسر الحرّ (fb0/evdev). */
// (② rfcs#46) POD تخطيط الحدث + sadFillEventPod مُضمَّنان أعلى الملفّ (قبل extern "C")
//   لأنّ الرأس المولَّد يضمّ <array> وقوالبَ بلغة C++؛ تضمينُه داخل كتلة extern "C"
//   يُعطيها ربطًا C فيفشل (C2894). هنا نستعمل sadFillEventPod بلا إعادة تضمين.
static void dispatchCompiledEvent(const IRNode* node, IREventType type,
                                  const std::string& expression,
                                  const sad::ui::EventData& evData) {
    if (!node) return;
    // (② rfcs#46) نبني POD الحدث مرّةً لكلّ إرسال ونمرّر مؤشّره لردّ النداء المترجَم.
    //   thunk المترجم يقرأ الخانات (أو يتجاهلها إن كان المعالِج بلا معامل).
    Sad::Types::EventLayout::SadEventPod evPod;
    Sad::Types::EventLayout::sadFillEventPod(evData, static_cast<int64_t>(type), evPod);
    // (AR) قيد أداء: مسحٌ خطّيّ O(n) على g_widgets لكلّ حدث؛ مقبولٌ لأحجام UI
    //      الصغيرة. عند الحاجة للأحجام الكبيرة استبدله بخريطة IRNode*→impl.
    for (auto& up : g_widgets) {
        SadWidgetImpl* impl = up.get();
        if (!impl || impl->irNode.get() != node) continue;
        // (AR) الاختيار صار للمُرسِل المشترك dispatchEvent: هو من يمرّ بالأطوار
        //      (التقاط/هدف/فقاعات) ويختار المعالِج الواحد ويمرّر تعبيره الفريد
        //      هنا. فنُطلق الرابطَ المطابق للتعبير **وحده** — لا كلّ روابط النوع.
        //      هكذا تُحترم تصفية الطور، ولا تختلط بيانات المعالِجات، ولا يتضاعف
        //      الإطلاق. (كان المُصرِف يُعيد الاختيار بالنوع فيكسر الثلاثة.)
        bool fired = false;
        if (!expression.empty()) {
            for (auto& b : impl->eventBindings) {
                if (b.cb && b.expression == expression) {
                    // (rfcs#51) نربط بيانات الحدث بالخيط طوال تنفيذ المعالِج،
                    //   فيصل إليها مدمج `أوقف_الانتشار` (sad_stop_propagation)؛
                    //   evData هنا هو مرجعُ dispatchEvent نفسه الذي يفحص
                    //   propagationStopped بين المعالِجات، فيسري الإيقاف.
                    sad::ui::ActiveEventScope eventScope(evData);
                    b.cb(b.data, &evPod); // (② rfcs#46) نمرّر POD الحدث
                    fired = true;
                    break; // مُعرِّفٌ فريد ⇒ رابطٌ واحد
                }
            }
        }
        if (!fired && type == IREventType::OnTap && impl->onTap)
            impl->onTap(impl->userData); // (AR) احتياطٌ مصنعيّ (SadCallback بلا حدث)
        return;
    }
}
#endif

/* ─── طباعة_شجرة(عنصر) — طباعة شجرة العناصر للتصحيح (م-تحكّم) ───
 * (AR) جسرٌ رفيع فوق منطق المكتبة المشترك printIRNodeTree ⇒ تطابق مخرَجٍ بالبناء
 *      مع مسار المفسّر (طباعة_شجرة). */
void sad_print_tree(SadWidget w) {
    auto* impl = toWidget(w);
    if (impl && impl->irNode)
        sad::ui::printIRNodeTree(impl->irNode, 0);
}

/* ─── دوال الثيم (م-تحكّم) — جسرٌ رفيع فوق حالة الثيم المكتبيّة المشتركة ───
 * (AR) المنطق والحالة يعيشان في المكتبة (sad::ui::* — features/graphics/core)،
 *      والمفسّر يستدعي الدوال نفسها (ui_core_builtins.cpp: toggleTheme/setTheme/
 *      isDarkMode) ⇒ مصدرُ حقيقةٍ واحد لا تكرار. المترجم يجسر إلى الدوال ذاتها.
 * (EN) Theme state/logic live in the library; both engines bridge to the same
 *      sad::ui:: functions ⇒ single source of truth, no divergence.
 *
 * (AR) إعادة الرسم: لا نستدعي rebuildUI هنا (بخلاف المفسّر) عمدًا — الثيم يغيّر
 *      الألوانَ لا بنيةَ الشجرة، وحلقة النافذة (sad_app_run ⇒ DesktopWindow::run)
 *      تقرأ ألوان الثيم وقتَ الرسم فتلتقط التبديلَ في الإطار التالي تلقائيًّا.
 *      التكافؤ المُختبَر (هل_داكن، headless) يؤكّد تطابق قلب الحالة بين المحرّكين.
 * (EN) We intentionally do NOT rebuild here: theme flips colors (read at paint
 *      time by the window loop), not tree structure — so the next repaint picks
 *      it up. Headless parity (isDark) verifies the state flip matches. */
void sad_toggle_theme(void) { sad::ui::toggleTheme(); }
void sad_set_dark(void)     { sad::ui::setTheme(sad::ui::ThemeMode::Dark); }
void sad_set_light(void)    { sad::ui::setTheme(sad::ui::ThemeMode::Light); }
bool sad_is_dark(void)      { return sad::ui::isDarkMode(); }

/* ─── دوال التنقّل (م-تحكّم) — جسرٌ رفيع فوق مكدّس التنقّل المكتبيّ sad::ui::nav ───
 * (AR) المنطق والحالة في المكتبة (nav.h/nav.cpp، مصدر الحقيقة المشترك)؛ المفسّر
 *      يجسر إلى المكدّس نفسه ⇒ تكافؤ عمق (عدد_الصفحات) بين المحرّكين حتى headless.
 *      رمز الصفحة معتم (SadWidget هنا). ونموذج البانِي (م1-ج) مُفعَّل: sad_navigate_builder
 *      وأخواته يخزّنون بانيًا يُستدعى عند كلّ رسم (تفاعليّة كاملة داخل الصفحة، لا لقطة). */
void sad_navigate(SadWidget page) {
    sad::ui::NavEntry e; e.page = page;
    sad::ui::nav().navigate(e);
}
bool sad_navigate_back(void) { return sad::ui::nav().back(); }
void sad_navigate_root(void) { sad::ui::nav().root(); }
/* (م2) انتقل_بتحريك: تنقّل + انتقال بصريّ مُعلَّق (تستهلكه حلقة النافذة). */
void sad_navigate_transition(SadWidget page, const char* transType, float durationSec) {
    /* (AR) مرجعٌ واحد للمُلاح بدل استدعاء nav() مرّتين (المفرد نفسه في الحالتين). */
    auto& navStack = sad::ui::nav();
    sad::ui::NavEntry e; e.page = page;
    navStack.navigate(e);
    navStack.setPendingTransition(transType ? transType : "", durationSec);
}
/* (م1-ج + م2) انتقل_بتحريك(دالّة): بانٍ (تفاعليّة) + انتقال بصريّ مُعلَّق. */
void sad_navigate_transition_builder(SadPageBuilder build, void* data, SadReleaseFn release,
                                     const char* transType, float durationSec) {
    auto& navStack = sad::ui::nav();
    sad::ui::NavEntry e; e.build = reinterpret_cast<sad::ui::PageBuilder>(build);
    e.data = data; e.release = reinterpret_cast<sad::ui::NavRelease>(release);
    navStack.navigate(e);
    navStack.setPendingTransition(transType ? transType : "", durationSec);
}
/* (م2) عودة_بتحريك: عودة + انتقال بصريّ مُعلَّق. يُرجع 1 إن نجحت العودة، 0 إن لا صفحة. */
int sad_navigate_back_transition(const char* transType, float durationSec) {
    auto& navStack = sad::ui::nav();
    const bool ok = navStack.back();
    if (ok) navStack.setPendingTransition(transType ? transType : "", durationSec);
    return ok ? 1 : 0;
}
void sad_replace_page(SadWidget page) {
    sad::ui::NavEntry e; e.page = page;
    sad::ui::nav().replace(e);
}
/* (م1-ج، توقيع البانِي) انتقل(دالّة): يخزّن بانيًا (build) لا لقطةً؛ حلقة النافذة
 * تستدعيه عند كلّ إعادة رسم (buildCurrent) ⇒ صفحةٌ طازجةٌ تفاعليّة. الملكيّة مُدارة:
 * `data` (بيئة الإغلاق) يملكها المكدّس ويحرّرها بـ`release` عند الإسقاط (Q5). */
void sad_navigate_builder(SadPageBuilder build, void* data, SadReleaseFn release) {
    sad::ui::NavEntry e; e.build = reinterpret_cast<sad::ui::PageBuilder>(build);
    e.data = data; e.release = reinterpret_cast<sad::ui::NavRelease>(release);
    sad::ui::nav().navigate(e);
}
void sad_replace_page_builder(SadPageBuilder build, void* data, SadReleaseFn release) {
    sad::ui::NavEntry e; e.build = reinterpret_cast<sad::ui::PageBuilder>(build);
    e.data = data; e.release = reinterpret_cast<sad::ui::NavRelease>(release);
    sad::ui::nav().replace(e);
}
long long sad_page_count(void) {
    return static_cast<long long>(sad::ui::nav().depth());
}
SadWidget sad_current_page(void) {
    // (AR) الصفحة الحالية (رمزها المعتم = SadWidget). حارسٌ بنيويّ headless (§Q3).
    //      (م1-ج) عبر buildCurrent: إن كان الإدخال بانيًا استدعاه ⇒ يكشف الشجرة
    //      المبنيّة فعلًا (لا مؤشّر الإغلاق)، فيتطابق طباعة_شجرة headless مع الرسم الحيّ.
    return sad::ui::nav().buildCurrent();
}
/* (انتقل_بتحريك_كامل) تنقّل (لقطة) + انتقال دخول مُعلَّق. انتقال الخروج على الصفحة القديمة
 * تحسينٌ حيّ لا تستهلكه حلقة النافذة بعدُ (نظير sad_navigate_transition، انتقال الدخول وحده).
 * أثر المكدّس (العمق/الصفحة الحاليّة) مطابقٌ للمفسّر بالبناء. */
void sad_navigate_exit_transition(SadWidget page, const char* entryType,
                                  const char* exitType, float durationSec) {
    (void)exitType; // (AR) انتقال الخروج على الصفحة القديمة — تحسينٌ حيّ لاحق (غير مُستهلَك)
    auto& navStack = sad::ui::nav();
    sad::ui::NavEntry e; e.page = page;
    navStack.navigate(e);
    navStack.setPendingTransition(entryType ? entryType : "", durationSec);
}
/* (انتقل_بتحريك_كامل، نموذج البانِي) بانٍ (تفاعليّة) + انتقال دخول مُعلَّق. */
void sad_navigate_exit_transition_builder(SadPageBuilder build, void* data, SadReleaseFn release,
                                          const char* entryType, const char* exitType,
                                          float durationSec) {
    (void)exitType;
    auto& navStack = sad::ui::nav();
    sad::ui::NavEntry e; e.build = reinterpret_cast<sad::ui::PageBuilder>(build);
    e.data = data; e.release = reinterpret_cast<sad::ui::NavRelease>(release);
    navStack.navigate(e);
    navStack.setPendingTransition(entryType ? entryType : "", durationSec);
}

/* ─── م-تحكّم UICore: الحالة والنافذة — جسرٌ رفيع فوق حالة المكتبة المشتركة ───
 * (AR) المنطق والحالة في المكتبة (nav.markDirty + windowController)، والمحرّكان
 *      يجسران إلى الدوال/المتحكّم نفسها ⇒ مصدرُ حقيقةٍ واحد لا تكرار. headless:
 *      تُكتب العمليّات ولا تُستهلَك (لا حلقة نافذة) ⇒ لا أثر على التكافؤ. */
void sad_update_state(void) { sad::ui::nav().markDirty(); }
void sad_stop_propagation(void) { sad::ui::stopActiveEventPropagation(); }
void sad_set_window_title(const char* title) {
    sad::ui::windowController().setTitle(title ? title : "");
}
void sad_close_window(void) { sad::ui::windowController().requestClose(); }

/* (توليد_ويب) يولّد HTML من شجرة العنصر عبر HtmlCodegen المكتبيّ (المنطق كلّه في المكتبة).
 * يعتمد المخرَج على شجرة IR + خيارات (اتّجاه/لغة/عنوان)؛ الخيارات الافتراضيّة في
 * HtmlCodegenOptions (rtl/ar/عنوان افتراضيّ) تطابق ما يضبطه المفسّر ⇒ مخرَجٌ بايتيٌّ مطابق.
 * (module.name لا يظهر في المخرَج فلا يؤثّر على التكافؤ.) النصّ المُعاد مخصّصٌ في الكومة
 * يملكه المستدعي (نظير sad_readline؛ لا مُحرِّر بعدُ في وقت تشغيل المترجم). */
char* sad_generate_web(SadWidget root, const char* title) {
    auto* impl = toWidget(root);
    std::string html;
    // ⚠ حدٌّ موثَّق (Amelia M1، تباعد ضيّق مقبول = سوء استخدام): على جذرٍ فارغ
    //   (root=null، أو بانٍ يُرجع لاشيء) يُرجع المترجم نصًّا فارغًا بصمت، بينما المفسّر
    //   يُطلق خطأ الكتالوج (RUN_BUILTIN_REQUIRES_ARG). الاستعمال الصحيح (عنصر أو بانٍ
    //   يُرجع عنصرًا) مطابقٌ بايتيًّا. لا نُدخِل بنية أخطاء في runtime المترجم لحالة
    //   المدخل الباطل هذه (نظير حدّ بانِي التنقّل الذي يُرجع لاشيء).
    if (impl && impl->irNode) {
        sad::ui::IRModule module;
        module.root = impl->irNode;
        sad::ui::web::HtmlCodegenOptions opts; // افتراضات المكتبة: rtl/ar/عنوان افتراضيّ (كالمفسّر)
        if (title && *title)
            opts.title = title; // تجاوز العنوان إن مُرِّر نصٌّ غير فارغ (نظير المفسّر: args[1])
        sad::ui::web::HtmlCodegen codegen(opts);
        html = codegen.generate(module);
    }
    // (AR) نسخةٌ في الكومة مملوكة للمستدعي (نظير strdup في sad_readline).
    char* out = static_cast<char*>(std::malloc(html.size() + 1));
    if (out)
        std::memcpy(out, html.c_str(), html.size() + 1);
    return out;
}

/* ─── تشغيل_تطبيق(عنصر|دالّة) — حلقة سطح المكتب (م-أ3ر/الإرسال) ───
 * (AR) جسرٌ رفيع فوق DesktopWindow في المكتبة: النافذة تملك الحلقة والتخطيط والرسم
 *      وhit-test وإرسال الأحداث؛ نوصّل callback يُرسِل إلى ردود النداء المُترجَمة.
 *      كلّ المنطق الرسوميّ في المكتبة؛ هذا الجسر يربط فقط.
 *
 * (AR) نموذجان لمصدر الجذر يتشاركان آليّة الحلقة نفسها عبر runNavRegisteredApp:
 *      • لقطة (sad_app_run): جذرٌ ثابتٌ يُعرَض أبدًا (توافق خلفيّ).
 *      • بانٍ (sad_app_run_builder، م1-ب): دالّة ص تُستدعى كلّ رسم ⇒ شجرةٌ طازجة،
 *        + نبضةٌ زمنيّة كلّ ثانية توسّخ المكدّس فتُحدَّث المشاهد الزمنيّة (الساعة).
 *      كلاهما يسجّل إدخال nav ثمّ يُشغّل الحلقة؛ الرسم دائمًا من nav().buildCurrent()
 *      ⇒ مصدر رسمٍ واحد لا قناتين. */
namespace {

// (م1-ب) عبّئ عرض/ارتفاع الجذر بأبعاد الشاشة إن غابا (كي يمتدّ التخطيط ليملأ الشاشة،
//        نظير جذر سطح المكتب). يُطبَّق على كلّ شجرةٍ من buildCurrent: اللقطة تُعبَّأ
//        مرّةً (الخصائص تبقى)، والبانِي يُنتج شجرةً طازجةً كلّ رسم فتلزمه كلّ مرّة.
inline void fillRootDims(const std::shared_ptr<IRNode>& node,
                         std::uint32_t w, std::uint32_t h) {
    if (!node) return;
    if (!node->findProperty(sad::ui::props::WIDTH))
        node->setProperty(sad::ui::props::WIDTH, static_cast<double>(w));
    if (!node->findProperty(sad::ui::props::HEIGHT))
        node->setProperty(sad::ui::props::HEIGHT, static_cast<double>(h));
}

// (م1-ب) حالة أجيال البانِي المشتركة بين buildRoot وحلقة الرسم: prevGen جيلُ الشجرة
//        المعروضة حاليًّا (تُحصَد عند استبدالها)، وbaseDepth عمقُ المكدّس عند البدء
//        (حارسٌ ضدّ حصاد صفحةٍ دُفعت بتنقّل)، وlastMinute آخر دقيقةٍ جداريّة أُعيد عندها
//        البناء (بوّابة النبضة: انظر أدناه).
struct BuilderGenState { uint64_t prevGen = 0; std::size_t baseDepth = 0; long long lastMinute = -1; };

// (م1-ب) الدقيقة الجداريّة الحاليّة (دقائق منذ حقبة يونكس، UTC). بوّابةُ النبضة:
//        عرضُ HH:MM لا يتغيّر إلّا عند تبدّل الدقيقة، فنُعيد بناء البانِي **مرّةً كلّ
//        دقيقة** لا كلّ ثانية. هذا (أ) الإيقاع الصحيح لساعةٍ بدقّة الدقيقة، و(ب)
//        يقلّص إعادةَ البناء ٦٠ ضعفًا — وإعادةُ البناء في الوضع الحرّ (بلا كانِس
//        مهملات) تسرّب كومةَ البرنامج المُترجَم؛ فالبوّابة تحدّ التسريب سِتّين ضعفًا.
//        (دَينٌ قائم: الإصلاح الجذريّ حوضُ ذاكرةٍ حرٌّ يُعاد ضبطه حول كلّ إعادة بناء.)
static long long currentWallMinute() {
    return std::chrono::duration_cast<std::chrono::minutes>(
        std::chrono::system_clock::now().time_since_epoch()).count();
}

// (م1-ب) آليّة تشغيل التطبيق بعد تسجيل إدخال nav الحاليّ (لقطةً أو بانيًا). الجذر
//        يُبنى دائمًا عبر nav().buildCurrent()؛ withPeriodicTick يحقن توسيخًا كلّ
//        ثانية (steady_clock) فيُعاد استدعاء البانِي دوريًّا (يعيد حساب الآن).
void runNavRegisteredApp(bool withPeriodicTick) {
#if defined(SAD_UI_FREESTANDING) && defined(__linux__)
    // ─── الفرع الحرّ (fb0/evdev): بديل SDL2 على لينكس بلا X11 ───
    // (AR) نفس ABI جسر SDL2: نمرّر شجرة IR نفسها إلى آليّة الحلقة الحرّة المشتركة
    //      (fs::runFreestandingApp)، ونوصّل إرسال الأحداث لردود النداء المُترجَمة
    //      (dispatchCompiledEvent) — فلا SDL2 في الثنائيّ إطلاقًا.
    sad::ui::freestanding::FreestandingAppConfig cfg;
    // (AR) خطّ العرض وجهاز الإطار عبر البيئة (يمرّرهما /init في sad-os): غيابهما
    //      ⇒ الخطّ المدمج و/dev/fb0. أسماء المتغيّرات عقدٌ مع مُشغِّل sad-os.
    if (const char* f = std::getenv("SAD_UI_FB_FONT")) cfg.fontPath = f;
    if (const char* d = std::getenv("SAD_UI_FB_DEVICE")) cfg.devicePath = d;

    // (م1-ب) حالة أجيال البانِي: كلّ إعادة بناءٍ (نبضة) تختم شجرةً بجيلٍ جديد، ثمّ
    //        تُحصَد الشجرة المُستبدَلة بعد عرض الجديدة ⇒ g_widgets محدود. حارس العمق
    //        (baseDepth) يمنع حصادَ صفحةٍ دُفعت للمكدّس بتنقّلٍ من ردّ نداء. مشتركةٌ
    //        بين buildRoot وonIterate عبر shared_ptr (تعيش مع الحلقة).
    auto genState = std::make_shared<BuilderGenState>();
    genState->baseDepth = sad::ui::nav().depth();
    if (withPeriodicTick) genState->lastMinute = currentWallMinute(); // الشجرة الأولى تعرض الدقيقة الحاليّة

    // بناء الجذر: من الصفحة الحاليّة (buildCurrent) — اللقطة تعيد الشجرة المخزّنة،
    // والبانِي يُنتجها طازجةً؛ نملأ أبعاد الجذر إن غابت كي يمتدّ التخطيط.
    cfg.buildRoot = [withPeriodicTick, genState](std::uint32_t w, std::uint32_t h)
        -> std::shared_ptr<IRNode>
    {
        if (withPeriodicTick) genState->prevGen = ++g_widgetGeneration; // ختمُ جيل الشجرة الأولى
        auto* pimpl = toWidget(sad::ui::nav().buildCurrent());
        if (!pimpl || !pimpl->irNode) return nullptr;
        fillRootDims(pimpl->irNode, w, h);
        return pimpl->irNode;
    };
    // إرسال النقر إلى ردود النداء المُترجَمة (بالعقدة لا بالتعبير النصّيّ):
    cfg.onEvent = [](IREventType type, const std::string& expr,
                     const IRNode* node, const sad::ui::EventData& evData)
    { dispatchCompiledEvent(node, type, expr, evData); }; // (② rfcs#46) نمرّر بيانات الحدث + تعبير المعالِج المُختار
    // مفتاح الخروج: F2/Escape (اتّفاقيّة الخروج من التطبيقات الحرّة):
    cfg.onKey = [](sad::ui::UnifiedKeyCode code) -> bool
    {
        return code == sad::ui::UnifiedKeyCode::F2 ||
               code == sad::ui::UnifiedKeyCode::Escape;
    };
    // كلّ دورة: (م1-ب) نبضةُ الدقيقة — نوسّخ المكدّس **مرّةً كلّ دقيقة جداريّة** (لا
    //   كلّ ثانية): عرضُ HH:MM لا يتبدّل أسرع، وإعادةُ البناء الحرّة تسرّب الكومة بلا
    //   كانِس فنحدّها بالإيقاع الصحيح. ثمّ نبدّل المحتوى إن اتّسخ (نبضةٌ أو تنقّل).
    cfg.onIterate = [withPeriodicTick, genState](
        sad::ui::freestanding::AppLoopContext& ctx) -> bool
    {
        auto& navStack = sad::ui::nav();
        if (withPeriodicTick)
        {
            const long long nowMin = currentWallMinute();
            if (nowMin != genState->lastMinute)
            {
                genState->lastMinute = nowMin;
                navStack.markDirty();
            }
        }
        if (navStack.takeDirty())
        {
            // (م1-ب) اختِم شجرةَ إعادة البناء بجيلٍ جديد كي نحصد سابقتها لاحقًا.
            const uint64_t builtGen = withPeriodicTick ? ++g_widgetGeneration : 0;
            auto* pimpl = toWidget(navStack.buildCurrent());
            if (pimpl && pimpl->irNode)
            {
                fillRootDims(pimpl->irNode, ctx.width, ctx.height);
                ctx.window.setContent(pimpl->irNode);
                ctx.window.invalidate();
                // (م1-ب) حصاد الشجرة المُستبدَلة بعد عرض الجديدة — إلّا إن نمَا عمقُ
                //        المكدّس (تنقّلٌ دفع الشجرةَ القديمة للعودة إليها): لا نحرّرها.
                //        (دَينٌ مقبول: التنقّل بعيدًا يُيَتِّم شجرةَ الأساس فلا تُحصَد —
                //         محدودٌ بعدد التنقّلات لا بالزمن، بخلاف تسريب النبضة المحسوم.)
                if (withPeriodicTick)
                {
                    const uint64_t replaced = genState->prevGen;
                    genState->prevGen = builtGen;
                    if (replaced != 0 && replaced != builtGen &&
                        navStack.depth() == genState->baseDepth)
                        reapWidgetGeneration(replaced);
                }
            }
        }
        return true;
    };

    std::string err;
    // (AR) فشل-معلَن: كان الخطأ يُلتقَط ويُهمَل، فيخرج التطبيق الحرّ صامتًا تمامًا
    //      (شاشة سوداء بلا سبب) عند تعذّر فتح جهاز الإطار/الإدخال. نُبلّغ على
    //      المسار القياسيّ للأخطاء كي يظهر السبب على وحدة التحكّم/التسلسليّ.
    //      (نستعمل fprintf لا std::cerr: الثنائيّ الحرّ يُفضَّل ألّا يجرّ تهيئة
    //       iostream/locale الساكنة، وstderr هو المسار المضمون هناك.)
    if (int rc = sad::ui::freestanding::runFreestandingApp(cfg, &err); rc != 0)
        std::fprintf(stderr, "sad_app_run (الوضع الحرّ): %s (رمز %d)\n",
                     err.empty() ? "فشل غير موصوف" : err.c_str(), rc);
    // بعد إغلاق النافذة: صفّر مكدّس التنقّل والمتحكّم (نظير الفرع المستضاف).
    sad::ui::nav().reset();
    sad::ui::windowController().reset();
#elif defined(SAD_UI_USE_SDL2)
    // (م1-ب) حالة أجيال البانِي (نظير الفرع الحرّ): تُحصَد الشجرة المُستبدَلة كلّ نبضة.
    auto genState = std::make_shared<BuilderGenState>();
    genState->baseDepth = sad::ui::nav().depth();
    if (withPeriodicTick) genState->lastMinute = currentWallMinute(); // الشجرة الأولى تعرض الدقيقة الحاليّة
    // (AR) الجذر الابتدائيّ من الصفحة الحاليّة (لقطةً أو بانيًا) — مصدر رسمٍ واحد.
    if (withPeriodicTick) genState->prevGen = ++g_widgetGeneration; // ختمُ جيل الشجرة الأولى
    auto* impl = toWidget(sad::ui::nav().buildCurrent());
    if (!impl || !impl->irNode) return;
    sad::ui::desktop::DesktopWindow window;
    sad::ui::desktop::WindowOptions options;
    // (AR) قراءة العنوان/الأبعاد من خصائص الجذر إن وُجدت (نظير UIBridge::run).
    //      العنوان نصٌّ فقط عمدًا (نظير UIBridge)؛ قيمةٌ غير نصّيّة تُتجاهَل صمتًا،
    //      بينما الأبعاد تقبل int64/double مرونةً لاستنتاج النوع.
    if (const auto* p = impl->irNode->findProperty(props::TITLE)) // عنوان
        if (auto* s = std::get_if<std::string>(&p->value)) options.title = *s;
    if (const auto* p = impl->irNode->findProperty(props::WIDTH)) { // عرض
        if (auto* n = std::get_if<int64_t>(&p->value)) options.width = static_cast<int>(*n);
        else if (auto* d = std::get_if<double>(&p->value)) options.width = static_cast<int>(*d);
    }
    if (const auto* p = impl->irNode->findProperty(props::HEIGHT)) { // ارتفاع
        if (auto* n = std::get_if<int64_t>(&p->value)) options.height = static_cast<int>(*n);
        else if (auto* d = std::get_if<double>(&p->value)) options.height = static_cast<int>(*d);
    }
    if (!window.create(options)) return;
    window.setContent(impl->irNode);
    window.setOnEventCallback(
        [](IREventType type, const std::string& expr,
           const IRNode* node, const EventData& data) {
            dispatchCompiledEvent(node, type, expr, data); // (② rfcs#46) بيانات الحدث + تعبير المعالِج المُختار
        });
    // (م1-ب) إعادة الرسم عند تبديل الصفحة أو نبضة الدقيقة: نوسّخ المكدّس **مرّةً كلّ
    //        دقيقة جداريّة** (لا كلّ ثانية) — الإيقاع الصحيح لـHH:MM وحدٌّ للتسريب،
    //        ثمّ نعيد ضبط المحتوى إن اتّسخ (نبضة/تنقّل).
    window.setTimerUpdateCallback([&window, withPeriodicTick, genState]() {
        auto& navStack = sad::ui::nav();
        if (withPeriodicTick) {
            const long long nowMin = currentWallMinute();
            if (nowMin != genState->lastMinute) {
                genState->lastMinute = nowMin;
                navStack.markDirty();
            }
        }
        if (navStack.takeDirty()) {
            // (م1-ج) ابنِ الصفحة الحاليّة عبر buildCurrent: إن كان الإدخال بانيًا
            //        (انتقل(دالّة)/تشغيل_تطبيق(دالّة)) استُدعي البانِي ⇒ شجرةٌ طازجةٌ
            //        تفاعليّة كلّ رسم؛ وإن كان لقطةً أُعيدت كما هي. مصدر رسمٍ واحد.
            const uint64_t builtGen = withPeriodicTick ? ++g_widgetGeneration : 0;
            auto* pimpl = toWidget(navStack.buildCurrent());
            if (pimpl && pimpl->irNode) {
                // (م2) إن كان ثمّة انتقال بصريّ مُعلَّق (انتقل_بتحريك) نبدّل بتحريك؛ وإلّا فورًا.
                std::string transType; float dur = sad::ui::kDefaultTransitionSec;
                const bool withTransition = navStack.takePendingTransition(transType, dur);
                if (withTransition)
                    window.setContentWithTransition(pimpl->irNode, transType, dur);
                else
                    window.setContent(pimpl->irNode);
                // (م1-ب) حصاد الشجرة المُستبدَلة — إلّا إن نمَا العمق بتنقّلٍ دفعها للمكدّس،
                //        أو كان ثمّة انتقالٌ بصريّ يُبقي الشجرة القديمة حيّةً أثناء التحريك.
                if (withPeriodicTick && !withTransition) {
                    const uint64_t replaced = genState->prevGen;
                    genState->prevGen = builtGen;
                    if (replaced != 0 && replaced != builtGen &&
                        navStack.depth() == genState->baseDepth)
                        reapWidgetGeneration(replaced);
                } else if (withPeriodicTick) {
                    genState->prevGen = builtGen; // حدّث الجيل دون حصادٍ (الانتقال يحمي القديم)
                }
            }
        }
    });
    window.run();
    window.destroy();
    // (AR) بعد إغلاق النافذة: صفّر مكدّس التنقّل (المقابض تشير لعناصر هذا التطبيق)
    //      والمتحكّم (لئلّا تتسرّب عمليّة عنوان/إغلاق مُعلَّقة إلى تشغيلٍ لاحق في العمليّة نفسها).
    sad::ui::nav().reset();
    sad::ui::windowController().reset();
#else
    (void)withPeriodicTick;
#endif
}

} // namespace

void sad_app_run(SadWidget root) {
    // (AR) نموذج اللقطة (توافق خلفيّ): جذرٌ ثابتٌ يُعرَض أبدًا، بلا نبضة زمنيّة.
    //      نسجّله صفحةً ابتدائيّة في المكدّس ونستهلك dirty الابتدائيّ (المحتوى يُضبَط
    //      في buildRoot)، ثمّ نُشغّل الحلقة المشتركة.
    auto* impl = toWidget(root);
    if (!impl || !impl->irNode) return;
    { auto& navStack = sad::ui::nav(); sad::ui::NavEntry e; e.page = root;
      navStack.replace(e); navStack.takeDirty(); }
    runNavRegisteredApp(/*withPeriodicTick=*/false);
}

/* ─── تشغيل_تطبيق(دالّة_بناء) — نموذج البانِي (م1-ب): دالّة ص تُستدعى كلّ رسم ───
 * (AR) نظير sad_navigate_builder: نسجّل بانيًا في المكدّس يُنتج الشجرة طازجةً، ونفعّل
 *      نبضةً زمنيّة كلّ ثانية توسّخ المكدّس فيُعاد بناء الصفحة دوريًّا (تُحدَّث الساعة).
 *      الملكيّة (Q5): release يحرّر بيئة الإغلاق `data` عند nav().reset() في نهاية الحلقة. */
void sad_app_run_builder(SadPageBuilder build, void* data, SadReleaseFn release) {
    if (!build) return;
    { auto& navStack = sad::ui::nav(); sad::ui::NavEntry e;
      e.build = build; e.data = data; e.release = release;
      navStack.replace(e); navStack.takeDirty(); }
    runNavRegisteredApp(/*withPeriodicTick=*/true);
}

void sad_app_destroy(SadApp app) {
    auto* a = toApp(app);
    if (!a) return;
    
    if (a->renderer) {
        a->renderer->shutdown();
        a->renderer.reset();
    }
    a->root = nullptr;
    a->initialized = false;
}

void sad_widget_destroy(SadWidget widget) {
    auto* w = toWidget(widget);
    if (!w) return;
    
    // فصل عن الأب
    if (w->parent) {
        sad_remove_child(w->parent, widget);
    }
    
    // تنظيف الأبناء
    w->children.clear();
    w->irNode.reset();
}

} /* extern "C" */
