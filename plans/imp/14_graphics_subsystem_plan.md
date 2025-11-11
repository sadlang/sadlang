# 🎨 خطة النظام الرسومي / Graphics Subsystem Implementation Plan

**التاريخ:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**المدة المقدرة:** 2 أسابيع

---

## 📋 ملخص / Summary

### (AR) الملخص
خطة تفصيلية لتطوير النظام الرسومي الأساسي للغة "ص". يشمل backend abstraction، event handling، basic widgets، وتكامل مع StdLib.

### (EN) Summary
Detailed plan for developing the basic Graphics Subsystem for Sad language. Includes backend abstraction, event handling, basic widgets, and StdLib integration.

---

## 🎯 الأهداف / Objectives

1. ✅ Graphics backend abstraction (SDL2/Win32/Console)
2. ✅ Window management
3. ✅ Basic drawing primitives (line, rect, circle)
4. ✅ Event system (mouse, keyboard, close)
5. ✅ Color management
6. ✅ Text rendering (basic)
7. ✅ تكامل مع StdLib
8. ⏳ Roadmap لـ advanced features (Phase 3)

---

## 📦 الاعتماديات / Dependencies

### من language_spec:
- ✅ `06_syntax_examples.md` - Graphics examples

### من plans:
- ✅ `09_integration_and_architecture.md` - Module interfaces
- ✅ `13_stdlib_and_modules_plan.md` - StdLib integration

### مكتبات خارجية:
- **SDL2** (Recommended): Cross-platform, mature
- **Win32 GDI** (Windows): Native, no dependencies
- **Console/ANSI** (Fallback): Text-only, minimal

---

## 🏗️ البنية المعمارية / Architecture

### Backend Abstraction

```cpp
// include/stdlib/graphics/graphics_backend.h
namespace Sad::StdLib::Graphics {

enum class BackendType {
    SDL2,
    WIN32_GDI,
    CONSOLE,
    HEADLESS  // للاختبار
};

// Color struct
struct Color {
    uint8_t r, g, b, a;
    
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}
    
    static Color Black()   { return {0, 0, 0}; }
    static Color White()   { return {255, 255, 255}; }
    static Color Red()     { return {255, 0, 0}; }
    static Color Green()   { return {0, 255, 0}; }
    static Color Blue()    { return {0, 0, 255}; }
    static Color Yellow()  { return {255, 255, 0}; }
};

// Point struct
struct Point {
    int x, y;
    Point(int x_, int y_) : x(x_), y(y_) {}
};

// Rectangle struct
struct Rect {
    int x, y, width, height;
    Rect(int x_, int y_, int w, int h) : x(x_), y(y_), width(w), height(h) {}
};

// Event types
enum class EventType {
    NONE,
    QUIT,
    KEY_DOWN,
    KEY_UP,
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_MOVE
};

struct Event {
    EventType type;
    int keyCode;     // للـ keyboard events
    Point mousePos;  // للـ mouse events
    int mouseButton; // للـ mouse events
    
    Event() : type(EventType::NONE), keyCode(0), mousePos(0, 0), mouseButton(0) {}
};

// Backend Interface
class IGraphicsBackend {
public:
    virtual ~IGraphicsBackend() = default;
    
    // Window management
    virtual bool createWindow(const std::string& title, int width, int height) = 0;
    virtual void destroyWindow() = 0;
    virtual void clear(const Color& color) = 0;
    virtual void present() = 0;
    
    // Drawing primitives
    virtual void drawLine(const Point& start, const Point& end, const Color& color) = 0;
    virtual void drawRect(const Rect& rect, const Color& color, bool filled = false) = 0;
    virtual void drawCircle(const Point& center, int radius, const Color& color, bool filled = false) = 0;
    virtual void drawText(const Point& pos, const std::string& text, const Color& color) = 0;
    
    // Event handling
    virtual bool pollEvent(Event& event) = 0;
    
    // Info
    virtual BackendType getType() const = 0;
};

} // namespace Sad::StdLib::Graphics
```

---

### SDL2 Backend

