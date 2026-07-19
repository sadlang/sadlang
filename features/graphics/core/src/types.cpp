/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ملف: types.cpp
 * المسار: features/graphics/core/src/types.cpp
 * ═══════════════════════════════════════════════════════════════════════════════
 *
 * تنفيذ دوال تحويل الأنواع (أسماء عربية ↔ enum).
 *
 * هذا الملف يُنفّذ الدوال المعلنة في types.h لتحويل أنواع العناصر
 * والمعدّلات بين قيم enum وأسمائها العربية.
 *
 * حقوق النشر (c) 2024-2026 فريق لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_ui/types.h"
#include "sad_ui/prop_keys.h" // (AR) SoT مفاتيح الخصائص المولَّد (props::PADDING …)
#include "sad_ui/text_normalize.h" // (AR) تجريد التشكيل من اسم الحدث قبل المطابقة
// (AR) قائمة X-macro لأنواع الأحداث مولَّدة من language-truth/ui_events.yaml.
#include "sad_ui/generated/event_vocab_generated.h"
// (AR) جداول الألوان الموحَّدة مولَّدة من language-truth/ui_colors.yaml.
#include "sad_ui/generated/color_table_generated.h"

#include <unordered_map>

namespace sad
{
    namespace ui
    {

        // ═══════════════════════════════════════════════════════════════════════════════
        // جداول التحويل — أنواع العناصر
        // ═══════════════════════════════════════════════════════════════════════════════

