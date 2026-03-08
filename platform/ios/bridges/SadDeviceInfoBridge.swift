// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadDeviceInfoBridge.swift
// الوصف: جسر معلومات الجهاز — iOS (UIDevice + ProcessInfo)
// ═══════════════════════════════════════════════════════════════════════════════

import UIKit

/// جسر معلومات الجهاز لغة ص — معلومات الجهاز والنظام
public final class SadDeviceInfoBridge {
    
    public static let shared = SadDeviceInfoBridge()
    private init() {}
    
    // ═════════════════════════════════════════════════════════════════════
    // معلومات الجهاز
    // ═════════════════════════════════════════════════════════════════════
    
    /// اسم الجهاز
    public var deviceName: String {
        UIDevice.current.name
    }
    
    /// طراز الجهاز (مثل: iPhone, iPad)
    public var deviceModel: String {
        UIDevice.current.model
    }
    
    /// معرف الطراز (مثل: iPhone14,5)
    public var modelIdentifier: String {
        var systemInfo = utsname()
        uname(&systemInfo)
        let machineMirror = Mirror(reflecting: systemInfo.machine)
        return machineMirror.children.reduce("") { id, element in
            guard let value = element.value as? Int8, value != 0 else { return id }
            return id + String(UnicodeScalar(UInt8(value)))
        }
    }
    
    /// هل الجهاز iPad
    public var isIPad: Bool {
        UIDevice.current.userInterfaceIdiom == .pad
    }
    
    /// هل الجهاز iPhone
    public var isIPhone: Bool {
        UIDevice.current.userInterfaceIdiom == .phone
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // معلومات النظام
    // ═════════════════════════════════════════════════════════════════════
    
    /// إصدار النظام (مثل: 17.0)
    public var systemVersion: String {
        UIDevice.current.systemVersion
    }
    
    /// اسم النظام (مثل: iOS)
    public var systemName: String {
        UIDevice.current.systemName
    }
    
    /// اسم التطبيق
    public var appName: String {
        Bundle.main.infoDictionary?["CFBundleDisplayName"] as? String
            ?? Bundle.main.infoDictionary?["CFBundleName"] as? String ?? ""
    }
    
    /// إصدار التطبيق
    public var appVersion: String {
        Bundle.main.infoDictionary?["CFBundleShortVersionString"] as? String ?? ""
    }
    
    /// رقم البناء
    public var buildNumber: String {
        Bundle.main.infoDictionary?["CFBundleVersion"] as? String ?? ""
    }
    
    /// معرف الحزمة
    public var bundleIdentifier: String {
        Bundle.main.bundleIdentifier ?? ""
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // معلومات الشاشة
    // ═════════════════════════════════════════════════════════════════════
    
    /// عرض الشاشة
    public var screenWidth: CGFloat {
        UIScreen.main.bounds.width
    }
    
    /// ارتفاع الشاشة
    public var screenHeight: CGFloat {
        UIScreen.main.bounds.height
    }
    
    /// مقياس الشاشة (مثل: 2x, 3x)
    public var screenScale: CGFloat {
        UIScreen.main.scale
    }
    
    /// سطوع الشاشة (0.0 - 1.0)
    public var brightness: CGFloat {
        get { UIScreen.main.brightness }
        set { UIScreen.main.brightness = max(0, min(1, newValue)) }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // معلومات البطارية
    // ═════════════════════════════════════════════════════════════════════
    
    /// تفعيل مراقبة البطارية
    public func enableBatteryMonitoring() {
        UIDevice.current.isBatteryMonitoringEnabled = true
    }
    
    /// مستوى البطارية (0.0 - 1.0)
    public var batteryLevel: Float {
        UIDevice.current.batteryLevel
    }
    
    /// حالة البطارية
    public var batteryState: String {
        switch UIDevice.current.batteryState {
        case .charging: return "شحن"
        case .full: return "ممتلئة"
        case .unplugged: return "غير_متصلة"
        case .unknown: return "غير_معروفة"
        @unknown default: return "غير_معروفة"
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // معلومات الذاكرة
    // ═════════════════════════════════════════════════════════════════════
    
    /// الذاكرة المتاحة (بالبايت)
    public var availableMemory: UInt64 {
        ProcessInfo.processInfo.physicalMemory
    }
    
    /// عدد المعالجات
    public var processorCount: Int {
        ProcessInfo.processInfo.processorCount
    }
    
    /// وقت تشغيل النظام (بالثواني)
    public var systemUptime: TimeInterval {
        ProcessInfo.processInfo.systemUptime
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مساحة التخزين
    // ═════════════════════════════════════════════════════════════════════
    
    /// المساحة الكلية (بالبايت)
    public var totalDiskSpace: Int64 {
        guard let attrs = try? FileManager.default.attributesOfFileSystem(
            forPath: NSHomeDirectory()) else { return 0 }
        return (attrs[.systemSize] as? Int64) ?? 0
    }
    
    /// المساحة المتاحة (بالبايت)
    public var freeDiskSpace: Int64 {
        guard let attrs = try? FileManager.default.attributesOfFileSystem(
            forPath: NSHomeDirectory()) else { return 0 }
        return (attrs[.systemFreeSize] as? Int64) ?? 0
    }
    
    /// المساحة المستخدمة (بالبايت)
    public var usedDiskSpace: Int64 {
        totalDiskSpace - freeDiskSpace
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // ملخص شامل
    // ═════════════════════════════════════════════════════════════════════
    
    /// ملخص معلومات الجهاز
    public var summary: [String: Any] {
        [
            "اسم_الجهاز": deviceName,
            "الطراز": deviceModel,
            "معرف_الطراز": modelIdentifier,
            "النظام": systemName,
            "إصدار_النظام": systemVersion,
            "عرض_الشاشة": screenWidth,
            "ارتفاع_الشاشة": screenHeight,
            "مقياس_الشاشة": screenScale,
            "عدد_المعالجات": processorCount,
            "الذاكرة": availableMemory,
            "البطارية": batteryLevel
        ]
    }
}
