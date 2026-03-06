# مرجع API - جسور Android
# API Reference - Android Bridges

## الفهرس

1. [جسور الاتصالات](#1-جسور-الاتصالات)
2. [جسور الويدجت](#2-جسور-الويدجت)
3. [جسور التلفزيون](#3-جسور-التلفزيون)
4. [الأنواع المشتركة](#4-الأنواع-المشتركة)

---

## 1. جسور الاتصالات

### SadSMSBridge

```kotlin
object SadSMSBridge {
    // التهيئة
    fun initialize(context: Context)
    
    // إرسال SMS
    fun send(phoneNumber: String, message: String): SendResult
    fun sendMultiple(phoneNumbers: List<String>, message: String): List<SendResult>
    
    // قراءة الرسائل
    fun readMessages(limit: Int = 50): List<SMSMessage>
    fun readConversation(phoneNumber: String): List<SMSMessage>
    fun searchMessages(query: String): List<SMSMessage>
    
    // المستمعات
    fun setOnMessageReceived(callback: (SMSMessage) -> Unit)
    fun setOnDeliveryReport(callback: (String, Boolean) -> Unit)
}

data class SMSMessage(
    val id: String,
    val address: String,
    val body: String,
    val timestamp: Long,
    val isRead: Boolean,
    val type: SMSType
)

enum class SMSType { INBOX, SENT, DRAFT }
```

### SadContactsBridge

```kotlin
object SadContactsBridge {
    fun initialize(context: Context)
    
    // القراءة
    fun getAll(): List<Contact>
    fun getById(id: String): Contact?
    fun search(query: String): List<Contact>
    fun getByPhoneNumber(phone: String): Contact?
    
    // الكتابة
    fun add(contact: Contact): String
    fun update(contact: Contact): Boolean
    fun delete(id: String): Boolean
    
    // المجموعات
    fun getGroups(): List<ContactGroup>
    fun getContactsInGroup(groupId: String): List<Contact>
}

data class Contact(
    val id: String = "",
    val displayName: String,
    val phoneNumbers: List<PhoneNumber> = emptyList(),
    val emails: List<Email> = emptyList(),
    val photoUri: String? = null
)
```

### SadNotificationBridge

```kotlin
object SadNotificationBridge {
    fun initialize(context: Context)
    
    // إنشاء القنوات
    fun createChannel(channel: NotificationChannel)
    
    // إرسال الإشعارات
    fun send(notification: NotificationConfig): Int
    fun sendWithActions(notification: NotificationConfig, actions: List<NotificationAction>): Int
    fun sendProgress(notification: NotificationConfig, progress: Int, max: Int): Int
    
    // الإدارة
    fun cancel(notificationId: Int)
    fun cancelAll()
    fun update(notificationId: Int, notification: NotificationConfig)
}

data class NotificationConfig(
    val title: String,
    val content: String,
    val channelId: String,
    val priority: NotificationPriority = NotificationPriority.DEFAULT,
    val smallIcon: Int = 0,
    val largeIcon: String? = null,
    val autoCancel: Boolean = true
)
```

---

## 2. جسور الويدجت

### SadWidgetCoreBridge

```kotlin
object SadWidgetCoreBridge {
    fun initialize(context: Context)
    
    // التحديث
    fun update(widgetId: Int, data: Map<String, Any>)
    fun updateAll(className: String, data: Map<String, Any>)
    
    // الإجراءات
    fun setOnAction(widgetId: Int, actionId: String, callback: () -> Unit)
    
    // دورة الحياة
    fun onEnabled()
    fun onDisabled()
    fun onDeleted(widgetIds: IntArray)
}
```

### SadGlanceBridge

```kotlin
object SadGlanceBridge {
    fun createWidget(config: GlanceWidgetConfig)
    fun updateWidget(name: String)
    
    // المكونات
    fun text(content: String, style: TextStyle? = null): GlanceComponent
    fun button(text: String, action: () -> Unit): GlanceComponent
    fun image(url: String, contentDescription: String): GlanceComponent
    fun column(children: List<GlanceComponent>): GlanceComponent
    fun row(children: List<GlanceComponent>): GlanceComponent
}

data class GlanceWidgetConfig(
    val name: String,
    val sizes: List<WidgetSize>,
    val refreshIntervalMinutes: Int = 30,
    val supportsConfiguration: Boolean = false
)
```

---

## 3. جسور التلفزيون

### SadTVCoreBridge

```kotlin
object SadTVCoreBridge {
    fun initialize(context: Context, config: TVAppConfig = TVAppConfig())
    
    // حالة التطبيق
    fun getAppState(): TVAppState
    fun setAppState(state: TVAppState)
    
    // التركيز
    fun getCurrentFocus(): FocusPosition?
    fun requestFocus(viewId: String)
    
    // المفاتيح
    fun handleKeyEvent(keyCode: Int): Boolean
    fun registerKeyHandler(keyCode: Int, handler: () -> Boolean)
}

data class TVAppConfig(
    val appName: String = "",
    val locale: String = "ar",
    val layoutDirection: LayoutDirection = LayoutDirection.RTL,
    val focusHighlightColor: Int = 0xFF1E88E5.toInt()
)
```

### SadTVBrowseBridge

```kotlin
object SadTVBrowseBridge {
    fun display(rows: List<BrowseRow>, config: BrowseConfig = BrowseConfig())
    fun updateRow(rowId: String, items: List<BrowseItem>)
    fun addRow(row: BrowseRow)
    fun removeRow(rowId: String)
    
    // الأحداث
    fun setOnItemSelected(callback: (BrowseItem) -> Unit)
    fun setOnItemClicked(callback: (BrowseItem) -> Unit)
}

data class BrowseRow(
    val id: String,
    val title: String,
    val items: List<BrowseItem>,
    val cardType: CardType = CardType.IMAGE_CARD
)

data class BrowseItem(
    val id: String,
    val title: String,
    val description: String = "",
    val imageUrl: String,
    val badgeText: String? = null
)
```

### SadTVVideoPlayerBridge

```kotlin
object SadTVVideoPlayerBridge {
    fun play(source: VideoSource, config: PlayerConfig = PlayerConfig())
    fun pause()
    fun resume()
    fun stop()
    fun seekTo(positionMs: Long)
    
    // الجودة
    fun getAvailableQualities(): List<VideoQuality>
    fun setQuality(quality: VideoQuality)
    
    // الترجمات
    fun getSubtitleTracks(): List<SubtitleTrack>
    fun setSubtitleTrack(track: SubtitleTrack?)
    
    // الصوت
    fun getAudioTracks(): List<AudioTrack>
    fun setAudioTrack(track: AudioTrack)
    
    // الأحداث
    fun setOnPlaybackStateChanged(callback: (PlaybackState) -> Unit)
    fun setOnProgress(callback: (Long) -> Unit)
    fun setOnError(callback: (PlayerError) -> Unit)
}

data class VideoSource(
    val url: String,
    val type: ContentType = ContentType.HLS,
    val drmLicense: String? = null,
    val drmType: DRMType? = null
)
```

### SadTVPiPBridge

```kotlin
object SadTVPiPBridge {
    fun enter(config: PiPConfig = PiPConfig())
    fun exit()
    fun isInPiPMode(): Boolean
    fun isSupported(): Boolean
    
    // الإجراءات
    fun setActions(actions: List<PiPAction>)
    fun updateActions(actions: List<PiPAction>)
    
    // الأحداث
    fun setOnPiPStateChanged(callback: (Boolean) -> Unit)
    fun setOnActionClicked(callback: (String) -> Unit)
}

data class PiPConfig(
    val aspectRatioWidth: Int = 16,
    val aspectRatioHeight: Int = 9,
    val actions: List<PiPAction> = emptyList(),
    val autoEnterEnabled: Boolean = true
)
```

---

## 4. الأنواع المشتركة

### نتيجة العملية

```kotlin
sealed class Result<T> {
    data class Success<T>(val data: T) : Result<T>()
    data class Error<T>(val message: String, val code: Int = -1) : Result<T>()
}
```

### الأخطاء

```kotlin
sealed class SadError : Exception() {
    data class NetworkError(override val message: String) : SadError()
    data class PermissionError(val permission: String) : SadError()
    data class NotFoundError(val resource: String) : SadError()
    data class InvalidInputError(val field: String) : SadError()
}
```

### الصلاحيات

```kotlin
enum class Permission {
    SMS_SEND,
    SMS_READ,
    CONTACTS_READ,
    CONTACTS_WRITE,
    CALL_PHONE,
    READ_CALL_LOG,
    CAMERA,
    RECORD_AUDIO,
    LOCATION,
    STORAGE_READ,
    STORAGE_WRITE,
    NOTIFICATIONS
}
```

---

## ملاحظات الاستخدام

### 1. التهيئة

جميع الجسور تتطلب استدعاء `initialize(context)` قبل الاستخدام.

### 2. الصلاحيات

تحقق من الصلاحيات قبل استخدام الجسور التي تتطلبها.

### 3. دورة الحياة

أفرج عن الموارد باستدعاء `release()` عند الانتهاء.

### 4. الخيوط

معظم العمليات آمنة للاستخدام من الخيط الرئيسي. العمليات الطويلة تعمل تلقائياً في خيط خلفي.

---

© 2024 فريق لغة ص
