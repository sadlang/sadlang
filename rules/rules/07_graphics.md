# 🎨 مكتبة الرسوم / Graphics Library

**التاريخ:** 10 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مرجع تفصيلي لمكتبة الرسوم

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **مكتبة الرسوم الكاملة** للغة "ص"، بما في ذلك نافذة الرسم، الأشكال، الصور، الأحداث، والتكامل مع backends مختلفة (SDL2، Win32، Direct2D).

### (EN) Description
This file documents the **complete graphics library** for Sad language, including window management, shapes, images, events, and integration with various backends (SDL2, Win32, Direct2D).

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/language_spec/03_oop.md` - OOP for graphics objects
- ✅ `plans/imp/14_graphics_subsystem_plan.md` - Graphics implementation
- ✅ `include/stdlib/graphics/` - Graphics headers

---

## 📋 قائمة الميزات / Features List

| # | الميزة (AR) | Feature (EN) | Backend | الأولوية |
|---|-----------|-----------|---------|---------|
| 1 | نافذة | Window | All | P0 |
| 2 | لوحة رسم | Canvas | All | P0 |
| 3 | أشكال أساسية | Basic Shapes | All | P0 |
| 4 | نصوص | Text Rendering | All | P0 |
| 5 | صور | Images | All | P1 |
| 6 | أحداث | Events | All | P0 |
| 7 | ألوان | Colors | All | P0 |
| 8 | رسوميات متقدمة | Sprites/Animations | SDL2 | P1 |
| 9 | واجهات | Widgets | Phase 2 | P2 |

---

## 1️⃣ Window Management (إدارة النوافذ)

### Syntax
```ebnf
window_creation ::= 'نافذة' '(' width ',' height ',' title ')'

window_methods ::= window '.' method_name '(' [args] ')'
```

### API Reference

```cpp
// include/stdlib/graphics/window.h
class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();
    
    // Window control
    void show();
    void hide();
    void close();
    bool isOpen() const;
    
    // Properties
    void setTitle(const std::string& title);
    void setSize(int width, int height);
    void setPosition(int x, int y);
    
    // Canvas access
    Canvas* getCanvas();
    
    // Event loop
    void processEvents();
    bool pollEvent(Event& event);
    
    // Rendering
    void clear(Color color = Color::Black);
    void display();
};
```

### Examples

```s
// إنشاء نافذة / Create window
استورد رسوم

// نافذة بسيطة / Simple window
نافذة ن = نافذة(800، 600، "مرحباً بالعالم")
ن.اظهر()

// حلقة الأحداث الرئيسية / Main event loop
بينما (ن.مفتوحة())
    // معالجة الأحداث / Process events
    ن.عالج_أحداث()
    
    // مسح الشاشة / Clear screen
    ن.امسح(لون.أسود())
    
    // رسم محتوى / Draw content
    // ... (سيأتي لاحقاً)
    
    // عرض / Display
    ن.اعرض()
نهاية

ن.أغلق()
```

---

## 2️⃣ Canvas & Drawing (لوحة الرسم)

### API Reference

```cpp
// include/stdlib/graphics/canvas.h
class Canvas {
public:
    // Basic shapes
    void drawPoint(int x, int y, Color color);
    void drawLine(int x1, int y1, int x2, int y2, Color color, int thickness = 1);
    void drawRect(int x, int y, int width, int height, Color color, bool filled = false);
    void drawCircle(int x, int y, int radius, Color color, bool filled = false);
    void drawEllipse(int x, int y, int radiusX, int radiusY, Color color, bool filled = false);
    void drawPolygon(const std::vector<Point>& points, Color color, bool filled = false);
    
    // Text
    void drawText(const std::string& text, int x, int y, Color color, int fontSize = 16);
    
    // Images
    void drawImage(Image* image, int x, int y);
    void drawImageScaled(Image* image, int x, int y, int width, int height);
    
