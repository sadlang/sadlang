/**
 * ==========================================================================
 * sad_android_ui.c - تطبيق محرك واجهة المستخدم الرسومي
 * Android Native UI Engine Implementation
 * ==========================================================================
 * 
 * الوصف (عربي):
 * --------------
 * هذا الملف يحتوي على التطبيق الكامل لمحرك واجهة المستخدم الأصلي.
 * يتضمن:
 * - رسم الواجهات باستخدام OpenGL ES 3.0
 * - نظام التخطيط (layout) لترتيب الواجهات
 * - معالجة أحداث اللمس والإيماءات
 * - رسم النصوص العربية مع دعم RTL
 * - نظام الرسوم المتحركة
 * 
 * البنية الداخلية:
 * ----------------
 * 1. OpenGL ES Setup: تهيئة سياق OpenGL وإنشاء برامج الشيدر
 * 2. Widget Rendering: رسم كل نوع من الواجهات
 * 3. Layout Engine: حساب مواقع وأحجام الواجهات
 * 4. Event Handling: معالجة اللمس والإيماءات
 * 5. Text Rendering: رسم النصوص باستخدام stb_truetype
 * 
 * Description (English):
 * ----------------------
 * This file contains the complete implementation of the native UI engine.
 * It includes:
 * - Widget rendering using OpenGL ES 3.0
 * - Layout system for arranging widgets
 * - Touch and gesture event handling
 * - Arabic text rendering with RTL support
 * - Animation system
 * 
 * ==========================================================================
 */

#include "sad_android_ui.h"
#include "../sad_android_memory.h"
#include "../sad_android_string.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* تضمين رؤوس OpenGL ES / Include OpenGL ES headers */
#ifdef __ANDROID__
    #include <GLES3/gl3.h>
    #include <EGL/egl.h>
    #include <android/native_activity.h>
    #include <android/native_window.h>
    #include <android/log.h>
    
    #define SAD_LOG_TAG "SadUI"
    #define SAD_LOGI(...) __android_log_print(ANDROID_LOG_INFO, SAD_LOG_TAG, __VA_ARGS__)
    #define SAD_LOGW(...) __android_log_print(ANDROID_LOG_WARN, SAD_LOG_TAG, __VA_ARGS__)
    #define SAD_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, SAD_LOG_TAG, __VA_ARGS__)
#else
    /* للاختبار على الكمبيوتر / For desktop testing */
    #define SAD_LOGI(...)
    #define SAD_LOGW(...)
    #define SAD_LOGE(...)
    typedef unsigned int GLuint;
    typedef int GLint;
    typedef float GLfloat;
    typedef unsigned int GLenum;
    #define GL_VERTEX_SHADER 0x8B31
    #define GL_FRAGMENT_SHADER 0x8B30
#endif

/* =========================================================================
 * الثوابت الداخلية / Internal Constants
 * ========================================================================= */

/** أقصى عدد رؤوس في دفعة واحدة / Max vertices in a single batch */
#define MAX_BATCH_VERTICES  16384

/** أقصى عدد فهارس في دفعة واحدة / Max indices in a single batch */
#define MAX_BATCH_INDICES   (MAX_BATCH_VERTICES * 6 / 4)

/** حجم الخط الافتراضي / Default font size */
#define DEFAULT_FONT_SIZE   16.0f


/* =========================================================================
 * هياكل OpenGL الداخلية / Internal OpenGL Structures
 * ========================================================================= */

/**
 * رأس (Vertex) للرسم
 * Vertex for rendering
 */
typedef struct RenderVertex {
    float x, y;         /**< الموقع / Position */
    float u, v;         /**< إحداثيات الأنسجة / Texture coordinates */
    float r, g, b, a;   /**< اللون / Color */
} RenderVertex;

/**
 * دفعة رسم / Render batch
 * 
 * نجمع عدة عمليات رسم في دفعة واحدة لتحسين الأداء
 * We batch multiple draw calls together to improve performance
 */
typedef struct RenderBatch {
    RenderVertex vertices[MAX_BATCH_VERTICES];  /**< الرؤوس / Vertices */
    uint16_t indices[MAX_BATCH_INDICES];        /**< الفهارس / Indices */
    int32_t vertex_count;                       /**< عدد الرؤوس / Vertex count */
    int32_t index_count;                        /**< عدد الفهارس / Index count */
    GLuint texture_id;                          /**< معرّف الأنسجة الحالية / Current texture ID */
} RenderBatch;

/**
 * سياق الرسم / Render context
 * 
 * يحتوي على كل موارد OpenGL
 * Contains all OpenGL resources
 */
typedef struct RenderContext {
    /* --- برامج الشيدر / Shader programs --- */
    GLuint solid_program;       /**< برنامج للأشكال الصلبة / Solid shapes program */
    GLuint textured_program;    /**< برنامج للأنسجة / Textured program */
    GLuint text_program;        /**< برنامج للنصوص / Text program */
    
    /* --- المتغيرات الموحدة / Uniforms --- */
    GLint u_projection;         /**< مصفوفة الإسقاط / Projection matrix */
    GLint u_texture;            /**< وحدة الأنسجة / Texture unit */
    
    /* --- كائنات المخزن / Buffer objects --- */
    GLuint vao;                 /**< Vertex Array Object */
    GLuint vbo;                 /**< Vertex Buffer Object */
    GLuint ebo;                 /**< Element Buffer Object */
    
    /* --- دفعة الرسم الحالية / Current render batch --- */
    RenderBatch batch;
    
    /* --- مصفوفة الإسقاط / Projection matrix --- */
    float projection[16];
    
    /* --- الحالة / State --- */
    bool is_initialized;
    int32_t screen_width;
    int32_t screen_height;
    
} RenderContext;

/* سياق الرسم العام / Global render context */
static RenderContext g_render_ctx = {0};


/* =========================================================================
 * معرّف الواجهات / Widget ID Generator
 * ========================================================================= */

static uint32_t g_next_widget_id = 1;

static uint32_t generate_widget_id(void) {
    return g_next_widget_id++;
}


/* =========================================================================
 * كود الشيدر / Shader Code
 * ========================================================================= */

/**
 * شيدر الرأس للأشكال الملونة
 * Vertex shader for colored shapes
 */
static const char* SOLID_VERTEX_SHADER = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "layout(location = 0) in vec2 a_position;\n"
    "layout(location = 1) in vec2 a_texcoord;\n"
    "layout(location = 2) in vec4 a_color;\n"
    "uniform mat4 u_projection;\n"
    "out vec4 v_color;\n"
    "out vec2 v_texcoord;\n"
    "void main() {\n"
    "    gl_Position = u_projection * vec4(a_position, 0.0, 1.0);\n"
    "    v_color = a_color;\n"
    "    v_texcoord = a_texcoord;\n"
    "}\n";

