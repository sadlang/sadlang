/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  تطبيق بنك ص - مثال على نظام الواجهات
 *  Sad Bank App - UI System Example
 * ═══════════════════════════════════════════════════════════════════════════════
 * 
 *  هذا المثال يوضح كيفية استخدام نظام واجهات ص لإنشاء تطبيق بنكي
 *  مع 5 صفحات ورسومات حقيقية
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "../graphics/sad_flutter_ui.h"
#include "../graphics/sad_ui_arabic.h"
#include <string.h>

// ═══════════════════════════════════════════════════════════════════════════════
//  المتغيرات العامة
// ═══════════════════════════════════════════════════════════════════════════════

static int الصفحة_الحالية = 0;
static SadWidget* محتوى_الصفحات = NULL;
static SadWidget* صفحات[5] = {NULL};

// ═══════════════════════════════════════════════════════════════════════════════
//  معالجات الأحداث
// ═══════════════════════════════════════════════════════════════════════════════

static void عند_تغيير_الصفحة(SadWidget* widget, void* userData) {
    int index = (int)(long)userData;
    
    // إخفاء كل الصفحات
    for (int i = 0; i < 5; i++) {
        if (صفحات[i]) {
            sad_set_visible(صفحات[i], false);
        }
    }
    
    // إظهار الصفحة المختارة
    if (index >= 0 && index < 5 && صفحات[index]) {
        sad_set_visible(صفحات[index], true);
        الصفحة_الحالية = index;
    }
}

static void عند_الضغط_تحويل(SadWidget* widget, void* userData) {
    // TODO: فتح صفحة التحويل
}

static void عند_الضغط_دفع(SadWidget* widget, void* userData) {
    // TODO: فتح صفحة الدفع
}

// ═══════════════════════════════════════════════════════════════════════════════
//  بناء الصفحات
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * بناء بطاقة معلومات
 */
static SadWidget* بطاقة_معلومات(const char* عنوان, const char* قيمة, SadColor لون) {
    SadWidget* card = بطاقة();
    عيّن_الحشو(card, حشو(16));
    
    SadWidget* col = عمود();
    عيّن_التباعد(col, 8);
    
    // العنوان
    SadWidget* titleWidget = نص(عنوان);
    titleWidget->textData.textStyle = SADTEXT_CAPTION;
    أضف(col, titleWidget);
    
    // القيمة
    SadWidget* valueWidget = نص(قيمة);
    valueWidget->textData.textStyle = SADTEXT_HEADLINE5;
    valueWidget->textData.textStyle.color = لون;
    أضف(col, valueWidget);
    
    أضف(card, col);
    return card;
}

/**
 * بناء زر إجراء سريع
 */
static SadWidget* زر_إجراء(const char* عنوان, int أيقونة, SadTapCallback دالة) {
    SadWidget* col = عمود();
    عيّن_المحاذاة(col, منتصف, منتصف);
    عيّن_التباعد(col, 8);
    عيّن_الحجم(col, 80, 80);
    
    // الأيقونة في دائرة
    SadWidget* iconContainer = حاوية();
    عيّن_الحجم(iconContainer, 56, 56);
    عيّن_الخلفية(iconContainer, لون_أساسي);
    عيّن_الزوايا(iconContainer, زوايا(28));
    عند_الضغط(iconContainer, دالة, NULL);
    
    SadWidget* iconWidget = أيقونة(أيقونة, 24, أبيض);
    أضف(iconContainer, iconWidget);
    أضف(col, iconContainer);
    
    // النص
    SadWidget* label = نص(عنوان);
    label->textData.textStyle = SADTEXT_CAPTION;
    أضف(col, label);
    
    return col;
}

/**
 * بناء عنصر معاملة
 */
