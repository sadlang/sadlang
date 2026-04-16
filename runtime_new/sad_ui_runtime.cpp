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
 * المسار: runtime_new/sad_ui_runtime.cpp
 * يتطابق مع: compiler_new/src/backend/llvm/llvm_codegen_ui.cpp
 *
 * ============================================================================
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ============================================================================
 */

#include "sad_ui_runtime.h"

#include <sad_ui/ir.h>
#include <sad_ui/ir_builder.h>
#include <sad_ui/layout.h>
#include <sad_ui/platform_renderer.h>
#include <sad_ui/node.h>
#include <sad_ui/types.h>

#ifdef SAD_UI_USE_SDL2
#include <sad_ui/desktop/renderer.h>
#endif

#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <iostream>
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
        // خاصية "نص" بالعربي + "text" كاحتياط
        setStringProperty(w, "\xd9\x86\xd8\xb5", text);        // نص
        setStringProperty(w, "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", text); // محتوى
        setStringProperty(w, "text", text);
    }
    return w;
}

SadWidget sad_text_styled(const char* text, float fontSize,
                          float r, float g, float b, float a) {
    auto* w = createWidget(UINodeType::Text);
    if (text) {
        setStringProperty(w, "\xd9\x86\xd8\xb5", text);
        setStringProperty(w, "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", text);
        setStringProperty(w, "text", text);
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
        setStringProperty(w, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", label);    // عنوان
        setStringProperty(w, "\xd9\x86\xd8\xb5", label);    // نص
        setStringProperty(w, "text", label);
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
        setStringProperty(w, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", label);
        setStringProperty(w, "\xd9\x86\xd8\xb5", label);
        setStringProperty(w, "text", label);
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
    auto* w = createWidget(UINodeType::Button);
    if (icon) {
        setStringProperty(w, "\xd8\xa3\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9", icon);
    }
    
    char colorHex[10];
    snprintf(colorHex, sizeof(colorHex), "#%02X%02X%02X%02X",
             (int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
    setStringProperty(w, "\xd9\x84\xd9\x88\xd9\x86_\xd8\xae\xd9\x84\xd9\x81\xd9\x8a\xd8\xa9", colorHex);
    setFloatProperty(w, "\xd8\xb8\xd9\x84", 6.0);  // ظل — FAB has elevation
    setFloatProperty(w, "\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7", 28.0);  // زوايا — round
    
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
    auto* w = createWidget(UINodeType::Slider);
    setFloatProperty(w, "\xd8\xa3\xd8\xaf\xd9\x86\xd9\x89", minVal);  // أدنى
    setFloatProperty(w, "\xd8\xa3\xd9\x82\xd8\xb5\xd9\x89", maxVal);  // أقصى
    w->onTap = onChanged;
    w->userData = userData;
    return w;
}

SadWidget sad_card(void) {
    auto* w = createWidget(UINodeType::Card);
    setFloatProperty(w, "\xd8\xb8\xd9\x84", 2.0);  // ظل
    setFloatProperty(w, "\xd8\xb2\xd9\x88\xd8\xa7\xd9\x8a\xd8\xa7", 8.0);  // زوايا
    return w;
}

SadWidget sad_scaffold(void) {
    return createWidget(UINodeType::Scaffold);
}

SadWidget sad_app_bar(const char* title) {
    auto* w = createWidget(UINodeType::AppBar);
    if (title) {
        setStringProperty(w, "\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86", title);  // عنوان
        setStringProperty(w, "text", title);
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
    if (message) {
        setStringProperty(w, "\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84\xd8\xa9", message);  // رسالة
    }
    return w;
}

SadWidget sad_image(const char* source) {
    auto* w = createWidget(UINodeType::Image);
    if (source) {
        setStringProperty(w, "\xd9\x85\xd8\xb5\xd8\xaf\xd8\xb1", source);  // مصدر
        setStringProperty(w, "src", source);
    }
    return w;
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
    setStringProperty(w, "\xd9\x86\xd8\xb5", text);
    setStringProperty(w, "\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89", text);
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
