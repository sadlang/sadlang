package sad.android.communication.nfc

import android.app.Activity
import android.app.PendingIntent
import android.content.Intent
import android.content.IntentFilter
import android.nfc.NdefMessage
import android.nfc.NdefRecord
import android.nfc.NfcAdapter
import android.nfc.Tag
import android.nfc.tech.*
import android.os.Build
import android.os.Bundle
import java.nio.charset.Charset

/**
 * جسر NFC - الاتصال قريب المدى
 * NFC Bridge - Near Field Communication
 * 
 * @author فريق لغة ص
 */
object SadNFCBridge {
    
    private var nfcAdapter: NfcAdapter? = null
    private var pendingIntent: PendingIntent? = null
    
    /**
     * نوع السجل / Record type
     */
    enum class RecordType {
        TEXT,           // نص
        URI,            // رابط
        MIME,           // نوع MIME
        EXTERNAL,       // خارجي
        UNKNOWN         // غير معروف
    }
    
    /**
     * سجل NFC / NFC record
     */
    data class NfcRecord(
        val type: RecordType,
        val payload: String,
        val languageCode: String? = null,
        val mimeType: String? = null
    )
    
    /**
     * معلومات البطاقة / Tag info
     */
    data class TagInfo(
        val id: String,
        val technologies: List<String>,
        val maxSize: Int,
        val isWritable: Boolean,
        val records: List<NfcRecord>
    )
    
    /**
     * مستمع NFC / NFC listener
     */
    interface NfcListener {
        fun onTagDiscovered(tag: TagInfo)
        fun onNdefMessage(records: List<NfcRecord>)
        fun onError(message: String)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة NFC / Initialize NFC
     */
    fun initialize(activity: Activity): Boolean {
        nfcAdapter = NfcAdapter.getDefaultAdapter(activity)
        
        if (nfcAdapter == null) return false
        
        val intent = Intent(activity, activity.javaClass).apply {
            addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP)
        }
        
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        
        pendingIntent = PendingIntent.getActivity(activity, 0, intent, flags)
        
        return true
    }
    
    /**
     * هل NFC متاح؟ / Is NFC available?
     */
    fun isAvailable(): Boolean = nfcAdapter != null
    
    /**
     * هل NFC مفعل؟ / Is NFC enabled?
     */
    fun isEnabled(): Boolean = nfcAdapter?.isEnabled == true
    
    // ═══════════════════════════════════════════════════════════════
    //                         التمكين/التعطيل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تفعيل وضع الاستقبال / Enable foreground dispatch
     */
    fun enableForegroundDispatch(activity: Activity, ndef: Boolean = true) {
        val adapter = nfcAdapter ?: return
        val intent = pendingIntent ?: return
        
        if (ndef) {
            val ndefFilter = IntentFilter(NfcAdapter.ACTION_NDEF_DISCOVERED).apply {
                try { addDataType("*/*") } catch (e: Exception) {}
            }
            
            val filters = arrayOf(ndefFilter)
            val techLists = arrayOf(
                arrayOf(Ndef::class.java.name),
                arrayOf(NdefFormatable::class.java.name)
            )
            
            adapter.enableForegroundDispatch(activity, intent, filters, techLists)
        } else {
            adapter.enableForegroundDispatch(activity, intent, null, null)
        }
    }
    
    /**
     * تعطيل وضع الاستقبال / Disable foreground dispatch
     */
    fun disableForegroundDispatch(activity: Activity) {
        nfcAdapter?.disableForegroundDispatch(activity)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         معالجة Intent
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * معالجة Intent / Handle NFC intent
     */
    fun handleIntent(intent: Intent): TagInfo? {
        if (intent.action !in listOf(
                NfcAdapter.ACTION_TAG_DISCOVERED,
                NfcAdapter.ACTION_NDEF_DISCOVERED,
                NfcAdapter.ACTION_TECH_DISCOVERED
            )) {
            return null
        }
        
        val tag = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            intent.getParcelableExtra(NfcAdapter.EXTRA_TAG, Tag::class.java)
        } else {
            @Suppress("DEPRECATION")
            intent.getParcelableExtra(NfcAdapter.EXTRA_TAG)
        } ?: return null
        
        return parseTag(tag)
    }
    
