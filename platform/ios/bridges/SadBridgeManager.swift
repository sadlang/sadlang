// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadBridgeManager.swift
// الوصف: مدير جسور لغة ص — نقطة الدخول الرئيسية لمنصة iOS
// ═══════════════════════════════════════════════════════════════════════════════
//
// هذا الملف يدير تهيئة جميع الجسور ويوفر واجهة موحدة للاستخدام.
//
// الاستخدام في AppDelegate أو SceneDelegate:
//   func application(_ application: UIApplication, ...) {
//       SadBridgeManager.shared.initialize()
//   }
//
// ═══════════════════════════════════════════════════════════════════════════════

import UIKit

/// مدير جسور لغة ص — SadBridgeManager
///
/// نقطة الدخول المركزية لتهيئة واستخدام جميع الجسور على iOS.
/// يجب استدعاء initialize() في AppDelegate.
public final class SadBridgeManager {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Singleton
    // ═══════════════════════════════════════════════════════════════════════════
    
    public static let shared = SadBridgeManager()
    private init() {}
    
    // ═══════════════════════════════════════════════════════════════════════════
    // حالة التهيئة
    // ═══════════════════════════════════════════════════════════════════════════
    
    private(set) var isInitialized = false
    
    /// التحقق من تهيئة الجسور
    public var isReady: Bool { isInitialized }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// تهيئة جميع الجسور
    ///
    /// - مثال الاستخدام:
    /// ```swift
    /// @main
    /// struct MyApp: App {
    ///     init() {
    ///         SadBridgeManager.shared.initialize()
    ///     }
    /// }
    /// ```
    public func initialize() {
        guard !isInitialized else { return }
        
        // تهيئة الجسور الأساسية
        SadNotificationBridge.shared.initialize()
        SadStorageBridge.shared.initialize()
        SadLocationBridge.shared.initialize()
        SadAudioBridge.shared.initialize()
        SadSensorsBridge.shared.initialize()
        SadBluetoothBridge.shared.initialize()
        SadNetworkBridge.shared.initialize()
        SadSpeechBridge.shared.initialize()
        SadDeviceInfoBridge.shared.enableBatteryMonitoring()
        
        isInitialized = true
        print("[ص] تم تهيئة جميع الجسور بنجاح (\(bridgeCount) جسر) — iOS")
    }
    
    /// عدد الجسور المتاحة
    public var bridgeCount: Int { 17 }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التنظيف
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// تنظيف جميع الجسور وتحرير الموارد
    public func cleanup() {
        SadNotificationBridge.shared.cleanup()
        SadLocationBridge.shared.stopUpdates()
        SadAudioBridge.shared.cleanup()
        SadCameraBridge.shared.cleanup()
        SadSensorsBridge.shared.cleanup()
        SadBluetoothBridge.shared.cleanup()
        SadNetworkBridge.shared.cleanup()
        SadSpeechBridge.shared.cleanup()
        SadMapsBridge.shared.cleanup()
        SadWebViewBridge.shared.cleanup()
        
        isInitialized = false
        print("[ص] تم تنظيف جميع الجسور — iOS")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // واجهة HTTP المباشرة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// إرسال طلب GET
    public func httpGet(url: String, completion: @escaping (Result<String, Error>) -> Void) {
        SadHttpBridge.shared.get(url: url, completion: completion)
    }
    
    /// إرسال طلب POST
    public func httpPost(url: String, body: String, completion: @escaping (Result<String, Error>) -> Void) {
        SadHttpBridge.shared.post(url: url, body: body, completion: completion)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // واجهة الإشعارات
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// إرسال إشعار محلي
    public func sendNotification(title: String, body: String, delay: TimeInterval = 0) {
        SadNotificationBridge.shared.send(title: title, body: body, delay: delay)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // واجهة التخزين
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// حفظ قيمة
    public func save(key: String, value: String) {
        SadStorageBridge.shared.save(key: key, value: value)
    }
    
    /// قراءة قيمة
    public func load(key: String) -> String? {
        SadStorageBridge.shared.load(key: key)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // معلومات المنصة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// اسم المنصة
    public var platformName: String { "iOS" }
    
    /// إصدار النظام
    public var systemVersion: String { SadDeviceInfoBridge.shared.systemVersion }
    
    /// اسم الجهاز
    public var deviceName: String { SadDeviceInfoBridge.shared.deviceName }
    
    /// نوع الجهاز
    public var deviceModel: String { SadDeviceInfoBridge.shared.deviceModel }
    
    /// هل هو iPad
    public var isIPad: Bool { SadDeviceInfoBridge.shared.isIPad }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // واجهات مباشرة للجسور
    // ═══════════════════════════════════════════════════════════════════════════
    
    public var http: SadHttpBridge { .shared }
    public var notification: SadNotificationBridge { .shared }
    public var location: SadLocationBridge { .shared }
    public var camera: SadCameraBridge { .shared }
    public var storage: SadStorageBridge { .shared }
    public var audio: SadAudioBridge { .shared }
    public var crypto: SadCryptoBridge { .shared }
    public var json: SadJsonBridge { .shared }
    public var permission: SadPermissionBridge { .shared }
    public var sensors: SadSensorsBridge { .shared }
    public var biometric: SadBiometricBridge { .shared }
    public var maps: SadMapsBridge { .shared }
    public var webView: SadWebViewBridge { .shared }
    public var contacts: SadContactsBridge { .shared }
    public var bluetooth: SadBluetoothBridge { .shared }
    public var network: SadNetworkBridge { .shared }
    public var speech: SadSpeechBridge { .shared }
    public var share: SadShareBridge { .shared }
    public var keychain: SadKeychainBridge { .shared }
    public var vibration: SadVibrationBridge { .shared }
    public var deviceInfo: SadDeviceInfoBridge { .shared }
}