/**
 * شيدر الجزء للأشكال الملونة
 * Fragment shader for colored shapes
 */
static const char* SOLID_FRAGMENT_SHADER = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec4 v_color;\n"
    "in vec2 v_texcoord;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = v_color;\n"
    "}\n";

/**
 * شيدر الجزء للنصوص (SDF)
 * Fragment shader for text (Signed Distance Field)
 */
static const char* TEXT_FRAGMENT_SHADER = 
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec4 v_color;\n"
    "in vec2 v_texcoord;\n"
    "uniform sampler2D u_texture;\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    float alpha = texture(u_texture, v_texcoord).r;\n"
    "    fragColor = vec4(v_color.rgb, v_color.a * alpha);\n"
    "}\n";


/* =========================================================================
 * دوال OpenGL المساعدة / OpenGL Helper Functions
 * ========================================================================= */

#ifdef __ANDROID__

/**
 * ترجمة شيدر
 * Compile a shader
 * 
 * @param type نوع الشيدر (GL_VERTEX_SHADER أو GL_FRAGMENT_SHADER)
 * @param source الكود المصدري / Source code
 * @return معرّف الشيدر أو 0 عند الفشل / Shader ID or 0 on failure
 */
static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        SAD_LOGE("فشل إنشاء الشيدر / Failed to create shader");
        return 0;
    }
    
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    
    if (!compiled) {
        GLint info_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
        
        if (info_len > 1) {
            char* info_log = (char*)malloc(info_len);
            glGetShaderInfoLog(shader, info_len, NULL, info_log);
            SAD_LOGE("خطأ ترجمة الشيدر / Shader compile error: %s", info_log);
            free(info_log);
        }
        
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

/**
 * إنشاء برنامج شيدر
 * Create a shader program
 * 
 * @param vertex_src كود شيدر الرأس / Vertex shader source
 * @param fragment_src كود شيدر الجزء / Fragment shader source
 * @return معرّف البرنامج أو 0 عند الفشل / Program ID or 0 on failure
 */
static GLuint create_program(const char* vertex_src, const char* fragment_src) {
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_src);
    if (vertex_shader == 0) return 0;
    
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_src);
    if (fragment_shader == 0) {
        glDeleteShader(vertex_shader);
        return 0;
    }
    
    GLuint program = glCreateProgram();
    if (program == 0) {
        SAD_LOGE("فشل إنشاء البرنامج / Failed to create program");
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }
    
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    GLint linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    
    if (!linked) {
        GLint info_len = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_len);
        
        if (info_len > 1) {
            char* info_log = (char*)malloc(info_len);
            glGetProgramInfoLog(program, info_len, NULL, info_log);
            SAD_LOGE("خطأ ربط البرنامج / Program link error: %s", info_log);
            free(info_log);
        }
        
        glDeleteProgram(program);
        glDeleteShader(vertex_shader);
        glDeleteShader(fragment_shader);
        return 0;
    }
    
    /* الشيدرات لم تعد مطلوبة بعد الربط / Shaders no longer needed after linking */
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return program;
}

/**
 * إنشاء مصفوفة إسقاط متعامدة
 * Create an orthographic projection matrix
 * 
 * @param left الحد الأيسر / Left bound
 * @param right الحد الأيمن / Right bound
 * @param bottom الحد السفلي / Bottom bound
 * @param top الحد العلوي / Top bound
 * @param out المصفوفة الناتجة (16 عنصر) / Output matrix (16 elements)
 */
static void create_ortho_matrix(float left, float right, float bottom, float top, float* out) {
    memset(out, 0, 16 * sizeof(float));
    
    out[0] = 2.0f / (right - left);
    out[5] = 2.0f / (top - bottom);
    out[10] = -1.0f;
    out[12] = -(right + left) / (right - left);
    out[13] = -(top + bottom) / (top - bottom);
    out[15] = 1.0f;
}

#endif /* __ANDROID__ */


/* =========================================================================
 * تهيئة نظام الرسم / Render System Initialization
 * ========================================================================= */

bool sad_render_init(SadApp* app) {
    if (!app) return false;
    
#ifdef __ANDROID__
    SAD_LOGI("تهيئة نظام الرسم / Initializing render system");
    
    /* إنشاء برنامج الأشكال الصلبة / Create solid shapes program */
    g_render_ctx.solid_program = create_program(SOLID_VERTEX_SHADER, SOLID_FRAGMENT_SHADER);
    if (g_render_ctx.solid_program == 0) {
        SAD_LOGE("فشل إنشاء برنامج الأشكال / Failed to create solid program");
        return false;
    }
    
    /* إنشاء برنامج النصوص / Create text program */
    g_render_ctx.text_program = create_program(SOLID_VERTEX_SHADER, TEXT_FRAGMENT_SHADER);
    if (g_render_ctx.text_program == 0) {
        SAD_LOGE("فشل إنشاء برنامج النصوص / Failed to create text program");
        return false;
    }
    
    /* الحصول على مواقع المتغيرات الموحدة / Get uniform locations */
    g_render_ctx.u_projection = glGetUniformLocation(g_render_ctx.solid_program, "u_projection");
    g_render_ctx.u_texture = glGetUniformLocation(g_render_ctx.text_program, "u_texture");
    
    /* إنشاء VAO و VBO و EBO / Create VAO, VBO, EBO */
    glGenVertexArrays(1, &g_render_ctx.vao);
    glGenBuffers(1, &g_render_ctx.vbo);
    glGenBuffers(1, &g_render_ctx.ebo);
    
    glBindVertexArray(g_render_ctx.vao);
    
    /* إعداد VBO / Setup VBO */
    glBindBuffer(GL_ARRAY_BUFFER, g_render_ctx.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RenderVertex) * MAX_BATCH_VERTICES, NULL, GL_DYNAMIC_DRAW);
    
    /* إعداد EBO / Setup EBO */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_render_ctx.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * MAX_BATCH_INDICES, NULL, GL_DYNAMIC_DRAW);
    
    /* تعريف سمات الرأس / Define vertex attributes */
    /* الموقع (2 floats) / Position */
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, x));
    glEnableVertexAttribArray(0);
    
    /* إحداثيات الأنسجة (2 floats) / Texture coordinates */
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, u));
    glEnableVertexAttribArray(1);
    
    /* اللون (4 floats) / Color */
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(RenderVertex), (void*)offsetof(RenderVertex, r));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    /* تفعيل الدمج للشفافية / Enable blending for transparency */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    /* حفظ أبعاد الشاشة / Store screen dimensions */
    g_render_ctx.screen_width = app->screen_width;
    g_render_ctx.screen_height = app->screen_height;
    
    /* إنشاء مصفوفة الإسقاط / Create projection matrix */
    create_ortho_matrix(0, (float)app->screen_width, (float)app->screen_height, 0, g_render_ctx.projection);
    
    g_render_ctx.is_initialized = true;
    SAD_LOGI("تم تهيئة نظام الرسم بنجاح / Render system initialized successfully");
    
