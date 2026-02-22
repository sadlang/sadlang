/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  مكونات واجهة المستخدم البنكية - Banking UI Components
 *  جزء من النظام الرسومي الثوري للغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 *  مكونات متخصصة للتطبيقات البنكية:
 *  - عرض البطاقات المصرفية (3D Card Display)
 *  - رسوم بيانية للمعاملات (Charts & Graphs)
 *  - عرض الرصيد مع حركات (Animated Balance)
 *  - مؤشر الأمان (Security Meter)
 *  - محفظة رقمية (Digital Wallet)
 *  - QR Code للدفع
 *  - التحقق البيومتري
 * 
 *  الإصدار: 2.1
 *  المؤلف: فريق تطوير لغة ص
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_BANKING_UI_H
#define SAD_BANKING_UI_H

#include "sad_ui_engine.h"

#ifdef __cplusplus
extern "C" {
#endif

// ═══════════════════════════════════════════════════════════════════════════════
//  الثوابت
// ═══════════════════════════════════════════════════════════════════════════════

#define SADBANK_MAX_TRANSACTIONS 100
#define SADBANK_MAX_CARDS 10
#define SADBANK_MAX_ACCOUNTS 5
#define SADBANK_QR_SIZE 256

// ألوان مصرفية
#define SADBANK_COLOR_GOLD       0xFFD700FF  // ذهبي
#define SADBANK_COLOR_PLATINUM   0xE5E4E2FF  // بلاتيني
#define SADBANK_COLOR_SUCCESS    0x00C853FF  // نجاح (إيداع)
#define SADBANK_COLOR_DANGER     0xFF1744FF  // خطر (سحب)
#define SADBANK_COLOR_SECURE     0x00BFA5FF  // آمن
#define SADBANK_COLOR_WARNING    0xFFAB00FF  // تحذير

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع البطاقات
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SADBANK_CARD_MADA,          // بطاقة مدى
    SADBANK_CARD_VISA,          // فيزا
    SADBANK_CARD_MASTERCARD,    // ماستركارد
    SADBANK_CARD_AMEX,          // أمريكان إكسبريس
    SADBANK_CARD_VIRTUAL,       // بطاقة افتراضية
    SADBANK_CARD_PREPAID        // مسبقة الدفع
} SadBankCardType;

typedef enum {
    SADBANK_CARD_DESIGN_CLASSIC,    // كلاسيكي
    SADBANK_CARD_DESIGN_GOLD,       // ذهبي
    SADBANK_CARD_DESIGN_PLATINUM,   // بلاتيني
    SADBANK_CARD_DESIGN_SIGNATURE,  // سيجنتشر
    SADBANK_CARD_DESIGN_ISLAMIC,    // إسلامي
    SADBANK_CARD_DESIGN_YOUTH       // شباب
} SadBankCardDesign;

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع المعاملات
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SADBANK_TX_DEPOSIT,         // إيداع
    SADBANK_TX_WITHDRAWAL,      // سحب
    SADBANK_TX_TRANSFER_IN,     // تحويل وارد
    SADBANK_TX_TRANSFER_OUT,    // تحويل صادر
    SADBANK_TX_PAYMENT,         // دفع
    SADBANK_TX_SALARY,          // راتب
    SADBANK_TX_BILL,            // فاتورة
    SADBANK_TX_REFUND,          // استرداد
    SADBANK_TX_FEE              // رسوم
} SadBankTxType;

// ═══════════════════════════════════════════════════════════════════════════════
//  أنواع الرسوم البيانية
// ═══════════════════════════════════════════════════════════════════════════════

typedef enum {
    SADBANK_CHART_LINE,         // خطي
    SADBANK_CHART_BAR,          // أعمدة
    SADBANK_CHART_PIE,          // دائري
    SADBANK_CHART_DONUT,        // حلقي (Donut)
    SADBANK_CHART_AREA,         // مساحة
    SADBANK_CHART_SPARKLINE     // خط صغير
} SadBankChartType;

// ═══════════════════════════════════════════════════════════════════════════════
//  هياكل البيانات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * معلومات البطاقة المصرفية
 */
