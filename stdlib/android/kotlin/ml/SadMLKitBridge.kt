// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadMLKitBridge.kt
// الوصف: جسر Kotlin لـ ML Kit (الذكاء الاصطناعي)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.graphics.Bitmap
import android.graphics.Rect
import android.net.Uri
import com.google.mlkit.vision.barcode.BarcodeScanner
import com.google.mlkit.vision.barcode.BarcodeScannerOptions
import com.google.mlkit.vision.barcode.BarcodeScanning
import com.google.mlkit.vision.barcode.common.Barcode
import com.google.mlkit.vision.common.InputImage
import com.google.mlkit.vision.face.Face
import com.google.mlkit.vision.face.FaceDetection
import com.google.mlkit.vision.face.FaceDetector
import com.google.mlkit.vision.face.FaceDetectorOptions
import com.google.mlkit.vision.label.ImageLabel
import com.google.mlkit.vision.label.ImageLabeler
import com.google.mlkit.vision.label.ImageLabeling
import com.google.mlkit.vision.label.defaults.ImageLabelerOptions
import com.google.mlkit.vision.text.Text
import com.google.mlkit.vision.text.TextRecognition
import com.google.mlkit.vision.text.TextRecognizer
import com.google.mlkit.vision.text.arabic.ArabicTextRecognizerOptions
import com.google.mlkit.vision.text.latin.TextRecognizerOptions
import java.io.IOException

/**
 * جسر ML Kit — SadMLKitBridge
 */
object SadMLKitBridge {
    
    private var context: Context? = null
    
    // Recognizers
    private var textRecognizer: TextRecognizer? = null
    private var arabicTextRecognizer: TextRecognizer? = null
    private var faceDetector: FaceDetector? = null
    private var barcodeScanner: BarcodeScanner? = null
    private var imageLabeler: ImageLabeler? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Text Recognition
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun recognizeText(
        bitmap: Bitmap,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        val recognizer = textRecognizer ?: TextRecognition.getClient(TextRecognizerOptions.DEFAULT_OPTIONS).also {
            textRecognizer = it
        }
        
        val image = InputImage.fromBitmap(bitmap, 0)
        
        recognizer.process(image)
            .addOnSuccessListener { result ->
                onSuccess(result.toMap())
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في التعرف على النص")
            }
    }
    
    @JvmStatic
    fun recognizeArabicText(
        bitmap: Bitmap,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        val recognizer = arabicTextRecognizer ?: TextRecognition.getClient(ArabicTextRecognizerOptions.Builder().build()).also {
            arabicTextRecognizer = it
        }
        
        val image = InputImage.fromBitmap(bitmap, 0)
        
        recognizer.process(image)
            .addOnSuccessListener { result ->
                onSuccess(result.toMap())
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في التعرف على النص العربي")
            }
    }
    
    @JvmStatic
    fun recognizeTextFromUri(
        uriString: String,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        val ctx = context ?: run {
            onError("لم يتم تهيئة السياق")
            return
        }
        
        try {
            val image = InputImage.fromFilePath(ctx, Uri.parse(uriString))
            val recognizer = textRecognizer ?: TextRecognition.getClient(TextRecognizerOptions.DEFAULT_OPTIONS).also {
                textRecognizer = it
            }
            
            recognizer.process(image)
                .addOnSuccessListener { result ->
                    onSuccess(result.toMap())
                }
                .addOnFailureListener { e ->
                    onError(e.localizedMessage ?: "فشل في التعرف على النص")
                }
        } catch (e: IOException) {
            onError("فشل في قراءة الصورة: ${e.message}")
        }
    }
    