#endif /* __ANDROID__ */
    
    return true;
}

void sad_render_cleanup(SadApp* app) {
    (void)app;
    
#ifdef __ANDROID__
    if (!g_render_ctx.is_initialized) return;
    
    SAD_LOGI("تنظيف نظام الرسم / Cleaning up render system");
    
    glDeleteVertexArrays(1, &g_render_ctx.vao);
    glDeleteBuffers(1, &g_render_ctx.vbo);
    glDeleteBuffers(1, &g_render_ctx.ebo);
    glDeleteProgram(g_render_ctx.solid_program);
    glDeleteProgram(g_render_ctx.text_program);
    
    memset(&g_render_ctx, 0, sizeof(g_render_ctx));
    
#endif /* __ANDROID__ */
}


/* =========================================================================
 * دوال دفعة الرسم / Render Batch Functions
 * ========================================================================= */

#ifdef __ANDROID__

/**
 * بدء دفعة جديدة
 * Begin a new batch
 */
static void batch_begin(void) {
    g_render_ctx.batch.vertex_count = 0;
    g_render_ctx.batch.index_count = 0;
    g_render_ctx.batch.texture_id = 0;
}

/**
 * إرسال الدفعة للرسم
 * Flush the batch to GPU
 */
static void batch_flush(void) {
    if (g_render_ctx.batch.vertex_count == 0) return;
    
    RenderBatch* batch = &g_render_ctx.batch;
    
    /* اختيار البرنامج المناسب / Choose appropriate program */
    GLuint program = (batch->texture_id != 0) ? g_render_ctx.text_program : g_render_ctx.solid_program;
    glUseProgram(program);
    
    /* تعيين مصفوفة الإسقاط / Set projection matrix */
    GLint proj_loc = glGetUniformLocation(program, "u_projection");
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, g_render_ctx.projection);
    
    /* إذا كان هناك أنسجة / If there's a texture */
    if (batch->texture_id != 0) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, batch->texture_id);
        GLint tex_loc = glGetUniformLocation(program, "u_texture");
        glUniform1i(tex_loc, 0);
    }
    
    /* رفع البيانات / Upload data */
    glBindVertexArray(g_render_ctx.vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, g_render_ctx.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, batch->vertex_count * sizeof(RenderVertex), batch->vertices);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_render_ctx.ebo);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, batch->index_count * sizeof(uint16_t), batch->indices);
    
    /* الرسم / Draw */
    glDrawElements(GL_TRIANGLES, batch->index_count, GL_UNSIGNED_SHORT, 0);
    
    glBindVertexArray(0);
    
    /* إعادة التعيين / Reset */
    batch->vertex_count = 0;
    batch->index_count = 0;
}

/**
 * رسم مستطيل ملون
 * Draw a colored rectangle
 * 
 * @param x الموقع الأفقي / X position
 * @param y الموقع العمودي / Y position
 * @param w العرض / Width
 * @param h الارتفاع / Height
 * @param color اللون / Color
 */
static void draw_rect(float x, float y, float w, float h, SadColor color) {
    RenderBatch* batch = &g_render_ctx.batch;
    
    /* التحقق من السعة / Check capacity */
    if (batch->vertex_count + 4 > MAX_BATCH_VERTICES || 
        batch->index_count + 6 > MAX_BATCH_INDICES) {
        batch_flush();
    }
    
    /* إذا كان هناك أنسجة مختلفة، نُرسل الدفعة / If different texture, flush */
    if (batch->texture_id != 0) {
        batch_flush();
        batch->texture_id = 0;
    }
    
    uint16_t base = (uint16_t)batch->vertex_count;
    
    /* إضافة الرؤوس الأربعة / Add 4 vertices */
    /* أعلى يسار / Top-left */
    batch->vertices[batch->vertex_count++] = (RenderVertex){
        x, y, 0, 0, color.r, color.g, color.b, color.a
    };
    /* أعلى يمين / Top-right */
    batch->vertices[batch->vertex_count++] = (RenderVertex){
        x + w, y, 1, 0, color.r, color.g, color.b, color.a
    };
    /* أسفل يمين / Bottom-right */
    batch->vertices[batch->vertex_count++] = (RenderVertex){
        x + w, y + h, 1, 1, color.r, color.g, color.b, color.a
    };
    /* أسفل يسار / Bottom-left */
    batch->vertices[batch->vertex_count++] = (RenderVertex){
        x, y + h, 0, 1, color.r, color.g, color.b, color.a
    };
    
    /* إضافة الفهارس (مثلثان) / Add indices (2 triangles) */
    batch->indices[batch->index_count++] = base + 0;
    batch->indices[batch->index_count++] = base + 1;
    batch->indices[batch->index_count++] = base + 2;
    batch->indices[batch->index_count++] = base + 0;
    batch->indices[batch->index_count++] = base + 2;
    batch->indices[batch->index_count++] = base + 3;
}

/**
 * رسم مستطيل بزوايا مستديرة
 * Draw a rounded rectangle
 * 
 * هذه نسخة مبسطة — النسخة الكاملة تستخدم مثلثات أكثر للزوايا
 * This is a simplified version — full version uses more triangles for corners
 */
static void draw_rounded_rect(float x, float y, float w, float h, float radius, SadColor color) {
    /* إذا كان نصف القطر صفر، نرسم مستطيل عادي / If radius is 0, draw normal rect */
    if (radius <= 0.0f) {
        draw_rect(x, y, w, h, color);
        return;
    }
    
    /* تحديد نصف القطر الأقصى / Clamp radius */
    float max_radius = fminf(w, h) / 2.0f;
    if (radius > max_radius) radius = max_radius;
    
    /* رسم المنطقة المركزية / Draw center region */
    draw_rect(x + radius, y, w - 2 * radius, h, color);
    
    /* رسم الجوانب / Draw sides */
    draw_rect(x, y + radius, radius, h - 2 * radius, color);              /* يسار / Left */
    draw_rect(x + w - radius, y + radius, radius, h - 2 * radius, color); /* يمين / Right */
    
    /* ملاحظة: الزوايا المستديرة تحتاج رسم قطاعات دائرية */
    /* Note: Rounded corners need arc segments - simplified here */
    /* TODO: إضافة رسم الزوايا بالتقريب / Add corner arc rendering */
}

