# تطبيق مهامي - قائمة مهام بلغة ص

تطبيق أندرويد كامل يوضح استخدام لغة ص مع جسور Android.

## المميزات

- ✅ إضافة وإكمال وحذف المهام
- ✅ حفظ البيانات محلياً (SharedPreferences)
- ✅ إشعارات تذكير (WorkManager)
- ✅ دعم كامل للعربية (RTL)
- ✅ تصميم Material 3
- ✅ وضع داكن/فاتح
- ✅ اهتزاز تفاعلي (Haptic)

## البنية

```
مهامي/
├── رئيسي.ص                    # كود لغة ص الأصلي
├── build.gradle               # إعدادات Gradle
├── src/main/
│   ├── AndroidManifest.xml
│   ├── kotlin/sad/examples/tasks/
│   │   ├── MainActivity.kt         # النشاط الرئيسي (Compose)
│   │   ├── SadTasksApplication.kt  # تهيئة التطبيق
│   │   ├── model/Task.kt           # نموذج المهمة
│   │   ├── viewmodel/TasksViewModel.kt  # منطق الأعمال
│   │   ├── workers/ReminderWorker.kt    # خدمة الإشعارات
│   │   └── ui/theme/Theme.kt       # السمة
│   └── res/
│       └── values/
│           ├── strings.xml    # النصوص العربية
│           ├── colors.xml     # الألوان
│           └── themes.xml     # السمات
```

## التشغيل

```bash
# بناء التطبيق
./gradlew :examples:android:مهامي:assembleDebug

# تثبيت على الجهاز
./gradlew :examples:android:مهامي:installDebug
```

## جسور لغة ص المستخدمة

| الجسر | الاستخدام |
|-------|----------|
| `android/storage/تفضيلات` | حفظ المهام محلياً |
| `android/notifications/إشعارات` | إرسال تذكيرات |
| `android/haptic/لمس` | اهتزاز تفاعلي |
| `android/date/تاريخ` | تنسيق التواريخ |
| `android/dialog/حوار` | نوافذ التأكيد |
| `android/toast/رسائل` | رسائل Toast |
| `android/theme/سمة` | الوضع الداكن/الفاتح |

## المقارنة: لغة ص مقابل Kotlin

### إضافة مهمة بلغة ص:
```sad
دالة أضف_مهمة(عنوان: نص، أولوية: رقم = 2) {
    متغير م = مهمة {
        معرف: uuid()،
        عنوان: عنوان،
        مكتملة: خطأ،
        تاريخ_الإنشاء: الآن()،
        أولوية: أولوية
    }
    المهام.أضف(م)
    رسائل.قصيرة("تمت إضافة المهمة ✓")
    لمس.نقرة()
نهاية
```

### نفس الكود بـ Kotlin:
```kotlin
fun addTask(title: String, priority: Int = 2) {
    val task = Task(
        id = UUID.randomUUID().toString(),
        title = title,
        completed = false,
        createdAt = System.currentTimeMillis(),
        priority = priority
    )
    _tasks.value = _tasks.value + task
    Toast.makeText(context, "تمت إضافة المهمة ✓", Toast.LENGTH_SHORT).show()
    // haptic feedback
}
```

## الترخيص

MIT License - فريق لغة ص
