// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadPermissionBridge.swift
// الوصف: جسر الأذونات — iOS (إدارة صلاحيات النظام)
// ═══════════════════════════════════════════════════════════════════════════════

import Foundation
import AVFoundation
import CoreLocation
import Photos
import Contacts
import UserNotifications

/// حالة الإذن
public enum SadPermissionStatus: String {
    case granted = "ممنوح"
    case denied = "مرفوض"
    case notDetermined = "غير_محدد"
    case restricted = "مقيد"
    case limited = "محدود"
}

/// نوع الإذن
public enum SadPermissionType: String {
    case camera = "الكاميرا"
    case microphone = "الميكروفون"
    case photoLibrary = "مكتبة_الصور"
    case location = "الموقع"
    case locationAlways = "الموقع_دائماً"
    case notifications = "الإشعارات"
    case contacts = "جهات_الاتصال"
}

/// جسر الأذونات لغة ص — إدارة صلاحيات النظام
public final class SadPermissionBridge {
    
    public static let shared = SadPermissionBridge()
    private init() {}
    
    // ═════════════════════════════════════════════════════════════════════
    // فحص الحالة
    // ═════════════════════════════════════════════════════════════════════
    
    /// فحص حالة إذن معين
    public func checkStatus(_ permission: SadPermissionType,
                            completion: @escaping (SadPermissionStatus) -> Void) {
        switch permission {
        case .camera:
            completion(mapAVStatus(AVCaptureDevice.authorizationStatus(for: .video)))
        case .microphone:
            completion(mapAVStatus(AVCaptureDevice.authorizationStatus(for: .audio)))
        case .photoLibrary:
            completion(mapPHStatus(PHPhotoLibrary.authorizationStatus()))
        case .location:
            completion(mapCLStatus(CLLocationManager().authorizationStatus))
        case .locationAlways:
            completion(mapCLStatus(CLLocationManager().authorizationStatus, requireAlways: true))
        case .notifications:
            UNUserNotificationCenter.current().getNotificationSettings { settings in
                DispatchQueue.main.async {
                    completion(self.mapUNStatus(settings.authorizationStatus))
                }
            }
        case .contacts:
            completion(mapCNStatus(CNContactStore.authorizationStatus(for: .contacts)))
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // طلب الأذونات
    // ═════════════════════════════════════════════════════════════════════
    
    /// طلب إذن معين
    public func request(_ permission: SadPermissionType,
                        completion: @escaping (SadPermissionStatus) -> Void) {
        switch permission {
        case .camera:
            AVCaptureDevice.requestAccess(for: .video) { granted in
                DispatchQueue.main.async {
                    completion(granted ? .granted : .denied)
                }
            }
        case .microphone:
            AVCaptureDevice.requestAccess(for: .audio) { granted in
                DispatchQueue.main.async {
                    completion(granted ? .granted : .denied)
                }
            }
        case .photoLibrary:
            PHPhotoLibrary.requestAuthorization { status in
                DispatchQueue.main.async {
                    completion(self.mapPHStatus(status))
                }
            }
        case .location, .locationAlways:
            // يحتاج CLLocationManager instance — يتم عبر SadLocationBridge
            completion(.notDetermined)
        case .notifications:
            UNUserNotificationCenter.current().requestAuthorization(options: [.alert, .badge, .sound]) {
                granted, _ in
                DispatchQueue.main.async {
                    completion(granted ? .granted : .denied)
                }
            }
        case .contacts:
            CNContactStore().requestAccess(for: .contacts) { granted, _ in
                DispatchQueue.main.async {
                    completion(granted ? .granted : .denied)
                }
            }
        }
    }
    
    /// طلب أذونات متعددة
    public func requestMultiple(_ permissions: [SadPermissionType],
                                completion: @escaping ([SadPermissionType: SadPermissionStatus]) -> Void) {
        var results: [SadPermissionType: SadPermissionStatus] = [:]
        let group = DispatchGroup()
        
        for permission in permissions {
            group.enter()
            request(permission) { status in
                results[permission] = status
                group.leave()
            }
        }
        
        group.notify(queue: .main) {
            completion(results)
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // فتح الإعدادات
    // ═════════════════════════════════════════════════════════════════════
    
    /// فتح إعدادات التطبيق (لتفعيل أذونات مرفوضة)
    public func openAppSettings() {
        guard let url = URL(string: UIApplication.openSettingsURLString) else { return }
        if UIApplication.shared.canOpenURL(url) {
            UIApplication.shared.open(url)
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تحويل الحالات
    // ═════════════════════════════════════════════════════════════════════
    
    private func mapAVStatus(_ status: AVAuthorizationStatus) -> SadPermissionStatus {
        switch status {
        case .authorized: return .granted
        case .denied: return .denied
        case .restricted: return .restricted
        case .notDetermined: return .notDetermined
        @unknown default: return .notDetermined
        }
    }
    
    private func mapPHStatus(_ status: PHAuthorizationStatus) -> SadPermissionStatus {
        switch status {
        case .authorized: return .granted
        case .denied: return .denied
        case .restricted: return .restricted
        case .notDetermined: return .notDetermined
        case .limited: return .limited
        @unknown default: return .notDetermined
        }
    }
    
    private func mapCLStatus(_ status: CLAuthorizationStatus,
                             requireAlways: Bool = false) -> SadPermissionStatus {
        switch status {
        case .authorizedAlways: return .granted
        case .authorizedWhenInUse: return requireAlways ? .limited : .granted
        case .denied: return .denied
        case .restricted: return .restricted
        case .notDetermined: return .notDetermined
        @unknown default: return .notDetermined
        }
    }
    
    private func mapUNStatus(_ status: UNAuthorizationStatus) -> SadPermissionStatus {
        switch status {
        case .authorized: return .granted
        case .denied: return .denied
        case .notDetermined: return .notDetermined
        case .provisional: return .limited
        case .ephemeral: return .limited
        @unknown default: return .notDetermined
        }
    }
    
    private func mapCNStatus(_ status: CNAuthorizationStatus) -> SadPermissionStatus {
        switch status {
        case .authorized: return .granted
        case .denied: return .denied
        case .restricted: return .restricted
        case .notDetermined: return .notDetermined
        @unknown default: return .notDetermined
        }
    }
}