```cpp
// include/stdlib/graphics/backends/sdl2_backend.h
#ifdef GRAPHICS_SDL2

#include <SDL2/SDL.h>

namespace Sad::StdLib::Graphics {

class SDL2Backend : public IGraphicsBackend {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool initialized = false;
    
public:
    SDL2Backend() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            ERROR_PRINT("SDL2 initialization failed: %s", SDL_GetError());
            return;
        }
        initialized = true;
        DEBUG_PRINT("SDL2 backend initialized");
    }
    
    ~SDL2Backend() {
        destroyWindow();
        if (initialized) {
            SDL_Quit();
        }
    }
    
    bool createWindow(const std::string& title, int width, int height) override {
        window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_SHOWN
        );
        
        if (!window) {
            ERROR_PRINT("Window creation failed: %s", SDL_GetError());
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            ERROR_PRINT("Renderer creation failed: %s", SDL_GetError());
            SDL_DestroyWindow(window);
            window = nullptr;
            return false;
        }
        
        DEBUG_PRINT("SDL2 window created: %dx%d", width, height);
        return true;
    }
    
    void destroyWindow() override {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
    }
    
    void clear(const Color& color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(renderer);
    }
    
    void present() override {
        SDL_RenderPresent(renderer);
    }
    
    void drawLine(const Point& start, const Point& end, const Color& color) override {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
    }
    
    void drawRect(const Rect& rect, const Color& color, bool filled) override {
        SDL_Rect sdlRect = {rect.x, rect.y, rect.width, rect.height};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        
        if (filled) {
            SDL_RenderFillRect(renderer, &sdlRect);
        } else {
            SDL_RenderDrawRect(renderer, &sdlRect);
        }
    }
    
    void drawCircle(const Point& center, int radius, const Color& color, bool filled) override {
        // Midpoint circle algorithm
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        
        int x = 0;
        int y = radius;
        int d = 1 - radius;
        
        while (x <= y) {
            if (filled) {
                SDL_RenderDrawLine(renderer, center.x - x, center.y + y, center.x + x, center.y + y);
                SDL_RenderDrawLine(renderer, center.x - x, center.y - y, center.x + x, center.y - y);
                SDL_RenderDrawLine(renderer, center.x - y, center.y + x, center.x + y, center.y + x);
                SDL_RenderDrawLine(renderer, center.x - y, center.y - x, center.x + y, center.y - x);
            } else {
                SDL_RenderDrawPoint(renderer, center.x + x, center.y + y);
                SDL_RenderDrawPoint(renderer, center.x - x, center.y + y);
                SDL_RenderDrawPoint(renderer, center.x + x, center.y - y);
                SDL_RenderDrawPoint(renderer, center.x - x, center.y - y);
                SDL_RenderDrawPoint(renderer, center.x + y, center.y + x);
                SDL_RenderDrawPoint(renderer, center.x - y, center.y + x);
                SDL_RenderDrawPoint(renderer, center.x + y, center.y - x);
                SDL_RenderDrawPoint(renderer, center.x - y, center.y - x);
            }
            
            if (d < 0) {
                d += 2 * x + 3;
            } else {
                d += 2 * (x - y) + 5;
                y--;
            }
            x++;
        }
    }
    
    void drawText(const Point& pos, const std::string& text, const Color& color) override {
        // Requires SDL_ttf for proper text rendering
        // For now, just log a warning
        WARN_PRINT("Text rendering not yet implemented in SDL2 backend");
    }
    
    bool pollEvent(Event& event) override {
        SDL_Event sdlEvent;
        if (!SDL_PollEvent(&sdlEvent)) {
            event.type = EventType::NONE;
            return false;
        }
        
        switch (sdlEvent.type) {
            case SDL_QUIT:
                event.type = EventType::QUIT;
                break;
                
            case SDL_KEYDOWN:
                event.type = EventType::KEY_DOWN;
                event.keyCode = sdlEvent.key.keysym.sym;
                break;
                
            case SDL_KEYUP:
                event.type = EventType::KEY_UP;
                event.keyCode = sdlEvent.key.keysym.sym;
                break;
                
            case SDL_MOUSEBUTTONDOWN:
                event.type = EventType::MOUSE_DOWN;
                event.mousePos = Point(sdlEvent.button.x, sdlEvent.button.y);
                event.mouseButton = sdlEvent.button.button;
                break;
                
            case SDL_MOUSEBUTTONUP:
                event.type = EventType::MOUSE_UP;
                event.mousePos = Point(sdlEvent.button.x, sdlEvent.button.y);
                event.mouseButton = sdlEvent.button.button;
                break;
                
            case SDL_MOUSEMOTION:
                event.type = EventType::MOUSE_MOVE;
                event.mousePos = Point(sdlEvent.motion.x, sdlEvent.motion.y);
                break;
                
            default:
                event.type = EventType::NONE;
                return false;
        }
        
        return true;
    }
    
    BackendType getType() const override {
        return BackendType::SDL2;
    }
};

} // namespace Sad::StdLib::Graphics

#endif // GRAPHICS_SDL2
```

---

### Graphics Module (StdLib Interface)

