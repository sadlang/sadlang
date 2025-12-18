# 📝 المرحلة 9: مكتبة الرسوم
## Phase 9: Graphics Library

**المدة:** أسبوعان (10 أيام)  
**الأولوية:** 🟢 P3 - عادية  
**التبعيات:** المرحلة 1-8

---

## 🎯 الأهداف

1. ✅ Window Management
2. ✅ Canvas & Drawing
3. ✅ Event System
4. ✅ Sprites & Animation
5. ✅ UI Widgets
6. ✅ SDL2 Backend

---

## 📋 المهام

### المهمة 9.1: Window Management (يوم)

```s
استورد رسوم

// Create window
var window = رسوم.Window("My App", 800, 600)
window.show()

// Window properties
window.set_title("New Title")
window.set_size(1024, 768)
window.set_position(100, 100)
window.maximize()
window.minimize()

// Main loop
while (window.is_open()) {
    window.poll_events()
    window.clear()
    // Draw here
    window.present()
}
```

**Implementation:**
```cpp
// SDL2 backend
class Window {
    SDL_Window* sdl_window;
    SDL_Renderer* renderer;
    
public:
    Window(string title, int w, int h);
    void show();
    void clear(Color color);
    void present();
    bool is_open();
};
```

### المهمة 9.2: Canvas & Drawing (2 أيام)

```s
استورد رسوم

var canvas = رسوم.Canvas(800, 600)

// Basic shapes
canvas.draw_line(0, 0, 100, 100, رسوم.RED)
canvas.draw_rect(50, 50, 200, 100, رسوم.BLUE)
canvas.fill_rect(300, 50, 200, 100, رسوم.GREEN)
canvas.draw_circle(400, 300, 50, رسوم.YELLOW)
canvas.fill_circle(500, 300, 50, رسوم.PURPLE)

// Polygons
var points = [[100, 100], [150, 50], [200, 100], [150, 150]]
canvas.draw_polygon(points, رسوم.WHITE)
canvas.fill_polygon(points, رسوم.CYAN)

// Text
canvas.draw_text("Hello World", 100, 500, رسوم.Font("Arial", 24))
canvas.draw_text_arabic("مرحبا بالعالم", 100, 550, رسوم.Font("Arial", 24))

// Images
var image = رسوم.load_image("sprite.png")
canvas.draw_image(image, 600, 100)

// Transformations
canvas.push_matrix()
canvas.translate(400, 300)
canvas.rotate(45)
canvas.scale(2, 2)
canvas.draw_rect(-25, -25, 50, 50, رسوم.RED)
canvas.pop_matrix()
```

**Implementation:**
```cpp
class Canvas {
    SDL_Renderer* renderer;
    std::stack<Transform> transforms;
    
public:
    void draw_line(int x1, int y1, int x2, int y2, Color color);
    void draw_rect(int x, int y, int w, int h, Color color);
    void fill_rect(int x, int y, int w, int h, Color color);
    void draw_circle(int x, int y, int r, Color color);
    void draw_image(Image* img, int x, int y);
    void draw_text(string text, int x, int y, Font* font);
};
```

### المهمة 9.3: Event System (يوم)

```s
استورد رسوم

var window = رسوم.Window("Events", 800, 600)

// Mouse events
window.on_mouse_down(function(x, y, button) {
    print("Mouse down at: " + x + ", " + y)
})

window.on_mouse_up(function(x, y, button) {
    print("Mouse up")
})

window.on_mouse_move(function(x, y) {
    // Track mouse position
})

// Keyboard events
window.on_key_down(function(key) {
    if (key == رسوم.KEY_ESCAPE) {
        window.close()
    }
})

window.on_key_up(function(key) {
    print("Key released: " + key)
})

// Window events
window.on_resize(function(width, height) {
    print("Window resized: " + width + "x" + height)
})

window.on_close(function() {
    print("Window closing")
    return true  // Allow close
})
```

**Implementation:**
```cpp
class EventSystem {
    std::unordered_map<EventType, std::vector<Callback>> handlers;
    
public:
    void register_handler(EventType type, Callback cb);
    void dispatch_event(Event* event);
    void poll_events();
};

enum class EventType {
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_MOVE,
    KEY_DOWN,
    KEY_UP,
    WINDOW_RESIZE,
    WINDOW_CLOSE
};
```

### المهمة 9.4: Sprites & Animation (2 أيام)

