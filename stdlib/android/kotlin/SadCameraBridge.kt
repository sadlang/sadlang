// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadCameraBridge.kt
// الوصف: جسر Kotlin للكاميرا في لغة ص
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يربط دوال __اندرويد_كاميرا_* بـ CameraX API
//
// الدوال المُنفّذة:
//   __اندرويد_كاميرا_افتح(previewView) -> bool
//   __اندرويد_كاميرا_التقط(path) -> bool
//   __اندرويد_كاميرا_اغلق() -> bool
//   __اندرويد_كاميرا_بدّل() -> bool (أمامية/خلفية)
//
// المتطلبات في build.gradle.kts:
//   implementation("androidx.camera:camera-core:1.3.0")
//   implementation("androidx.camera:camera-camera2:1.3.0")
//   implementation("androidx.camera:camera-lifecycle:1.3.0")
//   implementation("androidx.camera:camera-view:1.3.0")
//
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.net.Uri
import androidx.camera.core.*
import androidx.camera.lifecycle.ProcessCameraProvider
import androidx.camera.view.PreviewView
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.lifecycle.LifecycleOwner
import java.io.File
import java.text.SimpleDateFormat
import java.util.*
import java.util.concurrent.ExecutorService
import java.util.concurrent.Executors

/**
 * جسر الكاميرا — SadCameraBridge
 * 
 * يوفر واجهة للتحكم بالكاميرا من كود لغة ص.
 * يستخدم CameraX لسهولة الاستخدام والتوافق.
 */
object SadCameraBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إعداد
    // ═══════════════════════════════════════════════════════════════════════════
    
    private var appContext: Context? = null
    private var lifecycleOwner: LifecycleOwner? = null
    
    private var cameraProvider: ProcessCameraProvider? = null
    private var imageCapture: ImageCapture? = null
    private var camera: Camera? = null
    private var preview: Preview? = null
    
    private var isFrontCamera = false
    
    private val cameraExecutor: ExecutorService = Executors.newSingleThreadExecutor()
    
    /**
     * تهيئة الجسر
     * 
     * @param context سياق التطبيق (Activity)
     * @param owner مالك دورة الحياة (Activity implements LifecycleOwner)
     */
    @JvmStatic
    fun initialize(context: Context, owner: LifecycleOwner) {
        appContext = context
        lifecycleOwner = owner
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التحقق من الأذونات
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من إذن الكاميرا
     */
    @JvmStatic
    fun hasCameraPermission(): Boolean {
        val context = appContext ?: return false
        return ActivityCompat.checkSelfPermission(
            context,
            Manifest.permission.CAMERA
        ) == PackageManager.PERMISSION_GRANTED
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_كاميرا_افتح — فتح الكاميرا
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * فتح الكاميرا وبدء المعاينة
     * 
     * @param previewView عرض المعاينة (PreviewView)
     * @param useFrontCamera استخدام الكاميرا الأمامية؟
     * @param callback دالة الاستجابة
     */
    @JvmStatic
    fun openCamera(
        previewView: PreviewView,
        useFrontCamera: Boolean = false,
        callback: (Boolean, String?) -> Unit
    ) {
        val context = appContext
        val owner = lifecycleOwner
        
        if (context == null || owner == null) {
            callback(false, "لم يتم تهيئة الجسر")
            return
        }
        
        if (!hasCameraPermission()) {
            callback(false, "لا يوجد إذن للكاميرا")
            return
        }
        
        isFrontCamera = useFrontCamera
        
        val cameraProviderFuture = ProcessCameraProvider.getInstance(context)
        
        cameraProviderFuture.addListener({
            try {
                cameraProvider = cameraProviderFuture.get()
                
                // إعداد المعاينة
                preview = Preview.Builder()
                    .setTargetRotation(previewView.display.rotation)
                    .build()
                    .also { it.setSurfaceProvider(previewView.surfaceProvider) }
                
                // إعداد التقاط الصور
                imageCapture = ImageCapture.Builder()
                    .setCaptureMode(ImageCapture.CAPTURE_MODE_MINIMIZE_LATENCY)
                    .setTargetRotation(previewView.display.rotation)
                    .build()
                
                // اختيار الكاميرا
                val cameraSelector = if (useFrontCamera) {
                    CameraSelector.DEFAULT_FRONT_CAMERA
                } else {
                    CameraSelector.DEFAULT_BACK_CAMERA
                }
                
                // ربط حالات الاستخدام
                cameraProvider?.unbindAll()
                camera = cameraProvider?.bindToLifecycle(
                    owner,
                    cameraSelector,
                    preview,
                    imageCapture
                )
                
                callback(true, null)
                
            } catch (e: Exception) {
                callback(false, "فشل فتح الكاميرا: ${e.message}")
            }
        }, ContextCompat.getMainExecutor(context))
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_كاميرا_التقط — التقاط صورة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التقاط صورة وحفظها
     * 
     * @param outputPath مسار الحفظ (اختياري)
     * @param callback دالة الاستجابة (نجح؟، مسار الملف، خطأ؟)
     */
    @JvmStatic
    fun capturePhoto(
        outputPath: String? = null,
        callback: (Boolean, String?, String?) -> Unit
    ) {
        val context = appContext
        val capture = imageCapture
        
        if (context == null || capture == null) {
            callback(false, null, "الكاميرا غير مفتوحة")
            return
        }
        
        // إنشاء ملف الإخراج
        val photoFile = if (outputPath != null) {
            File(outputPath)
        } else {
            val timestamp = SimpleDateFormat("yyyyMMdd_HHmmss", Locale.US).format(Date())
            File(context.cacheDir, "IMG_$timestamp.jpg")
        }
        
        val outputOptions = ImageCapture.OutputFileOptions.Builder(photoFile).build()
        
        capture.takePicture(
            outputOptions,
            ContextCompat.getMainExecutor(context),
            object : ImageCapture.OnImageSavedCallback {
                override fun onImageSaved(output: ImageCapture.OutputFileResults) {
                    callback(true, photoFile.absolutePath, null)
                }
                
                override fun onError(exception: ImageCaptureException) {
                    callback(false, null, "فشل الالتقاط: ${exception.message}")
                }
            }
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_كاميرا_بدّل — تبديل الكاميرا
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تبديل بين الكاميرا الأمامية والخلفية
     * 
     * @param previewView عرض المعاينة
     * @param callback دالة الاستجابة
     */
    @JvmStatic
    fun switchCamera(
        previewView: PreviewView,
        callback: (Boolean, String?) -> Unit
    ) {
        isFrontCamera = !isFrontCamera
        openCamera(previewView, isFrontCamera, callback)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // __اندرويد_كاميرا_اغلق — إغلاق الكاميرا
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إغلاق الكاميرا وتحرير الموارد
     */
    @JvmStatic
    fun closeCamera(): Boolean {
        return try {
            cameraProvider?.unbindAll()
            camera = null
            preview = null
            imageCapture = null
            true
        } catch (e: Exception) {
            e.printStackTrace()
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التحكم بالكاميرا
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تشغيل/إيقاف الفلاش
     */
    @JvmStatic
    fun toggleFlash(): Boolean {
        val cam = camera ?: return false
        
        return try {
            val currentMode = cam.cameraInfo.torchState.value
            val newMode = currentMode != TorchState.ON
            cam.cameraControl.enableTorch(newMode)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * التكبير
     * 
     * @param zoomRatio نسبة التكبير (1.0 = بدون تكبير)
     */
    @JvmStatic
    fun setZoom(zoomRatio: Float): Boolean {
        val cam = camera ?: return false
        
        return try {
            cam.cameraControl.setZoomRatio(zoomRatio)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * التركيز على نقطة معينة
     * 
     * @param x الإحداثية السينية (0-1)
     * @param y الإحداثية الصادية (0-1)
     */
    @JvmStatic
    fun focusOnPoint(x: Float, y: Float): Boolean {
        val cam = camera ?: return false
        
        return try {
            val factory = SurfaceOrientedMeteringPointFactory(1f, 1f)
            val point = factory.createPoint(x, y)
            val action = FocusMeteringAction.Builder(point).build()
            cam.cameraControl.startFocusAndMetering(action)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    /**
     * تنظيف الموارد عند الإغلاق
     */
    @JvmStatic
    fun cleanup() {
        closeCamera()
        cameraExecutor.shutdown()
        appContext = null
        lifecycleOwner = null
    }
}
