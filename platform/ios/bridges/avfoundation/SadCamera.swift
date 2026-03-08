/**
 * =============================================================================
 * @file SadCamera.swift
 * @brief (AR) جسر AVFoundation — الكاميرا على iOS
 * @brief (EN) AVFoundation Bridge — Camera on iOS
 * 
 * المهمة: T251 - AVFoundation Camera
 * المرحلة: سبتمبر 2026 — iOS CoreML
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ التقاط الصور
 * ✓ تسجيل الفيديو
 * ✓ المعاينة المباشرة
 * ✓ تبديل الكاميرا
 * ✓ Flash control
 * ✓ Focus/Exposure control
 * ✓ QR/Barcode scanning
 * 
 * =============================================================================
 */

import Foundation
import AVFoundation
import UIKit
import Vision

// MARK: - الأنواع

/// نوع الكاميرا
public enum CameraPosition {
    case back       // الخلفية
    case front      // الأمامية
}

/// وضع Flash
public enum FlashMode {
    case off        // مغلق
    case on         // مفتوح
    case auto       // تلقائي
}

/// جودة الفيديو
public enum VideoQuality {
    case low        // 640x480
    case medium     // 1280x720
    case high       // 1920x1080
    case ultra      // 3840x2160 (4K)
    
    var preset: AVCaptureSession.Preset {
        switch self {
        case .low: return .vga640x480
        case .medium: return .hd1280x720
        case .high: return .hd1920x1080
        case .ultra: return .hd4K3840x2160
        }
    }
}

/// نتيجة مسح الباركود
public struct BarcodeResult {
    public let value: String
    public let type: String
    public let bounds: CGRect
}

/// مستمع أحداث الكاميرا
public protocol SadCameraDelegate: AnyObject {
    func onCameraReady()
    func onPhotoCaptured(_ image: UIImage)
    func onVideoRecordingStarted()
    func onVideoRecordingStopped(url: URL)
    func onBarcodeDetected(_ result: BarcodeResult)
    func onError(_ error: String)
}

// MARK: - تقديم extension افتراضي

public extension SadCameraDelegate {
    func onCameraReady() {}
    func onPhotoCaptured(_ image: UIImage) {}
    func onVideoRecordingStarted() {}
    func onVideoRecordingStopped(url: URL) {}
    func onBarcodeDetected(_ result: BarcodeResult) {}
    func onError(_ error: String) {}
}

// MARK: - SadCamera

public class SadCamera: NSObject {
    
    // MARK: - الخصائص
    
    private var captureSession: AVCaptureSession?
    private var photoOutput: AVCapturePhotoOutput?
    private var videoOutput: AVCaptureMovieFileOutput?
    private var previewLayer: AVCaptureVideoPreviewLayer?
    private var metadataOutput: AVCaptureMetadataOutput?
    
    private var currentDevice: AVCaptureDevice?
    private var currentPosition: CameraPosition = .back
    private var currentFlashMode: FlashMode = .auto
    
    private var isRecording = false
    
    public weak var delegate: SadCameraDelegate?
    
    // MARK: - التهيئة
    
    public override init() {
        super.init()
    }
    
    // MARK: - الإعداد
    
    /// إعداد الكاميرا
    public func setup(quality: VideoQuality = .high, position: CameraPosition = .back) {
        currentPosition = position
        
        captureSession = AVCaptureSession()
        captureSession?.sessionPreset = quality.preset
        
        guard let device = getDevice(for: position) else {
            delegate?.onError("لم يتم العثور على كاميرا")
            return
        }
        
        currentDevice = device
        
        do {
            let input = try AVCaptureDeviceInput(device: device)
            
            if captureSession?.canAddInput(input) == true {
                captureSession?.addInput(input)
            }
            
            // إعداد مخرج الصور
            photoOutput = AVCapturePhotoOutput()
            if let photoOutput = photoOutput,
               captureSession?.canAddOutput(photoOutput) == true {
                captureSession?.addOutput(photoOutput)
            }
            
            // إعداد مخرج الفيديو
            videoOutput = AVCaptureMovieFileOutput()
            if let videoOutput = videoOutput,
               captureSession?.canAddOutput(videoOutput) == true {
                captureSession?.addOutput(videoOutput)
            }
            
            delegate?.onCameraReady()
            
        } catch {
            delegate?.onError("فشل في إعداد الكاميرا: \(error.localizedDescription)")
        }
    }
    
    /// إضافة طبقة المعاينة
    public func addPreviewLayer(to view: UIView) {
        guard let session = captureSession else { return }
        
        previewLayer = AVCaptureVideoPreviewLayer(session: session)
        previewLayer?.videoGravity = .resizeAspectFill
        previewLayer?.frame = view.bounds
        
        if let layer = previewLayer {
            view.layer.insertSublayer(layer, at: 0)
        }
    }
    
