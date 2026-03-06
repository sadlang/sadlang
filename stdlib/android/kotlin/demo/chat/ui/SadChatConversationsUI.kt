package sad.android.demo.chat.ui

import android.content.Context

/**
 * واجهة قائمة المحادثات - Conversations List UI
 * 
 * @author فريق لغة ص
 */
object SadChatConversationsUI {
    
    /**
     * تكوين قائمة المحادثات / Conversations list config
     */
    data class ConversationsListConfig(
        val showSearchBar: Boolean = true,
        val showNewChatButton: Boolean = true,
        val showUnreadBadge: Boolean = true,
        val showLastMessage: Boolean = true,
        val showTimestamp: Boolean = true,
        val showOnlineStatus: Boolean = true,
        val swipeActionsEnabled: Boolean = true,
        val pullToRefresh: Boolean = true,
        val emptyStateMessage: String = "لا توجد محادثات"
    )
    
    /**
     * عنصر قائمة المحادثات / Conversation list item
     */
    data class ConversationListItem(
        val id: String,
        val title: String,
        val subtitle: String,
        val avatarUrl: String,
        val timestamp: String,
        val unreadCount: Int,
        val isPinned: Boolean,
        val isMuted: Boolean,
        val isOnline: Boolean,
        val isGroup: Boolean
    )
    
    /**
     * إجراء السحب / Swipe action
     */
    enum class SwipeAction {
        PIN,
        MUTE,
        DELETE,
        ARCHIVE,
        READ
    }
    
    /**
     * مستمع واجهة المحادثات / Conversations UI listener
     */
    interface ConversationsUIListener {
        fun onConversationClicked(conversationId: String)
        fun onConversationLongPressed(conversationId: String)
        fun onSwipeAction(conversationId: String, action: SwipeAction)
        fun onNewChatClicked()
        fun onSearchTextChanged(query: String)
    }
    
    private var uiListener: ConversationsUIListener? = null
    private var config = ConversationsListConfig()
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: ConversationsListConfig) {
        config = newConfig
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setUIListener(listener: ConversationsUIListener?) {
        uiListener = listener
    }
    
    /**
     * تحويل المحادثة لعنصر قائمة / Convert conversation to list item
     */
    fun toListItem(conversation: sad.android.demo.chat.SadChatApp.Conversation): ConversationListItem {
        return ConversationListItem(
            id = conversation.id,
            title = conversation.title,
            subtitle = conversation.lastMessage?.content ?: "",
            avatarUrl = conversation.avatarUrl,
            timestamp = formatTimestamp(conversation.lastMessage?.timestamp ?: conversation.createdAt),
            unreadCount = conversation.unreadCount,
            isPinned = conversation.isPinned,
            isMuted = conversation.isMuted,
            isOnline = conversation.participants.any { it.isOnline },
            isGroup = conversation.isGroup
        )
    }
    
    /**
     * تنسيق الوقت / Format timestamp
     */
    fun formatTimestamp(timestamp: Long): String {
        val now = System.currentTimeMillis()
        val diff = now - timestamp
        
        return when {
            diff < 60000 -> "الآن"
            diff < 3600000 -> "${diff / 60000} د"
            diff < 86400000 -> "${diff / 3600000} س"
            diff < 604800000 -> "${diff / 86400000} ي"
            else -> {
                val date = java.util.Date(timestamp)
                val format = java.text.SimpleDateFormat("d/M", java.util.Locale("ar"))
                format.format(date)
            }
        }
    }
    
    /**
     * الحصول على إجراءات السحب الافتراضية / Get default swipe actions
     */
    fun getDefaultSwipeActions(isPinned: Boolean, isMuted: Boolean): Pair<List<SwipeAction>, List<SwipeAction>> {
        val leftActions = listOf(
            if (isPinned) SwipeAction.PIN else SwipeAction.PIN,
            if (isMuted) SwipeAction.MUTE else SwipeAction.MUTE
        )
        
        val rightActions = listOf(
            SwipeAction.DELETE,
            SwipeAction.ARCHIVE
        )
        
        return leftActions to rightActions
    }
    
    /**
     * الحصول على نص الإجراء / Get action text
     */
    fun getActionText(action: SwipeAction, isPinned: Boolean = false, isMuted: Boolean = false): String {
        return when (action) {
            SwipeAction.PIN -> if (isPinned) "إلغاء التثبيت" else "تثبيت"
            SwipeAction.MUTE -> if (isMuted) "إلغاء الكتم" else "كتم"
            SwipeAction.DELETE -> "حذف"
            SwipeAction.ARCHIVE -> "أرشفة"
            SwipeAction.READ -> "قراءة"
        }
    }
}
