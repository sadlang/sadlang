package sad.android.demo.chat.ui

/**
 * واجهة شاشة المحادثة - Chat Screen UI
 * 
 * @author فريق لغة ص
 */
object SadChatMessagesUI {
    
    /**
     * تكوين شاشة المحادثة / Chat screen config
     */
    data class ChatScreenConfig(
        val showHeader: Boolean = true,
        val showTypingIndicator: Boolean = true,
        val showReadReceipts: Boolean = true,
        val showTimestamps: Boolean = true,
        val groupMessagesByDate: Boolean = true,
        val showScrollToBottom: Boolean = true,
        val enableReactions: Boolean = true,
        val enableReply: Boolean = true,
        val enableForward: Boolean = true,
        val enableCopy: Boolean = true,
        val enableDelete: Boolean = true,
        val maxAttachmentSize: Long = 25 * 1024 * 1024, // 25MB
        val inputPlaceholder: String = "اكتب رسالة..."
    )
    
    /**
     * عنصر الرسالة / Message item
     */
    data class MessageItem(
        val id: String,
        val content: String,
        val isOutgoing: Boolean,
        val senderName: String,
        val senderAvatar: String,
        val timestamp: String,
        val status: MessageStatusUI,
        val type: MessageTypeUI,
        val attachments: List<AttachmentUI>,
        val reactions: List<ReactionUI>,
        val replyTo: ReplyPreview?,
        val isFirstInGroup: Boolean,
        val isLastInGroup: Boolean
    )
    
    /**
     * حالة الرسالة UI
     */
    enum class MessageStatusUI {
        SENDING,
        SENT,
        DELIVERED,
        READ,
        FAILED
    }
    
    /**
     * نوع الرسالة UI
     */
    enum class MessageTypeUI {
        TEXT,
        IMAGE,
        VIDEO,
        AUDIO,
        DOCUMENT,
        LOCATION,
        CONTACT,
        STICKER,
        SYSTEM
    }
    
    /**
     * مرفق UI
     */
    data class AttachmentUI(
        val id: String,
        val type: MessageTypeUI,
        val url: String,
        val thumbnailUrl: String,
        val fileName: String,
        val fileSize: String,
        val duration: String = ""
    )
    
    /**
     * تفاعل UI
     */
    data class ReactionUI(
        val emoji: String,
        val count: Int,
        val isSelected: Boolean
    )
    
    /**
     * معاينة الرد / Reply preview
     */
    data class ReplyPreview(
        val messageId: String,
        val senderName: String,
        val content: String,
        val type: MessageTypeUI
    )
    
    /**
     * خيارات المرفقات / Attachment options
     */
    enum class AttachmentOption {
        CAMERA,
        GALLERY,
        VIDEO,
        AUDIO,
        DOCUMENT,
        LOCATION,
        CONTACT
    }
    
    /**
     * مستمع واجهة الرسائل / Messages UI listener
     */
    interface MessagesUIListener {
        fun onSendMessage(text: String)
        fun onSendAttachment(option: AttachmentOption)
        fun onMessageClicked(messageId: String)
        fun onMessageLongPressed(messageId: String)
        fun onReactionSelected(messageId: String, emoji: String)
        fun onReplyClicked(messageId: String)
        fun onAttachmentClicked(attachmentId: String)
        fun onScrolledToTop()
        fun onTypingStateChanged(isTyping: Boolean)
    }
    
