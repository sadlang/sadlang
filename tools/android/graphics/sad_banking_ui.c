/**
 * ═══════════════════════════════════════════════════════════════════════════════
 *  تنفيذ مكونات واجهة المستخدم البنكية
 *  Banking UI Components Implementation
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#include "sad_banking_ui.h"
#include "sad_ui_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <android/log.h>

#define LOG_TAG "SadBankUI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// ═══════════════════════════════════════════════════════════════════════════════
//  جداول البحث
// ═══════════════════════════════════════════════════════════════════════════════

static const char* TX_TYPE_STRINGS[] = {
    "إيداع",
    "سحب",
    "تحويل وارد",
    "تحويل صادر",
    "دفع",
    "راتب",
    "فاتورة",
    "استرداد",
    "رسوم"
};

static const char* TX_TYPE_EMOJIS[] = {
    "💰",  // إيداع
    "💸",  // سحب
    "⬇️",  // تحويل وارد
    "⬆️",  // تحويل صادر
    "💳",  // دفع
    "💼",  // راتب
    "📄",  // فاتورة
    "↩️",  // استرداد
    "📋"   // رسوم
};

static const SadColor TX_TYPE_COLORS[] = {
    0x00C853FF,  // إيداع - أخضر
    0xFF5252FF,  // سحب - أحمر
    0x448AFFFF,  // تحويل وارد - أزرق
    0xFF6D00FF,  // تحويل صادر - برتقالي
    0x7C4DFFFF,  // دفع - بنفسجي
    0x00BFA5FF,  // راتب - فيروزي
    0xFFAB00FF,  // فاتورة - أصفر
    0x69F0AEFF,  // استرداد - أخضر فاتح
    0x90A4AEFF   // رسوم - رمادي
};

// ═══════════════════════════════════════════════════════════════════════════════
//  تنسيق العملات
// ═══════════════════════════════════════════════════════════════════════════════

void sadbank_format_currency(
    char* output,
    int outputSize,
    float amount,
    const char* currency
) {
    // تنسيق المبلغ مع الفواصل
    char formatted[32];
    int intPart = (int)amount;
    int decPart = (int)((amount - intPart) * 100);
    
    // إضافة الفواصل للأرقام الكبيرة
    if (intPart >= 1000000) {
        snprintf(formatted, sizeof(formatted), "%d,%03d,%03d.%02d",
            intPart / 1000000,
            (intPart / 1000) % 1000,
            intPart % 1000,
            decPart);
    } else if (intPart >= 1000) {
        snprintf(formatted, sizeof(formatted), "%d,%03d.%02d",
            intPart / 1000,
            intPart % 1000,
            decPart);
    } else {
        snprintf(formatted, sizeof(formatted), "%d.%02d", intPart, decPart);
    }
    
    snprintf(output, outputSize, "%s %s", formatted, currency);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال المساعدة
// ═══════════════════════════════════════════════════════════════════════════════

const char* sadbank_tx_type_to_string(SadBankTxType type) {
    if (type >= 0 && type <= SADBANK_TX_FEE) {
        return TX_TYPE_STRINGS[type];
    }
    return "غير معروف";
}

SadColor sadbank_tx_type_color(SadBankTxType type) {
    if (type >= 0 && type <= SADBANK_TX_FEE) {
        return TX_TYPE_COLORS[type];
    }
    return 0x808080FF;  // رمادي
}

const char* sadbank_tx_type_emoji(SadBankTxType type) {
    if (type >= 0 && type <= SADBANK_TX_FEE) {
        return TX_TYPE_EMOJIS[type];
    }
    return "❓";
}

SadGradient sadbank_card_gradient(SadBankCardDesign design) {
    SadGradient gradient = {0};
    
    switch (design) {
        case SADBANK_CARD_DESIGN_CLASSIC:
            gradient.start = 0x1A237EFF;  // أزرق داكن
            gradient.end = 0x3F51B5FF;    // أزرق
            break;
        case SADBANK_CARD_DESIGN_GOLD:
            gradient.start = 0xFFD700FF;  // ذهبي
            gradient.end = 0xFFA000FF;    // برتقالي ذهبي
            break;
        case SADBANK_CARD_DESIGN_PLATINUM:
            gradient.start = 0xB0BEC5FF;  // فضي فاتح
            gradient.end = 0x78909CFF;    // فضي داكن
            break;
        case SADBANK_CARD_DESIGN_SIGNATURE:
            gradient.start = 0x212121FF;  // أسود
            gradient.end = 0x424242FF;    // رمادي داكن
            break;
        case SADBANK_CARD_DESIGN_ISLAMIC:
            gradient.start = 0x00695CFF;  // أخضر زمردي
            gradient.end = 0x004D40FF;    // أخضر داكن
            break;
        case SADBANK_CARD_DESIGN_YOUTH:
            gradient.start = 0xE91E63FF;  // وردي
            gradient.end = 0x9C27B0FF;    // بنفسجي
            break;
        default:
            gradient.start = 0x6750A4FF;
            gradient.end = 0x7F67BEFF;
            break;
    }
    
    gradient.angle = 135.0f;  // زاوية قطرية
    return gradient;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  إنشاء عناصر الواجهة - التنفيذ الكامل
// ═══════════════════════════════════════════════════════════════════════════════

SadWidgetId sadbank_create_card_display(
    const SadBankCard* card,
    const SadBankCardDisplayConfig* config
) {
    if (!card) return 0;
    
    // إنشاء حاوية البطاقة
    SadWidgetId container = sadui_create_container();
    
    // تطبيق النمط
    SadStyle style = sadui_default_style();
    style.cornerRadius = sadui_corners(16.0f);
    style.shadowRadius = 12.0f;
    style.shadowColor = 0x00000040;
    
    // تدرج لوني حسب نوع البطاقة
    style.backgroundGradient = sadbank_card_gradient(card->design);
    
    sadui_set_style(container, style);
    
    // إعداد التخطيط
    SadLayout layout = sadui_default_layout();
    layout.width = sadui_dp(340);
    layout.height = sadui_dp(200);
    layout.padding = sadui_edges(20);
    sadui_set_layout(container, layout);
    
    // إضافة شعار نوع البطاقة
    const char* cardLogo = "";
    switch (card->type) {
        case SADBANK_CARD_MADA: cardLogo = "🏦 مدى"; break;
        case SADBANK_CARD_VISA: cardLogo = "VISA"; break;
        case SADBANK_CARD_MASTERCARD: cardLogo = "Mastercard"; break;
        case SADBANK_CARD_AMEX: cardLogo = "AMEX"; break;
        case SADBANK_CARD_VIRTUAL: cardLogo = "💳 افتراضية"; break;
        case SADBANK_CARD_PREPAID: cardLogo = "💵 مسبقة الدفع"; break;
    }
    
    SadWidgetId logoLabel = sadui_create_label(cardLogo);
    sadui_add_child(container, logoLabel);
    
    // إضافة رقم البطاقة
    char cardNumDisplay[32];
    snprintf(cardNumDisplay, sizeof(cardNumDisplay), 
        "**** **** **** %s", card->cardNumber + 12);
    
    SadWidgetId numLabel = sadui_create_label(cardNumDisplay);
    sadui_add_child(container, numLabel);
    
    // إضافة اسم حامل البطاقة
    SadWidgetId nameLabel = sadui_create_label(card->holderName);
    sadui_add_child(container, nameLabel);
    
    // إضافة تاريخ الانتهاء
    char expiryDisplay[32];
    snprintf(expiryDisplay, sizeof(expiryDisplay), "صالحة حتى: %s", card->expiryDate);
    SadWidgetId expiryLabel = sadui_create_label(expiryDisplay);
    sadui_add_child(container, expiryLabel);
    
    // إضافة رمز NFC إذا كانت البطاقة تدعم الدفع اللاتلامسي
    if (card->isContactless && config && config->showContactless) {
        SadWidgetId nfcIcon = sadui_create_label("📶");
        sadui_add_child(container, nfcIcon);
    }
    
    LOGI("Created card display: %s ****%s", 
        cardLogo, card->cardNumber + 12);
    
    return container;
}

SadWidgetId sadbank_create_balance_display(
    float balance,
    const char* currency,
    bool showChange,
    float changePercent
) {
    SadWidgetId container = sadui_create_container();
    
    // تنسيق الرصيد
    char balanceStr[64];
    sadbank_format_currency(balanceStr, sizeof(balanceStr), balance, currency);
    
    // عنوان الرصيد
    SadWidgetId titleLabel = sadui_create_label("💰 الرصيد المتاح");
    sadui_add_child(container, titleLabel);
    
    // قيمة الرصيد
    SadWidgetId balanceLabel = sadui_create_label(balanceStr);
    SadStyle balanceStyle = sadui_default_style();
    balanceStyle.textColor = SAD_COLOR_PRIMARY;
    // balanceStyle.fontSize = 32;  // حجم كبير
    sadui_set_style(balanceLabel, balanceStyle);
    sadui_add_child(container, balanceLabel);
    
    // عرض التغيير إذا مطلوب
    if (showChange) {
        char changeStr[32];
        const char* arrow = changePercent >= 0 ? "▲" : "▼";
        SadColor changeColor = changePercent >= 0 ? SADBANK_COLOR_SUCCESS : SADBANK_COLOR_DANGER;
        
        snprintf(changeStr, sizeof(changeStr), "%s %.1f%%", arrow, 
            changePercent >= 0 ? changePercent : -changePercent);
        
        SadWidgetId changeLabel = sadui_create_label(changeStr);
        SadStyle changeStyle = sadui_default_style();
        changeStyle.textColor = changeColor;
        sadui_set_style(changeLabel, changeStyle);
        sadui_add_child(container, changeLabel);
    }
    
    LOGI("Created balance display: %s", balanceStr);
    return container;
}

SadWidgetId sadbank_create_transaction_item(const SadBankTransaction* tx) {
    if (!tx) return 0;
    
    SadWidgetId container = sadui_create_container();
    
    // تخطيط أفقي
    SadLayout layout = sadui_default_layout();
    layout.direction = SAD_LAYOUT_ROW;
    layout.padding = sadui_edges(12);
    sadui_set_layout(container, layout);
    
    // أيقونة المعاملة
    SadWidgetId icon = sadui_create_label(sadbank_tx_type_emoji(tx->type));
    sadui_add_child(container, icon);
    
    // تفاصيل المعاملة
    SadWidgetId details = sadui_create_container();
    {
        SadWidgetId desc = sadui_create_label(tx->description);
        sadui_add_child(details, desc);
        
        SadWidgetId date = sadui_create_label(tx->date);
        SadStyle dateStyle = sadui_default_style();
        dateStyle.textColor = 0x808080FF;
        sadui_set_style(date, dateStyle);
        sadui_add_child(details, date);
    }
    sadui_add_child(container, details);
    
    // المبلغ
    char amountStr[32];
    bool isCredit = (tx->type == SADBANK_TX_DEPOSIT || 
                     tx->type == SADBANK_TX_TRANSFER_IN ||
                     tx->type == SADBANK_TX_SALARY ||
                     tx->type == SADBANK_TX_REFUND);
    
    snprintf(amountStr, sizeof(amountStr), "%s%.2f ر.س",
        isCredit ? "+" : "-", tx->amount);
    
    SadWidgetId amountLabel = sadui_create_label(amountStr);
    SadStyle amountStyle = sadui_default_style();
    amountStyle.textColor = isCredit ? SADBANK_COLOR_SUCCESS : SADBANK_COLOR_DANGER;
    sadui_set_style(amountLabel, amountStyle);
    sadui_add_child(container, amountLabel);
    
    return container;
}

SadWidgetId sadbank_create_transaction_list(
    const SadBankTransaction* transactions,
    int count,
    bool groupByDate
) {
    SadWidgetId list = sadui_create_container();
    
    SadLayout layout = sadui_default_layout();
    layout.direction = SAD_LAYOUT_COLUMN;
    sadui_set_layout(list, layout);
    
    const char* lastDate = NULL;
    
    for (int i = 0; i < count && i < SADBANK_MAX_TRANSACTIONS; i++) {
        // إضافة فاصل التاريخ إذا مطلوب
        if (groupByDate && (!lastDate || strcmp(lastDate, transactions[i].date) != 0)) {
            SadWidgetId dateHeader = sadui_create_label(transactions[i].date);
            SadStyle dateStyle = sadui_default_style();
            dateStyle.backgroundColor = 0xF5F5F5FF;
            dateStyle.textColor = 0x757575FF;
            sadui_set_style(dateHeader, dateStyle);
            sadui_add_child(list, dateHeader);
            lastDate = transactions[i].date;
        }
        
        // إضافة عنصر المعاملة
        SadWidgetId item = sadbank_create_transaction_item(&transactions[i]);
        sadui_add_child(list, item);
    }
    
    LOGI("Created transaction list with %d items", count);
    return list;
}

SadWidgetId sadbank_create_chart(const SadBankChartConfig* config) {
    if (!config) return 0;
    
    SadWidgetId chart = sadui_create_container();
    
    // إعداد النمط
    SadStyle style = sadui_default_style();
    style.backgroundColor = config->backgroundColor;
    style.cornerRadius = sadui_corners(12);
    sadui_set_style(chart, style);
    
    // إضافة العنوان
    if (config->title[0] != '\0') {
        SadWidgetId title = sadui_create_label(config->title);
        sadui_add_child(chart, title);
    }
    
    // منطقة الرسم البياني
    SadWidgetId chartArea = sadui_create_container();
    SadLayout areaLayout = sadui_default_layout();
    areaLayout.width = sadui_fill();
    areaLayout.height = sadui_dp(200);
    sadui_set_layout(chartArea, areaLayout);
    sadui_add_child(chart, chartArea);
    
    // إضافة Legend إذا مطلوب
    if (config->showLegend && config->points && config->pointCount > 0) {
        SadWidgetId legend = sadui_create_container();
        SadLayout legendLayout = sadui_default_layout();
        legendLayout.direction = SAD_LAYOUT_ROW;
        legendLayout.mainAlign = SAD_ALIGN_CENTER;
        sadui_set_layout(legend, legendLayout);
        
        for (int i = 0; i < config->pointCount; i++) {
            char legendText[64];
            snprintf(legendText, sizeof(legendText), "● %s", config->points[i].label);
            SadWidgetId legendItem = sadui_create_label(legendText);
            SadStyle itemStyle = sadui_default_style();
            itemStyle.textColor = config->points[i].color;
            sadui_set_style(legendItem, itemStyle);
            sadui_add_child(legend, legendItem);
        }
        
        sadui_add_child(chart, legend);
    }
    
    LOGI("Created %s chart: %s", 
        config->type == SADBANK_CHART_PIE ? "pie" : 
        config->type == SADBANK_CHART_LINE ? "line" : "bar",
        config->title);
    
    return chart;
}

SadWidgetId sadbank_create_security_meter(int score, bool showLabel) {
    SadWidgetId container = sadui_create_container();
    
    // تحديد اللون حسب الدرجة
    SadColor meterColor;
    const char* label;
    
    if (score >= 80) {
        meterColor = SADBANK_COLOR_SUCCESS;
        label = "🔒 ممتاز";
    } else if (score >= 60) {
        meterColor = SADBANK_COLOR_SECURE;
        label = "🔐 جيد";
    } else if (score >= 40) {
        meterColor = SADBANK_COLOR_WARNING;
        label = "⚠️ متوسط";
    } else {
        meterColor = SADBANK_COLOR_DANGER;
        label = "🚨 ضعيف";
    }
    
    // شريط التقدم
    SadWidgetId progressBar = sadui_create_progress((float)score / 100.0f);
    SadStyle barStyle = sadui_default_style();
    barStyle.backgroundColor = 0xE0E0E0FF;
    barStyle.primaryColor = meterColor;
    barStyle.cornerRadius = sadui_corners(4);
    sadui_set_style(progressBar, barStyle);
    sadui_add_child(container, progressBar);
    
    // التسمية
    if (showLabel) {
        SadWidgetId labelWidget = sadui_create_label(label);
        SadStyle labelStyle = sadui_default_style();
        labelStyle.textColor = meterColor;
        sadui_set_style(labelWidget, labelStyle);
        sadui_add_child(container, labelWidget);
    }
    
    LOGI("Created security meter: %d%% (%s)", score, label);
    return container;
}

SadWidgetId sadbank_create_account_card(
    const char* accountName,
    const char* accountNumber,
    float balance,
    const char* currency,
    const char* icon
) {
    SadWidgetId card = sadui_create_container();
    
    // النمط
    SadStyle style = sadui_default_style();
    style.backgroundColor = SAD_COLOR_SURFACE;
    style.cornerRadius = sadui_corners(16);
    style.shadowRadius = 4;
    style.shadowColor = 0x00000020;
    sadui_set_style(card, style);
    
    // التخطيط
    SadLayout layout = sadui_default_layout();
    layout.padding = sadui_edges(16);
    layout.direction = SAD_LAYOUT_ROW;
    sadui_set_layout(card, layout);
    
    // الأيقونة
    SadWidgetId iconWidget = sadui_create_label(icon);
    sadui_add_child(card, iconWidget);
    
    // المعلومات
    SadWidgetId info = sadui_create_container();
    {
        SadWidgetId name = sadui_create_label(accountName);
        sadui_add_child(info, name);
        
        SadWidgetId number = sadui_create_label(accountNumber);
        SadStyle numStyle = sadui_default_style();
        numStyle.textColor = 0x808080FF;
        sadui_set_style(number, numStyle);
        sadui_add_child(info, number);
    }
    sadui_add_child(card, info);
    
    // الرصيد
    char balanceStr[32];
    sadbank_format_currency(balanceStr, sizeof(balanceStr), balance, currency);
    SadWidgetId balanceWidget = sadui_create_label(balanceStr);
    SadStyle balStyle = sadui_default_style();
    balStyle.textColor = SAD_COLOR_PRIMARY;
    sadui_set_style(balanceWidget, balStyle);
    sadui_add_child(card, balanceWidget);
    
    return card;
}

SadWidgetId sadbank_create_quick_actions(void) {
    SadWidgetId container = sadui_create_container();
    
    // تخطيط أفقي متساوي
    SadLayout layout = sadui_default_layout();
    layout.direction = SAD_LAYOUT_ROW;
    layout.mainAlign = SAD_ALIGN_SPACE_AROUND;
    layout.crossAlign = SAD_ALIGN_CENTER;
    layout.padding = sadui_edges(16);
    sadui_set_layout(container, layout);
    
    // الإجراءات
    const char* actions[][2] = {
        {"💸", "تحويل"},
        {"📝", "دفع"},
        {"📥", "طلب"},
        {"🔗", "المزيد"}
    };
    
    for (int i = 0; i < 4; i++) {
        SadWidgetId action = sadui_create_container();
        
        // الأيقونة
        SadWidgetId actionIcon = sadui_create_label(actions[i][0]);
        sadui_add_child(action, actionIcon);
        
        // النص
        SadWidgetId actionLabel = sadui_create_label(actions[i][1]);
        sadui_add_child(action, actionLabel);
        
        sadui_add_child(container, action);
    }
    
    return container;
}

SadWidgetId sadbank_create_biometric_prompt(
    SadBankBiometricType type,
    const char* title,
    const char* subtitle
) {
    SadWidgetId dialog = sadui_create_container();
    
    // نمط الحوار
    SadStyle style = sadui_default_style();
    style.backgroundColor = SAD_COLOR_SURFACE;
    style.cornerRadius = sadui_corners(28);
    style.shadowRadius = 16;
    style.shadowColor = 0x00000040;
    sadui_set_style(dialog, style);
    
    // الأيقونة حسب النوع
    const char* icon;
    switch (type) {
        case SADBANK_BIO_FINGERPRINT: icon = "👆"; break;
        case SADBANK_BIO_FACE: icon = "👤"; break;
        case SADBANK_BIO_IRIS: icon = "👁️"; break;
        default: icon = "🔐"; break;
    }
    
    SadWidgetId iconWidget = sadui_create_label(icon);
    sadui_add_child(dialog, iconWidget);
    
    // العنوان
    SadWidgetId titleWidget = sadui_create_label(title ? title : "التحقق البيومتري");
    sadui_add_child(dialog, titleWidget);
    
    // العنوان الفرعي
    SadWidgetId subtitleWidget = sadui_create_label(
        subtitle ? subtitle : "ضع إصبعك على المستشعر");
    SadStyle subStyle = sadui_default_style();
    subStyle.textColor = 0x808080FF;
    sadui_set_style(subtitleWidget, subStyle);
    sadui_add_child(dialog, subtitleWidget);
    
    // زر الإلغاء
    SadWidgetId cancelBtn = sadui_create_button("إلغاء");
    sadui_add_child(dialog, cancelBtn);
    
    LOGI("Created biometric prompt: %s", title);
    return dialog;
}

SadWidgetId sadbank_create_notification_card(
    const char* title,
    const char* message,
    const char* type,
    bool dismissible
) {
    SadWidgetId card = sadui_create_container();
    
    // تحديد اللون والأيقونة
    SadColor bgColor;
    const char* icon;
    
    if (strcmp(type, "success") == 0) {
        bgColor = 0xE8F5E9FF;  // أخضر فاتح
        icon = "✅";
    } else if (strcmp(type, "warning") == 0) {
        bgColor = 0xFFF8E1FF;  // أصفر فاتح
        icon = "⚠️";
    } else if (strcmp(type, "error") == 0) {
        bgColor = 0xFFEBEEFF;  // أحمر فاتح
        icon = "❌";
    } else {
        bgColor = 0xE3F2FDFF;  // أزرق فاتح
        icon = "ℹ️";
    }
    
    // النمط
    SadStyle style = sadui_default_style();
    style.backgroundColor = bgColor;
    style.cornerRadius = sadui_corners(12);
    sadui_set_style(card, style);
    
    // التخطيط
    SadLayout layout = sadui_default_layout();
    layout.direction = SAD_LAYOUT_ROW;
    layout.padding = sadui_edges(16);
    sadui_set_layout(card, layout);
    
    // الأيقونة
    SadWidgetId iconWidget = sadui_create_label(icon);
    sadui_add_child(card, iconWidget);
    
    // المحتوى
    SadWidgetId content = sadui_create_container();
    {
        SadWidgetId titleWidget = sadui_create_label(title);
        sadui_add_child(content, titleWidget);
        
        SadWidgetId msgWidget = sadui_create_label(message);
        SadStyle msgStyle = sadui_default_style();
        msgStyle.textColor = 0x616161FF;
        sadui_set_style(msgWidget, msgStyle);
        sadui_add_child(content, msgWidget);
    }
    sadui_add_child(card, content);
    
    // زر الإغلاق
    if (dismissible) {
        SadWidgetId closeBtn = sadui_create_button("✕");
        sadui_add_child(card, closeBtn);
    }
    
    return card;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال التحديث
// ═══════════════════════════════════════════════════════════════════════════════

void sadbank_update_balance(SadWidgetId widget, float newBalance, bool animate) {
    // TODO: تطبيق الحركة إذا مطلوب
    char balanceStr[64];
    sadbank_format_currency(balanceStr, sizeof(balanceStr), newBalance, "ر.س");
    sadui_set_text(widget, balanceStr);
    
    LOGI("Updated balance to: %s", balanceStr);
}

void sadbank_update_biometric_state(SadWidgetId widget, SadBankBiometricState state) {
    const char* statusText;
    SadColor statusColor;
    
    switch (state) {
        case SADBANK_BIO_IDLE:
            statusText = "في الانتظار...";
            statusColor = 0x808080FF;
            break;
        case SADBANK_BIO_SCANNING:
            statusText = "جاري المسح...";
            statusColor = SAD_COLOR_PRIMARY;
            break;
        case SADBANK_BIO_SUCCESS:
            statusText = "✅ تم التحقق بنجاح";
            statusColor = SADBANK_COLOR_SUCCESS;
            break;
        case SADBANK_BIO_FAILED:
            statusText = "❌ فشل التحقق";
            statusColor = SADBANK_COLOR_DANGER;
            break;
        case SADBANK_BIO_LOCKED:
            statusText = "🔒 مقفل - حاول لاحقاً";
            statusColor = SADBANK_COLOR_DANGER;
            break;
        default:
            return;
    }
    
    LOGI("Biometric state: %s", statusText);
}

void sadbank_flip_card(SadWidgetId widget) {
    // TODO: تطبيق حركة القلب
    LOGI("Card flip requested");
}

void sadbank_toggle_card_freeze(SadWidgetId widget, bool frozen) {
    // TODO: تحديث حالة التجميد
    LOGI("Card freeze: %s", frozen ? "frozen" : "active");
}

// ═══════════════════════════════════════════════════════════════════════════════
//  دوال الحركات
// ═══════════════════════════════════════════════════════════════════════════════

SadAnimId sadbank_animate_count_up(SadWidgetId widget, float duration) {
    SadAnimation anim = {
        .property = SAD_ANIM_CUSTOM,
        .duration = duration,
        .easing = SAD_EASE_OUT_CUBIC
    };
    return sadui_animate(widget, &anim);
}

SadAnimId sadbank_animate_success_pulse(SadWidgetId widget) {
    SadAnimation anim = {
        .property = SAD_ANIM_SCALE,
        .from = 1.0f,
        .to = 1.1f,
        .duration = 0.2f,
        .easing = SAD_EASE_OUT_BACK,
        .repeatMode = SAD_REPEAT_REVERSE,
        .repeatCount = 1
    };
    return sadui_animate(widget, &anim);
}

SadAnimId sadbank_animate_error_shake(SadWidgetId widget) {
    SadAnimation anim = {
        .property = SAD_ANIM_X,
        .from = -10.0f,
        .to = 10.0f,
        .duration = 0.1f,
        .easing = SAD_EASE_LINEAR,
        .repeatMode = SAD_REPEAT_REVERSE,
        .repeatCount = 3
    };
    return sadui_animate(widget, &anim);
}

SadAnimId sadbank_animate_card_reveal(SadWidgetId widget) {
    SadAnimation anim = {
        .property = SAD_ANIM_SCALE,
        .from = 0.8f,
        .to = 1.0f,
        .duration = 0.4f,
        .easing = SAD_EASE_OUT_BACK
    };
    
    // إضافة حركة الشفافية
    SadAnimation fadeAnim = {
        .property = SAD_ANIM_ALPHA,
        .from = 0.0f,
        .to = 1.0f,
        .duration = 0.3f,
        .easing = SAD_EASE_OUT_QUAD
    };
    
    sadui_animate(widget, &fadeAnim);
    return sadui_animate(widget, &anim);
}

SadAnimId sadbank_animate_fingerprint_scan(SadWidgetId widget) {
    SadAnimation anim = {
        .property = SAD_ANIM_ALPHA,
        .from = 0.5f,
        .to = 1.0f,
        .duration = 0.8f,
        .easing = SAD_EASE_IN_OUT_SINE,
        .repeatMode = SAD_REPEAT_REVERSE,
        .repeatCount = -1  // لانهائي
    };
    return sadui_animate(widget, &anim);
}

SadAnimId sadbank_animate_chart_draw(SadWidgetId widget, float duration) {
    SadAnimation anim = {
        .property = SAD_ANIM_CUSTOM,
        .duration = duration,
        .easing = SAD_EASE_OUT_CUBIC
    };
    return sadui_animate(widget, &anim);
}
