# 🎨 محرك الرسومات / Graphics Engine Plan (s_graphics)

**المرحلة / Phase:** 4  
**المدة / Duration:** 3-6 أشهر / months  
**الأولوية / Priority:** P1 (مهمة / Important)

---

## 📋 نظرة عامة / Overview

### (AR) الوصف
محرك رسومات عصري مستوحى من Flutter يوفر واجهات مستخدم تفاعلية بنمط تصريحي (Declarative). يدعم Widgets قابلة للتخصيص، نظام Layout متقدم، Hot Reload، وأداء عالٍ عبر منصات متعددة.

### (EN) Description
Modern graphics engine inspired by Flutter providing interactive UIs with declarative style. Supports customizable widgets, advanced layout system, hot reload, and high performance across multiple platforms.

---

## 🎯 الأهداف / Objectives

### الأهداف الرئيسية

1. **Declarative UI**
   - تصميم واجهات بنمط تصريحي
   - Reactive updates تلقائية
   - Component-based architecture

2. **Rich Widget Library**
   - 30+ widgets أساسية
   - قابلة للتخصيص والتوسيع
   - أنماط متعددة (Material, Cupertino, Custom)

3. **Advanced Layout System**
   - Flexbox-like layout
   - Constraints-based positioning
   - Responsive design support

4. **High Performance**
   - 60 FPS rendering
   - Efficient repainting
   - GPU acceleration

5. **Developer Experience**
   - Hot reload للتطوير السريع
   - Widget inspector
   - Performance profiler

---

## 🏗️ المعمارية / Architecture

### النظرة العامة

```
┌────────────────────────────────────────────────────────┐
│                  Application Layer                     │
│  ┌──────────────────────────────────────────────────┐ │
│  │         Sad Code (UI Description)                │ │
│  │  نافذة(                                          │ │
│  │    عنوان: "تطبيقي"،                             │ │
│  │    محتوى: عمود([                                │ │
│  │      نص("مرحباً")،                               │ │
│  │      زر(عند_الضغط: {...})                       │ │
│  │    ])                                            │ │
│  │  )                                               │ │
│  └──────────────────────────────────────────────────┘ │
└────────────────┬───────────────────────────────────────┘
                 │
┌────────────────▼───────────────────────────────────────┐
│               Widget Layer (s_graphics)                 │
├─────────────────────────────────────────────────────────┤
│  Widget Tree Management                                 │
│  ├─ StatelessWidget                                     │
│  ├─ StatefulWidget                                      │
│  ├─ Widget Lifecycle                                    │
│  └─ State Management                                    │
├─────────────────────────────────────────────────────────┤
│  Layout Engine                                          │
│  ├─ Constraint System                                   │
│  ├─ Layout Algorithm (Flexbox)                          │
│  ├─ Size Negotiation                                    │
│  └─ Position Calculation                                │
├─────────────────────────────────────────────────────────┤
│  Rendering Pipeline                                     │
│  ├─ Render Tree Building                                │
│  ├─ Paint Operations                                    │
│  ├─ Compositing Layers                                  │
│  └─ Damage Tracking (Dirty Regions)                     │
├─────────────────────────────────────────────────────────┤
│  Event System                                           │
│  ├─ Hit Testing                                         │
│  ├─ Gesture Recognition                                 │
│  ├─ Event Propagation                                   │
│  └─ Focus Management                                    │
└────────────────┬────────────────────────────────────────┘
                 │
┌────────────────▼────────────────────────────────────────┐
│            Platform Layer (Backends)                    │
├──────────────────────────────────────────────────────────┤
│  Native Backends                                         │
│  ├─ Windows (DirectX/GDI+)                              │
│  ├─ Linux (X11/Wayland + OpenGL)                       │
│  ├─ macOS (Metal/Cocoa)                                 │
│  └─ Web (Canvas/WebGL)                                  │
├──────────────────────────────────────────────────────────┤
│  Graphics Libraries                                      │
│  ├─ Skia (Primary - cross-platform)                    │
│  ├─ Cairo (Alternative)                                 │
│  └─ Direct rendering (fallback)                         │
└──────────────────────────────────────────────────────────┘
```

---

## 📂 هيكل المشروع / Project Structure