static SadWidget* عنصر_معاملة(const char* وصف, const char* مبلغ, const char* تاريخ, bool وارد) {
    SadWidget* row = صف();
    عيّن_التباعد(row, 12);
    عيّن_المحاذاة(row, توزيع_متساوي, منتصف);
    عيّن_الحشو(row, حشو_متناظر(12, 0));
    
    // الأيقونة
    SadWidget* iconBg = حاوية();
    عيّن_الحجم(iconBg, 40, 40);
    عيّن_الخلفية(iconBg, وارد ? sadcolor(76, 175, 80, 50) : sadcolor(244, 67, 54, 50));
    عيّن_الزوايا(iconBg, زوايا(20));
    
    SadWidget* icon = أيقونة(وارد ? SADICON_ARROW_DOWN : SADICON_ARROW_UP, 20, 
                              وارد ? أخضر : أحمر);
    أضف(iconBg, icon);
    أضف(row, iconBg);
    
    // التفاصيل
    SadWidget* details = عمود();
    عيّن_التباعد(details, 4);
    عيّن_المرونة(details, 1);
    
    SadWidget* descWidget = نص(وصف);
    descWidget->textData.textStyle = SADTEXT_BODY1;
    أضف(details, descWidget);
    
    SadWidget* dateWidget = نص(تاريخ);
    dateWidget->textData.textStyle = SADTEXT_CAPTION;
    أضف(details, dateWidget);
    
    أضف(row, details);
    
    // المبلغ
    SadWidget* amountWidget = نص(مبلغ);
    amountWidget->textData.textStyle.fontSize = 16;
    amountWidget->textData.textStyle.fontStyle = SADFONT_BOLD;
    amountWidget->textData.textStyle.color = وارد ? أخضر : أحمر;
    أضف(row, amountWidget);
    
    return row;
}

/**
 * صفحة الرئيسية
 */
static SadWidget* ابنِ_صفحة_الرئيسية(void) {
    SadWidget* page = عمود();
    عيّن_الخلفية(page, لون_الخلفية);
    عيّن_الحشو(page, حشو(16));
    عيّن_التباعد(page, 16);
    
    // الترحيب
    SadWidget* welcome = نص("مرحباً، أحمد 👋");
    welcome->textData.textStyle = SADTEXT_HEADLINE5;
    أضف(page, welcome);
    
    // بطاقة الرصيد
    SadWidget* balanceCard = بطاقة();
    عيّن_الخلفية(balanceCard, لون_أساسي);
    عيّن_الحشو(balanceCard, حشو(24));
    عيّن_الزوايا(balanceCard, زوايا(16));
    
    SadWidget* balanceCol = عمود();
    عيّن_التباعد(balanceCol, 8);
    
    SadWidget* balanceLabel = نص("الرصيد المتاح");
    balanceLabel->textData.textStyle.color = أبيض;
    balanceLabel->textData.textStyle.fontSize = 14;
    أضف(balanceCol, balanceLabel);
    
    SadWidget* balanceValue = نص("25,750.50 ر.س");
    balanceValue->textData.textStyle = SADTEXT_HEADLINE4;
    balanceValue->textData.textStyle.color = أبيض;
    أضف(balanceCol, balanceValue);
    
    أضف(balanceCard, balanceCol);
    أضف(page, balanceCard);
    
    // الإجراءات السريعة
    SadWidget* actionsRow = صف();
    عيّن_المحاذاة(actionsRow, توزيع_حول, منتصف);
    
    أضف(actionsRow, زر_إجراء("تحويل", SADICON_TRANSFER, عند_الضغط_تحويل));
    أضف(actionsRow, زر_إجراء("دفع", SADICON_PAYMENT, عند_الضغط_دفع));
    أضف(actionsRow, زر_إجراء("طلب", SADICON_ARROW_DOWN, NULL));
    أضف(actionsRow, زر_إجراء("المزيد", SADICON_MENU, NULL));
    
    أضف(page, actionsRow);
    
    // آخر المعاملات
    SadWidget* transactionsCard = بطاقة();
    عيّن_الحشو(transactionsCard, حشو(16));
    
    SadWidget* transCol = عمود();
    عيّن_التباعد(transCol, 12);
    
    SadWidget* transTitle = نص("آخر المعاملات");
    transTitle->textData.textStyle = SADTEXT_HEADLINE6;
    أضف(transCol, transTitle);
    
    أضف(transCol, فاصل());
    أضف(transCol, عنصر_معاملة("راتب شهري", "+5,000 ر.س", "20 فبراير 2026", true));
    أضف(transCol, فاصل());
    أضف(transCol, عنصر_معاملة("مطعم البيك", "-85 ر.س", "19 فبراير 2026", false));
    أضف(transCol, فاصل());
    أضف(transCol, عنصر_معاملة("محطة وقود", "-200 ر.س", "18 فبراير 2026", false));
    
    أضف(transactionsCard, transCol);
    أضف(page, transactionsCard);
    
    return page;
}