/**
 * رسم حدود مستطيل
 * Draw rectangle outline
 */
static void draw_rect_outline(float x, float y, float w, float h, SadColor color, float thickness) {
    /* أعلى / Top */
    draw_rect(x, y, w, thickness, color);
    /* أسفل / Bottom */
    draw_rect(x, y + h - thickness, w, thickness, color);
    /* يسار / Left */
    draw_rect(x, y + thickness, thickness, h - 2 * thickness, color);
    /* يمين / Right */
    draw_rect(x + w - thickness, y + thickness, thickness, h - 2 * thickness, color);
}

#endif /* __ANDROID__ */


/* =========================================================================
 * رسم الإطار / Frame Rendering
 * ========================================================================= */

void sad_render_begin_frame(SadApp* app) {
    if (!app) return;
    
#ifdef __ANDROID__
    /* مسح الشاشة / Clear screen */
    SadColor bg = app->current_screen ? app->current_screen->background_color : SAD_COLOR_BACKGROUND;
    glClearColor(bg.r, bg.g, bg.b, bg.a);
    glClear(GL_COLOR_BUFFER_BIT);
    
    /* بدء دفعة جديدة / Start new batch */
    batch_begin();
    
#endif
}

void sad_render_end_frame(SadApp* app) {
    if (!app) return;
    
#ifdef __ANDROID__
    /* إرسال آخر دفعة / Flush last batch */
    batch_flush();
    
    /* تبديل المخازن / Swap buffers */
    if (app->egl_display && app->egl_surface) {
        eglSwapBuffers((EGLDisplay)app->egl_display, (EGLSurface)app->egl_surface);
    }
    
#endif
}


/* =========================================================================
 * رسم الواجهات / Widget Rendering
 * ========================================================================= */

/**
 * رسم واجهة وأطفالها
 * Render a widget and its children
 */
void sad_render_widget(SadWidget* widget) {
    if (!widget || !widget->visible) return;
    
#ifdef __ANDROID__
    
    SadRect* b = &widget->bounds;
    
    /* --- رسم الظل (إذا كان هناك ارتفاع) / Draw shadow if elevated --- */
    if (widget->elevation > 0.0f) {
        float shadow_offset = widget->elevation * 0.5f;
        float shadow_blur = widget->elevation;
        SadColor shadow_color = {0, 0, 0, 0.2f};
        draw_rounded_rect(b->x + shadow_offset, b->y + shadow_offset + shadow_blur,
                          b->width, b->height, widget->corner_radius, shadow_color);
    }
    
    /* --- رسم الخلفية / Draw background --- */
    if (widget->background_color.a > 0.0f) {
        SadColor bg = widget->background_color;
        bg.a *= widget->opacity;
        
        if (widget->corner_radius > 0.0f) {
            draw_rounded_rect(b->x, b->y, b->width, b->height, widget->corner_radius, bg);
        } else {
            draw_rect(b->x, b->y, b->width, b->height, bg);
        }
    }
    
    /* --- رسم الحدود / Draw border --- */
    if (widget->border_width > 0.0f && widget->border_color.a > 0.0f) {
        SadColor bc = widget->border_color;
        bc.a *= widget->opacity;
        draw_rect_outline(b->x, b->y, b->width, b->height, bc, widget->border_width);
    }
    
    /* --- رسم المحتوى حسب النوع / Draw content based on type --- */
    switch (widget->type) {
        case SAD_WIDGET_TEXT:
        case SAD_WIDGET_BUTTON:
            /* TODO: رسم النص / Draw text */
            /* سيُنفّذ عند إضافة نظام الخطوط / Will be implemented with font system */
            break;
            
        case SAD_WIDGET_TEXT_INPUT:
            /* رسم مؤشر الكتابة / Draw cursor */
            if (widget->state == SAD_STATE_FOCUSED) {
                /* TODO: رسم المؤشر / Draw cursor */
            }
            break;
            
        case SAD_WIDGET_CHECKBOX:
        case SAD_WIDGET_SWITCH:
            /* رسم حالة التبديل / Draw toggle state */
            if (widget->data.toggle.is_checked) {
                /* TODO: رسم علامة الصح / Draw checkmark */
            }
            break;
            
        case SAD_WIDGET_SLIDER:
            /* رسم المنزلق / Draw slider track and thumb */
            {
                float track_height = 4.0f;
                float thumb_size = 20.0f;
                float track_y = b->y + (b->height - track_height) / 2.0f;
                float progress = (widget->data.slider.current_value - widget->data.slider.min_value) /
                                 (widget->data.slider.max_value - widget->data.slider.min_value);
                float thumb_x = b->x + progress * (b->width - thumb_size);
                
                /* المسار / Track */
                draw_rounded_rect(b->x, track_y, b->width, track_height, 2.0f, SAD_COLOR_OUTLINE);
                
                /* المسار الملون / Filled track */
                draw_rounded_rect(b->x, track_y, thumb_x - b->x + thumb_size/2, track_height, 2.0f, SAD_COLOR_PRIMARY);
                
                /* المقبض / Thumb */
                draw_rounded_rect(thumb_x, b->y + (b->height - thumb_size) / 2.0f,
                                  thumb_size, thumb_size, thumb_size/2, SAD_COLOR_PRIMARY);
            }
            break;
            
        default:
            break;
    }
    
    /* --- رسم الأطفال / Draw children --- */
    for (int32_t i = 0; i < widget->child_count; i++) {
        sad_render_widget(widget->children[i]);
    }
    
    /* --- استدعاء رسم مخصص / Call custom draw callback --- */
    if (widget->on_draw) {
        widget->on_draw(widget, widget->callback_user_data);
    }
    
#else
    (void)widget;
#endif
}


/* =========================================================================
 * إنشاء وتدمير الواجهات / Widget Creation and Destruction
 * ========================================================================= */