```
s_graphics/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── widgets/
│   │   ├── widget.h                 # Base widget class
│   │   ├── stateless_widget.h
│   │   ├── stateful_widget.h
│   │   ├── container.h
│   │   ├── text.h
│   │   ├── button.h
│   │   ├── image.h
│   │   ├── column.h
│   │   ├── row.h
│   │   ├── stack.h
│   │   ├── padding.h
│   │   ├── center.h
│   │   └── ...                      # 30+ widgets
│   ├── layout/
│   │   ├── constraints.h            # Layout constraints
│   │   ├── box_constraints.h
│   │   ├── flex_layout.h            # Flexbox layout
│   │   ├── grid_layout.h
│   │   └── custom_layout.h
│   ├── rendering/
│   │   ├── render_object.h          # Base render object
│   │   ├── render_box.h
│   │   ├── render_flex.h
│   │   ├── paint_context.h
│   │   ├── canvas.h
│   │   └── layer.h
│   ├── painting/
│   │   ├── color.h
│   │   ├── paint.h
│   │   ├── text_style.h
│   │   ├── decoration.h
│   │   └── gradient.h
│   ├── events/
│   │   ├── event.h
│   │   ├── pointer_event.h
│   │   ├── keyboard_event.h
│   │   ├── gesture_detector.h
│   │   └── focus_manager.h
│   ├── animation/
│   │   ├── animation.h
│   │   ├── tween.h
│   │   ├── animation_controller.h
│   │   └── curves.h
│   └── platform/
│       ├── window.h
│       ├── platform_backend.h
│       ├── skia_backend.h
│       └── native_backend.h
├── src/
│   ├── widgets/
│   ├── layout/
│   ├── rendering/
│   ├── painting/
│   ├── events/
│   ├── animation/
│   └── platform/
├── bindings/
│   └── sad/
│       ├── graphics_bindings.h      # FFI to Sad language
│       └── graphics_bindings.cpp
├── examples/
│   ├── hello_world.s
│   ├── counter_app.s
│   ├── todo_list.s
│   └── gallery.s
└── tests/
    ├── widget_tests.cpp
    ├── layout_tests.cpp
    ├── rendering_tests.cpp
    └── integration_tests.cpp
```

---

## 🎨 الـ Widgets الأساسية / Core Widgets

### 1. Container Widgets

#### 1.1 Container
```cpp
// Container: أساسي لجميع الـ widgets
class Container : public StatelessWidget {
public:
    Container({
        Widget* child = nullptr,
        Color backgroundColor,
        EdgeInsets padding,
        EdgeInsets margin,
        Decoration decoration,
        double width, double height,
        Alignment alignment
    });
};
```

**مثال بلغة ص:**
```sad
متغير صندوق = حاوية(
    عرض: 200،
    ارتفاع: 100،
    لون_خلفية: أزرق،
    حشو: حشو_كل(16)،
    محتوى: نص("مرحباً")
)
```

---

#### 1.2 Column & Row
```cpp
// Column: ترتيب عمودي
class Column : public MultiChildWidget {
public:
    Column({
        std::vector<Widget*> children,
        MainAxisAlignment mainAxisAlignment,
        CrossAxisAlignment crossAxisAlignment,
        MainAxisSize mainAxisSize
    });
};

// Row: ترتيب أفقي
class Row : public MultiChildWidget {
    // Similar to Column but horizontal
};
```

**مثال:**
```sad
عمود(
    محاذاة_رئيسية: وسط،
    أطفال: [
        نص("العنوان"، نمط: نمط_نص(حجم: 24)),
        صف(
            أطفال: [
                زر("نعم"، عند_الضغط: {...}),
                زر("لا"، عند_الضغط: {...})
            ]
        )
    ]
)
```

---

### 2. Basic Widgets

#### 2.1 Text
```cpp
class Text : public StatelessWidget {
public:
    Text(const std::string& text, {
        TextStyle style,
        TextAlign align,
        int maxLines,
        TextOverflow overflow
    });
};

struct TextStyle {
    std::string fontFamily;
    double fontSize;
    FontWeight fontWeight;
    Color color;
    TextDecoration decoration;
};
```

**مثال:**
```sad
نص(
    "مرحباً بالعالم"،
    نمط: نمط_نص(
        حجم: 18،
        لون: أحمر،
        سمك: غامق،
        خط: "Arial"
    )،
    محاذاة: وسط
)
```

---