typedef struct {
    SadBankCardType type;
    SadBankCardDesign design;
    char cardNumber[20];        // رقم البطاقة (آخر 4 أرقام فقط للعرض)
    char holderName[64];        // اسم حامل البطاقة
    char expiryDate[8];         // تاريخ الانتهاء MM/YY
    float creditLimit;          // الحد الائتماني
    float availableBalance;     // الرصيد المتاح
    bool isActive;              // نشطة
    bool isFrozen;              // مجمّدة
    bool isContactless;         // تدعم الدفع اللاتلامسي
} SadBankCard;

/**
 * معاملة مالية
 */
typedef struct {
    SadBankTxType type;
    float amount;               // المبلغ
    char description[128];      // الوصف
    char date[16];              // التاريخ
    char time[12];              // الوقت
    char reference[32];         // رقم المرجع
    char merchant[64];          // التاجر/الجهة
    char category[32];          // الفئة
    SadColor categoryColor;     // لون الفئة
    char iconEmoji[8];          // رمز تعبيري
} SadBankTransaction;

/**
 * نقطة بيانات للرسم البياني
 */
typedef struct {
    float value;
    char label[32];
    SadColor color;
} SadBankChartPoint;

/**
 * إعدادات الرسم البياني
 */
typedef struct {
    SadBankChartType type;
    char title[64];
    SadBankChartPoint* points;
    int pointCount;
    bool showLabels;
    bool showValues;
    bool showLegend;
    bool animated;
    SadColor backgroundColor;
    SadColor gridColor;
    float animationDuration;
} SadBankChartConfig;

/**
 * إعدادات عرض البطاقة
 */
typedef struct {
    bool show3D;                // عرض ثلاثي الأبعاد
    bool showGlare;             // تأثير اللمعان
    bool flippable;             // قابلة للقلب
    bool showChip;              // عرض الشريحة
    bool showContactless;       // عرض رمز NFC
    float rotationAngle;        // زاوية الدوران للـ 3D
    float scale;                // الحجم
} SadBankCardDisplayConfig;

/**
 * حالة التحقق البيومتري
 */
typedef enum {
    SADBANK_BIO_IDLE,           // في الانتظار
    SADBANK_BIO_SCANNING,       // جاري المسح
    SADBANK_BIO_SUCCESS,        // نجاح
    SADBANK_BIO_FAILED,         // فشل
    SADBANK_BIO_LOCKED          // مقفل
} SadBankBiometricState;

/**
 * نوع التحقق البيومتري
 */
typedef enum {
    SADBANK_BIO_FINGERPRINT,    // بصمة الإصبع
    SADBANK_BIO_FACE,           // بصمة الوجه
    SADBANK_BIO_IRIS            // بصمة العين
} SadBankBiometricType;

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال إنشاء العناصر البنكية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * إنشاء عرض بطاقة مصرفية
 * @param card معلومات البطاقة
 * @param config إعدادات العرض
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_card_display(
    const SadBankCard* card,
    const SadBankCardDisplayConfig* config
);