/**
 * صفحة التحويلات
 */
static SadWidget* ابنِ_صفحة_التحويلات(void) {
    SadWidget* page = عمود();
    عيّن_الخلفية(page, لون_الخلفية);
    عيّن_الحشو(page, حشو(16));
    عيّن_التباعد(page, 16);
    
    SadWidget* title = نص("التحويلات");
    title->textData.textStyle = SADTEXT_HEADLINE5;
    أضف(page, title);
    
    // نوع التحويل
    SadWidget* typeCard = بطاقة();
    عيّن_الحشو(typeCard, حشو(16));
    
    SadWidget* typeCol = عمود();
    عيّن_التباعد(typeCol, 12);
    
    SadWidget* typeTitle = نص("اختر نوع التحويل");
    typeTitle->textData.textStyle = SADTEXT_HEADLINE6;
    أضف(typeCol, typeTitle);
    
    أضف(typeCol, زر_ملون("تحويل محلي", لون_أساسي, NULL, NULL));
    أضف(typeCol, زر_ملون("تحويل دولي", لون_ثانوي, NULL, NULL));
    أضف(typeCol, زر_نص("بين حساباتي", NULL, NULL));
    
    أضف(typeCard, typeCol);
    أضف(page, typeCard);
    
    // المستفيدون
    SadWidget* beneficiaryCard = بطاقة();
    عيّن_الحشو(beneficiaryCard, حشو(16));
    
    SadWidget* benCol = عمود();
    عيّن_التباعد(benCol, 12);
    
    SadWidget* benTitle = نص("المستفيدون المحفوظون");
    benTitle->textData.textStyle = SADTEXT_HEADLINE6;
    أضف(benCol, benTitle);
    
    أضف(benCol, عنصر_معاملة("سارة أحمد", "SA0987654321", "بنك الراجحي", true));
    أضف(benCol, فاصل());
    أضف(benCol, عنصر_معاملة("محمد علي", "SA1122334455", "البنك الأهلي", true));
    
    أضف(beneficiaryCard, benCol);
    أضف(page, beneficiaryCard);
    
    return page;
}

/**
 * صفحة المدفوعات
 */