#### 2.2 Button
```cpp
class Button : public StatefulWidget {
public:
    Button({
        std::string text,
        std::function<void()> onPressed,
        ButtonStyle style,
        Widget* child
    });
};

struct ButtonStyle {
    Color backgroundColor;
    Color foregroundColor;
    EdgeInsets padding;
    BorderRadius borderRadius;
    double elevation;
};
```

**مثال:**
```sad
زر(
    "اضغط هنا"،
    عند_الضغط: دالة() {
        اطبع("تم الضغط!")
    }،
    نمط: نمط_زر(
        لون_خلفية: أزرق،
        حشو: حشو_كل(12)
    )
)
```

---

#### 2.3 Image
```cpp
class Image : public StatelessWidget {
public:
    // From file
    static Image* file(const std::string& path);
    
    // From network
    static Image* network(const std::string& url);
    
    // From memory
    static Image* memory(const std::vector<uint8_t>& bytes);
    
    Image({
        double width, double height,
        BoxFit fit,
        Alignment alignment
    });
};
```

**مثال:**
```sad
صورة(
    مسار: "assets/logo.png"،
    عرض: 100،
    ارتفاع: 100،
    ملاءمة: ملاءمة_احتواء
)
```

---

### 3. Input Widgets

#### 3.1 TextField
```cpp
class TextField : public StatefulWidget {
public:
    TextField({
        std::string placeholder,
        TextEditingController* controller,
        std::function<void(std::string)> onChanged,
        std::function<void(std::string)> onSubmitted,
        InputDecoration decoration,
        bool obscureText = false
    });
};
```

**مثال:**
```sad
متغير مراقب = مراقب_نص()

حقل_نص(
    عنصر_نائب: "أدخل اسمك"،
    مراقب: مراقب،
    عند_التغيير: دالة(قيمة) {
        اطبع("القيمة: " + قيمة)
    }،
    زخرفة: زخرفة_إدخال(
        أيقونة: أيقونة_شخص،
        تسمية: "الاسم"
    )
)
```

---

#### 3.2 Checkbox & Radio
```cpp
class Checkbox : public StatefulWidget {
public:
    Checkbox({
        bool value,
        std::function<void(bool)> onChanged,
        Color activeColor
    });
};

class Radio : public StatefulWidget {
public:
    Radio({
        int value,
        int groupValue,
        std::function<void(int)> onChanged
    });
};
```

---

### 4. Layout Widgets

#### 4.1 Padding & Margin
```cpp
class Padding : public SingleChildWidget {
public:
    Padding({
        EdgeInsets padding,
        Widget* child
    });
};

struct EdgeInsets {
    double top, right, bottom, left;
    
    static EdgeInsets all(double value);
    static EdgeInsets symmetric({double vertical, double horizontal});
    static EdgeInsets only({double top, double right, double bottom, double left});
};
```

**مثال:**
```sad
حشو(
    قيمة: حشو_متماثل(عمودي: 20، أفقي: 16)،
    طفل: نص("محتوى")
)
```

---

#### 4.2 Center & Align
```cpp
class Center : public SingleChildWidget {
public:
    Center({Widget* child});
};

class Align : public SingleChildWidget {
public:
    Align({
        Alignment alignment,
        Widget* child
    });
};
```

---

#### 4.3 Stack & Positioned
```cpp
class Stack : public MultiChildWidget {
public:
    Stack({
        std::vector<Widget*> children,
        StackFit fit,
        Alignment alignment
    });
};

class Positioned : public SingleChildWidget {
public:
    Positioned({
        double top, double right, double bottom, double left,
        Widget* child
    });
};
```

**مثال:**
```sad
كومة(
    أطفال: [
        صورة("background.png")،
        موضع(
            أعلى: 20،
            يسار: 20،
            طفل: نص("نص فوق الصورة")
        )
    ]
)
```

---

### 5. Advanced Widgets

#### 5.1 ListView
```cpp
class ListView : public StatefulWidget {
public:
    ListView({
        std::vector<Widget*> children,
        Axis scrollDirection,
        EdgeInsets padding
    });
    
    // Builder pattern
    static ListView* builder({
        int itemCount,
        std::function<Widget*(BuildContext, int)> itemBuilder
    });
};
```