        /// جدول: UINodeType → اسم عربي
        static const std::unordered_map<UINodeType, std::string> &getNodeTypeNames()
        {
            static const std::unordered_map<UINodeType, std::string> table = {
                // ─── عناصر العرض ──────────
                {UINodeType::Text, "نص"},
                {UINodeType::Image, "صورة"},
                {UINodeType::Icon, "أيقونة"},
                // ─── عناصر الإدخال ─────────
                {UINodeType::Button, "زر"},
                {UINodeType::TextField, "حقل"},
                {UINodeType::TextArea, "منطقة_نص"},
                {UINodeType::Toggle, "مفتاح"},
                {UINodeType::Slider, "منزلق"},
                {UINodeType::Picker, "قائمة_اختيار"},
                {UINodeType::DatePicker, "قائمة_تاريخ"},
                {UINodeType::ColorPicker, "قائمة_لون"},
                {UINodeType::Checkbox, "مربع_اختيار"},
                {UINodeType::Radio, "زر_راديو"},
                // ─── حاويات التخطيط ───────
                {UINodeType::Column, "عمود"},
                {UINodeType::Row, "صف"},
                {UINodeType::Stack, "تكديس"},
                {UINodeType::Grid, "شبكة"},
                {UINodeType::LazyColumn, "عمود_كسول"},
                {UINodeType::LazyRow, "صف_كسول"},
                {UINodeType::LazyGrid, "شبكة_كسولة"},
                {UINodeType::Spacer, "فاصل"},
                {UINodeType::Divider, "خط_فاصل"},
                {UINodeType::Wrap, "التفاف"},
                // ─── حاويات مركبة ─────────
                {UINodeType::Container, "حاوية"},
                {UINodeType::Card, "بطاقة"},
                {UINodeType::Surface, "سطح"},
                {UINodeType::Scaffold, "هيكل"},
                {UINodeType::Box, "صندوق"},
                // ─── التنقل ───────────────
                {UINodeType::NavigationView, "عرض_تنقل"},
                {UINodeType::NavigationLink, "رابط_تنقل"},
                {UINodeType::TabView, "عرض_علامات"},
                {UINodeType::TabItem, "علامة"},
                // ─── القوائم ──────────────
                {UINodeType::List, "قائمة"},
                {UINodeType::Section, "قسم"},
                {UINodeType::ForEach, "لكل"},
                // ─── النوافذ المنبثقة ─────
                {UINodeType::Alert, "تنبيه"},
                {UINodeType::Sheet, "ورقة"},
                {UINodeType::Dialog, "حوار"},
                {UINodeType::Menu, "قائمة_خيارات"},
                // ─── عناصر خاصة ──────────
                {UINodeType::Canvas, "لوحة"},
                {UINodeType::MapView, "خريطة"},
                {UINodeType::WebView, "عرض_ويب"},
                {UINodeType::VideoPlayer, "مشغل_فيديو"},
                {UINodeType::ScrollView, "عرض_تمرير"},
                {UINodeType::CustomWidget, "عنصر_مخصص"},
                // ─── بنيوية ──────────────
                {UINodeType::Conditional, "شرطي"},
                {UINodeType::Group, "مجموعة"},
                // ─── مكونات جديدة (v2) ────
                {UINodeType::ProgressBar, "شريط_تقدم"},
                {UINodeType::Badge, "شارة"},
                {UINodeType::Chip, "رقاقة"},
                {UINodeType::Avatar, "صورة_رمزية"},
                {UINodeType::SearchBar, "حقل_بحث"},
                {UINodeType::BottomSheet, "لوحة_سفلية"},
                {UINodeType::FAB, "زر_عائم"},
                {UINodeType::SnackBar, "رسالة_منبثقة"},
                {UINodeType::Drawer, "درج"},
                {UINodeType::AppBar, "شريط_تطبيق"},
                // ─── بيانات (v3) ─────────
                {UINodeType::Tooltip, "تلميح"},
                {UINodeType::DataTable, "جدول_بيانات"},
                {UINodeType::TreeView, "عرض_شجري"},
                {UINodeType::Breadcrumb, "مسار_تنقل"},
                {UINodeType::Pagination, "ترقيم_صفحات"},
                {UINodeType::Timeline, "خط_زمني"},
                // ─── وسائط (v3) ──────────
                {UINodeType::Carousel, "عرض_دوار"},
                {UINodeType::RichText, "نص_منسق"},
                {UINodeType::Markdown, "ماركداون"},
                {UINodeType::CodeBlock, "كتلة_كود"},
                {UINodeType::AudioPlayer, "مشغل_صوت"},
                {UINodeType::ImageGallery, "معرض_صور"},
                // ─── حركة (v3) ───────────
                {UINodeType::Skeleton, "هيكل_تحميل"},
                {UINodeType::Shimmer, "وميض"},
                {UINodeType::AnimatedList, "قائمة_متحركة"},
                {UINodeType::Expandable, "أكورديون"},
                {UINodeType::Collapsible, "قابل_للطي"},
                {UINodeType::Swipeable, "قابل_للسحب"},
                // ─── تحكم متقدم (v3) ────
                {UINodeType::Stepper, "خطوات"},
                {UINodeType::SegmentedControl, "تحكم_مقسم"},
                {UINodeType::RatingBar, "تقييم"},
                {UINodeType::TimePicker, "منتقي_وقت"},
                {UINodeType::Calendar, "تقويم"},
                {UINodeType::ColorWheel, "عجلة_ألوان"},
                // ─── تنقل متقدم (v3) ────
                {UINodeType::BottomNav, "تنقل_سفلي"},
                {UINodeType::SideNav, "تنقل_جانبي"},
                {UINodeType::Toolbar, "شريط_أدوات"},
                {UINodeType::SplitView, "عرض_مقسم"},
                {UINodeType::Tabs, "ألسنة"},
                {UINodeType::MegaMenu, "قائمة_ضخمة"},
                // ─── تخطيط أساسي Flutter (v4) ────
                {UINodeType::Center, "وسط"},
                {UINodeType::Padding, "حشوة"},
                {UINodeType::SizedBox, "مقاس_محدد"},
                {UINodeType::Expanded, "موسع"},
                {UINodeType::Flexible, "مرن"},
                {UINodeType::Align, "محاذي"},
                {UINodeType::SafeArea, "منطقة_آمنة"},
                {UINodeType::GestureDetector, "كاشف_إيماءات"},
                {UINodeType::InkWell, "حبر"},
                {UINodeType::ListView, "قائمة_عرض"},
                {UINodeType::FractionallySizedBox, "صندوق_نسبي"},
                {UINodeType::ConstrainedBox, "صندوق_مقيد"},
                {UINodeType::AspectRatio, "نسبة_عرض"},
            };
            return table;
        }