    // Transformations
    void translate(int x, int y);
    void rotate(float angle);
    void scale(float sx, float sy);
    void resetTransform();
    
    // State
    void save();
    void restore();
};
```

### Examples

```s
// رسم أشكال أساسية / Draw basic shapes
استورد رسوم

نافذة ن = نافذة(800، 600، "أشكال")
لوحة ل = ن.احصل_لوحة()

بينما( ن.مفتوحة())
    ن.عالج_أحداث()
    ن.امسح(لون.أبيض())
    
    // رسم مستطيل / Draw rectangle
    ل.ارسم_مستطيل(50، 50، 200، 100، لون.أحمر()، صحيح)
    
    // رسم دائرة / Draw circle
    ل.ارسم_دائرة(400، 300، 80، لون.أزرق()، خطأ)
    
    // رسم خط / Draw line
    ل.ارسم_خط(100، 400، 700، 400، لون.أخضر()، 3)
    
    // رسم نص / Draw text
    ل.ارسم_نص("مرحباً بكم!"، 300، 500، لون.أسود()، 24)
    
    ن.اعرض()
نهاية
```

---

## 3️⃣ Colors (الألوان)

### API Reference

```cpp
// include/stdlib/graphics/color.h
class Color {
public:
    uint8_t r, g, b, a;
    
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    
    // Predefined colors
    static Color Black();
    static Color White();
    static Color Red();
    static Color Green();
    static Color Blue();
    static Color Yellow();
    static Color Cyan();
    static Color Magenta();
    
    // From hex
    static Color fromHex(const std::string& hex);
    
    // From HSV
    static Color fromHSV(float h, float s, float v);
};
```

### Examples

```s
// استخدام الألوان / Using colors
استورد رسوم

// ألوان محددة مسبقاً / Predefined colors
لون أحمر = لون.أحمر()
لون أزرق = لون.أزرق()

// لون مخصص RGB / Custom RGB color
لون بنفسجي = لون(128، 0، 128)

// لون مع شفافية / Color with transparency
لون شفاف = لون(255، 0، 0، 128)  // أحمر نصف شفاف

// من hex / From hex
لون برتقالي = لون.من_هكس("#FF8800")

// من HSV / From HSV
لون قوس_قزح = لون.من_HSV(180، 1.0، 1.0)
```

---

## 4️⃣ Events (الأحداث)

### Event Types

```cpp
// include/stdlib/graphics/event.h
enum class EventType {
    None,
    Closed,
    Resized,
    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseWheelScrolled
};

class Event {
public:
    EventType type;
    
    // Key event
    struct KeyEvent {
        int code;
        bool alt;
        bool control;
        bool shift;
    } key;
    
    // Mouse event
    struct MouseEvent {
        int x;
        int y;
        int button;  // 0=left, 1=right, 2=middle
        float wheel;
    } mouse;
    
    // Size event
    struct SizeEvent {
        int width;
        int height;
    } size;
};
```

### Examples

```s
// معالجة الأحداث / Event handling
استورد رسوم

نافذة ن = نافذة(800، 600، "أحداث")
رقم موقع_س = 400
رقم موقع_ص = 300

