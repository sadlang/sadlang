// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
//  sad_flutter_widgets.h — المكونات الأساسية بأسلوب Flutter
//  Basic Widgets: Text, Button, Container, Icon, Image, etc.
// ═══════════════════════════════════════════════════════════════════════════════

#pragma once

#include "sad_flutter_core.h"
#include <optional>

namespace sad {
namespace flutter {

// ═══════════════════════════════════════════════════════════════════════════════
//  Enums & Structs — أنواع التنسيق
// ═══════════════════════════════════════════════════════════════════════════════

/// محاذاة النص
enum class TextAlign {
    Right,      // يمين (افتراضي للعربية)
    Left,       // يسار
    Center,     // وسط
    Justify     // ملء السطر
};

/// وزن الخط
enum class FontWeight {
    Normal,     // عادي
    Bold,       // عريض
    Light,      // خفيف
    W100, W200, W300, W400, W500, W600, W700, W800, W900
};

/// نمط الخط
enum class FontStyle {
    Normal,     // عادي
    Italic      // مائل
};

/// تجاوز النص
enum class TextOverflow {
    Clip,       // قص
    Ellipsis,   // ثلاث نقاط
    Fade,       // تلاشي
    Visible     // ظاهر
};

/// لون
struct Color {
    uint8_t r = 0, g = 0, b = 0, a = 255;
    
    Color() = default;
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : r(r), g(g), b(b), a(a) {}
    
    /// ألوان جاهزة
    static Color black()       { return {0, 0, 0}; }
    static Color white()       { return {255, 255, 255}; }
    static Color red()         { return {244, 67, 54}; }
    static Color green()       { return {76, 175, 80}; }
    static Color blue()        { return {33, 150, 243}; }
    static Color yellow()      { return {255, 235, 59}; }
    static Color orange()      { return {255, 152, 0}; }
    static Color purple()      { return {156, 39, 176}; }
    static Color grey()        { return {158, 158, 158}; }
    static Color transparent() { return {0, 0, 0, 0}; }
    
    /// من HEX
    static Color fromHex(uint32_t hex) {
        return {
            uint8_t((hex >> 16) & 0xFF),
            uint8_t((hex >> 8) & 0xFF),
            uint8_t(hex & 0xFF),
            255
        };
    }
};

/// حواف
struct EdgeInsets {
    float top = 0, right = 0, bottom = 0, left = 0;
    
    EdgeInsets() = default;
    EdgeInsets(float all) : top(all), right(all), bottom(all), left(all) {}
    EdgeInsets(float vertical, float horizontal)
        : top(vertical), right(horizontal), bottom(vertical), left(horizontal) {}
    EdgeInsets(float t, float r, float b, float l)
        : top(t), right(r), bottom(b), left(l) {}
    
    static EdgeInsets all(float v) { return EdgeInsets(v); }
    static EdgeInsets symmetric(float v = 0, float h = 0) { return EdgeInsets(v, h); }
    static EdgeInsets only(float t = 0, float r = 0, float b = 0, float l = 0) {
        return EdgeInsets(t, r, b, l);
    }
};

/// انحناء الحواف
struct BorderRadius {
    float topLeft = 0, topRight = 0, bottomRight = 0, bottomLeft = 0;
    
    BorderRadius() = default;
    BorderRadius(float all)
        : topLeft(all), topRight(all), bottomRight(all), bottomLeft(all) {}
    
    static BorderRadius all(float r) { return BorderRadius(r); }
    static BorderRadius circular(float r) { return BorderRadius(r); }
    static BorderRadius only(float tl = 0, float tr = 0, float br = 0, float bl = 0) {
        BorderRadius b;
        b.topLeft = tl; b.topRight = tr; b.bottomRight = br; b.bottomLeft = bl;
        return b;
    }
};

/// الحدود
struct Border {
    float width = 0;
    Color color = Color::grey();
    
    Border() = default;
    Border(float w, Color c = Color::grey()) : width(w), color(c) {}
    
    static Border all(float width = 1, Color color = Color::grey()) {
        return Border(width, color);
    }
};

/// الظل
struct BoxShadow {
    Color color = Color(0, 0, 0, 64);
    float blurRadius = 4;
    float spreadRadius = 0;
    float offsetX = 0, offsetY = 2;
};

/// نمط النص
struct TextStyle {
    float fontSize = 14;
    FontWeight fontWeight = FontWeight::Normal;
    FontStyle fontStyle = FontStyle::Normal;
    Color color = Color::black();
    std::string fontFamily;
    float letterSpacing = 0;
    float wordSpacing = 0;
    float height = 1.0f; // line height multiplier
    