/**
 * إنشاء عرض الرصيد المتحرك
 * @param balance الرصيد
 * @param currency العملة (مثل "ر.س")
 * @param showChange عرض التغيير
 * @param changePercent نسبة التغيير
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_balance_display(
    float balance,
    const char* currency,
    bool showChange,
    float changePercent
);

/**
 * إنشاء قائمة المعاملات
 * @param transactions مصفوفة المعاملات
 * @param count عدد المعاملات
 * @param groupByDate تجميع حسب التاريخ
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_transaction_list(
    const SadBankTransaction* transactions,
    int count,
    bool groupByDate
);

/**
 * إنشاء عنصر معاملة واحدة
 * @param tx المعاملة
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_transaction_item(const SadBankTransaction* tx);

/**
 * إنشاء رسم بياني
 * @param config إعدادات الرسم البياني
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_chart(const SadBankChartConfig* config);

/**
 * إنشاء رمز QR للدفع
 * @param data البيانات للترميز
 * @param size حجم الرمز
 * @param foregroundColor لون الأمامية
 * @param backgroundColor لون الخلفية
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_qr_code(
    const char* data,
    int size,
    SadColor foregroundColor,
    SadColor backgroundColor
);

/**
 * إنشاء واجهة التحقق البيومتري
 * @param type نوع التحقق
 * @param title العنوان
 * @param subtitle العنوان الفرعي
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_biometric_prompt(
    SadBankBiometricType type,
    const char* title,
    const char* subtitle
);

/**
 * إنشاء مؤشر الأمان
 * @param score درجة الأمان (0-100)
 * @param showLabel عرض التسمية
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_security_meter(int score, bool showLabel);

/**
 * إنشاء بطاقة حساب مصغرة
 * @param accountName اسم الحساب
 * @param accountNumber رقم الحساب
 * @param balance الرصيد
 * @param currency العملة
 * @param icon الأيقونة (emoji)
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_account_card(
    const char* accountName,
    const char* accountNumber,
    float balance,
    const char* currency,
    const char* icon
);

/**
 * إنشاء شريط إجراءات سريعة
 * Actions: تحويل، دفع، طلب، المزيد
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_quick_actions(void);

/**
 * إنشاء بطاقة إشعار
 * @param title العنوان
 * @param message الرسالة
 * @param type النوع (info, success, warning, error)
 * @param dismissible قابلة للإغلاق
 * @return معرّف العنصر
 */
SadWidgetId sadbank_create_notification_card(
    const char* title,
    const char* message,
    const char* type,
    bool dismissible
);

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال التحديث والتفاعل
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تحديث الرصيد مع حركة
 */
void sadbank_update_balance(SadWidgetId widget, float newBalance, bool animate);

/**
 * تحديث حالة التحقق البيومتري
 */
void sadbank_update_biometric_state(SadWidgetId widget, SadBankBiometricState state);

/**
 * قلب البطاقة (أمام/خلف)
 */
void sadbank_flip_card(SadWidgetId widget);

/**
 * تجميد/إلغاء تجميد البطاقة
 */
void sadbank_toggle_card_freeze(SadWidgetId widget, bool frozen);

/**
 * تحديث بيانات الرسم البياني
 */
void sadbank_update_chart_data(
    SadWidgetId widget,
    const SadBankChartPoint* points,
    int count,
    bool animate
);

/**
 * إضافة معاملة جديدة للقائمة
 */
void sadbank_add_transaction(SadWidgetId widget, const SadBankTransaction* tx);

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال الحركات والمؤثرات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * حركة عد الرصيد (من 0 إلى القيمة)
 */
SadAnimId sadbank_animate_count_up(SadWidgetId widget, float duration);

/**
 * حركة تأثير النجاح (نبضة خضراء)
 */
SadAnimId sadbank_animate_success_pulse(SadWidgetId widget);

/**
 * حركة تأثير الخطأ (اهتزاز)
 */
SadAnimId sadbank_animate_error_shake(SadWidgetId widget);

/**
 * حركة ظهور البطاقة
 */
SadAnimId sadbank_animate_card_reveal(SadWidgetId widget);

/**
 * حركة المسح الضوئي للبصمة
 */
SadAnimId sadbank_animate_fingerprint_scan(SadWidgetId widget);

/**
 * حركة رسم بياني متدرج
 */
SadAnimId sadbank_animate_chart_draw(SadWidgetId widget, float duration);

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال مساعدة
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تنسيق المبلغ المالي بالعربية
 * مثال: 25,750.50 ر.س
 */
void sadbank_format_currency(
    char* output,
    int outputSize,
    float amount,
    const char* currency
);

/**
 * تحويل نوع المعاملة إلى نص عربي
 */
const char* sadbank_tx_type_to_string(SadBankTxType type);

/**
 * الحصول على لون نوع المعاملة
 */
SadColor sadbank_tx_type_color(SadBankTxType type);

/**
 * الحصول على رمز emoji لنوع المعاملة
 */
const char* sadbank_tx_type_emoji(SadBankTxType type);

/**
 * إنشاء تدرج لوني للبطاقة حسب النوع
 */
SadGradient sadbank_card_gradient(SadBankCardDesign design);

#ifdef __cplusplus
}
#endif

#endif // SAD_BANKING_UI_H
