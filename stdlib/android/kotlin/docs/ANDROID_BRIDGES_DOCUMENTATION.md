# توثيق جسور Android لـ لغة ص
# Android Bridges Documentation for Sad Language

هذا التوثيق يشرح كيفية استخدام جسور Android المكتوبة بـ Kotlin في لغة ص.

---

## نظرة عامة

تم إنشاء **100 جسر** تغطي ثلاث فئات رئيسية:

| الفئة | العدد | المسار |
|-------|-------|--------|
| الاتصالات (Communication) | 45 | `communication/` |
| الويدجت (Widgets) | 25 | `widgets/` |
| التلفزيون (TV) | 30 | `tv/` |

---

## 1. جسور الاتصالات (45 جسر)

### 1.1 الرسائل النصية SMS

```sad
استورد "android/communication/sms"

# إرسال رسالة
متغير نتيجة = SadSMSBridge.أرسل("+966501234567"، "مرحباً!")

# قراءة الرسائل
متغير الرسائل = SadSMSBridge.اقرأ_الرسائل(50)
لكل رسالة في الرسائل
    اطبع_سطر(رسالة.المحتوى)
نهاية
```

### 1.2 جهات الاتصال

```sad
استورد "android/communication/contacts"

# الحصول على جهات الاتصال
متغير جهات = SadContactsBridge.احصل_على_الكل()

# إضافة جهة اتصال
SadContactsBridge.أضف(
    الاسم: "أحمد محمد"،
    الهاتف: "+966501234567"،
    البريد: "ahmed@example.com"
)

# البحث
متغير نتائج = SadContactsBridge.ابحث("أحمد")
```

### 1.3 المكالمات

```sad
استورد "android/communication/calls"

# إجراء مكالمة
SadCallBridge.اتصل("+966501234567")

# سجل المكالمات
متغير السجل = SadCallBridge.احصل_على_السجل(20)
لكل مكالمة في السجل
    اطبع_سطر(مكالمة.الرقم + " - " + مكالمة.المدة)
نهاية
```

### 1.4 الإشعارات

```sad
استورد "android/communication/notifications"

# إرسال إشعار
SadNotificationBridge.أرسل(
    العنوان: "رسالة جديدة"،
    المحتوى: "لديك رسالة من أحمد"،
    القناة: "messages"
)

# إشعار مع إجراءات
SadNotificationBridge.أرسل_مع_إجراءات(
    العنوان: "مكالمة فائتة"،
    المحتوى: "أحمد حاول الاتصال بك"،
    الإجراءات: [
        إجراء(النص: "رد"، المعرف: "call_back")،
        إجراء(النص: "رسالة"، المعرف: "send_sms")
    ]
)
```

### قائمة جسور الاتصالات الكاملة:

| # | الجسر | الوصف |
|---|-------|-------|
| 1 | SadSMSBridge | إرسال واستقبال الرسائل النصية |
| 2 | SadContactsBridge | إدارة جهات الاتصال |
| 3 | SadCallBridge | إجراء المكالمات وسجل المكالمات |
| 4 | SadNotificationBridge | إرسال الإشعارات |
| 5 | SadEmailBridge | إرسال البريد الإلكتروني |
| 6 | SadMessagingBridge | المراسلة الفورية |
| 7 | SadVoIPBridge | مكالمات الإنترنت |
| 8 | SadBluetoothBridge | الاتصال بالبلوتوث |
| 9 | SadNFCBridge | الاتصال قريب المدى |
| 10 | SadWiFiBridge | شبكات الواي فاي |
| 11-45 | ... | (راجع الملفات المصدرية) |

---

## 2. جسور الويدجت (25 جسر)

### 2.1 ويدجت أساسي

```sad
استورد "android/widgets/core"

# تحديث الويدجت
SadWidgetCoreBridge.حدّث(
    المعرف: "my_widget"،
    البيانات: خريطة(
        "العنوان": "أهلاً!"،
        "القيمة": 42
    )
)
```

### 2.2 ويدجت Glance (Jetpack)

```sad
استورد "android/widgets/glance"

# إنشاء ويدجت بـ Glance
SadGlanceBridge.أنشئ_ويدجت(
    الاسم: "counter_widget"،
    العرض: WIDGET_SIZE_MEDIUM،
    المحتوى: لامدا()
        عمود()
            نص("العداد: " + القيمة)
            زر("زيادة"، فعل: action_increment)
        نهاية
    نهاية
)
```

### 2.3 ويدجت الساعة

