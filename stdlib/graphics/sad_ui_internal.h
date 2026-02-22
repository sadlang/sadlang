/**
 * @file sad_ui_internal.h
 * @brief (AR) تصريحات داخلية مشتركة لنظام واجهة المستخدم
 * @brief (EN) Internal shared declarations for the UI framework
 * 
 * Shared between sad_ui_fonts_arabic.cpp, sad_ui_layout_render.cpp,
 * and sad_ui_api.cpp (originally one translation unit).
 */

#pragma once

#include "sad_ui_framework.h"
#include <SDL.h>
#include <string>
#include <vector>
#include <unordered_map>

namespace sad {
namespace ui {

// ═══════════════════════════════════════════════════════════════════
// AppState struct (shared between fonts_arabic and api)
// ═══════════════════════════════════════════════════════════════════

struct AppState {
    int id = 0;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int rootWidget = -1;
    bool running = true;
    int width = 800, height = 600;
    RGBA bgColor = {245, 245, 245, 255};
    int mouseX = 0, mouseY = 0;
    bool mouseDown = false;
    bool prevMouseDown = false;
    bool mouseClicked = false;
    std::string keyInput;
    bool backspace = false;
    Uint32 lastTickMs = 0;
    double deltaMs = 16.0;
    int targetFps = 60;
    double fps = 60.0;
    double fpsAccum = 0;
    int fpsFrames = 0;
};

// ═══════════════════════════════════════════════════════════════════
// Extern global variables (defined in sad_ui_fonts_arabic.cpp)
// ═══════════════════════════════════════════════════════════════════

extern std::unordered_map<int, Widget> g_widgets;
extern int g_nextWidgetId;
extern std::unordered_map<int, AppState> g_apps;
extern int g_nextAppId;
extern const unsigned char FONT_DATA[96][7];

// ═══════════════════════════════════════════════════════════════════
// Functions defined in sad_ui_fonts_arabic.cpp
// ═══════════════════════════════════════════════════════════════════

Widget* getWidget(int id);
Widget* getWidgetByName(const std::string& name);
AppState* getApp(int id);
int createWidget(WidgetType type);
void ensureFontLoaded();
bool isFontLoaded();
int measureTextWidth(const std::string& text, int scaleOrPixelSize);
int measureTextHeight(int scaleOrPixelSize);
void drawCharBitmap(SDL_Renderer* renderer, char c, int x, int y, int scale, RGBA color);
void drawText(SDL_Renderer* renderer, const std::string& text,
              int x, int y, int scaleOrPixelSize, RGBA color,
              const std::string& align = "left", int containerWidth = 0);

// ═══════════════════════════════════════════════════════════════════
// Functions defined in sad_ui_layout_render.cpp
// ═══════════════════════════════════════════════════════════════════

void layoutWidget(int widgetId, float x, float y, float availW, float availH);
void renderWidget(SDL_Renderer* renderer, int widgetId);

}} // namespace sad::ui
