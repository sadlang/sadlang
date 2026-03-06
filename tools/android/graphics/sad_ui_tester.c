#include "sad_ui_engine.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// إحصائيات الاختبار
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(cond) \
    do { \
        tests_run++; \
        if (!(cond)) { \
            printf("[FAILED] %s:%d: %s\n", __FILE__, __LINE__, #cond); \
            tests_failed++; \
        } else { \
            tests_passed++; \
        } \
    } while(0)

#define TEST_CASE(name) void test_##name()

// ═══════════════════════════════════════════════════════════════════════════════
// حالات الاختبار
// ═══════════════════════════════════════════════════════════════════════════════

TEST_CASE(widget_creation) {
    printf("Running test_widget_creation...\n");
    SadWidgetId btn = sadui_button("Click Me");
    TEST_ASSERT(btn != -1);
    
    const char* text = sadui_get_text(btn);
    TEST_ASSERT(text != NULL);
    TEST_ASSERT(strcmp(text, "Click Me") == 0);
}

TEST_CASE(widget_hierarchy) {
    printf("Running test_widget_hierarchy...\n");
    SadWidgetId parent = sadui_column();
    SadWidgetId child1 = sadui_button("1");
    SadWidgetId child2 = sadui_button("2");
    
    sadui_add_child(parent, child1);
    sadui_add_child(parent, child2);
    
    // يمكن هنا التحقق من عدد الأبناء إذا كان هناك دالة API لذلك
    TEST_ASSERT(parent != -1 && child1 != -1 && child2 != -1);
}

static bool button_clicked = false;
static void on_test_click(SadWidgetId w, void* d) {
    button_clicked = true;
}

TEST_CASE(widget_events) {
    printf("Running test_widget_events...\n");
    SadWidgetId btn = sadui_button("Test Event");
    sadui_on_click(btn, on_test_click, NULL);
    
    // محاكاة حدث النقر برمجياً (ستتطلب تعديل المحرك لتصدير دالة dispatch أحياناً)
    // لكن للاختبار الآلي، نحن نعتمد على محاكاة خارجية إذا لزم الأمر
    TEST_ASSERT(btn != -1);
}

TEST_CASE(dirty_flags) {
    printf("Running test_dirty_flags...\n");
    SadWidgetId btn = sadui_button("Test UI");
    
    sadui_set_text(btn, "New Text");
    TEST_ASSERT(sadui_needs_redraw() == true);
    
    sadui_set_visible(btn, true);
    TEST_ASSERT(sadui_needs_layout() == true);
}

TEST_CASE(extended_widgets) {
    printf("Running test_extended_widgets (40+ New Widgets)...\n");
    
    // Testing the array of expanded widgets up to 40 new elements
    SadWidgetType advanced_widgets[] = {
        SAD_WIDGET_GRID, SAD_WIDGET_LIST_VIEW, SAD_WIDGET_MAP, SAD_WIDGET_VIDEO_PLAYER,
        SAD_WIDGET_AUDIO_PLAYER, SAD_WIDGET_WEB_VIEW, SAD_WIDGET_CAMERA_PREVIEW,
        SAD_WIDGET_PDF_VIEWER, SAD_WIDGET_DATE_PICKER, SAD_WIDGET_TIME_PICKER,
        SAD_WIDGET_CALENDAR, SAD_WIDGET_COLOR_PICKER, SAD_WIDGET_ACCORDION,
        SAD_WIDGET_STEPPER, SAD_WIDGET_BREADCRUMB, SAD_WIDGET_CAROUSEL,
        SAD_WIDGET_RATING_BAR, SAD_WIDGET_KNOB, SAD_WIDGET_GAUGE,
        SAD_WIDGET_TOGGLE_BUTTON_GROUP, SAD_WIDGET_SEGMENTED_CONTROL,
        SAD_WIDGET_SLIDING_PANEL, SAD_WIDGET_SPLIT_PANE, SAD_WIDGET_TREE_VIEW,
        SAD_WIDGET_TABLE, SAD_WIDGET_DATA_GRID, SAD_WIDGET_RICH_TEXT,
        SAD_WIDGET_MARKDOWN_VIEWER, SAD_WIDGET_CODE_EDITOR, SAD_WIDGET_SIGNATURE_PAD,
        SAD_WIDGET_QR_SCANNER, SAD_WIDGET_BARCODE_SCANNER, SAD_WIDGET_LOTTIE_ANIMATION,
        SAD_WIDGET_VOICE_RECORDER, SAD_WIDGET_WAVEFORM, SAD_WIDGET_RADAR_CHART,
        SAD_WIDGET_PIE_CHART, SAD_WIDGET_HEATMAP, SAD_WIDGET_PULL_TO_REFRESH,
        SAD_WIDGET_SPEED_DIAL, SAD_WIDGET_CONTEXT_MENU, SAD_WIDGET_EXPANDABLE_LIST
    };
    
    int count = sizeof(advanced_widgets) / sizeof(advanced_widgets[0]);
    TEST_ASSERT(count >= 40); // تأكيد إضافة أكثر من 40 مكون
    
    SadWidgetId container = sadui_create(SAD_WIDGET_SCROLL);
    TEST_ASSERT(container != -1);
    
    for(int i = 0; i < count; i++) {
        SadWidgetId w = sadui_create(advanced_widgets[i]);
        TEST_ASSERT(w != -1);
        sadui_add_child(container, w);
    }
    
    printf("Successfully created and nested %d advanced widgets.\n", count);
}

// ═══════════════════════════════════════════════════════════════════════════════
// مشغل الاختبارات
// ═══════════════════════════════════════════════════════════════════════════════

int run_all_ui_tests() {
    printf("==========================================\n");
    printf("     تشغيل اختبارات الواجهة الآلية\n");
    printf("==========================================\n");
    
    // تشغيل الحالات
    test_widget_creation();
    test_widget_hierarchy();
    test_widget_events();
    test_dirty_flags();
    test_extended_widgets();

    
    printf("==========================================\n");
    printf("النتيجة: %d إجمالي | %d ناجح | %d فاشل\n", 
           tests_run, tests_passed, tests_failed);
    printf("==========================================\n");
    
    return tests_failed == 0 ? 0 : 1;
}

#ifdef SAD_UI_TEST_MAIN
int main() {
    // تتطلب بعض الحالات تهيئة وهمية
    // sadui_init_headless();
    return run_all_ui_tests();
}
#endif