static SadWidget* ابنِ_صفحة_المدفوعات(void) {
    SadWidget* page = عمود();
    عيّن_الخلفية(page, لون_الخلفية);
    عيّن_الحشو(page, حشو(16));
    عيّن_التباعد(page, 16);
    
    SadWidget* title = نص("المدفوعات");
    title->textData.textStyle = SADTEXT_HEADLINE5;
    أضف(page, title);
    
    // الفواتير المعلقة
    SadWidget* billsCard = بطاقة();
    عيّن_الخلفية(billsCard, sadcolor(255, 243, 224, 255)); // برتقالي فاتح
    عيّن_الحشو(billsCard, حشو(16));
    
    SadWidget* billsCol = عمود();
    عيّن_التباعد(billsCol, 8);
    
    SadWidget* billsTitle = نص("⚠️ فواتير معلقة");
    billsTitle->textData.textStyle = SADTEXT_HEADLINE6;
    أضف(billsCol, billsTitle);
    
    SadWidget* billsAmount = نص("869 ر.س");
    billsAmount->textData.textStyle.fontSize = 24;
    billsAmount->textData.textStyle.fontStyle = SADFONT_BOLD;
    billsAmount->textData.textStyle.color = برتقالي;
    أضف(billsCol, billsAmount);
    
    أضف(billsCol, زر_ملون("دفع الكل", برتقالي, NULL, NULL));
    
    أضف(billsCard, billsCol);
    أضف(page, billsCard);
    
    // خدمات الدفع
    SadWidget* servicesRow = صف();
    عيّن_المحاذاة(servicesRow, توزيع_حول, منتصف);
    
    أضف(servicesRow, زر_إجراء("سداد", SADICON_RECEIPT, NULL));
    أضف(servicesRow, زر_إجراء("جوال", SADICON_PHONE, NULL));
    أضف(servicesRow, زر_إجراء("حكومية", SADICON_ACCOUNT_BALANCE, NULL));
    أضف(servicesRow, زر_إجراء("تبرعات", SADICON_FAVORITE, NULL));
    
    أضف(page, servicesRow);
    
    return page;
}

/**
 * صفحة البطاقات
 */
static SadWidget* ابنِ_صفحة_البطاقات(void) {
    SadWidget* page = عمود();
    عيّن_الخلفية(page, لون_الخلفية);
    عيّن_الحشو(page, حشو(16));
    عيّن_التباعد(page, 16);
    
    SadWidget* title = نص("بطاقاتي");
    title->textData.textStyle = SADTEXT_HEADLINE5;
    أضف(page, title);
    
    // بطاقة مدى
    SadWidget* madaCard = بطاقة();
    عيّن_التدرج(madaCard, sadcolor(30, 60, 114, 255), sadcolor(42, 82, 152, 255), 135);
    عيّن_الحشو(madaCard, حشو(24));
    عيّن_الزوايا(madaCard, زوايا(16));
    عيّن_الحجم(madaCard, -1, 180);
    
    SadWidget* cardCol = عمود();
    عيّن_التباعد(cardCol, 16);
    
    SadWidget* cardType = نص("بطاقة مدى");
    cardType->textData.textStyle.color = أبيض;
    cardType->textData.textStyle.fontSize = 14;
    أضف(cardCol, cardType);
    
    أضف(cardCol, مسافة(20));
    
    SadWidget* cardNumber = نص("•••• •••• •••• 4532");
    cardNumber->textData.textStyle.color = أبيض;
    cardNumber->textData.textStyle.fontSize = 22;
    cardNumber->textData.textStyle.letterSpacing = 2;
    أضف(cardCol, cardNumber);
    
    SadWidget* cardExpiry = نص("الصلاحية: 08/26");
    cardExpiry->textData.textStyle.color = sadcolor(200, 200, 200, 255);
    cardExpiry->textData.textStyle.fontSize = 12;
    أضف(cardCol, cardExpiry);
    
    أضف(madaCard, cardCol);
    أضف(page, madaCard);
    
    // إدارة البطاقة
    SadWidget* manageCard = بطاقة();
    عيّن_الحشو(manageCard, حشو(16));
    
    SadWidget* manageCol = عمود();
    عيّن_التباعد(manageCol, 12);
    
    SadWidget* manageTitle = نص("إدارة البطاقة");
    manageTitle->textData.textStyle = SADTEXT_HEADLINE6;
    أضف(manageCol, manageTitle);
    
    أضف(manageCol, زر_نص("🔒 تجميد البطاقة", NULL, NULL));
    أضف(manageCol, زر_نص("🔑 تغيير الرقم السري", NULL, NULL));
    أضف(manageCol, زر_نص("📋 كشف حساب البطاقة", NULL, NULL));
    
    أضف(manageCard, manageCol);
    أضف(page, manageCard);
    
    return page;
}

/**
 * صفحة الإعدادات
 */
