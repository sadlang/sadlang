package sad.android.communication.ir

import android.content.Context
import android.hardware.ConsumerIrManager
import android.os.Build
import androidx.annotation.RequiresApi

/**
 * جسر الأشعة تحت الحمراء - التحكم عن بعد
 * Infrared Bridge - Remote control
 * 
 * @author فريق لغة ص
 */
object SadInfraredBridge {
    
    private var irManager: ConsumerIrManager? = null
    
    /**
     * نطاق الترددات المدعومة / Supported frequency range
     */
    data class FrequencyRange(
        val minFrequency: Int,
        val maxFrequency: Int
    )
    
    /**
     * أمر IR / IR command
     */
    data class IrCommand(
        val name: String,
        val frequency: Int,
        val pattern: IntArray
    ) {
        override fun equals(other: Any?): Boolean {
            if (this === other) return true
            if (javaClass != other?.javaClass) return false
            other as IrCommand
            return name == other.name && frequency == other.frequency && pattern.contentEquals(other.pattern)
        }
        override fun hashCode(): Int = 31 * name.hashCode() + pattern.contentHashCode()
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة الجسر / Initialize bridge
     */
    fun initialize(context: Context): Boolean {
        irManager = context.getSystemService(Context.CONSUMER_IR_SERVICE) as? ConsumerIrManager
        return irManager != null
    }
    
    /**
     * هل IR متاح؟ / Is IR available?
     */
    fun isAvailable(): Boolean {
        return irManager?.hasIrEmitter() == true
    }
    
    /**
     * الحصول على نطاقات الترددات المدعومة / Get supported frequency ranges
     */
    fun getSupportedFrequencies(): List<FrequencyRange> {
        val ranges = irManager?.carrierFrequencies ?: return emptyList()
        return ranges.map { FrequencyRange(it.minFrequency, it.maxFrequency) }
    }
    
    /**
     * هل التردد مدعوم؟ / Is frequency supported?
     */
    fun isFrequencySupported(frequency: Int): Boolean {
        return getSupportedFrequencies().any { 
            frequency >= it.minFrequency && frequency <= it.maxFrequency 
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الإرسال
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال نمط IR / Transmit IR pattern
     */
    fun transmit(frequency: Int, pattern: IntArray): Boolean {
        if (!isAvailable()) return false
        
        return try {
            irManager?.transmit(frequency, pattern)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * إرسال أمر IR / Transmit IR command
     */
    fun transmitCommand(command: IrCommand): Boolean {
        return transmit(command.frequency, command.pattern)
    }
    
    /**
     * إرسال كود Pronto Hex / Transmit Pronto hex code
     */
    fun transmitProntoHex(prontoHex: String): Boolean {
        val pattern = parseProntoHex(prontoHex) ?: return false
        val frequency = calculateProntoFrequency(prontoHex) ?: return false
        return transmit(frequency, pattern)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         أكواد الأجهزة الشائعة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * ترددات شائعة / Common frequencies
     */
    object Frequencies {
        const val NEC = 38000      // معظم أجهزة التحكم
        const val RC5 = 36000     // Philips
        const val RC6 = 36000     // Microsoft MCE
        const val SONY = 40000    // Sony
        const val SAMSUNG = 38000 // Samsung
        const val LG = 38000      // LG
    }
    
    /**
     * أوامر TV شائعة / Common TV commands
     */
    object TvCommands {
        
        /**
         * NEC Power Toggle - تشغيل/إيقاف
         */
        fun necPower(): IrCommand {
            return IrCommand(
                name = "power",
                frequency = Frequencies.NEC,
                pattern = intArrayOf(
                    9000, 4500,  // Header
                    560, 560, 560, 560, 560, 560, 560, 560,  // Address 0x00
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,  // Address inverted
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 560, 560, 1690, 560, 560, 560, 560,  // Command (Power)
                    560, 560, 560, 560, 560, 560, 560, 1690,
                    560, 1690, 560, 560, 560, 1690, 560, 1690,  // Command inverted
                    560, 1690, 560, 1690, 560, 1690, 560, 560,
                    560  // Stop bit
                )
            )
        }
        
        /**
         * NEC Volume Up - رفع الصوت
         */
        fun necVolumeUp(): IrCommand {
            return IrCommand(
                name = "volume_up",
                frequency = Frequencies.NEC,
                pattern = intArrayOf(
                    9000, 4500,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 1690, 560, 560, 560, 560, 560, 560,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 560, 560, 1690, 560, 1690, 560, 1690,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560
                )
            )
        }
        
        /**
         * NEC Volume Down - خفض الصوت
         */
        fun necVolumeDown(): IrCommand {
            return IrCommand(
                name = "volume_down",
                frequency = Frequencies.NEC,
                pattern = intArrayOf(
                    9000, 4500,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 1690, 560, 1690, 560, 560, 560, 560,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 560, 560, 560, 560, 1690, 560, 1690,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560
                )
            )
        }
        
        /**
         * NEC Channel Up - القناة التالية
         */
        fun necChannelUp(): IrCommand {
            return IrCommand(
                name = "channel_up",
                frequency = Frequencies.NEC,
                pattern = intArrayOf(
                    9000, 4500,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 560, 560, 560, 560, 1690, 560, 560,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 1690, 560, 1690, 560, 560, 560, 1690,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560
                )
            )
        }
        
        /**
         * NEC Mute - كتم الصوت
         */
        fun necMute(): IrCommand {
            return IrCommand(
                name = "mute",
                frequency = Frequencies.NEC,
                pattern = intArrayOf(
                    9000, 4500,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 560, 560, 560, 560, 560, 560, 560,
                    560, 1690, 560, 560, 560, 560, 560, 560,
                    560, 1690, 560, 1690, 560, 1690, 560, 1690,
                    560, 560, 560, 1690, 560, 1690, 560, 1690,
                    560
                )
            )
        }
    }
    
    /**
     * أوامر المكيف / AC commands
     */
    object AcCommands {
        // أوامر المكيفات أكثر تعقيداً ومختلفة حسب الماركة
        // يمكن إضافة أوامر محددة لكل ماركة
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         تحويل الأكواد
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحويل NEC إلى نمط / Convert NEC to pattern
     */
    fun necToPattern(address: Int, command: Int): IntArray {
        val pattern = mutableListOf<Int>()
        
        // Header
        pattern.add(9000)
        pattern.add(4500)
        
        // بناء البيانات
        val data = (address and 0xFF) or 
                   ((address.inv() and 0xFF) shl 8) or
                   ((command and 0xFF) shl 16) or
                   ((command.inv() and 0xFF) shl 24)
        
        for (i in 0 until 32) {
            pattern.add(560)
            pattern.add(if ((data shr i) and 1 == 1) 1690 else 560)
        }
        
        // Stop bit
        pattern.add(560)
        
        return pattern.toIntArray()
    }
    
    /**
     * تحويل Sony SIRC إلى نمط / Convert Sony SIRC to pattern
     */
    fun sircToPattern(command: Int, address: Int, bits: Int = 12): IntArray {
        val pattern = mutableListOf<Int>()
        
        // Header
        pattern.add(2400)
        pattern.add(600)
        
        // Command (7 bits)
        for (i in 0 until 7) {
            pattern.add(if ((command shr i) and 1 == 1) 1200 else 600)
            pattern.add(600)
        }
        
        // Address (5 bits for 12-bit, 8 bits for 15-bit, 13 bits for 20-bit)
        val addressBits = bits - 7
        for (i in 0 until addressBits) {
            pattern.add(if ((address shr i) and 1 == 1) 1200 else 600)
            pattern.add(600)
        }
        
        return pattern.toIntArray()
    }
    
    /**
     * تحليل Pronto Hex / Parse Pronto hex
     */
    private fun parseProntoHex(prontoHex: String): IntArray? {
        return try {
            val parts = prontoHex.trim().split("\\s+".toRegex())
            if (parts.size < 4) return null
            
            val frequency = parts[1].toInt(16)
            val firstSeqLength = parts[2].toInt(16) * 2
            val secondSeqLength = parts[3].toInt(16) * 2
            
            val totalLength = firstSeqLength + secondSeqLength
            if (parts.size < 4 + totalLength) return null
            
            val pulse = 1000000.0 / (frequency * 0.241246)
            
            (4 until (4 + totalLength)).map { i ->
                (parts[i].toInt(16) * pulse).toInt()
            }.toIntArray()
        } catch (e: Exception) {
            null
        }
    }
    
    /**
     * حساب تردد Pronto / Calculate Pronto frequency
     */
    private fun calculateProntoFrequency(prontoHex: String): Int? {
        return try {
            val parts = prontoHex.trim().split("\\s+".toRegex())
            if (parts.size < 2) return null
            
            val freqCode = parts[1].toInt(16)
            if (freqCode == 0) return null
            
            (1000000.0 / (freqCode * 0.241246)).toInt()
        } catch (e: Exception) {
            null
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        irManager = null
    }
}