```cpp
// include/stdlib/graphics/graphics_module.h
namespace Sad::StdLib::Graphics {

class GraphicsModule {
private:
    std::unique_ptr<IGraphicsBackend> backend;
    bool windowCreated = false;
    
public:
    GraphicsModule() {
        // Choose backend based on compile flags
        #ifdef GRAPHICS_SDL2
            backend = std::make_unique<SDL2Backend>();
            DEBUG_PRINT("Using SDL2 graphics backend");
        #elif defined(GRAPHICS_WIN32)
            backend = std::make_unique<Win32Backend>();
            DEBUG_PRINT("Using Win32 GDI graphics backend");
        #else
            backend = std::make_unique<ConsoleBackend>();
            DEBUG_PRINT("Using Console graphics backend (fallback)");
        #endif
    }
    
    // StdLib function wrappers
    static Value createWindow(const std::vector<Value>& args) {
        if (args.size() < 3) {
            throw RuntimeError("create_window requires title, width, height");
        }
        
        std::string title = args[0].asString();
        int width = static_cast<int>(args[1].asInteger());
        int height = static_cast<int>(args[2].asInteger());
        
        bool success = getInstance().backend->createWindow(title, width, height);
        getInstance().windowCreated = success;
        
        return Value(success);
    }
    
    static Value clear(const std::vector<Value>& args) {
        Color color = args.empty() ? Color::Black() : 
                      Color(
                          static_cast<uint8_t>(args[0].asInteger()),
                          static_cast<uint8_t>(args[1].asInteger()),
                          static_cast<uint8_t>(args[2].asInteger())
                      );
        
        getInstance().backend->clear(color);
        return Value();
    }
    
    static Value present(const std::vector<Value>&) {
        getInstance().backend->present();
        return Value();
    }
    
    static Value drawLine(const std::vector<Value>& args) {
        if (args.size() < 7) {
            throw RuntimeError("draw_line requires x1, y1, x2, y2, r, g, b");
        }
        
        Point start(static_cast<int>(args[0].asInteger()), 
                   static_cast<int>(args[1].asInteger()));
        Point end(static_cast<int>(args[2].asInteger()), 
                 static_cast<int>(args[3].asInteger()));
        Color color(static_cast<uint8_t>(args[4].asInteger()),
                   static_cast<uint8_t>(args[5].asInteger()),
                   static_cast<uint8_t>(args[6].asInteger()));
        
        getInstance().backend->drawLine(start, end, color);
        return Value();
    }
    
    static Value drawRect(const std::vector<Value>& args) {
        if (args.size() < 7) {
            throw RuntimeError("draw_rect requires x, y, w, h, r, g, b");
        }
        
        Rect rect(static_cast<int>(args[0].asInteger()),
                 static_cast<int>(args[1].asInteger()),
                 static_cast<int>(args[2].asInteger()),
                 static_cast<int>(args[3].asInteger()));
        Color color(static_cast<uint8_t>(args[4].asInteger()),
                   static_cast<uint8_t>(args[5].asInteger()),
                   static_cast<uint8_t>(args[6].asInteger()));
        bool filled = args.size() > 7 ? args[7].asBool() : false;
        
        getInstance().backend->drawRect(rect, color, filled);
        return Value();
    }
    
    static Value drawCircle(const std::vector<Value>& args) {
        if (args.size() < 6) {
            throw RuntimeError("draw_circle requires x, y, radius, r, g, b");
        }
        
        Point center(static_cast<int>(args[0].asInteger()),
                    static_cast<int>(args[1].asInteger()));
        int radius = static_cast<int>(args[2].asInteger());
        Color color(static_cast<uint8_t>(args[3].asInteger()),
                   static_cast<uint8_t>(args[4].asInteger()),
                   static_cast<uint8_t>(args[5].asInteger()));
        bool filled = args.size() > 6 ? args[6].asBool() : false;
        
        getInstance().backend->drawCircle(center, radius, color, filled);
        return Value();
    }
    
    static Value pollEvent(const std::vector<Value>&) {
        Event event;
        bool hasEvent = getInstance().backend->pollEvent(event);
        
        if (!hasEvent) {
            return Value(); // None
        }
        
        // Return event as map
        std::map<std::string, Value> eventMap;
        eventMap["type"] = Value(static_cast<int64_t>(event.type));
        eventMap["keyCode"] = Value(static_cast<int64_t>(event.keyCode));
        eventMap["mouseX"] = Value(static_cast<int64_t>(event.mousePos.x));
        eventMap["mouseY"] = Value(static_cast<int64_t>(event.mousePos.y));
        eventMap["mouseButton"] = Value(static_cast<int64_t>(event.mouseButton));
        
        return Value::makeMap(eventMap);
    }
    
private:
    static GraphicsModule& getInstance() {
        static GraphicsModule instance;
        return instance;
    }
};

} // namespace Sad::StdLib::Graphics
```

