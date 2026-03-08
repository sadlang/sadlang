/**
 * =============================================================================
 * @file SadNfc.kt
 * @brief (AR) واجهة NFC للغة ص
 * @brief (EN) NFC Interface for Sad Language
 * =============================================================================
 */

package sad.lang.android

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.nfc.*
import android.nfc.tech.*
import android.os.Build
import android.os.Bundle
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import java.nio.charset.Charset

/**
 * نظام NFC للغة ص
 */
class SadNfc(private val context: Context) {
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * نتيجة قراءة NFC
     */
    sealed class NfcResult {
        data class Success(val data: List<NdefRecord>) : NfcResult()
        data class Error(val message: String) : NfcResult()
        object NotSupported : NfcResult()
        object Disabled : NfcResult()
    }
    
    /**
     * سجل NDEF
     */
    data class NdefRecord(
        val type: String,
        val payload: String,
        val id: ByteArray?
    ) {
        override fun equals(other: Any?): Boolean {
            if (this === other) return true
            if (javaClass != other?.javaClass) return false
            other as NdefRecord
            return type == other.type && payload == other.payload
        }
        
        override fun hashCode(): Int {
            return 31 * type.hashCode() + payload.hashCode()
        }
    }
    
    /**
     * مستمع NFC
     */
    interface NfcListener {
        fun onTagDiscovered(tag: Tag)
        fun onNdefMessage(records: List<NdefRecord>)
        fun onError(message: String)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Properties
    // ═══════════════════════════════════════════════════════════════════════════
    
    private val nfcAdapter: NfcAdapter? by lazy {
        NfcAdapter.getDefaultAdapter(context)
    }
    
    private var listener: NfcListener? = null
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Availability
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هل NFC متاح؟
     */
    fun isAvailable(): Boolean = nfcAdapter != null
    
    /**
     * هل NFC مُفعّل؟
     */
    fun isEnabled(): Boolean = nfcAdapter?.isEnabled == true
    
