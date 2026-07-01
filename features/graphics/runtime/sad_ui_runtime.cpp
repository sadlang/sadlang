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
#include <sad_ui/print_tree.h> // (AR) م-تحكّم: طباعة_شجرة عبر منطق المكتبة المشترك
#include <sad_ui/layout.h>
#include <sad_ui/platform_renderer.h>
#include <sad_ui/node.h>
#include <sad_ui/types.h>

#ifdef SAD_UI_USE_SDL2
#include <sad_ui/desktop/renderer.h>
#include <sad_ui/desktop/window.h>
#endif

#include <string>
#include <vector>
#include <memory>
#include <cstring>
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

    // (AR) ربط الأحداث الانسيابيّة عند_* (م-أ3ر، L2): اسم الحدث ⇒ ردّ نداء + بيانات.
    //      الحدث نفسه يُسجَّل على irNode (تطابق getEvents مع المفسّر)؛ هذا الجدول
    //      يحمل ردّ النداء المُترجَم للإرسال (لا يحمله IREvent الذي يخزّن تعبيرًا نصّيًّا).
    struct EventBinding {
        std::string name;
        IREventType type = IREventType::Custom; // (AR) مُشتقٌّ مرّةً عند الربط (لا يُعاد اشتقاقه لكلّ إرسال)
        SadCallback cb = nullptr;
        void* data = nullptr;
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

/* ── دوال مساعدة / Helper functions ── */

static SadWidgetImpl* createWidget(UINodeType type) {
    auto widget = std::make_unique<SadWidgetImpl>(type);
    SadWidgetImpl* ptr = widget.get();
    g_widgets.push_back(std::move(widget));
    return ptr;
}

static SadWidgetImpl* toWidget(SadWidget w) {
    return static_cast<SadWidgetImpl*>(w);
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

/* ═══════════════════════════════════════════════════════════════════════════
 * 1. مصانع العناصر / Widget Factories
 * ═══════════════════════════════════════════════════════════════════════════ */

extern "C" {

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
void sad_add_event(SadWidget w, const char* name, SadCallback cb, void* data) {
    auto* impl = toWidget(w);
    if (!impl || !impl->irNode || !name) return;
    const std::string evName(name);
    IREvent ev;
    ev.type = stringToIREventType(evName);
    if (ev.type == IREventType::Custom)
        ev.customEventName = evName;
    // (AR) علامة: المعالج ردّ نداء مُترجَم لا تعبير نصّيّ. ملاحظة تكافؤ: المفسّر
    //      يستعمل "__event_"+m (ui_widget_method_call.cpp:92) — علامةٌ مختلفة، لكنّ
    //      `expression` **ليس جزءًا من مخرَج التكافؤ** (طباعة_شجرة تقرأ getEventName
    //      المعتمِد على type/customEventName لا expression) فلا تباعد.
    ev.expression = "__compiled_cb";
    impl->irNode->addEvent(ev);
    impl->eventBindings.push_back({evName, ev.type, cb, data}); // نخزّن النوع المُشتقّ مرّةً
    // (AR) ⚠ TODO(حظر عند تنفيذ المُرسِل الكامل): شيم توافق مع مسار onTap القديم في
    //      المصانع (sad_button…): أوّل معالج نقر يملأ خانة onTap إن كانت فارغة.
    //      الحارس nullptr يمنع تجاوز معالج المصنع. متى نُفِّذ المُرسِل الكامل **يجب
    //      إزالة هذا الشيم** وجعل eventBindings مصدرَ الحقيقة الوحيد لأحداث عند_*
    //      وإلّا فأيّ مُرسِلٍ يقرأ onTap مباشرةً يُطلق النقر مرّتين.
    if (cb && impl->onTap == nullptr && ev.type == IREventType::OnTap) {
        impl->onTap = cb;
        impl->userData = data;
    }
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
    if (types.empty()) types.push_back("fadeIn"); // احتياطيّ كالمفسّر
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
    auto* p = toWidget(parent);
    auto* c = toWidget(child);
    if (!p || !c) return;
    
    c->parent = nullptr;
    auto& kids = p->children;
    kids.erase(std::remove(kids.begin(), kids.end(), c), kids.end());
    
    // IRNode لا تدعم removeChild مباشرة — يتم التعامل على مستوى SadWidgetImpl فقط
}

void sad_clear_children(SadWidget widget) {
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

void sad_set_padding(SadWidget widget, float top, float right,
                     float bottom, float left) {
    auto* w = toWidget(widget);
    if (!w) return;
    setFloatProperty(w, "\xd8\xad\xd8\xb4\xd9\x88_\xd8\xa3\xd8\xb9\xd9\x84\xd9\x89", top);      // حشو_أعلى
    setFloatProperty(w, "\xd8\xad\xd8\xb4\xd9\x88_\xd9\x8a\xd9\x85\xd9\x8a\xd9\x86", right);    // حشو_يمين
    setFloatProperty(w, "\xd8\xad\xd8\xb4\xd9\x88_\xd8\xa3\xd8\xb3\xd9\x81\xd9\x84", bottom);    // حشو_أسفل
    setFloatProperty(w, "\xd8\xad\xd8\xb4\xd9\x88_\xd9\x8a\xd8\xb3\xd8\xa7\xd8\xb1", left);     // حشو_يسار
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

#ifdef SAD_UI_USE_SDL2
/* ─── إرسال حدثٍ من النافذة إلى ردود النداء المُترجَمة (م-أ3ر/الإرسال) ───
 * (AR) النافذة (المكتبة) تكتشف العنصر المُصاب (hit-test داخليّ) وتستدعي هذا الجسر
 *      بنوع الحدث والعقدة؛ نبحث عن SadWidgetImpl صاحب العقدة ونستدعي ربط عند_*
 *      المطابق للنوع (stringToIREventType من القلب فالمطابقة دقيقة)، وإلّا onTap
 *      المصنعيّ لحدث النقر. (لا نُكرّر الإرسال: الربط له الأولويّة على onTap.) */
static void dispatchCompiledEvent(const IRNode* node, IREventType type) {
    if (!node) return;
    // (AR) قيد أداء: مسحٌ خطّيّ O(n) على g_widgets لكلّ حدث؛ مقبولٌ لأحجام UI
    //      الصغيرة. عند الحاجة للأحجام الكبيرة استبدله بخريطة IRNode*→impl.
    for (auto& up : g_widgets) {
        SadWidgetImpl* impl = up.get();
        if (!impl || impl->irNode.get() != node) continue;
        // (AR) نُطلق كلّ رابطٍ يطابق نوعَ الحدث — مطابقةٌ لحلقة handleEvent في المفسّر
        //      (ui_bridge_events.cpp:36-38) التي تمرّ على كلّ node->getEvents() وتُطلق
        //      كلّ حدثٍ نوعه == eventType (لا واحدًا). فعدّة معالجاتٍ بنفس النوع تُطلق كلّها.
        // (AR) قيدٌ موروثٌ من القلب (مطابقٌ للمفسّر لا انحرافَ): كلّ حدثٍ غير قياسيّ
        //      يُصنَّف IREventType::Custom، فحدثان مخصّصان مختلفان (عند_س/عند_ص) يُطلقان
        //      معًا لأيّهما — لأنّ المطابقة بالنوع لا بالاسم المخصّص. سلوكٌ مطابقٌ
        //      لـui_bridge_events.cpp تمامًا (تمييزُ الأحداث المخصّصة تحسينٌ للقلب لاحقًا).
        bool fired = false;
        for (auto& b : impl->eventBindings) {
            if (b.cb && b.type == type) { // النوع مُشتقٌّ مسبقًا (لا stringToIREventType لكلّ إرسال)
                b.cb(b.data);
                fired = true;
            }
        }
        if (!fired && type == IREventType::OnTap && impl->onTap)
            impl->onTap(impl->userData);
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

/* ─── تشغيل_تطبيق(عنصر) — حلقة سطح المكتب (م-أ3ر/الإرسال) ───
 * (AR) جسرٌ رفيع فوق DesktopWindow في المكتبة: النافذة تملك الحلقة والتخطيط والرسم
 *      وhit-test وإرسال الأحداث؛ نوصّل callback يُرسِل إلى ردود النداء المُترجَمة.
 *      كلّ المنطق الرسوميّ في المكتبة؛ هذا الجسر يربط فقط. */
void sad_app_run(SadWidget root) {
#ifdef SAD_UI_USE_SDL2
    auto* impl = toWidget(root);
    if (!impl || !impl->irNode) return;
    sad::ui::desktop::DesktopWindow window;
    sad::ui::desktop::WindowOptions options;
    // (AR) قراءة العنوان/الأبعاد من خصائص الجذر إن وُجدت (نظير UIBridge::run).
    //      العنوان نصٌّ فقط عمدًا (نظير UIBridge)؛ قيمةٌ غير نصّيّة تُتجاهَل صمتًا،
    //      بينما الأبعاد تقبل int64/double مرونةً لاستنتاج النوع.
    if (const auto* p = impl->irNode->findProperty("\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86")) // عنوان
        if (auto* s = std::get_if<std::string>(&p->value)) options.title = *s;
    if (const auto* p = impl->irNode->findProperty("\xd8\xb9\xd8\xb1\xd8\xb6")) { // عرض
        if (auto* n = std::get_if<int64_t>(&p->value)) options.width = static_cast<int>(*n);
        else if (auto* d = std::get_if<double>(&p->value)) options.width = static_cast<int>(*d);
    }
    if (const auto* p = impl->irNode->findProperty("\xd8\xa7\xd8\xb1\xd8\xaa\xd9\x81\xd8\xa7\xd8\xb9")) { // ارتفاع
        if (auto* n = std::get_if<int64_t>(&p->value)) options.height = static_cast<int>(*n);
        else if (auto* d = std::get_if<double>(&p->value)) options.height = static_cast<int>(*d);
    }
    if (!window.create(options)) return;
    window.setContent(impl->irNode);
    window.setOnEventCallback(
        [](IREventType type, const std::string& /*elementId*/,
           const IRNode* node, const EventData& /*data*/) {
            dispatchCompiledEvent(node, type);
        });
    window.run();
    window.destroy();
#else
    (void)root;
#endif
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