        /// جدول معكوس: اسم عربي → UINodeType
        static const std::unordered_map<std::string, UINodeType> &getNodeTypeByName()
        {
            static std::unordered_map<std::string, UINodeType> table;
            static bool initialized = false;
            if (!initialized)
            {
                for (const auto &[type, name] : getNodeTypeNames())
                {
                    table[name] = type;
                }
                table["\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb7"] = UINodeType::Center;                                   // توسيط → وسط
                table["\xd9\x85\xd8\xb1\xd9\x86"] = UINodeType::Flexible;                                                 // مرن → مرن
                table["\xd9\x85\xd9\x88\xd8\xb3\xd9\x91\xd8\xb9"] = UINodeType::Expanded;                                 // موسّع → موسع
                table["\xd8\xad\xd8\xb4\xd9\x88"] = UINodeType::Padding;                                                  // حشو → حشوة
                table["\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd9\x84\xd9\x84\xd9\x86\xd9\x82\xd8\xb1"] = UINodeType::InkWell; // قابل_للنقر → حبر
                // ─── أسماء بديلة للتوافق ───
                table["\xd8\xb7\xd8\xa8\xd9\x82\xd8\xa7\xd8\xaa"] = UINodeType::Stack;                                               // طبقات (قديم)
                table["\xd8\xad\xd9\x82\xd9\x84_\xd9\x86\xd8\xb5"] = UINodeType::TextField;                                          // حقل_نص (قديم)
                table["\xd8\xae\xd8\xa7\xd9\x86\xd8\xa9_\xd8\xa7\xd8\xae\xd8\xaa\xd9\x8a\xd8\xa7\xd8\xb1"] = UINodeType::Checkbox;   // خانة_اختيار (قديم)
                table["\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa8\xd8\xad\xd8\xab"] = UINodeType::SearchBar;                          // شريط_بحث (قديم)
                table["\xd9\x88\xd8\xb1\xd9\x82\xd8\xa9_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a\xd8\xa9"] = UINodeType::BottomSheet;        // ورقة_سفلية (قديم)
                table["\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xa5\xd8\xb4\xd8\xb9\xd8\xa7\xd8\xb1"] = UINodeType::SnackBar;           // شريط_إشعار (قديم)
                table["\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd9\x82\xd9\x8a\xd9\x8a\xd9\x85"] = UINodeType::RatingBar;          // شريط_تقييم (قديم)
                table["\xd9\x82\xd8\xa7\xd8\xa8\xd9\x84_\xd9\x84\xd9\x84\xd8\xaa\xd9\x88\xd8\xb3\xd8\xb9"] = UINodeType::Expandable; // قابل_للتوسع (قديم)
                table["\xd9\x87\xd9\x8a\xd9\x83\xd9\x84_\xd8\xb9\xd8\xb8\xd9\x85\xd9\x8a"] = UINodeType::Skeleton;                   // هيكل_عظمي (بديل)
                // ─── أسماء إضافية من ملفات ص ───
                table["\xd8\xb9\xd9\x86\xd9\x88\xd8\xa7\xd9\x86"] = UINodeType::Text;                                                    // عنوان → نص
                table["\xd8\xaa\xd8\xb3\xd9\x85\xd9\x8a\xd8\xa9"] = UINodeType::Text;                                                    // تسمية → نص
                table["\xd9\x81\xd9\x82\xd8\xb1\xd8\xa9"] = UINodeType::Text;                                                            // فقرة → نص
                table["\xd8\xb1\xd8\xa7\xd8\xa8\xd8\xb7"] = UINodeType::Text;                                                            // رابط → نص
                table["\xd9\x83\xd9\x88\xd8\xaf"] = UINodeType::CodeBlock;                                                               // كود → كتلة_كود
                table["\xd8\xa7\xd9\x82\xd8\xaa\xd8\xa8\xd8\xa7\xd8\xb3"] = UINodeType::Text;                                            // اقتباس → نص
                table["\xd8\xb2\xd8\xb1_\xd9\x85\xd8\xad\xd9\x8a\xd8\xb7"] = UINodeType::Button;                                         // زر_محيط → زر
                table["\xd8\xb2\xd8\xb1_\xd9\x86\xd8\xb5\xd9\x8a"] = UINodeType::Button;                                                 // زر_نصي → زر
                table["\xd8\xb2\xd8\xb1_\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9"] = UINodeType::Button;                         // زر_ايقونة → زر
                table["\xd8\xb2\xd8\xb1_\xd8\xaa\xd8\xa8\xd8\xaf\xd9\x8a\xd9\x84"] = UINodeType::Toggle;                                 // زر_تبديل → تبديل
                table["\xd8\xad\xd9\x82\xd9\x84_\xd8\xb3\xd8\xb1"] = UINodeType::TextField;                                              // حقل_سر → حقل
                table["\xd9\x85\xd9\x86\xd8\xb7\xd9\x82\xd8\xa9_\xd9\x86\xd8\xb5"] = UINodeType::TextArea;                               // منطقة_نص → منطقة_نص
                table["\xd8\xaa\xd9\x82\xd8\xaf\xd9\x85_\xd8\xaf\xd8\xa7\xd8\xa6\xd8\xb1\xd9\x8a"] = UINodeType::ProgressBar;            // تقدم_دائري → شريط_تقدم
                table["\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84"] = UINodeType::Skeleton;                                                // تحميل → هيكل_تحميل
                table["\xd9\x88\xd8\xb3\xd9\x85"] = UINodeType::Chip;                                                                    // وسم → رقاقة
                table["\xd8\xaa\xd9\x88\xd8\xb3\xd9\x8a\xd8\xb7"] = UINodeType::Center;                                                  // توسيط → وسط (مُحدَّث)
                table["\xd9\x85\xd8\xb1\xd9\x86"] = UINodeType::Flexible;                                                                // مرن → مرن (مُحدَّث)
                table["\xd9\x85\xd9\x88\xd8\xb3\xd9\x91\xd8\xb9"] = UINodeType::Expanded;                                                // موسّع → موسع (مُحدَّث)
                table["\xd8\xae\xd8\xb7_\xd9\x81\xd8\xa7\xd8\xb5\xd9\x84"] = UINodeType::Divider;                                        // خط_فاصل → فاصل_خط
                table["\xd8\xad\xd9\x88\xd8\xa7\xd8\xb1_\xd8\xaa\xd8\xa3\xd9\x83\xd9\x8a\xd8\xaf"] = UINodeType::Dialog;                 // حوار_تأكيد → حوار
                table["\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd9\x86\xd8\xb3\xd8\xaf\xd9\x84\xd8\xa9"] = UINodeType::Picker; // قائمة_منسدلة → منتقي
                table["\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd9\x85\xd9\x86\xd8\xa8\xd8\xab\xd9\x82\xd8\xa9"] = UINodeType::Menu;   // قائمة_منبثقة → قائمة_خيارات
                table["\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9"] = UINodeType::List;                   // عنصر_قائمة → قائمة
                table["\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xb4\xd8\xa8\xd9\x83\xd9\x8a\xd8\xa9"] = UINodeType::LazyGrid;       // قائمة_شبكية
                table["\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1"] = UINodeType::ScrollView;     // قائمة_تمرير
                table["\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8"] = UINodeType::Tabs;                   // شريط_تبويب → ألسنة
                table["\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8"] = UINodeType::TabItem;                                                 // تبويب → علامة
                table["\xd9\x85\xd8\xad\xd8\xaa\xd9\x88\xd9\x89_\xd8\xaa\xd8\xa8\xd9\x88\xd9\x8a\xd8\xa8"] = UINodeType::TabView;        // محتوى_تبويب
                table["\xd8\xb4\xd8\xb1\xd9\x8a\xd8\xb7_\xd8\xb3\xd9\x81\xd9\x84\xd9\x8a"] = UINodeType::BottomNav;                      // شريط_سفلي
                table["\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xaa\xd9\x86\xd9\x82\xd9\x84"] = UINodeType::TabItem;                        // عنصر_تنقل
                table["\xd8\xb9\xd9\x86\xd8\xb5\xd8\xb1_\xd8\xaf\xd8\xb1\xd8\xac"] = UINodeType::List;                                   // عنصر_درج
                table["\xd8\xa7\xd9\x8a\xd9\x82\xd9\x88\xd9\x86\xd8\xa9"] = UINodeType::Icon;                                            // ايقونة (بدون أ)
                initialized = true;
            }
            return table;
        }