    private fun Text.toMap(): Map<String, Any?> {
        return mapOf(
            "text" to text,
            "blocks" to textBlocks.map { block ->
                mapOf(
                    "text" to block.text,
                    "boundingBox" to block.boundingBox?.toMap(),
                    "cornerPoints" to block.cornerPoints?.map { listOf(it.x, it.y) },
                    "recognizedLanguages" to block.recognizedLanguages.map { it.languageCode },
                    "lines" to block.lines.map { line ->
                        mapOf(
                            "text" to line.text,
                            "boundingBox" to line.boundingBox?.toMap(),
                            "elements" to line.elements.map { element ->
                                mapOf(
                                    "text" to element.text,
                                    "boundingBox" to element.boundingBox?.toMap()
                                )
                            }
                        )
                    }
                )
            }
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Face Detection
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun detectFaces(
        bitmap: Bitmap,
        performanceMode: String = "fast",
        detectLandmarks: Boolean = false,
        detectContours: Boolean = false,
        classifyFaces: Boolean = false,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        val options = FaceDetectorOptions.Builder()
            .setPerformanceMode(
                if (performanceMode == "accurate") FaceDetectorOptions.PERFORMANCE_MODE_ACCURATE
                else FaceDetectorOptions.PERFORMANCE_MODE_FAST
            )
            .apply {
                if (detectLandmarks) setLandmarkMode(FaceDetectorOptions.LANDMARK_MODE_ALL)
                if (detectContours) setContourMode(FaceDetectorOptions.CONTOUR_MODE_ALL)
                if (classifyFaces) setClassificationMode(FaceDetectorOptions.CLASSIFICATION_MODE_ALL)
            }
            .build()
        
        val detector = FaceDetection.getClient(options)
        val image = InputImage.fromBitmap(bitmap, 0)
        
        detector.process(image)
            .addOnSuccessListener { faces ->
                onSuccess(faces.map { it.toMap() })
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في كشف الوجوه")
            }
    }
    
    private fun Face.toMap(): Map<String, Any?> {
        return mapOf(
            "boundingBox" to boundingBox.toMap(),
            "trackingId" to trackingId,
            "headEulerAngleX" to headEulerAngleX,
            "headEulerAngleY" to headEulerAngleY,
            "headEulerAngleZ" to headEulerAngleZ,
            "smilingProbability" to smilingProbability,
            "leftEyeOpenProbability" to leftEyeOpenProbability,
            "rightEyeOpenProbability" to rightEyeOpenProbability,
            "isSmiling" to (smilingProbability?.let { it > 0.5 } ?: false),
            "isLeftEyeOpen" to (leftEyeOpenProbability?.let { it > 0.5 } ?: true),
            "isRightEyeOpen" to (rightEyeOpenProbability?.let { it > 0.5 } ?: true)
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Barcode Scanning
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun scanBarcodes(
        bitmap: Bitmap,
        formats: List<Int>? = null,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        val options = if (formats != null && formats.isNotEmpty()) {
            BarcodeScannerOptions.Builder()
                .setBarcodeFormats(formats.first(), *formats.drop(1).toIntArray())
                .build()
        } else {
            BarcodeScannerOptions.Builder()
                .setBarcodeFormats(Barcode.FORMAT_ALL_FORMATS)
                .build()
        }
        
        val scanner = BarcodeScanning.getClient(options)
        val image = InputImage.fromBitmap(bitmap, 0)
        
        scanner.process(image)
            .addOnSuccessListener { barcodes ->
                onSuccess(barcodes.map { it.toMap() })
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في مسح الباركود")
            }
    }
    
    @JvmStatic
    fun scanQRCode(
        bitmap: Bitmap,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        scanBarcodes(bitmap, listOf(Barcode.FORMAT_QR_CODE), onSuccess, onError)
    }
    
    private fun Barcode.toMap(): Map<String, Any?> {
        return mapOf(
            "rawValue" to rawValue,
            "displayValue" to displayValue,
            "format" to format,
            "formatName" to getFormatName(format),
            "valueType" to valueType,
            "valueTypeName" to getValueTypeName(valueType),
            "boundingBox" to boundingBox?.toMap(),
            "url" to url?.url,
            "email" to email?.let { mapOf("address" to it.address, "subject" to it.subject, "body" to it.body) },
            "phone" to phone?.number,
            "sms" to sms?.let { mapOf("phoneNumber" to it.phoneNumber, "message" to it.message) },
            "wifi" to wifi?.let { mapOf("ssid" to it.ssid, "password" to it.password, "encryptionType" to it.encryptionType) },
            "geoPoint" to geoPoint?.let { mapOf("lat" to it.lat, "lng" to it.lng) },
            "contactInfo" to contactInfo?.let { 
                mapOf(
                    "name" to it.name?.formattedName,
                    "organization" to it.organization,
                    "title" to it.title,
                    "phones" to it.phones.map { phone -> phone.number },
                    "emails" to it.emails.map { email -> email.address }
                )
            }
        )
    }
    
    private fun getFormatName(format: Int): String {
        return when (format) {
            Barcode.FORMAT_QR_CODE -> "QR Code"
            Barcode.FORMAT_EAN_13 -> "EAN-13"
            Barcode.FORMAT_EAN_8 -> "EAN-8"
            Barcode.FORMAT_UPC_A -> "UPC-A"
            Barcode.FORMAT_UPC_E -> "UPC-E"
            Barcode.FORMAT_CODE_128 -> "Code 128"
            Barcode.FORMAT_CODE_39 -> "Code 39"
            Barcode.FORMAT_CODE_93 -> "Code 93"
            Barcode.FORMAT_CODABAR -> "Codabar"
            Barcode.FORMAT_ITF -> "ITF"
            Barcode.FORMAT_DATA_MATRIX -> "Data Matrix"
            Barcode.FORMAT_PDF417 -> "PDF417"
            Barcode.FORMAT_AZTEC -> "Aztec"
            else -> "Unknown"
        }
    }
    
    private fun getValueTypeName(valueType: Int): String {
        return when (valueType) {
            Barcode.TYPE_TEXT -> "نص"
            Barcode.TYPE_URL -> "رابط"
            Barcode.TYPE_EMAIL -> "بريد إلكتروني"
            Barcode.TYPE_PHONE -> "هاتف"
            Barcode.TYPE_SMS -> "رسالة SMS"
            Barcode.TYPE_WIFI -> "شبكة WiFi"
            Barcode.TYPE_GEO -> "موقع جغرافي"
            Barcode.TYPE_CONTACT_INFO -> "معلومات اتصال"
            Barcode.TYPE_CALENDAR_EVENT -> "حدث تقويم"
            Barcode.TYPE_DRIVER_LICENSE -> "رخصة قيادة"
            Barcode.TYPE_ISBN -> "ISBN"
            Barcode.TYPE_PRODUCT -> "منتج"
            else -> "غير معروف"
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Image Labeling
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun labelImage(
        bitmap: Bitmap,
        confidenceThreshold: Float = 0.7f,
        onSuccess: (List<Map<String, Any?>>) -> Unit,
        onError: (String) -> Unit
    ) {
        val options = ImageLabelerOptions.Builder()
            .setConfidenceThreshold(confidenceThreshold)
            .build()
        
        val labeler = ImageLabeling.getClient(options)
        val image = InputImage.fromBitmap(bitmap, 0)
        
        labeler.process(image)
            .addOnSuccessListener { labels ->
                onSuccess(labels.map { it.toMap() })
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تصنيف الصورة")
            }
    }
    
    private fun ImageLabel.toMap(): Map<String, Any?> {
        return mapOf(
            "text" to text,
            "confidence" to confidence,
            "index" to index
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun Rect.toMap(): Map<String, Int> {
        return mapOf(
            "left" to left,
            "top" to top,
            "right" to right,
            "bottom" to bottom,
            "width" to width(),
            "height" to height()
        )
    }
    
    @JvmStatic
    fun close() {
        textRecognizer?.close()
        arabicTextRecognizer?.close()
        faceDetector?.close()
        barcodeScanner?.close()
        imageLabeler?.close()
        
        textRecognizer = null
        arabicTextRecognizer = null
        faceDetector = null
        barcodeScanner = null
        imageLabeler = null
    }
    
    // Barcode formats for convenience
    object BarcodeFormats {
        const val QR_CODE = Barcode.FORMAT_QR_CODE
        const val EAN_13 = Barcode.FORMAT_EAN_13
        const val EAN_8 = Barcode.FORMAT_EAN_8
        const val UPC_A = Barcode.FORMAT_UPC_A
        const val UPC_E = Barcode.FORMAT_UPC_E
        const val CODE_128 = Barcode.FORMAT_CODE_128
        const val CODE_39 = Barcode.FORMAT_CODE_39
        const val CODE_93 = Barcode.FORMAT_CODE_93
        const val CODABAR = Barcode.FORMAT_CODABAR
        const val ITF = Barcode.FORMAT_ITF
        const val DATA_MATRIX = Barcode.FORMAT_DATA_MATRIX
        const val PDF417 = Barcode.FORMAT_PDF417
        const val AZTEC = Barcode.FORMAT_AZTEC
        const val ALL = Barcode.FORMAT_ALL_FORMATS
    }
}