    /**
     * هل HCE متاح؟
     */
    fun isHceSupported(): Boolean {
        return context.packageManager.hasSystemFeature("android.hardware.nfc.hce")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Reader Mode
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تفعيل وضع القارئ
     */
    fun enableReaderMode(activity: Activity, listener: NfcListener) {
        this.listener = listener
        
        val adapter = nfcAdapter
        if (adapter == null) {
            listener.onError("NFC غير متاح")
            return
        }
        
        val flags = NfcAdapter.FLAG_READER_NFC_A or
                   NfcAdapter.FLAG_READER_NFC_B or
                   NfcAdapter.FLAG_READER_NFC_F or
                   NfcAdapter.FLAG_READER_NFC_V or
                   NfcAdapter.FLAG_READER_NFC_BARCODE
        
        val options = Bundle().apply {
            putInt(NfcAdapter.EXTRA_READER_PRESENCE_CHECK_DELAY, 250)
        }
        
        adapter.enableReaderMode(activity, { tag ->
            listener.onTagDiscovered(tag)
            
            // Try to read NDEF
            val ndef = Ndef.get(tag)
            if (ndef != null) {
                try {
                    ndef.connect()
                    val ndefMessage = ndef.ndefMessage
                    if (ndefMessage != null) {
                        val records = parseNdefMessage(ndefMessage)
                        listener.onNdefMessage(records)
                    }
                    ndef.close()
                } catch (e: Exception) {
                    listener.onError("خطأ في قراءة NDEF: ${e.message}")
                }
            }
        }, flags, options)
    }
    
    /**
     * إيقاف وضع القارئ
     */
    fun disableReaderMode(activity: Activity) {
        nfcAdapter?.disableReaderMode(activity)
        listener = null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Read/Write NDEF
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * قراءة علامة NDEF
     */
    suspend fun readTag(tag: Tag): NfcResult = withContext(Dispatchers.IO) {
        val ndef = Ndef.get(tag)
        if (ndef == null) {
            return@withContext NfcResult.Error("العلامة لا تدعم NDEF")
        }
        
        try {
            ndef.connect()
            val ndefMessage = ndef.ndefMessage
            ndef.close()
            
            if (ndefMessage == null) {
                return@withContext NfcResult.Success(emptyList())
            }
            
            val records = parseNdefMessage(ndefMessage)
            NfcResult.Success(records)
        } catch (e: Exception) {
            NfcResult.Error("خطأ في القراءة: ${e.message}")
        }
    }
    
    /**
     * كتابة نص على علامة NDEF
     */
    suspend fun writeText(tag: Tag, text: String, languageCode: String = "ar"): NfcResult = withContext(Dispatchers.IO) {
        val ndef = Ndef.get(tag)
        if (ndef == null) {
            return@withContext NfcResult.Error("العلامة لا تدعم NDEF")
        }
        
        try {
            ndef.connect()
            
            if (!ndef.isWritable) {
                ndef.close()
                return@withContext NfcResult.Error("العلامة محمية ضد الكتابة")
            }
            
            val record = createTextRecord(text, languageCode)
            val ndefMessage = NdefMessage(arrayOf(record))
            
            if (ndefMessage.byteArrayLength > ndef.maxSize) {
                ndef.close()
                return@withContext NfcResult.Error("النص أكبر من سعة العلامة")
            }
            
            ndef.writeNdefMessage(ndefMessage)
            ndef.close()
            
            NfcResult.Success(listOf(NdefRecord("text/plain", text, null)))
        } catch (e: Exception) {
            NfcResult.Error("خطأ في الكتابة: ${e.message}")
        }
    }
    
    /**
     * كتابة رابط URI
     */
    suspend fun writeUri(tag: Tag, uri: String): NfcResult = withContext(Dispatchers.IO) {
        val ndef = Ndef.get(tag)
        if (ndef == null) {
            return@withContext NfcResult.Error("العلامة لا تدعم NDEF")
        }
        
        try {
            ndef.connect()
            
            if (!ndef.isWritable) {
                ndef.close()
                return@withContext NfcResult.Error("العلامة محمية ضد الكتابة")
            }
            
            val record = android.nfc.NdefRecord.createUri(uri)
            val ndefMessage = NdefMessage(arrayOf(record))
            
            ndef.writeNdefMessage(ndefMessage)
            ndef.close()
            
            NfcResult.Success(listOf(NdefRecord("uri", uri, null)))
        } catch (e: Exception) {
            NfcResult.Error("خطأ في الكتابة: ${e.message}")
        }
    }
    
    /**
     * مسح علامة NDEF
     */
    suspend fun eraseTag(tag: Tag): NfcResult = withContext(Dispatchers.IO) {
        val ndef = Ndef.get(tag)
        if (ndef == null) {
            return@withContext NfcResult.Error("العلامة لا تدعم NDEF")
        }
        
        try {
            ndef.connect()
            
            if (!ndef.isWritable) {
                ndef.close()
                return@withContext NfcResult.Error("العلامة محمية ضد الكتابة")
            }
            
            // Write empty message
            val emptyRecord = android.nfc.NdefRecord(
                android.nfc.NdefRecord.TNF_EMPTY,
                ByteArray(0),
                ByteArray(0),
                ByteArray(0)
            )
            val emptyMessage = NdefMessage(arrayOf(emptyRecord))
            ndef.writeNdefMessage(emptyMessage)
            ndef.close()
            
            NfcResult.Success(emptyList())
        } catch (e: Exception) {
            NfcResult.Error("خطأ في المسح: ${e.message}")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Format Tags
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تهيئة علامة كـ NDEF
     */
    suspend fun formatTag(tag: Tag): NfcResult = withContext(Dispatchers.IO) {
        val ndefFormatable = NdefFormatable.get(tag)
        if (ndefFormatable == null) {
            return@withContext NfcResult.Error("لا يمكن تهيئة هذه العلامة")
        }
        
        try {
            ndefFormatable.connect()
            
            val emptyRecord = android.nfc.NdefRecord(
                android.nfc.NdefRecord.TNF_EMPTY,
                ByteArray(0),
                ByteArray(0),
                ByteArray(0)
            )
            val emptyMessage = NdefMessage(arrayOf(emptyRecord))
            
            ndefFormatable.format(emptyMessage)
            ndefFormatable.close()
            
            NfcResult.Success(emptyList())
        } catch (e: Exception) {
            NfcResult.Error("خطأ في التهيئة: ${e.message}")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Tag Information
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * معلومات العلامة
     */
    data class TagInfo(
        val id: String,
        val technologies: List<String>,
        val isNdef: Boolean,
        val isWritable: Boolean,
        val maxSize: Int,
        val type: String
    )
    
    /**
     * الحصول على معلومات العلامة
     */
    fun getTagInfo(tag: Tag): TagInfo {
        val id = tag.id.joinToString("") { "%02X".format(it) }
        val technologies = tag.techList.map { it.substringAfterLast('.') }
        
        val ndef = Ndef.get(tag)
        val isNdef = ndef != null
        val isWritable = ndef?.isWritable ?: false
        val maxSize = ndef?.maxSize ?: 0
        val type = ndef?.type ?: "unknown"
        
        return TagInfo(
            id = id,
            technologies = technologies,
            isNdef = isNdef,
            isWritable = isWritable,
            maxSize = maxSize,
            type = type
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    //  Helpers
    // ═══════════════════════════════════════════════════════════════════════════
    
    private fun parseNdefMessage(message: NdefMessage): List<NdefRecord> {
        return message.records.map { record ->
            val payload = when (record.tnf) {
                android.nfc.NdefRecord.TNF_WELL_KNOWN -> {
                    if (record.type.contentEquals(android.nfc.NdefRecord.RTD_TEXT)) {
                        parseTextRecord(record)
                    } else if (record.type.contentEquals(android.nfc.NdefRecord.RTD_URI)) {
                        record.toUri()?.toString() ?: ""
                    } else {
                        String(record.payload, Charset.forName("UTF-8"))
                    }
                }
                android.nfc.NdefRecord.TNF_ABSOLUTE_URI -> {
                    String(record.payload, Charset.forName("UTF-8"))
                }
                else -> {
                    String(record.payload, Charset.forName("UTF-8"))
                }
            }
            
            NdefRecord(
                type = String(record.type, Charset.forName("UTF-8")),
                payload = payload,
                id = record.id
            )
        }
    }
    
    private fun parseTextRecord(record: android.nfc.NdefRecord): String {
        val payload = record.payload
        val textEncoding = if ((payload[0].toInt() and 0x80) == 0) "UTF-8" else "UTF-16"
        val languageCodeLength = payload[0].toInt() and 0x3F
        
        return String(
            payload,
            languageCodeLength + 1,
            payload.size - languageCodeLength - 1,
            Charset.forName(textEncoding)
        )
    }
    
    private fun createTextRecord(text: String, languageCode: String): android.nfc.NdefRecord {
        val language = languageCode.toByteArray(Charset.forName("US-ASCII"))
        val textBytes = text.toByteArray(Charset.forName("UTF-8"))
        
        val payload = ByteArray(1 + language.size + textBytes.size)
        payload[0] = language.size.toByte()
        System.arraycopy(language, 0, payload, 1, language.size)
        System.arraycopy(textBytes, 0, payload, 1 + language.size, textBytes.size)
        
        return android.nfc.NdefRecord(
            android.nfc.NdefRecord.TNF_WELL_KNOWN,
            android.nfc.NdefRecord.RTD_TEXT,
            ByteArray(0),
            payload
        )
    }
    
    /**
     * معالجة Intent من NFC
     */
    fun handleIntent(intent: Intent): List<NdefRecord>? {
        if (NfcAdapter.ACTION_NDEF_DISCOVERED != intent.action &&
            NfcAdapter.ACTION_TECH_DISCOVERED != intent.action &&
            NfcAdapter.ACTION_TAG_DISCOVERED != intent.action) {
            return null
        }
        
        val rawMessages = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            intent.getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES, android.nfc.NdefMessage::class.java)
        } else {
            @Suppress("DEPRECATION")
            intent.getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES)
        }
        
        if (rawMessages.isNullOrEmpty()) return null
        
        val records = mutableListOf<NdefRecord>()
        rawMessages.forEach { rawMessage ->
            val ndefMessage = rawMessage as NdefMessage
            records.addAll(parseNdefMessage(ndefMessage))
        }
        
        return records
    }
}