**مثال:**
```sad
قائمة_عرض(
    أطفال: [
        بطاقة(محتوى: نص("عنصر 1"))،
        بطاقة(محتوى: نص("عنصر 2"))،
        بطاقة(محتوى: نص("عنصر 3"))
    ]
)

# أو مع builder
قائمة_عرض_بناء(
    عدد: 100،
    باني: دالة(سياق، فهرس) {
        أرجع بطاقة(
            محتوى: نص("عنصر " + نص(فهرس))
        )
    }
)
```

---

#### 5.2 GridView
```cpp
class GridView : public StatefulWidget {
public:
    GridView({
        std::vector<Widget*> children,
        int crossAxisCount,
        double mainAxisSpacing,
        double crossAxisSpacing
    });
};
```

---

#### 5.3 Card
```cpp
class Card : public StatelessWidget {
public:
    Card({
        Widget* child,
        Color color,
        double elevation,
        EdgeInsets margin
    });
};
```

---

## 🎭 نظام الحالة / State Management

### StatefulWidget Example

```cpp
// C++ Implementation
class CounterWidget : public StatefulWidget {
public:
    State* createState() override {
        return new CounterState();
    }
};

class CounterState : public State<CounterWidget> {
private:
    int counter_ = 0;
    
public:
    void increment() {
        setState([this]() {
            counter_++;
        });
    }
    
    Widget* build(BuildContext* context) override {
        return new Column({
            new Text("العداد: " + std::to_string(counter_)),
            new Button({
                .text = "زيادة",
                .onPressed = [this]() { increment(); }
            })
        });
    }
};
```

**مثال بلغة ص:**
```sad
صنف عداد يرث ويدجت_بحالة {
    خاص عداد: رقم = 0
    
    دالة زيادة() {
        تعيين_الحالة(دالة() {
            هذا.عداد = هذا.عداد + 1
        })
    }
    
    دالة بناء(سياق) {
        أرجع عمود(
            أطفال: [
                نص("العداد: " + نص(هذا.عداد))،
                زر(
                    "زيادة"،
                    عند_الضغط: هذا.زيادة
                )
            ]
        )
    }
}

# الاستخدام
متغير تطبيقي = عداد()
```

---

## 🎨 نظام الـ Layout / Layout System

### Constraints-Based Layout

```cpp
struct BoxConstraints {
    double minWidth, maxWidth;
    double minHeight, maxHeight;
    
    bool isTight() const;
    bool isUnbounded() const;
    Size constrain(Size size) const;
};

class RenderBox {
public:
    // Layout protocol
    virtual Size computeDryLayout(BoxConstraints constraints);
    virtual void performLayout();
    
protected:
    BoxConstraints constraints_;
    Size size_;
};
```

### Layout Algorithm
```
1. Parent passes constraints to child
2. Child chooses size within constraints
3. Parent positions child
4. Parent returns its own size
```

---

## 🖌️ نظام الرسم / Painting System

### Canvas API

```cpp
class Canvas {
public:
    // Basic shapes
    void drawRect(const Rect& rect, const Paint& paint);
    void drawCircle(const Point& center, double radius, const Paint& paint);
    void drawLine(const Point& p1, const Point& p2, const Paint& paint);
    void drawPath(const Path& path, const Paint& paint);
    
    // Text
    void drawText(const std::string& text, const Point& position, 
                 const TextStyle& style);
    
    // Images
    void drawImage(const Image& image, const Point& position);
    
    // Transforms
    void save();
    void restore();
    void translate(double dx, double dy);
    void rotate(double radians);
    void scale(double sx, double sy);
};
```

---

## ⚡ Hot Reload

### Implementation

```cpp
class HotReloadManager {
public:
    void watchFile(const std::string& path);
    void onFileChanged(const std::string& path);
    void reloadWidget(Widget* widget);
    
private:
    // استخراج حالة Widget
    void extractState(Widget* widget);
    
    // إعادة بناء مع الحفاظ على الحالة
    Widget* rebuildWithState(Widget* newWidget);
};
```

**الاستخدام:**
```bash
# تشغيل مع hot reload
sad run --hot-reload my_app.s

# عند تعديل my_app.s
# → التطبيق يُحدّث تلقائياً دون إعادة تشغيل
```

---

## 📊 خطة التنفيذ / Implementation Plan

### المرحلة 4.1: الأساسيات (شهر 1-2)

**الأسبوع 1-2: Core Infrastructure**
- [ ] Widget base classes
- [ ] BuildContext
- [ ] State management
- [ ] Basic rendering pipeline