    TextStyle() = default;
    
    TextStyle& setFontSize(float s) { fontSize = s; return *this; }
    TextStyle& setBold() { fontWeight = FontWeight::Bold; return *this; }
    TextStyle& setColor(Color c) { color = c; return *this; }
    TextStyle& setFontFamily(const std::string& f) { fontFamily = f; return *this; }
};

/// قيود الحجم
struct BoxConstraints {
    float minWidth = 0, maxWidth = 999999;
    float minHeight = 0, maxHeight = 999999;
    
    BoxConstraints() = default;
    BoxConstraints(float minW, float maxW, float minH, float maxH)
        : minWidth(minW), maxWidth(maxW), minHeight(minH), maxHeight(maxH) {}
    
    static BoxConstraints tight(float w, float h) {
        return BoxConstraints(w, w, h, h);
    }
    static BoxConstraints loose(float w, float h) {
        return BoxConstraints(0, w, 0, h);
    }
};

/// التزيين (decoration)
struct BoxDecoration {
    std::optional<Color> color;
    std::optional<BorderRadius> borderRadius;
    std::optional<Border> border;
    std::vector<BoxShadow> boxShadow;
    // TODO: gradient, image
    
    BoxDecoration& setColor(Color c) { color = c; return *this; }
    BoxDecoration& setRadius(float r) { borderRadius = BorderRadius::all(r); return *this; }
    BoxDecoration& setBorder(Border b) { border = b; return *this; }
    BoxDecoration& addShadow(BoxShadow s) { boxShadow.push_back(s); return *this; }
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Text — نص
// ═══════════════════════════════════════════════════════════════════════════════

/// RenderObject للنص
class TextRenderObject : public RenderObject {
public:
    std::string text;
    TextStyle style;
    TextAlign textAlign = TextAlign::Right;
    int maxLines = 0; // 0 = unlimited
    TextOverflow overflow = TextOverflow::Clip;
    
    void layout(float maxWidth, float maxHeight) override;
    void paint() override;
};

/// Text — مكون عرض النص
class Text : public RenderObjectWidget {
public:
    Text(std::string data,
         TextStyle style = {},
         TextAlign textAlign = TextAlign::Right,
         int maxLines = 0,
         TextOverflow overflow = TextOverflow::Clip,
         Key key = Key())
        : RenderObjectWidget(std::move(key))
        , data_(std::move(data))
        , style_(std::move(style))
        , textAlign_(textAlign)
        , maxLines_(maxLines)
        , overflow_(overflow)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override {
        auto ro = std::make_unique<TextRenderObject>();
        ro->text = data_;
        ro->style = style_;
        ro->textAlign = textAlign_;
        ro->maxLines = maxLines_;
        ro->overflow = overflow_;
        return ro;
    }
    
    void updateRenderObject(BuildContext* ctx, RenderObject* ro) override {
        auto* tro = static_cast<TextRenderObject*>(ro);
        tro->text = data_;
        tro->style = style_;
        tro->textAlign = textAlign_;
        tro->maxLines = maxLines_;
        tro->overflow = overflow_;
    }
    
    std::string typeName() const override { return "Text"; }
    
private:
    std::string data_;
    TextStyle style_;
    TextAlign textAlign_;
    int maxLines_;
    TextOverflow overflow_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Container — حاوية
// ═══════════════════════════════════════════════════════════════════════════════

/// RenderObject للحاوية
class ContainerRenderObject : public RenderObject {
public:
    std::optional<float> width, height;
    EdgeInsets padding, margin;
    BoxDecoration decoration;
    
    void layout(float maxWidth, float maxHeight) override;
    void paint() override;
};

/// Container — حاوية مع تزيين
class Container : public SingleChildRenderObjectWidget {
public:
    Container(
        WidgetPtr child = nullptr,
        std::optional<float> width = std::nullopt,
        std::optional<float> height = std::nullopt,
        EdgeInsets padding = {},
        EdgeInsets margin = {},
        BoxDecoration decoration = {},
        Key key = Key()
    )
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , width_(width)
        , height_(height)
        , padding_(padding)
        , margin_(margin)
        , decoration_(std::move(decoration))
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override {
        auto ro = std::make_unique<ContainerRenderObject>();
        ro->width = width_;
        ro->height = height_;
        ro->padding = padding_;
        ro->margin = margin_;
        ro->decoration = decoration_;
        return ro;
    }
    