```sad
استورد "android/widgets/clock"

SadClockWidgetBridge.أنشئ(
    التنسيق: "hh:mm a"،
    اللغة: "ar"،
    المنطقة_الزمنية: "Asia/Riyadh"
)
```

### 2.4 ويدجت الطقس

```sad
استورد "android/widgets/weather"

SadWeatherWidgetBridge.حدّث(
    الموقع: "الرياض"،
    الوحدة: CELSIUS
)
```

### قائمة جسور الويدجت الكاملة:

| # | الجسر | الوصف |
|---|-------|-------|
| 1 | SadWidgetCoreBridge | الوظائف الأساسية |
| 2 | SadGlanceBridge | Jetpack Glance |
| 3 | SadLayoutBridge | تخطيط الويدجت |
| 4 | SadActionBridge | الإجراءات والأحداث |
| 5 | SadUpdateBridge | تحديث البيانات |
| 6 | SadConfigBridge | إعدادات الويدجت |
| 7 | SadListBridge | قوائم الويدجت |
| 8 | SadDataBridge | ربط البيانات |
| 9 | SadThemeBridge | السمات والألوان |
| 10 | SadAnimationBridge | الحركات |
| 11 | SadProviderBridge | مزود الويدجت |
| 12 | SadClockWidgetBridge | ويدجت الساعة |
| 13 | SadWeatherWidgetBridge | ويدجت الطقس |
| 14 | SadBatteryWidgetBridge | ويدجت البطارية |
| 15 | SadMusicWidgetBridge | ويدجت الموسيقى |
| 16 | SadCalendarWidgetBridge | ويدجت التقويم |
| 17 | SadShortcutsWidgetBridge | ويدجت الاختصارات |
| 18 | SadCountdownWidgetBridge | ويدجت العد التنازلي |
| 19 | SadNotificationWidgetBridge | ويدجت الإشعارات |
| 20 | SadPinWidgetBridge | تثبيت الويدجت |
| 21 | SadQuotesWidgetBridge | ويدجت الاقتباسات |
| 22 | SadStepsWidgetBridge | ويدجت الخطوات |
| 23 | SadStorageWidgetBridge | ويدجت التخزين |
| 24 | SadNetworkWidgetBridge | ويدجت الشبكة |
| 25 | SadSystemInfoWidgetBridge | ويدجت معلومات النظام |

---

## 3. جسور التلفزيون (30 جسر)

### 3.1 التنقل بالـ D-pad

```sad
استورد "android/tv/navigation"

# التعامل مع التنقل
SadTVNavigationBridge.عند_الضغط(لامدا(الاتجاه)
    طابق الاتجاه
        عندما UP
            اطبع_سطر("للأعلى")
        عندما DOWN
            اطبع_سطر("للأسفل")
        عندما LEFT
            اطبع_سطر("لليسار")
        عندما RIGHT
            اطبع_سطر("لليمين")
    نهاية
نهاية)
```

### 3.2 واجهة التصفح (Browse)

```sad
استورد "android/tv/browse"

# إنشاء صفوف
متغير الصفوف = [
    صف_تصفح(
        العنوان: "الأفلام الجديدة"،
        العناصر: [
            عنصر(العنوان: "فيلم 1"، الصورة: "url1")،
            عنصر(العنوان: "فيلم 2"، الصورة: "url2")
        ]
    )
]

SadTVBrowseBridge.عرض(الصفوف)
```

### 3.3 مشغل الفيديو

```sad
استورد "android/tv/player"

# تشغيل فيديو
SadTVVideoPlayerBridge.شغّل(
    المصدر: "https://example.com/video.mp4"،
    العنوان: "الحلقة 1"،
    الترجمات: ["ar"، "en"]
)

# التحكم
SadTVVideoPlayerBridge.إيقاف_مؤقت()
SadTVVideoPlayerBridge.استئناف()
SadTVVideoPlayerBridge.انتقل_إلى(120000)  # بالمللي ثانية
```

### 3.4 صورة داخل صورة (PiP)

```sad
استورد "android/tv/picture"

# تفعيل PiP
SadTVPiPBridge.فعّل(
    العرض: 16،
    الارتفاع: 9،
    الإجراءات: [
        إجراء_pip("تشغيل"، PLAY)،
        إجراء_pip("إيقاف"، PAUSE)
    ]
)
```

### قائمة جسور التلفزيون الكاملة:

