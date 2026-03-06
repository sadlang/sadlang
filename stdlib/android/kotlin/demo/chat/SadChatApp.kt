package sad.android.demo.chat

import android.content.Context
import sad.android.communication.sms.SadSMSBridge
import sad.android.communication.messaging.SadMessagingBridge
import sad.android.communication.notifications.SadNotificationBridge
import sad.android.communication.contacts.SadContactsBridge

/**
 * تطبيق الدردشة التجريبي - يستخدم جسور الاتصالات
 * Demo Chat Application - Uses communication bridges
 * 
 * يوضح كيفية استخدام جسور لغة ص لبناء تطبيق دردشة كامل
 *
 * @author فريق لغة ص
 */
object SadChatApp {
    
    private var context: Context? = null
    private var isInitialized = false
    
    // ═══════════════════════════════════════════════════════════════
    //                         نماذج البيانات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * محادثة / Conversation
     */
    data class Conversation(
        val id: String,
        val title: String,
        val participants: List<Participant>,
        val lastMessage: Message? = null,
        val unreadCount: Int = 0,
        val isPinned: Boolean = false,
        val isMuted: Boolean = false,
        val avatarUrl: String = "",
        val isGroup: Boolean = false,
        val createdAt: Long = System.currentTimeMillis()
    )
    
    /**
     * مشارك / Participant
     */
    data class Participant(
        val id: String,
        val name: String,
        val phoneNumber: String = "",
        val avatarUrl: String = "",
        val isOnline: Boolean = false,
        val lastSeen: Long = 0
    )
    
    /**
     * رسالة / Message
     */
    data class Message(
        val id: String,
        val conversationId: String,
        val senderId: String,
        val content: String,
        val type: MessageType = MessageType.TEXT,
        val status: MessageStatus = MessageStatus.SENT,
        val timestamp: Long = System.currentTimeMillis(),
        val replyToId: String? = null,
        val attachments: List<Attachment> = emptyList(),
        val reactions: List<Reaction> = emptyList()
    )
    
    /**
     * نوع الرسالة / Message type
     */
    enum class MessageType {
        TEXT,
        IMAGE,
        VIDEO,
        AUDIO,
        DOCUMENT,
        LOCATION,
        CONTACT,
        STICKER
    }
    
    /**
     * حالة الرسالة / Message status
     */
    enum class MessageStatus {
        SENDING,
        SENT,
        DELIVERED,
        READ,
        FAILED
    }
    
    /**
     * مرفق / Attachment
     */
    data class Attachment(
        val id: String,
        val type: MessageType,
        val url: String,
        val thumbnailUrl: String = "",
        val fileName: String = "",
        val fileSize: Long = 0,
        val mimeType: String = ""
    )
    
    /**
     * تفاعل / Reaction
     */
    data class Reaction(
        val emoji: String,
        val userId: String,
        val timestamp: Long = System.currentTimeMillis()
    )
    
    // ═══════════════════════════════════════════════════════════════
    //                         المستمعات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * مستمع التطبيق / App listener
     */
    interface ChatAppListener {
        fun onConversationUpdated(conversation: Conversation)
        fun onNewMessage(message: Message)
        fun onMessageStatusChanged(messageId: String, status: MessageStatus)
        fun onTypingStateChanged(conversationId: String, userId: String, isTyping: Boolean)
        fun onUserStatusChanged(userId: String, isOnline: Boolean)
    }
    
    private var appListener: ChatAppListener? = null
    
    // البيانات المحلية
    private val conversations = mutableMapOf<String, Conversation>()
    private val messages = mutableMapOf<String, MutableList<Message>>()
    private val currentUserId = "current_user"
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة التطبيق / Initialize app
     * 
     * ```sad
     * # مثال الاستخدام في لغة ص
     * استورد "android/demo/chat"
     * 
     * دالة رئيسية()
     *     تطبيق_الدردشة.هيئ(السياق)
     *     اطبع_سطر("تم تهيئة تطبيق الدردشة")
     * نهاية
     * ```
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        
        // تهيئة الجسور المطلوبة
        // SadSMSBridge.initialize(ctx)
        // SadMessagingBridge.initialize(ctx)
        // SadNotificationBridge.initialize(ctx)
        // SadContactsBridge.initialize(ctx)
        
        setupSampleData()
        isInitialized = true
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setAppListener(listener: ChatAppListener?) {
        appListener = listener
    }
    