    void updateRenderObject(BuildContext* ctx, RenderObject* ro) override {
        auto* cro = static_cast<ContainerRenderObject*>(ro);
        cro->width = width_;
        cro->height = height_;
        cro->padding = padding_;
        cro->margin = margin_;
        cro->decoration = decoration_;
    }
    
    std::string typeName() const override { return "Container"; }
    
private:
    std::optional<float> width_, height_;
    EdgeInsets padding_, margin_;
    BoxDecoration decoration_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  SizedBox — صندوق بحجم ثابت
// ═══════════════════════════════════════════════════════════════════════════════

class SizedBox : public SingleChildRenderObjectWidget {
public:
    SizedBox(
        std::optional<float> width = std::nullopt,
        std::optional<float> height = std::nullopt,
        WidgetPtr child = nullptr,
        Key key = Key()
    )
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , width_(width)
        , height_(height)
    {}
    
    /// صندوق مربع
    static std::shared_ptr<SizedBox> square(float dim, WidgetPtr child = nullptr) {
        return std::make_shared<SizedBox>(dim, dim, std::move(child));
    }
    
    /// صندوق بعرض فقط
    static std::shared_ptr<SizedBox> fromWidth(float w, WidgetPtr child = nullptr) {
        return std::make_shared<SizedBox>(w, std::nullopt, std::move(child));
    }
    
    /// صندوق بارتفاع فقط
    static std::shared_ptr<SizedBox> fromHeight(float h, WidgetPtr child = nullptr) {
        return std::make_shared<SizedBox>(std::nullopt, h, std::move(child));
    }
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "SizedBox"; }
    
private:
    std::optional<float> width_, height_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  GestureDetector — كاشف اللمس
// ═══════════════════════════════════════════════════════════════════════════════

using VoidCallback = std::function<void()>;
using TapCallback = std::function<void(float x, float y)>;

class GestureDetector : public SingleChildRenderObjectWidget {
public:
    GestureDetector(
        WidgetPtr child,
        VoidCallback onTap = nullptr,
        VoidCallback onDoubleTap = nullptr,
        VoidCallback onLongPress = nullptr,
        TapCallback onTapDown = nullptr,
        TapCallback onTapUp = nullptr,
        Key key = Key()
    )
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , onTap_(std::move(onTap))
        , onDoubleTap_(std::move(onDoubleTap))
        , onLongPress_(std::move(onLongPress))
        , onTapDown_(std::move(onTapDown))
        , onTapUp_(std::move(onTapUp))
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "GestureDetector"; }
    
    VoidCallback onTap() const { return onTap_; }
    
private:
    VoidCallback onTap_, onDoubleTap_, onLongPress_;
    TapCallback onTapDown_, onTapUp_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  ElevatedButton — زر مرتفع
// ═══════════════════════════════════════════════════════════════════════════════

class ElevatedButton : public StatelessWidget {
public:
    ElevatedButton(
        WidgetPtr child,
        VoidCallback onPressed,
        Color backgroundColor = Color::blue(),
        Color foregroundColor = Color::white(),
        EdgeInsets padding = EdgeInsets::symmetric(12, 24),
        BorderRadius borderRadius = BorderRadius::all(8),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , child_(std::move(child))
        , onPressed_(std::move(onPressed))
        , backgroundColor_(backgroundColor)
        , foregroundColor_(foregroundColor)
        , padding_(padding)
        , borderRadius_(borderRadius)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "ElevatedButton"; }
    
private:
    WidgetPtr child_;
    VoidCallback onPressed_;
    Color backgroundColor_, foregroundColor_;
    EdgeInsets padding_;
    BorderRadius borderRadius_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  TextButton — زر نصي
// ═══════════════════════════════════════════════════════════════════════════════

class TextButton : public StatelessWidget {
public:
    TextButton(
        WidgetPtr child,
        VoidCallback onPressed,
        Color foregroundColor = Color::blue(),
        EdgeInsets padding = EdgeInsets::symmetric(8, 16),
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , child_(std::move(child))
        , onPressed_(std::move(onPressed))
        , foregroundColor_(foregroundColor)
        , padding_(padding)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "TextButton"; }
    
private:
    WidgetPtr child_;
    VoidCallback onPressed_;
    Color foregroundColor_;
    EdgeInsets padding_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Icon — أيقونة
// ═══════════════════════════════════════════════════════════════════════════════

/// أيقونات مدمجة (Material Icons style)
enum class Icons {
    Add, Remove, Edit, Delete, Save, Close, Menu,
    Home, Settings, Search, Person, Star, Favorite,
    Check, Clear, ArrowBack, ArrowForward, MoreVert,
    Visibility, VisibilityOff, Lock, LockOpen,
    Refresh, Share, Download, Upload, Camera,
    // يمكن إضافة المزيد
    _Count
};

class IconRenderObject : public RenderObject {
public:
    Icons icon = Icons::Star;
    float size = 24;
    Color color = Color::black();
    