بينما ن.مفتوحة()
    // معالجة كل حدث / Process each event
    لكل حدث في ن.احصل_أحداث()
        // إغلاق النافذة / Window closed
        إذا حدث.نوع == حدث.مغلق
            ن.أغلق()
        نهاية
        
        // ضغط مفتاح / Key pressed
        إذا حدث.نوع == حدث.مفتاح_مضغوط
            إذا حدث.مفتاح.كود == مفتاح.هروب
                ن.أغلق()
            وإلا_إذا حدث.مفتاح.كود == مفتاح.يسار
                موقع_س = موقع_س - 10
            وإلا_إذا حدث.مفتاح.كود == مفتاح.يمين
                موقع_س = موقع_س + 10
            وإلا_إذا حدث.مفتاح.كود == مفتاح.أعلى
                موقع_ص = موقع_ص - 10
            وإلا_إذا حدث.مفتاح.كود == مفتاح.أسفل
                موقع_ص = موقع_ص + 10
            نهاية
        نهاية
        
        // حركة الماوس / Mouse moved
        إذا حدث.نوع == حدث.ماوس_تحرك
            موقع_س = حدث.ماوس.س
            موقع_ص = حدث.ماوس.ص
        نهاية
        
        // نقر الماوس / Mouse clicked
        إذا حدث.نوع == حدث.ماوس_مضغوط
            إذا حدث.ماوس.زر == 0  // زر أيسر / left button
                اطبع(ن"نقر عند ({حدث.ماوس.س}, {حدث.ماوس.ص})")
            نهاية
        نهاية
    نهاية
    
    // رسم / Draw
    ن.امسح(لون.أبيض())
    لوحة ل = ن.احصل_لوحة()
    ل.ارسم_دائرة(موقع_س، موقع_ص، 20، لون.أحمر()، صحيح)
    ن.اعرض()
نهاية
```

---

## 5️⃣ Images (الصور)

### API Reference

```cpp
// include/stdlib/graphics/image.h
class Image {
public:
    Image(const std::string& filename);
    Image(int width, int height);
    ~Image();
    
    // Properties
    int getWidth() const;
    int getHeight() const;
    
    // Pixel manipulation
    Color getPixel(int x, int y) const;
    void setPixel(int x, int y, Color color);
    
    // Save/Load
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename);
    
    // Effects
    void flipHorizontal();
    void flipVertical();
    void rotate90();
};
```

### Examples

```s
// تحميل وعرض صورة / Load and display image
استورد رسوم

نافذة ن = نافذة(800، 600، "صورة")
صورة ص = صورة("صورة.png")

بينما ن.مفتوحة()
    ن.عالج_أحداث()
    ن.امسح(لون.أبيض())
    
    لوحة ل = ن.احصل_لوحة()
    
    // رسم الصورة / Draw image
    ل.ارسم_صورة(ص، 100، 100)
    
    // رسم صورة مقاسة / Draw scaled image
    ل.ارسم_صورة_مقاسة(ص، 300، 100، 200، 200)
    
    ن.اعرض()
نهاية
```

---

## 6️⃣ Animation & Sprites (الرسوميات المتحركة)

### API Reference

```cpp
// include/stdlib/graphics/sprite.h
class Sprite {
public:
    Sprite(Image* image);
    
    // Position
    void setPosition(float x, float y);
    Point getPosition() const;
    
    // Scale
    void setScale(float sx, float sy);
    
    // Rotation
    void setRotation(float angle);
    
    // Texture rect (for sprite sheets)
    void setTextureRect(int x, int y, int width, int height);
    
    // Rendering
    void draw(Canvas* canvas);
};

class Animation {
public:
    Animation(Image* spriteSheet, int frameWidth, int frameHeight);
    
    void addFrame(int x, int y);
    void setFrameDuration(float seconds);
    void play();
    void pause();
    void reset();
    
    void update(float deltaTime);
    Sprite* getCurrentFrame();
};
```

### Examples

```s
// رسومية متحركة / Animated sprite
استورد رسوم

نافذة ن = نافذة(800، 600، "رسومية")
صورة ورقة = صورة("شخصية.png")

// إنشاء رسومية / Create sprite
رسومية شخصية = رسومية(ورقة)
شخصية.ضع_موقع(400، 300)

// إنشاء حركة / Create animation
حركة مشي = حركة(ورقة، 64، 64)
مشي.أضف_إطار(0، 0)
مشي.أضف_إطار(64، 0)
مشي.أضف_إطار(128، 0)
مشي.أضف_إطار(192، 0)
مشي.ضع_مدة_إطار(0.1)
مشي.شغل()

ساعة س = ساعة()

