# دليل البدء السريع - جسور Android
# Quick Start Guide - Android Bridges

## المتطلبات

- Android Studio Arctic Fox أو أحدث
- Kotlin 1.8+
- Android SDK 21+ (للهواتف)
- Android SDK 21+ with Leanback (للتلفزيون)

## التثبيت

### 1. إضافة المستودع

```gradle
// settings.gradle.kts
dependencyResolutionManagement {
    repositories {
        maven { url = uri("https://sad-lang.org/maven") }
    }
}
```

### 2. إضافة الاعتمادية

```gradle
// build.gradle.kts (app)
dependencies {
    implementation("org.sadlang:android-bridges:1.0.0")
    
    // للويدجت
    implementation("org.sadlang:android-widgets:1.0.0")
    
    // للتلفزيون
    implementation("org.sadlang:android-tv:1.0.0")
}
```

## المثال الأول

### إرسال رسالة SMS

```sad
# ملف: example.ص

استورد "android/communication/sms"

دالة رئيسية()
    # تهيئة الجسر
    SadSMSBridge.هيئ(السياق)
    
    # إرسال رسالة
    متغير نتيجة = SadSMSBridge.أرسل(
        "+966501234567"،
        "مرحباً من لغة ص!"
    )
    
    إذا (نتيجة.نجح)
        اطبع_سطر("تم إرسال الرسالة")
    وإلا
        اطبع_سطر("فشل: " + نتيجة.الخطأ)
    نهاية
نهاية
```

### إنشاء ويدجت بسيط

```sad
# ملف: my_widget.ص

استورد "android/widgets/core"
استورد "android/widgets/glance"

دالة رئيسية()
    SadGlanceBridge.أنشئ_ويدجت(
        الاسم: "my_first_widget"،
        العرض: SMALL،
        المحتوى: لامدا()
            عمود(
                المحاذاة: CENTER
            )
                نص("مرحباً!")
                نص(احصل_على_الوقت())
            نهاية_عمود
        نهاية
    )
نهاية

دالة احصل_على_الوقت()
    ارجع التاريخ.الآن().تنسيق("hh:mm a")
نهاية
```

### تطبيق تلفزيون بسيط

```sad
# ملف: tv_app.ص

استورد "android/tv/core"
استورد "android/tv/browse"
استورد "android/tv/navigation"

دالة رئيسية()
    # تهيئة التطبيق
    SadTVCoreBridge.هيئ(السياق)
    
    # إنشاء صفوف المحتوى
    متغير الصفوف = [
        صف(
            العنوان: "الأفلام الشائعة"،
            العناصر: احصل_على_الأفلام()
        )،
        صف(
            العنوان: "المسلسلات"،
            العناصر: احصل_على_المسلسلات()
        )
    ]
    
    # عرض الواجهة
    SadTVBrowseBridge.عرض(الصفوف)
    
    # الاستماع للتنقل
    SadTVNavigationBridge.عند_الاختيار(لامدا(عنصر)
        اطبع_سطر("تم اختيار: " + عنصر.العنوان)
        SadTVDetailsBridge.عرض(عنصر)
    نهاية)
نهاية

دالة احصل_على_الأفلام()
    ارجع [
        عنصر(المعرف: "1"، العنوان: "فيلم 1"، الصورة: "url1")،
        عنصر(المعرف: "2"، العنوان: "فيلم 2"، الصورة: "url2")
    ]
نهاية
```

## الخطوات التالية

1. راجع [التوثيق الكامل](ANDROID_BRIDGES_DOCUMENTATION.md)
2. استعرض [التطبيقات التجريبية](../demo/)
3. اطلع على [أمثلة إضافية](EXAMPLES.md)

---

© 2024 فريق لغة ص
