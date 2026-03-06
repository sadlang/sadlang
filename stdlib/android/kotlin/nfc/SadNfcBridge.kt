// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadNfcBridge.kt
// الوصف: جسر Kotlin لـ NFC
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.nfc.NdefMessage
import android.nfc.NdefRecord
import android.nfc.NfcAdapter
import android.nfc.Tag
import android.nfc.tech.Ndef
import android.os.Build
import java.nio.charset.Charset

/**
 * جسر NFC — SadNfcBridge
 */
object SadNfcBridge {
    
    private var context: Context? = null
    private var nfcAdapter: NfcAdapter? = null
    private var lastTag: Tag? = null
    private var onTagDiscovered: ((Map<String, Any>) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        nfcAdapter = NfcAdapter.getDefaultAdapter(ctx)
    }
    
    @JvmStatic
    fun isSupported(): Boolean = nfcAdapter != null
    
    @JvmStatic
    fun isEnabled(): Boolean = nfcAdapter?.isEnabled == true
    
    @JvmStatic
    fun enableForegroundDispatch(activity: Activity) {
        val adapter = nfcAdapter ?: return
        
        val intent = Intent(activity, activity.javaClass).apply {
            addFlags(Intent.FLAG_ACTIVITY_SINGLE_TOP)
        }
        val flags = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_MUTABLE
        } else {
            PendingIntent.FLAG_UPDATE_CURRENT
        }
        val pendingIntent = PendingIntent.getActivity(activity, 0, intent, flags)
        
        val filters = arrayOf(
            IntentFilter(NfcAdapter.ACTION_NDEF_DISCOVERED),
            IntentFilter(NfcAdapter.ACTION_TAG_DISCOVERED),
            IntentFilter(NfcAdapter.ACTION_TECH_DISCOVERED)
        )
        
        adapter.enableForegroundDispatch(activity, pendingIntent, filters, null)
    }
    
    @JvmStatic
    fun disableForegroundDispatch(activity: Activity) {
        nfcAdapter?.disableForegroundDispatch(activity)
    }
    
    @JvmStatic
    fun handleIntent(intent: Intent): Map<String, Any>? {
        val action = intent.action ?: return null
        
        if (action != NfcAdapter.ACTION_NDEF_DISCOVERED &&
            action != NfcAdapter.ACTION_TAG_DISCOVERED &&
            action != NfcAdapter.ACTION_TECH_DISCOVERED) {
            return null
        }
        
        val tag = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            intent.getParcelableExtra(NfcAdapter.EXTRA_TAG, Tag::class.java)
        } else {
            @Suppress("DEPRECATION")
            intent.getParcelableExtra(NfcAdapter.EXTRA_TAG)
        } ?: return null
        
        lastTag = tag
        
        val result = mutableMapOf<String, Any>(
            "id" to tag.id.toHexString(),
            "techList" to tag.techList.toList()
        )
        
        // قراءة NDEF
        val ndef = Ndef.get(tag)
        if (ndef != null) {
            try {
                ndef.connect()
                val message = ndef.ndefMessage
                if (message != null) {
                    result["records"] = parseNdefMessage(message)
                }
                ndef.close()
            } catch (e: Exception) {
                result["error"] = e.message ?: "خطأ في قراءة NFC"
            }
        }
        
        onTagDiscovered?.invoke(result)
        return result
    }
    
    private fun parseNdefMessage(message: NdefMessage): List<Map<String, Any>> {
        return message.records.map { record ->
            mapOf(
                "tnf" to record.tnf,
                "type" to String(record.type, Charset.forName("UTF-8")),
                "payload" to String(record.payload, Charset.forName("UTF-8")),
                "payloadHex" to record.payload.toHexString()
            )
        }
    }
    
    @JvmStatic
    fun writeText(text: String): Boolean {
        val tag = lastTag ?: return false
        val ndef = Ndef.get(tag) ?: return false
        
        return try {
            val record = NdefRecord.createTextRecord("ar", text)
            val message = NdefMessage(arrayOf(record))
            
            ndef.connect()
            if (!ndef.isWritable) {
                ndef.close()
                return false
            }
            ndef.writeNdefMessage(message)
            ndef.close()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun writeUri(uri: String): Boolean {
        val tag = lastTag ?: return false
        val ndef = Ndef.get(tag) ?: return false
        
        return try {
            val record = NdefRecord.createUri(uri)
            val message = NdefMessage(arrayOf(record))
            
            ndef.connect()
            ndef.writeNdefMessage(message)
            ndef.close()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun setOnTagDiscovered(callback: (Map<String, Any>) -> Unit) {
        onTagDiscovered = callback
    }
    
    private fun ByteArray.toHexString(): String = 
        joinToString("") { "%02X".format(it) }
}