        std::string nodeTypeToArabicName(UINodeType type)
        {
            const auto &table = getNodeTypeNames();
            auto it = table.find(type);
            if (it != table.end())
            {
                return it->second;
            }
            return "غير_معروف";
        }

        std::optional<UINodeType> arabicNameToNodeType(const std::string &name)
        {
            const auto &table = getNodeTypeByName();
            auto it = table.find(name);
            if (it != table.end())
            {
                return it->second;
            }
            return std::nullopt;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // جداول التحويل — أنواع المعدّلات
        // ═══════════════════════════════════════════════════════════════════════════════

        /// جدول: ModifierType → اسم عربي
        static const std::unordered_map<ModifierType, std::string> &getModifierTypeNames()
        {
            static const std::unordered_map<ModifierType, std::string> table = {
                // ─── الألوان ──────────────
                {ModifierType::ForegroundColor, "لون"},
                {ModifierType::BackgroundColor, "لون_خلفية"},
                {ModifierType::TintColor, "لون_تظليل"},
                {ModifierType::GradientColor, "تدرج"},
                // ─── الخط ─────────────────
                {ModifierType::FontSize, "حجم_خط"},
                {ModifierType::FontWeight, "وزن_خط"},
                {ModifierType::FontFamily, "عائلة_خط"},
                {ModifierType::FontStyle, "نمط_خط"},
                {ModifierType::TextAlign, "محاذاة_نص"},
                {ModifierType::LineSpacing, "تباعد_أسطر"},
                {ModifierType::TextDecoration, "زخرفة_نص"},
                // ─── الأبعاد ──────────────
                {ModifierType::Width, "عرض"},
                {ModifierType::Height, "ارتفاع"},
                {ModifierType::MinWidth, "أدنى_عرض"},
                {ModifierType::MinHeight, "أدنى_ارتفاع"},
                {ModifierType::MaxWidth, "أقصى_عرض"},
                {ModifierType::MaxHeight, "أقصى_ارتفاع"},
                // ─── المسافات ─────────────
                {ModifierType::Padding, props::PADDING}, // SoT: القانونيّ «حشوة»
                {ModifierType::PaddingTop, props::PADDING_TOP},       // SoT «حشوة_أعلى»
                {ModifierType::PaddingBottom, props::PADDING_BOTTOM}, // SoT «حشوة_أسفل»
                {ModifierType::PaddingStart, props::PADDING_START},   // SoT «حشوة_بداية» (منطقيّ RTL)
                {ModifierType::PaddingEnd, props::PADDING_END},       // SoT «حشوة_نهاية» (منطقيّ RTL)
                {ModifierType::Margin, "هامش"},
                // ─── الحدود ───────────────
                {ModifierType::Border, "حد"},
                {ModifierType::BorderRadius, "نصف_قطر"},
                {ModifierType::BorderWidth, "سمك_حد"},
                {ModifierType::BorderColor, "لون_حد"},
                // ─── الموضع ───────────────
                {ModifierType::Alignment, "محاذاة"},
                {ModifierType::Position, "موقع"},
                {ModifierType::Offset, "إزاحة"},
                // ─── التأثيرات ────────────
                {ModifierType::Shadow, "ظل"},
                {ModifierType::Opacity, "عتامة"},
                {ModifierType::Blur, "ضبابية"},
                {ModifierType::ClipShape, "شكل_قص"},
                // ─── التحويلات ────────────
                {ModifierType::Rotation, "دوران"},
                {ModifierType::Scale, "مقياس"},
                {ModifierType::Translation, "ترجمة"},
                // ─── الأحداث ──────────────
                {ModifierType::OnTap, "عند_النقر"},
                {ModifierType::OnLongPress, "عند_الضغط_المطول"},
                {ModifierType::OnDrag, "عند_السحب"},
                {ModifierType::OnAppear, "عند_الظهور"},
                {ModifierType::OnDisappear, "عند_الاختفاء"},
                {ModifierType::OnValueChange, "عند_التغيير"},
                // ─── سهولة الوصول ─────────
                {ModifierType::AccessibilityLabel, "تسمية_وصول"},
                {ModifierType::AccessibilityHint, "تلميح_وصول"},
                // ─── الرسوم المتحركة ─────
                {ModifierType::Animation, "رسوم_متحركة"},
                {ModifierType::Transition, "انتقال"},
            };
            return table;
        }

        /// جدول معكوس: اسم عربي → ModifierType
        static const std::unordered_map<std::string, ModifierType> &getModifierTypeByName()
        {
            static std::unordered_map<std::string, ModifierType> table;
            static bool initialized = false;
            if (!initialized)
            {
                for (const auto &[type, name] : getModifierTypeNames())
                {
                    table[name] = type;
                }
                initialized = true;
            }
            return table;
        }

        std::string modifierTypeToArabicName(ModifierType type)
        {
            const auto &table = getModifierTypeNames();
            auto it = table.find(type);
            if (it != table.end())
            {
                return it->second;
            }
            return "غير_معروف";
        }

        std::optional<ModifierType> arabicNameToModifierType(const std::string &name)
        {
            const auto &table = getModifierTypeByName();
            auto it = table.find(name);
            if (it != table.end())
            {
                return it->second;
            }
            return std::nullopt;
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // جداول التحويل — أنواع أحداث IR (IREventType)
        // ═══════════════════════════════════════════════════════════════════════════════

        /// جدول: IREventType → الاسم العربي الأساسي (مولَّد من ui_events.yaml)
        static const std::unordered_map<IREventType, std::string> &getIREventTypeNames()
        {
            static const std::unordered_map<IREventType, std::string> table = {
#define X(id, str) {IREventType::id, str},
                SAD_UI_EVENT_VOCAB(X)
#undef X
            };
            return table;
        }

        /// جدول معكوس: اسم عربيّ قانونيّ → IREventType (مولَّد؛ لا بدائل)
        static const std::unordered_map<std::string, IREventType> &getIREventTypeByName()
        {
            static const std::unordered_map<std::string, IREventType> table = {
#define X(id, str) {str, IREventType::id},
                SAD_UI_EVENT_VOCAB(X)
#undef X
            };
            return table;
        }

        IREventType stringToIREventType(const std::string &name)
        {
            // (AR) اسمُ الحدث وسيطٌ نصّيّ قد يحمل تشكيلًا — نجرّده قبل البحث فيستوي
            //   «عند_التغيّر» و«عند_التغير». لا بدائل إنجليزيّة (قانونيّ فقط).
            const std::string key = stripArabicDiacritics(name);
            const auto &table = getIREventTypeByName();
            auto it = table.find(key);
            if (it != table.end())
            {
                return it->second;
            }
            return IREventType::Custom;
        }

        const std::string &irEventTypeToString(IREventType type)
        {
            static const std::string unknown = "\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5"; // مخصص
            const auto &table = getIREventTypeNames();
            auto it = table.find(type);
            if (it != table.end())
            {
                return it->second;
            }
            return unknown;
        }

        bool isKnownEventName(const std::string &name)
        {
            return stringToIREventType(name) != IREventType::Custom;
        }

        IREventType modifierTypeToIREventType(ModifierType mod)
        {
            switch (mod)
            {
            case ModifierType::OnTap:
                return IREventType::OnTap;
            case ModifierType::OnLongPress:
                return IREventType::OnLongPress;
            case ModifierType::OnDrag:
                return IREventType::OnDrag;
            case ModifierType::OnAppear:
                return IREventType::OnAppear;
            case ModifierType::OnDisappear:
                return IREventType::OnDisappear;
            case ModifierType::OnValueChange:
                return IREventType::OnChange;
            default:
                return IREventType::Custom;
            }
        }

        // ═══════════════════════════════════════════════════════════════════════════════
        // جداول التحويل — الألوان المسماة
        // ═══════════════════════════════════════════════════════════════════════════════

        /// جدول: اسم عربيّ قانونيّ → NamedColor (مولَّد من ui_colors.yaml؛ لا بدائل)
        static const std::unordered_map<std::string, NamedColor> &getColorByName()
        {
            static const std::unordered_map<std::string, NamedColor> table = {
#define X(name, id) {name, NamedColor::id},
                SAD_UI_COLOR_BY_NAME(X)
#undef X
            };
            return table;
        }

        std::optional<NamedColor> arabicNameToColor(const std::string &name)
        {
            const auto &table = getColorByName();
            auto it = table.find(name);
            if (it != table.end())
            {
                return it->second;
            }
            return std::nullopt;
        }

        uint32_t namedColorToRGBA(NamedColor color)
        {
            // (AR) مولَّد من ui_colors.yaml (SAD_UI_COLOR_RGBA) — مصدرٌ واحد مع
            //   getColorByName والجدول النصّيّ، فلا تباعد صامت.
            switch (color)
            {
#define X(id, rgba) case NamedColor::id: return rgba;
                SAD_UI_COLOR_RGBA(X)
#undef X
            default:
                return 0x000000FF;
            }
        }

    } // namespace ui
} // namespace sad
