// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadQRCodeBridge.kt
// الوصف: جسر Kotlin لإنشاء وقراءة رموز QR
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Color
import com.google.zxing.*
import com.google.zxing.common.BitMatrix
import com.google.zxing.common.HybridBinarizer
import com.google.zxing.qrcode.QRCodeWriter
import com.google.zxing.qrcode.decoder.ErrorCorrectionLevel
import java.io.File
import java.io.FileOutputStream

/**
 * جسر رموز QR — SadQRCodeBridge
 */
object SadQRCodeBridge {
    
    private var context: Context? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Generate QR Code
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun generateQRCode(
        content: String,
        width: Int = 512,
        height: Int = 512,
        foregroundColor: Int = Color.BLACK,
        backgroundColor: Int = Color.WHITE,
        errorCorrectionLevel: String = "M"
    ): Bitmap? {
        return try {
            val hints = mutableMapOf<EncodeHintType, Any>()
            hints[EncodeHintType.CHARACTER_SET] = "UTF-8"
            hints[EncodeHintType.ERROR_CORRECTION] = when (errorCorrectionLevel.uppercase()) {
                "L" -> ErrorCorrectionLevel.L // ~7% correction
                "M" -> ErrorCorrectionLevel.M // ~15% correction
                "Q" -> ErrorCorrectionLevel.Q // ~25% correction
                "H" -> ErrorCorrectionLevel.H // ~30% correction
                else -> ErrorCorrectionLevel.M
            }
            hints[EncodeHintType.MARGIN] = 1
            
            val writer = QRCodeWriter()
            val bitMatrix = writer.encode(content, BarcodeFormat.QR_CODE, width, height, hints)
            
            createBitmapFromBitMatrix(bitMatrix, foregroundColor, backgroundColor)
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun generateQRCodeArabic(
        content: String,
        width: Int = 512,
        height: Int = 512
    ): Bitmap? {
        // QR codes already support UTF-8, which includes Arabic
        return generateQRCode(content, width, height)
    }
    
    @JvmStatic
    fun generateQRCodeWithLogo(
        content: String,
        logo: Bitmap,
        width: Int = 512,
        height: Int = 512,
        logoSizePercent: Float = 0.2f
    ): Bitmap? {
        // Use high error correction for logo overlay
        val qrCode = generateQRCode(content, width, height, errorCorrectionLevel = "H")
            ?: return null
        
        return try {
            val combined = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
            val canvas = android.graphics.Canvas(combined)
            
            // Draw QR code
            canvas.drawBitmap(qrCode, 0f, 0f, null)
            
            // Calculate logo dimensions
            val logoSize = (minOf(width, height) * logoSizePercent).toInt()
            val scaledLogo = Bitmap.createScaledBitmap(logo, logoSize, logoSize, true)
            
            // Draw logo in center
            val left = (width - logoSize) / 2f
            val top = (height - logoSize) / 2f
            canvas.drawBitmap(scaledLogo, left, top, null)
            
            combined
        } catch (e: Exception) {
            qrCode
        }
    }
    
    @JvmStatic
    fun saveQRCodeToFile(
        bitmap: Bitmap,
        filePath: String,
        format: String = "PNG",
        quality: Int = 100
    ): Boolean {
        return try {
            val file = File(filePath)
            file.parentFile?.mkdirs()
            
            val compressFormat = when (format.uppercase()) {
                "JPEG", "JPG" -> Bitmap.CompressFormat.JPEG
                "WEBP" -> Bitmap.CompressFormat.WEBP
                else -> Bitmap.CompressFormat.PNG
            }
            
            FileOutputStream(file).use { out ->
                bitmap.compress(compressFormat, quality, out)
            }
            true
        } catch (e: Exception) {
            false
        }
    }
    
    private fun createBitmapFromBitMatrix(
        matrix: BitMatrix,
        foregroundColor: Int,
        backgroundColor: Int
    ): Bitmap {
        val width = matrix.width
        val height = matrix.height
        val pixels = IntArray(width * height)
        
        for (y in 0 until height) {
            val offset = y * width
            for (x in 0 until width) {
                pixels[offset + x] = if (matrix[x, y]) foregroundColor else backgroundColor
            }
        }
        
        val bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888)
        bitmap.setPixels(pixels, 0, width, 0, 0, width, height)
        return bitmap
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Generate Barcode (1D)
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun generateBarcode(
        content: String,
        format: String = "CODE_128",
        width: Int = 600,
        height: Int = 200,
        foregroundColor: Int = Color.BLACK,
        backgroundColor: Int = Color.WHITE
    ): Bitmap? {
        return try {
            val barcodeFormat = when (format.uppercase()) {
                "CODE_128" -> BarcodeFormat.CODE_128
                "CODE_39" -> BarcodeFormat.CODE_39
                "EAN_13" -> BarcodeFormat.EAN_13
                "EAN_8" -> BarcodeFormat.EAN_8
                "UPC_A" -> BarcodeFormat.UPC_A
                "UPC_E" -> BarcodeFormat.UPC_E
                "ITF" -> BarcodeFormat.ITF
                "CODABAR" -> BarcodeFormat.CODABAR
                else -> BarcodeFormat.CODE_128
            }
            
            val writer = MultiFormatWriter()
            val bitMatrix = writer.encode(content, barcodeFormat, width, height)
            
            createBitmapFromBitMatrix(bitMatrix, foregroundColor, backgroundColor)
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Read QR Code
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun readQRCode(bitmap: Bitmap): String? {
        return try {
            val width = bitmap.width
            val height = bitmap.height
            val pixels = IntArray(width * height)
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height)
            
            val source = RGBLuminanceSource(width, height, pixels)
            val binaryBitmap = BinaryBitmap(HybridBinarizer(source))
            
            val reader = MultiFormatReader()
            val hints = mapOf(
                DecodeHintType.TRY_HARDER to true,
                DecodeHintType.CHARACTER_SET to "UTF-8",
                DecodeHintType.POSSIBLE_FORMATS to listOf(BarcodeFormat.QR_CODE)
            )
            
            val result = reader.decode(binaryBitmap, hints)
            result.text
        } catch (e: NotFoundException) {
            null
        } catch (e: Exception) {
            null
        }
    }
    
    @JvmStatic
    fun readBarcode(bitmap: Bitmap): Map<String, String>? {
        return try {
            val width = bitmap.width
            val height = bitmap.height
            val pixels = IntArray(width * height)
            bitmap.getPixels(pixels, 0, width, 0, 0, width, height)
            
            val source = RGBLuminanceSource(width, height, pixels)
            val binaryBitmap = BinaryBitmap(HybridBinarizer(source))
            
            val reader = MultiFormatReader()
            val hints = mapOf(
                DecodeHintType.TRY_HARDER to true,
                DecodeHintType.CHARACTER_SET to "UTF-8"
            )
            
            val result = reader.decode(binaryBitmap, hints)
            
            mapOf(
                "text" to result.text,
                "format" to result.barcodeFormat.name
            )
        } catch (e: NotFoundException) {
            null
        } catch (e: Exception) {
            null
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Content Helpers
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createWifiContent(
        ssid: String,
        password: String,
        securityType: String = "WPA"
    ): String {
        val security = when (securityType.uppercase()) {
            "WPA", "WPA2" -> "WPA"
            "WEP" -> "WEP"
            "OPEN", "NONE" -> "nopass"
            else -> "WPA"
        }
        
        return "WIFI:T:$security;S:$ssid;P:$password;;"
    }
    
    @JvmStatic
    fun createContactContent(
        name: String,
        phone: String? = null,
        email: String? = null,
        organization: String? = null,
        address: String? = null,
        url: String? = null
    ): String {
        val vCard = StringBuilder()
        vCard.append("BEGIN:VCARD\n")
        vCard.append("VERSION:3.0\n")
        vCard.append("N:$name\n")
        vCard.append("FN:$name\n")
        phone?.let { vCard.append("TEL:$it\n") }
        email?.let { vCard.append("EMAIL:$it\n") }
        organization?.let { vCard.append("ORG:$it\n") }
        address?.let { vCard.append("ADR:$it\n") }
        url?.let { vCard.append("URL:$it\n") }
        vCard.append("END:VCARD")
        return vCard.toString()
    }
    
    @JvmStatic
    fun createPhoneContent(phoneNumber: String): String {
        return "tel:$phoneNumber"
    }
    
    @JvmStatic
    fun createSmsContent(phoneNumber: String, message: String = ""): String {
        return if (message.isNotEmpty()) {
            "SMSTO:$phoneNumber:$message"
        } else {
            "SMSTO:$phoneNumber"
        }
    }
    
    @JvmStatic
    fun createEmailContent(
        email: String,
        subject: String = "",
        body: String = ""
    ): String {
        val sb = StringBuilder("mailto:$email")
        val params = mutableListOf<String>()
        
        if (subject.isNotEmpty()) params.add("subject=${java.net.URLEncoder.encode(subject, "UTF-8")}")
        if (body.isNotEmpty()) params.add("body=${java.net.URLEncoder.encode(body, "UTF-8")}")
        
        if (params.isNotEmpty()) {
            sb.append("?${params.joinToString("&")}")
        }
        
        return sb.toString()
    }
    
    @JvmStatic
    fun createGeoContent(latitude: Double, longitude: Double, label: String? = null): String {
        return if (label != null) {
            "geo:$latitude,$longitude?q=$latitude,$longitude(${java.net.URLEncoder.encode(label, "UTF-8")})"
        } else {
            "geo:$latitude,$longitude"
        }
    }
    
    @JvmStatic
    fun createEventContent(
        title: String,
        location: String = "",
        startTime: Long,
        endTime: Long,
        description: String = ""
    ): String {
        val dateFormat = java.text.SimpleDateFormat("yyyyMMdd'T'HHmmss'Z'", java.util.Locale.US)
        dateFormat.timeZone = java.util.TimeZone.getTimeZone("UTC")
        
        val sb = StringBuilder()
        sb.append("BEGIN:VEVENT\n")
        sb.append("SUMMARY:$title\n")
        sb.append("LOCATION:$location\n")
        sb.append("DTSTART:${dateFormat.format(java.util.Date(startTime))}\n")
        sb.append("DTEND:${dateFormat.format(java.util.Date(endTime))}\n")
        if (description.isNotEmpty()) {
            sb.append("DESCRIPTION:$description\n")
        }
        sb.append("END:VEVENT")
        return sb.toString()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getSupportedBarcodeFormats(): List<String> {
        return listOf(
            "QR_CODE",
            "CODE_128",
            "CODE_39",
            "EAN_13",
            "EAN_8",
            "UPC_A",
            "UPC_E",
            "ITF",
            "CODABAR"
        )
    }
    
    @JvmStatic
    fun getErrorCorrectionLevels(): List<Map<String, String>> {
        return listOf(
            mapOf("code" to "L", "name" to "Low", "nameAr" to "منخفض", "recovery" to "~7%"),
            mapOf("code" to "M", "name" to "Medium", "nameAr" to "متوسط", "recovery" to "~15%"),
            mapOf("code" to "Q", "name" to "Quartile", "nameAr" to "ربعي", "recovery" to "~25%"),
            mapOf("code" to "H", "name" to "High", "nameAr" to "عالي", "recovery" to "~30%")
        )
    }
}