static SadWidget* ابنِ_صفحة_الاعدادات(void) {
    SadWidget* page = عمود();
    عيّن_الخلفية(page, لون_الخلفية);
    عيّن_الحشو(page, حشو(16));
    عيّن_التباعد(page, 16);
    
    SadWidget* title = نص("الإعدادات");
    title->textData.textStyle = SADTEXT_HEADLINE5;
    أضف(page, title);
    
    // معلومات الحساب
    SadWidget* profileCard = بطاقة();
    عيّن_الحشو(profileCard, حشو(16));
    
    SadWidget* profileRow = صف();
    عيّن_التباعد(profileRow, 16);
    عيّن_المحاذاة(profileRow, بداية, منتصف);
    
    SadWidget* avatar = صورة_رمزية(NULL, 30);
    عيّن_الخلفية(avatar, لون_أساسي);
    أضف(profileRow, avatar);
    
    SadWidget* profileInfo = عمود();
    عيّن_التباعد(profileInfo, 4);
    
    SadWidget* name = نص("أحمد محمد");
    name->textData.textStyle = SADTEXT_HEADLINE6;
    أضف(profileInfo, name);
    
    SadWidget* email = نص("ahmed@example.com");
    email->textData.textStyle = SADTEXT_CAPTION;
    أضف(profileInfo, email);
    
    أضف(profileRow, profileInfo);
    أضف(profileCard, profileRow);
    أضف(page, profileCard);
    
    // إعدادات الأمان
    SadWidget* securityCard = بطاقة();
    عيّن_الحشو(securityCard, حشو(16));
    
    SadWidget* secCol = عمود();
    عيّن_التباعد(secCol, 12);
    
    SadWidget* secTitle = نص("🔐 الأمان");
    secTitle->textData.textStyle = SADTEXT_HEADLINE6;
    أضف(secCol, secTitle);
    
    // البصمة
    SadWidget* fingerprintRow = صف();
    عيّن_المحاذاة(fingerprintRow, توزيع_متساوي, منتصف);
    
    SadWidget* fingerprintLabel = نص("تسجيل بالبصمة");
    أضف(fingerprintRow, fingerprintLabel);
    
    SadWidget* fingerprintSwitch = مفتاح(true, NULL, NULL);
    أضف(fingerprintRow, fingerprintSwitch);
    
    أضف(secCol, fingerprintRow);
    
    // التحقق بخطوتين
    SadWidget* twoFactorRow = صف();
    عيّن_المحاذاة(twoFactorRow, توزيع_متساوي, منتصف);
    
    SadWidget* twoFactorLabel = نص("التحقق بخطوتين");
    أضف(twoFactorRow, twoFactorLabel);
    
    SadWidget* twoFactorSwitch = مفتاح(true, NULL, NULL);
    أضف(twoFactorRow, twoFactorSwitch);
    
    أضف(secCol, twoFactorRow);
    
    أضف(securityCard, secCol);
    أضف(page, securityCard);
    
    // الإشعارات
    SadWidget* notifCard = بطاقة();
    عيّن_الحشو(notifCard, حشو(16));
    
    SadWidget* notifCol = عمود();
    عيّن_التباعد(notifCol, 12);
    
    SadWidget* notifTitle = نص("🔔 الإشعارات");
    notifTitle->textData.textStyle = SADTEXT_HEADLINE6;
    أضف(notifCol, notifTitle);
    
    SadWidget* notifRow = صف();
    عيّن_المحاذاة(notifRow, توزيع_متساوي, منتصف);
    
    SadWidget* notifLabel = نص("إشعارات العمليات");
    أضف(notifRow, notifLabel);
    
    SadWidget* notifSwitch = مفتاح(true, NULL, NULL);
    أضف(notifRow, notifSwitch);
    
    أضف(notifCol, notifRow);
    
    أضف(notifCard, notifCol);
    أضف(page, notifCard);
    
    return page;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  بناء الواجهة الرئيسية
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * بناء شريط التنقل السفلي
 */
static SadWidget* ابنِ_شريط_التنقل(void) {
    SadWidget* nav = صف();
    عيّن_الحجم(nav, -1, 56);
    عيّن_الخلفية(nav, لون_السطح);
    عيّن_المحاذاة(nav, توزيع_حول, منتصف);
    
    // عناصر التنقل
    const char* labels[] = {"الرئيسية", "التحويلات", "المدفوعات", "البطاقات", "الإعدادات"};
    int icons[] = {SADICON_HOME, SADICON_TRANSFER, SADICON_PAYMENT, SADICON_CREDIT_CARD, SADICON_SETTINGS};
    
    for (int i = 0; i < 5; i++) {
        SadWidget* item = عمود();
        عيّن_المحاذاة(item, منتصف, منتصف);
        عيّن_التباعد(item, 4);
        عيّن_الحجم(item, 64, 48);
        عند_الضغط(item, عند_تغيير_الصفحة, (void*)(long)i);
        
        SadColor color = (i == الصفحة_الحالية) ? لون_أساسي : رمادي;
        
        SadWidget* icon = أيقونة(icons[i], 24, color);
        أضف(item, icon);
        
        SadWidget* label = نص(labels[i]);
        label->textData.textStyle.fontSize = 10;
        label->textData.textStyle.color = color;
        أضف(item, label);
        
        أضف(nav, item);
    }
    
    return nav;
}

/**
 * الدالة الرئيسية لبناء الواجهة
 * يستدعيها الجسر تلقائياً
 */
SadWidget* ابنِ_الواجهة(void) {
    // الهيكل الرئيسي
    SadWidget* scaffold = عمود();
    عيّن_الخلفية(scaffold, لون_الخلفية);
    
    // شريط التطبيق
    SadWidget* appBar = صف();
    عيّن_الحجم(appBar, -1, 56);
    عيّن_الخلفية(appBar, لون_أساسي);
    عيّن_الحشو(appBar, حشو_متناظر(0, 16));
    عيّن_المحاذاة(appBar, توزيع_متساوي, منتصف);
    
    SadWidget* appTitle = نص("🏦 بنك ص");
    appTitle->textData.textStyle = SADTEXT_HEADLINE6;
    appTitle->textData.textStyle.color = أبيض;
    أضف(appBar, appTitle);
    
    SadWidget* notifIcon = زر_أيقونة(SADICON_NOTIFICATIONS, NULL, NULL);
    notifIcon->iconData.iconColor = أبيض;
    أضف(appBar, notifIcon);
    
    أضف(scaffold, appBar);
    
    // محتوى الصفحات (Stack)
    محتوى_الصفحات = مكدس();
    عيّن_المرونة(محتوى_الصفحات, 1);
    
    // بناء الصفحات
    صفحات[0] = ابنِ_صفحة_الرئيسية();
    صفحات[1] = ابنِ_صفحة_التحويلات();
    صفحات[2] = ابنِ_صفحة_المدفوعات();
    صفحات[3] = ابنِ_صفحة_البطاقات();
    صفحات[4] = ابنِ_صفحة_الاعدادات();
    
    // إضافة الصفحات وإخفاء غير النشطة
    for (int i = 0; i < 5; i++) {
        أضف(محتوى_الصفحات, صفحات[i]);
        if (i != الصفحة_الحالية) {
            أخفِ(صفحات[i]);
        }
    }
    
    // إضافة التمرير
    SadWidget* scrollable = تمرير();
    أضف(scrollable, محتوى_الصفحات);
    أضف(scaffold, scrollable);
    
    // شريط التنقل السفلي
    SadWidget* bottomNav = ابنِ_شريط_التنقل();
    أضف(scaffold, bottomNav);
    
    return scaffold;
}

// للتوافق مع الجسر القديم
int main(void) {
    // لا شيء - الواجهة تُبنى عبر ابنِ_الواجهة
    return 0;
}