---

### StdLib Integration

```cpp
// src/stdlib/graphics/graphics_module.cpp
void StdLibManager::initGraphicsModule() {
    using namespace Graphics;
    
    registerFunction("graphics", "إنشئ_نافذة", GraphicsModule::createWindow);
    registerFunction("graphics", "create_window", GraphicsModule::createWindow);
    
    registerFunction("graphics", "نظف", GraphicsModule::clear);
    registerFunction("graphics", "clear", GraphicsModule::clear);
    
    registerFunction("graphics", "إعرض", GraphicsModule::present);
    registerFunction("graphics", "present", GraphicsModule::present);
    
    registerFunction("graphics", "إرسم_خط", GraphicsModule::drawLine);
    registerFunction("graphics", "draw_line", GraphicsModule::drawLine);
    
    registerFunction("graphics", "إرسم_مستطيل", GraphicsModule::drawRect);
    registerFunction("graphics", "draw_rect", GraphicsModule::drawRect);
    
    registerFunction("graphics", "إرسم_دائرة", GraphicsModule::drawCircle);
    registerFunction("graphics", "draw_circle", GraphicsModule::drawCircle);
    
    registerFunction("graphics", "إستقبل_حدث", GraphicsModule::pollEvent);
    registerFunction("graphics", "poll_event", GraphicsModule::pollEvent);
    
    DEBUG_PRINT("Graphics module initialized with 14 functions");
}
```

---

## 📁 الملفات المطلوبة / Required Files

```
include/stdlib/graphics/
├── graphics_backend.h           🆕 NEW (Interface + structs)
├── graphics_module.h            🆕 NEW (StdLib wrapper)
├── backends/
│   ├── sdl2_backend.h          🆕 NEW
│   ├── win32_backend.h         🆕 NEW (optional)
│   └── console_backend.h       🆕 NEW (fallback)

src/stdlib/graphics/
├── graphics_module.cpp          🆕 NEW
├── backends/
│   ├── sdl2_backend.cpp        🆕 NEW
│   ├── win32_backend.cpp       🆕 NEW
│   └── console_backend.cpp     🆕 NEW

tests/stdlib_tests/graphics/
├── backend_test.cpp             🆕 NEW (10 tests)
├── drawing_test.cpp             🆕 NEW (12 tests)
├── events_test.cpp              🆕 NEW (8 tests)
└── integration_test.cpp         🆕 NEW (5 tests)
```

---

## 🔧 تقسيم العمل / Task Breakdown

### Week 1: Backend Implementation

#### Task 1.1: Graphics Backend Interface (يوم 1-2)
**الملفات:**
- `include/stdlib/graphics/graphics_backend.h`

**الخطوات:**
1. Define IGraphicsBackend interface
2. Define Color, Point, Rect, Event structs
3. Define BackendType enum

**الاختبارات:**
```cpp
TEST(GraphicsBackendTest, ColorCreation) {
    Color red = Color::Red();
    EXPECT_EQ(red.r, 255);
    EXPECT_EQ(red.g, 0);
    EXPECT_EQ(red.b, 0);
}

TEST(GraphicsBackendTest, PointCreation) {
    Point p(10, 20);
    EXPECT_EQ(p.x, 10);
    EXPECT_EQ(p.y, 20);
}
```

**معايير القبول:**
- ✅ Interface compiles
- ✅ Structs defined
- ✅ Tests pass (≥5 tests)

---

#### Task 1.2: SDL2 Backend (يوم 3-7)
**الملفات:**
- `include/stdlib/graphics/backends/sdl2_backend.h`
- `src/stdlib/graphics/backends/sdl2_backend.cpp`

**الخطوات:**
1. Implement createWindow/destroyWindow
2. Implement clear/present
3. Implement drawLine/drawRect/drawCircle
4. Implement pollEvent

**الاختبارات:**
```cpp
TEST(SDL2BackendTest, WindowCreation) {
    SDL2Backend backend;
    bool success = backend.createWindow("Test", 800, 600);
    
    EXPECT_TRUE(success);
    backend.destroyWindow();
}

TEST(SDL2BackendTest, Drawing) {
    SDL2Backend backend;
    backend.createWindow("Test", 800, 600);
    
    backend.clear(Color::Black());
    backend.drawLine(Point(0, 0), Point(100, 100), Color::Red());
    backend.drawRect(Rect(50, 50, 100, 100), Color::Green(), true);
    backend.present();
    
    // No crash = success
    backend.destroyWindow();
}
```