| # | الجسر | الوصف |
|---|-------|-------|
| 1 | SadTVCoreBridge | الوظائف الأساسية للتلفزيون |
| 2 | SadTVBrowseBridge | واجهة التصفح |
| 3 | SadTVDetailsBridge | صفحة التفاصيل |
| 4 | SadTVPlaybackBridge | التحكم بالتشغيل |
| 5 | SadTVSearchBridge | البحث الصوتي |
| 6 | SadTVCardsBridge | بطاقات العرض |
| 7 | SadTVRecommendationsBridge | التوصيات |
| 8 | SadTVNavigationBridge | التنقل D-pad |
| 9 | SadTVChannelsBridge | القنوات والبرامج |
| 10 | SadTVRemoteBridge | جهاز التحكم |
| 11 | SadTVOnboardingBridge | شاشات الترحيب |
| 12 | SadTVPreferencesBridge | الإعدادات |
| 13 | SadTVPresenterBridge | عرض المحتوى |
| 14 | SadTVVideoPlayerBridge | مشغل الفيديو |
| 15 | SadTVPiPBridge | صورة داخل صورة |
| 16 | SadTVAudioFocusBridge | التركيز الصوتي |
| 17 | SadTVMediaSessionBridge | جلسة الوسائط |
| 18 | SadTVAccessibilityBridge | إمكانية الوصول |
| 19 | SadTVInputBridge | مدخلات المفاتيح |
| 20 | SadTVVoiceBridge | الأوامر الصوتية |
| 21 | SadTVScreensaverBridge | شاشة التوقف |
| 22 | SadTVGridBridge | عرض الشبكة |
| 23 | SadTVHeadersBridge | العناوين |
| 24 | SadTVGuidedStepBridge | الخطوات الموجهة |
| 25 | SadTVErrorBridge | صفحات الخطأ |
| 26 | SadTVHDRBridge | دعم HDR |
| 27 | SadTVPreviewBridge | قنوات المعاينة |
| 28 | SadTVGamepadBridge | جهاز التحكم بالألعاب |
| 29 | SadTVFocusAnimationBridge | حركات التركيز |
| 30 | SadTVLoadingBridge | حالات التحميل |

---

## التطبيقات التجريبية

### تطبيق الدردشة

```sad
استورد "android/demo/chat"

# تهيئة التطبيق
تطبيق_الدردشة.هيئ(السياق)

# إنشاء محادثة
متغير محادثة = تطبيق_الدردشة.أنشئ_محادثة([
    مشارك(الاسم: "أحمد"، الهاتف: "+966501234567")
])

# إرسال رسالة
تطبيق_الدردشة.أرسل_رسالة(محادثة.المعرف، "مرحباً!")
```

### تطبيق الإنتاجية

```sad
استورد "android/demo/productivity"

# تهيئة التطبيق
تطبيق_الإنتاجية.هيئ(السياق)

# إضافة مهمة
متغير مهمة = تطبيق_الإنتاجية.أضف_مهمة(
    "إنهاء التقرير"،
    الأولوية: عاجل،
    التاريخ: غداً
)

# إكمال المهمة
تطبيق_الإنتاجية.أكمل_مهمة(مهمة.المعرف)

# إحصائيات اليوم
متغير إحصائيات = تطبيق_الإنتاجية.إحصائيات_اليوم()
اطبع_سطر("المهام المكتملة: " + إحصائيات.المكتملة)
```

---

## أفضل الممارسات

### 1. معالجة الأخطاء

```sad
حاول
    SadSMSBridge.أرسل(الرقم، الرسالة)
امسك خطأ
    اطبع_سطر("فشل الإرسال: " + خطأ.الرسالة)
نهاية
```

### 2. الصلاحيات

```sad
# التحقق من الصلاحيات قبل الاستخدام
إذا (SadPermissionBridge.لديه_صلاحية(SMS_SEND))
    SadSMSBridge.أرسل(الرقم، الرسالة)
وإلا
    SadPermissionBridge.اطلب(SMS_SEND)
نهاية
```

### 3. دورة الحياة

```sad
# في onResume
SadWidgetCoreBridge.استأنف()

# في onPause
SadWidgetCoreBridge.أوقف_مؤقتاً()

# في onDestroy
SadWidgetCoreBridge.حرر()
```

---

## المراجع

- [Android Developers](https://developer.android.com)
- [Leanback Library](https://developer.android.com/training/tv)
- [App Widgets](https://developer.android.com/develop/ui/views/appwidgets)
- [توثيق لغة ص](../../../docs/)

---

© 2024 فريق لغة ص
