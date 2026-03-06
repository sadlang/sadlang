/**
 * اختبار نظام Flutter-like لواجهات لغة ص
 * Test for Sad Flutter-like UI system
 * + اختبار خدمات النظام: شبكة، ملفات، قواعد بيانات
 */
#include <stdio.h>
#include <stdlib.h>
#include "sad_android_bridge_v2.h"
#include "sad_ui_engine.h"
#include "sad_network.h"
#include "sad_files.h"
#include "sad_database.h"
#include "sad_navigator.h"

static int g_clickCount = 0;
static مقبض_ملفات* g_files = NULL;
static مقبض_قاعدة* g_db = NULL;
static مقبض_شبكة* g_network = NULL;

/* Forward declarations */
static void on_button_click(SadWidgetId id, void* userData);

/* دوال التنقل */
static void on_settings_click(SadWidgetId id, void* userData) {
    printf("الانتقال للإعدادات...\n");
    اذهب_إلى("/الإعدادات");
}

static void on_network_click(SadWidgetId id, void* userData) {
    printf("الانتقال لاختبار الشبكة...\n");
    اذهب_إلى("/الشبكة");
}

static void on_back_click(SadWidgetId id, void* userData) {
    printf("الرجوع للشاشة الرئيسية...\n");
    ارجع();
}

/* بناء الشاشة الرئيسية */
static SadWidgetId build_main_screen(const SadNavParams* params) {
    (void)params;
    
    SadWidgetId root = sadui_column();
    SadLayout rootLayout = {0};
    rootLayout.width  = (SadSize){SAD_SIZE_FILL, 0};
    rootLayout.height = (SadSize){SAD_SIZE_FILL, 0};
    rootLayout.mainAxis  = SAD_ALIGN_START;
    rootLayout.crossAxis = SAD_ALIGN_CENTER;
    rootLayout.padding   = sadui_edges(24);
    rootLayout.gap       = 16;
    sadui_set_layout(root, &rootLayout);

    sadui_add_child(root, sadui_heading("🏠 الشاشة الرئيسية"));
    sadui_add_child(root, sadui_text("مرحباً بك في التطبيق"));
    sadui_add_child(root, sadui_divider());

    /* بطاقة الخدمات */
    SadWidgetId svcCard = sadui_card();
    sadui_add_child(root, svcCard);
    SadLayout svcLayout = {0};
    svcLayout.width  = (SadSize){SAD_SIZE_FILL, 0};
    svcLayout.height = (SadSize){SAD_SIZE_WRAP, 0};
    svcLayout.padding = sadui_edges(16);
    svcLayout.gap = 6;
    svcLayout.crossAxis = SAD_ALIGN_CENTER;
    sadui_set_layout(svcCard, &svcLayout);

    sadui_add_child(svcCard, sadui_heading("خدمات النظام"));
    sadui_add_child(svcCard, sadui_text(g_files ? "✓ الملفات: جاهز" : "✗ الملفات: فشل"));
    sadui_add_child(svcCard, sadui_text(g_db    ? "✓ قاعدة البيانات: جاهز" : "✗ قاعدة البيانات: فشل"));

    /* زر الإعدادات */
    SadWidgetId settingsBtn = sadui_button("⚙ الإعدادات");
    sadui_on_click(settingsBtn, on_settings_click, NULL);
    sadui_add_child(root, settingsBtn);

    /* زر اختبار */
    SadWidgetId testBtn = sadui_button("اختبر الضغط");
    sadui_on_click(testBtn, on_button_click, NULL);
    sadui_add_child(root, testBtn);

    /* زر اختبار الشبكة */
    SadWidgetId networkBtn = sadui_button("🌐 اختبار الشبكة");
    sadui_on_click(networkBtn, on_network_click, NULL);
    sadui_add_child(root, networkBtn);

    return root;
}

/* متغيرات نتائج الشبكة */
static char g_network_result[512] = "لم يُختبر بعد";
static int g_http_code = 0;

/* اختبار GET */
static void on_test_get(SadWidgetId id, void* userData) {
    (void)id; (void)userData;
    
    if (!g_network) {
        g_network = أنشئ_شبكة();
    }
    
    /* استخدام httpbin.org للاختبار */
    استجابة_شبكة رد = أرسل_طلب(g_network, "http://httpbin.org/ip");
    
    if (رد.الحالة == حالة_نجاح) {
        snprintf(g_network_result, sizeof(g_network_result),
            "✓ نجح! كود: %d\nالجسم: %.100s", 
            رد.كود_الحالة, رد.الجسم ? رد.الجسم : "(فارغ)");
        g_http_code = رد.كود_الحالة;
    } else {
        snprintf(g_network_result, sizeof(g_network_result),
            "✗ فشل! الحالة: %d\nخطأ: %s", 
            (int)رد.الحالة, رد.رسالة_الخطأ ? رد.رسالة_الخطأ : "غير معروف");
        g_http_code = 0;
    }
    
    حرر_استجابة(&رد);
    
    /* إعادة بناء الشاشة */
    اذهب_إلى("/الشبكة");
}