SadWidget* sad_widget_create(SadWidgetType type) {
    SadWidget* widget = (SadWidget*)sad_alloc(sizeof(SadWidget));
    if (!widget) return NULL;
    
    memset(widget, 0, sizeof(SadWidget));
    
    widget->id = generate_widget_id();
    widget->type = type;
    widget->visible = true;
    widget->enabled = true;
    widget->opacity = 1.0f;
    widget->font_size = DEFAULT_FONT_SIZE;
    widget->background_color = SAD_COLOR_TRANSPARENT;
    widget->foreground_color = SAD_COLOR_ON_SURFACE;
    widget->border_color = SAD_COLOR_OUTLINE;
    
    /* إعدادات افتراضية حسب النوع / Default settings based on type */
    switch (type) {
        case SAD_WIDGET_BUTTON:
            widget->background_color = SAD_COLOR_PRIMARY;
            widget->foreground_color = SAD_COLOR_ON_PRIMARY;
            widget->corner_radius = 8.0f;
            widget->padding = SAD_INSETS_SYM(16.0f, 12.0f);
            widget->focusable = true;
            widget->data.button.is_filled = true;
            break;
            
        case SAD_WIDGET_TEXT_INPUT:
            widget->background_color = SAD_COLOR_SURFACE;
            widget->border_width = 1.0f;
            widget->corner_radius = 4.0f;
            widget->padding = SAD_INSETS_ALL(12.0f);
            widget->focusable = true;
            break;
            
        case SAD_WIDGET_CARD:
            widget->background_color = SAD_COLOR_SURFACE;
            widget->corner_radius = 12.0f;
            widget->elevation = 2.0f;
            widget->padding = SAD_INSETS_ALL(16.0f);
            break;
            
        case SAD_WIDGET_COLUMN:
        case SAD_WIDGET_ROW:
            widget->main_align = SAD_MAIN_START;
            widget->cross_align = SAD_CROSS_START;
            break;
            
        case SAD_WIDGET_SLIDER:
            widget->data.slider.min_value = 0.0f;
            widget->data.slider.max_value = 100.0f;
            widget->data.slider.current_value = 50.0f;
            widget->data.slider.step = 1.0f;
            widget->focusable = true;
            break;
            
        case SAD_WIDGET_CHECKBOX:
        case SAD_WIDGET_SWITCH:
            widget->focusable = true;
            break;
            
        case SAD_WIDGET_SCROLL:
            widget->clip_children = true;
            widget->data.scroll.scroll_vertical = true;
            break;
            
        default:
            break;
    }
    
    return widget;
}

void sad_widget_destroy(SadWidget* widget) {
    if (!widget) return;
    
    /* تدمير الأطفال أولاً / Destroy children first */
    for (int32_t i = 0; i < widget->child_count; i++) {
        sad_widget_destroy(widget->children[i]);
    }
    
    sad_free(widget);
}

bool sad_widget_add_child(SadWidget* parent, SadWidget* child) {
    if (!parent || !child) return false;
    if (parent->child_count >= SAD_UI_MAX_CHILDREN) return false;
    
    parent->children[parent->child_count++] = child;
    child->parent = parent;
    
    return true;
}

void sad_widget_remove_child(SadWidget* parent, SadWidget* child) {
    if (!parent || !child) return;
    
    for (int32_t i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == child) {
            /* نقل العناصر للخلف / Shift elements back */
            for (int32_t j = i; j < parent->child_count - 1; j++) {
                parent->children[j] = parent->children[j + 1];
            }
            parent->child_count--;
            child->parent = NULL;
            break;
        }
    }
}

void sad_widget_clear_children(SadWidget* parent) {
    if (!parent) return;
    
    for (int32_t i = 0; i < parent->child_count; i++) {
        if (parent->children[i]) {
            parent->children[i]->parent = NULL;
        }
    }
    parent->child_count = 0;
}


/* =========================================================================
 * دوال بناء الواجهات السريعة / Quick Widget Builders
 * ========================================================================= */

SadWidget* sad_text(const char* text, float font_size, const SadColor* color) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_TEXT);
    if (!widget) return NULL;
    
    if (text) {
        strncpy(widget->text, text, SAD_UI_MAX_TEXT - 1);
        widget->text[SAD_UI_MAX_TEXT - 1] = '\0';
        widget->is_rtl = sad_text_is_rtl(text);
    }
    
    if (font_size > 0) {
        widget->font_size = font_size;
    }
    
    if (color) {
        widget->foreground_color = *color;
    }
    
    return widget;
}

SadWidget* sad_button(const char* text, SadOnClickCallback on_click, void* user_data) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_BUTTON);
    if (!widget) return NULL;
    
    if (text) {
        strncpy(widget->text, text, SAD_UI_MAX_TEXT - 1);
        widget->text[SAD_UI_MAX_TEXT - 1] = '\0';
        widget->is_rtl = sad_text_is_rtl(text);
    }
    
    widget->on_click = on_click;
    widget->callback_user_data = user_data;
    
    return widget;
}

SadWidget* sad_input(const char* placeholder, SadOnTextChangeCallback on_change, void* user_data) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_TEXT_INPUT);
    if (!widget) return NULL;
    
    if (placeholder) {
        strncpy(widget->data.input.placeholder, placeholder, sizeof(widget->data.input.placeholder) - 1);
    }
    
    widget->on_text_change = on_change;
    widget->callback_user_data = user_data;
    
    return widget;
}

SadWidget* sad_column(SadMainAxisAlign main_align, SadCrossAxisAlign cross_align, float spacing) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_COLUMN);
    if (!widget) return NULL;
    
    widget->main_align = main_align;
    widget->cross_align = cross_align;
    widget->spacing = spacing;
    
    return widget;
}

SadWidget* sad_row(SadMainAxisAlign main_align, SadCrossAxisAlign cross_align, float spacing) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_ROW);
    if (!widget) return NULL;
    
    widget->main_align = main_align;
    widget->cross_align = cross_align;
    widget->spacing = spacing;
    
    return widget;
}

SadWidget* sad_stack(void) {
    return sad_widget_create(SAD_WIDGET_STACK);
}

SadWidget* sad_scroll(bool horizontal, bool vertical) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_SCROLL);
    if (!widget) return NULL;
    
    widget->data.scroll.scroll_horizontal = horizontal;
    widget->data.scroll.scroll_vertical = vertical;
    
    return widget;
}

SadWidget* sad_list(float item_height) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_LIST);
    if (!widget) return NULL;
    
    widget->data.list.item_height = item_height > 0 ? item_height : 56.0f;
    
    return widget;
}

SadWidget* sad_card(float elevation) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_CARD);
    if (!widget) return NULL;
    
    widget->elevation = elevation;
    
    return widget;
}

SadWidget* sad_checkbox(const char* label, bool checked, SadOnValueChangeCallback on_change, void* user_data) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_CHECKBOX);
    if (!widget) return NULL;
    
    if (label) {
        strncpy(widget->text, label, SAD_UI_MAX_TEXT - 1);
    }
    
    widget->data.toggle.is_checked = checked;
    widget->on_value_change = on_change;
    widget->callback_user_data = user_data;
    
    return widget;
}

SadWidget* sad_switch(bool is_on, SadOnValueChangeCallback on_change, void* user_data) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_SWITCH);
    if (!widget) return NULL;
    
    widget->data.toggle.is_checked = is_on;
    widget->on_value_change = on_change;
    widget->callback_user_data = user_data;
    
    return widget;
}