**الأسبوع 3-4: Basic Widgets**
- [ ] Container, Text, Button
- [ ] Column, Row, Stack
- [ ] Padding, Center, Align

**الأسبوع 5-6: Layout System**
- [ ] BoxConstraints
- [ ] Layout algorithm
- [ ] Size negotiation

**الأسبوع 7-8: Platform Integration**
- [ ] Window creation
- [ ] Skia integration
- [ ] Event loop

---

### المرحلة 4.2: الميزات المتقدمة (شهر 2-3)

**الشهر 2:**
- [ ] Input widgets (TextField, Checkbox, Radio)
- [ ] Image loading
- [ ] ListView & GridView
- [ ] Gesture detection

**الشهر 3:**
- [ ] Animation system
- [ ] Themes & styling
- [ ] Focus management
- [ ] Accessibility

---

### المرحلة 4.3: الأدوات والتحسينات (شهر 3-4)

- [ ] Hot reload
- [ ] Widget inspector
- [ ] Performance profiler
- [ ] Optimization (GPU, caching)

---

## ✅ معايير القبول / Acceptance Criteria

### الأداء (Performance)
- ✅ 60 FPS في معظم الحالات
- ✅ Startup time < 500ms
- ✅ Memory usage < 100MB لتطبيق متوسط

### الوظائف (Features)
- ✅ 30+ widgets أساسية
- ✅ Layout system كامل
- ✅ Event handling شامل
- ✅ Hot reload يعمل

### تجربة المطور (DX)
- ✅ API واضح وبسيط
- ✅ أمثلة شاملة
- ✅ توثيق كامل
- ✅ أدوات تطوير مفيدة

---

## 📚 أمثلة تطبيقات / Example Applications

### مثال 1: Counter App
```sad
صنف تطبيق_عداد يرث ويدجت_بحالة {
    خاص عداد = 0
    
    دالة بناء(سياق) {
        أرجع نافذة(
            عنوان: "تطبيق العداد"،
            محتوى: مركز(
                طفل: عمود(
                    محاذاة_رئيسية: وسط،
                    أطفال: [
                        نص(
                            "العداد:"،
                            نمط: نمط_نص(حجم: 20)
                        )،
                        نص(
                            نص(هذا.عداد)،
                            نمط: نمط_نص(حجم: 48، سمك: غامق)
                        )،
                        زر(
                            "زيادة"،
                            عند_الضغط: دالة() {
                                تعيين_الحالة(دالة() {
                                    هذا.عداد = هذا.عداد + 1
                                })
                            }
                        )
                    ]
                )
            )
        )
    }
}

# تشغيل التطبيق
تشغيل_تطبيق(تطبيق_عداد())
```

---

### مثال 2: Todo List
```sad
صنف قائمة_مهام يرث ويدجت_بحالة {
    خاص مهام: مصفوفة = []
    خاص مراقب_نص = مراقب_نص()
    
    دالة إضافة_مهمة() {
        متغير نص = هذا.مراقب_نص.نص
        إذا (نص != "") {
            تعيين_الحالة(دالة() {
                أضف(هذا.مهام، نص)
                هذا.مراقب_نص.امسح()
            })
        }
    }
    
    دالة بناء(سياق) {
        أرجع نافذة(
            عنوان: "قائمة المهام"،
            محتوى: عمود(
                أطفال: [
                    # حقل الإدخال
                    حشو(
                        قيمة: حشو_كل(16)،
                        طفل: صف(
                            أطفال: [
                                موسع(
                                    طفل: حقل_نص(
                                        مراقب: هذا.مراقب_نص،
                                        عنصر_نائب: "مهمة جديدة"
                                    )
                                )،
                                زر("إضافة"، عند_الضغط: هذا.إضافة_مهمة)
                            ]
                        )
                    )،
                    # قائمة المهام
                    موسع(
                        طفل: قائمة_عرض_بناء(
                            عدد: الطول(هذا.مهام)،
                            باني: دالة(سياق، فهرس) {
                                أرجع بطاقة(
                                    هامش: حشو_كل(8)،
                                    طفل: حشو(
                                        قيمة: حشو_كل(16)،
                                        طفل: نص(هذا.مهام[فهرس])
                                    )
                                )
                            }
                        )
                    )
                ]
            )
        )
    }
}
```

---

**آخر تحديث:** 29 نوفمبر 2025  
**الحالة:** ✅ خطة جاهزة للتنفيذ