    /**
     * تحليل البطاقة / Parse tag
     */
    fun parseTag(tag: Tag): TagInfo {
        val id = tag.id.toHexString()
        val technologies = tag.techList.map { it.substringAfterLast('.') }
        
        var maxSize = 0
        var isWritable = false
        val records = mutableListOf<NfcRecord>()
        
        // محاولة قراءة NDEF
        val ndef = Ndef.get(tag)
        if (ndef != null) {
            try {
                ndef.connect()
                maxSize = ndef.maxSize
                isWritable = ndef.isWritable
                
                ndef.cachedNdefMessage?.records?.forEach { record ->
                    records.add(parseNdefRecord(record))
                }
                
                ndef.close()
            } catch (e: Exception) {
                try { ndef.close() } catch (_: Exception) {}
            }
        }
        
        return TagInfo(
            id = id,
            technologies = technologies,
            maxSize = maxSize,
            isWritable = isWritable,
            records = records
        )
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الكتابة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * كتابة نص / Write text
     */
    fun writeText(tag: Tag, text: String, languageCode: String = "ar"): Boolean {
        val record = createTextRecord(text, languageCode)
        return writeNdef(tag, NdefMessage(arrayOf(record)))
    }
    
    /**
     * كتابة رابط / Write URI
     */
    fun writeUri(tag: Tag, uri: String): Boolean {
        val record = NdefRecord.createUri(uri)
        return writeNdef(tag, NdefMessage(arrayOf(record)))
    }
    
    /**
     * كتابة MIME / Write MIME
     */
    fun writeMime(tag: Tag, mimeType: String, data: ByteArray): Boolean {
        val record = NdefRecord.createMime(mimeType, data)
        return writeNdef(tag, NdefMessage(arrayOf(record)))
    }
    
    /**
     * كتابة سجلات متعددة / Write multiple records
     */
    fun writeRecords(tag: Tag, records: List<NfcRecord>): Boolean {
        val ndefRecords = records.map { record ->
            when (record.type) {
                RecordType.TEXT -> createTextRecord(record.payload, record.languageCode ?: "ar")
                RecordType.URI -> NdefRecord.createUri(record.payload)
                RecordType.MIME -> NdefRecord.createMime(
                    record.mimeType ?: "text/plain",
                    record.payload.toByteArray()
                )
                RecordType.EXTERNAL -> NdefRecord.createExternal(
                    "sad.lang",
                    record.mimeType ?: "data",
                    record.payload.toByteArray()
                )
                else -> NdefRecord.createTextRecord(record.languageCode ?: "ar", record.payload)
            }
        }
        
        return writeNdef(tag, NdefMessage(ndefRecords.toTypedArray()))
    }
    
    /**
     * كتابة رسالة NDEF / Write NDEF message
     */
    fun writeNdef(tag: Tag, message: NdefMessage): Boolean {
        val ndef = Ndef.get(tag)
        
        if (ndef != null) {
            try {
                ndef.connect()
                
                if (!ndef.isWritable) {
                    ndef.close()
                    return false
                }
                
                if (message.byteArrayLength > ndef.maxSize) {
                    ndef.close()
                    return false
                }
                
                ndef.writeNdefMessage(message)
                ndef.close()
                return true
            } catch (e: Exception) {
                try { ndef.close() } catch (_: Exception) {}
                return false
            }
        }
        
        // محاولة التنسيق
        val formatable = NdefFormatable.get(tag)
        if (formatable != null) {
            try {
                formatable.connect()
                formatable.format(message)
                formatable.close()
                return true
            } catch (e: Exception) {
                try { formatable.close() } catch (_: Exception) {}
            }
        }
        
        return false
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         Beam
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إرسال عبر Beam / Send via Beam (deprecated in API 29)
     */
    @Suppress("DEPRECATION")
    fun enableBeam(activity: Activity, text: String) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) return
        
        val record = createTextRecord(text, "ar")
        val message = NdefMessage(arrayOf(record))
        
        nfcAdapter?.setNdefPushMessage(message, activity)
    }
    