SadWidget* sad_slider(float min, float max, float value, SadOnValueChangeCallback on_change, void* user_data) {
    SadWidget* widget = sad_widget_create(SAD_WIDGET_SLIDER);
    if (!widget) return NULL;
    
    widget->data.slider.min_value = min;
    widget->data.slider.max_value = max;
    widget->data.slider.current_value = value;
    widget->on_value_change = on_change;
    widget->callback_user_data = user_data;
    
    return widget;
}


/* =========================================================================
 * دوال تعديل الواجهات / Widget Modification Functions
 * ========================================================================= */

void sad_widget_set_text(SadWidget* widget, const char* text) {
    if (!widget || !text) return;
    strncpy(widget->text, text, SAD_UI_MAX_TEXT - 1);
    widget->text[SAD_UI_MAX_TEXT - 1] = '\0';
    widget->is_rtl = sad_text_is_rtl(text);
}

void sad_widget_set_font_size(SadWidget* widget, float size) {
    if (!widget) return;
    widget->font_size = size > 0 ? size : DEFAULT_FONT_SIZE;
}

void sad_widget_set_background(SadWidget* widget, SadColor color) {
    if (!widget) return;
    widget->background_color = color;
}

void sad_widget_set_foreground(SadWidget* widget, SadColor color) {
    if (!widget) return;
    widget->foreground_color = color;
}

void sad_widget_set_border(SadWidget* widget, SadColor color, float width) {
    if (!widget) return;
    widget->border_color = color;
    widget->border_width = width;
}

void sad_widget_set_corner_radius(SadWidget* widget, float radius) {
    if (!widget) return;
    widget->corner_radius = radius >= 0 ? radius : 0;
}

void sad_widget_set_margin(SadWidget* widget, SadEdgeInsets margin) {
    if (!widget) return;
    widget->margin = margin;
}

void sad_widget_set_padding(SadWidget* widget, SadEdgeInsets padding) {
    if (!widget) return;
    widget->padding = padding;
}

void sad_widget_set_size(SadWidget* widget, float width, float height) {
    if (!widget) return;
    widget->requested_bounds.width = width;
    widget->requested_bounds.height = height;
}

void sad_widget_set_opacity(SadWidget* widget, float opacity) {
    if (!widget) return;
    widget->opacity = (opacity < 0) ? 0 : ((opacity > 1) ? 1 : opacity);
}

void sad_widget_set_enabled(SadWidget* widget, bool enabled) {
    if (!widget) return;
    widget->enabled = enabled;
    widget->state = enabled ? SAD_STATE_NORMAL : SAD_STATE_DISABLED;
}

void sad_widget_set_visible(SadWidget* widget, bool visible) {
    if (!widget) return;
    widget->visible = visible;
}

void sad_widget_set_flex(SadWidget* widget, float flex) {
    if (!widget) return;
    widget->flex = flex >= 0 ? flex : 0;
}


/* =========================================================================
 * الشاشات / Screens
 * ========================================================================= */

SadScreen* sad_screen_create(const char* name) {
    SadScreen* screen = (SadScreen*)sad_alloc(sizeof(SadScreen));
    if (!screen) return NULL;
    
    memset(screen, 0, sizeof(SadScreen));
    
    if (name) {
        strncpy(screen->name, name, sizeof(screen->name) - 1);
    }
    
    screen->background_color = SAD_COLOR_BACKGROUND;
    
    return screen;
}

void sad_screen_destroy(SadScreen* screen) {
    if (!screen) return;
    
    if (screen->root) {
        sad_widget_destroy(screen->root);
    }
    
    sad_free(screen);
}

void sad_screen_set_root(SadScreen* screen, SadWidget* root) {
    if (!screen) return;
    screen->root = root;
}


/* =========================================================================
 * التخطيط / Layout
 * ========================================================================= */

/**
 * حساب تخطيط واجهة
 * Calculate widget layout
 */
