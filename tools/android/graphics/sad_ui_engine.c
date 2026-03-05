/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ محرك سَد الرسومي
 *  SadUI Engine Implementation
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui_engine.h"
#include "sad_arabic_text.h"

#include <android/native_window.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO  // نستخدم fopen يدوياً
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#include "stb_image.h"

#define LOG_TAG "SadUI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// ═══════════════════════════════════════════════════════════════════════════════
//  هياكل داخلية
// ═══════════════════════════════════════════════════════════════════════════════

typedef struct SadWidget {
    SadWidgetId id;
    SadWidgetType type;
    
    // الخصائص
    char* text;
    float value;
    bool enabled;
    bool visible;
    bool checked;
    
    // التخطيط والنمط
    SadLayout layout;
    SadStyle style;
    
    // المستطيل المحسوب
    SadRect computedRect;
    
    // التسلسل الهرمي
    struct SadWidget* parent;
    struct SadWidget** children;
    int childCount;
    int childCapacity;
    
    // معالجات الأحداث
    SadEventHandler onClick;
    void* onClickData;
    SadGestureHandler onGesture;
    void* onGestureData;
    SadTouchHandler onTouch;
    void* onTouchData;
    SadValueHandler onValueChange;
    void* onValueChangeData;
    SadTextHandler onTextChange;
    void* onTextChangeData;
    
    // حالة التفاعل
    bool hovered;
    bool pressed;
    bool focused;
    
} SadWidget;

typedef struct SadAnimState {
    SadAnimId id;
    SadWidgetId widget;
    SadAnimation config;
    float elapsed;
    float progress;
    bool active;
    bool completed;
} SadAnimState;

typedef struct {
    // EGL
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    ANativeWindow* window;
    
    // الأبعاد
    int screenWidth;
    int screenHeight;
    float density;
    
    // الشيدرز
    GLuint shaderProgram;
    GLuint textShaderProgram;
    GLuint imageShaderProgram;    // شيدر الصور
    
    // الـ VBO و VAO
    GLuint quadVAO;
    GLuint quadVBO;
    
    // Uniforms
    GLint uProjection;
    GLint uColor;
    GLint uBorderRadius;
    GLint uRect;
    GLint uBorderColor;
    GLint uBorderWidth;
    GLint uHasBorder;
    GLint uHasGradient;
    GLint uGradientStart;
    GLint uGradientEnd;
    GLint uGradientAngle;
    
    // المكونات
    SadWidget* widgets[SADUI_MAX_WIDGETS];
    int widgetCount;
    SadWidgetId nextWidgetId;
    SadWidget* rootWidget;
    
    // الرسوم المتحركة
    SadAnimState animations[SADUI_MAX_ANIMATIONS];
    int animationCount;
    SadAnimId nextAnimId;
    
    // الثيم
    SadTheme currentTheme;
    
    // اللمس
    SadWidgetId pressedWidget;
    SadWidgetId focusedWidget;
    float lastTouchX, lastTouchY;
    
    // تحسين الأداء — إعادة الرسم عند الحاجة فقط
    bool needsRedraw;     // يحتاج إعادة رسم
    bool needsLayout;     // يحتاج إعادة حساب التخطيط
    uint32_t frameCount;  // عداد الإطارات
    float fpsTimer;       // مؤقت FPS
    int fps;              // FPS الحالي
    
    // حالة التهيئة
    bool initialized;
    
} SadUIContext;

static SadUIContext g_ctx = {0};

// تصريحات مسبقة
static void render_image_widget(SadWidget* widget);
static void apply_animation_to_widget(SadWidget* w, SadAnimState* anim);
static float apply_easing(float t, SadEasing easing);

// ═══════════════════════════════════════════════════════════════════════════════
//  الشيدرز (Shaders)
// ═══════════════════════════════════════════════════════════════════════════════

static const char* VERTEX_SHADER = 
    "#version 300 es\n"
    "layout(location = 0) in vec2 aPos;\n"
    "layout(location = 1) in vec2 aTexCoord;\n"
    "out vec2 vTexCoord;\n"
    "out vec2 vLocalPos;\n"
    "uniform mat4 uProjection;\n"
    "uniform vec4 uRect;\n"
    "void main() {\n"
    "    vec2 pos = uRect.xy + aPos * uRect.zw;\n"
    "    gl_Position = uProjection * vec4(pos, 0.0, 1.0);\n"
    "    vTexCoord = aTexCoord;\n"
    "    vLocalPos = aPos;\n"
    "}\n";

static const char* FRAGMENT_SHADER = 
    "#version 300 es\n"
    "precision highp float;\n"
    "in vec2 vTexCoord;\n"
    "in vec2 vLocalPos;\n"
    "out vec4 FragColor;\n"
    "\n"
    "uniform vec4 uColor;\n"
    "uniform vec4 uBorderRadius;\n"  // topLeft, topRight, bottomRight, bottomLeft
    "uniform vec4 uRect;\n"
    "uniform vec4 uBorderColor;\n"
    "uniform float uBorderWidth;\n"
    "uniform bool uHasBorder;\n"
    "uniform bool uHasGradient;\n"
    "uniform vec4 uGradientStart;\n"
    "uniform vec4 uGradientEnd;\n"
    "uniform float uGradientAngle;\n"
    "\n"
    "float roundedBoxSDF(vec2 p, vec2 size, vec4 radius) {\n"
    "    vec2 q = abs(p) - size;\n"
    "    float r = 0.0;\n"
    "    if (p.x > 0.0 && p.y > 0.0) r = radius.y;\n"  // topRight
    "    else if (p.x <= 0.0 && p.y > 0.0) r = radius.x;\n"  // topLeft
    "    else if (p.x <= 0.0 && p.y <= 0.0) r = radius.w;\n"  // bottomLeft
    "    else r = radius.z;\n"  // bottomRight
    "    return length(max(q - vec2(r), 0.0)) + min(max(q.x - r, q.y - r), 0.0) - r;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec2 size = uRect.zw * 0.5;\n"
    "    vec2 center = vec2(0.5);\n"
    "    vec2 p = (vLocalPos - center) * uRect.zw;\n"
    "    \n"
    "    float dist = roundedBoxSDF(p, size - vec2(0.5), uBorderRadius);\n"
    "    \n"
    "    vec4 color = uColor;\n"
    "    if (uHasGradient) {\n"
    "        float angle = radians(uGradientAngle);\n"
    "        vec2 dir = vec2(cos(angle), sin(angle));\n"
    "        float t = dot(vLocalPos - 0.5, dir) + 0.5;\n"
    "        color = mix(uGradientStart, uGradientEnd, clamp(t, 0.0, 1.0));\n"
    "    }\n"
    "    \n"
    "    float alpha = 1.0 - smoothstep(-1.0, 1.0, dist);\n"
    "    \n"
    "    if (uHasBorder && uBorderWidth > 0.0) {\n"
    "        float borderDist = abs(dist) - uBorderWidth * 0.5;\n"
    "        float borderAlpha = 1.0 - smoothstep(-1.0, 1.0, borderDist);\n"
    "        color = mix(color, uBorderColor, borderAlpha * step(dist, 0.0));\n"
    "    }\n"
    "    \n"
    "    FragColor = vec4(color.rgb, color.a * alpha);\n"
    "}\n";

// ═══════════════════════════════════════════════════════════════════════════════
//  شيدر الصور (Image Shader)
// ═══════════════════════════════════════════════════════════════════════════════

static const char* IMAGE_VERTEX_SHADER =
    "#version 300 es\n"
    "layout(location = 0) in vec2 aPos;\n"
    "layout(location = 1) in vec2 aTexCoord;\n"
    "out vec2 vTexCoord;\n"
    "uniform mat4 uProjection;\n"
    "uniform vec4 uRect;\n"
    "void main() {\n"
    "    vec2 pos = uRect.xy + aPos * uRect.zw;\n"
    "    gl_Position = uProjection * vec4(pos, 0.0, 1.0);\n"
    "    vTexCoord = aTexCoord;\n"
    "}\n";

static const char* IMAGE_FRAGMENT_SHADER =
    "#version 300 es\n"
    "precision highp float;\n"
    "in vec2 vTexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D uTexture;\n"
    "uniform float uOpacity;\n"
    "uniform vec4 uBorderRadius;\n"
    "uniform vec4 uRect;\n"
    "\n"
    "float roundedBoxSDF(vec2 p, vec2 size, vec4 radius) {\n"
    "    vec2 q = abs(p) - size;\n"
    "    float r = 0.0;\n"
    "    if (p.x > 0.0 && p.y > 0.0) r = radius.y;\n"
    "    else if (p.x <= 0.0 && p.y > 0.0) r = radius.x;\n"
    "    else if (p.x <= 0.0 && p.y <= 0.0) r = radius.w;\n"
    "    else r = radius.z;\n"
    "    return length(max(q - vec2(r), 0.0)) + min(max(q.x - r, q.y - r), 0.0) - r;\n"
    "}\n"
    "\n"
    "void main() {\n"
    "    vec4 texColor = texture(uTexture, vTexCoord);\n"
    "    vec2 size = uRect.zw * 0.5;\n"
    "    vec2 center = vec2(0.5);\n"
    "    vec2 p = (vTexCoord - center) * uRect.zw;\n"
    "    float dist = roundedBoxSDF(p, size - vec2(0.5), uBorderRadius);\n"
    "    float alpha = 1.0 - smoothstep(-1.0, 1.0, dist);\n"
    "    FragColor = vec4(texColor.rgb, texColor.a * alpha * uOpacity);\n"
    "}\n";

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال OpenGL المساعدة
// ═══════════════════════════════════════════════════════════════════════════════

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        LOGE("Shader compilation failed: %s", log);
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static GLuint create_shader_program(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertexSrc);
    GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragmentSrc);
    
    if (!vertex || !fragment) {
        if (vertex) glDeleteShader(vertex);
        if (fragment) glDeleteShader(fragment);
        return 0;
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, NULL, log);
        LOGE("Shader program linking failed: %s", log);
        glDeleteProgram(program);
        program = 0;
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return program;
}