بينما ن.مفتوحة()
    عشري دلتا = س.إعادة_تشغيل()
    
    ن.عالج_أحداث()
    
    // تحديث الحركة / Update animation
    مشي.حدث(دلتا)
    
    // رسم / Draw
    ن.امسح(لون.أبيض())
    لوحة ل = ن.احصل_لوحة()
    
    رسومية إطار = مشي.احصل_إطار_حالي()
    إطار.ارسم(ل)
    
    ن.اعرض()
نهاية
```

---

## 7️⃣ Backend Implementation (تنفيذ الواجهات الخلفية)

### Architecture

```
┌─────────────────────────────────────┐
│     Sad Graphics API (Abstract)     │
│  (Window, Canvas, Event, etc.)      │
└──────────────┬──────────────────────┘
               │
       ┌───────┴───────┐
       │               │
       ▼               ▼
┌──────────────┐ ┌──────────────┐
│ SDL2 Backend │ │ Win32/D2D    │
│              │ │ Backend      │
└──────────────┘ └──────────────┘
```

### Backend Selection

```cpp
// include/stdlib/graphics/backend.h
enum class GraphicsBackend {
    Auto,      // Auto-detect best available
    SDL2,      // Cross-platform SDL2
    Win32,     // Windows GDI
    Direct2D,  // Windows Direct2D
    OpenGL     // OpenGL (future)
};

class GraphicsSystem {
public:
    static void initialize(GraphicsBackend backend = GraphicsBackend::Auto);
    static void shutdown();
    static GraphicsBackend getCurrentBackend();
};
```

### Implementation Notes

```cpp
// src/stdlib/graphics/window.cpp
Window::Window(int width, int height, const std::string& title) {
    // Select backend based on platform
    #ifdef _WIN32
        if (GraphicsSystem::getCurrentBackend() == GraphicsBackend::Direct2D) {
            impl = new D2DWindowImpl(width, height, title);
        } else {
            impl = new Win32WindowImpl(width, height, title);
        }
    #else
        impl = new SDL2WindowImpl(width, height, title);
    #endif
    
    DEBUG_PRINT("Created window: %dx%d '%s'", width, height, title.c_str());
}

// src/stdlib/graphics/backends/sdl2_window.cpp
class SDL2WindowImpl : public WindowImpl {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    
public:
    SDL2WindowImpl(int w, int h, const std::string& title) {
        SDL_Init(SDL_INIT_VIDEO);
        window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            w, h,
            SDL_WINDOW_SHOWN
        );
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
    
    void clear(Color color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    }
    
    void display() override {
        SDL_RenderPresent(renderer);
    }
    
    // ... more methods
};
```

---

## 📝 Complete Example (مثال كامل)

### Simple Game

```s
// لعبة بسيطة: التقاط الكرات / Simple game: Catch the balls
استورد رسوم
استورد عشوائي

// إعدادات / Settings
رقم عرض_نافذة = 800
رقم ارتفاع_نافذة = 600

// إنشاء نافذة / Create window
نافذة ن = نافذة(عرض_نافذة، ارتفاع_نافذة، "التقاط الكرات")

// اللاعب / Player
صنف لاعب
    رقم س
    رقم ص
    رقم حجم = 30
    لون لون = لون.أزرق()
    
    باني(رقم س، رقم ص)
        هذا.س = س
        هذا.ص = ص
    نهاية
    
    دالة ارسم(لوحة ل)
        ل.ارسم_مستطيل(هذا.س - هذا.حجم / 2، 
                        هذا.ص - هذا.حجم / 2،
                        هذا.حجم، هذا.حجم،
                        هذا.لون، صحيح)
    نهاية
    
    دالة تحرك(رقم دس، رقم دص)
        هذا.س = هذا.س + دس
        هذا.ص = هذا.ص + دص
        
        // حدود الشاشة / Screen bounds
        إذا (هذا.س < 0)
            هذا.س = 0
        نهاية
        إذا (هذا.س > عرض_نافذة)
            هذا.س = عرض_نافذة
        نهاية
        إذا (هذا.ص < 0)
            هذا.ص = 0
        نهاية
        إذا (هذا.ص > ارتفاع_نافذة)
            هذا.ص = ارتفاع_نافذة
        نهاية
    نهاية