    private fun setupSampleData() {
        // إنشاء بيانات تجريبية
        val participant1 = Participant(
            id = "user1",
            name = "أحمد محمد",
            phoneNumber = "+966501234567",
            isOnline = true
        )
        
        val participant2 = Participant(
            id = "user2",
            name = "سارة علي",
            phoneNumber = "+966507654321",
            isOnline = false,
            lastSeen = System.currentTimeMillis() - 3600000
        )
        
        val conversation1 = Conversation(
            id = "conv1",
            title = "أحمد محمد",
            participants = listOf(participant1),
            lastMessage = Message(
                id = "msg1",
                conversationId = "conv1",
                senderId = "user1",
                content = "مرحباً! كيف حالك؟",
                status = MessageStatus.READ
            ),
            unreadCount = 0
        )
        
        val conversation2 = Conversation(
            id = "conv2",
            title = "مجموعة العمل",
            participants = listOf(participant1, participant2),
            isGroup = true,
            lastMessage = Message(
                id = "msg2",
                conversationId = "conv2",
                senderId = "user2",
                content = "اجتماع الساعة 3",
                status = MessageStatus.DELIVERED
            ),
            unreadCount = 3
        )
        
        conversations["conv1"] = conversation1
        conversations["conv2"] = conversation2
        
        messages["conv1"] = mutableListOf(conversation1.lastMessage!!)
        messages["conv2"] = mutableListOf(conversation2.lastMessage!!)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المحادثات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على جميع المحادثات / Get all conversations
     * 
     * ```sad
     * متغير المحادثات = تطبيق_الدردشة.احصل_على_المحادثات()
     * لكل محادثة في المحادثات
     *     اطبع_سطر(محادثة.العنوان)
     * نهاية
     * ```
     */
    fun getConversations(): List<Conversation> {
        return conversations.values
            .sortedByDescending { it.lastMessage?.timestamp ?: it.createdAt }
            .sortedByDescending { it.isPinned }
    }
    
    /**
     * الحصول على محادثة / Get conversation
     */
    fun getConversation(id: String): Conversation? = conversations[id]
    
    /**
     * إنشاء محادثة جديدة / Create new conversation
     * 
     * ```sad
     * متغير المشاركون = [
     *     مشارك(المعرف: "user1"، الاسم: "أحمد")
     * ]
     * متغير محادثة_جديدة = تطبيق_الدردشة.أنشئ_محادثة(المشاركون)
     * ```
     */
    fun createConversation(participants: List<Participant>, title: String = ""): Conversation {
        val isGroup = participants.size > 1
        val id = "conv_${System.currentTimeMillis()}"
        
        val conversation = Conversation(
            id = id,
            title = title.ifEmpty { participants.firstOrNull()?.name ?: "محادثة جديدة" },
            participants = participants,
            isGroup = isGroup
        )
        
        conversations[id] = conversation
        messages[id] = mutableListOf()
        
        appListener?.onConversationUpdated(conversation)
        return conversation
    }
    
    /**
     * حذف محادثة / Delete conversation
     */
    fun deleteConversation(id: String): Boolean {
        conversations.remove(id) ?: return false
        messages.remove(id)
        return true
    }
    
    /**
     * تثبيت/إلغاء تثبيت محادثة / Pin/unpin conversation
     */
    fun togglePinConversation(id: String): Boolean {
        val conversation = conversations[id] ?: return false
        conversations[id] = conversation.copy(isPinned = !conversation.isPinned)
        appListener?.onConversationUpdated(conversations[id]!!)
        return true
    }
    
    /**
     * كتم/إلغاء كتم محادثة / Mute/unmute conversation
     */
    fun toggleMuteConversation(id: String): Boolean {
        val conversation = conversations[id] ?: return false
        conversations[id] = conversation.copy(isMuted = !conversation.isMuted)
        appListener?.onConversationUpdated(conversations[id]!!)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الرسائل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على رسائل محادثة / Get conversation messages
     * 
     * ```sad
     * متغير الرسائل = تطبيق_الدردشة.احصل_على_الرسائل("conv1")
     * لكل رسالة في الرسائل
     *     اطبع_سطر(رسالة.المحتوى)
     * نهاية
     * ```
     */
    fun getMessages(conversationId: String, limit: Int = 50): List<Message> {
        return messages[conversationId]
            ?.sortedByDescending { it.timestamp }
            ?.take(limit)
            ?: emptyList()
    }
    
    /**
     * إرسال رسالة نصية / Send text message
     * 
     * ```sad
     * متغير رسالة = تطبيق_الدردشة.أرسل_رسالة("conv1"، "مرحباً!")
     * إذا (رسالة != لاشيء)
     *     اطبع_سطر("تم إرسال الرسالة")
     * نهاية
     * ```
     */
    fun sendMessage(conversationId: String, content: String, replyToId: String? = null): Message? {
        val conversation = conversations[conversationId] ?: return null
        
        val message = Message(
            id = "msg_${System.currentTimeMillis()}",
            conversationId = conversationId,
            senderId = currentUserId,
            content = content,
            type = MessageType.TEXT,
            status = MessageStatus.SENDING,
            replyToId = replyToId
        )
        
        messages.getOrPut(conversationId) { mutableListOf() }.add(message)
        
        // تحديث المحادثة
        conversations[conversationId] = conversation.copy(lastMessage = message)
        
        // محاكاة الإرسال
        simulateSendMessage(message)
        
        appListener?.onNewMessage(message)
        return message
    }
    
    /**
     * إرسال رسالة وسائط / Send media message
     */
    fun sendMediaMessage(
        conversationId: String,
        type: MessageType,
        attachmentUrl: String,
        caption: String = ""
    ): Message? {
        val conversation = conversations[conversationId] ?: return null
        
        val attachment = Attachment(
            id = "att_${System.currentTimeMillis()}",
            type = type,
            url = attachmentUrl
        )
        
        val message = Message(
            id = "msg_${System.currentTimeMillis()}",
            conversationId = conversationId,
            senderId = currentUserId,
            content = caption,
            type = type,
            status = MessageStatus.SENDING,
            attachments = listOf(attachment)
        )
        
        messages.getOrPut(conversationId) { mutableListOf() }.add(message)
        conversations[conversationId] = conversation.copy(lastMessage = message)
        
        simulateSendMessage(message)
        appListener?.onNewMessage(message)
        
        return message
    }
    
    private fun simulateSendMessage(message: Message) {
        android.os.Handler(android.os.Looper.getMainLooper()).postDelayed({
            updateMessageStatus(message.id, MessageStatus.SENT)
        }, 500)
        
        android.os.Handler(android.os.Looper.getMainLooper()).postDelayed({
            updateMessageStatus(message.id, MessageStatus.DELIVERED)
        }, 1500)
    }
    
    /**
     * تحديث حالة الرسالة / Update message status
     */
    fun updateMessageStatus(messageId: String, status: MessageStatus) {
        for ((convId, msgList) in messages) {
            val index = msgList.indexOfFirst { it.id == messageId }
            if (index >= 0) {
                msgList[index] = msgList[index].copy(status = status)
                appListener?.onMessageStatusChanged(messageId, status)
                return
            }
        }
    }
    
    /**
     * حذف رسالة / Delete message
     */
    fun deleteMessage(conversationId: String, messageId: String): Boolean {
        val msgList = messages[conversationId] ?: return false
        return msgList.removeIf { it.id == messageId }
    }
    
    /**
     * إضافة تفاعل / Add reaction
     */
    fun addReaction(conversationId: String, messageId: String, emoji: String): Boolean {
        val msgList = messages[conversationId] ?: return false
        val index = msgList.indexOfFirst { it.id == messageId }
        if (index < 0) return false
        
        val reaction = Reaction(emoji, currentUserId)
        val message = msgList[index]
        val newReactions = message.reactions.toMutableList().apply {
            removeIf { it.userId == currentUserId }
            add(reaction)
        }
        
        msgList[index] = message.copy(reactions = newReactions)
        return true
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         البحث
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * البحث في الرسائل / Search messages
     * 
     * ```sad
     * متغير النتائج = تطبيق_الدردشة.ابحث("مرحبا")
     * اطبع_سطر("وجدت " + النتائج.الحجم + " رسالة")
     * ```
     */
    fun searchMessages(query: String): List<Message> {
        if (query.isBlank()) return emptyList()
        
        val results = mutableListOf<Message>()
        for ((_, msgList) in messages) {
            results.addAll(msgList.filter {
                it.content.contains(query, ignoreCase = true)
            })
        }
        
        return results.sortedByDescending { it.timestamp }
    }
    
    /**
     * البحث في المحادثات / Search conversations
     */
    fun searchConversations(query: String): List<Conversation> {
        if (query.isBlank()) return getConversations()
        
        return conversations.values.filter {
            it.title.contains(query, ignoreCase = true) ||
            it.participants.any { p -> p.name.contains(query, ignoreCase = true) }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الكتابة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال حالة الكتابة / Send typing state
     */
    fun sendTypingState(conversationId: String, isTyping: Boolean) {
        // في التطبيق الحقيقي: إرسال للخادم
        appListener?.onTypingStateChanged(conversationId, currentUserId, isTyping)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإحصائيات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * عدد الرسائل غير المقروءة / Total unread count
     */
    fun getTotalUnreadCount(): Int {
        return conversations.values.sumOf { it.unreadCount }
    }
    
    /**
     * تعليم الكل كمقروء / Mark all as read
     */
    fun markAllAsRead(conversationId: String) {
        val conversation = conversations[conversationId] ?: return
        conversations[conversationId] = conversation.copy(unreadCount = 0)
        appListener?.onConversationUpdated(conversations[conversationId]!!)
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        conversations.clear()
        messages.clear()
        appListener = null
        context = null
        isInitialized = false
    }
}