void sad_layout_widget(SadWidget* widget, float available_width, float available_height) {
    if (!widget || !widget->visible) return;
    
    /* تطبيق الهوامش / Apply margins */
    float content_x = widget->margin.left;
    float content_y = widget->margin.top;
    float content_width = available_width - widget->margin.left - widget->margin.right;
    float content_height = available_height - widget->margin.top - widget->margin.bottom;
    
    /* إذا كان هناك حجم مطلوب، نستخدمه / Use requested size if set */
    if (widget->requested_bounds.width > 0) {
        content_width = widget->requested_bounds.width;
    }
    if (widget->requested_bounds.height > 0) {
        content_height = widget->requested_bounds.height;
    }
    
    /* تعيين الحدود / Set bounds */
    widget->bounds.x = content_x;
    widget->bounds.y = content_y;
    widget->bounds.width = content_width;
    widget->bounds.height = content_height;
    
    /* المساحة المتاحة للمحتوى (بعد طرح البطانة) / Content area (after padding) */
    float inner_x = widget->padding.left;
    float inner_y = widget->padding.top;
    float inner_width = content_width - widget->padding.left - widget->padding.right;
    float inner_height = content_height - widget->padding.top - widget->padding.bottom;
    
    /* تخطيط الأطفال حسب نوع الحاوية / Layout children based on container type */
    switch (widget->type) {
        case SAD_WIDGET_COLUMN: {
            /* تخطيط عمودي / Vertical layout */
            float y_offset = inner_y;
            float total_flex = 0;
            float fixed_height = 0;
            
            /* حساب المساحة الثابتة والمرنة / Calculate fixed and flex space */
            for (int32_t i = 0; i < widget->child_count; i++) {
                SadWidget* child = widget->children[i];
                if (!child->visible) continue;
                
                if (child->flex > 0) {
                    total_flex += child->flex;
                } else {
                    fixed_height += child->requested_bounds.height > 0 ? 
                                    child->requested_bounds.height : 48.0f; /* حجم افتراضي */
                }
                
                if (i < widget->child_count - 1) {
                    fixed_height += widget->spacing;
                }
            }
            
            float remaining = inner_height - fixed_height;
            float flex_unit = (total_flex > 0 && remaining > 0) ? remaining / total_flex : 0;
            
            /* تخطيط كل طفل / Layout each child */
            for (int32_t i = 0; i < widget->child_count; i++) {
                SadWidget* child = widget->children[i];
                if (!child->visible) continue;
                
                float child_height;
                if (child->flex > 0) {
                    child_height = flex_unit * child->flex;
                } else {
                    child_height = child->requested_bounds.height > 0 ? 
                                   child->requested_bounds.height : 48.0f;
                }
                
                float child_width = inner_width;
                if (widget->cross_align != SAD_CROSS_STRETCH && child->requested_bounds.width > 0) {
                    child_width = child->requested_bounds.width;
                }
                
                /* تعيين موقع الطفل / Set child position */
                float child_x = inner_x;
                switch (widget->cross_align) {
                    case SAD_CROSS_CENTER:
                        child_x = inner_x + (inner_width - child_width) / 2;
                        break;
                    case SAD_CROSS_END:
                        child_x = inner_x + inner_width - child_width;
                        break;
                    default:
                        break;
                }
                
                child->bounds.x = widget->bounds.x + child_x;
                child->bounds.y = widget->bounds.y + y_offset;
                child->bounds.width = child_width;
                child->bounds.height = child_height;
                
                /* تخطيط أطفال الطفل بشكل متكرر / Recursively layout grandchildren */
                sad_layout_widget(child, child_width, child_height);
                
                y_offset += child_height + widget->spacing;
            }
            break;
        }
        
        case SAD_WIDGET_ROW: {
            /* تخطيط أفقي / Horizontal layout */
            float x_offset = inner_x;
            float total_flex = 0;
            float fixed_width = 0;
            
            for (int32_t i = 0; i < widget->child_count; i++) {
                SadWidget* child = widget->children[i];
                if (!child->visible) continue;
                
                if (child->flex > 0) {
                    total_flex += child->flex;
                } else {
                    fixed_width += child->requested_bounds.width > 0 ? 
                                   child->requested_bounds.width : 80.0f;
                }
                
                if (i < widget->child_count - 1) {
                    fixed_width += widget->spacing;
                }
            }
            
            float remaining = inner_width - fixed_width;
            float flex_unit = (total_flex > 0 && remaining > 0) ? remaining / total_flex : 0;
            
            for (int32_t i = 0; i < widget->child_count; i++) {
                SadWidget* child = widget->children[i];
                if (!child->visible) continue;
                
                float child_width;
                if (child->flex > 0) {
                    child_width = flex_unit * child->flex;
                } else {
                    child_width = child->requested_bounds.width > 0 ? 
                                  child->requested_bounds.width : 80.0f;
                }
                
                float child_height = inner_height;
                if (widget->cross_align != SAD_CROSS_STRETCH && child->requested_bounds.height > 0) {
                    child_height = child->requested_bounds.height;
                }
                
                float child_y = inner_y;
                switch (widget->cross_align) {
                    case SAD_CROSS_CENTER:
                        child_y = inner_y + (inner_height - child_height) / 2;
                        break;
                    case SAD_CROSS_END:
                        child_y = inner_y + inner_height - child_height;
                        break;
                    default:
                        break;
                }
                
                child->bounds.x = widget->bounds.x + x_offset;
                child->bounds.y = widget->bounds.y + child_y;
                child->bounds.width = child_width;
                child->bounds.height = child_height;
                
                sad_layout_widget(child, child_width, child_height);
                
                x_offset += child_width + widget->spacing;
            }
            break;
        }
        
        case SAD_WIDGET_STACK: {
            /* تخطيط متراكب / Stack layout - children fill container */
            for (int32_t i = 0; i < widget->child_count; i++) {
                SadWidget* child = widget->children[i];
                if (!child->visible) continue;
                
                child->bounds.x = widget->bounds.x + inner_x;
                child->bounds.y = widget->bounds.y + inner_y;
                child->bounds.width = child->requested_bounds.width > 0 ? 
                                      child->requested_bounds.width : inner_width;
                child->bounds.height = child->requested_bounds.height > 0 ? 
                                       child->requested_bounds.height : inner_height;
                
                sad_layout_widget(child, child->bounds.width, child->bounds.height);
            }
            break;
        }
        
        default:
            /* تخطيط الأطفال بنفس حجم الأب / Default - children get parent size */
            for (int32_t i = 0; i < widget->child_count; i++) {
                SadWidget* child = widget->children[i];
                if (!child->visible) continue;
                
                sad_layout_widget(child, inner_width, inner_height);
            }
            break;
    }
}

void sad_layout_update(SadApp* app) {
    if (!app || !app->current_screen || !app->current_screen->root) return;
    
    sad_layout_widget(app->current_screen->root, 
                      (float)app->screen_width, 
                      (float)app->screen_height);
}


/* =========================================================================
 * معالجة الأحداث / Event Handling
 * ========================================================================= */

/**
 * البحث عن الواجهة عند نقطة معينة
 * Find widget at a given point
 */
static SadWidget* find_widget_at(SadWidget* widget, float x, float y) {
    if (!widget || !widget->visible || !widget->enabled) return NULL;
    
    /* التحقق من الأطفال أولاً (من الأعلى للأسفل) / Check children first (top to bottom) */
    for (int32_t i = widget->child_count - 1; i >= 0; i--) {
        SadWidget* hit = find_widget_at(widget->children[i], x, y);
        if (hit) return hit;
    }
    
    /* التحقق من هذه الواجهة / Check this widget */
    SadRect* b = &widget->bounds;
    if (x >= b->x && x <= b->x + b->width &&
        y >= b->y && y <= b->y + b->height) {
        return widget;
    }
    
    return NULL;
}

void sad_handle_touch(SadApp* app, SadTouchEvent* event) {
    if (!app || !event || !app->current_screen || !app->current_screen->root) return;
    
    SadWidget* hit = find_widget_at(app->current_screen->root, 
                                    event->points[0].x, 
                                    event->points[0].y);
    
    switch (event->type) {
        case SAD_TOUCH_DOWN:
            if (hit && hit->enabled) {
                hit->state = SAD_STATE_PRESSED;
                if (hit->focusable) {
                    app->focused_widget = hit;
                }
            }
            break;
            
        case SAD_TOUCH_UP:
            if (hit && hit->state == SAD_STATE_PRESSED) {
                hit->state = SAD_STATE_NORMAL;
                
                /* إطلاق حدث النقر / Fire click event */
                if (hit->on_click) {
                    hit->on_click(hit, hit->callback_user_data);
                }
                
                /* تبديل حالة checkbox/switch / Toggle checkbox/switch */
                if (hit->type == SAD_WIDGET_CHECKBOX || hit->type == SAD_WIDGET_SWITCH) {
                    hit->data.toggle.is_checked = !hit->data.toggle.is_checked;
                    if (hit->on_value_change) {
                        hit->on_value_change(hit, hit->data.toggle.is_checked ? 1.0f : 0.0f, 
                                             hit->callback_user_data);
                    }
                }
            }
            break;
            
        case SAD_TOUCH_MOVE:
            /* TODO: معالجة السحب للمنزلقات والتمرير / Handle drag for sliders and scrolling */
            break;
            
        case SAD_TOUCH_CANCEL:
            if (hit) {
                hit->state = SAD_STATE_NORMAL;
            }
            break;
    }
}

