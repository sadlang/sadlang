// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadCryptoBridge.kt
// الوصف: جسر Kotlin للتشفير والأمان
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.os.Build
import android.security.keystore.KeyGenParameterSpec
import android.security.keystore.KeyProperties
import android.util.Base64
import java.nio.charset.StandardCharsets
import java.security.*
import java.security.spec.PKCS8EncodedKeySpec
import java.security.spec.X509EncodedKeySpec
import javax.crypto.*
import javax.crypto.spec.GCMParameterSpec
import javax.crypto.spec.IvParameterSpec
import javax.crypto.spec.PBEKeySpec
import javax.crypto.spec.SecretKeySpec

/**
 * جسر التشفير — SadCryptoBridge
 */
object SadCryptoBridge {
    
    private const val ANDROID_KEYSTORE = "AndroidKeyStore"
    private const val AES_TRANSFORMATION = "AES/GCM/NoPadding"
    private const val AES_CBC_TRANSFORMATION = "AES/CBC/PKCS5Padding"
    private const val RSA_TRANSFORMATION = "RSA/ECB/PKCS1Padding"
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Hashing
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun md5(text: String): String {
        return hash(text, "MD5")
    }
    
    @JvmStatic
    fun sha1(text: String): String {
        return hash(text, "SHA-1")
    }
    
    @JvmStatic
    fun sha256(text: String): String {
        return hash(text, "SHA-256")
    }
    
    @JvmStatic
    fun sha512(text: String): String {
        return hash(text, "SHA-512")
    }
    
    @JvmStatic
    fun hash(text: String, algorithm: String): String {
        return try {
            val digest = MessageDigest.getInstance(algorithm)
            val bytes = digest.digest(text.toByteArray(StandardCharsets.UTF_8))
            bytesToHex(bytes)
        } catch (e: Exception) {
            ""
        }
    }
    
