/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * بسم الله الرحمن الرحيم
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * @file style_checker_provider.cpp
 * @brief تنفيذ مدقق الأنماط والممارسات الأفضل للغة ص
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 *                        تنفيذ نظام مدقق الأنماط الثوري
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على التنفيذ الكامل لنظام فحص الأنماط. يتضمن:
 * - تنفيذ جميع قواعد النمط الافتراضية
 * - خوارزميات الفحص والتحليل
 * - نظام الإصلاح التلقائي
 * - توليد التقارير
 * 
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "style_checker_provider.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>

namespace sad {
namespace lsp {

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ الدوال المساعدة العامة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تحويل مستوى الخطورة إلى نص عربي قابل للقراءة
 * 
 * هذه الدالة تحول قيمة التعداد إلى نص مفهوم للمستخدم.
 * مفيدة جداً في توليد التقارير ورسائل الخطأ.
 * 
 * @param الخطورة مستوى الخطورة
 * @return نص يصف مستوى الخطورة
 */
std::string خطورة_إلى_نص(StyleSeverity الخطورة) {
    switch (الخطورة) {
        case StyleSeverity::معلومة: return "معلومة";
        case StyleSeverity::تلميح:  return "تلميح";
        case StyleSeverity::تحذير:  return "تحذير";
        case StyleSeverity::خطأ:    return "خطأ";
        default:                    return "غير معروف";
    }
}

/**
 * @brief تحويل فئة القاعدة إلى نص عربي
 * 
 * تساعد هذه الدالة في تصنيف المشاكل وعرضها بشكل منظم.
 * 
 * @param الفئة فئة القاعدة
 * @return نص يصف الفئة
 */
std::string فئة_إلى_نص(StyleCategory الفئة) {
    switch (الفئة) {
        case StyleCategory::تسمية:   return "قواعد التسمية";
        case StyleCategory::تنسيق:   return "قواعد التنسيق";
        case StyleCategory::توثيق:   return "قواعد التوثيق";
        case StyleCategory::تعقيد:   return "قواعد التعقيد";
        case StyleCategory::ممارسات: return "الممارسات الأفضل";
        case StyleCategory::أداء:    return "قواعد الأداء";
        case StyleCategory::أمان:    return "قواعد الأمان";
        case StyleCategory::نظافة:   return "نظافة الكود";
        default:                     return "فئة غير معروفة";
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ المُنشئات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief المُنشئ الافتراضي
 * 
 * يقوم هذا المُنشئ بتهيئة مدقق الأنماط مع الإعدادات الافتراضية
 * وتحميل جميع قواعد النمط المضمنة. الإعدادات الافتراضية توازن
 * بين الصرامة والمرونة لتناسب معظم المشاريع.
 */
StyleCheckerProvider::StyleCheckerProvider() {
    // تهيئة الإعدادات الافتراضية - تم تعيينها في تعريف البنية
    
    // تحميل القواعد الافتراضية
    تهيئة_القواعد_الافتراضية_();
}

/**
 * @brief المُنشئ مع إعدادات مخصصة
 * 
 * يسمح هذا المُنشئ بتخصيص سلوك المدقق بالكامل.
 * مفيد للمشاريع التي لها متطلبات خاصة.
 * 
 * @param config الإعدادات المخصصة
 */
StyleCheckerProvider::StyleCheckerProvider(const StyleCheckerConfig& config)
    : الإعدادات_(config) {
    
    تهيئة_القواعد_الافتراضية_();
    
    // تعطيل القواعد المحددة في الإعدادات
    for (const auto& معرف : الإعدادات_.القواعد_المعطلة) {
        تعطيل_قاعدة(معرف);
    }
}

/**
 * @brief تحميل الإعدادات من ملف خارجي
 * 
 * يدعم ملفات JSON للإعدادات. هذا يسمح للمشاريع بتخزين
 * إعدادات النمط في ملف يمكن مشاركته مع الفريق.
 * 
 * مثال لملف الإعدادات (sadstyle.json):
 * {
 *   "الحد_الأقصى_لطول_السطر": 120,
 *   "تفضيل_الأسماء_العربية": true,
 *   "القواعد_المعطلة": ["تسمية-003"]
 * }
 * 
 * @param config_path مسار ملف الإعدادات
 * @return true إذا تم التحميل بنجاح
 */
bool StyleCheckerProvider::تحميل_الإعدادات(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        return false;
    }
    
    // قراءة محتوى الملف
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // تحليل JSON البسيط
    // ملاحظة: هذا تحليل مبسط، في الإنتاج نستخدم مكتبة JSON كاملة
    
    // البحث عن "الحد_الأقصى_لطول_السطر"
    auto pos = content.find("\"الحد_الأقصى_لطول_السطر\"");
    if (pos != std::string::npos) {
        auto colon = content.find(':', pos);
        auto comma = content.find_first_of(",}", colon);
        if (colon != std::string::npos && comma != std::string::npos) {
            std::string value = content.substr(colon + 1, comma - colon - 1);
            // إزالة المسافات
            value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
            الإعدادات_.الحد_الأقصى_لطول_السطر = std::stoi(value);
        }
    }
    
    // يمكن إضافة المزيد من الإعدادات هنا...
    
    return true;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تهيئة القواعد الافتراضية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تهيئة جميع قواعد النمط الافتراضية
 * 
 * هذه الدالة الضخمة تقوم بتسجيل جميع قواعد النمط المضمنة.
 * كل قاعدة لها معرف فريد ووصف ودالة فحص.
 * 
 * ─────────────────────────────────────────────────────────────────────────────
 * تنظيم معرفات القواعد:
 * ─────────────────────────────────────────────────────────────────────────────
 * - تسمية-XXX: قواعد التسمية
 * - تنسيق-XXX: قواعد التنسيق
 * - توثيق-XXX: قواعد التوثيق
 * - تعقيد-XXX: قواعد التعقيد
 * - ممارسة-XXX: الممارسات الأفضل
 * - أداء-XXX: قواعد الأداء
 * - أمان-XXX: قواعد الأمان
 * ─────────────────────────────────────────────────────────────────────────────
 */
void StyleCheckerProvider::تهيئة_القواعد_الافتراضية_() {
    
    // ═════════════════════════════════════════════════════════════════════════
    //                           قواعد التسمية
    // ═════════════════════════════════════════════════════════════════════════
    
    // ─────────────────────────────────────────────────────────────────────────
    // قاعدة تسمية-001: الأسماء القصيرة جداً
    // ─────────────────────────────────────────────────────────────────────────
    {
        StyleRule rule;
        rule.المعرف = "تسمية-001";
        rule.الاسم = "الأسماء القصيرة";
        rule.الوصف = "الأسماء يجب أن تكون واضحة ومعبرة، الأسماء المكونة من حرف أو حرفين غالباً تكون غامضة";
        rule.الخطورة_الافتراضية = StyleSeverity::تحذير;
        rule.الفئة = StyleCategory::تسمية;
        rule.مفعلة = true;
        
        rule.فحص = [this](const std::string& الكود) -> std::vector<StyleIssue> {
            std::vector<StyleIssue> issues;
            
            // استخراج المعرفات من الكود
            auto identifiers = استخراج_المعرفات_(الكود);
            
            for (const auto& [اسم, موقع] : identifiers) {
                // تجاهل الكلمات المفتاحية والحالات الخاصة
                if (اسم == "س" || اسم == "ص" || اسم == "ع") {
                    // حروف شائعة في الرياضيات - تحذير خفيف
                    if (اسم.length() <= 2) {
                        StyleIssue issue;
                        issue.معرف_القاعدة = "تسمية-001";
                        issue.الرسالة = "الاسم '" + اسم + "' قصير جداً وقد يكون غامضاً";
                        issue.الشرح = "استخدم اسماً أكثر وضوحاً يصف الغرض من المتغير. "
                                     "مثلاً بدلاً من 'س' استخدم 'العدد_الأول' أو 'المسافة'.";
                        issue.الخطورة = StyleSeverity::تلميح;
                        issue.الفئة = StyleCategory::تسمية;
                        issue.الموقع = موقع;
                        
                        // اقتراح إصلاح
                        StyleFix fix;
                        fix.وصف = "استخدم اسماً أوضح";
                        fix.النص_القديم = اسم;
                        fix.النص_الجديد = اسم + "_القيمة";
                        fix.آمن = false; // يحتاج مراجعة
                        issue.الإصلاحات.push_back(fix);
                        
                        issues.push_back(issue);
                    }
                }
            }
            
            return issues;
        };
        
        القواعد_.push_back(rule);
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // قاعدة تسمية-002: تفضيل الأسماء العربية
    // ─────────────────────────────────────────────────────────────────────────
    {
        StyleRule rule;
        rule.المعرف = "تسمية-002";
        rule.الاسم = "تفضيل العربية";
        rule.الوصف = "يُفضل استخدام أسماء عربية للمتغيرات والدوال في لغة ص";
        rule.الخطورة_الافتراضية = StyleSeverity::تلميح;
        rule.الفئة = StyleCategory::تسمية;
        rule.مفعلة = true;
        
        rule.فحص = [this](const std::string& الكود) -> std::vector<StyleIssue> {
            std::vector<StyleIssue> issues;
            
            if (!الإعدادات_.تفضيل_الأسماء_العربية) {
                return issues; // القاعدة معطلة
            }
            
            auto identifiers = استخراج_المعرفات_(الكود);
            
            for (const auto& [اسم, موقع] : identifiers) {
                if (!هل_اسم_عربي_(اسم)) {
                    StyleIssue issue;
                    issue.معرف_القاعدة = "تسمية-002";
                    issue.الرسالة = "الاسم '" + اسم + "' ليس عربياً";
                    issue.الشرح = "لغة ص تدعم الأسماء العربية بشكل كامل. "
                                 "استخدام أسماء عربية يجعل الكود أسهل للقراءة والفهم.";
                    issue.الخطورة = StyleSeverity::تلميح;
                    issue.الفئة = StyleCategory::تسمية;
                    issue.الموقع = موقع;
                    
                    issues.push_back(issue);
                }
            }
            
            return issues;
        };
        
        القواعد_.push_back(rule);
    }
    
    // ═════════════════════════════════════════════════════════════════════════
    //                           قواعد التنسيق
    // ═════════════════════════════════════════════════════════════════════════
    
    // ─────────────────────────────────────────────────────────────────────────
    // قاعدة تنسيق-001: طول السطر
    // ─────────────────────────────────────────────────────────────────────────
    {
        StyleRule rule;
        rule.المعرف = "تنسيق-001";
        rule.الاسم = "طول السطر";
        rule.الوصف = "الأسطر الطويلة جداً تصعب القراءة";
        rule.الخطورة_الافتراضية = StyleSeverity::تحذير;
        rule.الفئة = StyleCategory::تنسيق;
        rule.مفعلة = true;
        
        rule.فحص = [this](const std::string& الكود) -> std::vector<StyleIssue> {
            std::vector<StyleIssue> issues;
            
            auto lines = تقسيم_إلى_أسطر_(الكود);
            
            for (size_t i = 0; i < lines.size(); ++i) {
                // حساب طول السطر (مع مراعاة UTF-8)
                int length = 0;
                for (size_t j = 0; j < lines[i].size(); ) {
                    unsigned char c = lines[i][j];
                    if ((c & 0x80) == 0) { j += 1; }      // ASCII
                    else if ((c & 0xE0) == 0xC0) { j += 2; }  // 2-byte UTF-8
                    else if ((c & 0xF0) == 0xE0) { j += 3; }  // 3-byte UTF-8
                    else if ((c & 0xF8) == 0xF0) { j += 4; }  // 4-byte UTF-8
                    else { j += 1; }
                    length++;
                }
                
                if (length > الإعدادات_.الحد_الأقصى_لطول_السطر) {
                    StyleIssue issue;
                    issue.معرف_القاعدة = "تنسيق-001";
                    issue.الرسالة = "السطر طويل جداً (" + std::to_string(length) + 
                                   " حرف، الحد الأقصى " + 
                                   std::to_string(الإعدادات_.الحد_الأقصى_لطول_السطر) + ")";
                    issue.الشرح = "الأسطر الطويلة تصعب القراءة والمراجعة. "
                                 "حاول تقسيم السطر إلى عدة أسطر أو استخدام متغيرات وسيطة.";
                    issue.الخطورة = StyleSeverity::تحذير;
                    issue.الفئة = StyleCategory::تنسيق;
                    issue.الموقع.سطر_البداية = static_cast<int>(i);
                    issue.الموقع.عمود_البداية = الإعدادات_.الحد_الأقصى_لطول_السطر;
                    issue.الموقع.سطر_النهاية = static_cast<int>(i);
                    issue.الموقع.عمود_النهاية = length;
                    
                    issues.push_back(issue);
                }
            }
            
            return issues;
        };
        
        القواعد_.push_back(rule);
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // قاعدة تنسيق-002: المسافات البادئة
    // ─────────────────────────────────────────────────────────────────────────
    {
        StyleRule rule;
        rule.المعرف = "تنسيق-002";
        rule.الاسم = "المسافات البادئة";
        rule.الوصف = "يجب استخدام مسافات بادئة متسقة";
        rule.الخطورة_الافتراضية = StyleSeverity::تحذير;
        rule.الفئة = StyleCategory::تنسيق;
        rule.مفعلة = true;
        
        rule.فحص = [this](const std::string& الكود) -> std::vector<StyleIssue> {
            std::vector<StyleIssue> issues;
            
            auto lines = تقسيم_إلى_أسطر_(الكود);
            int expected_indent = 0;
            
            for (size_t i = 0; i < lines.size(); ++i) {
                const auto& line = lines[i];
                if (line.empty()) continue;
                
                int actual_indent = حساب_المسافة_البادئة_(line);
                
                // فحص استخدام Tab vs مسافات
                if (!الإعدادات_.استخدام_تبويب && line.find('\t') != std::string::npos) {
                    StyleIssue issue;
                    issue.معرف_القاعدة = "تنسيق-002";
                    issue.الرسالة = "استخدام Tab بدلاً من المسافات";
                    issue.الشرح = "يُفضل استخدام المسافات لتوحيد المظهر على جميع المحررات.";
                    issue.الخطورة = StyleSeverity::تلميح;
                    issue.الفئة = StyleCategory::تنسيق;
                    issue.الموقع.سطر_البداية = static_cast<int>(i);
                    
                    // إصلاح تلقائي: استبدال Tab بمسافات
                    std::string fixed_line = line;
                    std::string spaces(الإعدادات_.حجم_المسافة_البادئة, ' ');
                    size_t pos = 0;
                    while ((pos = fixed_line.find('\t', pos)) != std::string::npos) {
                        fixed_line.replace(pos, 1, spaces);
                        pos += spaces.length();
                    }
                    
                    StyleFix fix;
                    fix.وصف = "استبدال Tab بمسافات";
                    fix.النص_القديم = line;
                    fix.النص_الجديد = fixed_line;
                    fix.آمن = true;
                    issue.الإصلاحات.push_back(fix);
                    
                    issues.push_back(issue);
                }
                
                // فحص تناسق المسافة البادئة
                if (actual_indent % الإعدادات_.حجم_المسافة_البادئة != 0) {
                    StyleIssue issue;
                    issue.معرف_القاعدة = "تنسيق-002";
                    issue.الرسالة = "المسافة البادئة غير متسقة (" + std::to_string(actual_indent) + 
                                   " مسافة، يجب أن تكون مضاعفات " + 
                                   std::to_string(الإعدادات_.حجم_المسافة_البادئة) + ")";
                    issue.الخطورة = StyleSeverity::تحذير;
                    issue.الفئة = StyleCategory::تنسيق;
                    issue.الموقع.سطر_البداية = static_cast<int>(i);
                    
                    issues.push_back(issue);
                }
            }
            
            return issues;
        };
        
        القواعد_.push_back(rule);
    }
    
    // ─────────────────────────────────────────────────────────────────────────
    // قاعدة تنسيق-003: المسافات الزائدة في نهاية الأسطر
    // ─────────────────────────────────────────────────────────────────────────
    {
        StyleRule rule;
        rule.المعرف = "تنسيق-003";
        rule.الاسم = "المسافات الزائدة";
        rule.الوصف = "لا يجب وجود مسافات في نهاية الأسطر";
        rule.الخطورة_الافتراضية = StyleSeverity::تلميح;
        rule.الفئة = StyleCategory::تنسيق;
        rule.مفعلة = true;
        
        rule.فحص = [this](const std::string& الكود) -> std::vector<StyleIssue> {
            std::vector<StyleIssue> issues;
            
            auto lines = تقسيم_إلى_أسطر_(الكود);
            
            for (size_t i = 0; i < lines.size(); ++i) {
                const auto& line = lines[i];
                if (line.empty()) continue;
                
                // فحص المسافات في النهاية
                size_t last_non_space = line.find_last_not_of(" \t");
                if (last_non_space != std::string::npos && 
                    last_non_space < line.length() - 1) {
                    
                    StyleIssue issue;
                    issue.معرف_القاعدة = "تنسيق-003";
                    issue.الرسالة = "مسافات زائدة في نهاية السطر";
                    issue.الخطورة = StyleSeverity::تلميح;
                    issue.الفئة = StyleCategory::تنسيق;
                    issue.الموقع.سطر_البداية = static_cast<int>(i);
                    issue.الموقع.عمود_البداية = static_cast<int>(last_non_space + 1);
                    issue.الموقع.سطر_النهاية = static_cast<int>(i);
                    issue.الموقع.عمود_النهاية = static_cast<int>(line.length());
                    
                    // إصلاح تلقائي
                    StyleFix fix;
                    fix.وصف = "إزالة المسافات الزائدة";
                    fix.النص_القديم = line;
                    fix.النص_الجديد = line.substr(0, last_non_space + 1);
                    fix.آمن = true;
                    issue.الإصلاحات.push_back(fix);
                    
                    issues.push_back(issue);
                }
            }
            
            return issues;
        };
        
        القواعد_.push_back(rule);
    }
    
    // ═════════════════════════════════════════════════════════════════════════
    //                           قواعد التعقيد
    // ═════════════════════════════════════════════════════════════════════════
    
    // ─────────────────────────────────────────────────────────────────────────
    // قاعدة تعقيد-001: عمق التداخل
    // ─────────────────────────────────────────────────────────────────────────
    {
        StyleRule rule;
        rule.المعرف = "تعقيد-001";
        rule.الاسم = "عمق التداخل";
        rule.الوصف = "التداخل العميق يصعب قراءة وفهم الكود";
        rule.الخطورة_الافتراضية = StyleSeverity::تحذير;
        rule.الفئة = StyleCategory::تعقيد;
        rule.مفعلة = true;
        
        rule.فحص = [this](const std::string& الكود) -> std::vector<StyleIssue> {
            std::vector<StyleIssue> issues;
            
            auto lines = تقسيم_إلى_أسطر_(الكود);
            
            for (size_t i = 0; i < lines.size(); ++i) {
                int indent = حساب_المسافة_البادئة_(lines[i]) / الإعدادات_.حجم_المسافة_البادئة;
                
                if (indent > الإعدادات_.الحد_الأقصى_لعمق_التداخل) {
                    StyleIssue issue;
                    issue.معرف_القاعدة = "تعقيد-001";
                    issue.الرسالة = "التداخل عميق جداً (مستوى " + std::to_string(indent) + 
                                   "، الحد الأقصى " + std::to_string(الإعدادات_.الحد_الأقصى_لعمق_التداخل) + ")";
                    issue.الشرح = "حاول استخدام Guard Clauses أو استخراج دوال فرعية لتقليل التداخل.\n"
                                 "مثال: بدلاً من:\n"
                                 "  إذا (شرط) {\n"
                                 "      // كود طويل\n"
                                 "  }\n"
                                 "استخدم:\n"
                                 "  إذا (!شرط) { أرجع }\n"
                                 "  // كود طويل";
                    issue.الخطورة = StyleSeverity::تحذير;
                    issue.الفئة = StyleCategory::تعقيد;
                    issue.الموقع.سطر_البداية = static_cast<int>(i);
                    
                    issues.push_back(issue);
                }
            }
            
            return issues;
        };
        
        القواعد_.push_back(rule);
    }
    
    // ═════════════════════════════════════════════════════════════════════════
    //                           قواعد الممارسات الأفضل
    // ═════════════════════════════════════════════════════════════════════════
    
    // ─────────────────────────────────────────────────────────────────────────
    // قاعدة ممارسة-001: القيم السحرية
    // ─────────────────────────────────────────────────────────────────────────
    {
        StyleRule rule;
        rule.المعرف = "ممارسة-001";
        rule.الاسم = "القيم السحرية";
        rule.الوصف = "تجنب استخدام أرقام بدون تسمية (magic numbers)";
        rule.الخطورة_الافتراضية = StyleSeverity::تلميح;
        rule.الفئة = StyleCategory::ممارسات;
        rule.مفعلة = true;
        
        rule.فحص = [this](const std::string& الكود) -> std::vector<StyleIssue> {
            std::vector<StyleIssue> issues;
            
            auto lines = تقسيم_إلى_أسطر_(الكود);
            
            // الأرقام المسموحة: 0, 1, -1, 2
            std::vector<std::string> allowed = {"0", "1", "-1", "2", "10", "100"};
            
            for (size_t i = 0; i < lines.size(); ++i) {
                const auto& line = lines[i];
                
                // تجاهل تعريفات الثوابت
                if (line.find("ثابت") != std::string::npos) continue;
                if (line.find("متغير") != std::string::npos) continue;
                
                // البحث عن الأرقام
                std::regex number_regex(R"(\b(\d+)\b)");
                std::smatch match;
                std::string::const_iterator start = line.begin();
                
                while (std::regex_search(start, line.end(), match, number_regex)) {
                    std::string number = match[1].str();
                    
                    // تحقق هل الرقم مسموح
                    bool is_allowed = std::find(allowed.begin(), allowed.end(), number) != allowed.end();
                    
                    if (!is_allowed && std::stoi(number) > 2) {
                        StyleIssue issue;
                        issue.معرف_القاعدة = "ممارسة-001";
                        issue.الرسالة = "قيمة سحرية: " + number;
                        issue.الشرح = "استخدم ثابتاً مسمى بدلاً من الرقم المباشر.\n"
                                     "مثال:\n"
                                     "  ثابت الحد_الأقصى = " + number + "\n"
                                     "  ... استخدم الحد_الأقصى ...";
                        issue.الخطورة = StyleSeverity::تلميح;
                        issue.الفئة = StyleCategory::ممارسات;
                        issue.الموقع.سطر_البداية = static_cast<int>(i);
                        
                        issues.push_back(issue);
                    }
                    
                    start = match.suffix().first;
                }
            }
            
            return issues;
        };
        
        القواعد_.push_back(rule);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              تنفيذ دوال الفحص
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief فحص الكود بالكامل
 * 
 * هذه هي الدالة الرئيسية لفحص الكود. تقوم بتطبيق جميع القواعد
 * المفعلة وتجمع النتائج في قائمة مرتبة.
 * 
 * @param الكود الكود المصدري للفحص
 * @param مسار_الملف مسار الملف (اختياري)
 * @return قائمة بجميع المشاكل المكتشفة
 */
std::vector<StyleIssue> StyleCheckerProvider::فحص_الكود(
    const std::string& الكود,
    const std::string& مسار_الملف) {
    
    std::vector<StyleIssue> all_issues;
    
    // تطبيق جميع القواعد المفعلة
    for (const auto& rule : القواعد_) {
        if (!rule.مفعلة) continue;
        
        // تنفيذ دالة الفحص
        auto issues = rule.فحص(الكود);
        
        // إضافة المشاكل للقائمة الكلية
        for (const auto& issue : issues) {
            all_issues.push_back(issue);
        }
    }
    
    // ترتيب حسب الموقع (سطر ثم عمود)
    std::sort(all_issues.begin(), all_issues.end(),
              [](const StyleIssue& a, const StyleIssue& b) {
                  if (a.الموقع.سطر_البداية != b.الموقع.سطر_البداية) {
                      return a.الموقع.سطر_البداية < b.الموقع.سطر_البداية;
                  }
                  return a.الموقع.عمود_البداية < b.الموقع.عمود_البداية;
              });
    
    // تحديث مسار الملف إذا تم تقديمه
    if (!مسار_الملف.empty()) {
        for (auto& issue : all_issues) {
            issue.رابط_التوثيق = مسار_الملف + ":" + 
                               std::to_string(issue.الموقع.سطر_البداية + 1);
        }
    }
    
    return all_issues;
}

/**
 * @brief فحص سطر واحد
 */
std::vector<StyleIssue> StyleCheckerProvider::فحص_سطر(
    const std::string& السطر,
    int رقم_السطر) {
    
    // فحص السطر كجزء من الكود
    return فحص_الكود(السطر, "");
}

/**
 * @brief فحص فئة معينة
 */
std::vector<StyleIssue> StyleCheckerProvider::فحص_فئة(
    const std::string& الكود,
    StyleCategory الفئة) {
    
    auto all_issues = فحص_الكود(الكود);
    
    // فلترة حسب الفئة
    std::vector<StyleIssue> filtered;
    std::copy_if(all_issues.begin(), all_issues.end(),
                 std::back_inserter(filtered),
                 [الفئة](const StyleIssue& i) { return i.الفئة == الفئة; });
    
    return filtered;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              فحوصات متخصصة
// ═══════════════════════════════════════════════════════════════════════════════

std::vector<StyleIssue> StyleCheckerProvider::فحص_التسمية(const std::string& الكود) {
    return فحص_فئة(الكود, StyleCategory::تسمية);
}

std::vector<StyleIssue> StyleCheckerProvider::فحص_التنسيق(const std::string& الكود) {
    return فحص_فئة(الكود, StyleCategory::تنسيق);
}

std::vector<StyleIssue> StyleCheckerProvider::فحص_التوثيق(const std::string& الكود) {
    return فحص_فئة(الكود, StyleCategory::توثيق);
}

std::vector<StyleIssue> StyleCheckerProvider::فحص_التعقيد(const std::string& الكود) {
    return فحص_فئة(الكود, StyleCategory::تعقيد);
}

std::vector<StyleIssue> StyleCheckerProvider::فحص_الممارسات(const std::string& الكود) {
    return فحص_فئة(الكود, StyleCategory::ممارسات);
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              الإصلاح التلقائي
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief إصلاح جميع المشاكل الآمنة تلقائياً
 * 
 * تقوم هذه الدالة بتطبيق جميع الإصلاحات المعلمة كآمنة.
 * الإصلاحات الآمنة لا تغير سلوك البرنامج.
 * 
 * @param الكود الكود الأصلي
 * @return الكود بعد تطبيق الإصلاحات
 */
std::string StyleCheckerProvider::إصلاح_تلقائي(const std::string& الكود) {
    std::string result = الكود;
    
    // فحص الكود للحصول على المشاكل
    auto issues = فحص_الكود(الكود);
    
    // ترتيب عكسي (من النهاية للبداية) لتجنب تعارض المواقع
    std::sort(issues.begin(), issues.end(),
              [](const StyleIssue& a, const StyleIssue& b) {
                  if (a.الموقع.سطر_البداية != b.الموقع.سطر_البداية) {
                      return a.الموقع.سطر_البداية > b.الموقع.سطر_البداية;
                  }
                  return a.الموقع.عمود_البداية > b.الموقع.عمود_البداية;
              });
    
    // تطبيق الإصلاحات الآمنة
    for (const auto& issue : issues) {
        for (const auto& fix : issue.الإصلاحات) {
            if (fix.آمن) {
                // استبدال النص
                size_t pos = result.find(fix.النص_القديم);
                if (pos != std::string::npos) {
                    result.replace(pos, fix.النص_القديم.length(), fix.النص_الجديد);
                }
            }
        }
    }
    
    return result;
}

/**
 * @brief تطبيق إصلاح محدد
 */
std::string StyleCheckerProvider::تطبيق_إصلاح(
    const std::string& الكود,
    const StyleIssue& المشكلة,
    int رقم_الإصلاح) {
    
    if (رقم_الإصلاح < 0 || رقم_الإصلاح >= static_cast<int>(المشكلة.الإصلاحات.size())) {
        return الكود;
    }
    
    const auto& fix = المشكلة.الإصلاحات[رقم_الإصلاح];
    
    std::string result = الكود;
    size_t pos = result.find(fix.النص_القديم);
    if (pos != std::string::npos) {
        result.replace(pos, fix.النص_القديم.length(), fix.النص_الجديد);
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              إدارة القواعد
// ═══════════════════════════════════════════════════════════════════════════════

void StyleCheckerProvider::إضافة_قاعدة(const StyleRule& القاعدة) {
    القواعد_.push_back(القاعدة);
}

void StyleCheckerProvider::تعطيل_قاعدة(const std::string& المعرف) {
    for (auto& rule : القواعد_) {
        if (rule.المعرف == المعرف) {
            rule.مفعلة = false;
            break;
        }
    }
}

void StyleCheckerProvider::تفعيل_قاعدة(const std::string& المعرف) {
    for (auto& rule : القواعد_) {
        if (rule.المعرف == المعرف) {
            rule.مفعلة = true;
            break;
        }
    }
}

std::vector<StyleRule> StyleCheckerProvider::جلب_القواعد() const {
    return القواعد_;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              التقارير
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief توليد تقرير نصي مفصل
 */
std::string StyleCheckerProvider::توليد_تقرير(const std::vector<StyleIssue>& المشاكل) {
    std::ostringstream report;
    
    report << "═══════════════════════════════════════════════════════════════\n";
    report << "              تقرير فحص أنماط الكود - لغة ص\n";
    report << "═══════════════════════════════════════════════════════════════\n\n";
    
    auto stats = إحصائيات(المشاكل);
    
    report << "📊 ملخص:\n";
    report << "───────────────────────────────────────────────────────────────\n";
    report << "  إجمالي المشاكل: " << المشاكل.size() << "\n";
    report << "  أخطاء: " << stats["أخطاء"] << "\n";
    report << "  تحذيرات: " << stats["تحذيرات"] << "\n";
    report << "  تلميحات: " << stats["تلميحات"] << "\n";
    report << "  معلومات: " << stats["معلومات"] << "\n\n";
    
    if (المشاكل.empty()) {
        report << "✅ لا توجد مشاكل في النمط! الكود يتبع أفضل الممارسات.\n";
        return report.str();
    }
    
    report << "📋 تفاصيل المشاكل:\n";
    report << "───────────────────────────────────────────────────────────────\n\n";
    
    for (size_t i = 0; i < المشاكل.size(); ++i) {
        const auto& issue = المشاكل[i];
        
        // رمز الخطورة
        std::string icon;
        switch (issue.الخطورة) {
            case StyleSeverity::خطأ:    icon = "❌"; break;
            case StyleSeverity::تحذير:  icon = "⚠️"; break;
            case StyleSeverity::تلميح:  icon = "💡"; break;
            case StyleSeverity::معلومة: icon = "ℹ️"; break;
        }
        
        report << (i + 1) << ". " << icon << " [" << issue.معرف_القاعدة << "] ";
        report << issue.الرسالة << "\n";
        report << "   📍 الموقع: " << issue.الموقع.إلى_نص() << "\n";
        
        if (!issue.الشرح.empty()) {
            report << "   📝 " << issue.الشرح << "\n";
        }
        
        if (!issue.الإصلاحات.empty()) {
            report << "   🔧 إصلاحات متاحة: " << issue.الإصلاحات.size() << "\n";
        }
        
        report << "\n";
    }
    
    return report.str();
}

/**
 * @brief توليد تقرير JSON
 */
std::string StyleCheckerProvider::توليد_تقرير_json(const std::vector<StyleIssue>& المشاكل) {
    std::ostringstream json;
    
    json << "{\n";
    json << "  \"total\": " << المشاكل.size() << ",\n";
    json << "  \"issues\": [\n";
    
    for (size_t i = 0; i < المشاكل.size(); ++i) {
        const auto& issue = المشاكل[i];
        
        json << "    {\n";
        json << "      \"rule\": \"" << issue.معرف_القاعدة << "\",\n";
        json << "      \"message\": \"" << issue.الرسالة << "\",\n";
        json << "      \"severity\": \"" << خطورة_إلى_نص(issue.الخطورة) << "\",\n";
        json << "      \"category\": \"" << فئة_إلى_نص(issue.الفئة) << "\",\n";
        json << "      \"line\": " << (issue.الموقع.سطر_البداية + 1) << ",\n";
        json << "      \"column\": " << (issue.الموقع.عمود_البداية + 1) << "\n";
        json << "    }";
        
        if (i < المشاكل.size() - 1) json << ",";
        json << "\n";
    }
    
    json << "  ]\n";
    json << "}\n";
    
    return json.str();
}

/**
 * @brief حساب إحصائيات المشاكل
 */
std::map<std::string, int> StyleCheckerProvider::إحصائيات(const std::vector<StyleIssue>& المشاكل) {
    std::map<std::string, int> stats;
    stats["أخطاء"] = 0;
    stats["تحذيرات"] = 0;
    stats["تلميحات"] = 0;
    stats["معلومات"] = 0;
    
    for (const auto& issue : المشاكل) {
        switch (issue.الخطورة) {
            case StyleSeverity::خطأ:    stats["أخطاء"]++; break;
            case StyleSeverity::تحذير:  stats["تحذيرات"]++; break;
            case StyleSeverity::تلميح:  stats["تلميحات"]++; break;
            case StyleSeverity::معلومة: stats["معلومات"]++; break;
        }
    }
    
    return stats;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              الدوال المساعدة الخاصة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief تقسيم النص إلى أسطر
 */
std::vector<std::string> StyleCheckerProvider::تقسيم_إلى_أسطر_(const std::string& الكود) {
    std::vector<std::string> lines;
    std::istringstream stream(الكود);
    std::string line;
    
    while (std::getline(stream, line)) {
        // إزالة \r إذا كان موجوداً (Windows line endings)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines.push_back(line);
    }
    
    return lines;
}

/**
 * @brief استخراج المعرفات من الكود
 * 
 * تبحث عن أسماء المتغيرات والدوال في الكود
 */
std::vector<std::pair<std::string, StyleLocation>> 
StyleCheckerProvider::استخراج_المعرفات_(const std::string& الكود) {
    std::vector<std::pair<std::string, StyleLocation>> identifiers;
    
    auto lines = تقسيم_إلى_أسطر_(الكود);
    
    // أنماط تعريف المتغيرات والدوال
    std::regex var_regex(R"(متغير\s+([^\s=]+))");
    std::regex const_regex(R"(ثابت\s+([^\s=]+))");
    std::regex func_regex(R"(دالة\s+([^\s(]+))");
    
    for (size_t i = 0; i < lines.size(); ++i) {
        std::smatch match;
        
        // متغيرات
        std::string::const_iterator start = lines[i].begin();
        while (std::regex_search(start, lines[i].cend(), match, var_regex)) {
            StyleLocation loc;
            loc.سطر_البداية = static_cast<int>(i);
            loc.عمود_البداية = static_cast<int>(match.position(1));
            identifiers.push_back({match[1].str(), loc});
            start = match.suffix().first;
        }
        
        // ثوابت
        start = lines[i].begin();
        while (std::regex_search(start, lines[i].cend(), match, const_regex)) {
            StyleLocation loc;
            loc.سطر_البداية = static_cast<int>(i);
            loc.عمود_البداية = static_cast<int>(match.position(1));
            identifiers.push_back({match[1].str(), loc});
            start = match.suffix().first;
        }
        
        // دوال
        start = lines[i].begin();
        while (std::regex_search(start, lines[i].cend(), match, func_regex)) {
            StyleLocation loc;
            loc.سطر_البداية = static_cast<int>(i);
            loc.عمود_البداية = static_cast<int>(match.position(1));
            identifiers.push_back({match[1].str(), loc});
            start = match.suffix().first;
        }
    }
    
    return identifiers;
}

/**
 * @brief حساب عدد المسافات البادئة في السطر
 */
int StyleCheckerProvider::حساب_المسافة_البادئة_(const std::string& السطر) {
    int count = 0;
    for (char c : السطر) {
        if (c == ' ') count++;
        else if (c == '\t') count += الإعدادات_.حجم_المسافة_البادئة;
        else break;
    }
    return count;
}

/**
 * @brief التحقق من أن الاسم يحتوي على حروف عربية
 */
bool StyleCheckerProvider::هل_اسم_عربي_(const std::string& الاسم) {
    for (size_t i = 0; i < الاسم.size(); ) {
        unsigned char c = الاسم[i];
        
        // UTF-8 Arabic range: U+0600-U+06FF (3 bytes: E0 99 80 to E0 9B BF)
        // Actually Arabic is: D8 80 to D9 BF (2 bytes in UTF-8)
        if ((c >= 0xD8 && c <= 0xD9) || (c >= 0xDA && c <= 0xDB)) {
            return true; // حرف عربي
        }
        
        // تقدم حسب طول الحرف UTF-8
        if ((c & 0x80) == 0) i += 1;
        else if ((c & 0xE0) == 0xC0) i += 2;
        else if ((c & 0xF0) == 0xE0) i += 3;
        else if ((c & 0xF8) == 0xF0) i += 4;
        else i += 1;
    }
    return false;
}

/**
 * @brief التحقق من وضوح الاسم (ليس من الأسماء الغامضة)
 */
bool StyleCheckerProvider::هل_اسم_واضح_(const std::string& الاسم) {
    // قائمة الأسماء الغامضة
    std::vector<std::string> unclear = {
        "x", "y", "z", "a", "b", "c", "i", "j", "k",
        "tmp", "temp", "foo", "bar", "baz",
        "data", "info", "value", "result"
    };
    
    for (const auto& bad : unclear) {
        if (الاسم == bad) return false;
    }
    
    // الاسم واضح إذا كان طوله مناسباً ولم يكن في القائمة
    return الاسم.length() >= 3;
}

/**
 * @brief حساب التعقيد السيكلوماتي للكود
 */
int StyleCheckerProvider::حساب_التعقيد_(const std::string& كود_الدالة) {
    int complexity = 1; // نبدأ بـ 1
    
    // كل نقطة قرار تزيد التعقيد
    std::vector<std::string> decision_points = {
        "إذا", "وإلا", "طالما", "لكل", "اختر", "حالة",
        "&&", "||", "؟"
    };
    
    for (const auto& dp : decision_points) {
        size_t pos = 0;
        while ((pos = كود_الدالة.find(dp, pos)) != std::string::npos) {
            complexity++;
            pos += dp.length();
        }
    }
    
    return complexity;
}

/**
 * @brief حساب أقصى عمق للتداخل
 */
int StyleCheckerProvider::حساب_عمق_التداخل_(const std::string& الكود) {
    int max_depth = 0;
    int current_depth = 0;
    
    for (char c : الكود) {
        if (c == '{') {
            current_depth++;
            max_depth = std::max(max_depth, current_depth);
        } else if (c == '}') {
            current_depth = std::max(0, current_depth - 1);
        }
    }
    
    return max_depth;
}

// ═══════════════════════════════════════════════════════════════════════════════
//                              دوال المصنع
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * @brief إنشاء مدقق مع إعدادات صارمة جداً
 */
StyleCheckerProvider إنشاء_مدقق_صارم() {
    StyleCheckerConfig config;
    config.وضع_صارم = true;
    config.الحد_الأقصى_لطول_السطر = 80;
    config.الحد_الأقصى_للتعقيد = 5;
    config.الحد_الأقصى_لعمق_التداخل = 3;
    config.الحد_الأقصى_لطول_الدالة = 30;
    return StyleCheckerProvider(config);
}

/**
 * @brief إنشاء مدقق مع إعدادات مرنة
 */
StyleCheckerProvider إنشاء_مدقق_مرن() {
    StyleCheckerConfig config;
    config.وضع_صارم = false;
    config.الحد_الأقصى_لطول_السطر = 150;
    config.الحد_الأقصى_للتعقيد = 20;
    config.الحد_الأقصى_لعمق_التداخل = 6;
    config.الحد_الأقصى_لطول_الدالة = 100;
    config.تفضيل_الأسماء_العربية = false;
    return StyleCheckerProvider(config);
}

} // namespace lsp
} // namespace sad
