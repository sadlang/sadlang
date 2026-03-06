package sad.android.tv.cards

import android.content.Context
import android.graphics.Color
import android.os.Handler
import android.os.Looper

/**
 * جسر بطاقات التلفزيون - ImageCardView و بطاقات Leanback
 * TV Cards Bridge - ImageCardView and Leanback cards
 * 
 * @author فريق لغة ص
 */
object SadTVCardsBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * نوع البطاقة / Card type
     */
    enum class CardType {
        IMAGE_CARD,        // بطاقة صورة قياسية
        TEXT_CARD,         // بطاقة نصية
        ICON_CARD,         // بطاقة أيقونة
        ACTION_CARD,       // بطاقة إجراء
        DIVIDER_CARD,      // بطاقة فاصل
        CUSTOM_CARD        // بطاقة مخصصة
    }
    
    /**
     * تكوين البطاقة / Card config
     */
    data class CardConfig(
        val type: CardType = CardType.IMAGE_CARD,
        val width: Int = 313,
        val height: Int = 176,
        val titleLines: Int = 1,
        val contentLines: Int = 2,
        val showTitle: Boolean = true,
        val showContent: Boolean = true,
        val showBadge: Boolean = true,
        val selectedScale: Float = 1.1f,
        val selectedElevation: Float = 8f,
        val cornerRadius: Float = 8f
    )
    
    /**
     * نمط البطاقة / Card style
     */
    data class CardStyle(
        val backgroundColor: Int = Color.WHITE,
        val titleColor: Int = Color.BLACK,
        val contentColor: Int = Color.GRAY,
        val badgeColor: Int = 0xFF1E88E5.toInt(),
        val selectedOverlayColor: Int = 0x33000000,
        val infoAreaColor: Int = Color.WHITE,
        val infoSelectedColor: Int = 0xFF1E88E5.toInt()
    )
    
    /**
     * بيانات البطاقة / Card data
     */
    data class CardData(
        val id: String,
        val title: String,
        val content: String = "",
        val imageUrl: String = "",
        val iconResId: Int? = null,
        val badgeText: String? = null,
        val badgeIconResId: Int? = null,
        val extraBadge: String? = null,
        val progress: Float? = null,    // 0-1
        val metadata: Map<String, Any> = emptyMap()
    )
    
    /**
     * بطاقة مع التكوين / Card with config
     */
    data class ConfiguredCard(
        val data: CardData,
        val config: CardConfig = CardConfig(),
        val style: CardStyle = CardStyle()
    )
    
    /**
     * مستمع البطاقة / Card listener
     */
    interface CardListener {
        fun onCardFocused(card: CardData)
        fun onCardUnfocused(card: CardData)
        fun onCardClicked(card: CardData)
        fun onCardLongClicked(card: CardData): Boolean
    }
    
    private var cardListener: CardListener? = null
    private var defaultConfig = CardConfig()
    private var defaultStyle = CardStyle()
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    /**
     * تعيين التكوين الافتراضي / Set default config
     */
    fun setDefaultConfig(config: CardConfig) {
        defaultConfig = config
    }
    
    /**
     * تعيين النمط الافتراضي / Set default style
     */
    fun setDefaultStyle(style: CardStyle) {
        defaultStyle = style
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setCardListener(listener: CardListener?) {
        cardListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إنشاء البطاقات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء بطاقة صورة / Create image card
     */
    fun createImageCard(
        id: String,
        title: String,
        content: String = "",
        imageUrl: String
    ): ConfiguredCard {
        return ConfiguredCard(
            data = CardData(
                id = id,
                title = title,
                content = content,
                imageUrl = imageUrl
            ),
            config = defaultConfig.copy(type = CardType.IMAGE_CARD),
            style = defaultStyle
        )
    }
    
    /**
     * إنشاء بطاقة أيقونة / Create icon card
     */
    fun createIconCard(
        id: String,
        title: String,
        iconResId: Int
    ): ConfiguredCard {
        return ConfiguredCard(
            data = CardData(
                id = id,
                title = title,
                iconResId = iconResId
            ),
            config = defaultConfig.copy(
                type = CardType.ICON_CARD,
                width = 160,
                height = 160
            ),
            style = defaultStyle
        )
    }
    
    /**
     * إنشاء بطاقة إجراء / Create action card
     */
    fun createActionCard(
        id: String,
        title: String,
        iconResId: Int? = null
    ): ConfiguredCard {
        return ConfiguredCard(
            data = CardData(
                id = id,
                title = title,
                iconResId = iconResId
            ),
            config = defaultConfig.copy(
                type = CardType.ACTION_CARD,
                width = 120,
                height = 120,
                showContent = false
            ),
            style = defaultStyle
        )
    }
    
    /**
     * إنشاء بطاقة فيلم / Create movie card
     */
    fun createMovieCard(
        id: String,
        title: String,
        year: String,
        imageUrl: String,
        rating: String? = null,
        progress: Float? = null
    ): ConfiguredCard {
        return ConfiguredCard(
            data = CardData(
                id = id,
                title = title,
                content = year,
                imageUrl = imageUrl,
                badgeText = rating,
                progress = progress
            ),
            config = defaultConfig.copy(
                type = CardType.IMAGE_CARD,
                width = 180,
                height = 260
            ),
            style = defaultStyle
        )
    }
    
    /**
     * إنشاء بطاقة مسلسل / Create series card
     */
    fun createSeriesCard(
        id: String,
        title: String,
        seasons: Int,
        imageUrl: String,
        newEpisodes: Boolean = false
    ): ConfiguredCard {
        return ConfiguredCard(
            data = CardData(
                id = id,
                title = title,
                content = "$seasons مواسم",
                imageUrl = imageUrl,
                extraBadge = if (newEpisodes) "جديد" else null
            ),
            config = defaultConfig.copy(
                type = CardType.IMAGE_CARD,
                width = 180,
                height = 260
            ),
            style = defaultStyle
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أنماط جاهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * نمط الوضع الداكن / Dark mode style
     */
    fun darkModeStyle(): CardStyle {
        return CardStyle(
            backgroundColor = 0xFF2D2D2D.toInt(),
            titleColor = Color.WHITE,
            contentColor = 0xFFB3B3B3.toInt(),
            badgeColor = 0xFFE50914.toInt(),  // Netflix red
            selectedOverlayColor = 0x55FFFFFF,
            infoAreaColor = 0xFF1F1F1F.toInt(),
            infoSelectedColor = 0xFF2196F3.toInt()
        )
    }
    
    /**
     * نمط Netflix / Netflix style
     */
    fun netflixStyle(): CardStyle {
        return CardStyle(
            backgroundColor = 0xFF141414.toInt(),
            titleColor = Color.WHITE,
            contentColor = 0xFF808080.toInt(),
            badgeColor = 0xFFE50914.toInt(),
            selectedOverlayColor = 0x33E50914,
            infoAreaColor = 0xFF141414.toInt(),
            infoSelectedColor = 0xFFE50914.toInt()
        )
    }
    
    /**
     * نمط Material / Material style
     */
    fun materialStyle(): CardStyle {
        return CardStyle(
            backgroundColor = Color.WHITE,
            titleColor = 0xFF212121.toInt(),
            contentColor = 0xFF757575.toInt(),
            badgeColor = 0xFF1E88E5.toInt(),
            selectedOverlayColor = 0x331E88E5,
            infoAreaColor = Color.WHITE,
            infoSelectedColor = 0xFF1E88E5.toInt()
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أحداث البطاقة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إعلام بالتركيز / Notify focused
     */
    fun notifyCardFocused(cardId: String, cards: List<ConfiguredCard>) {
        val card = cards.find { it.data.id == cardId }?.data ?: return
        mainHandler.post { cardListener?.onCardFocused(card) }
    }
    
    /**
     * إعلام بإلغاء التركيز / Notify unfocused
     */
    fun notifyCardUnfocused(cardId: String, cards: List<ConfiguredCard>) {
        val card = cards.find { it.data.id == cardId }?.data ?: return
        mainHandler.post { cardListener?.onCardUnfocused(card) }
    }
    
    /**
     * إعلام بالنقر / Notify clicked
     */
    fun notifyCardClicked(cardId: String, cards: List<ConfiguredCard>) {
        val card = cards.find { it.data.id == cardId }?.data ?: return
        mainHandler.post { cardListener?.onCardClicked(card) }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        cardListener = null
        context = null
    }
}