نهاية

// الكرة / Ball
صنف كرة
    رقم س
    رقم ص
    رقم سرعة_ص = 2
    رقم نصف_قطر = 15
    لون لون = لون.أحمر()
    
    باني()
        هذا.س = عشوائي.رقم(0، عرض_نافذة)
        هذا.ص = 0
    نهاية
    
    دالة حدث()
        هذا.ص = هذا.ص + هذا.سرعة_ص
    نهاية
    
    دالة ارسم(لوحة ل)
        ل.ارسم_دائرة(هذا.س، هذا.ص، هذا.نصف_قطر، هذا.لون، صحيح)
    نهاية
    
    دالة منطقي خارج_الشاشة()
        إرجاع هذا.ص > ارتفاع_نافذة + هذا.نصف_قطر
    نهاية
    
    دالة منطقي يتصادم_مع(لاعب ل)
        رقم مسافة_س = هذا.س - ل.س
        رقم مسافة_ص = هذا.ص - ل.ص
        رقم مسافة = (مسافة_س ** 2 + مسافة_ص ** 2) ** 0.5
        إرجاع مسافة < (هذا.نصف_قطر + ل.حجم / 2)
    نهاية
نهاية

// إعداد اللعبة / Game setup
لاعب ل = لاعب(عرض_نافذة / 2، ارتفاع_نافذة - 50)
مصفوفة كرات = []
رقم نقاط = 0
ساعة ساعة_توليد = ساعة()

// حلقة اللعبة الرئيسية / Main game loop
بينما ن.مفتوحة()
    // أحداث / Events
    لكل حدث في ن.احصل_أحداث()
        إذا (حدث.نوع == حدث.مغلق)
            ن.أغلق()
        نهاية
        
        // تحكم بالمفاتيح / Keyboard control
        إذا (حدث.نوع == حدث.مفتاح_مضغوط)
            إذا (حدث.مفتاح.كود == مفتاح.يسار)
                ل.تحرك(-20، 0)
            وإلا_إذا (حدث.مفتاح.كود == مفتاح.يمين)
                ل.تحرك(20، 0)
            نهاية
        نهاية
    نهاية
    
    // توليد كرات جديدة / Spawn new balls
    إذا (ساعة_توليد.انقضى() > 1.0)
        كرات.أضف(كرة())
        ساعة_توليد.إعادة_تشغيل()
    نهاية
    
    // تحديث الكرات / Update balls
    لكل ك في كرات
        ك.حدث()
        
        // فحص التصادم / Check collision
        إذا (ك.يتصادم_مع(ل))
            نقاط = نقاط + 1
            كرات.احذف(ك)
        وإلا_إذا (ك.خارج_الشاشة())
            كرات.احذف(ك)
        نهاية
    نهاية
    
    // رسم / Draw
    ن.امسح(لون.أبيض())
    لوحة لوحة = ن.احصل_لوحة()
    
    // رسم اللاعب / Draw player
    ل.ارسم(لوحة)
    
    // رسم الكرات / Draw balls
    لكل ك في كرات
        ك.ارسم(لوحة)
    نهاية
    
    // رسم النقاط / Draw score
    لوحة.ارسم_نص(ن"النقاط: {نقاط}"، 10، 10، لون.أسود()، 24)
    
    ن.اعرض()
نهاية

