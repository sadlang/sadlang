/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ نظام واجهات ص - Sad Flutter-like UI Implementation
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_flutter_ui.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef __ANDROID__
#include <android/log.h>
#include <GLES3/gl3.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "SadUI", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "SadUI", __VA_ARGS__)
#else
#include <stdio.h>
#define LOGI(...) printf(__VA_ARGS__)
#define LOGE(...) fprintf(stderr, __VA_ARGS__)
// محاكاة OpenGL للتطوير
#define GL_TRIANGLES 0
#define GL_TRIANGLE_FAN 0
#define GL_FLOAT 0
#define GL_UNSIGNED_BYTE 0
#define GL_BLEND 0
#define GL_SRC_ALPHA 0
#define GL_ONE_MINUS_SRC_ALPHA 0
typedef unsigned int GLuint;
typedef int GLint;
typedef float GLfloat;
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  الثوابت الداخلية
// ═══════════════════════════════════════════════════════════════════════════════

#define MAX_VERTICES 65536
#define PI 3.14159265359f

// ═══════════════════════════════════════════════════════════════════════════════
//  بنى الرسم الداخلية
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct {
    float x, y;
    float u, v;
    float r, g, b, a;
} Vertex;

// حالة الرسم العامة
static struct {
    SadApp* currentApp;
    Vertex* vertices;
    int vertexCount;
    GLuint vbo;
    GLuint vao;
    
    // الشيدرات
    GLuint colorProgram;
    GLuint textProgram;
    
    // مصفوفة الإسقاط
    float projection[16];
    
    // الخط
    void* fontAtlas;
    GLuint fontTexture;
    
} g_render = {0};

// ═══════════════════════════════════════════════════════════════════════════════
//  الشيدرات
// ═══════════════════════════════════════════════════════════════════════════════

static const char* VERTEX_SHADER_SRC = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "layout(location = 0) in vec2 aPos;\n"
    "layout(location = 1) in vec2 aTexCoord;\n"
    "layout(location = 2) in vec4 aColor;\n"
    "uniform mat4 uProjection;\n"
    "out vec2 vTexCoord;\n"
    "out vec4 vColor;\n"
    "void main() {\n"
    "    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);\n"
    "    vTexCoord = aTexCoord;\n"
    "    vColor = aColor;\n"
    "}\n";

static const char* FRAGMENT_SHADER_COLOR = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec4 vColor;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = vColor;\n"
    "}\n";

static const char* FRAGMENT_SHADER_TEXT = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 vTexCoord;\n"
    "in vec4 vColor;\n"
    "uniform sampler2D uTexture;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    float alpha = texture(uTexture, vTexCoord).r;\n"
    "    fragColor = vec4(vColor.rgb, vColor.a * alpha);\n"
    "}\n";

static const char* FRAGMENT_SHADER_ROUNDED = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec2 vTexCoord;\n"
    "in vec4 vColor;\n"
    "uniform vec4 uRect;\n"        // x, y, width, height
    "uniform vec4 uRadius;\n"      // topLeft, topRight, bottomRight, bottomLeft
    "out vec4 fragColor;\n"
    "float roundedBox(vec2 p, vec2 b, float r) {\n"
    "    vec2 q = abs(p) - b + r;\n"
    "    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;\n"
    "}\n"
    "void main() {\n"
    "    vec2 center = uRect.xy + uRect.zw * 0.5;\n"
    "    vec2 halfSize = uRect.zw * 0.5;\n"
    "    vec2 p = gl_FragCoord.xy - center;\n"
    "    float r = uRadius.x;\n"   // استخدام نصف قطر واحد للتبسيط
    "    float d = roundedBox(p, halfSize, r);\n"
    "    float aa = fwidth(d);\n"
    "    float alpha = 1.0 - smoothstep(-aa, aa, d);\n"
    "    fragColor = vec4(vColor.rgb, vColor.a * alpha);\n"
    "}\n";

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال OpenGL المساعدة
// ═══════════════════════════════════════════════════════════════════════════════

#ifdef __ANDROID__

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        LOGE("Shader compile error: %s", log);
        return 0;
    }
    return shader;
}

static GLuint create_program(const char* vertSrc, const char* fragSrc) {
    GLuint vert = compile_shader(GL_VERTEX_SHADER, vertSrc);
    GLuint frag = compile_shader(GL_FRAGMENT_SHADER, fragSrc);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        LOGE("Program link error: %s", log);
        return 0;
    }
    return program;
}

static void init_render_state(int width, int height) {
    // إنشاء الشيدرات
    g_render.colorProgram = create_program(VERTEX_SHADER_SRC, FRAGMENT_SHADER_COLOR);
    g_render.textProgram = create_program(VERTEX_SHADER_SRC, FRAGMENT_SHADER_TEXT);
    
    // إنشاء VBO و VAO
    glGenVertexArrays(1, &g_render.vao);
    glGenBuffers(1, &g_render.vbo);
    
    glBindVertexArray(g_render.vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_render.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_VERTICES, NULL, GL_DYNAMIC_DRAW);
    
    // تعيين المتغيرات
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, r));
    glEnableVertexAttribArray(2);
    
    // مصفوفة الإسقاط (Orthographic)
    float left = 0, right = (float)width;
    float bottom = (float)height, top = 0;
    float near_val = -1, far_val = 1;
    
    memset(g_render.projection, 0, sizeof(g_render.projection));
    g_render.projection[0] = 2.0f / (right - left);
    g_render.projection[5] = 2.0f / (top - bottom);
    g_render.projection[10] = -2.0f / (far_val - near_val);
    g_render.projection[12] = -(right + left) / (right - left);
    g_render.projection[13] = -(top + bottom) / (top - bottom);
    g_render.projection[14] = -(far_val + near_val) / (far_val - near_val);
    g_render.projection[15] = 1.0f;
    
    // تفعيل الشفافية
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // تخصيص ذاكرة الرؤوس
    g_render.vertices = (Vertex*)malloc(sizeof(Vertex) * MAX_VERTICES);
    g_render.vertexCount = 0;
    
    LOGI("Render state initialized: %dx%d", width, height);
}