**معايير القبول:**
- ✅ Window creation works
- ✅ All drawing primitives work
- ✅ Event polling works
- ✅ Tests pass (≥15 tests)

---

### Week 2: Module Integration & Testing

#### Task 2.1: GraphicsModule Wrapper (يوم 8-10)
**الملفات:**
- `include/stdlib/graphics/graphics_module.h`
- `src/stdlib/graphics/graphics_module.cpp`

**الخطوات:**
1. Wrap backend in singleton
2. Create StdLib function wrappers
3. Register with StdLibManager

**الاختبارات:**
```cpp
TEST(GraphicsModuleTest, CreateWindow) {
    std::vector<Value> args = {
        Value("Test Window"),
        Value(int64_t(800)),
        Value(int64_t(600))
    };
    
    Value result = GraphicsModule::createWindow(args);
    EXPECT_TRUE(result.asBool());
}

TEST(GraphicsModuleTest, DrawingOperations) {
    GraphicsModule::createWindow({Value("Test"), Value(800), Value(600)});
    
    GraphicsModule::clear({Value(0), Value(0), Value(0)});
    GraphicsModule::drawLine({Value(0), Value(0), Value(100), Value(100), 
                             Value(255), Value(0), Value(0)});
    GraphicsModule::present({});
    
    // No crash = success
}
```

---

#### Task 2.2: Integration Test (يوم 11-12)
```cpp
TEST(GraphicsIntegration, CompleteProgram) {
    std::string code = R"(
        استخدم graphics
        
        graphics.إنشئ_نافذة("مثال رسومي"، 800، 600)
        
        منطقي يعمل = صحيح
        بينما يعمل {
            graphics.نظف(0، 0، 0)
            
            graphics.إرسم_مستطيل(100، 100، 200، 150، 255، 0، 0، صحيح)
            graphics.إرسم_دائرة(400، 300، 50، 0، 255، 0، صحيح)
            graphics.إرسم_خط(0، 0، 800، 600، 255، 255، 255)
            
            graphics.إعرض()
            
            خريطة حدث = graphics.إستقبل_حدث()
            إذا حدث != لاشيء && حدث["type"] == 0 {
                يعمل = خطأ
            }
        }
    )";
    
    auto result = executeCode(code);
    EXPECT_TRUE(result.success);
}
```

---

## 🧪 الاختبارات / Tests Summary

### Unit Tests (35+ tests)
- Backend interface (5 tests)
- SDL2 backend (15 tests)
- Module wrapper (10 tests)
- Integration (5 tests)

---

## ⏱️ الجدول الزمني / Timeline

| الأسبوع | المهام | الساعات |
|---------|--------|---------|
| Week 1 | Backend Interface + SDL2 | 40 ساعة |
| Week 2 | Module Wrapper + Integration | 40 ساعة |
| **Total** | **2 weeks** | **80 ساعة** |

---

## 🚀 أوامر PowerShell / PowerShell Commands

```powershell
# تثبيت SDL2 (vcpkg)
vcpkg install sdl2:x64-windows

# إنشاء فرع العمل
git checkout -b feat/graphics/basic-subsystem

# بناء مع Graphics support
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DGRAPHICS_SDL2=ON -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build . --config Debug

# اختبار
ctest -R graphics --output-on-failure -C Debug

# Commit
git add include/stdlib/graphics/* src/stdlib/graphics/* tests/stdlib_tests/graphics/*
git commit -m "[graphics] Implemented basic graphics subsystem

- Graphics backend abstraction (Interface)
- SDL2 backend with window management
- Drawing primitives: line, rect, circle
- Event system (keyboard, mouse, quit)
- Color management
- StdLib integration
- 35+ unit tests with ≥80% coverage"

git push --set-upstream origin feat/graphics/basic-subsystem
```

---

## ⚠️ المخاطر / Risks

| الخطر | التخفيف |
|-------|---------|
| SDL2 dependency | Win32/Console fallback |
| Text rendering complex | Defer to Phase 3 |
| Cross-platform issues | CI testing on multiple OS |

---

## 🔮 Roadmap (Phase 3)

- **Advanced Text**: SDL_ttf integration
- **Image Loading**: SDL_image for PNG/JPG
- **Sprites**: Texture management
- **UI Widgets**: Button, TextBox, etc.
- **Animation**: Frame-based system

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ
