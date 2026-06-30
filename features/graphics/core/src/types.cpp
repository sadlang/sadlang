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
                {ModifierType::Padding, "حشوة"},
                {ModifierType::PaddingTop, "حشوة_أعلى"},
                {ModifierType::PaddingBottom, "حشوة_أسفل"},
                {ModifierType::PaddingStart, "حشوة_بداية"},
                {ModifierType::PaddingEnd, "حشوة_نهاية"},
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

        /// جدول: IREventType → الاسم العربي الأساسي
        static const std::unordered_map<IREventType, std::string> &getIREventTypeNames()
        {
            static const std::unordered_map<IREventType, std::string> table = {
                {IREventType::OnTap, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x86\xd9\x82\xd8\xb1"},                                                                                          // عند_النقر
                {IREventType::OnDoubleTap, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x86\xd9\x82\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb2\xd8\xaf\xd9\x88\xd8\xac"},                           // عند_النقر_المزدوج
                {IREventType::OnLongPress, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb6\xd8\xba\xd8\xb7_\xd8\xa7\xd9\x84\xd9\x85\xd8\xb7\xd9\x88\xd9\x84"},                                   // عند_الضغط_المطول
                {IREventType::OnDrag, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8"},                                                                                         // عند_السحب
                {IREventType::OnSwipeLeft, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8_\xd9\x8a\xd8\xb3\xd8\xa7\xd8\xb1"},                                                   // عند_السحب_يسار
                {IREventType::OnSwipeRight, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8_\xd9\x8a\xd9\x85\xd9\x8a\xd9\x86"},                                                  // عند_السحب_يمين
                {IREventType::OnSwipeUp, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8_\xd8\xa3\xd8\xb9\xd9\x84\xd9\x89"},                                                     // عند_السحب_أعلى
                {IREventType::OnSwipeDown, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8_\xd8\xa3\xd8\xb3\xd9\x81\xd9\x84"},                                                   // عند_السحب_أسفل
                {IREventType::OnZoom, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd9\x83\xd8\xa8\xd9\x8a\xd8\xb1"},                                                                         // عند_التكبير
                {IREventType::OnChange, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1"},                                                                       // عند_التغيير
                {IREventType::OnInput, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xaf\xd8\xae\xd8\xa7\xd9\x84"},                                                                        // عند_الإدخال
                {IREventType::OnRelease, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd8\xb1"},                                                                      // عند_التحرير
                {IREventType::OnScroll, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd9\x85\xd8\xb1\xd9\x8a\xd8\xb1"},                                                                       // عند_التمرير
                {IREventType::OnHover, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x88\xd9\x8a\xd9\x85"},                                                                        // عند_التحويم
                {IREventType::OnHoverExit, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb1\xd9\x88\xd8\xac"},                                                                            // عند_الخروج
                {IREventType::OnFocus, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2"},                                                                        // عند_التركيز
                {IREventType::OnBlur, "\xd8\xb9\xd9\x86\xd8\xaf_\xd9\x81\xd9\x82\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xb1\xd9\x83\xd9\x8a\xd8\xb2"},                                                // عند_فقد_التركيز
                {IREventType::OnAppear, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb8\xd9\x87\xd9\x88\xd8\xb1"},                                                                               // عند_الظهور
                {IREventType::OnDisappear, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa7\xd8\xae\xd8\xaa\xd9\x81\xd8\xa7\xd8\xa1"},                                                            // عند_الاختفاء
                {IREventType::OnKeyDown, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xb6\xd8\xba\xd8\xb7_\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad"},                                                             // عند_ضغط_مفتاح
                {IREventType::OnKeyUp, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xb1\xd9\x81\xd8\xb9_\xd9\x85\xd9\x81\xd8\xaa\xd8\xa7\xd8\xad"},                                                               // عند_رفع_مفتاح
                {IREventType::OnSubmit, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb1\xd8\xb3\xd8\xa7\xd9\x84"},                                                                       // عند_الإرسال
                {IREventType::OnDragStart, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa8\xd8\xaf\xd8\xa1_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8"},                                                           // عند_بدء_السحب
                {IREventType::OnDragEnd, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x86\xd8\xaa\xd9\x87\xd8\xa7\xd8\xa1_\xd8\xa7\xd9\x84\xd8\xb3\xd8\xad\xd8\xa8"},                                     // عند_انتهاء_السحب
                {IREventType::OnDrop, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xa5\xd8\xb3\xd9\x82\xd8\xa7\xd8\xb7"},                                                                         // عند_الإسقاط
                {IREventType::OnRotate, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaf\xd9\x88\xd8\xb1\xd8\xa7\xd9\x86"},                                                                       // عند_الدوران
                {IREventType::OnContextMenu, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x85\xd8\xa9_\xd8\xa7\xd9\x84\xd8\xb3\xd9\x8a\xd8\xa7\xd9\x82\xd9\x8a\xd8\xa9"}, // عند_القائمة_السياقية
                {IREventType::OnSelect, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd8\xaf\xd9\x8a\xd8\xaf"},                                                                       // عند_التحديد
                {IREventType::OnResize, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xaa\xd8\xba\xd9\x8a\xd9\x8a\xd8\xb1_\xd8\xa7\xd9\x84\xd8\xad\xd8\xac\xd9\x85"},                                              // عند_تغيير_الحجم
                {IREventType::OnAnimationEnd, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x86\xd8\xaa\xd9\x87\xd8\xa7\xd8\xa1_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd9\x83"},                // عند_انتهاء_التحريك
                {IREventType::OnLoad, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xaa\xd8\xad\xd9\x85\xd9\x8a\xd9\x84"},                                                                         // عند_التحميل
                {IREventType::OnError, "\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xae\xd8\xb7\xd8\xa3"},                                                                                        // عند_الخطأ
                {IREventType::Custom, "\xd9\x85\xd8\xae\xd8\xb5\xd8\xb5"},                                                                                                                          // مخصص
            };
            return table;
        }

        /// جدول معكوس: اسم (عربي أو إنجليزي أو مستعار) → IREventType
        static const std::unordered_map<std::string, IREventType> &getIREventTypeByName()
        {
            static std::unordered_map<std::string, IREventType> table;
            static bool initialized = false;
            if (!initialized)
            {
                // ─── الأسماء العربية الأساسية ───
                for (const auto &[type, name] : getIREventTypeNames())
                {
                    if (type != IREventType::Custom)
                    {
                        table[name] = type;
                    }
                }
                // ─── الأسماء المستعارة العربية ───
                table["\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xa7\xd9\x84\xd8\xb6\xd8\xba\xd8\xb7"] = IREventType::OnTap;                                                     // عند_الضغط → عند_النقر
                table["\xd8\xb9\xd9\x86\xd8\xaf_\xd8\xaa\xd8\xba\xd9\x8a\xd9\x91\xd8\xb1_\xd8\xa7\xd9\x84\xd9\x82\xd9\x8a\xd9\x85\xd8\xa9"] = IREventType::OnChange; // عند_تغيّر_القيمة

                // ─── الأسماء الإنجليزية ───
                table["onTap"] = IREventType::OnTap;
                table["onClick"] = IREventType::OnTap;
                table["on_tap"] = IREventType::OnTap;
                table["on_click"] = IREventType::OnTap;
                table["onDoubleTap"] = IREventType::OnDoubleTap;
                table["on_double_tap"] = IREventType::OnDoubleTap;
                table["onLongPress"] = IREventType::OnLongPress;
                table["on_long_press"] = IREventType::OnLongPress;
                table["onDrag"] = IREventType::OnDrag;
                table["onSwipe"] = IREventType::OnDrag;
                table["on_drag"] = IREventType::OnDrag;
                table["onSwipeLeft"] = IREventType::OnSwipeLeft;
                table["onSwipeRight"] = IREventType::OnSwipeRight;
                table["onSwipeUp"] = IREventType::OnSwipeUp;
                table["onSwipeDown"] = IREventType::OnSwipeDown;
                table["onZoom"] = IREventType::OnZoom;
                table["onPinch"] = IREventType::OnZoom;
                table["on_zoom"] = IREventType::OnZoom;
                table["onChange"] = IREventType::OnChange;
                table["on_change"] = IREventType::OnChange;
                table["onInput"] = IREventType::OnInput;
                table["on_input"] = IREventType::OnInput;
                table["onRelease"] = IREventType::OnRelease;
                table["on_release"] = IREventType::OnRelease;
                table["onScroll"] = IREventType::OnScroll;
                table["on_scroll"] = IREventType::OnScroll;
                table["onHover"] = IREventType::OnHover;
                table["onMouseEnter"] = IREventType::OnHover;
                table["on_hover"] = IREventType::OnHover;
                table["onHoverExit"] = IREventType::OnHoverExit;
                table["onMouseLeave"] = IREventType::OnHoverExit;
                table["on_hover_exit"] = IREventType::OnHoverExit;
                table["onFocus"] = IREventType::OnFocus;
                table["on_focus"] = IREventType::OnFocus;
                table["onBlur"] = IREventType::OnBlur;
                table["on_blur"] = IREventType::OnBlur;
                table["onAppear"] = IREventType::OnAppear;
                table["on_appear"] = IREventType::OnAppear;
                table["onDisappear"] = IREventType::OnDisappear;
                table["on_disappear"] = IREventType::OnDisappear;
                table["onKeyDown"] = IREventType::OnKeyDown;
                table["on_key_down"] = IREventType::OnKeyDown;
                table["onKeyUp"] = IREventType::OnKeyUp;
                table["on_key_up"] = IREventType::OnKeyUp;

                // ─── الأحداث الجديدة ───
                table["onSubmit"] = IREventType::OnSubmit;
                table["on_submit"] = IREventType::OnSubmit;
                table["onDragStart"] = IREventType::OnDragStart;
                table["on_drag_start"] = IREventType::OnDragStart;
                table["onDragEnd"] = IREventType::OnDragEnd;
                table["on_drag_end"] = IREventType::OnDragEnd;
                table["onDrop"] = IREventType::OnDrop;
                table["on_drop"] = IREventType::OnDrop;
                table["onRotate"] = IREventType::OnRotate;
                table["on_rotate"] = IREventType::OnRotate;
                table["onContextMenu"] = IREventType::OnContextMenu;
                table["on_context_menu"] = IREventType::OnContextMenu;
                table["onRightClick"] = IREventType::OnContextMenu;
                table["onSelect"] = IREventType::OnSelect;
                table["on_select"] = IREventType::OnSelect;
                table["onResize"] = IREventType::OnResize;
                table["on_resize"] = IREventType::OnResize;
                table["onAnimationEnd"] = IREventType::OnAnimationEnd;
                table["on_animation_end"] = IREventType::OnAnimationEnd;
                table["onLoad"] = IREventType::OnLoad;
                table["on_load"] = IREventType::OnLoad;
                table["onError"] = IREventType::OnError;
                table["on_error"] = IREventType::OnError;

                initialized = true;
            }
            return table;
        }

        IREventType stringToIREventType(const std::string &name)
        {
            const auto &table = getIREventTypeByName();
            auto it = table.find(name);
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

        /// جدول: اسم عربي/إنجليزي → NamedColor
        static const std::unordered_map<std::string, NamedColor> &getColorByName()
        {
            static const std::unordered_map<std::string, NamedColor> table = {
                // ─── ألوان عربية ──────────
                {"أسود", NamedColor::Black},
                {"أبيض", NamedColor::White},
                {"أحمر", NamedColor::Red},
                {"أخضر", NamedColor::Green},
                {"أزرق", NamedColor::Blue},
                {"أصفر", NamedColor::Yellow},
                {"برتقالي", NamedColor::Orange},
                {"بنفسجي", NamedColor::Purple},
                {"وردي", NamedColor::Pink},
                {"بني", NamedColor::Brown},
                {"رمادي", NamedColor::Gray},
                {"رمادي_فاتح", NamedColor::LightGray},
                {"رمادي_غامق", NamedColor::DarkGray},
                {"سماوي", NamedColor::Cyan},
                {"أزرق_مخضر", NamedColor::Teal},
                {"شفاف", NamedColor::Transparent},
                {"أساسي", NamedColor::Primary},
                {"ثانوي", NamedColor::Secondary},
                {"سطح", NamedColor::Surface},
                {"خلفية", NamedColor::Background},
                {"خطأ", NamedColor::Error},
                // ─── ألوان إنجليزية ───────
                {"black", NamedColor::Black},
                {"white", NamedColor::White},
                {"red", NamedColor::Red},
                {"green", NamedColor::Green},
                {"blue", NamedColor::Blue},
                {"yellow", NamedColor::Yellow},
                {"orange", NamedColor::Orange},
                {"purple", NamedColor::Purple},
                {"pink", NamedColor::Pink},
                {"brown", NamedColor::Brown},
                {"gray", NamedColor::Gray},
                {"grey", NamedColor::Grey},
                {"lightgray", NamedColor::LightGray},
                {"darkgray", NamedColor::DarkGray},
                {"cyan", NamedColor::Cyan},
                {"teal", NamedColor::Teal},
                {"transparent", NamedColor::Transparent},
                {"primary", NamedColor::Primary},
                {"secondary", NamedColor::Secondary},
                {"surface", NamedColor::Surface},
                {"background", NamedColor::Background},
                {"error", NamedColor::Error},
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
            switch (color)
            {
            case NamedColor::Black:
                return 0x000000FF;
            case NamedColor::White:
                return 0xFFFFFFFF;
            case NamedColor::Red:
                return 0xF44336FF;
            case NamedColor::Green:
                return 0x4CAF50FF;
            case NamedColor::Blue:
                return 0x2196F3FF;
            case NamedColor::Yellow:
                return 0xFFEB3BFF;
            case NamedColor::Orange:
                return 0xFF9800FF;
            case NamedColor::Purple:
                return 0x9C27B0FF;
            case NamedColor::Pink:
                return 0xE91E63FF;
            case NamedColor::Brown:
                return 0x795548FF;
            case NamedColor::Gray:
                return 0x9E9E9EFF;
            case NamedColor::Grey:
                return 0x9E9E9EFF;
            case NamedColor::LightGray:
                return 0xBDBDBDFF;
            case NamedColor::DarkGray:
                return 0x616161FF;
            case NamedColor::Cyan:
                return 0x00BCD4FF;
            case NamedColor::Teal:
                return 0x009688FF;
            case NamedColor::Transparent:
                return 0x00000000;
            case NamedColor::Primary:
                return 0x1E88E5FF;
            case NamedColor::Secondary:
                return 0x43A047FF;
            case NamedColor::Surface:
                return 0xFFFFFFFF;
            case NamedColor::Background:
                return 0xFAFAFAFF;
            case NamedColor::OnPrimary:
                return 0xFFFFFFFF;
            case NamedColor::OnSecondary:
                return 0xFFFFFFFF;
            case NamedColor::OnSurface:
                return 0x212121FF;
            case NamedColor::OnBackground:
                return 0x212121FF;
            case NamedColor::Error:
                return 0xE53935FF;
            case NamedColor::OnError:
                return 0xFFFFFFFF;
            default:
                return 0x000000FF;
            }
        }

    } // namespace ui
} // namespace sad