    @Suppress("DEPRECATION")
    fun disableBeam(activity: Activity) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) return
        
        nfcAdapter?.setNdefPushMessage(null, activity)
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         قراءة التقنيات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * قراءة MifareClassic / Read MifareClassic
     */
    fun readMifareClassic(tag: Tag, sector: Int, block: Int): ByteArray? {
        val mifare = MifareClassic.get(tag) ?: return null
        
        try {
            mifare.connect()
            
            if (!mifare.authenticateSectorWithKeyA(sector, MifareClassic.KEY_DEFAULT)) {
                mifare.close()
                return null
            }
            
            val blockIndex = mifare.sectorToBlock(sector) + block
            val data = mifare.readBlock(blockIndex)
            
            mifare.close()
            return data
        } catch (e: Exception) {
            try { mifare.close() } catch (_: Exception) {}
            return null
        }
    }
    
    /**
     * قراءة MifareUltralight / Read MifareUltralight
     */
    fun readMifareUltralight(tag: Tag, page: Int): ByteArray? {
        val mifare = MifareUltralight.get(tag) ?: return null
        
        try {
            mifare.connect()
            val data = mifare.readPages(page)
            mifare.close()
            return data
        } catch (e: Exception) {
            try { mifare.close() } catch (_: Exception) {}
            return null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         مساعدات
    // ═══════════════════════════════════════════════════════════════
    
    private fun parseNdefRecord(record: NdefRecord): NfcRecord {
        return when (record.tnf) {
            NdefRecord.TNF_WELL_KNOWN -> {
                if (record.type.contentEquals(NdefRecord.RTD_TEXT)) {
                    val payload = record.payload
                    val languageCodeLength = payload[0].toInt() and 0x3F
                    val languageCode = String(payload, 1, languageCodeLength, Charsets.US_ASCII)
                    val text = String(payload, 1 + languageCodeLength, 
                        payload.size - 1 - languageCodeLength, Charsets.UTF_8)
                    NfcRecord(RecordType.TEXT, text, languageCode)
                } else if (record.type.contentEquals(NdefRecord.RTD_URI)) {
                    NfcRecord(RecordType.URI, record.toUri().toString())
                } else {
                    NfcRecord(RecordType.UNKNOWN, String(record.payload, Charsets.UTF_8))
                }
            }
            NdefRecord.TNF_MIME_MEDIA -> {
                NfcRecord(
                    RecordType.MIME,
                    String(record.payload, Charsets.UTF_8),
                    mimeType = String(record.type, Charsets.US_ASCII)
                )
            }
            NdefRecord.TNF_EXTERNAL_TYPE -> {
                NfcRecord(
                    RecordType.EXTERNAL,
                    String(record.payload, Charsets.UTF_8),
                    mimeType = String(record.type, Charsets.US_ASCII)
                )
            }
            else -> NfcRecord(RecordType.UNKNOWN, String(record.payload, Charsets.UTF_8))
        }
    }
    
    private fun createTextRecord(text: String, languageCode: String): NdefRecord {
        val langBytes = languageCode.toByteArray(Charsets.US_ASCII)
        val textBytes = text.toByteArray(Charsets.UTF_8)
        
        val payload = ByteArray(1 + langBytes.size + textBytes.size)
        payload[0] = langBytes.size.toByte()
        System.arraycopy(langBytes, 0, payload, 1, langBytes.size)
        System.arraycopy(textBytes, 0, payload, 1 + langBytes.size, textBytes.size)
        
        return NdefRecord(NdefRecord.TNF_WELL_KNOWN, NdefRecord.RTD_TEXT, ByteArray(0), payload)
    }
    
    private fun ByteArray.toHexString(): String {
        return joinToString("") { String.format("%02X", it) }
    }
}