```s
استورد رسوم

// Sprite
class Player {
    var sprite: رسوم.Sprite
    var x: float = 0
    var y: float = 0
    var velocity_x: float = 0
    var velocity_y: float = 0
    
    constructor() {
        this.sprite = رسوم.Sprite("player.png")
        this.sprite.set_scale(2, 2)
    }
    
    function update(delta_time: float) {
        this.x += this.velocity_x * delta_time
        this.y += this.velocity_y * delta_time
        
        this.sprite.set_position(this.x, this.y)
    }
    
    function draw(canvas) {
        this.sprite.draw(canvas)
    }
}

// Sprite sheet animation
var anim = رسوم.Animation("spritesheet.png", 32, 32)
anim.add_frame(0, 0)  // Frame at (0, 0)
anim.add_frame(32, 0) // Frame at (32, 0)
anim.add_frame(64, 0) // Frame at (64, 0)
anim.set_frame_rate(10)  // 10 FPS

// In game loop
anim.update(delta_time)
anim.draw(canvas, x, y)
```

**Implementation:**
```cpp
class Sprite {
    SDL_Texture* texture;
    float x, y;
    float scale_x, scale_y;
    float rotation;
    
public:
    Sprite(string filename);
    void set_position(float x, float y);
    void set_scale(float sx, float sy);
    void set_rotation(float angle);
    void draw(Canvas* canvas);
};

class Animation {
    std::vector<SDL_Rect> frames;
    int current_frame;
    float frame_time;
    float elapsed;
    
public:
    void add_frame(int x, int y, int w, int h);
    void update(float delta_time);
    void draw(Canvas* canvas, int x, int y);
};
```

### المهمة 9.5: UI Widgets (4 أيام)

```s
استورد رسوم.ui

// Button
var button = ui.Button("Click Me", 100, 100, 150, 40)
button.on_click(function() {
    print("Button clicked!")
})

button.set_background_color(رسوم.BLUE)
button.set_text_color(رسوم.WHITE)
button.set_font(رسوم.Font("Arial", 18))

// Text input
var input = ui.TextInput(100, 150, 250, 30)
input.set_placeholder("Enter name...")
input.on_change(function(text) {
    print("Text: " + text)
})

// Label
var label = ui.Label("Status: Ready", 100, 200)
label.set_font(رسوم.Font("Arial", 14))

// Checkbox
var checkbox = ui.Checkbox("Enable feature", 100, 250)
checkbox.on_change(function(checked) {
    print("Checked: " + checked)
})

// Slider
var slider = ui.Slider(100, 300, 200, 0, 100)
slider.set_value(50)
slider.on_change(function(value) {
    print("Value: " + value)
})

// List
var list = ui.List(100, 350, 200, 150)
list.add_item("Item 1")
list.add_item("Item 2")
list.add_item("Item 3")
list.on_select(function(index, item) {
    print("Selected: " + item)
})

// Panel
var panel = ui.Panel(400, 100, 350, 400)
panel.set_background_color(رسوم.Color(240, 240, 240))

// Add widgets to panel
var button2 = ui.Button("Inside Panel", 20, 20, 100, 30)
panel.add(button2)

// Main UI loop
while (window.is_open()) {
    window.poll_events()
    window.clear(رسوم.WHITE)
    
    // Update and draw widgets
    button.update()
    button.draw(canvas)
    
    input.update()
    input.draw(canvas)
    
    panel.update()
    panel.draw(canvas)
    
    window.present()
}
```

**Implementation:**
```cpp
namespace UI {

class Widget {
protected:
    int x, y, width, height;
    bool is_hovered;
    bool is_pressed;
    
public:
    virtual void update() = 0;
    virtual void draw(Canvas* canvas) = 0;
    virtual void handle_event(Event* event) = 0;
};

class Button : public Widget {
    string text;
    Color bg_color;
    Color text_color;
    Font* font;
    Callback on_click;
    
public:
    Button(string text, int x, int y, int w, int h);
    void update() override;
    void draw(Canvas* canvas) override;
    void handle_event(Event* event) override;
};

class TextInput : public Widget {
    string text;
    string placeholder;
    bool is_focused;
    Callback on_change;
    
public:
    void update() override;
    void draw(Canvas* canvas) override;
    void handle_event(Event* event) override;
};

} // namespace UI
```

---

## 📅 الجدول الزمني

| اليوم | المهمة | الساعات |
|------|---------|----------|
| 1 | Window management | 8 |
| 2-3 | Canvas & drawing | 16 |
| 4 | Event system | 8 |
| 5-6 | Sprites & animation | 16 |
| 7-10 | UI widgets | 32 |

**المجموع:** 80 ساعة

---

## 🎯 النتائج المتوقعة

- ✅ مكتبة رسوم كاملة
- ✅ SDL2 backend
- ✅ 10+ widgets جاهزة
- ✅ نظام events متقدم
- ✅ دعم الرسوم المتحركة
- ✅ أمثلة ألعاب بسيطة

---

**الحالة:** 📝 جاهز للتنفيذ
