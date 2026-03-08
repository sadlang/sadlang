// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadCameraBridge.swift
// الوصف: جسر الكاميرا — iOS (AVFoundation)
// ═══════════════════════════════════════════════════════════════════════════════

import AVFoundation
import UIKit

/// جسر كاميرا لغة ص — التقاط الصور والفيديو
public final class SadCameraBridge: NSObject {
    
    public static let shared = SadCameraBridge()
    private override init() { super.init() }
    
    private var captureSession: AVCaptureSession?
    private var photoOutput: AVCapturePhotoOutput?
    private var videoOutput: AVCaptureMovieFileOutput?
    private var previewLayer: AVCaptureVideoPreviewLayer?
    private var photoCallback: ((UIImage?) -> Void)?
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═════════════════════════════════════════════════════════════════════
    
    /// تهيئة الكاميرا
    /// - Parameter position: الكاميرا (.front أو .back)
    public func setup(position: AVCaptureDevice.Position = .back) {
        let session = AVCaptureSession()
        session.sessionPreset = .high
        
        guard let device = AVCaptureDevice.default(
            .builtInWideAngleCamera, for: .video, position: position
        ) else { return }
        
        guard let input = try? AVCaptureDeviceInput(device: device) else { return }
        
        if session.canAddInput(input) {
            session.addInput(input)
        }
        
        let photo = AVCapturePhotoOutput()
        if session.canAddOutput(photo) {
            session.addOutput(photo)
            photoOutput = photo
        }
        
        captureSession = session
    }
    
    /// تنظيف الموارد
    public func cleanup() {
        captureSession?.stopRunning()
        captureSession = nil
        photoOutput = nil
        previewLayer = nil
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // التقاط الصور
    // ═════════════════════════════════════════════════════════════════════
    
    /// بدء تشغيل الكاميرا
    public func startPreview() {
        DispatchQueue.global(qos: .userInitiated).async { [weak self] in
            self?.captureSession?.startRunning()
        }
    }
    
    /// إيقاف الكاميرا
    public func stopPreview() {
        captureSession?.stopRunning()
    }
    
    /// التقاط صورة
    public func capturePhoto(completion: @escaping (UIImage?) -> Void) {
        photoCallback = completion
        let settings = AVCapturePhotoSettings()
        photoOutput?.capturePhoto(with: settings, delegate: self)
    }
    
    /// الحصول على طبقة المعاينة لعرضها في واجهة المستخدم
    public func getPreviewLayer(frame: CGRect) -> AVCaptureVideoPreviewLayer? {
        guard let session = captureSession else { return nil }
        let layer = AVCaptureVideoPreviewLayer(session: session)
        layer.frame = frame
        layer.videoGravity = .resizeAspectFill
        previewLayer = layer
        return layer
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // التبديل بين الكاميرات
    // ═════════════════════════════════════════════════════════════════════
    
    /// التبديل بين الكاميرا الأمامية والخلفية
    public func switchCamera() {
        guard let session = captureSession,
              let currentInput = session.inputs.first as? AVCaptureDeviceInput else { return }
        
        let newPosition: AVCaptureDevice.Position = 
            currentInput.device.position == .back ? .front : .back
        
        guard let newDevice = AVCaptureDevice.default(
            .builtInWideAngleCamera, for: .video, position: newPosition
        ) else { return }
        
        guard let newInput = try? AVCaptureDeviceInput(device: newDevice) else { return }
        
        session.beginConfiguration()
        session.removeInput(currentInput)
        if session.canAddInput(newInput) {
            session.addInput(newInput)
        }
        session.commitConfiguration()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // التحكم في الفلاش
    // ═════════════════════════════════════════════════════════════════════
    
    /// تشغيل/إطفاء الفلاش
    public func toggleFlash() {
        guard let device = AVCaptureDevice.default(for: .video),
              device.hasTorch else { return }
        try? device.lockForConfiguration()
        device.torchMode = device.torchMode == .on ? .off : .on
        device.unlockForConfiguration()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الصلاحيات
    // ═════════════════════════════════════════════════════════════════════
    
    /// التحقق من صلاحية الكاميرا
    public var isAuthorized: Bool {
        AVCaptureDevice.authorizationStatus(for: .video) == .authorized
    }
    
    /// طلب صلاحية الكاميرا
    public func requestPermission(completion: @escaping (Bool) -> Void) {
        AVCaptureDevice.requestAccess(for: .video, completionHandler: completion)
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// AVCapturePhotoCaptureDelegate
// ═════════════════════════════════════════════════════════════════════════════

extension SadCameraBridge: AVCapturePhotoCaptureDelegate {
    public func photoOutput(_ output: AVCapturePhotoOutput,
                           didFinishProcessingPhoto photo: AVCapturePhoto,
                           error: Error?) {
        guard let data = photo.fileDataRepresentation(),
              let image = UIImage(data: data) else {
            photoCallback?(nil)
            return
        }
        photoCallback?(image)
    }
}