    /// تحديث حجم المعاينة
    public func updatePreviewFrame(_ frame: CGRect) {
        previewLayer?.frame = frame
    }
    
    // MARK: - التحكم بالجلسة
    
    /// بدء الكاميرا
    public func start() {
        DispatchQueue.global(qos: .userInitiated).async { [weak self] in
            self?.captureSession?.startRunning()
        }
    }
    
    /// إيقاف الكاميرا
    public func stop() {
        captureSession?.stopRunning()
    }
    
    /// تبديل الكاميرا
    public func switchCamera() {
        guard let session = captureSession else { return }
        
        session.beginConfiguration()
        
        // إزالة المدخل الحالي
        if let currentInput = session.inputs.first as? AVCaptureDeviceInput {
            session.removeInput(currentInput)
        }
        
        // تبديل الموقع
        currentPosition = (currentPosition == .back) ? .front : .back
        
        // إضافة المدخل الجديد
        if let device = getDevice(for: currentPosition),
           let input = try? AVCaptureDeviceInput(device: device) {
            if session.canAddInput(input) {
                session.addInput(input)
                currentDevice = device
            }
        }
        
        session.commitConfiguration()
    }
    
    // MARK: - التقاط الصور
    
    /// التقاط صورة
    public func capturePhoto() {
        guard let photoOutput = photoOutput else {
            delegate?.onError("مخرج الصور غير جاهز")
            return
        }
        
        let settings = AVCapturePhotoSettings()
        
        // Flash
        if let device = currentDevice, device.hasFlash {
            switch currentFlashMode {
            case .off:
                settings.flashMode = .off
            case .on:
                settings.flashMode = .on
            case .auto:
                settings.flashMode = .auto
            }
        }
        
        photoOutput.capturePhoto(with: settings, delegate: self)
    }
    
    /// تعيين وضع Flash
    public func setFlashMode(_ mode: FlashMode) {
        currentFlashMode = mode
    }
    
    // MARK: - تسجيل الفيديو
    
    /// بدء تسجيل الفيديو
    public func startRecording() {
        guard let videoOutput = videoOutput, !isRecording else { return }
        
        let tempURL = FileManager.default.temporaryDirectory
            .appendingPathComponent(UUID().uuidString)
            .appendingPathExtension("mov")
        
        videoOutput.startRecording(to: tempURL, recordingDelegate: self)
        isRecording = true
        delegate?.onVideoRecordingStarted()
    }
    
    /// إيقاف تسجيل الفيديو
    public func stopRecording() {
        guard isRecording else { return }
        videoOutput?.stopRecording()
        isRecording = false
    }
    
    /// هل نسجل حالياً؟
    public func isCurrentlyRecording() -> Bool {
        return isRecording
    }
    
    // MARK: - التركيز والتعريض
    
    /// التركيز على نقطة
    public func focus(at point: CGPoint, in viewBounds: CGRect) {
        guard let device = currentDevice else { return }
        
        let focusPoint = previewLayer?.captureDevicePointConverted(fromLayerPoint: point) ?? point
        
        do {
            try device.lockForConfiguration()
            
            if device.isFocusPointOfInterestSupported {
                device.focusPointOfInterest = focusPoint
                device.focusMode = .autoFocus
            }
            
            if device.isExposurePointOfInterestSupported {
                device.exposurePointOfInterest = focusPoint
                device.exposureMode = .autoExpose
            }
            
            device.unlockForConfiguration()
        } catch {
            delegate?.onError("فشل في ضبط التركيز")
        }
    }
    
    /// تعيين Zoom
    public func setZoom(_ factor: CGFloat) {
        guard let device = currentDevice else { return }
        
        let clampedFactor = min(max(factor, 1.0), device.activeFormat.videoMaxZoomFactor)
        
        do {
            try device.lockForConfiguration()
            device.videoZoomFactor = clampedFactor
            device.unlockForConfiguration()
        } catch {
            delegate?.onError("فشل في ضبط التكبير")
        }
    }
    
    /// تفعيل/تعطيل الفلاش (torch mode)
    public func setTorch(_ enabled: Bool) {
        guard let device = currentDevice, device.hasTorch else { return }
        
        do {
            try device.lockForConfiguration()
            device.torchMode = enabled ? .on : .off
            device.unlockForConfiguration()
        } catch {
            delegate?.onError("فشل في تفعيل الفلاش")
        }
    }
    
    // MARK: - مسح الباركود
    
    /// تفعيل مسح الباركود
    public func enableBarcodeScanning() {
        guard let session = captureSession else { return }
        
        metadataOutput = AVCaptureMetadataOutput()
        
        if let output = metadataOutput, session.canAddOutput(output) {
            session.addOutput(output)
            output.setMetadataObjectsDelegate(self, queue: DispatchQueue.main)
            output.metadataObjectTypes = [
                .qr,
                .ean8,
                .ean13,
                .pdf417,
                .code128,
                .code39,
                .code93,
                .upce,
                .aztec,
                .dataMatrix
            ]
        }
    }
    
