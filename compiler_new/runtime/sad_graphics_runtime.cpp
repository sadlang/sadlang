/**
 * =============================================================================
 * ملف: sad_graphics_runtime.cpp
 * الوصف: مكتبة Runtime للرسومات — تُربط مع البرامج المترجمة
 * =============================================================================
 * 
 * 🎨 مكتبة Runtime للرسومات
 * ═════════════════════════════════════════
 * 
 * هذا الملف يوفر دوال Runtime التي يستدعيها البرنامج المترجم.
 * يتم ربط هذه المكتبة مع الملف التنفيذي النهائي.
 * 
 * البناء:
 *   gcc -c sad_graphics_runtime.cpp -o sad_graphics_runtime.o
 *   ar rcs libsad_graphics.a sad_graphics_runtime.o
 * 
 * =============================================================================
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstring>
#include <algorithm>

// =============================================================================
// الأنواع الأساسية
// =============================================================================

extern "C" {

/**
 * @brief نوع اللون
 */
struct SadColor {
    uint8_t r, g, b, a;
};

/**
 * @brief نوع الويدجت
 */
struct SadWidget {
    int id;
    int type;
    float x, y, width, height;
    SadColor bgColor;
    SadColor fgColor;
    float opacity;
    bool visible;
    bool enabled;
    float borderRadius;
    float borderWidth;
    SadColor borderColor;
    int parentId;
    char* text;
    float fontSize;
    int zIndex;
    // المزيد حسب الحاجة
};

// =============================================================================
// المتغيرات العامة
// =============================================================================

static SDL_Window* g_window = nullptr;
static SDL_Renderer* g_renderer = nullptr;
static TTF_Font* g_font = nullptr;
static std::unordered_map<int, SadWidget*> g_widgets;
static int g_nextWidgetId = 1;
static bool g_running = true;

// =============================================================================
// دوال التهيئة والإغلاق
// =============================================================================

/**
 * @brief تهيئة نظام الرسومات
 */
int sad_ui_init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        SDL_Log("خطأ في تهيئة SDL: %s", SDL_GetError());
        return -1;
    }
    
    if (TTF_Init() < 0) {
        SDL_Log("خطأ في تهيئة TTF: %s", TTF_GetError());
        return -1;
    }
    
    return 0;
}

/**
 * @brief إغلاق نظام الرسومات
 */
void sad_ui_quit() {
    // تنظيف الويدجات
    for (auto& pair : g_widgets) {
        if (pair.second->text) {
            free(pair.second->text);
        }
        delete pair.second;
    }
    g_widgets.clear();
    
    // تنظيف الخط
    if (g_font) {
        TTF_CloseFont(g_font);
        g_font = nullptr;
    }
    
    // تنظيف SDL
    if (g_renderer) {
        SDL_DestroyRenderer(g_renderer);
        g_renderer = nullptr;
    }
    
    if (g_window) {
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }
    
    TTF_Quit();
    SDL_Quit();
}

// =============================================================================
// إدارة النوافذ
// =============================================================================

/**
 * @brief إنشاء نافذة
 */