    void layout(float maxWidth, float maxHeight) override;
    void paint() override;
};

class Icon : public RenderObjectWidget {
public:
    Icon(Icons icon, float size = 24, Color color = Color::black(), Key key = Key())
        : RenderObjectWidget(std::move(key))
        , icon_(icon)
        , size_(size)
        , color_(color)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override {
        auto ro = std::make_unique<IconRenderObject>();
        ro->icon = icon_;
        ro->size = size_;
        ro->color = color_;
        return ro;
    }
    
    void updateRenderObject(BuildContext* ctx, RenderObject* ro) override {
        auto* iro = static_cast<IconRenderObject*>(ro);
        iro->icon = icon_;
        iro->size = size_;
        iro->color = color_;
    }
    
    std::string typeName() const override { return "Icon"; }
    
private:
    Icons icon_;
    float size_;
    Color color_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  IconButton — زر أيقونة
// ═══════════════════════════════════════════════════════════════════════════════

class IconButton : public StatelessWidget {
public:
    IconButton(
        Icons icon,
        VoidCallback onPressed,
        float size = 24,
        Color color = Color::black(),
        float splashRadius = 20,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , icon_(icon)
        , onPressed_(std::move(onPressed))
        , size_(size)
        , color_(color)
        , splashRadius_(splashRadius)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "IconButton"; }
    
private:
    Icons icon_;
    VoidCallback onPressed_;
    float size_;
    Color color_;
    float splashRadius_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Divider — خط فاصل
// ═══════════════════════════════════════════════════════════════════════════════

class Divider : public RenderObjectWidget {
public:
    Divider(
        float height = 1,
        float thickness = 1,
        Color color = Color::grey(),
        float indent = 0,
        float endIndent = 0,
        Key key = Key()
    )
        : RenderObjectWidget(std::move(key))
        , height_(height)
        , thickness_(thickness)
        , color_(color)
        , indent_(indent)
        , endIndent_(endIndent)
    {}
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Divider"; }
    
private:
    float height_, thickness_, indent_, endIndent_;
    Color color_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Spacer — فاصل مرن
// ═══════════════════════════════════════════════════════════════════════════════

class Spacer : public RenderObjectWidget {
public:
    Spacer(int flex = 1, Key key = Key())
        : RenderObjectWidget(std::move(key))
        , flex_(flex)
    {}
    
    int flex() const { return flex_; }
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Spacer"; }
    
private:
    int flex_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Opacity — شفافية
// ═══════════════════════════════════════════════════════════════════════════════

class Opacity : public SingleChildRenderObjectWidget {
public:
    Opacity(float opacity, WidgetPtr child, Key key = Key())
        : SingleChildRenderObjectWidget(std::move(child), std::move(key))
        , opacity_(std::max(0.0f, std::min(1.0f, opacity)))
    {}
    
    float opacity() const { return opacity_; }
    
    std::unique_ptr<RenderObject> createRenderObject(BuildContext* ctx) override;
    std::string typeName() const override { return "Opacity"; }
    
private:
    float opacity_;
};

// ═══════════════════════════════════════════════════════════════════════════════
//  Visibility — إظهار/إخفاء
// ═══════════════════════════════════════════════════════════════════════════════

class Visibility : public StatelessWidget {
public:
    Visibility(
        WidgetPtr child,
        bool visible = true,
        bool maintainSize = false,
        Key key = Key()
    )
        : StatelessWidget(std::move(key))
        , child_(std::move(child))
        , visible_(visible)
        , maintainSize_(maintainSize)
    {}
    
    WidgetPtr build(BuildContext* ctx) override;
    std::string typeName() const override { return "Visibility"; }
    
private:
    WidgetPtr child_;
    bool visible_, maintainSize_;
};

} // namespace flutter
} // namespace sad
