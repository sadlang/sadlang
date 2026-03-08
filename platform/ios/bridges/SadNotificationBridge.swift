// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadNotificationBridge.swift
// الوصف: جسر الإشعارات المحلية — iOS (UNUserNotificationCenter)
// ═══════════════════════════════════════════════════════════════════════════════

import UserNotifications
import UIKit

/// جسر إشعارات لغة ص — يدعم الإشعارات المحلية والمجدولة
public final class SadNotificationBridge: NSObject {
    
    public static let shared = SadNotificationBridge()
    private override init() { super.init() }
    
    private let center = UNUserNotificationCenter.current()
    private var isAuthorized = false
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═════════════════════════════════════════════════════════════════════
    
    /// تهيئة جسر الإشعارات وطلب الصلاحيات
    public func initialize() {
        center.delegate = self
        center.requestAuthorization(options: [.alert, .sound, .badge]) { [weak self] granted, _ in
            self?.isAuthorized = granted
        }
    }
    
    /// تنظيف الموارد
    public func cleanup() {
        center.removeAllPendingNotificationRequests()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // إرسال الإشعارات
    // ═════════════════════════════════════════════════════════════════════
    
    /// إرسال إشعار محلي
    /// - Parameters:
    ///   - title: عنوان الإشعار
    ///   - body: نص الإشعار
    ///   - delay: التأخير بالثواني (0 = فوري)
    ///   - categoryId: معرّف الفئة (اختياري)
    public func send(title: String, body: String, delay: TimeInterval = 0,
                     categoryId: String? = nil) {
        let content = UNMutableNotificationContent()
        content.title = title
        content.body = body
        content.sound = .default
        if let cat = categoryId {
            content.categoryIdentifier = cat
        }
        
        let trigger: UNNotificationTrigger?
        if delay > 0 {
            trigger = UNTimeIntervalNotificationTrigger(timeInterval: delay, repeats: false)
        } else {
            trigger = UNTimeIntervalNotificationTrigger(timeInterval: 0.1, repeats: false)
        }
        
        let id = UUID().uuidString
        let request = UNNotificationRequest(identifier: id, content: content, trigger: trigger)
        center.add(request)
    }
    
    /// جدولة إشعار يومي
    /// - Parameters:
    ///   - title: عنوان الإشعار
    ///   - body: نص الإشعار
    ///   - hour: الساعة (0-23)
    ///   - minute: الدقيقة (0-59)
    public func scheduleDaily(title: String, body: String, hour: Int, minute: Int) {
        let content = UNMutableNotificationContent()
        content.title = title
        content.body = body
        content.sound = .default
        
        var dateComponents = DateComponents()
        dateComponents.hour = hour
        dateComponents.minute = minute
        
        let trigger = UNCalendarNotificationTrigger(dateMatching: dateComponents, repeats: true)
        let id = "daily_\(hour)_\(minute)"
        let request = UNNotificationRequest(identifier: id, content: content, trigger: trigger)
        center.add(request)
    }
    
    /// إلغاء إشعار بالمعرّف
    public func cancel(identifier: String) {
        center.removePendingNotificationRequests(withIdentifiers: [identifier])
    }
    
    /// إلغاء جميع الإشعارات المعلّقة
    public func cancelAll() {
        center.removeAllPendingNotificationRequests()
    }
    
    /// التحقق من صلاحية الإشعارات
    public func checkPermission(completion: @escaping (Bool) -> Void) {
        center.getNotificationSettings { settings in
            completion(settings.authorizationStatus == .authorized)
        }
    }
    
    /// تعيين شارة التطبيق
    public func setBadge(count: Int) {
        DispatchQueue.main.async {
            UIApplication.shared.applicationIconBadgeNumber = count
        }
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// UNUserNotificationCenterDelegate
// ═════════════════════════════════════════════════════════════════════════════

extension SadNotificationBridge: UNUserNotificationCenterDelegate {
    public func userNotificationCenter(
        _ center: UNUserNotificationCenter,
        willPresent notification: UNNotification,
        withCompletionHandler completionHandler: @escaping (UNNotificationPresentationOptions) -> Void
    ) {
        // عرض الإشعار أثناء استخدام التطبيق
        completionHandler([.banner, .sound, .badge])
    }
    
    public func userNotificationCenter(
        _ center: UNUserNotificationCenter,
        didReceive response: UNNotificationResponse,
        withCompletionHandler completionHandler: @escaping () -> Void
    ) {
        // معالجة الاستجابة عند الضغط على الإشعار
        completionHandler()
    }
}
