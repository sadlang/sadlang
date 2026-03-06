package sad.android.tv.hdr

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.view.Display

/**
 * جسر HDR التلفزيوني - HDR support and display modes
 * TV HDR Bridge - HDR support and display modes
 * 
 * @author فريق لغة ص
 */
object SadTVHDRBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * نوع HDR
     */
    enum class HDRType {
        NONE,
        HDR10,
        HDR10_PLUS,
        DOLBY_VISION,
        HLG,
        UNKNOWN
    }
    
    /**
     * معلومات العرض / Display info
     */
    data class DisplayInfo(
        val width: Int,
        val height: Int,
        val refreshRate: Float,
        val supportedHdrTypes: List<HDRType>,
        val hdrCapabilities: HDRCapabilities?,
        val isHdrActive: Boolean,
        val colorMode: ColorMode
    )
    
    /**
     * قدرات HDR
     */
    data class HDRCapabilities(
        val supportedTypes: List<HDRType>,
        val maxLuminance: Float,
        val maxAverageLuminance: Float,
        val minLuminance: Float
    )
    
    /**
     * وضع الألوان / Color mode
     */
    enum class ColorMode {
        STANDARD,
        WIDE_COLOR_GAMUT,
        HDR
    }
    
    /**
     * جودة الفيديو / Video quality
     */
    data class VideoQuality(
        val width: Int,
        val height: Int,
        val frameRate: Int,
        val hdrType: HDRType,
        val bitrate: Int,
        val codec: String
    )
    
    /**
     * مستمع HDR
     */
    interface HDRListener {
        fun onHDRStateChanged(isActive: Boolean, type: HDRType)
        fun onDisplayModeChanged(info: DisplayInfo)
    }
    
    private var hdrListener: HDRListener? = null
    private var cachedDisplayInfo: DisplayInfo? = null
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        refreshDisplayInfo()
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setHDRListener(listener: HDRListener?) {
        hdrListener = listener
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معلومات العرض
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحديث معلومات العرض / Refresh display info
     */
    fun refreshDisplayInfo() {
        val ctx = context ?: return
        val windowManager = ctx.getSystemService(Context.WINDOW_SERVICE) as android.view.WindowManager
        val display = windowManager.defaultDisplay
        
        val hdrCaps = if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.N) {
            display.hdrCapabilities?.let { caps ->
                HDRCapabilities(
                    supportedTypes = mapHdrTypes(caps.supportedHdrTypes),
                    maxLuminance = caps.desiredMaxLuminance,
                    maxAverageLuminance = caps.desiredMaxAverageLuminance,
                    minLuminance = caps.desiredMinLuminance
                )
            }
        } else null
        
        val metrics = android.util.DisplayMetrics()
        display.getMetrics(metrics)
        
        cachedDisplayInfo = DisplayInfo(
            width = metrics.widthPixels,
            height = metrics.heightPixels,
            refreshRate = display.refreshRate,
            supportedHdrTypes = hdrCaps?.supportedTypes ?: emptyList(),
            hdrCapabilities = hdrCaps,
            isHdrActive = isHDRActive(),
            colorMode = getCurrentColorMode()
        )
    }
    
    /**
     * الحصول على معلومات العرض / Get display info
     */
    fun getDisplayInfo(): DisplayInfo? = cachedDisplayInfo
    
    private fun mapHdrTypes(types: IntArray): List<HDRType> {
        if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.N) {
            return emptyList()
        }
        
        return types.map { type ->
            when (type) {
                Display.HdrCapabilities.HDR_TYPE_HDR10 -> HDRType.HDR10
                Display.HdrCapabilities.HDR_TYPE_HDR10_PLUS -> HDRType.HDR10_PLUS
                Display.HdrCapabilities.HDR_TYPE_DOLBY_VISION -> HDRType.DOLBY_VISION
                Display.HdrCapabilities.HDR_TYPE_HLG -> HDRType.HLG
                else -> HDRType.UNKNOWN
            }
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التحقق من الدعم
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * هل HDR مدعوم؟ / Is HDR supported?
     */
    fun isHDRSupported(): Boolean {
        return cachedDisplayInfo?.supportedHdrTypes?.isNotEmpty() ?: false
    }
    
    /**
     * هل نوع معين مدعوم؟ / Is specific type supported?
     */
    fun isHDRTypeSupported(type: HDRType): Boolean {
        return cachedDisplayInfo?.supportedHdrTypes?.contains(type) ?: false
    }
    
    /**
     * هل HDR نشط؟ / Is HDR active?
     */
    fun isHDRActive(): Boolean {
        // في التطبيق الحقيقي: التحقق من حالة العرض الفعلية
        return false
    }
    
    /**
     * الحصول على نوع HDR النشط / Get active HDR type
     */
    fun getActiveHDRType(): HDRType {
        if (!isHDRActive()) return HDRType.NONE
        
        // في التطبيق الحقيقي: إرجاع النوع الفعلي
        return cachedDisplayInfo?.supportedHdrTypes?.firstOrNull() ?: HDRType.NONE
    }
    
    /**
     * الحصول على وضع الألوان الحالي / Get current color mode
     */
    private fun getCurrentColorMode(): ColorMode {
        return if (isHDRActive()) ColorMode.HDR
        else ColorMode.STANDARD
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         اختيار الجودة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * اختيار أفضل جودة / Select best quality
     */
    fun selectBestQuality(qualities: List<VideoQuality>): VideoQuality? {
        if (qualities.isEmpty()) return null
        
        // تفضيل HDR إذا كان مدعوماً
        val supportedHdr = cachedDisplayInfo?.supportedHdrTypes ?: emptyList()
        
        val hdrQualities = qualities.filter { it.hdrType in supportedHdr }
        if (hdrQualities.isNotEmpty()) {
            return hdrQualities.maxByOrNull { it.width * it.height }
        }
        
        // إذا لم يكن HDR متاحاً، اختر أعلى دقة SDR
        return qualities.filter { it.hdrType == HDRType.NONE }
            .maxByOrNull { it.width * it.height }
            ?: qualities.maxByOrNull { it.width * it.height }
    }
    
    /**
     * فلترة الجودات حسب الدعم / Filter qualities by support
     */
    fun filterSupportedQualities(qualities: List<VideoQuality>): List<VideoQuality> {
        val supportedHdr = cachedDisplayInfo?.supportedHdrTypes ?: emptyList()
        val displayInfo = cachedDisplayInfo
        
        return qualities.filter { quality ->
            // تصفية حسب HDR
            (quality.hdrType == HDRType.NONE || quality.hdrType in supportedHdr) &&
            // تصفية حسب الدقة
            (displayInfo == null || (quality.width <= displayInfo.width && quality.height <= displayInfo.height))
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         المعلومات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * الحصول على اسم نوع HDR / Get HDR type name
     */
    fun getHDRTypeName(type: HDRType): String {
        return when (type) {
            HDRType.NONE -> "SDR"
            HDRType.HDR10 -> "HDR10"
            HDRType.HDR10_PLUS -> "HDR10+"
            HDRType.DOLBY_VISION -> "دولبي فيجن"
            HDRType.HLG -> "HLG"
            HDRType.UNKNOWN -> "غير معروف"
        }
    }
    
    /**
     * الحصول على وصف القدرات / Get capabilities description
     */
    fun getCapabilitiesDescription(): String {
        val caps = cachedDisplayInfo?.hdrCapabilities ?: return "HDR غير مدعوم"
        
        val types = caps.supportedTypes.map { getHDRTypeName(it) }
        return "أنواع HDR المدعومة: ${types.joinToString("، ")}\n" +
               "السطوع الأقصى: ${caps.maxLuminance} nits"
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        cachedDisplayInfo = null
        hdrListener = null
        context = null
    }
}