void sad_handle_key(SadApp* app, int32_t key_code, bool is_down) {
    if (!app || !app->focused_widget) return;
    
    SadWidget* focused = app->focused_widget;
    
    /* معالجة إدخال النص / Handle text input */
    if (focused->type == SAD_WIDGET_TEXT_INPUT && is_down) {
        /* TODO: إضافة معالجة لوحة المفاتيح / Add keyboard handling */
    }
    
    (void)key_code;
}


/* =========================================================================
 * التطبيق / Application
 * ========================================================================= */

SadApp* sad_app_create(void) {
    SadApp* app = (SadApp*)sad_alloc(sizeof(SadApp));
    if (!app) return NULL;
    
    memset(app, 0, sizeof(SadApp));
    
    /* إعدادات الموضوع الافتراضي / Default theme */
    app->theme_primary = SAD_COLOR_PRIMARY;
    app->theme_secondary = SAD_COLOR_SECONDARY;
    app->theme_background = SAD_COLOR_BACKGROUND;
    app->theme_surface = SAD_COLOR_SURFACE;
    app->theme_error = SAD_COLOR_ERROR;
    app->is_dark_mode = false;
    
    app->screen_density = 1.0f;
    
    return app;
}

void sad_app_destroy(SadApp* app) {
    if (!app) return;
    
    /* تدمير الشاشات / Destroy screens */
    for (int32_t i = 0; i < app->screen_count; i++) {
        sad_screen_destroy(app->screens[i]);
    }
    
    sad_free(app);
}

bool sad_app_add_screen(SadApp* app, SadScreen* screen) {
    if (!app || !screen) return false;
    if (app->screen_count >= SAD_UI_MAX_SCREENS) return false;
    
    app->screens[app->screen_count++] = screen;
    
    if (!app->current_screen) {
        app->current_screen = screen;
    }
    
    return true;
}

bool sad_app_navigate_to(SadApp* app, const char* screen_name) {
    if (!app || !screen_name) return false;
    
    for (int32_t i = 0; i < app->screen_count; i++) {
        if (strcmp(app->screens[i]->name, screen_name) == 0) {
            /* استدعاء on_exit للشاشة الحالية / Call on_exit for current screen */
            if (app->current_screen && app->current_screen->on_exit) {
                app->current_screen->on_exit(app->current_screen);
            }
            
            app->current_screen = app->screens[i];
            
            /* استدعاء on_enter للشاشة الجديدة / Call on_enter for new screen */
            if (app->current_screen->on_enter) {
                app->current_screen->on_enter(app->current_screen);
            }
            
            /* إعادة حساب التخطيط / Recalculate layout */
            sad_layout_update(app);
            
            return true;
        }
    }
    
    return false;
}

void sad_app_quit(SadApp* app) {
    if (!app) return;
    app->is_running = false;
}


/* =========================================================================
 * دوال مساعدة / Utility Functions
 * ========================================================================= */

int64_t sad_get_time_ms(void) {
    struct timespec ts;
#ifdef __ANDROID__
    clock_gettime(CLOCK_MONOTONIC, &ts);
#else
    /* fallback للكمبيوتر / Desktop fallback */
    ts.tv_sec = 0;
    ts.tv_nsec = 0;
#endif
    return (int64_t)ts.tv_sec * 1000 + (int64_t)ts.tv_nsec / 1000000;
}

bool sad_text_is_rtl(const char* text) {
    if (!text) return false;
    
    /* نتحقق من أول حرف Unicode / Check first Unicode character */
    const unsigned char* p = (const unsigned char*)text;
    
    while (*p) {
        /* تخطي ASCII والمسافات / Skip ASCII and spaces */
        if (*p < 0x80) {
            p++;
            continue;
        }
        
        /* فك ترميز UTF-8 / Decode UTF-8 */
        uint32_t codepoint = 0;
        
        if ((*p & 0xE0) == 0xC0) {
            codepoint = (*p & 0x1F) << 6;
            p++;
            if (*p) codepoint |= (*p & 0x3F);
            p++;
        } else if ((*p & 0xF0) == 0xE0) {
            codepoint = (*p & 0x0F) << 12;
            p++;
            if (*p) { codepoint |= (*p & 0x3F) << 6; p++; }
            if (*p) { codepoint |= (*p & 0x3F); p++; }
        } else if ((*p & 0xF8) == 0xF0) {
            codepoint = (*p & 0x07) << 18;
            p++;
            if (*p) { codepoint |= (*p & 0x3F) << 12; p++; }
            if (*p) { codepoint |= (*p & 0x3F) << 6; p++; }
            if (*p) { codepoint |= (*p & 0x3F); p++; }
        } else {
            p++;
            continue;
        }
        
        /* نطاقات RTL: عربي، عبري، سريانية، ثعنة / RTL ranges: Arabic, Hebrew, Syriac, Thaana */
        if ((codepoint >= 0x0600 && codepoint <= 0x06FF) ||  /* Arabic */
            (codepoint >= 0x0750 && codepoint <= 0x077F) ||  /* Arabic Supplement */
            (codepoint >= 0x08A0 && codepoint <= 0x08FF) ||  /* Arabic Extended-A */
            (codepoint >= 0xFB50 && codepoint <= 0xFDFF) ||  /* Arabic Presentation Forms-A */
            (codepoint >= 0xFE70 && codepoint <= 0xFEFF) ||  /* Arabic Presentation Forms-B */
            (codepoint >= 0x0590 && codepoint <= 0x05FF) ||  /* Hebrew */
            (codepoint >= 0x0700 && codepoint <= 0x074F) ||  /* Syriac */
            (codepoint >= 0x0780 && codepoint <= 0x07BF)) {  /* Thaana */
            return true;
        }
        
        /* إذا وجدنا حرف LTR، نعيد false / If we find LTR char, return false */
        if ((codepoint >= 0x0041 && codepoint <= 0x005A) ||  /* A-Z */
            (codepoint >= 0x0061 && codepoint <= 0x007A)) {  /* a-z */
            return false;
        }
    }
    
    return false;
}

SadWidget* sad_widget_find_by_tag(SadWidget* root, const char* tag) {
    if (!root || !tag) return NULL;
    
    if (strcmp(root->tag, tag) == 0) {
        return root;
    }
    
    for (int32_t i = 0; i < root->child_count; i++) {
        SadWidget* found = sad_widget_find_by_tag(root->children[i], tag);
        if (found) return found;
    }
    
    return NULL;
}
