# نظام واجهات المستخدم في لغة ص
# SadUI System Documentation

<div dir="rtl">

## نظرة عامة

نظام SadUI هو نظام متكامل لبناء واجهات المستخدم بلغة ص، يدعم توليد كود أصلي لمنصات متعددة:

| المنصة | التقنية | الحالة |
|--------|---------|--------|
| Android | Jetpack Compose | ✅ مكتمل |
| iOS | SwiftUI | ⚠️ قيد التطوير |
| Web | HTML/CSS/JS | ⚠️ قيد التطوير |
| Desktop | SDL2 | ⚠️ قيد التطوير |

---

## البدء السريع

### توليد تطبيق Android

```bash
# الأمر الأساسي
sadc ui generate android -o my_app/

# مع الخيارات
sadc ui generate android -o my_app/ --name "تطبيقي" --package com.example.myapp

# بالعربية
sadc واجهة توليد أندرويد -o تطبيقي/
```

### الملفات المُولَدة

```
my_app/
├── app/
│   ├── build.gradle.kts
│   └── src/main/
│       ├── AndroidManifest.xml
│       ├── java/com/example/myapp/
│       │   ├── MainActivity.kt
│       │   └── ui/theme/Theme.kt
│       └── res/values/
│           ├── strings.xml      # النصوص العربية
│           ├── colors.xml
│           └── themes.xml
├── gradle/
│   └── libs.versions.toml
└── settings.gradle.kts
```

---

## البنية المعمارية

```
┌─────────────────────────────────────────────────────────────────┐
│                     كود ص / Sad Source                          │
│                 واجهة { نص("مرحبا") }                           │
└────────────────────────┬────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│                    SadUI IR                                      │
│         (التمثيل الوسيط الموحد للواجهات)                         │
│                                                                  │
│  WidgetType::Text, WidgetType::Button, WidgetType::Column, etc. │
└────────────────────────┬────────────────────────────────────────┘
                         │
         ┌───────────────┼───────────────┐
         ▼               ▼               ▼
┌──────────────┐ ┌──────────────┐ ┌──────────────┐
│   Android    │ │     iOS      │ │     Web      │
│   Compose    │ │   SwiftUI    │ │  HTML/CSS    │
│   CodeGen    │ │   CodeGen    │ │   CodeGen    │
└──────────────┘ └──────────────┘ └──────────────┘
         │               │               │
         ▼               ▼               ▼
    MainActivity.kt  ContentView.swift  index.html
```

---

## أنواع العناصر (Widgets)

### العرض الأساسي
| العنصر | الوصف | Compose |
|--------|-------|---------|
| `Text` | نص عادي | `Text()` |
| `Image` | صورة | `Image()` |
| `Icon` | أيقونة | `Icon()` |

### الإدخال
| العنصر | الوصف | Compose |
|--------|-------|---------|
| `Button` | زر | `Button()` |
| `TextField` | حقل نص | `TextField()` |
| `Checkbox` | مربع اختيار | `Checkbox()` |
| `Toggle` | مفتاح | `Switch()` |
| `Slider` | منزلق | `Slider()` |

### التخطيط
| العنصر | الوصف | Compose |
|--------|-------|---------|
| `Column` | ترتيب رأسي | `Column()` |
| `Row` | ترتيب أفقي | `Row()` |
| `Stack` | تراكب | `Box()` |
| `Grid` | شبكة | `LazyVerticalGrid()` |
| `Spacer` | فاصل | `Spacer()` |

### الحاويات
| العنصر | الوصف | Compose |
|--------|-------|---------|
| `Card` | بطاقة | `Card()` |
| `ScrollView` | تمرير | `LazyColumn()` |
| `Scaffold` | هيكل | `Scaffold()` |
| `AppBar` | شريط علوي | `TopAppBar()` |

---

## مثال كامل

### كود ص (مستقبلي)