static void setup_quad_geometry(void) {
    float vertices[] = {
        // pos        // texcoord
        0.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 0.0f,   1.0f, 0.0f,
        1.0f, 1.0f,   1.0f, 1.0f,
        0.0f, 0.0f,   0.0f, 0.0f,
        1.0f, 1.0f,   1.0f, 1.0f,
        0.0f, 1.0f,   0.0f, 1.0f,
    };
    
    glGenVertexArrays(1, &g_ctx.quadVAO);
    glGenBuffers(1, &g_ctx.quadVBO);
    
    glBindVertexArray(g_ctx.quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_ctx.quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

static void create_projection_matrix(float* mat, float width, float height) {
    // مصفوفة إسقاط متعامدة
    memset(mat, 0, 16 * sizeof(float));
    mat[0] = 2.0f / width;
    mat[5] = -2.0f / height;  // Y مقلوب
    mat[10] = -1.0f;
    mat[12] = -1.0f;
    mat[13] = 1.0f;
    mat[15] = 1.0f;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إدارة EGL
// ═══════════════════════════════════════════════════════════════════════════════

static bool init_egl(ANativeWindow* window) {
    const EGLint attribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    
    const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    
    g_ctx.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_ctx.display == EGL_NO_DISPLAY) {
        LOGE("Failed to get EGL display");
        return false;
    }
    
    if (!eglInitialize(g_ctx.display, NULL, NULL)) {
        LOGE("Failed to initialize EGL");
        return false;
    }
    
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(g_ctx.display, attribs, &config, 1, &numConfigs) || numConfigs == 0) {
        LOGE("Failed to choose EGL config");
        return false;
    }
    
    EGLint format;
    eglGetConfigAttrib(g_ctx.display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);
    
    g_ctx.surface = eglCreateWindowSurface(g_ctx.display, config, window, NULL);
    if (g_ctx.surface == EGL_NO_SURFACE) {
        LOGE("Failed to create EGL surface");
        return false;
    }
    
    g_ctx.context = eglCreateContext(g_ctx.display, config, EGL_NO_CONTEXT, context_attribs);
    if (g_ctx.context == EGL_NO_CONTEXT) {
        LOGE("Failed to create EGL context");
        return false;
    }
    
    if (!eglMakeCurrent(g_ctx.display, g_ctx.surface, g_ctx.surface, g_ctx.context)) {
        LOGE("Failed to make EGL context current");
        return false;
    }
    
    eglQuerySurface(g_ctx.display, g_ctx.surface, EGL_WIDTH, &g_ctx.screenWidth);
    eglQuerySurface(g_ctx.display, g_ctx.surface, EGL_HEIGHT, &g_ctx.screenHeight);
    
    g_ctx.window = window;
    
    LOGI("EGL initialized: %dx%d", g_ctx.screenWidth, g_ctx.screenHeight);
    return true;
}

static void term_egl(void) {
    if (g_ctx.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(g_ctx.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (g_ctx.context != EGL_NO_CONTEXT) {
            eglDestroyContext(g_ctx.display, g_ctx.context);
        }
        if (g_ctx.surface != EGL_NO_SURFACE) {
            eglDestroySurface(g_ctx.display, g_ctx.surface);
        }
        eglTerminate(g_ctx.display);
    }
    g_ctx.display = EGL_NO_DISPLAY;
    g_ctx.context = EGL_NO_CONTEXT;
    g_ctx.surface = EGL_NO_SURFACE;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الثيمات الافتراضية
// ═══════════════════════════════════════════════════════════════════════════════

static SadTheme g_lightTheme = {
    .primary = 0x6750A4FF,
    .primaryVariant = 0x4F378BFF,
    .secondary = 0x625B71FF,
    .secondaryVariant = 0x4A4458FF,
    .background = 0xFFFBFEFF,
    .surface = 0xFFFBFEFF,
    .error = 0xB3261EFF,
    .onPrimary = 0xFFFFFFFF,
    .onSecondary = 0xFFFFFFFF,
    .onBackground = 0x1C1B1FFF,
    .onSurface = 0x1C1B1FFF,
    .onError = 0xFFFFFFFF,
    .cornerRadiusSmall = 8.0f,
    .cornerRadiusMedium = 16.0f,
    .cornerRadiusLarge = 24.0f,
    .elevationSmall = 2.0f,
    .elevationMedium = 4.0f,
    .elevationLarge = 8.0f,
    .fontSizeSmall = 12.0f,
    .fontSizeMedium = 16.0f,
    .fontSizeLarge = 20.0f,
    .fontSizeHeading = 28.0f,
    .rtl = true
};

static SadTheme g_darkTheme = {
    .primary = 0xD0BCFFFF,
    .primaryVariant = 0xB69DF8FF,
    .secondary = 0xCCC2DCFF,
    .secondaryVariant = 0xB0A7C0FF,
    .background = 0x1C1B1FFF,
    .surface = 0x2D2D30FF,
    .error = 0xF2B8B5FF,
    .onPrimary = 0x381E72FF,
    .onSecondary = 0x332D41FF,
    .onBackground = 0xE6E1E5FF,
    .onSurface = 0xE6E1E5FF,
    .onError = 0x601410FF,
    .cornerRadiusSmall = 8.0f,
    .cornerRadiusMedium = 16.0f,
    .cornerRadiusLarge = 24.0f,
    .elevationSmall = 2.0f,
    .elevationMedium = 4.0f,
    .elevationLarge = 8.0f,
    .fontSizeSmall = 12.0f,
    .fontSizeMedium = 16.0f,
    .fontSizeLarge = 20.0f,
    .fontSizeHeading = 28.0f,
    .rtl = true
};

// ═══════════════════════════════════════════════════════════════════════════════
//  API الرئيسي
// ═══════════════════════════════════════════════════════════════════════════════

bool sadui_init(void* window) {
    if (g_ctx.initialized) {
        LOGD("SadUI already initialized");
        return true;
    }
    
    LOGI("Initializing SadUI Engine v%d.%d", SADUI_VERSION_MAJOR, SADUI_VERSION_MINOR);
    
    // تهيئة EGL
    if (!init_egl((ANativeWindow*)window)) {
        LOGE("Failed to initialize EGL");
        return false;
    }
    
    // تهيئة OpenGL
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // إنشاء الشيدرز
    g_ctx.shaderProgram = create_shader_program(VERTEX_SHADER, FRAGMENT_SHADER);
    if (!g_ctx.shaderProgram) {
        LOGE("Failed to create shader program");
        return false;
    }
    
    // جلب مواقع الـ uniforms
    g_ctx.uProjection = glGetUniformLocation(g_ctx.shaderProgram, "uProjection");
    g_ctx.uColor = glGetUniformLocation(g_ctx.shaderProgram, "uColor");
    g_ctx.uBorderRadius = glGetUniformLocation(g_ctx.shaderProgram, "uBorderRadius");
    g_ctx.uRect = glGetUniformLocation(g_ctx.shaderProgram, "uRect");
    g_ctx.uBorderColor = glGetUniformLocation(g_ctx.shaderProgram, "uBorderColor");
    g_ctx.uBorderWidth = glGetUniformLocation(g_ctx.shaderProgram, "uBorderWidth");
    g_ctx.uHasBorder = glGetUniformLocation(g_ctx.shaderProgram, "uHasBorder");
    g_ctx.uHasGradient = glGetUniformLocation(g_ctx.shaderProgram, "uHasGradient");
    g_ctx.uGradientStart = glGetUniformLocation(g_ctx.shaderProgram, "uGradientStart");
    g_ctx.uGradientEnd = glGetUniformLocation(g_ctx.shaderProgram, "uGradientEnd");
    g_ctx.uGradientAngle = glGetUniformLocation(g_ctx.shaderProgram, "uGradientAngle");
    
    // إنشاء شيدر الصور
    g_ctx.imageShaderProgram = create_shader_program(IMAGE_VERTEX_SHADER, IMAGE_FRAGMENT_SHADER);
    if (!g_ctx.imageShaderProgram) {
        LOGE("Failed to create image shader program");
        // ليست خطأ حرجة — نستمر بدون دعم الصور
    } else {
        LOGI("Image shader created successfully");
    }
    
    // إنشاء الهندسة
    setup_quad_geometry();
    
    // تعيين الثيم الافتراضي
    g_ctx.currentTheme = g_lightTheme;
    
    g_ctx.nextWidgetId = 1;
    g_ctx.nextAnimId = 1;
    g_ctx.initialized = true;
    
    LOGI("SadUI Engine initialized successfully");
    return true;
}

void sadui_shutdown(void) {
    if (!g_ctx.initialized) return;
    
    LOGI("Shutting down SadUI Engine");
    
    // تحرير المكونات
    for (int i = 0; i < g_ctx.widgetCount; i++) {
        if (g_ctx.widgets[i]) {
            if (g_ctx.widgets[i]->text) {
                free(g_ctx.widgets[i]->text);
            }
            if (g_ctx.widgets[i]->children) {
                free(g_ctx.widgets[i]->children);
            }
            free(g_ctx.widgets[i]);
        }
    }
    
    // تحرير OpenGL
    if (g_ctx.quadVAO) glDeleteVertexArrays(1, &g_ctx.quadVAO);
    if (g_ctx.quadVBO) glDeleteBuffers(1, &g_ctx.quadVBO);
    if (g_ctx.shaderProgram) glDeleteProgram(g_ctx.shaderProgram);
    
    term_egl();
    
    memset(&g_ctx, 0, sizeof(g_ctx));
}

void sadui_set_screen_size(int width, int height) {
    g_ctx.screenWidth = width;
    g_ctx.screenHeight = height;
    glViewport(0, 0, width, height);
}

void sadui_get_screen_size(int* width, int* height) {
    if (width) *width = g_ctx.screenWidth;
    if (height) *height = g_ctx.screenHeight;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إدارة المكونات
// ═══════════════════════════════════════════════════════════════════════════════

SadWidgetId sadui_create(SadWidgetType type) {
    if (g_ctx.widgetCount >= SADUI_MAX_WIDGETS) {
        LOGE("Maximum widget count reached");
        return 0;
    }
    
    SadWidget* widget = (SadWidget*)calloc(1, sizeof(SadWidget));
    if (!widget) return 0;
    
    widget->id = g_ctx.nextWidgetId++;
    widget->type = type;
    widget->enabled = true;
    widget->visible = true;
    
    // افتراضيات التخطيط
    widget->layout.mainAxis = SAD_ALIGN_START;
    widget->layout.crossAxis = SAD_ALIGN_START;
    widget->layout.width = (SadSize){SAD_SIZE_WRAP, 0};
    widget->layout.height = (SadSize){SAD_SIZE_WRAP, 0};
    widget->layout.rtl = g_ctx.currentTheme.rtl;
    
    // افتراضيات النمط
    widget->style.opacity = 1.0f;
    widget->style.scaleX = 1.0f;
    widget->style.scaleY = 1.0f;
    widget->style.fontSize = g_ctx.currentTheme.fontSizeMedium;
    
    // تعيين ألوان حسب النوع
    switch (type) {
        case SAD_WIDGET_BUTTON:
            widget->style.backgroundColor = g_ctx.currentTheme.primary;
            widget->style.textColor = g_ctx.currentTheme.onPrimary;
            widget->style.borderRadius = sadui_corners(g_ctx.currentTheme.cornerRadiusMedium);
            widget->layout.padding = sadui_edges(16);
            widget->style.hasShadow = true;
            widget->style.shadow = (SadShadow){0, 2, 6, 0, 0x00000030};
            break;
            
        case SAD_WIDGET_OUTLINED_BUTTON:
            widget->style.backgroundColor = SAD_COLOR_TRANSPARENT;
            widget->style.textColor = g_ctx.currentTheme.primary;
            widget->style.borderColor = g_ctx.currentTheme.primary;
            widget->style.borderWidth = 2;
            widget->style.borderRadius = sadui_corners(g_ctx.currentTheme.cornerRadiusMedium);
            widget->layout.padding = sadui_edges(16);
            break;
            
        case SAD_WIDGET_CARD:
            widget->style.backgroundColor = g_ctx.currentTheme.surface;
            widget->style.borderRadius = sadui_corners(g_ctx.currentTheme.cornerRadiusMedium);
            widget->style.hasShadow = true;
            widget->style.shadow = (SadShadow){0, 4, 8, 0, 0x00000040};
            widget->layout.padding = sadui_edges(16);
            break;
            
        case SAD_WIDGET_FAB:
            widget->style.backgroundColor = 0xEADDFFFF;  // MD3 primary-container
            widget->style.textColor = 0x21005DFF;         // MD3 on-primary-container
            widget->style.borderRadius = sadui_corners(16);
            widget->layout.padding = sadui_edges(16);
            widget->style.hasShadow = true;
            widget->style.shadow = (SadShadow){0, 4, 12, 0, 0x00000050};
            break;
            
        case SAD_WIDGET_CHIP:
            widget->style.backgroundColor = 0xE8DEF8FF;  // MD3 secondary-container
            widget->style.textColor = 0x1D192BFF;         // MD3 on-secondary-container
            widget->style.borderRadius = sadui_corners(8);
            widget->layout.padding = (SadEdges){8, 12, 8, 12};
            widget->style.fontSize = 14;
            break;
            
        case SAD_WIDGET_DIVIDER:
            widget->style.backgroundColor = 0xCAC4D0FF;  // MD3 outline-variant
            widget->layout.width = (SadSize){SAD_SIZE_FILL, 0};
            widget->layout.height = (SadSize){SAD_SIZE_FIXED, 1};
            break;
            
        case SAD_WIDGET_PROGRESS:
            widget->layout.width = (SadSize){SAD_SIZE_FILL, 0};
            widget->layout.height = (SadSize){SAD_SIZE_FIXED, 4};
            widget->value = 0.0f;
            break;
            
        case SAD_WIDGET_TEXT:
        case SAD_WIDGET_PARAGRAPH:
            widget->style.textColor = g_ctx.currentTheme.onSurface;
            break;
            
        case SAD_WIDGET_HEADING:
            widget->style.textColor = g_ctx.currentTheme.onSurface;
            widget->style.fontSize = g_ctx.currentTheme.fontSizeHeading;
            widget->style.fontWeight = 700;
            break;
            
        default:
            widget->style.backgroundColor = SAD_COLOR_TRANSPARENT;
            widget->style.textColor = g_ctx.currentTheme.onSurface;
            break;
    }
    
    g_ctx.widgets[g_ctx.widgetCount++] = widget;
    return widget->id;
}

static SadWidget* get_widget(SadWidgetId id) {
    for (int i = 0; i < g_ctx.widgetCount; i++) {
        if (g_ctx.widgets[i] && g_ctx.widgets[i]->id == id) {
            return g_ctx.widgets[i];
        }
    }
    return NULL;
}

void sadui_destroy(SadWidgetId id) {
    for (int i = 0; i < g_ctx.widgetCount; i++) {
        if (g_ctx.widgets[i] && g_ctx.widgets[i]->id == id) {
            SadWidget* w = g_ctx.widgets[i];
            if (w->text) free(w->text);
            if (w->children) free(w->children);
            free(w);
            
            // ضغط المصفوفة
            for (int j = i; j < g_ctx.widgetCount - 1; j++) {
                g_ctx.widgets[j] = g_ctx.widgets[j + 1];
            }
            g_ctx.widgetCount--;
            return;
        }
    }
}

void sadui_add_child(SadWidgetId parentId, SadWidgetId childId) {
    SadWidget* parent = get_widget(parentId);
    SadWidget* child = get_widget(childId);
    if (!parent || !child) return;
    
    if (parent->childCount >= parent->childCapacity) {
        int newCap = parent->childCapacity == 0 ? 8 : parent->childCapacity * 2;
        parent->children = realloc(parent->children, newCap * sizeof(SadWidget*));
        parent->childCapacity = newCap;
    }
    
    parent->children[parent->childCount++] = child;
    child->parent = parent;
    sadui_invalidate_layout();  // إضافة عنصر تتطلب إعادة حساب التخطيط
}

void sadui_set_root(SadWidgetId id) {
    g_ctx.rootWidget = get_widget(id);
}

void sadui_set_text(SadWidgetId id, const char* text) {
    SadWidget* w = get_widget(id);
    if (!w) return;
    
    if (w->text) free(w->text);
    w->text = text ? strdup(text) : NULL;
    sadui_invalidate();  // تغيير النص يتطلب إعادة رسم
}

const char* sadui_get_text(SadWidgetId id) {
    SadWidget* w = get_widget(id);
    return w ? w->text : NULL;
}

void sadui_set_layout(SadWidgetId id, const SadLayout* layout) {
    SadWidget* w = get_widget(id);
    if (w && layout) w->layout = *layout;
}

void sadui_set_style(SadWidgetId id, const SadStyle* style) {
    SadWidget* w = get_widget(id);
    if (w && style) {
        w->style = *style;
        sadui_invalidate();  // تغيير الأنماط يتطلب إعادة رسم
    }
}

void sadui_set_value(SadWidgetId id, float value) {
    SadWidget* w = get_widget(id);
    if (w) {
        w->value = value;
        sadui_invalidate();  // تغيير القيمة يتطلب إعادة رسم
    }
}

float sadui_get_value(SadWidgetId id) {
    SadWidget* w = get_widget(id);
    return w ? w->value : 0;
}

void sadui_set_enabled(SadWidgetId id, bool enabled) {
    SadWidget* w = get_widget(id);
    if (w) w->enabled = enabled;
}

void sadui_set_visible(SadWidgetId id, bool visible) {
    SadWidget* w = get_widget(id);
    if (w) {
        w->visible = visible;
        sadui_invalidate_layout();  // تغيير الرؤية يتطلب إعادة حساب التخطيط
    }
}

void sadui_set_checked(SadWidgetId id, bool checked) {
    SadWidget* w = get_widget(id);
    if (w) w->checked = checked;
}

bool sadui_get_checked(SadWidgetId id) {
    SadWidget* w = get_widget(id);
    return w ? w->checked : false;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  معالجات الأحداث
// ═══════════════════════════════════════════════════════════════════════════════

void sadui_on_click(SadWidgetId id, SadEventHandler handler, void* userData) {
    SadWidget* w = get_widget(id);
    if (w) {
        w->onClick = handler;
        w->onClickData = userData;
    }
}

void sadui_on_gesture(SadWidgetId id, SadGestureHandler handler, void* userData) {
    SadWidget* w = get_widget(id);
    if (w) {
        w->onGesture = handler;
        w->onGestureData = userData;
    }
}

void sadui_on_touch(SadWidgetId id, SadTouchHandler handler, void* userData) {
    SadWidget* w = get_widget(id);
    if (w) {
        w->onTouch = handler;
        w->onTouchData = userData;
    }
}

void sadui_on_value_change(SadWidgetId id, SadValueHandler handler, void* userData) {
    SadWidget* w = get_widget(id);
    if (w) {
        w->onValueChange = handler;
        w->onValueChangeData = userData;
    }
}

void sadui_on_text_change(SadWidgetId id, SadTextHandler handler, void* userData) {
    SadWidget* w = get_widget(id);
    if (w) {
        w->onTextChange = handler;
        w->onTextChangeData = userData;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الثيمات
// ═══════════════════════════════════════════════════════════════════════════════

void sadui_set_theme(const SadTheme* theme) {
    if (theme) g_ctx.currentTheme = *theme;
}

const SadTheme* sadui_get_theme(void) {
    return &g_ctx.currentTheme;
}

const SadTheme* sadui_theme_light(void) {
    return &g_lightTheme;
}

const SadTheme* sadui_theme_dark(void) {
    return &g_darkTheme;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  تحسين الأداء — نظام Dirty Flags
// ═══════════════════════════════════════════════════════════════════════════════

void sadui_invalidate(void) {
    g_ctx.needsRedraw = true;
}

void sadui_invalidate_widget(SadWidgetId widget) {
    (void)widget; // TODO: تتبع العناصر المتغيرة فقط
    g_ctx.needsRedraw = true;
}

void sadui_invalidate_layout(void) {
    g_ctx.needsLayout = true;
    g_ctx.needsRedraw = true;
}

bool sadui_needs_redraw(void) {
    return g_ctx.needsRedraw;
}

bool sadui_needs_layout(void) {
    return g_ctx.needsLayout;
}

int sadui_get_fps(void) {
    return g_ctx.fps;
}

uint32_t sadui_get_frame_count(void) {
    return g_ctx.frameCount;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الرسم
// ═══════════════════════════════════════════════════════════════════════════════

static void color_to_vec4(SadColor color, float* out) {
    out[0] = ((color >> 24) & 0xFF) / 255.0f;
    out[1] = ((color >> 16) & 0xFF) / 255.0f;
    out[2] = ((color >> 8) & 0xFF) / 255.0f;
    out[3] = (color & 0xFF) / 255.0f;
}

static void render_rect(SadRect rect, const SadStyle* style) {
    glUseProgram(g_ctx.shaderProgram);
    
    // مصفوفة الإسقاط
    float projection[16];
    create_projection_matrix(projection, (float)g_ctx.screenWidth, (float)g_ctx.screenHeight);
    glUniformMatrix4fv(g_ctx.uProjection, 1, GL_FALSE, projection);
    
    // رسم الظل (طبقات متعددة للتأثير الناعم)
    if (style->hasShadow && style->shadow.blur > 0) {
        glUniform1i(g_ctx.uHasGradient, 0);
        glUniform1i(g_ctx.uHasBorder, 0);
        
        // 3 طبقات ظل بأحجام متزايدة وشفافية متناقصة
        float blur = style->shadow.blur;
        float shadowColor[4];
        color_to_vec4(style->shadow.color, shadowColor);
        
        for (int layer = 2; layer >= 0; layer--) {
            float expand = blur * (0.3f + layer * 0.35f);
            float alpha = shadowColor[3] * (0.15f / (layer + 1));
            
            SadRect shadowRect = {
                rect.x + style->shadow.offsetX - expand,
                rect.y + style->shadow.offsetY - expand,
                rect.width + expand * 2,
                rect.height + expand * 2
            };
            
            glUniform4f(g_ctx.uRect, shadowRect.x, shadowRect.y, shadowRect.width, shadowRect.height);
            float sc[4] = {shadowColor[0], shadowColor[1], shadowColor[2], alpha};
            glUniform4fv(g_ctx.uColor, 1, sc);
            
            // زوايا مستديرة للظل أكبر قليلاً
            float sr = style->borderRadius.topLeft + expand * 0.5f;
            glUniform4f(g_ctx.uBorderRadius, sr, sr, sr, sr);
            
            glBindVertexArray(g_ctx.quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    
    // المستطيل الأصلي
    glUniform4f(g_ctx.uRect, rect.x, rect.y, rect.width, rect.height);
    
    // اللون
    float color[4];
    color_to_vec4(style->backgroundColor, color);
    color[3] *= style->opacity;
    glUniform4fv(g_ctx.uColor, 1, color);
    
    // الزوايا المستديرة
    glUniform4f(g_ctx.uBorderRadius, 
        style->borderRadius.topLeft,
        style->borderRadius.topRight,
        style->borderRadius.bottomRight,
        style->borderRadius.bottomLeft);
    
    // الحدود
    glUniform1i(g_ctx.uHasBorder, style->borderWidth > 0);
    if (style->borderWidth > 0) {
        float borderColor[4];
        color_to_vec4(style->borderColor, borderColor);
        glUniform4fv(g_ctx.uBorderColor, 1, borderColor);
        glUniform1f(g_ctx.uBorderWidth, style->borderWidth);
    }
    
    // التدرج
    if (style->gradient) {
        glUniform1i(g_ctx.uHasGradient, 1);
        float startColor[4], endColor[4];
        color_to_vec4(style->gradient->start, startColor);
        color_to_vec4(style->gradient->end, endColor);
        glUniform4fv(g_ctx.uGradientStart, 1, startColor);
        glUniform4fv(g_ctx.uGradientEnd, 1, endColor);
        glUniform1f(g_ctx.uGradientAngle, style->gradient->angle);
    } else {
        glUniform1i(g_ctx.uHasGradient, 0);
    }
    
    // الرسم
    glBindVertexArray(g_ctx.quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

static void compute_layout(SadWidget* widget, SadRect available) {
    if (!widget || !widget->visible) return;
    
    // حساب الحجم
    float width = available.width;
    float height = available.height;
    
    // كثافة الشاشة لحسابات WRAP
    float density = g_ctx.screenWidth / 411.0f;
    if (density < 1.0f) density = 1.0f;
    
    switch (widget->layout.width.mode) {
        case SAD_SIZE_FIXED:
            width = widget->layout.width.value;
            break;
        case SAD_SIZE_PERCENT:
            width = available.width * widget->layout.width.value / 100.0f;
            break;
        case SAD_SIZE_FILL:
            width = available.width;
            break;
        default: {
            // WRAP: حساب الحجم بناء على المحتوى
            if (widget->text && widget->text[0] != '\0') {
                SadFontHandle defaultFont = sad_font_get_default();
                if (defaultFont) {
                    SadTextOptions opts = {0};
                    float baseFontSize = widget->style.fontSize > 0 ? widget->style.fontSize : 16.0f;
                    opts.fontSize = baseFontSize * density;
                    opts.enableShaping = true;
                    SadTextMetrics m = sad_text_measure(defaultFont, widget->text, &opts, 0);
                    width = m.width + widget->layout.padding.left + widget->layout.padding.right + 16;
                }
            } else {
                width = available.width;  // حاويات WRAP تأخذ عرض الأب
            }
            break;
        }
    }
    
    // ارتفاع مبدئي (سيتم تحديثه لـ WRAP بعد حساب الأبناء)
    bool heightIsWrap = false;
    switch (widget->layout.height.mode) {
        case SAD_SIZE_FIXED:
            height = widget->layout.height.value;
            break;
        case SAD_SIZE_PERCENT:
            height = available.height * widget->layout.height.value / 100.0f;
            break;
        case SAD_SIZE_FILL:
            height = available.height;
            break;
        default: {
            if (widget->text && widget->text[0] != '\0') {
                SadFontHandle defaultFont = sad_font_get_default();
                if (defaultFont) {
                    SadTextOptions opts = {0};
                    float baseFontSize = widget->style.fontSize > 0 ? widget->style.fontSize : 16.0f;
                    opts.fontSize = baseFontSize * density;
                    opts.enableShaping = true;
                    SadTextMetrics m = sad_text_measure(defaultFont, widget->text, &opts, 0);
                    height = m.height + widget->layout.padding.top + widget->layout.padding.bottom + 8;
                }
            } else if (widget->childCount > 0) {
                heightIsWrap = true;
                height = available.height;  // مؤقت، سيُحدّث بعد حساب الأبناء
            } else {
                height = 60 * density;
            }
            break;
        }
    }
    
    widget->computedRect = (SadRect){
        available.x + widget->layout.margin.left,
        available.y + widget->layout.margin.top,
        width - widget->layout.margin.left - widget->layout.margin.right,
        height - widget->layout.margin.top - widget->layout.margin.bottom
    };
    
    // حساب تخطيط الأبناء
    if (widget->childCount > 0) {
        SadRect childArea = {
            widget->computedRect.x + widget->layout.padding.left,
            widget->computedRect.y + widget->layout.padding.top,
            widget->computedRect.width - widget->layout.padding.left - widget->layout.padding.right,
            widget->computedRect.height - widget->layout.padding.top - widget->layout.padding.bottom
        };
        
        float offset = 0;
        for (int i = 0; i < widget->childCount; i++) {
            SadWidget* child = widget->children[i];
            SadRect childAvail;
            
            if (widget->type == SAD_WIDGET_COLUMN) {
                childAvail = (SadRect){
                    childArea.x,
                    childArea.y + offset,
                    childArea.width,
                    childArea.height - offset
                };
                compute_layout(child, childAvail);
                offset += child->computedRect.height + widget->layout.gap;
            } else if (widget->type == SAD_WIDGET_ROW) {
                childAvail = (SadRect){
                    childArea.x + offset,
                    childArea.y,
                    childArea.width - offset,
                    childArea.height
                };
                compute_layout(child, childAvail);
                offset += child->computedRect.width + widget->layout.gap;
            } else {
                // حاويات أخرى (Card, Container...) — رص عمودي افتراضي
                childAvail = (SadRect){
                    childArea.x,
                    childArea.y + offset,
                    childArea.width,
                    childArea.height - offset
                };
                compute_layout(child, childAvail);
                offset += child->computedRect.height + widget->layout.gap;
            }
        }
        
        // تحديث ارتفاع WRAP بعد حساب جميع الأبناء
        if (heightIsWrap && offset > 0) {
            float totalChildHeight = offset - widget->layout.gap; // طرح آخر gap
            float newHeight = totalChildHeight + widget->layout.padding.top + widget->layout.padding.bottom;
            widget->computedRect.height = newHeight;
        }
        
        // محاذاة المحور الثانوي (crossAxis) — لجميع أنواع الحاويات
        if (widget->layout.crossAxis == SAD_ALIGN_CENTER) {
            for (int i = 0; i < widget->childCount; i++) {
                SadWidget* child = widget->children[i];
                // Column والحاويات الأخرى: محاذاة أفقية مركزية
                if (widget->type != SAD_WIDGET_ROW) {
                    float childW = child->computedRect.width;
                    float parentW = widget->computedRect.width - widget->layout.padding.left - widget->layout.padding.right;
                    if (childW < parentW) {
                        child->computedRect.x = widget->computedRect.x + widget->layout.padding.left + (parentW - childW) / 2.0f;
                    }
                } else {
                    // Row: محاذاة عمودية مركزية
                    float childH = child->computedRect.height;
                    float parentH = widget->computedRect.height - widget->layout.padding.top - widget->layout.padding.bottom;
                    if (childH < parentH) {
                        child->computedRect.y = widget->computedRect.y + widget->layout.padding.top + (parentH - childH) / 2.0f;
                    }
                }
            }
        }
    }
}

static void render_widget(SadWidget* widget) {
    if (!widget || !widget->visible) return;
    
    // تطبيق الرسوم المتحركة على المكون
    for (int i = 0; i < g_ctx.animationCount; i++) {
        SadAnimState* anim = &g_ctx.animations[i];
        if (anim->active && anim->widget == widget->id) {
            apply_animation_to_widget(widget, anim);
        }
    }
    
    // رسم الخلفية مع تأثير الضغط
    if ((widget->style.backgroundColor & 0xFF) != 0 || widget->style.borderWidth > 0) {
        SadStyle drawStyle = widget->style;
        
        // تأثير بصري عند الضغط (تعتيم اللون + رفع الظل)
        if (widget->pressed && widget->enabled) {
            uint32_t bg = drawStyle.backgroundColor;
            uint8_t r = (bg >> 24) & 0xFF;
            uint8_t g = (bg >> 16) & 0xFF;
            uint8_t b = (bg >> 8) & 0xFF;
            uint8_t a = bg & 0xFF;
            r = (uint8_t)(r * 0.8f);
            g = (uint8_t)(g * 0.8f);
            b = (uint8_t)(b * 0.8f);
            drawStyle.backgroundColor = (r << 24) | (g << 16) | (b << 8) | a;
            
            // رفع الظل عند الضغط
            if (drawStyle.hasShadow) {
                drawStyle.shadow.blur *= 1.5f;
                drawStyle.shadow.offsetY *= 1.5f;
            }
        }
        
        render_rect(widget->computedRect, &drawStyle);
    }
    
    // رسم الصورة لمكونات الصورة
    if (widget->type == SAD_WIDGET_IMAGE || widget->type == SAD_WIDGET_AVATAR) {
        render_image_widget(widget);
    }
    
    // رسم النص للمكونات النصية
    if (widget->text && widget->text[0] != '\0') {
        SadFontHandle defaultFont = sad_font_get_default();
        if (defaultFont) {
            // حساب كثافة الشاشة لتكبير الخط بشكل مناسب
            float density = g_ctx.screenWidth / 411.0f;
            if (density < 1.0f) density = 1.0f;
            
            SadTextOptions opts = {0};
            float baseFontSize = widget->style.fontSize > 0 ? widget->style.fontSize : 16.0f;
            opts.fontSize = baseFontSize * density;
            opts.textColor = widget->style.textColor;
            opts.rtl = widget->layout.rtl;
            opts.enableShaping = true;
            opts.enableKerning = true;
            opts.lineSpacing = 1.2f;
            
            float textX = widget->computedRect.x + widget->layout.padding.left;
            float textY = widget->computedRect.y + widget->layout.padding.top;
            
            sad_text_draw(defaultFont, widget->text, textX, textY, &opts);
        }
    }
    
    // رسم عناصر Material Design الخاصة
    if (widget->type == SAD_WIDGET_DIVIDER) {
        float density = g_ctx.screenWidth / 411.0f;
        if (density < 1.0f) density = 1.0f;
        SadRect divRect = widget->computedRect;
        divRect.height = 1.0f * density;  // خط رفيع بكثافة الشاشة
        SadStyle divStyle = {0};
        divStyle.backgroundColor = 0xCAC4D0FF; // MD3 outline-variant
        divStyle.opacity = 1.0f;
        render_rect(divRect, &divStyle);
    }
    
    if (widget->type == SAD_WIDGET_PROGRESS && widget->value >= 0) {
        float density = g_ctx.screenWidth / 411.0f;
        if (density < 1.0f) density = 1.0f;
        // خلفية شريط التقدم
        SadRect bgRect = widget->computedRect;
        bgRect.height = 4.0f * density;
        SadStyle bgStyle = {0};
        bgStyle.backgroundColor = 0xE8DEF8FF;  // MD3 secondary-container
        bgStyle.opacity = 1.0f;
        bgStyle.borderRadius = sadui_corners(2.0f * density);
        render_rect(bgRect, &bgStyle);
        // الجزء المملوء
        SadRect fillRect = bgRect;
        float progress = widget->value > 1.0f ? 1.0f : widget->value;
        fillRect.width = bgRect.width * progress;
        SadStyle fillStyle = {0};
        fillStyle.backgroundColor = g_ctx.currentTheme.primary;
        fillStyle.opacity = 1.0f;
        fillStyle.borderRadius = sadui_corners(2.0f * density);
        render_rect(fillRect, &fillStyle);
    }
    
    // رسم الأبناء
    for (int i = 0; i < widget->childCount; i++) {
        render_widget(widget->children[i]);
    }
}

void sadui_render(float deltaTime) {
    if (!g_ctx.initialized || !g_ctx.rootWidget) return;
    
    // تحديث عداد FPS
    g_ctx.fpsTimer += deltaTime;
    g_ctx.frameCount++;
    if (g_ctx.fpsTimer >= 1.0f) {
        g_ctx.fps = g_ctx.frameCount;
        g_ctx.frameCount = 0;
        g_ctx.fpsTimer = 0.0f;
    }
    
    // تحديث الرسوم المتحركة — يُلغي الصحة تلقائيًا
    bool hasActiveAnimations = false;
    for (int i = 0; i < g_ctx.animationCount; i++) {
        SadAnimState* anim = &g_ctx.animations[i];
        if (!anim->active) continue;
        
        hasActiveAnimations = true;
        anim->elapsed += deltaTime * 1000;
        if (anim->elapsed < anim->config.delay) continue;
        
        float t = (anim->elapsed - anim->config.delay) / anim->config.duration;
        if (t >= 1.0f) {
            if (anim->config.loop) {
                anim->elapsed = 0;
                t = anim->config.reverse ? 1.0f : 0.0f;
            } else {
                anim->completed = true;
                anim->active = false;
                t = 1.0f;
            }
        }
        anim->progress = t;
    }
    
    // الرسوم المتحركة تتطلب إعادة رسم مستمرة
    if (hasActiveAnimations) {
        g_ctx.needsRedraw = true;
    }
    
    // تحسين: لا ترسم إذا لم تتغير الواجهة
    // (معطّل مؤقتًا لضمان التوافقية — فعّل بحذر)
    // if (!g_ctx.needsRedraw && !hasActiveAnimations) return;
    
    // حساب التخطيط (فقط إذا تغير)
    if (g_ctx.needsLayout) {
        SadRect screen = {0, 0, (float)g_ctx.screenWidth, (float)g_ctx.screenHeight};
        compute_layout(g_ctx.rootWidget, screen);
        g_ctx.needsLayout = false;
    } else {
        // لا زلنا بحاجة لحساب التخطيط دائمًا حاليًا
        SadRect screen = {0, 0, (float)g_ctx.screenWidth, (float)g_ctx.screenHeight};
        compute_layout(g_ctx.rootWidget, screen);
    }
    
    // مسح الشاشة
    float bgColor[4];
    color_to_vec4(g_ctx.currentTheme.background, bgColor);
    glClearColor(bgColor[0], bgColor[1], bgColor[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // رسم شجرة المكونات
    render_widget(g_ctx.rootWidget);
    
    // إعادة تعيين علم الرسم
    g_ctx.needsRedraw = false;
    
    // تبديل البفرز
    eglSwapBuffers(g_ctx.display, g_ctx.surface);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  معالجة اللمس
// ═══════════════════════════════════════════════════════════════════════════════

/** هل المكون تفاعلي؟ */
static bool widget_is_interactive(SadWidget* w) {
    if (!w) return false;
    if (w->onClick || w->onGesture || w->onTouch) return true;
    switch (w->type) {
        case SAD_WIDGET_BUTTON:
        case SAD_WIDGET_ICON_BUTTON:
        case SAD_WIDGET_FAB:
        case SAD_WIDGET_OUTLINED_BUTTON:
        case SAD_WIDGET_CHECKBOX:
        case SAD_WIDGET_SWITCH:
        case SAD_WIDGET_RADIO:
        case SAD_WIDGET_SLIDER:
        case SAD_WIDGET_TEXT_FIELD:
        case SAD_WIDGET_PASSWORD:
        case SAD_WIDGET_TEXTAREA:
        case SAD_WIDGET_DROPDOWN:
            return true;
        default:
            return false;
    }
}

static SadWidget* hit_test(SadWidget* widget, float x, float y) {
    if (!widget || !widget->visible) return NULL;
    
    // فحص الحدود أولاً — لا نتعمّق إذا النقطة خارج المكون
    SadRect r = widget->computedRect;
    if (x < r.x || x >= r.x + r.width || y < r.y || y >= r.y + r.height) {
        return NULL;
    }
    
    // فحص الأبناء (من الأمام للخلف)
    for (int i = widget->childCount - 1; i >= 0; i--) {
        SadWidget* hit = hit_test(widget->children[i], x, y);
        if (hit) return hit;
    }
    
    // إرجاع هذا المكون فقط إذا كان تفاعلياً
    if (widget_is_interactive(widget)) {
        return widget;
    }
    
    return NULL;
}

/** بحث عن أعمق مكون (تفاعلي أو لا) تحت النقطة */
static SadWidget* hit_test_any(SadWidget* widget, float x, float y) {
    if (!widget || !widget->visible) return NULL;
    SadRect r = widget->computedRect;
    if (x < r.x || x >= r.x + r.width || y < r.y || y >= r.y + r.height)
        return NULL;
    for (int i = widget->childCount - 1; i >= 0; i--) {
        SadWidget* hit = hit_test_any(widget->children[i], x, y);
        if (hit) return hit;
    }
    return widget;
}

void sadui_handle_touch(SadTouchEvent* event) {
    if (!g_ctx.initialized || !event) return;
    
    SadWidget* target = hit_test(g_ctx.rootWidget, event->x, event->y);
    
    switch (event->action) {
        case SAD_TOUCH_DOWN: {
            SadWidget* handler = target;
            
            // إذا لم نجد مكوناً تفاعلياً، ابحث عن أي مكون ثم اصعد للوالد التفاعلي
            if (!handler) {
                SadWidget* leaf = hit_test_any(g_ctx.rootWidget, event->x, event->y);
                if (leaf) {
                    LOGI("لمس: hit_test_any وجد مكون %u نوع=%d نص='%s' rect=(%.0f,%.0f,%.0f,%.0f)",
                         leaf->id, leaf->type, leaf->text ? leaf->text : "?",
                         leaf->computedRect.x, leaf->computedRect.y, 
                         leaf->computedRect.width, leaf->computedRect.height);
                    while (leaf && !widget_is_interactive(leaf)) {
                        leaf = leaf->parent;
                    }
                    handler = leaf;
                }
            }
            
            if (handler && handler->enabled) {
                handler->pressed = true;
                g_ctx.pressedWidget = handler->id;
                g_ctx.lastTouchX = event->x;
                g_ctx.lastTouchY = event->y;
                LOGI("لمس: DOWN على مكون %u نوع=%d نص='%s' rect=(%.0f,%.0f,%.0f,%.0f)",
                     handler->id, handler->type,
                     handler->text ? handler->text : "?",
                     handler->computedRect.x, handler->computedRect.y,
                     handler->computedRect.width, handler->computedRect.height);
                
                if (handler->onTouch) {
                    handler->onTouch(handler->id, event, handler->onTouchData);
                }
            } else {
                LOGI("لمس: DOWN لا يوجد مكون تفاعلي عند (%.0f,%.0f)", event->x, event->y);
                // طباعة جميع المكونات التفاعلية ومواقعها للتصحيح
                for (int i = 0; i < g_ctx.widgetCount; i++) {
                    SadWidget* w = g_ctx.widgets[i];
                    if (w && widget_is_interactive(w)) {
                        LOGI("  مكون تفاعلي: id=%u نوع=%d نص='%s' rect=(%.0f,%.0f,%.0f,%.0f) onClick=%s",
                             w->id, w->type, w->text ? w->text : "?",
                             w->computedRect.x, w->computedRect.y,
                             w->computedRect.width, w->computedRect.height,
                             w->onClick ? "نعم" : "لا");
                    }
                }
            }
            break;
        }
            
        case SAD_TOUCH_UP:
            if (g_ctx.pressedWidget) {
                SadWidget* pressed = get_widget(g_ctx.pressedWidget);
                if (pressed) {
                    pressed->pressed = false;
                    
                    // إذا كان اللمس لا يزال فوق المكون، فهو نقرة
                    if (pressed->onClick) {
                        SadRect r = pressed->computedRect;
                        if (event->x >= r.x && event->x < r.x + r.width &&
                            event->y >= r.y && event->y < r.y + r.height) {
                            LOGI("نقرة! تنفيذ onClick لمكون %u نص='%s'",
                                 pressed->id, pressed->text ? pressed->text : "?");
                            pressed->onClick(pressed->id, pressed->onClickData);
                        }
                    }
                    
                    if (pressed->onTouch) {
                        pressed->onTouch(pressed->id, event, pressed->onTouchData);
                    }
                }
                g_ctx.pressedWidget = 0;
            }
            break;
            
        case SAD_TOUCH_MOVE:
            if (g_ctx.pressedWidget) {
                SadWidget* pressed = get_widget(g_ctx.pressedWidget);
                if (pressed && pressed->onTouch) {
                    pressed->onTouch(pressed->id, event, pressed->onTouchData);
                }
            }
            break;
            
        case SAD_TOUCH_CANCEL:
            if (g_ctx.pressedWidget) {
                SadWidget* pressed = get_widget(g_ctx.pressedWidget);
                if (pressed) {
                    pressed->pressed = false;
                }
                g_ctx.pressedWidget = 0;
            }
            break;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  الرسوم المتحركة
// ═══════════════════════════════════════════════════════════════════════════════

// دوال Easing
static float apply_easing(float t, SadEasing easing) {
    switch (easing) {
        case SAD_EASE_LINEAR:
            return t;
        case SAD_EASE_IN:
            return t * t;
        case SAD_EASE_OUT:
            return t * (2.0f - t);
        case SAD_EASE_IN_OUT:
            return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
        case SAD_EASE_ELASTIC: {
            if (t == 0 || t == 1) return t;
            return powf(2.0f, -10.0f * t) * sinf((t * 10.0f - 0.75f) * (float)(2.0 * M_PI / 3.0)) + 1.0f;
        }
        case SAD_EASE_BOUNCE: {
            if (t < 1.0f / 2.75f) {
                return 7.5625f * t * t;
            } else if (t < 2.0f / 2.75f) {
                float t2 = t - 1.5f / 2.75f;
                return 7.5625f * t2 * t2 + 0.75f;
            } else if (t < 2.5f / 2.75f) {
                float t2 = t - 2.25f / 2.75f;
                return 7.5625f * t2 * t2 + 0.9375f;
            } else {
                float t2 = t - 2.625f / 2.75f;
                return 7.5625f * t2 * t2 + 0.984375f;
            }
        }
        case SAD_EASE_BACK: {
            float c1 = 1.70158f;
            float c3 = c1 + 1.0f;
            return 1.0f + c3 * powf(t - 1.0f, 3.0f) + c1 * powf(t - 1.0f, 2.0f);
        }
        case SAD_EASE_SPRING: {
            float decay = 4.0f;
            float frequency = 6.0f;
            return 1.0f - expf(-decay * t) * cosf(frequency * t * (float)M_PI);
        }
        default:
            return t;
    }
}

// تطبيق الرسوم المتحركة على المكون
static void apply_animation_to_widget(SadWidget* w, SadAnimState* anim) {
    if (!w || !anim || !anim->active) return;
    
    float progress = apply_easing(anim->progress, anim->config.easing);
    
    switch (anim->config.type) {
        case SAD_ANIM_FADE:
            w->style.opacity = progress;
            break;
            
        case SAD_ANIM_SLIDE_UP:
            w->computedRect.y -= (1.0f - progress) * 50.0f;
            break;
            
        case SAD_ANIM_SLIDE_DOWN:
            w->computedRect.y += (1.0f - progress) * 50.0f;
            break;
            
        case SAD_ANIM_SLIDE_LEFT:
            w->computedRect.x -= (1.0f - progress) * 50.0f;
            break;
            
        case SAD_ANIM_SLIDE_RIGHT:
            w->computedRect.x += (1.0f - progress) * 50.0f;
            break;
            
        case SAD_ANIM_SCALE:
            // يُطبّق في الرسم
            break;
            
        case SAD_ANIM_ROTATE:
            // TODO: يحتاج shader rotation
            break;
            
        case SAD_ANIM_BOUNCE:
            w->computedRect.y -= sinf(progress * (float)M_PI) * 20.0f;
            break;
            
        case SAD_ANIM_SHAKE: {
            float shake = sinf(progress * 20.0f * (float)M_PI) * (1.0f - progress) * 10.0f;
            w->computedRect.x += shake;
            break;
        }
            
        case SAD_ANIM_PULSE: {
            float scale = 1.0f + sinf(progress * (float)M_PI) * 0.1f;
            float dx = w->computedRect.width * (1.0f - scale) * 0.5f;
            float dy = w->computedRect.height * (1.0f - scale) * 0.5f;
            w->computedRect.x += dx;
            w->computedRect.y += dy;
            w->computedRect.width *= scale;
            w->computedRect.height *= scale;
            break;
        }
            
        case SAD_ANIM_CUSTOM: {
            // استخدام fromValue/toValue
            float value = anim->config.fromValue + progress * (anim->config.toValue - anim->config.fromValue);
            w->style.opacity = value; // مثال
            break;
        }
            
        default:
            break;
    }
}

SadAnimId sadui_animate(SadWidgetId widget, const SadAnimation* anim) {
    if (!anim || g_ctx.animationCount >= SADUI_MAX_ANIMATIONS) return 0;
    
    SadAnimState* state = &g_ctx.animations[g_ctx.animationCount++];
    state->id = g_ctx.nextAnimId++;
    state->widget = widget;
    state->config = *anim;
    state->elapsed = 0;
    state->progress = 0;
    state->active = true;
    state->completed = false;
    
    return state->id;
}

void sadui_stop_animation(SadAnimId id) {
    for (int i = 0; i < g_ctx.animationCount; i++) {
        if (g_ctx.animations[i].id == id) {
            g_ctx.animations[i].active = false;
            return;
        }
    }
}

void sadui_stop_all_animations(SadWidgetId widget) {
    for (int i = 0; i < g_ctx.animationCount; i++) {
        if (g_ctx.animations[i].widget == widget) {
            g_ctx.animations[i].active = false;
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال المساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/* sadui_edges / sadui_corners — defined as static inline in sad_ui_engine.h */

SadEdges sadui_edges_vh(float vertical, float horizontal) {
    return (SadEdges){vertical, horizontal, vertical, horizontal};
}

SadEdges sadui_edges_ltrb(float left, float top, float right, float bottom) {
    return (SadEdges){top, right, bottom, left};
}

SadCorners sadui_corners_top(float top) {
    return (SadCorners){top, top, 0, 0};
}

SadCorners sadui_corners_bottom(float bottom) {
    return (SadCorners){0, 0, bottom, bottom};
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Canvas API - رسم مخصص
// ═══════════════════════════════════════════════════════════════════════════════

static SadRect g_canvasClip = {0};
static bool g_canvasActive = false;

void sadcanvas_begin(SadRect clip) {
    g_canvasClip = clip;
    g_canvasActive = true;
    
    // تفعيل القص
    glEnable(GL_SCISSOR_TEST);
    glScissor((GLint)clip.x, 
              g_ctx.screenHeight - (GLint)clip.y - (GLint)clip.height,
              (GLsizei)clip.width, 
              (GLsizei)clip.height);
}

void sadcanvas_end(void) {
    g_canvasActive = false;
    glDisable(GL_SCISSOR_TEST);
}

void sadcanvas_clear(SadColor color) {
    float c[4];
    color_to_vec4(color, c);
    glClearColor(c[0], c[1], c[2], c[3]);
    glClear(GL_COLOR_BUFFER_BIT);
}

void sadcanvas_draw_rect(SadCanvas ctx, SadRect rect, SadColor color) {
    (void)ctx;
    SadStyle style = {0};
    style.backgroundColor = color;
    style.opacity = 1.0f;
    render_rect(rect, &style);
}

void sadcanvas_draw_rounded_rect(SadCanvas ctx, SadRect rect, float radius, SadColor color) {
    (void)ctx;
    SadStyle style = {0};
    style.backgroundColor = color;
    style.opacity = 1.0f;
    style.borderRadius = sadui_corners(radius);
    render_rect(rect, &style);
}

void sadcanvas_draw_circle(SadCanvas ctx, float cx, float cy, float radius, SadColor color) {
    (void)ctx;
    SadRect rect = {cx - radius, cy - radius, radius * 2, radius * 2};
    SadStyle style = {0};
    style.backgroundColor = color;
    style.opacity = 1.0f;
    style.borderRadius = sadui_corners(radius);
    render_rect(rect, &style);
}

void sadcanvas_draw_ellipse(SadCanvas ctx, float cx, float cy, float rx, float ry, SadColor color) {
    (void)ctx;
    // نرسم مستطيل بزوايا مستديرة كبيرة جداً
    SadRect rect = {cx - rx, cy - ry, rx * 2, ry * 2};
    SadStyle style = {0};
    style.backgroundColor = color;
    style.opacity = 1.0f;
    style.borderRadius = (SadCorners){rx, rx, rx, rx};
    render_rect(rect, &style);
}

// شيدر الخطوط
static const char* LINE_VERTEX_SHADER = 
    "#version 300 es\n"
    "layout(location = 0) in vec2 aPos;\n"
    "uniform mat4 uProjection;\n"
    "void main() {\n"
    "    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);\n"
    "}\n";

static const char* LINE_FRAGMENT_SHADER = 
    "#version 300 es\n"
    "precision highp float;\n"
    "uniform vec4 uColor;\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = uColor;\n"
    "}\n";

static GLuint g_lineProgram = 0;
static GLuint g_lineVAO = 0;
static GLuint g_lineVBO = 0;

static void ensure_line_shader(void) {
    if (g_lineProgram) return;
    
    g_lineProgram = create_shader_program(LINE_VERTEX_SHADER, LINE_FRAGMENT_SHADER);
    
    glGenVertexArrays(1, &g_lineVAO);
    glGenBuffers(1, &g_lineVBO);
    
    glBindVertexArray(g_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4, NULL, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
}

void sadcanvas_draw_line(SadCanvas ctx, float x1, float y1, float x2, float y2, float width, SadColor color) {
    (void)ctx;
    ensure_line_shader();
    
    glUseProgram(g_lineProgram);
    
    float projection[16];
    create_projection_matrix(projection, (float)g_ctx.screenWidth, (float)g_ctx.screenHeight);
    glUniformMatrix4fv(glGetUniformLocation(g_lineProgram, "uProjection"), 1, GL_FALSE, projection);
    
    float c[4];
    color_to_vec4(color, c);
    glUniform4fv(glGetUniformLocation(g_lineProgram, "uColor"), 1, c);
    
    float verts[] = {x1, y1, x2, y2};
    
    glBindVertexArray(g_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_lineVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    
    glLineWidth(width);
    glDrawArrays(GL_LINES, 0, 2);
    
    glBindVertexArray(0);
}

void sadcanvas_draw_arc(SadCanvas ctx, float cx, float cy, float r, float startAngle, float endAngle, 
                        SadColor color) {
    (void)ctx;
    float width = 2.0f; /* default width */
    ensure_line_shader();
    
    glUseProgram(g_lineProgram);
    
    float projection[16];
    create_projection_matrix(projection, (float)g_ctx.screenWidth, (float)g_ctx.screenHeight);
    glUniformMatrix4fv(glGetUniformLocation(g_lineProgram, "uProjection"), 1, GL_FALSE, projection);
    
    float c[4];
    color_to_vec4(color, c);
    glUniform4fv(glGetUniformLocation(g_lineProgram, "uColor"), 1, c);
    
    // حساب عدد الأجزاء
    int segments = (int)((endAngle - startAngle) / 0.1f);
    if (segments < 8) segments = 8;
    if (segments > 128) segments = 128;
    
    float* verts = (float*)malloc(segments * 2 * sizeof(float));
    
    for (int i = 0; i < segments; i++) {
        float angle = startAngle + (endAngle - startAngle) * i / (segments - 1);
        verts[i * 2] = cx + cosf(angle) * r;
        verts[i * 2 + 1] = cy + sinf(angle) * r;
    }
    
    glBindVertexArray(g_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, segments * 2 * sizeof(float), verts, GL_DYNAMIC_DRAW);
    
    glLineWidth(width);
    glDrawArrays(GL_LINE_STRIP, 0, segments);
    
    glBindVertexArray(0);
    free(verts);
}

void sadcanvas_draw_polygon(const SadVec2* points, int count, SadColor fillColor, 
                            SadColor strokeColor, float strokeWidth) {
    if (!points || count < 3) return;
    
    // TODO: تنفيذ رسم المضلعات بالتثليث
    // حالياً نرسم الحدود فقط
    
    if (strokeWidth > 0 && (strokeColor & 0xFF) != 0) {
        ensure_line_shader();
        
        glUseProgram(g_lineProgram);
        
        float projection[16];
        create_projection_matrix(projection, (float)g_ctx.screenWidth, (float)g_ctx.screenHeight);
        glUniformMatrix4fv(glGetUniformLocation(g_lineProgram, "uProjection"), 1, GL_FALSE, projection);
        
        float c[4];
        color_to_vec4(strokeColor, c);
        glUniform4fv(glGetUniformLocation(g_lineProgram, "uColor"), 1, c);
        
        float* verts = (float*)malloc((count + 1) * 2 * sizeof(float));
        for (int i = 0; i < count; i++) {
            verts[i * 2] = points[i].x;
            verts[i * 2 + 1] = points[i].y;
        }
        // إغلاق المضلع
        verts[count * 2] = points[0].x;
        verts[count * 2 + 1] = points[0].y;
        
        glBindVertexArray(g_lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, g_lineVBO);
        glBufferData(GL_ARRAY_BUFFER, (count + 1) * 2 * sizeof(float), verts, GL_DYNAMIC_DRAW);
        
        glLineWidth(strokeWidth);
        glDrawArrays(GL_LINE_STRIP, 0, count + 1);
        
        glBindVertexArray(0);
        free(verts);
    }
}

void sadcanvas_draw_path(SadCanvas ctx, SadVec2* points, int count, float width, SadColor color, bool closed) {
    (void)ctx;
    if (!points || count < 2) return;
    
    ensure_line_shader();
    
    glUseProgram(g_lineProgram);
    
    float projection[16];
    create_projection_matrix(projection, (float)g_ctx.screenWidth, (float)g_ctx.screenHeight);
    glUniformMatrix4fv(glGetUniformLocation(g_lineProgram, "uProjection"), 1, GL_FALSE, projection);
    
    float c[4];
    color_to_vec4(color, c);
    glUniform4fv(glGetUniformLocation(g_lineProgram, "uColor"), 1, c);
    
    int vertCount = closed ? count + 1 : count;
    float* verts = (float*)malloc(vertCount * 2 * sizeof(float));
    
    for (int i = 0; i < count; i++) {
        verts[i * 2] = points[i].x;
        verts[i * 2 + 1] = points[i].y;
    }
    
    if (closed) {
        verts[count * 2] = points[0].x;
        verts[count * 2 + 1] = points[0].y;
    }
    
    glBindVertexArray(g_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, vertCount * 2 * sizeof(float), verts, GL_DYNAMIC_DRAW);
    
    glLineWidth(width);
    glDrawArrays(GL_LINE_STRIP, 0, vertCount);
    
    glBindVertexArray(0);
    free(verts);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  نظام الخطوط (Font System)
// ═══════════════════════════════════════════════════════════════════════════════

#define FONT_ATLAS_SIZE 1024
#define MAX_FONTS 8

typedef struct {
    SadFontId id;
    char name[64];
    GLuint textureId;
    float* glyphWidths;
    int glyphCount;
    float fontSize;
    float lineHeight;
    bool loaded;
} SadFontData;

static SadFontData g_fonts[MAX_FONTS] = {0};
static int g_fontCount = 0;
static SadFontId g_nextFontId = 1;
static SadFontId g_defaultFont = 0;

// شيدر النص
static const char* TEXT_VERTEX_SHADER = 
    "#version 300 es\n"
    "layout(location = 0) in vec2 aPos;\n"
    "layout(location = 1) in vec2 aTexCoord;\n"
    "out vec2 vTexCoord;\n"
    "uniform mat4 uProjection;\n"
    "void main() {\n"
    "    gl_Position = uProjection * vec4(aPos, 0.0, 1.0);\n"
    "    vTexCoord = aTexCoord;\n"
    "}\n";

static const char* TEXT_FRAGMENT_SHADER = 
    "#version 300 es\n"
    "precision highp float;\n"
    "in vec2 vTexCoord;\n"
    "out vec4 FragColor;\n"
    "uniform sampler2D uTexture;\n"
    "uniform vec4 uColor;\n"
    "void main() {\n"
    "    float alpha = texture(uTexture, vTexCoord).r;\n"
    "    FragColor = vec4(uColor.rgb, uColor.a * alpha);\n"
    "}\n";

static GLuint g_textProgram = 0;
static GLuint g_textVAO = 0;
static GLuint g_textVBO = 0;

static void ensure_text_shader(void) {
    if (g_textProgram) return;
    
    g_textProgram = create_shader_program(TEXT_VERTEX_SHADER, TEXT_FRAGMENT_SHADER);
    
    glGenVertexArrays(1, &g_textVAO);
    glGenBuffers(1, &g_textVBO);
    
    glBindVertexArray(g_textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

SadFontId sadui_load_font(const char* path, float size) {
    // TODO: تحميل خط من ملف TTF باستخدام stb_truetype
    // حالياً نُنشئ خطاً افتراضياً بسيطاً
    
    if (g_fontCount >= MAX_FONTS) return 0;
    
    SadFontData* font = &g_fonts[g_fontCount++];
    font->id = g_nextFontId++;
    strncpy(font->name, path ? path : "default", 63);
    font->fontSize = size;
    font->lineHeight = size * 1.2f;
    
    // إنشاء texture فارغ للخط
    glGenTextures(1, &font->textureId);
    glBindTexture(GL_TEXTURE_2D, font->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    font->loaded = true;
    
    if (g_defaultFont == 0) {
        g_defaultFont = font->id;
    }
    
    return font->id;
}

void sadui_unload_font(SadFontId fontId) {
    for (int i = 0; i < g_fontCount; i++) {
        if (g_fonts[i].id == fontId) {
            if (g_fonts[i].textureId) {
                glDeleteTextures(1, &g_fonts[i].textureId);
            }
            if (g_fonts[i].glyphWidths) {
                free(g_fonts[i].glyphWidths);
            }
            
            // ضغط المصفوفة
            for (int j = i; j < g_fontCount - 1; j++) {
                g_fonts[j] = g_fonts[j + 1];
            }
            g_fontCount--;
            return;
        }
    }
}

void sadui_set_default_font(SadFontId fontId) {
    g_defaultFont = fontId;
}

// رسم النص (مبسط حالياً - يحتاج تكامل stb_truetype للعربية الكاملة)
void sadcanvas_draw_text(SadCanvas ctx, const char* text, float x, float y, float size, SadColor color) {
    (void)ctx;
    if (!text || !*text) return;
    
    // TODO: تنفيذ رسم النص الكامل مع دعم العربية
    // حالياً نرسم مستطيلاً بحجم تقريبي للنص
    
    size_t len = strlen(text);
    float width = len * size * 0.6f;  // تقريب
    float height = size;
    
    SadRect rect = {x, y, width, height};
    SadStyle style = {0};
    style.backgroundColor = color;
    style.opacity = 0.3f;  // شبه شفاف للإشارة إلى أنه placeholder
    render_rect(rect, &style);
    
    LOGD("Drawing text (placeholder): %s at (%.0f, %.0f)", text, x, y);
}

SadSize sadcanvas_measure_text(const char* text, float size) {
    if (!text) return (SadSize){SAD_SIZE_FIXED, 0};
    
    size_t len = strlen(text);
    // تقريب بسيط - الحجم الحقيقي يحتاج قياس الخط
    float width = len * size * 0.6f;
    
    return (SadSize){SAD_SIZE_FIXED, width};
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إدارة الصور
// ═══════════════════════════════════════════════════════════════════════════════

#define MAX_IMAGES 64

typedef struct {
    SadImageId id;
    GLuint textureId;
    int width;
    int height;
    bool loaded;
} SadImageData;

static SadImageData g_images[MAX_IMAGES] = {0};
static int g_imageCount = 0;
static SadImageId g_nextImageId = 1;

SadImageId sadui_load_image(const char* path) {
    if (!path || g_imageCount >= MAX_IMAGES) return 0;
    
    // قراءة الملف إلى الذاكرة
    FILE* f = fopen(path, "rb");
    if (!f) {
        LOGE("تعذر فتح ملف الصورة: %s", path);
        return 0;
    }
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    unsigned char* fileData = (unsigned char*)malloc(fileSize);
    if (!fileData) {
        fclose(f);
        LOGE("تعذر تخصيص ذاكرة لملف الصورة: %ld bytes", fileSize);
        return 0;
    }
    fread(fileData, 1, fileSize, f);
    fclose(f);
    
    // فك تشفير الصورة باستخدام stb_image
    int w, h, channels;
    unsigned char* pixels = stbi_load_from_memory(fileData, (int)fileSize, &w, &h, &channels, 4);
    free(fileData);
    
    if (!pixels) {
        LOGE("تعذر فك تشفير الصورة: %s — %s", path, stbi_failure_reason());
        return 0;
    }
    
    // إنشاء texture OpenGL
    SadImageData* img = &g_images[g_imageCount++];
    img->id = g_nextImageId++;
    img->width = w;
    img->height = h;
    
    glGenTextures(1, &img->textureId);
    glBindTexture(GL_TEXTURE_2D, img->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    stbi_image_free(pixels);
    img->loaded = true;
    
    LOGI("تم تحميل الصورة: %s (%dx%d) → id=%d", path, w, h, img->id);
    return img->id;
}

SadImageId sadui_load_image_from_memory(const void* data, int size) {
    if (!data || size <= 0 || g_imageCount >= MAX_IMAGES) return 0;
    
    int w, h, channels;
    unsigned char* pixels = stbi_load_from_memory((const unsigned char*)data, size, &w, &h, &channels, 4);
    if (!pixels) {
        LOGE("تعذر فك تشفير الصورة من الذاكرة: %s", stbi_failure_reason());
        return 0;
    }
    
    SadImageData* img = &g_images[g_imageCount++];
    img->id = g_nextImageId++;
    img->width = w;
    img->height = h;
    
    glGenTextures(1, &img->textureId);
    glBindTexture(GL_TEXTURE_2D, img->textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    stbi_image_free(pixels);
    img->loaded = true;
    
    LOGI("تم تحميل صورة من الذاكرة (%dx%d) → id=%d", w, h, img->id);
    return img->id;
}

void sadui_unload_image(SadImageId imageId) {
    for (int i = 0; i < g_imageCount; i++) {
        if (g_images[i].id == imageId) {
            if (g_images[i].textureId) {
                glDeleteTextures(1, &g_images[i].textureId);
            }
            for (int j = i; j < g_imageCount - 1; j++) {
                g_images[j] = g_images[j + 1];
            }
            g_imageCount--;
            return;
        }
    }
}

void sadui_get_image_size(SadImageId imageId, int* width, int* height) {
    for (int i = 0; i < g_imageCount; i++) {
        if (g_images[i].id == imageId) {
            if (width) *width = g_images[i].width;
            if (height) *height = g_images[i].height;
            return;
        }
    }
    if (width) *width = 0;
    if (height) *height = 0;
}

void sadcanvas_draw_image(SadCanvas ctx, int imageId, SadRect dest) {
    (void)ctx;
    if (!g_ctx.imageShaderProgram) return;
    
    // البحث عن بيانات الصورة
    SadImageData* img = NULL;
    for (int i = 0; i < g_imageCount; i++) {
        if (g_images[i].id == imageId && g_images[i].loaded) {
            img = &g_images[i];
            break;
        }
    }
    if (!img) return;
    
    // استخدام شيدر الصور
    glUseProgram(g_ctx.imageShaderProgram);
    
    GLint uProj = glGetUniformLocation(g_ctx.imageShaderProgram, "uProjection");
    GLint uRectI = glGetUniformLocation(g_ctx.imageShaderProgram, "uRect");
    GLint uTex = glGetUniformLocation(g_ctx.imageShaderProgram, "uTexture");
    GLint uOp = glGetUniformLocation(g_ctx.imageShaderProgram, "uOpacity");
    GLint uBR = glGetUniformLocation(g_ctx.imageShaderProgram, "uBorderRadius");
    
    // مصفوفة الإسقاط
    float proj[16] = {0};
    proj[0] = 2.0f / g_ctx.screenWidth;
    proj[5] = -2.0f / g_ctx.screenHeight;
    proj[10] = -1.0f;
    proj[12] = -1.0f;
    proj[13] = 1.0f;
    proj[15] = 1.0f;
    glUniformMatrix4fv(uProj, 1, GL_FALSE, proj);
    
    glUniform4f(uRectI, dest.x, dest.y, dest.width, dest.height);
    glUniform1f(uOp, 1.0f);
    glUniform4f(uBR, 0, 0, 0, 0);
    
    // ربط texture الصورة
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, img->textureId);
    glUniform1i(uTex, 0);
    
    glBindVertexArray(g_ctx.quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    // العودة للشيدر الأساسي
    glUseProgram(g_ctx.shaderProgram);
}

/** رسم جزء من صورة */
void sadcanvas_draw_image_region(SadCanvas ctx, int imageId, SadRect src, SadRect dest) {
    // حالياً نرسم الصورة كاملة — يمكن إضافة دعم UV لاحقاً
    sadcanvas_draw_image(ctx, imageId, dest);
}

/** رسم صورة من widget — دالة داخلية للمحرك */
static void render_image_widget(SadWidget* widget) {
    if (!widget || !g_ctx.imageShaderProgram) return;
    
    int imageId = (int)widget->value;
    SadImageData* img = NULL;
    for (int i = 0; i < g_imageCount; i++) {
        if (g_images[i].id == imageId && g_images[i].loaded) {
            img = &g_images[i];
            break;
        }
    }
    if (!img) return;
    
    glUseProgram(g_ctx.imageShaderProgram);
    
    GLint uProj = glGetUniformLocation(g_ctx.imageShaderProgram, "uProjection");
    GLint uRectI = glGetUniformLocation(g_ctx.imageShaderProgram, "uRect");
    GLint uTex = glGetUniformLocation(g_ctx.imageShaderProgram, "uTexture");
    GLint uOp = glGetUniformLocation(g_ctx.imageShaderProgram, "uOpacity");
    GLint uBR = glGetUniformLocation(g_ctx.imageShaderProgram, "uBorderRadius");
    
    float proj[16] = {0};
    proj[0] = 2.0f / g_ctx.screenWidth;
    proj[5] = -2.0f / g_ctx.screenHeight;
    proj[10] = -1.0f;
    proj[12] = -1.0f;
    proj[13] = 1.0f;
    proj[15] = 1.0f;
    glUniformMatrix4fv(uProj, 1, GL_FALSE, proj);
    
    SadRect r = widget->computedRect;
    glUniform4f(uRectI, r.x, r.y, r.width, r.height);
    glUniform1f(uOp, widget->style.opacity);
    glUniform4f(uBR, widget->style.borderRadius.topLeft,
                widget->style.borderRadius.topRight,
                widget->style.borderRadius.bottomRight,
                widget->style.borderRadius.bottomLeft);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, img->textureId);
    glUniform1i(uTex, 0);
    
    glBindVertexArray(g_ctx.quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    
    glUseProgram(g_ctx.shaderProgram);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  المكونات المُركّبة (Convenience Widgets)
// ═══════════════════════════════════════════════════════════════════════════════

SadWidgetId sadui_button(const char* text) {
    SadWidgetId btn = sadui_create(SAD_WIDGET_BUTTON);
    if (btn && text) {
        sadui_set_text(btn, text);
    }
    return btn;
}

SadWidgetId sadui_text(const char* text) {
    SadWidgetId txt = sadui_create(SAD_WIDGET_TEXT);
    if (txt && text) {
        sadui_set_text(txt, text);
    }
    return txt;
}

SadWidgetId sadui_heading(const char* text) {
    SadWidgetId h = sadui_create(SAD_WIDGET_HEADING);
    if (h && text) {
        sadui_set_text(h, text);
    }
    return h;
}

SadWidgetId sadui_image(int imageId) {
    SadWidgetId img = sadui_create(SAD_WIDGET_IMAGE);
    if (img) {
        SadWidget* w = get_widget(img);
        if (w) {
            w->value = (float)imageId;
        }
    }
    return img;
}

SadWidgetId sadui_card(void) {
    return sadui_create(SAD_WIDGET_CARD);
}

SadWidgetId sadui_column(void) {
    return sadui_create(SAD_WIDGET_COLUMN);
}

SadWidgetId sadui_row(void) {
    return sadui_create(SAD_WIDGET_ROW);
}

SadWidgetId sadui_container(void) {
    return sadui_create(SAD_WIDGET_CONTAINER);
}

SadWidgetId sadui_divider(void) {
    return sadui_create(SAD_WIDGET_DIVIDER);
}

SadWidgetId sadui_chip(const char* text) {
    SadWidgetId c = sadui_create(SAD_WIDGET_CHIP);
    if (c && text) sadui_set_text(c, text);
    return c;
}

SadWidgetId sadui_fab(const char* text) {
    SadWidgetId f = sadui_create(SAD_WIDGET_FAB);
    if (f && text) sadui_set_text(f, text);
    return f;
}

SadWidgetId sadui_scrollable(SadScrollDirection direction) {
    SadWidgetId scroll = sadui_create(SAD_WIDGET_SCROLL);
    // TODO: تعيين اتجاه التمرير
    return scroll;
}

SadWidgetId sadui_textfield(const char* placeholder) {
    SadWidgetId field = sadui_create(SAD_WIDGET_TEXT_FIELD);
    // TODO: تعيين placeholder
    return field;
}

SadWidgetId sadui_checkbox(const char* label, bool checked) {
    SadWidgetId cb = sadui_create(SAD_WIDGET_CHECKBOX);
    if (cb) {
        sadui_set_text(cb, label);
        sadui_set_checked(cb, checked);
    }
    return cb;
}

SadWidgetId sadui_slider(float min, float max, float value) {
    SadWidgetId slider = sadui_create(SAD_WIDGET_SLIDER);
    if (slider) {
        // TODO: تخزين min/max
        sadui_set_value(slider, value);
    }
    return slider;
}

SadWidgetId sadui_progress(float value) {
    SadWidgetId progress = sadui_create(SAD_WIDGET_PROGRESS);
    if (progress) {
        sadui_set_value(progress, value);
    }
    return progress;
}