    private var uiListener: MessagesUIListener? = null
    private var config = ChatScreenConfig()
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: ChatScreenConfig) {
        config = newConfig
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setUIListener(listener: MessagesUIListener?) {
        uiListener = listener
    }
    
    /**
     * تحويل الرسالة لعنصر UI / Convert message to UI item
     */
    fun toMessageItem(
        message: sad.android.demo.chat.SadChatApp.Message,
        currentUserId: String,
        senderName: String,
        senderAvatar: String,
        isFirstInGroup: Boolean = true,
        isLastInGroup: Boolean = true
    ): MessageItem {
        return MessageItem(
            id = message.id,
            content = message.content,
            isOutgoing = message.senderId == currentUserId,
            senderName = senderName,
            senderAvatar = senderAvatar,
            timestamp = formatTime(message.timestamp),
            status = mapStatus(message.status),
            type = mapType(message.type),
            attachments = message.attachments.map { toAttachmentUI(it) },
            reactions = groupReactions(message.reactions, currentUserId),
            replyTo = null, // TODO: تحميل الرسالة المرد عليها
            isFirstInGroup = isFirstInGroup,
            isLastInGroup = isLastInGroup
        )
    }
    
    private fun mapStatus(status: sad.android.demo.chat.SadChatApp.MessageStatus): MessageStatusUI {
        return when (status) {
            sad.android.demo.chat.SadChatApp.MessageStatus.SENDING -> MessageStatusUI.SENDING
            sad.android.demo.chat.SadChatApp.MessageStatus.SENT -> MessageStatusUI.SENT
            sad.android.demo.chat.SadChatApp.MessageStatus.DELIVERED -> MessageStatusUI.DELIVERED
            sad.android.demo.chat.SadChatApp.MessageStatus.READ -> MessageStatusUI.READ
            sad.android.demo.chat.SadChatApp.MessageStatus.FAILED -> MessageStatusUI.FAILED
        }
    }
    
    private fun mapType(type: sad.android.demo.chat.SadChatApp.MessageType): MessageTypeUI {
        return when (type) {
            sad.android.demo.chat.SadChatApp.MessageType.TEXT -> MessageTypeUI.TEXT
            sad.android.demo.chat.SadChatApp.MessageType.IMAGE -> MessageTypeUI.IMAGE
            sad.android.demo.chat.SadChatApp.MessageType.VIDEO -> MessageTypeUI.VIDEO
            sad.android.demo.chat.SadChatApp.MessageType.AUDIO -> MessageTypeUI.AUDIO
            sad.android.demo.chat.SadChatApp.MessageType.DOCUMENT -> MessageTypeUI.DOCUMENT
            sad.android.demo.chat.SadChatApp.MessageType.LOCATION -> MessageTypeUI.LOCATION
            sad.android.demo.chat.SadChatApp.MessageType.CONTACT -> MessageTypeUI.CONTACT
            sad.android.demo.chat.SadChatApp.MessageType.STICKER -> MessageTypeUI.STICKER
        }
    }
    
    private fun toAttachmentUI(attachment: sad.android.demo.chat.SadChatApp.Attachment): AttachmentUI {
        return AttachmentUI(
            id = attachment.id,
            type = mapType(attachment.type),
            url = attachment.url,
            thumbnailUrl = attachment.thumbnailUrl,
            fileName = attachment.fileName,
            fileSize = formatFileSize(attachment.fileSize)
        )
    }
    
    private fun groupReactions(
        reactions: List<sad.android.demo.chat.SadChatApp.Reaction>,
        currentUserId: String
    ): List<ReactionUI> {
        return reactions.groupBy { it.emoji }
            .map { (emoji, list) ->
                ReactionUI(
                    emoji = emoji,
                    count = list.size,
                    isSelected = list.any { it.userId == currentUserId }
                )
            }
    }
    
    private fun formatTime(timestamp: Long): String {
        val date = java.util.Date(timestamp)
        val format = java.text.SimpleDateFormat("h:mm a", java.util.Locale("ar"))
        return format.format(date)
    }
    
    private fun formatFileSize(bytes: Long): String {
        return when {
            bytes < 1024 -> "$bytes B"
            bytes < 1024 * 1024 -> "${bytes / 1024} KB"
            else -> "${"%.1f".format(bytes / (1024.0 * 1024))} MB"
        }
    }
    
    /**
     * الحصول على خيارات المرفقات / Get attachment options
     */
    fun getAttachmentOptions(): List<Pair<AttachmentOption, String>> {
        return listOf(
            AttachmentOption.CAMERA to "الكاميرا",
            AttachmentOption.GALLERY to "الصور",
            AttachmentOption.VIDEO to "فيديو",
            AttachmentOption.AUDIO to "صوت",
            AttachmentOption.DOCUMENT to "ملف",
            AttachmentOption.LOCATION to "الموقع",
            AttachmentOption.CONTACT to "جهة اتصال"
        )
    }
    
    /**
     * الحصول على التفاعلات الافتراضية / Get default reactions
     */
    fun getDefaultReactions(): List<String> {
        return listOf("❤️", "👍", "😂", "😮", "😢", "👏")
    }
    
    /**
     * الحصول على أيقونة الحالة / Get status icon
     */
    fun getStatusIcon(status: MessageStatusUI): String {
        return when (status) {
            MessageStatusUI.SENDING -> "⏳"
            MessageStatusUI.SENT -> "✓"
            MessageStatusUI.DELIVERED -> "✓✓"
            MessageStatusUI.READ -> "✓✓" // أزرق
            MessageStatusUI.FAILED -> "⚠️"
        }
    }
}