void* sad_ui_create_window(const char* title, int width, int height) {
    g_window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (!g_window) {
        SDL_Log("خطأ في إنشاء النافذة: %s", SDL_GetError());
        return nullptr;
    }
    
    g_renderer = SDL_CreateRenderer(
        g_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    
    if (!g_renderer) {
        SDL_Log("خطأ في إنشاء الـ renderer: %s", SDL_GetError());
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
        return nullptr;
    }
    
    return g_window;
}

/**
 * @brief تدمير نافذة
 */
void sad_ui_destroy_window(void* window) {
    if (window == g_window) {
        if (g_renderer) {
            SDL_DestroyRenderer(g_renderer);
            g_renderer = nullptr;
        }
        SDL_DestroyWindow(g_window);
        g_window = nullptr;
    }
}

/**
 * @brief تحديث النافذة
 */
void sad_ui_update_window(void* window) {
    if (g_renderer) {
        SDL_RenderPresent(g_renderer);
    }
}

/**
 * @brief تعيين عنوان النافذة
 */
void sad_ui_set_window_title(void* window, const char* title) {
    if (window) {
        SDL_SetWindowTitle((SDL_Window*)window, title);
    }
}

/**
 * @brief تعيين حجم النافذة
 */
void sad_ui_set_window_size(void* window, int width, int height) {
    if (window) {
        SDL_SetWindowSize((SDL_Window*)window, width, height);
    }
}

// =============================================================================
// إدارة الويدجات
// =============================================================================

/**
 * @brief إنشاء ويدجت
 */
int sad_ui_create_widget(int type) {
    SadWidget* widget = new SadWidget();
    widget->id = g_nextWidgetId++;
    widget->type = type;
    widget->x = 0;
    widget->y = 0;
    widget->width = 100;
    widget->height = 50;
    widget->bgColor = {255, 255, 255, 255};
    widget->fgColor = {0, 0, 0, 255};
    widget->opacity = 1.0f;
    widget->visible = true;
    widget->enabled = true;
    widget->borderRadius = 0;
    widget->borderWidth = 0;
    widget->borderColor = {0, 0, 0, 255};
    widget->parentId = -1;
    widget->text = nullptr;
    widget->fontSize = 16;
    widget->zIndex = 0;
    
    g_widgets[widget->id] = widget;
    return widget->id;
}

/**
 * @brief تدمير ويدجت
 */
void sad_ui_destroy_widget(int id) {
    auto it = g_widgets.find(id);
    if (it != g_widgets.end()) {
        if (it->second->text) {
            free(it->second->text);
        }
        delete it->second;
        g_widgets.erase(it);
    }
}

/**
 * @brief تعيين موقع الويدجت
 */
void sad_ui_set_position(int id, float x, float y) {
    auto it = g_widgets.find(id);
    if (it != g_widgets.end()) {
        it->second->x = x;
        it->second->y = y;
    }
}

/**
 * @brief تعيين حجم الويدجت
 */
void sad_ui_set_size(int id, float width, float height) {
    auto it = g_widgets.find(id);
    if (it != g_widgets.end()) {
        it->second->width = width;
        it->second->height = height;
    }
}

/**
 * @brief تعيين نص الويدجت
 */
void sad_ui_set_text(int id, const char* text) {
    auto it = g_widgets.find(id);
    if (it != g_widgets.end()) {
        if (it->second->text) {
            free(it->second->text);
        }
        it->second->text = strdup(text);
    }
}

/**
 * @brief تعيين لون الخلفية
 */
void sad_ui_set_bg_color(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    auto it = g_widgets.find(id);
    if (it != g_widgets.end()) {
        it->second->bgColor = {r, g, b, a};
    }
}

/**
 * @brief تعيين لون النص
 */
void sad_ui_set_fg_color(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    auto it = g_widgets.find(id);
    if (it != g_widgets.end()) {
        it->second->fgColor = {r, g, b, a};
    }
}

/**
 * @brief تعيين رؤية الويدجت
 */
void sad_ui_set_visible(int id, bool visible) {
    auto it = g_widgets.find(id);
    if (it != g_widgets.end()) {
        it->second->visible = visible;
    }
}

/**
 * @brief تعيين تفعيل الويدجت
 */
void sad_ui_set_enabled(int id, bool enabled) {
    auto it = g_widgets.find(id);
    if (it != g_widgets.end()) {
        it->second->enabled = enabled;
    }
}

/**
 * @brief إضافة ابن
 */
void sad_ui_add_child(int parentId, int childId) {
    auto it = g_widgets.find(childId);
    if (it != g_widgets.end()) {
        it->second->parentId = parentId;
    }
}

// =============================================================================
// الرسم
// =============================================================================

/**
 * @brief رسم ويدجت واحد
 */
void sad_ui_render_widget(void* renderer, int id) {
    auto it = g_widgets.find(id);
    if (it == g_widgets.end() || !it->second->visible) {
        return;
    }
    
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    SadWidget* w = it->second;
    
    // رسم الخلفية
    SDL_Rect rect = {
        (int)w->x,
        (int)w->y,
        (int)w->width,
        (int)w->height
    };
    
    SDL_SetRenderDrawColor(r, w->bgColor.r, w->bgColor.g, 
                            w->bgColor.b, w->bgColor.a);
    SDL_RenderFillRect(r, &rect);
    
    // رسم الحدود
    if (w->borderWidth > 0) {
        SDL_SetRenderDrawColor(r, w->borderColor.r, w->borderColor.g,
                                w->borderColor.b, w->borderColor.a);
        SDL_RenderDrawRect(r, &rect);
    }
    
    // رسم النص إذا كان موجوداً
    if (w->text && g_font) {
        SDL_Color color = {w->fgColor.r, w->fgColor.g, 
                           w->fgColor.b, w->fgColor.a};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(g_font, w->text, color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(r, surface);
            if (texture) {
                SDL_Rect textRect = {
                    (int)(w->x + 5),
                    (int)(w->y + (w->height - surface->h) / 2),
                    surface->w,
                    surface->h
                };
                SDL_RenderCopy(r, texture, nullptr, &textRect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

/**
 * @brief رسم جميع الويدجات
 */
void sad_ui_render_all(void* renderer) {
    SDL_Renderer* r = (SDL_Renderer*)renderer;
    
    // مسح الشاشة
    SDL_SetRenderDrawColor(r, 30, 30, 30, 255);
    SDL_RenderClear(r);
    
    // ترتيب الويدجات حسب zIndex
    std::vector<SadWidget*> sorted;
    for (auto& pair : g_widgets) {
        sorted.push_back(pair.second);
    }
    std::sort(sorted.begin(), sorted.end(), 
        [](SadWidget* a, SadWidget* b) { return a->zIndex < b->zIndex; });
    
    // رسم الويدجات
    for (SadWidget* w : sorted) {
        if (w->visible) {
            sad_ui_render_widget(renderer, w->id);
        }
    }
    
    // عرض الإطار
    SDL_RenderPresent(r);
}

// =============================================================================
// معالجة الأحداث
// =============================================================================

/**
 * @brief معالجة حدث واحد
 */
int sad_ui_process_event(SDL_Event* event) {
    if (event->type == SDL_QUIT) {
        g_running = false;
        return 1;
    }
    
    // معالجة أحداث الماوس
    if (event->type == SDL_MOUSEBUTTONDOWN) {
        int x = event->button.x;
        int y = event->button.y;
        
        // البحث عن الويدجت الذي تم النقر عليه
        for (auto& pair : g_widgets) {
            SadWidget* w = pair.second;
            if (w->visible && w->enabled) {
                if (x >= w->x && x <= w->x + w->width &&
                    y >= w->y && y <= w->y + w->height) {
                    // تم النقر على هذا الويدجت
                    // يمكن استدعاء معالج الحدث هنا
                    return w->id;
                }
            }
        }
    }
    
    return 0;
}

/**
 * @brief حلقة الأحداث الرئيسية
 */
int sad_ui_event_loop(void* window, void* renderer) {
    SDL_Event event;
    g_running = true;
    
    while (g_running) {
        // معالجة الأحداث
        while (SDL_PollEvent(&event)) {
            sad_ui_process_event(&event);
        }
        
        // رسم الواجهة
        sad_ui_render_all(renderer);
        
        // تأخير صغير
        SDL_Delay(16); // ~60 FPS
    }
    
    return 0;
}

/**
 * @brief انتظار حدث
 */
int sad_ui_wait_event() {
    SDL_Event event;
    if (SDL_WaitEvent(&event)) {
        return sad_ui_process_event(&event);
    }
    return -1;
}

/**
 * @brief فحص وجود حدث
 */
int sad_ui_poll_event() {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        return sad_ui_process_event(&event);
    }
    return 0;
}

// =============================================================================
// دوال الرسم المباشر
// =============================================================================

/**
 * @brief تعيين لون الرسم
 */
void sad_ui_set_draw_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    if (g_renderer) {
        SDL_SetRenderDrawColor(g_renderer, r, g, b, a);
    }
}

/**
 * @brief مسح الشاشة
 */
void sad_ui_clear() {
    if (g_renderer) {
        SDL_RenderClear(g_renderer);
    }
}

/**
 * @brief عرض المحتوى
 */
void sad_ui_present() {
    if (g_renderer) {
        SDL_RenderPresent(g_renderer);
    }
}

/**
 * @brief رسم مستطيل
 */
void sad_ui_draw_rect(int x, int y, int w, int h) {
    if (g_renderer) {
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderDrawRect(g_renderer, &rect);
    }
}

/**
 * @brief ملء مستطيل
 */
void sad_ui_fill_rect(int x, int y, int w, int h) {
    if (g_renderer) {
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderFillRect(g_renderer, &rect);
    }
}

/**
 * @brief رسم خط
 */
void sad_ui_draw_line(int x1, int y1, int x2, int y2) {
    if (g_renderer) {
        SDL_RenderDrawLine(g_renderer, x1, y1, x2, y2);
    }
}

// =============================================================================
// دوال مساعدة
// =============================================================================

/**
 * @brief الحصول على حجم الشاشة
 */
void sad_ui_get_screen_size(int* width, int* height) {
    if (g_window) {
        SDL_GetWindowSize(g_window, width, height);
    } else {
        *width = 800;
        *height = 600;
    }
}

/**
 * @brief الحصول على موقع الماوس
 */
void sad_ui_get_mouse_pos(int* x, int* y) {
    SDL_GetMouseState(x, y);
}

/**
 * @brief فحص ضغط مفتاح
 */
bool sad_ui_is_key_pressed(int key) {
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    return state[key] != 0;
}

/**
 * @brief تحميل خط
 */
void sad_ui_load_font(const char* path, int size) {
    if (g_font) {
        TTF_CloseFont(g_font);
    }
    g_font = TTF_OpenFont(path, size);
}

/**
 * @brief الحصول على الـ renderer
 */
void* sad_ui_get_renderer() {
    return g_renderer;
}

/**
 * @brief الحصول على النافذة
 */
void* sad_ui_get_window() {
    return g_window;
}

} // extern "C"