static void flush_vertices(void) {
    if (g_render.vertexCount == 0) return;
    
    glBindBuffer(GL_ARRAY_BUFFER, g_render.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * g_render.vertexCount, g_render.vertices);
    
    glUseProgram(g_render.colorProgram);
    GLint projLoc = glGetUniformLocation(g_render.colorProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, g_render.projection);
    
    glBindVertexArray(g_render.vao);
    glDrawArrays(GL_TRIANGLES, 0, g_render.vertexCount);
    
    g_render.vertexCount = 0;
}

static void push_vertex(float x, float y, float u, float v, SadColor c) {
    if (g_render.vertexCount >= MAX_VERTICES) {
        flush_vertices();
    }
    Vertex* v_ptr = &g_render.vertices[g_render.vertexCount++];
    v_ptr->x = x;
    v_ptr->y = y;
    v_ptr->u = u;
    v_ptr->v = v;
    v_ptr->r = c.r / 255.0f;
    v_ptr->g = c.g / 255.0f;
    v_ptr->b = c.b / 255.0f;
    v_ptr->a = c.a / 255.0f;
}

static void push_quad(float x, float y, float w, float h, SadColor c) {
    // مثلث 1
    push_vertex(x, y, 0, 0, c);
    push_vertex(x + w, y, 1, 0, c);
    push_vertex(x + w, y + h, 1, 1, c);
    // مثلث 2
    push_vertex(x, y, 0, 0, c);
    push_vertex(x + w, y + h, 1, 1, c);
    push_vertex(x, y + h, 0, 1, c);
}

static void push_rounded_rect(float x, float y, float w, float h, float r, SadColor c) {
    // رسم مستطيل مستدير باستخدام عدة مثلثات
    if (r <= 0) {
        push_quad(x, y, w, h, c);
        return;
    }
    
    r = fminf(r, fminf(w, h) / 2.0f);
    
    // المستطيل المركزي
    push_quad(x + r, y, w - 2*r, h, c);
    // الجانبين
    push_quad(x, y + r, r, h - 2*r, c);
    push_quad(x + w - r, y + r, r, h - 2*r, c);
    
    // الزوايا (تقريبية بمثلثات)
    int segments = 8;
    float angleStep = (PI / 2.0f) / segments;
    
    // الزاوية العليا اليسرى
    float cx = x + r, cy = y + r;
    for (int i = 0; i < segments; i++) {
        float a1 = PI + i * angleStep;
        float a2 = PI + (i + 1) * angleStep;
        push_vertex(cx, cy, 0.5f, 0.5f, c);
        push_vertex(cx + r * cosf(a1), cy + r * sinf(a1), 0, 0, c);
        push_vertex(cx + r * cosf(a2), cy + r * sinf(a2), 0, 0, c);
    }
    
    // الزاوية العليا اليمنى
    cx = x + w - r; cy = y + r;
    for (int i = 0; i < segments; i++) {
        float a1 = -PI/2 + i * angleStep;
        float a2 = -PI/2 + (i + 1) * angleStep;
        push_vertex(cx, cy, 0.5f, 0.5f, c);
        push_vertex(cx + r * cosf(a1), cy + r * sinf(a1), 0, 0, c);
        push_vertex(cx + r * cosf(a2), cy + r * sinf(a2), 0, 0, c);
    }
    
    // الزاوية السفلى اليمنى
    cx = x + w - r; cy = y + h - r;
    for (int i = 0; i < segments; i++) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;
        push_vertex(cx, cy, 0.5f, 0.5f, c);
        push_vertex(cx + r * cosf(a1), cy + r * sinf(a1), 0, 0, c);
        push_vertex(cx + r * cosf(a2), cy + r * sinf(a2), 0, 0, c);
    }
    
    // الزاوية السفلى اليسرى
    cx = x + r; cy = y + h - r;
    for (int i = 0; i < segments; i++) {
        float a1 = PI/2 + i * angleStep;
        float a2 = PI/2 + (i + 1) * angleStep;
        push_vertex(cx, cy, 0.5f, 0.5f, c);
        push_vertex(cx + r * cosf(a1), cy + r * sinf(a1), 0, 0, c);
        push_vertex(cx + r * cosf(a2), cy + r * sinf(a2), 0, 0, c);
    }
}

static void push_circle(float cx, float cy, float radius, SadColor c) {
    int segments = 32;
    float angleStep = (2.0f * PI) / segments;
    
    for (int i = 0; i < segments; i++) {
        float a1 = i * angleStep;
        float a2 = (i + 1) * angleStep;
        push_vertex(cx, cy, 0.5f, 0.5f, c);
        push_vertex(cx + radius * cosf(a1), cy + radius * sinf(a1), 0, 0, c);
        push_vertex(cx + radius * cosf(a2), cy + radius * sinf(a2), 0, 0, c);
    }
}

#else
// تنفيذات وهمية لغير Android
static void init_render_state(int width, int height) {}
static void flush_vertices(void) {}
static void push_quad(float x, float y, float w, float h, SadColor c) {}
static void push_rounded_rect(float x, float y, float w, float h, float r, SadColor c) {}
static void push_circle(float cx, float cy, float radius, SadColor c) {}
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  إنشاء الـ Widget
// ═══════════════════════════════════════════════════════════════════════════════

static int g_nextWidgetId = 1;

static SadWidget* create_widget(SadWidgetType type) {
    SadWidget* w = (SadWidget*)calloc(1, sizeof(SadWidget));
    if (!w) return NULL;
    
    w->type = type;
    w->id = g_nextWidgetId++;
    w->width = -1;  // auto
    w->height = -1; // auto
    w->visible = true;
    w->enabled = true;
    w->opacity = 1.0f;
    w->decoration.type = SADDECOR_NONE;
    
    // تسجيل في التطبيق
    if (g_render.currentApp) {
        SadApp* app = g_render.currentApp;
        if (app->widgetCount >= app->widgetCapacity) {
            int newCap = app->widgetCapacity ? app->widgetCapacity * 2 : 64;
            app->allWidgets = (SadWidget**)realloc(app->allWidgets, sizeof(SadWidget*) * newCap);
            app->widgetCapacity = newCap;
        }
        app->allWidgets[app->widgetCount++] = w;
    }
    
    return w;
}