```sad
واجهة تطبيقي
    شاشة الرئيسية
        سقالة
            شريط_تطبيق عنوان="تطبيقي"
            نهاية
            
            محتوى
                عمود حشو=16 تباعد=8
                    نص "مرحباً بك" حجم=24 سمك=غامق
                    
                    بطاقة عرض=كامل
                        عمود حشو=16
                            نص "لغة ص" حجم=18
                            نص "لغة البرمجة العربية" لون=رمادي
                        نهاية
                    نهاية
                    
                    زر "اضغط هنا" عند_النقر=عداد_زيادة
                نهاية
            نهاية
        نهاية
    نهاية
نهاية
```

### الكود المُولَد (Kotlin)

```kotlin
@Composable
fun MainScreen() {
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text(stringResource(R.string.app_name)) }
            )
        }
    ) { padding ->
        Column(
            modifier = Modifier.padding(16.dp),
            verticalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            Text(
                text = stringResource(R.string.welcome),
                fontSize = 24.sp,
                fontWeight = FontWeight.Bold
            )
            
            Card(modifier = Modifier.fillMaxWidth()) {
                Column(modifier = Modifier.padding(16.dp)) {
                    Text("لغة ص", fontSize = 18.sp)
                    Text("لغة البرمجة العربية", color = Color.Gray)
                }
            }
            
            Button(onClick = { counter++ }) {
                Text(stringResource(R.string.click_here))
            }
        }
    }
}
```

---

## الخصائص المدعومة

### الأبعاد
```kotlin
Dimension.points(16)      // 16dp
Dimension.percent(50)     // 50%
Dimension.fill()          // fillMaxWidth/Height
Dimension.fit()          // wrapContentSize
```

### الألوان
```kotlin
Color.fromHex("#2196F3")  // أزرق
Color.red()               // أحمر
Color.transparent()       // شفاف
```

### المحاذاة
```kotlin
HAlignment.Leading        // بداية (RTL-aware)
HAlignment.Center         // وسط
HAlignment.Trailing       // نهاية
VAlignment.Top            // أعلى
VAlignment.Center         // وسط
VAlignment.Bottom         // أسفل
```

### الخطوط
```kotlin
FontWeight.Thin
FontWeight.Light
FontWeight.Regular
FontWeight.Medium
FontWeight.SemiBold
FontWeight.Bold
FontWeight.ExtraBold
FontWeight.Black
```

---

## دعم RTL

نظام SadUI يدعم RTL (من اليمين لليسار) أصلياً:

- `EdgeInsets` يستخدم `leading`/`trailing` بدل `left`/`right`
- `HAlignment.Leading` يعني اليمين في العربية
- `strings.xml` يحتوي على النصوص العربية
- `AndroidManifest.xml` يفعّل `supportsRtl="true"`

---

## التشغيل على Android Studio

1. **افتح المشروع:**
   ```
   File → Open → build_ui_test/
   ```

2. **انتظر مزامنة Gradle** (قد يستغرق بضع دقائق)

3. **شغّل التطبيق:**
   - اضغط `Shift+F10`
   - أو اضغط زر ▶️ الأخضر

4. **اختر الجهاز:**
   - محاكي (AVD)
   - أو جهاز متصل عبر USB

---

## استكشاف الأخطاء

### خطأ: R.string لم يُعثر عليه
- تأكد من وجود `strings.xml` في `res/values/`
- أعد مزامنة Gradle: `File → Sync Project`

### خطأ: SDK غير موجود
- افتح SDK Manager
- ثبّت Android SDK 34

### خطأ: Kotlin version
- تأكد من توافق إصدارات Kotlin في `libs.versions.toml`

---

## الخطوات القادمة

1. ✅ **Android Compose** - مكتمل
2. ⬜ **iOS SwiftUI** - قريباً
3. ⬜ **قراءة ملفات ص** - تحليل بناء الواجهة من كود ص
4. ⬜ **Hot Reload** - تحديث فوري
5. ⬜ **أدوات تصميم** - محرر مرئي

---

## المراجع

- [Jetpack Compose Documentation](https://developer.android.com/jetpack/compose)
- [Material Design 3](https://m3.material.io/)
- [Android RTL Support](https://developer.android.com/training/basics/supporting-devices/languages)

</div>