/* اختبار POST */  
static void on_test_post(SadWidgetId id, void* userData) {
    (void)id; (void)userData;
    
    if (!g_network) {
        g_network = أنشئ_شبكة();
    }
    
    const char* json_body = "{\"name\":\"لغة ص\",\"version\":\"1.0\",\"arabic\":true}";
    استجابة_شبكة رد = أرسل_POST(g_network, "http://httpbin.org/post", json_body);
    
    if (رد.الحالة == حالة_نجاح) {
        snprintf(g_network_result, sizeof(g_network_result),
            "✓ POST نجح! كود: %d\nطول الرد: %zu", 
            رد.كود_الحالة, رد.حجم_الجسم);
        g_http_code = رد.كود_الحالة;
    } else {
        snprintf(g_network_result, sizeof(g_network_result),
            "✗ POST فشل! الحالة: %d", (int)رد.الحالة);
        g_http_code = 0;
    }
    
    حرر_استجابة(&رد);
    اذهب_إلى("/الشبكة");
}

/* بناء شاشة الشبكة */
static SadWidgetId build_network_screen(const SadNavParams* params) {
    (void)params;
    
    SadWidgetId root = sadui_column();
    SadLayout rootLayout = {0};
    rootLayout.width  = (SadSize){SAD_SIZE_FILL, 0};
    rootLayout.height = (SadSize){SAD_SIZE_FILL, 0};
    rootLayout.mainAxis  = SAD_ALIGN_START;
    rootLayout.crossAxis = SAD_ALIGN_CENTER;
    rootLayout.padding   = sadui_edges(24);
    rootLayout.gap       = 16;
    sadui_set_layout(root, &rootLayout);

    sadui_add_child(root, sadui_heading("🌐 اختبار الشبكة"));
    sadui_add_child(root, sadui_text("اختبار طلبات HTTP"));
    sadui_add_child(root, sadui_divider());

    /* بطاقة النتائج */
    SadWidgetId resultCard = sadui_card();
    sadui_add_child(root, resultCard);
    SadLayout cardLayout = {0};
    cardLayout.width  = (SadSize){SAD_SIZE_FILL, 0};
    cardLayout.height = (SadSize){SAD_SIZE_WRAP, 0};
    cardLayout.padding = sadui_edges(16);
    cardLayout.gap = 8;
    cardLayout.crossAxis = SAD_ALIGN_START;
    sadui_set_layout(resultCard, &cardLayout);

    sadui_add_child(resultCard, sadui_heading("النتيجة:"));
    sadui_add_child(resultCard, sadui_text(g_network_result));

    /* أزرار الاختبار */
    SadWidgetId btnRow = sadui_row();
    sadui_add_child(root, btnRow);
    SadLayout rowLayout = {0};
    rowLayout.width  = (SadSize){SAD_SIZE_WRAP, 0};
    rowLayout.height = (SadSize){SAD_SIZE_WRAP, 0};
    rowLayout.gap = 12;
    rowLayout.crossAxis = SAD_ALIGN_CENTER;
    sadui_set_layout(btnRow, &rowLayout);

    SadWidgetId getBtn = sadui_button("GET");
    sadui_on_click(getBtn, on_test_get, NULL);
    sadui_add_child(btnRow, getBtn);

    SadWidgetId postBtn = sadui_button("POST");
    sadui_on_click(postBtn, on_test_post, NULL);
    sadui_add_child(btnRow, postBtn);

    /* chips للمعلومات */
    SadWidgetId chipRow = sadui_row();
    sadui_add_child(root, chipRow);
    SadLayout chipLayout = {0};
    chipLayout.width = (SadSize){SAD_SIZE_WRAP, 0};
    chipLayout.height = (SadSize){SAD_SIZE_WRAP, 0};
    chipLayout.gap = 8;
    sadui_set_layout(chipRow, &chipLayout);

    sadui_add_child(chipRow, sadui_chip("HTTP/1.1"));
    sadui_add_child(chipRow, sadui_chip("BSD Sockets"));
    sadui_add_child(chipRow, sadui_chip(g_http_code == 200 ? "200 OK" : "منتظر"));

    /* زر الرجوع */
    SadWidgetId backBtn = sadui_button("← الرجوع");
    sadui_on_click(backBtn, on_back_click, NULL);
    sadui_add_child(root, backBtn);

    return root;
}

