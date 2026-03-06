// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadCryptoBridge.kt
// الوصف: جسر Kotlin للتشفير في لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يربط دوال __اندرويد_تشفير_* بـ Java Crypto APIs
//
// الدوال المُنفّذة:
//   __اندرويد_تشفير_hash(text, algorithm) -> string
//   __اندرويد_تشفير_encrypt(text, key) -> string
//   __اندرويد_تشفير_decrypt(encrypted, key) -> string
//   __اندرويد_تشفير_random(length) -> string
//
// لا تحتاج اعتماديات إضافية (تستخدم Java Crypto المدمج)
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.os.Build
import android.security.keystore.KeyGenParameterSpec
import android.security.keystore.KeyProperties
import android.util.Base64
import java.security.KeyStore
import java.security.MessageDigest
import java.security.SecureRandom
import javax.crypto.Cipher
import javax.crypto.KeyGenerator
import javax.crypto.SecretKey
import javax.crypto.spec.GCMParameterSpec
import javax.crypto.spec.IvParameterSpec
import javax.crypto.spec.SecretKeySpec

/**
 * جسر التشفير — SadCryptoBridge
 * 
 * يوفر واجهة للتشفير وفك التشفير من كود لغة ص.
 * يدعم خوارزميات متعددة: SHA256, MD5, AES, RSA
 */
object SadCryptoBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // ثوابت
    // ═══════════════════════════════════════════════════════════════════════════
    
    private const val AES_KEY_SIZE = 256
    private const val GCM_IV_LENGTH = 12
    private const val GCM_TAG_LENGTH = 128
    private const val KEYSTORE_ALIAS = "sad_crypto_key"
    
    private val secureRandom = SecureRandom()
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_تشفير_hash — حساب Hash
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * حساب hash لنص
     * 
     * @param text النص
     * @param algorithm الخوارزمية (SHA-256, SHA-512, MD5, SHA-1)
     * @return hex string
     */
    @JvmStatic
    fun hash(text: String, algorithm: String = "SHA-256"): String {
        return try {
            val digest = MessageDigest.getInstance(algorithm)
            val bytes = digest.digest(text.toByteArray(Charsets.UTF_8))
            bytesToHex(bytes)
        } catch (e: Exception) {
            ""
        }
    }
    
    /**
     * SHA-256 hash
     */
    @JvmStatic
    fun sha256(text: String): String = hash(text, "SHA-256")
    
    /**
     * SHA-512 hash
     */
    @JvmStatic
    fun sha512(text: String): String = hash(text, "SHA-512")
    
    /**
     * MD5 hash (للتوافق فقط، غير آمن!)
     */
    @JvmStatic
    @Deprecated("MD5 غير آمن، استخدم SHA-256")
    fun md5(text: String): String = hash(text, "MD5")
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_تشفير_encrypt — تشفير AES-GCM
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تشفير نص بـ AES-GCM
     * 
     * @param plaintext النص الأصلي
     * @param key المفتاح (32 حرف = 256 بت)
     * @return النص المشفر (Base64: IV + ciphertext + tag)
     */
    @JvmStatic
    fun encrypt(plaintext: String, key: String): String {
        return try {
            val keyBytes = normalizeKey(key)
            val secretKey = SecretKeySpec(keyBytes, "AES")
            
            // توليد IV عشوائي
            val iv = ByteArray(GCM_IV_LENGTH)
            secureRandom.nextBytes(iv)
            
            val cipher = Cipher.getInstance("AES/GCM/NoPadding")
            cipher.init(Cipher.ENCRYPT_MODE, secretKey, GCMParameterSpec(GCM_TAG_LENGTH, iv))
            
            val ciphertext = cipher.doFinal(plaintext.toByteArray(Charsets.UTF_8))
            
            // IV + ciphertext
            val combined = ByteArray(iv.size + ciphertext.size)
            System.arraycopy(iv, 0, combined, 0, iv.size)
            System.arraycopy(ciphertext, 0, combined, iv.size, ciphertext.size)
            
            Base64.encodeToString(combined, Base64.NO_WRAP)
            
        } catch (e: Exception) {
            e.printStackTrace()
            ""
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_تشفير_decrypt — فك تشفير AES-GCM
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * فك تشفير نص
     * 
     * @param encrypted النص المشفر (Base64)
     * @param key المفتاح
     * @return النص الأصلي أو فارغ عند الفشل
     */
    @JvmStatic
    fun decrypt(encrypted: String, key: String): String {
        return try {
            val keyBytes = normalizeKey(key)
            val secretKey = SecretKeySpec(keyBytes, "AES")
            
            val combined = Base64.decode(encrypted, Base64.NO_WRAP)
            
            // استخراج IV
            val iv = ByteArray(GCM_IV_LENGTH)
            System.arraycopy(combined, 0, iv, 0, iv.size)
            
            // استخراج ciphertext
            val ciphertext = ByteArray(combined.size - iv.size)
            System.arraycopy(combined, iv.size, ciphertext, 0, ciphertext.size)
            
            val cipher = Cipher.getInstance("AES/GCM/NoPadding")
            cipher.init(Cipher.DECRYPT_MODE, secretKey, GCMParameterSpec(GCM_TAG_LENGTH, iv))
            
            String(cipher.doFinal(ciphertext), Charsets.UTF_8)
            
        } catch (e: Exception) {
            e.printStackTrace()
            ""
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_تشفير_random — توليد عشوائي آمن
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد نص عشوائي آمن
     * 
     * @param length الطول
     * @param useHex استخدام hex أو Base64
     * @return نص عشوائي
     */
    @JvmStatic
    fun randomString(length: Int = 32, useHex: Boolean = true): String {
        val bytes = ByteArray((length + 1) / 2)
        secureRandom.nextBytes(bytes)
        
        return if (useHex) {
            bytesToHex(bytes).take(length)
        } else {
            Base64.encodeToString(bytes, Base64.NO_WRAP).take(length)
        }
    }
    
    /**
     * توليد bytes عشوائية
     */
    @JvmStatic
    fun randomBytes(length: Int): ByteArray {
        val bytes = ByteArray(length)
        secureRandom.nextBytes(bytes)
        return bytes
    }
    
    /**
     * توليد رقم عشوائي
     */
    @JvmStatic
    fun randomInt(min: Int = 0, max: Int = Int.MAX_VALUE): Int {
        return secureRandom.nextInt(max - min) + min
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_تشفير_مفتاح — توليد مفتاح
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * توليد مفتاح AES جديد
     * 
     * @return المفتاح (hex)
     */
    @JvmStatic
    fun generateKey(): String {
        val keyGen = KeyGenerator.getInstance("AES")
        keyGen.init(AES_KEY_SIZE, secureRandom)
        return bytesToHex(keyGen.generateKey().encoded)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Android Keystore Integration
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تشفير باستخدام Android Keystore (أكثر أماناً)
     * المفتاح محمي بالأجهزة في الأجهزة الداعمة
     */
    @JvmStatic
    fun encryptWithKeystore(plaintext: String, alias: String = KEYSTORE_ALIAS): String {
        return try {
            val secretKey = getOrCreateKeystoreKey(alias)
            
            val cipher = Cipher.getInstance("AES/GCM/NoPadding")
            cipher.init(Cipher.ENCRYPT_MODE, secretKey)
            
            val iv = cipher.iv
            val ciphertext = cipher.doFinal(plaintext.toByteArray(Charsets.UTF_8))
            
            // IV + ciphertext
            val combined = ByteArray(iv.size + ciphertext.size)
            System.arraycopy(iv, 0, combined, 0, iv.size)
            System.arraycopy(ciphertext, 0, combined, iv.size, ciphertext.size)
            
            Base64.encodeToString(combined, Base64.NO_WRAP)
            
        } catch (e: Exception) {
            e.printStackTrace()
            ""
        }
    }
    
    /**
     * فك تشفير باستخدام Android Keystore
     */
    @JvmStatic
    fun decryptWithKeystore(encrypted: String, alias: String = KEYSTORE_ALIAS): String {
        return try {
            val keyStore = KeyStore.getInstance("AndroidKeyStore")
            keyStore.load(null)
            
            val secretKey = keyStore.getKey(alias, null) as? SecretKey
                ?: throw IllegalStateException("المفتاح غير موجود")
            
            val combined = Base64.decode(encrypted, Base64.NO_WRAP)
            
            val iv = ByteArray(GCM_IV_LENGTH)
            System.arraycopy(combined, 0, iv, 0, iv.size)
            
            val ciphertext = ByteArray(combined.size - iv.size)
            System.arraycopy(combined, iv.size, ciphertext, 0, ciphertext.size)
            
            val cipher = Cipher.getInstance("AES/GCM/NoPadding")
            cipher.init(Cipher.DECRYPT_MODE, secretKey, GCMParameterSpec(GCM_TAG_LENGTH, iv))
            
            String(cipher.doFinal(ciphertext), Charsets.UTF_8)
            
        } catch (e: Exception) {
            e.printStackTrace()
            ""
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تحويل bytes إلى hex
     */
    private fun bytesToHex(bytes: ByteArray): String {
        return bytes.joinToString("") { "%02x".format(it) }
    }
    
    /**
     * تحويل hex إلى bytes
     */
    private fun hexToBytes(hex: String): ByteArray {
        val len = hex.length
        val data = ByteArray(len / 2)
        for (i in 0 until len step 2) {
            data[i / 2] = ((Character.digit(hex[i], 16) shl 4) +
                    Character.digit(hex[i + 1], 16)).toByte()
        }
        return data
    }
    
    /**
     * تطبيع المفتاح لـ 256 بت
     */
    private fun normalizeKey(key: String): ByteArray {
        // إذا كان hex
        if (key.length == 64 && key.all { it.isDigit() || it in 'a'..'f' || it in 'A'..'F' }) {
            return hexToBytes(key)
        }
        
        // استخدام SHA-256 لتوليد مفتاح من أي نص
        val digest = MessageDigest.getInstance("SHA-256")
        return digest.digest(key.toByteArray(Charsets.UTF_8))
    }
    
    /**
     * إنشاء أو الحصول على مفتاح من Keystore
     */
    private fun getOrCreateKeystoreKey(alias: String): SecretKey {
        val keyStore = KeyStore.getInstance("AndroidKeyStore")
        keyStore.load(null)
        
        // التحقق من وجود المفتاح
        if (keyStore.containsAlias(alias)) {
            return keyStore.getKey(alias, null) as SecretKey
        }
        
        // إنشاء مفتاح جديد
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            val keyGen = KeyGenerator.getInstance(
                KeyProperties.KEY_ALGORITHM_AES,
                "AndroidKeyStore"
            )
            
            keyGen.init(
                KeyGenParameterSpec.Builder(
                    alias,
                    KeyProperties.PURPOSE_ENCRYPT or KeyProperties.PURPOSE_DECRYPT
                )
                    .setBlockModes(KeyProperties.BLOCK_MODE_GCM)
                    .setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_NONE)
                    .setKeySize(AES_KEY_SIZE)
                    .build()
            )
            
            return keyGen.generateKey()
        }
        
        throw UnsupportedOperationException("يتطلب Android 6.0+")
    }
}