    @JvmStatic
    fun hashBytes(data: ByteArray, algorithm: String): ByteArray {
        return try {
            val digest = MessageDigest.getInstance(algorithm)
            digest.digest(data)
        } catch (e: Exception) {
            ByteArray(0)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // HMAC
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun hmacSha256(text: String, key: String): String {
        return hmac(text, key, "HmacSHA256")
    }
    
    @JvmStatic
    fun hmacSha512(text: String, key: String): String {
        return hmac(text, key, "HmacSHA512")
    }
    
    @JvmStatic
    fun hmac(text: String, key: String, algorithm: String): String {
        return try {
            val mac = Mac.getInstance(algorithm)
            val secretKey = SecretKeySpec(key.toByteArray(StandardCharsets.UTF_8), algorithm)
            mac.init(secretKey)
            val bytes = mac.doFinal(text.toByteArray(StandardCharsets.UTF_8))
            bytesToHex(bytes)
        } catch (e: Exception) {
            ""
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // AES Encryption
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun aesEncrypt(plaintext: String, key: String): String? {
        return try {
            val keyBytes = deriveKey(key, 32)
            val secretKey = SecretKeySpec(keyBytes, "AES")
            
            val cipher = Cipher.getInstance(AES_TRANSFORMATION)
            cipher.init(Cipher.ENCRYPT_MODE, secretKey)
            
            val iv = cipher.iv
            val encrypted = cipher.doFinal(plaintext.toByteArray(StandardCharsets.UTF_8))
            
            // Combine IV and encrypted data
            val combined = ByteArray(iv.size + encrypted.size)
            System.arraycopy(iv, 0, combined, 0, iv.size)
            System.arraycopy(encrypted, 0, combined, iv.size, encrypted.size)
            
            Base64.encodeToString(combined, Base64.NO_WRAP)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun aesDecrypt(ciphertext: String, key: String): String? {
        return try {
            val combined = Base64.decode(ciphertext, Base64.NO_WRAP)
            
            val keyBytes = deriveKey(key, 32)
            val secretKey = SecretKeySpec(keyBytes, "AES")
            
            // Extract IV (12 bytes for GCM)
            val iv = combined.copyOfRange(0, 12)
            val encrypted = combined.copyOfRange(12, combined.size)
            
            val cipher = Cipher.getInstance(AES_TRANSFORMATION)
            val spec = GCMParameterSpec(128, iv)
            cipher.init(Cipher.DECRYPT_MODE, secretKey, spec)
            
            val decrypted = cipher.doFinal(encrypted)
            String(decrypted, StandardCharsets.UTF_8)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun aesEncryptCBC(plaintext: String, key: String, iv: String): String? {
        return try {
            val keyBytes = deriveKey(key, 16)
            val ivBytes = deriveKey(iv, 16)
            
            val secretKey = SecretKeySpec(keyBytes, "AES")
            val ivSpec = IvParameterSpec(ivBytes)
            
            val cipher = Cipher.getInstance(AES_CBC_TRANSFORMATION)
            cipher.init(Cipher.ENCRYPT_MODE, secretKey, ivSpec)
            
            val encrypted = cipher.doFinal(plaintext.toByteArray(StandardCharsets.UTF_8))
            Base64.encodeToString(encrypted, Base64.NO_WRAP)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun aesDecryptCBC(ciphertext: String, key: String, iv: String): String? {
        return try {
            val keyBytes = deriveKey(key, 16)
            val ivBytes = deriveKey(iv, 16)
            
            val secretKey = SecretKeySpec(keyBytes, "AES")
            val ivSpec = IvParameterSpec(ivBytes)
            
            val cipher = Cipher.getInstance(AES_CBC_TRANSFORMATION)
            cipher.init(Cipher.DECRYPT_MODE, secretKey, ivSpec)
            
            val encrypted = Base64.decode(ciphertext, Base64.NO_WRAP)
            val decrypted = cipher.doFinal(encrypted)
            String(decrypted, StandardCharsets.UTF_8)
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // RSA Encryption
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun generateRsaKeyPair(keySize: Int = 2048): Map<String, String>? {
        return try {
            val keyPairGenerator = KeyPairGenerator.getInstance("RSA")
            keyPairGenerator.initialize(keySize)
            val keyPair = keyPairGenerator.generateKeyPair()
            
            mapOf(
                "publicKey" to Base64.encodeToString(keyPair.public.encoded, Base64.NO_WRAP),
                "privateKey" to Base64.encodeToString(keyPair.private.encoded, Base64.NO_WRAP)
            )
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun rsaEncrypt(plaintext: String, publicKeyBase64: String): String? {
        return try {
            val publicKeyBytes = Base64.decode(publicKeyBase64, Base64.NO_WRAP)
            val keySpec = X509EncodedKeySpec(publicKeyBytes)
            val keyFactory = KeyFactory.getInstance("RSA")
            val publicKey = keyFactory.generatePublic(keySpec)
            
            val cipher = Cipher.getInstance(RSA_TRANSFORMATION)
            cipher.init(Cipher.ENCRYPT_MODE, publicKey)
            
            val encrypted = cipher.doFinal(plaintext.toByteArray(StandardCharsets.UTF_8))
            Base64.encodeToString(encrypted, Base64.NO_WRAP)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun rsaDecrypt(ciphertext: String, privateKeyBase64: String): String? {
        return try {
            val privateKeyBytes = Base64.decode(privateKeyBase64, Base64.NO_WRAP)
            val keySpec = PKCS8EncodedKeySpec(privateKeyBytes)
            val keyFactory = KeyFactory.getInstance("RSA")
            val privateKey = keyFactory.generatePrivate(keySpec)
            
            val cipher = Cipher.getInstance(RSA_TRANSFORMATION)
            cipher.init(Cipher.DECRYPT_MODE, privateKey)
            
            val encrypted = Base64.decode(ciphertext, Base64.NO_WRAP)
            val decrypted = cipher.doFinal(encrypted)
            String(decrypted, StandardCharsets.UTF_8)
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Digital Signatures
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun sign(data: String, privateKeyBase64: String, algorithm: String = "SHA256withRSA"): String? {
        return try {
            val privateKeyBytes = Base64.decode(privateKeyBase64, Base64.NO_WRAP)
            val keySpec = PKCS8EncodedKeySpec(privateKeyBytes)
            val keyFactory = KeyFactory.getInstance("RSA")
            val privateKey = keyFactory.generatePrivate(keySpec)
            
            val signature = Signature.getInstance(algorithm)
            signature.initSign(privateKey)
            signature.update(data.toByteArray(StandardCharsets.UTF_8))
            
            val signatureBytes = signature.sign()
            Base64.encodeToString(signatureBytes, Base64.NO_WRAP)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun verify(
        data: String,
        signatureBase64: String,
        publicKeyBase64: String,
        algorithm: String = "SHA256withRSA"
    ): Boolean {
        return try {
            val publicKeyBytes = Base64.decode(publicKeyBase64, Base64.NO_WRAP)
            val keySpec = X509EncodedKeySpec(publicKeyBytes)
            val keyFactory = KeyFactory.getInstance("RSA")
            val publicKey = keyFactory.generatePublic(keySpec)
            
            val signature = Signature.getInstance(algorithm)
            signature.initVerify(publicKey)
            signature.update(data.toByteArray(StandardCharsets.UTF_8))
            
            val signatureBytes = Base64.decode(signatureBase64, Base64.NO_WRAP)
            signature.verify(signatureBytes)
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Android Keystore
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun generateKeystoreKey(alias: String): Boolean {
        return try {
            if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
                return false
            }
            
            val keyGenerator = KeyGenerator.getInstance(
                KeyProperties.KEY_ALGORITHM_AES,
                ANDROID_KEYSTORE
            )
            
            val spec = KeyGenParameterSpec.Builder(
                alias,
                KeyProperties.PURPOSE_ENCRYPT or KeyProperties.PURPOSE_DECRYPT
            )
                .setBlockModes(KeyProperties.BLOCK_MODE_GCM)
                .setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_NONE)
                .setKeySize(256)
                .build()
            
            keyGenerator.init(spec)
            keyGenerator.generateKey()
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun encryptWithKeystore(plaintext: String, alias: String): String? {
        return try {
            val keyStore = KeyStore.getInstance(ANDROID_KEYSTORE)
            keyStore.load(null)
            
            val secretKey = keyStore.getKey(alias, null) as? SecretKey ?: return null
            
            val cipher = Cipher.getInstance("AES/GCM/NoPadding")
            cipher.init(Cipher.ENCRYPT_MODE, secretKey)
            
            val iv = cipher.iv
            val encrypted = cipher.doFinal(plaintext.toByteArray(StandardCharsets.UTF_8))
            
            val combined = ByteArray(iv.size + encrypted.size)
            System.arraycopy(iv, 0, combined, 0, iv.size)
            System.arraycopy(encrypted, 0, combined, iv.size, encrypted.size)
            
            Base64.encodeToString(combined, Base64.NO_WRAP)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun decryptWithKeystore(ciphertext: String, alias: String): String? {
        return try {
            val keyStore = KeyStore.getInstance(ANDROID_KEYSTORE)
            keyStore.load(null)
            
            val secretKey = keyStore.getKey(alias, null) as? SecretKey ?: return null
            
            val combined = Base64.decode(ciphertext, Base64.NO_WRAP)
            val iv = combined.copyOfRange(0, 12)
            val encrypted = combined.copyOfRange(12, combined.size)
            
            val cipher = Cipher.getInstance("AES/GCM/NoPadding")
            val spec = GCMParameterSpec(128, iv)
            cipher.init(Cipher.DECRYPT_MODE, secretKey, spec)
            
            val decrypted = cipher.doFinal(encrypted)
            String(decrypted, StandardCharsets.UTF_8)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun deleteKeystoreKey(alias: String): Boolean {
        return try {
            val keyStore = KeyStore.getInstance(ANDROID_KEYSTORE)
            keyStore.load(null)
            keyStore.deleteEntry(alias)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun keystoreKeyExists(alias: String): Boolean {
        return try {
            val keyStore = KeyStore.getInstance(ANDROID_KEYSTORE)
            keyStore.load(null)
            keyStore.containsAlias(alias)
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun listKeystoreAliases(): List<String> {
        return try {
            val keyStore = KeyStore.getInstance(ANDROID_KEYSTORE)
            keyStore.load(null)
            keyStore.aliases().toList()
        } catch (e: Exception) {
            emptyList()
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Password Hashing (PBKDF2)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun hashPassword(password: String, salt: String, iterations: Int = 10000): String? {
        return try {
            val spec = PBEKeySpec(
                password.toCharArray(),
                salt.toByteArray(StandardCharsets.UTF_8),
                iterations,
                256
            )
            val factory = SecretKeyFactory.getInstance("PBKDF2WithHmacSHA256")
            val hash = factory.generateSecret(spec).encoded
            Base64.encodeToString(hash, Base64.NO_WRAP)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun verifyPassword(password: String, salt: String, hash: String, iterations: Int = 10000): Boolean {
        val computedHash = hashPassword(password, salt, iterations)
        return computedHash == hash
    }
    
    @JvmStatic
    fun generateSalt(length: Int = 16): String {
        val random = SecureRandom()
        val salt = ByteArray(length)
        random.nextBytes(salt)
        return Base64.encodeToString(salt, Base64.NO_WRAP)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Random
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun generateSecureRandomBytes(length: Int): ByteArray {
        val random = SecureRandom()
        val bytes = ByteArray(length)
        random.nextBytes(bytes)
        return bytes
    }
    
    @JvmStatic
    fun generateSecureRandomHex(length: Int): String {
        return bytesToHex(generateSecureRandomBytes(length))
    }
    
    @JvmStatic
    fun generateSecureRandomBase64(length: Int): String {
        return Base64.encodeToString(generateSecureRandomBytes(length), Base64.NO_WRAP)
    }
    
    @JvmStatic
    fun generateUuid(): String {
        return java.util.UUID.randomUUID().toString()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Base64
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun base64Encode(text: String): String {
        return Base64.encodeToString(text.toByteArray(StandardCharsets.UTF_8), Base64.NO_WRAP)
    }
    
    @JvmStatic
    fun base64Decode(encoded: String): String {
        return String(Base64.decode(encoded, Base64.NO_WRAP), StandardCharsets.UTF_8)
    }
    
    @JvmStatic
    fun base64EncodeBytes(bytes: ByteArray): String {
        return Base64.encodeToString(bytes, Base64.NO_WRAP)
    }
    
    @JvmStatic
    fun base64DecodeToBytes(encoded: String): ByteArray {
        return Base64.decode(encoded, Base64.NO_WRAP)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Helpers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun bytesToHex(bytes: ByteArray): String {
        return bytes.joinToString("") { "%02x".format(it) }
    }
    
    private fun deriveKey(password: String, length: Int): ByteArray {
        val digest = MessageDigest.getInstance("SHA-256")
        val hash = digest.digest(password.toByteArray(StandardCharsets.UTF_8))
        return hash.copyOf(length)
    }
}