اطبع(ن"اللعبة انتهت! النقاط النهائية: {نقاط}")
```

---

## 🔧 Implementation Files

### Headers to Create

1. **`include/stdlib/graphics/window.h`** - Window management
2. **`include/stdlib/graphics/canvas.h`** - Drawing API
3. **`include/stdlib/graphics/color.h`** - Color system
4. **`include/stdlib/graphics/event.h`** - Event handling
5. **`include/stdlib/graphics/image.h`** - Image loading/manipulation
6. **`include/stdlib/graphics/sprite.h`** - Sprite and animation
7. **`include/stdlib/graphics/backend.h`** - Backend abstraction
8. **`include/stdlib/graphics/geometry.h`** - Point, Rect, etc.

### Source Files

1. **`src/stdlib/graphics/window.cpp`**
2. **`src/stdlib/graphics/canvas.cpp`**
3. **`src/stdlib/graphics/backends/sdl2_backend.cpp`**
4. **`src/stdlib/graphics/backends/win32_backend.cpp`**
5. **`src/stdlib/graphics/backends/d2d_backend.cpp`**

### CMake Configuration

```cmake
# Find graphics libraries
if(WIN32)
    # Windows: Use Win32 API by default, D2D optional
    set(GRAPHICS_BACKEND "Win32" CACHE STRING "Graphics backend")
    option(USE_DIRECT2D "Use Direct2D for graphics" OFF)
else()
    # Unix/Linux: Use SDL2
    find_package(SDL2 REQUIRED)
    set(GRAPHICS_BACKEND "SDL2")
endif()

# Graphics library sources
set(GRAPHICS_SOURCES
    src/stdlib/graphics/window.cpp
    src/stdlib/graphics/canvas.cpp
    src/stdlib/graphics/color.cpp
    src/stdlib/graphics/event.cpp
    src/stdlib/graphics/image.cpp
    src/stdlib/graphics/sprite.cpp
)

# Backend-specific sources
if(GRAPHICS_BACKEND STREQUAL "SDL2")
    list(APPEND GRAPHICS_SOURCES
        src/stdlib/graphics/backends/sdl2_backend.cpp
    )
    target_link_libraries(sad_interpreter SDL2::SDL2)
elseif(GRAPHICS_BACKEND STREQUAL "Win32")
    list(APPEND GRAPHICS_SOURCES
        src/stdlib/graphics/backends/win32_backend.cpp
    )
    if(USE_DIRECT2D)
        list(APPEND GRAPHICS_SOURCES
            src/stdlib/graphics/backends/d2d_backend.cpp
        )
        target_link_libraries(sad_interpreter d2d1 dwrite)
    endif()
endif()
```

---

## 🧪 Tests

### File: `tests/spec_rules/graphics_hello.s`

```s
// اختبار مكتبة الرسوم الأساسي / Basic graphics library test
استورد رسوم

// إنشاء نافذة / Create window
نافذة ن = نافذة(400، 300، "اختبار الرسوم")

رقم إطارات = 0

// حلقة رئيسية / Main loop
بينما ن.مفتوحة() && إطارات < 60
    ن.عالج_أحداث()
    ن.امسح(لون.أبيض())
    
    لوحة ل = ن.احصل_لوحة()
    
    // رسم دائرة / Draw circle
    ل.ارسم_دائرة(200، 150، 50، لون.أحمر()، صحيح)
    
    // رسم نص / Draw text
    ل.ارسم_نص("مرحباً!"، 150، 20، لون.أسود()، 20)
    
    ن.اعرض()
    إطارات = إطارات + 1
نهاية

ن.أغلق()
اطبع(ن"تم رسم {إطارات} إطار")
```

---

## 🔧 Git Workflow

```powershell
# إنشاء فرع / Create branch
git checkout -b feat/spec/graphics

# إضافة الملفات / Add files
git add docs/language_spec/rules/07_graphics.md
git add tests/spec_rules/graphics_hello.s
git add include/stdlib/graphics/*.h
git add src/stdlib/graphics/*.cpp

# Commit
git commit -m "[spec/graphics] Add graphics library specification

- Window management API
- Canvas drawing primitives
- Color system
- Event handling
- Image and sprite support
- Multiple backend support (SDL2, Win32, Direct2D)
- Complete game example
- Test file"

# Push
git push --set-upstream origin feat/spec/graphics
```

---

**آخر تحديث:** 10 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 30+