    /// تعطيل مسح الباركود
    public func disableBarcodeScanning() {
        if let output = metadataOutput {
            captureSession?.removeOutput(output)
            metadataOutput = nil
        }
    }
    
    // MARK: - حفظ الوسائط
    
    /// حفظ صورة في ألبوم الصور
    public func saveToPhotoLibrary(_ image: UIImage, completion: @escaping (Bool) -> Void) {
        UIImageWriteToSavedPhotosAlbum(image, nil, nil, nil)
        completion(true)
    }
    
    /// حفظ فيديو في ألبوم الصور
    public func saveVideoToPhotoLibrary(_ url: URL, completion: @escaping (Bool) -> Void) {
        UISaveVideoAtPathToSavedPhotosAlbum(url.path, nil, nil, nil)
        completion(true)
    }
    
    // MARK: - Private
    
    private func getDevice(for position: CameraPosition) -> AVCaptureDevice? {
        let devicePosition: AVCaptureDevice.Position = (position == .back) ? .back : .front
        
        // محاولة الحصول على wide angle أولاً
        if let device = AVCaptureDevice.default(.builtInWideAngleCamera, for: .video, position: devicePosition) {
            return device
        }
        
        // fallback
        return AVCaptureDevice.default(for: .video)
    }
}

// MARK: - AVCapturePhotoCaptureDelegate

extension SadCamera: AVCapturePhotoCaptureDelegate {
    
    public func photoOutput(_ output: AVCapturePhotoOutput, didFinishProcessingPhoto photo: AVCapturePhoto, error: Error?) {
        if let error = error {
            delegate?.onError("فشل في التقاط الصورة: \(error.localizedDescription)")
            return
        }
        
        guard let data = photo.fileDataRepresentation(),
              let image = UIImage(data: data) else {
            delegate?.onError("فشل في معالجة الصورة")
            return
        }
        
        DispatchQueue.main.async {
            self.delegate?.onPhotoCaptured(image)
        }
    }
}

// MARK: - AVCaptureFileOutputRecordingDelegate

extension SadCamera: AVCaptureFileOutputRecordingDelegate {
    
    public func fileOutput(_ output: AVCaptureFileOutput, didFinishRecordingTo outputFileURL: URL, from connections: [AVCaptureConnection], error: Error?) {
        isRecording = false
        
        if let error = error {
            delegate?.onError("فشل في تسجيل الفيديو: \(error.localizedDescription)")
            return
        }
        
        DispatchQueue.main.async {
            self.delegate?.onVideoRecordingStopped(url: outputFileURL)
        }
    }
}

// MARK: - AVCaptureMetadataOutputObjectsDelegate

extension SadCamera: AVCaptureMetadataOutputObjectsDelegate {
    
    public func metadataOutput(_ output: AVCaptureMetadataOutput, didOutput metadataObjects: [AVMetadataObject], from connection: AVCaptureConnection) {
        guard let metadataObject = metadataObjects.first as? AVMetadataMachineReadableCodeObject,
              let stringValue = metadataObject.stringValue else { return }
        
        let result = BarcodeResult(
            value: stringValue,
            type: metadataObject.type.rawValue,
            bounds: metadataObject.bounds
        )
        
        delegate?.onBarcodeDetected(result)
    }
}

// MARK: - دوال مساعدة للغة ص

/// إنشاء الكاميرا
public func كاميرا_انشئ() -> SadCamera {
    return SadCamera()
}

/// إعداد الكاميرا
public func كاميرا_اعداد(_ camera: SadCamera, أمامية: Bool = false) {
    camera.setup(position: أمامية ? .front : .back)
}

/// بدء الكاميرا
public func كاميرا_ابدأ(_ camera: SadCamera) {
    camera.start()
}

/// إيقاف الكاميرا
public func كاميرا_أوقف(_ camera: SadCamera) {
    camera.stop()
}

/// التقاط صورة
public func كاميرا_صور(_ camera: SadCamera) {
    camera.capturePhoto()
}

/// تبديل الكاميرا
public func كاميرا_بدّل(_ camera: SadCamera) {
    camera.switchCamera()
}

/// بدء تسجيل الفيديو
public func كاميرا_سجّل(_ camera: SadCamera) {
    camera.startRecording()
}

/// إيقاف تسجيل الفيديو
public func كاميرا_أوقف_التسجيل(_ camera: SadCamera) {
    camera.stopRecording()
}

/// تفعيل الفلاش
public func كاميرا_فلاش(_ camera: SadCamera, مفعّل: Bool) {
    camera.setTorch(مفعّل)
}

/// تفعيل مسح الباركود
public func كاميرا_باركود(_ camera: SadCamera) {
    camera.enableBarcodeScanning()
}