/* بناء شاشة الإعدادات */
static SadWidgetId build_settings_screen(const SadNavParams* params) {
    (void)params;
    
    SadWidgetId root = sadui_column();
    SadLayout rootLayout = {0};
    rootLayout.width  = (SadSize){SAD_SIZE_FILL, 0};
    rootLayout.height = (SadSize){SAD_SIZE_FILL, 0};
    rootLayout.mainAxis  = SAD_ALIGN_START;
    rootLayout.crossAxis = SAD_ALIGN_CENTER;
    rootLayout.padding   = sadui_edges(24);
    rootLayout.gap       = 16;
    sadui_set_layout(root, &rootLayout);

    sadui_add_child(root, sadui_heading("⚙ الإعدادات"));
    sadui_add_child(root, sadui_text("تخصيص التطبيق"));
    sadui_add_child(root, sadui_divider());

    /* بطاقة الإعدادات */
    SadWidgetId card = sadui_card();
    sadui_add_child(root, card);
    SadLayout cardLayout = {0};
    cardLayout.width  = (SadSize){SAD_SIZE_FILL, 0};
    cardLayout.height = (SadSize){SAD_SIZE_WRAP, 0};
    cardLayout.padding = sadui_edges(16);
    cardLayout.gap = 8;
    cardLayout.crossAxis = SAD_ALIGN_START;
    sadui_set_layout(card, &cardLayout);

    sadui_add_child(card, sadui_text("• اللغة: العربية"));
    sadui_add_child(card, sadui_text("• السمة: فاتحة"));
    sadui_add_child(card, sadui_text("• الإشعارات: مفعّلة"));

    /* chips للإعدادات */
    SadWidgetId chipRow = sadui_row();
    sadui_add_child(root, chipRow);
    SadLayout chipLayout = {0};
    chipLayout.width = (SadSize){SAD_SIZE_WRAP, 0};
    chipLayout.height = (SadSize){SAD_SIZE_WRAP, 0};
    chipLayout.gap = 8;
    chipLayout.crossAxis = SAD_ALIGN_CENTER;
    sadui_set_layout(chipRow, &chipLayout);

    sadui_add_child(chipRow, sadui_chip("حسابي"));
    sadui_add_child(chipRow, sadui_chip("الأمان"));
    sadui_add_child(chipRow, sadui_chip("المساعدة"));

    /* زر الرجوع */
    SadWidgetId backBtn = sadui_button("← الرجوع");
    sadui_on_click(backBtn, on_back_click, NULL);
    sadui_add_child(root, backBtn);

    return root;
}

/* callback عند الضغط على الزر */
static void on_button_click(SadWidgetId id, void* userData) {
    g_clickCount++;
    printf("تم الضغط على الزر! العدد: %d\n", g_clickCount);
    
    /* اختبار الملفات */
    if (g_files) {
        char msg[128];
        snprintf(msg, sizeof(msg), "الضغطة رقم %d", g_clickCount);
        اكتب_ملف_نصي(g_files, "سجل.txt", msg);
        
        char* قراءة = اقرأ_ملف_نصي(g_files, "سجل.txt");
        if (قراءة) {
            printf("محتوى الملف: %s\n", قراءة);
            free(قراءة);
        }
    }
    
    /* اختبار قاعدة البيانات */
    if (g_db) {
        char sql[256];
        snprintf(sql, sizeof(sql),
            "INSERT INTO سجل_الأحداث (الحدث, العدد) VALUES ('ضغط_زر', %d);",
            g_clickCount);
        نفذ_أمر(g_db, sql);
        
        نتيجة_استعلام ن = نفذ_استعلام(g_db,
            "SELECT COUNT(*) as العدد FROM سجل_الأحداث;");
        if (ن.الحالة == قاعدة_نجاح && ن.عدد_الصفوف > 0) {
            printf("عدد الأحداث المسجلة: %s\n",
                   ن.الصفوف[0].القيم[0]);
        }
        حرر_نتيجة(&ن);
    }
}

/* الدالة الرئيسية — يستدعيها الجسر في الوضع البسيط */
void __sad_main(void) {
    printf("مرحباً من لغة ص — نظام Flutter-like!\n");
}

/* دالة بناء الواجهة */
void __sad_ui_main(SadAndroidApp* app) {
    (void)app;

    /* تهيئة خدمات النظام */
    g_files = أنشئ_نظام_ملفات("/data/data/com.sad.flutter/files");
    
    g_db = افتح_قاعدة_بيانات("/data/data/com.sad.flutter/files/تطبيقي.db");
    if (g_db) {
        نفذ_أمر(g_db, "CREATE TABLE IF NOT EXISTS سجل_الأحداث ("
                       "المعرف INTEGER PRIMARY KEY AUTOINCREMENT, "
                       "الحدث TEXT, "
                       "العدد INTEGER, "
                       "الوقت DATETIME DEFAULT CURRENT_TIMESTAMP);");
    }

    /* تهيئة نظام التنقل */
    متنقل_تهيئة();
    
    /* تهيئة الشبكة */
    g_network = أنشئ_شبكة();
    
    /* تسجيل الشاشات */
    متنقل_سجل("/الرئيسية", build_main_screen);
    متنقل_سجل("/الإعدادات", build_settings_screen);
    متنقل_سجل("/الشبكة", build_network_screen);
    
    /* بدء التطبيق بالشاشة الرئيسية */
    اذهب_إلى("/الرئيسية");
}