static void destroy_widget(SadWidget* w) {
    if (!w) return;
    
    // تحرير النص
    if (w->type == SADWIDGET_TEXT && w->textData.text) {
        free(w->textData.text);
    }
    if (w->type == SADWIDGET_BUTTON && w->buttonData.label) {
        free(w->buttonData.label);
    }
    
    // تحرير الأبناء
    if (w->children) {
        free(w->children);
    }
    
    free(w);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إدارة التطبيق
// ═══════════════════════════════════════════════════════════════════════════════

SadApp* sadapp_create(int width, int height, float density) {
    SadApp* app = (SadApp*)calloc(1, sizeof(SadApp));
    if (!app) return NULL;
    
    app->screenWidth = width;
    app->screenHeight = height;
    app->density = density > 0 ? density : 2.0f;
    app->running = true;
    app->isRTL = true;  // افتراضي RTL للعربية
    
    // الألوان الافتراضية
    app->primaryColor = SADCOLOR_PRIMARY;
    app->secondaryColor = SADCOLOR_SECONDARY;
    app->backgroundColor = SADCOLOR_BACKGROUND;
    app->surfaceColor = SADCOLOR_SURFACE;
    app->textColor = SADCOLOR_BLACK;
    
    g_render.currentApp = app;
    
#ifdef __ANDROID__
    init_render_state(width, height);
#endif
    
    LOGI("SadApp created: %dx%d @%.1fx", width, height, density);
    return app;
}

void sadapp_destroy(SadApp* app) {
    if (!app) return;
    
    // تحرير جميع الـ Widgets
    for (int i = 0; i < app->widgetCount; i++) {
        destroy_widget(app->allWidgets[i]);
    }
    free(app->allWidgets);
    
#ifdef __ANDROID__
    if (g_render.vertices) free(g_render.vertices);
    if (g_render.colorProgram) glDeleteProgram(g_render.colorProgram);
    if (g_render.textProgram) glDeleteProgram(g_render.textProgram);
    if (g_render.vbo) glDeleteBuffers(1, &g_render.vbo);
    if (g_render.vao) glDeleteVertexArrays(1, &g_render.vao);
#endif
    
    memset(&g_render, 0, sizeof(g_render));
    free(app);
}

void sadapp_set_root(SadApp* app, SadWidget* root) {
    if (!app) return;
    app->rootWidget = root;
    app->needsLayout = true;
    app->needsPaint = true;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  نظام التخطيط
// ═══════════════════════════════════════════════════════════════════════════════

static void layout_widget(SadWidget* w, float x, float y, float maxWidth, float maxHeight);

static void layout_column(SadWidget* col, float x, float y, float maxWidth, float maxHeight) {
    float spacing = col->layoutData.spacing;
    float contentHeight = 0;
    float totalFlex = 0;
    float fixedHeight = 0;
    
    // الحساب الأولي
    for (int i = 0; i < col->childCount; i++) {
        SadWidget* child = col->children[i];
        if (!child->visible) continue;
        
        if (child->flex > 0) {
            totalFlex += child->flex;
        } else {
            layout_widget(child, 0, 0, maxWidth - col->padding.left - col->padding.right, 0);
            fixedHeight += child->bounds.height + child->margin.top + child->margin.bottom;
        }
    }
    
    fixedHeight += spacing * (col->childCount - 1);
    float flexSpace = maxHeight - col->padding.top - col->padding.bottom - fixedHeight;
    
    // التخطيط الفعلي
    float curY = y + col->padding.top;
    float contentWidth = maxWidth - col->padding.left - col->padding.right;
    
    for (int i = 0; i < col->childCount; i++) {
        SadWidget* child = col->children[i];
        if (!child->visible) continue;
        
        curY += child->margin.top;
        
        float childHeight;
        if (child->flex > 0 && totalFlex > 0) {
            childHeight = (child->flex / totalFlex) * flexSpace;
        } else {
            childHeight = child->bounds.height;
        }
        
        float childX = x + col->padding.left + child->margin.left;
        float childWidth = contentWidth - child->margin.left - child->margin.right;
        
        // المحاذاة العرضية
        if (col->layoutData.crossAxisAlignment == SADALIGN_CENTER && child->width > 0) {
            childX = x + col->padding.left + (contentWidth - child->width) / 2;
            childWidth = child->width;
        } else if (col->layoutData.crossAxisAlignment == SADALIGN_END && child->width > 0) {
            childX = x + maxWidth - col->padding.right - child->margin.right - child->width;
            childWidth = child->width;
        }
        
        layout_widget(child, childX, curY, childWidth, childHeight);
        
        curY += child->bounds.height + child->margin.bottom + spacing;
        contentHeight = curY - y;
    }
    
    col->bounds.x = x;
    col->bounds.y = y;
    col->bounds.width = maxWidth;
    col->bounds.height = (col->height > 0) ? col->height : contentHeight;
}

static void layout_row(SadWidget* row, float x, float y, float maxWidth, float maxHeight) {
    float spacing = row->layoutData.spacing;
    float totalFlex = 0;
    float fixedWidth = 0;
    
    // الحساب الأولي
    for (int i = 0; i < row->childCount; i++) {
        SadWidget* child = row->children[i];
        if (!child->visible) continue;
        
        if (child->flex > 0) {
            totalFlex += child->flex;
        } else {
            layout_widget(child, 0, 0, 0, maxHeight - row->padding.top - row->padding.bottom);
            fixedWidth += child->bounds.width + child->margin.left + child->margin.right;
        }
    }
    
    fixedWidth += spacing * (row->childCount - 1);
    float flexSpace = maxWidth - row->padding.left - row->padding.right - fixedWidth;
    
    // التخطيط الفعلي (RTL)
    float curX;
    if (g_render.currentApp && g_render.currentApp->isRTL) {
        curX = x + maxWidth - row->padding.right;
    } else {
        curX = x + row->padding.left;
    }
    float contentHeight = row->padding.top + row->padding.bottom;
    float rowHeight = maxHeight - row->padding.top - row->padding.bottom;
    
    for (int i = 0; i < row->childCount; i++) {
        SadWidget* child = row->children[i];
        if (!child->visible) continue;
        
        float childWidth;
        if (child->flex > 0 && totalFlex > 0) {
            childWidth = (child->flex / totalFlex) * flexSpace;
        } else {
            childWidth = child->bounds.width;
        }
        
        float childY = y + row->padding.top + child->margin.top;
        float childHeight = rowHeight - child->margin.top - child->margin.bottom;
        
        // RTL
        if (g_render.currentApp && g_render.currentApp->isRTL) {
            curX -= child->margin.right + childWidth;
            layout_widget(child, curX, childY, childWidth, childHeight);
            curX -= child->margin.left + spacing;
        } else {
            curX += child->margin.left;
            layout_widget(child, curX, childY, childWidth, childHeight);
            curX += childWidth + child->margin.right + spacing;
        }
        
        if (child->bounds.height + row->padding.top + row->padding.bottom > contentHeight) {
            contentHeight = child->bounds.height + row->padding.top + row->padding.bottom;
        }
    }
    
    row->bounds.x = x;
    row->bounds.y = y;
    row->bounds.width = maxWidth;
    row->bounds.height = (row->height > 0) ? row->height : contentHeight;
}

static void layout_widget(SadWidget* w, float x, float y, float maxWidth, float maxHeight) {
    if (!w || !w->visible) return;
    
    // حساب الأبعاد
    float width = (w->width > 0) ? w->width : maxWidth;
    float height = (w->height > 0) ? w->height : maxHeight;
    
    switch (w->type) {
        case SADWIDGET_COLUMN:
            layout_column(w, x, y, width, height);
            break;
            
        case SADWIDGET_ROW:
            layout_row(w, x, y, width, height);
            break;
            
        case SADWIDGET_TEXT: {
            // حساب حجم النص
            float fontSize = w->textData.textStyle.fontSize;
            if (fontSize <= 0) fontSize = 16;
            int len = w->textData.text ? (int)strlen(w->textData.text) : 0;
            // تقدير بسيط للعرض (حرف عربي ≈ 0.7 من حجم الخط)
            float textWidth = len * fontSize * 0.7f;
            float textHeight = fontSize * w->textData.textStyle.lineHeight;
            
            w->bounds.x = x;
            w->bounds.y = y;
            w->bounds.width = (w->width > 0) ? w->width : fminf(textWidth, maxWidth);
            w->bounds.height = (w->height > 0) ? w->height : textHeight;
            break;
        }
            
        case SADWIDGET_BUTTON: {
            float minHeight = 48;
            float minWidth = 88;
            
            w->bounds.x = x;
            w->bounds.y = y;
            w->bounds.width = (w->width > 0) ? w->width : fmaxf(minWidth, maxWidth);
            w->bounds.height = (w->height > 0) ? w->height : minHeight;
            break;
        }
            
        case SADWIDGET_CARD:
        case SADWIDGET_CONTAINER:
        case SADWIDGET_SCAFFOLD:
            // تخطيط الأبناء
            w->bounds.x = x;
            w->bounds.y = y;
            w->bounds.width = width;
            w->bounds.height = height;
            
            for (int i = 0; i < w->childCount; i++) {
                layout_widget(w->children[i], 
                    x + w->padding.left, 
                    y + w->padding.top,
                    width - w->padding.left - w->padding.right,
                    height - w->padding.top - w->padding.bottom);
            }
            break;
            
        default:
            w->bounds.x = x;
            w->bounds.y = y;
            w->bounds.width = width;
            w->bounds.height = height;
            break;
    }
}

void sadapp_layout(SadApp* app) {
    if (!app || !app->rootWidget) return;
    
    layout_widget(app->rootWidget, 0, 0, (float)app->screenWidth, (float)app->screenHeight);
    app->needsLayout = false;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الرسم
// ═══════════════════════════════════════════════════════════════════════════════

static void render_widget(SadWidget* w);

static void render_decoration(SadWidget* w) {
    if (w->decoration.type == SADDECOR_NONE && 
        w->decoration.borderRadius.topLeft == 0 &&
        w->decoration.border.width == 0) {
        return;
    }
    
    SadColor bgColor = w->decoration.color;
    if (w->decoration.type == SADDECOR_NONE) {
        bgColor = SADCOLOR_TRANSPARENT;
    }
    
    float r = w->decoration.borderRadius.topLeft;
    push_rounded_rect(w->bounds.x, w->bounds.y, w->bounds.width, w->bounds.height, r, bgColor);
}

static void render_text(SadWidget* w) {
    // TODO: رسم النص الفعلي باستخدام الخطوط
    // حالياً نرسم مستطيل كعنصر نائب
    SadColor textColor = w->textData.textStyle.color;
    if (textColor.a == 0) textColor = SADCOLOR_BLACK;
    
    // خلفية للنص (للتصحيح)
    // push_quad(w->bounds.x, w->bounds.y, w->bounds.width, w->bounds.height, 
    //           sadcolor(200, 200, 200, 50));
}

static void render_button(SadWidget* w) {
    SadColor bgColor = w->buttonData.buttonColor;
    if (bgColor.a == 0) bgColor = SADCOLOR_PRIMARY;
    
    if (w->buttonData.isPressed) {
        bgColor = w->buttonData.pressedColor;
        if (bgColor.a == 0) {
            bgColor.r = (uint8_t)(bgColor.r * 0.8f);
            bgColor.g = (uint8_t)(bgColor.g * 0.8f);
            bgColor.b = (uint8_t)(bgColor.b * 0.8f);
        }
    }
    
    if (!w->enabled) {
        bgColor = w->buttonData.disabledColor;
        if (bgColor.a == 0) bgColor = SADCOLOR_GREY;
    }
    
    float radius = 8;
    if (w->decoration.borderRadius.topLeft > 0) {
        radius = w->decoration.borderRadius.topLeft;
    }
    
    // الظل
    if (w->buttonData.isElevated && w->enabled) {
        SadColor shadowColor = sadcolor(0, 0, 0, 50);
        push_rounded_rect(w->bounds.x + 2, w->bounds.y + 4, 
                         w->bounds.width, w->bounds.height, radius, shadowColor);
    }
    
    push_rounded_rect(w->bounds.x, w->bounds.y, w->bounds.width, w->bounds.height, radius, bgColor);
    
    // TODO: رسم نص الزر
}

static void render_card(SadWidget* w) {
    SadColor bgColor = SADCOLOR_SURFACE;
    if (w->decoration.type == SADDECOR_COLOR) {
        bgColor = w->decoration.color;
    }
    
    float radius = w->decoration.borderRadius.topLeft;
    if (radius == 0) radius = 12;
    
    // الظل
    SadColor shadowColor = sadcolor(0, 0, 0, 30);
    push_rounded_rect(w->bounds.x + 1, w->bounds.y + 3, 
                     w->bounds.width, w->bounds.height, radius, shadowColor);
    
    push_rounded_rect(w->bounds.x, w->bounds.y, w->bounds.width, w->bounds.height, radius, bgColor);
    
    // رسم الأبناء
    for (int i = 0; i < w->childCount; i++) {
        render_widget(w->children[i]);
    }
}

static void render_appbar(SadWidget* w) {
    SadColor bgColor = SADCOLOR_PRIMARY;
    if (w->decoration.type == SADDECOR_COLOR) {
        bgColor = w->decoration.color;
    }
    
    push_quad(w->bounds.x, w->bounds.y, w->bounds.width, w->bounds.height, bgColor);
    
    // TODO: رسم العنوان والأيقونات
}

static void render_bottomnav(SadWidget* w) {
    // الظل العلوي
    SadColor shadowColor = sadcolor(0, 0, 0, 20);
    push_quad(w->bounds.x, w->bounds.y - 2, w->bounds.width, 2, shadowColor);
    
    // الخلفية
    push_quad(w->bounds.x, w->bounds.y, w->bounds.width, w->bounds.height, SADCOLOR_SURFACE);
    
    // TODO: رسم الأيقونات
}

static void render_widget(SadWidget* w) {
    if (!w || !w->visible) return;
    
    // تطبيق الشفافية
    if (w->opacity < 1.0f) {
        // TODO: تطبيق الشفافية
    }
    
    switch (w->type) {
        case SADWIDGET_CONTAINER:
        case SADWIDGET_COLUMN:
        case SADWIDGET_ROW:
        case SADWIDGET_STACK:
        case SADWIDGET_SCAFFOLD:
            render_decoration(w);
            for (int i = 0; i < w->childCount; i++) {
                render_widget(w->children[i]);
            }
            break;
            
        case SADWIDGET_TEXT:
            render_text(w);
            break;
            
        case SADWIDGET_BUTTON:
            render_button(w);
            break;
            
        case SADWIDGET_CARD:
            render_card(w);
            break;
            
        case SADWIDGET_APPBAR:
            render_appbar(w);
            break;
            
        case SADWIDGET_BOTTOMNAV:
            render_bottomnav(w);
            break;
            
        case SADWIDGET_DIVIDER:
            push_quad(w->bounds.x, w->bounds.y, w->bounds.width, 1, SADCOLOR_GREY);
            break;
            
        case SADWIDGET_PROGRESSBAR: {
            // الخلفية
            push_rounded_rect(w->bounds.x, w->bounds.y, w->bounds.width, w->bounds.height, 
                            w->bounds.height/2, w->progressData.backgroundColor);
            // التقدم
            float progWidth = w->bounds.width * w->progressData.progress;
            push_rounded_rect(w->bounds.x, w->bounds.y, progWidth, w->bounds.height,
                            w->bounds.height/2, w->progressData.progressColor);
            break;
        }
            
        case SADWIDGET_CHECKBOX: {
            float size = w->bounds.height;
            SadColor boxColor = w->toggleData.checked ? w->toggleData.activeColor : SADCOLOR_GREY;
            if (boxColor.a == 0) boxColor = SADCOLOR_PRIMARY;
            push_rounded_rect(w->bounds.x, w->bounds.y, size, size, 4, boxColor);
            // TODO: رسم علامة الصح
            break;
        }
            
        case SADWIDGET_SWITCH: {
            float trackWidth = w->bounds.width;
            float trackHeight = w->bounds.height * 0.6f;
            float thumbRadius = w->bounds.height / 2;
            
            SadColor trackColor = w->toggleData.checked ? 
                sadcolor(w->toggleData.activeColor.r, w->toggleData.activeColor.g, 
                        w->toggleData.activeColor.b, 128) : 
                sadcolor(128, 128, 128, 128);
            
            float trackY = w->bounds.y + (w->bounds.height - trackHeight) / 2;
            push_rounded_rect(w->bounds.x, trackY, trackWidth, trackHeight, 
                            trackHeight/2, trackColor);
            
            SadColor thumbColor = w->toggleData.checked ? w->toggleData.activeColor : SADCOLOR_WHITE;
            if (thumbColor.a == 0) thumbColor = SADCOLOR_PRIMARY;
            
            float thumbX = w->toggleData.checked ? 
                w->bounds.x + trackWidth - thumbRadius * 2 : w->bounds.x;
            push_circle(thumbX + thumbRadius, w->bounds.y + w->bounds.height/2, thumbRadius, thumbColor);
            break;
        }
            
        default:
            render_decoration(w);
            for (int i = 0; i < w->childCount; i++) {
                render_widget(w->children[i]);
            }
            break;
    }
}

void sadapp_render(SadApp* app) {
    if (!app) return;
    
#ifdef __ANDROID__
    glClearColor(
        app->backgroundColor.r / 255.0f,
        app->backgroundColor.g / 255.0f,
        app->backgroundColor.b / 255.0f,
        1.0f
    );
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (app->needsLayout) {
        sadapp_layout(app);
    }
    
    if (app->rootWidget) {
        render_widget(app->rootWidget);
    }
    
    flush_vertices();
#endif
    
    app->needsPaint = false;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الأحداث
// ═══════════════════════════════════════════════════════════════════════════════

static SadWidget* find_widget_at(SadWidget* w, float x, float y) {
    if (!w || !w->visible) return NULL;
    
    // هل النقطة داخل الـ Widget؟
    if (x < w->bounds.x || x > w->bounds.x + w->bounds.width ||
        y < w->bounds.y || y > w->bounds.y + w->bounds.height) {
        return NULL;
    }
    
    // البحث في الأبناء (من الأمام للخلف)
    for (int i = w->childCount - 1; i >= 0; i--) {
        SadWidget* found = find_widget_at(w->children[i], x, y);
        if (found) return found;
    }
    
    // إذا كان العنصر تفاعلياً
    if (w->onTap || w->onLongPress || 
        w->type == SADWIDGET_BUTTON ||
        w->type == SADWIDGET_CHECKBOX ||
        w->type == SADWIDGET_SWITCH ||
        w->type == SADWIDGET_SLIDER ||
        w->type == SADWIDGET_TEXTFIELD) {
        return w;
    }
    
    return NULL;
}

bool sadapp_touch(SadApp* app, int action, float x, float y) {
    if (!app || !app->rootWidget) return false;
    
    // 0 = down, 1 = up, 2 = move, 3 = cancel
    
    if (action == 0) { // down
        SadWidget* hit = find_widget_at(app->rootWidget, x, y);
        if (hit) {
            app->pressedWidget = hit;
            
            if (hit->type == SADWIDGET_BUTTON) {
                hit->buttonData.isPressed = true;
                app->needsPaint = true;
            }
            
            LOGI("Touch down on widget %d (type %d)", hit->id, hit->type);
            return true;
        }
    }
    else if (action == 1) { // up
        if (app->pressedWidget) {
            SadWidget* hit = find_widget_at(app->rootWidget, x, y);
            
            if (hit == app->pressedWidget) {
                // نقرة!
                if (hit->type == SADWIDGET_BUTTON) {
                    hit->buttonData.isPressed = false;
                    if (hit->onTap) {
                        hit->onTap(hit, hit->userData);
                    }
                }
                else if (hit->type == SADWIDGET_CHECKBOX) {
                    hit->toggleData.checked = !hit->toggleData.checked;
                    if (hit->onChanged) {
                        hit->onChanged(hit, &hit->toggleData.checked, hit->userData);
                    }
                }
                else if (hit->type == SADWIDGET_SWITCH) {
                    hit->toggleData.checked = !hit->toggleData.checked;
                    if (hit->onChanged) {
                        hit->onChanged(hit, &hit->toggleData.checked, hit->userData);
                    }
                }
                else if (hit->onTap) {
                    hit->onTap(hit, hit->userData);
                }
                
                LOGI("Tap on widget %d", hit->id);
            }
            
            if (app->pressedWidget->type == SADWIDGET_BUTTON) {
                app->pressedWidget->buttonData.isPressed = false;
            }
            
            app->pressedWidget = NULL;
            app->needsPaint = true;
            return true;
        }
    }
    else if (action == 3) { // cancel
        if (app->pressedWidget) {
            if (app->pressedWidget->type == SADWIDGET_BUTTON) {
                app->pressedWidget->buttonData.isPressed = false;
            }
            app->pressedWidget = NULL;
            app->needsPaint = true;
        }
    }
    
    return false;
}

void sadapp_run_frame(SadApp* app) {
    if (!app) return;
    
    if (app->needsLayout) {
        sadapp_layout(app);
    }
    
    if (app->needsPaint || app->needsRebuild) {
        sadapp_render(app);
    }
}

bool sadapp_needs_redraw(SadApp* app) {
    return app && (app->needsPaint || app->needsLayout || app->needsRebuild);
}

void sadapp_rebuild(SadApp* app) {
    if (app) {
        app->needsRebuild = true;
        app->needsLayout = true;
        app->needsPaint = true;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إنشاء الـ Widgets
// ═══════════════════════════════════════════════════════════════════════════════

SadWidget* sad_container(void) {
    return create_widget(SADWIDGET_CONTAINER);
}

SadWidget* sad_column(void) {
    SadWidget* w = create_widget(SADWIDGET_COLUMN);
    if (w) {
        w->layoutData.mainAxisAlignment = SADALIGN_START;
        w->layoutData.crossAxisAlignment = SADALIGN_STRETCH;
        w->layoutData.spacing = 0;
    }
    return w;
}

SadWidget* sad_row(void) {
    SadWidget* w = create_widget(SADWIDGET_ROW);
    if (w) {
        w->layoutData.mainAxisAlignment = SADALIGN_START;
        w->layoutData.crossAxisAlignment = SADALIGN_CENTER;
        w->layoutData.spacing = 0;
    }
    return w;
}

SadWidget* sad_stack(void) {
    return create_widget(SADWIDGET_STACK);
}

SadWidget* sad_text(const char* text) {
    SadWidget* w = create_widget(SADWIDGET_TEXT);
    if (w && text) {
        w->textData.text = strdup(text);
        w->textData.textStyle = SADTEXT_BODY1;
    }
    return w;
}

SadWidget* sad_text_styled(const char* text, SadTextStyle style) {
    SadWidget* w = create_widget(SADWIDGET_TEXT);
    if (w && text) {
        w->textData.text = strdup(text);
        w->textData.textStyle = style;
    }
    return w;
}

SadWidget* sad_icon(int iconCode, float size, SadColor color) {
    SadWidget* w = create_widget(SADWIDGET_ICON);
    if (w) {
        w->iconData.iconCode = iconCode;
        w->iconData.iconSize = size;
        w->iconData.iconColor = color;
        w->width = size;
        w->height = size;
    }
    return w;
}

SadWidget* sad_image(const char* path) {
    SadWidget* w = create_widget(SADWIDGET_IMAGE);
    if (w && path) {
        w->imageData.src = path;
    }
    return w;
}

SadWidget* sad_button(const char* label, SadTapCallback onTap, void* userData) {
    SadWidget* w = create_widget(SADWIDGET_BUTTON);
    if (w) {
        if (label) w->buttonData.label = strdup(label);
        w->buttonData.labelStyle = SADTEXT_BUTTON;
        w->buttonData.buttonColor = SADCOLOR_PRIMARY;
        w->buttonData.isElevated = true;
        w->buttonData.elevation = 2;
        w->onTap = onTap;
        w->userData = userData;
        w->height = 48;
    }
    return w;
}

SadWidget* sad_elevated_button(const char* label, SadColor color, SadTapCallback onTap, void* userData) {
    SadWidget* w = sad_button(label, onTap, userData);
    if (w) {
        w->buttonData.buttonColor = color;
        w->buttonData.isElevated = true;
    }
    return w;
}

SadWidget* sad_text_button(const char* label, SadTapCallback onTap, void* userData) {
    SadWidget* w = sad_button(label, onTap, userData);
    if (w) {
        w->buttonData.buttonColor = SADCOLOR_TRANSPARENT;
        w->buttonData.labelStyle.color = SADCOLOR_PRIMARY;
        w->buttonData.isElevated = false;
    }
    return w;
}

SadWidget* sad_icon_button(int iconCode, SadTapCallback onTap, void* userData) {
    SadWidget* w = create_widget(SADWIDGET_ICONBUTTON);
    if (w) {
        w->iconData.iconCode = iconCode;
        w->iconData.iconSize = 24;
        w->iconData.iconColor = SADCOLOR_BLACK;
        w->width = 48;
        w->height = 48;
        w->onTap = onTap;
        w->userData = userData;
    }
    return w;
}

SadWidget* sad_fab(int iconCode, SadColor color, SadTapCallback onTap, void* userData) {
    SadWidget* w = create_widget(SADWIDGET_FAB);
    if (w) {
        w->iconData.iconCode = iconCode;
        w->iconData.iconSize = 24;
        w->iconData.iconColor = SADCOLOR_WHITE;
        w->width = 56;
        w->height = 56;
        w->decoration.color = color;
        w->decoration.type = SADDECOR_COLOR;
        w->decoration.borderRadius = sadradius_all(28);
        w->onTap = onTap;
        w->userData = userData;
    }
    return w;
}

SadWidget* sad_textfield(const char* label, const char* placeholder) {
    SadWidget* w = create_widget(SADWIDGET_TEXTFIELD);
    if (w) {
        if (label) w->textFieldData.label = strdup(label);
        if (placeholder) w->textFieldData.placeholder = strdup(placeholder);
        w->textFieldData.value = strdup("");
        w->height = 56;
    }
    return w;
}

SadWidget* sad_checkbox(bool checked, SadValueCallback onChanged, void* userData) {
    SadWidget* w = create_widget(SADWIDGET_CHECKBOX);
    if (w) {
        w->toggleData.checked = checked;
        w->toggleData.activeColor = SADCOLOR_PRIMARY;
        w->width = 24;
        w->height = 24;
        w->onChanged = onChanged;
        w->userData = userData;
    }
    return w;
}

SadWidget* sad_switch(bool value, SadValueCallback onChanged, void* userData) {
    SadWidget* w = create_widget(SADWIDGET_SWITCH);
    if (w) {
        w->toggleData.checked = value;
        w->toggleData.activeColor = SADCOLOR_PRIMARY;
        w->width = 52;
        w->height = 32;
        w->onChanged = onChanged;
        w->userData = userData;
    }
    return w;
}

SadWidget* sad_slider(float value, float min, float max, SadValueCallback onChanged, void* userData) {
    SadWidget* w = create_widget(SADWIDGET_SLIDER);
    if (w) {
        w->sliderData.value = value;
        w->sliderData.min = min;
        w->sliderData.max = max;
        w->sliderData.activeColor = SADCOLOR_PRIMARY;
        w->sliderData.trackColor = SADCOLOR_GREY;
        w->height = 48;
        w->onChanged = onChanged;
        w->userData = userData;
    }
    return w;
}

SadWidget* sad_card(void) {
    SadWidget* w = create_widget(SADWIDGET_CARD);
    if (w) {
        w->decoration.color = SADCOLOR_SURFACE;
        w->decoration.type = SADDECOR_COLOR;
        w->decoration.borderRadius = sadradius_all(12);
        w->padding = sadpadding_all(16);
    }
    return w;
}

SadWidget* sad_appbar(const char* title) {
    SadWidget* w = create_widget(SADWIDGET_APPBAR);
    if (w) {
        if (title) w->textData.text = strdup(title);
        w->height = 56;
        w->decoration.color = SADCOLOR_PRIMARY;
        w->decoration.type = SADDECOR_COLOR;
    }
    return w;
}

SadWidget* sad_bottomnav(void) {
    SadWidget* w = create_widget(SADWIDGET_BOTTOMNAV);
    if (w) {
        w->height = 56;
    }
    return w;
}

void sad_bottomnav_add_item(SadWidget* nav, int iconCode, const char* label) {
    // TODO: إضافة عنصر للتنقل
}

SadWidget* sad_scaffold(void) {
    return create_widget(SADWIDGET_SCAFFOLD);
}

SadWidget* sad_listview(void) {
    SadWidget* w = create_widget(SADWIDGET_LISTVIEW);
    if (w) {
        w->layoutData.scrollable = true;
    }
    return w;
}

SadWidget* sad_scrollview(void) {
    SadWidget* w = create_widget(SADWIDGET_SCROLLVIEW);
    if (w) {
        w->layoutData.scrollable = true;
    }
    return w;
}

SadWidget* sad_divider(void) {
    SadWidget* w = create_widget(SADWIDGET_DIVIDER);
    if (w) {
        w->height = 1;
    }
    return w;
}

SadWidget* sad_spacer(float size) {
    SadWidget* w = create_widget(SADWIDGET_SPACER);
    if (w) {
        w->height = size;
        w->width = size;
    }
    return w;
}

SadWidget* sad_progressbar(float value) {
    SadWidget* w = create_widget(SADWIDGET_PROGRESSBAR);
    if (w) {
        w->progressData.progress = value;
        w->progressData.progressColor = SADCOLOR_PRIMARY;
        w->progressData.backgroundColor = sadcolor(200, 200, 200, 255);
        w->height = 4;
    }
    return w;
}

SadWidget* sad_circular_progress(void) {
    SadWidget* w = create_widget(SADWIDGET_CIRCLEPROGRESS);
    if (w) {
        w->progressData.indeterminate = true;
        w->progressData.progressColor = SADCOLOR_PRIMARY;
        w->width = 36;
        w->height = 36;
    }
    return w;
}

SadWidget* sad_avatar(const char* imagePath, float radius) {
    SadWidget* w = create_widget(SADWIDGET_AVATAR);
    if (w) {
        if (imagePath) w->imageData.src = imagePath;
        w->width = radius * 2;
        w->height = radius * 2;
        w->decoration.borderRadius = sadradius_all(radius);
    }
    return w;
}

SadWidget* sad_badge(const char* text, SadColor color) {
    SadWidget* w = create_widget(SADWIDGET_BADGE);
    if (w) {
        if (text) w->textData.text = strdup(text);
        w->decoration.color = color;
        w->decoration.type = SADDECOR_COLOR;
        w->decoration.borderRadius = sadradius_all(10);
        w->padding = sadpadding_symmetric(4, 8);
    }
    return w;
}

SadWidget* sad_chip(const char* label) {
    SadWidget* w = create_widget(SADWIDGET_CHIP);
    if (w) {
        if (label) w->textData.text = strdup(label);
        w->decoration.borderRadius = sadradius_all(16);
        w->decoration.border.width = 1;
        w->decoration.border.color = SADCOLOR_GREY;
        w->padding = sadpadding_symmetric(8, 12);
        w->height = 32;
    }
    return w;
}

SadWidget* sad_canvas(SadDrawCallback onDraw, void* userData) {
    SadWidget* w = create_widget(SADWIDGET_CANVAS);
    if (w) {
        w->canvasData.onDraw = onDraw;
        w->canvasData.drawUserData = userData;
    }
    return w;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تعديل الـ Widgets
// ═══════════════════════════════════════════════════════════════════════════════

void sad_add_child(SadWidget* parent, SadWidget* child) {
    if (!parent || !child) return;
    
    if (parent->childCount >= parent->childCapacity) {
        int newCap = parent->childCapacity ? parent->childCapacity * 2 : 8;
        parent->children = (SadWidget**)realloc(parent->children, sizeof(SadWidget*) * newCap);
        parent->childCapacity = newCap;
    }
    
    parent->children[parent->childCount++] = child;
    child->parent = parent;
}

void sad_add_children(SadWidget* parent, SadWidget** children, int count) {
    for (int i = 0; i < count; i++) {
        sad_add_child(parent, children[i]);
    }
}

void sad_set_size(SadWidget* widget, float width, float height) {
    if (!widget) return;
    widget->width = width;
    widget->height = height;
}

void sad_set_margin(SadWidget* widget, SadEdgeInsets margin) {
    if (!widget) return;
    widget->margin = margin;
}

void sad_set_padding(SadWidget* widget, SadEdgeInsets padding) {
    if (!widget) return;
    widget->padding = padding;
}

void sad_set_background(SadWidget* widget, SadColor color) {
    if (!widget) return;
    widget->decoration.type = SADDECOR_COLOR;
    widget->decoration.color = color;
}

void sad_set_gradient(SadWidget* widget, SadColor color1, SadColor color2, float angle) {
    if (!widget) return;
    widget->decoration.type = SADDECOR_GRADIENT;
    widget->decoration.gradientType = SADGRADIENT_LINEAR;
    widget->decoration.gradientColors[0] = color1;
    widget->decoration.gradientColors[1] = color2;
    widget->decoration.gradientColorCount = 2;
    widget->decoration.gradientAngle = angle;
}

void sad_set_border(SadWidget* widget, float width, SadColor color) {
    if (!widget) return;
    widget->decoration.border.width = width;
    widget->decoration.border.color = color;
}

void sad_set_border_radius(SadWidget* widget, SadBorderRadius radius) {
    if (!widget) return;
    widget->decoration.borderRadius = radius;
}

void sad_set_shadow(SadWidget* widget, SadColor color, float blur, float offsetX, float offsetY) {
    if (!widget) return;
    widget->decoration.hasShadow = true;
    widget->decoration.shadow.color = color;
    widget->decoration.shadow.blurRadius = blur;
    widget->decoration.shadow.offsetX = offsetX;
    widget->decoration.shadow.offsetY = offsetY;
}

void sad_set_opacity(SadWidget* widget, float opacity) {
    if (!widget) return;
    widget->opacity = opacity;
}

void sad_set_alignment(SadWidget* widget, SadAlignment mainAxis, SadAlignment crossAxis) {
    if (!widget) return;
    widget->layoutData.mainAxisAlignment = mainAxis;
    widget->layoutData.crossAxisAlignment = crossAxis;
}

void sad_set_spacing(SadWidget* widget, float spacing) {
    if (!widget) return;
    widget->layoutData.spacing = spacing;
}

void sad_set_flex(SadWidget* widget, float flex) {
    if (!widget) return;
    widget->flex = flex;
}

void sad_set_visible(SadWidget* widget, bool visible) {
    if (!widget) return;
    widget->visible = visible;
}

void sad_set_enabled(SadWidget* widget, bool enabled) {
    if (!widget) return;
    widget->enabled = enabled;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الأحداث
// ═══════════════════════════════════════════════════════════════════════════════

void sad_on_tap(SadWidget* widget, SadTapCallback callback, void* userData) {
    if (!widget) return;
    widget->onTap = callback;
    widget->userData = userData;
}

void sad_on_long_press(SadWidget* widget, SadTapCallback callback, void* userData) {
    if (!widget) return;
    widget->onLongPress = callback;
    widget->userData = userData;
}

void sad_on_changed(SadWidget* widget, SadValueCallback callback, void* userData) {
    if (!widget) return;
    widget->onChanged = callback;
    widget->userData = userData;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الرسم المباشر
// ═══════════════════════════════════════════════════════════════════════════════

void sad_draw_rect(float x, float y, float width, float height, SadColor color) {
    push_quad(x, y, width, height, color);
}

void sad_draw_rounded_rect(float x, float y, float width, float height, 
                           SadBorderRadius radius, SadColor color) {
    push_rounded_rect(x, y, width, height, radius.topLeft, color);
}

void sad_draw_circle(float cx, float cy, float radius, SadColor color) {
    push_circle(cx, cy, radius, color);
}

void sad_draw_line(float x1, float y1, float x2, float y2, float width, SadColor color) {
    // TODO: رسم خط
}

void sad_draw_text(const char* text, float x, float y, SadTextStyle style) {
    // TODO: رسم نص
}

void sad_draw_image(uint32_t textureId, float x, float y, float width, float height) {
    // TODO: رسم صورة
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الحوارات
// ═══════════════════════════════════════════════════════════════════════════════

void sad_show_dialog(SadApp* app, const char* title, const char* message, 
                     SadTapCallback onConfirm, SadTapCallback onCancel, void* userData) {
    // TODO: عرض حوار
}

void sad_show_snackbar(SadApp* app, const char* message, int durationMs) {
    // TODO: عرض رسالة مؤقتة
}

void sad_show_bottom_sheet(SadApp* app, SadWidget* content) {
    // TODO: عرض ورقة سفلية
}

void sad_dismiss(SadApp* app) {
    // TODO: إغلاق الحوار
}
