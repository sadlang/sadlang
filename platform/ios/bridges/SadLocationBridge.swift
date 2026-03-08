// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadLocationBridge.swift
// الوصف: جسر الموقع الجغرافي — iOS (CLLocationManager)
// ═══════════════════════════════════════════════════════════════════════════════

import CoreLocation

/// جسر موقع لغة ص — يدعم GPS والموقع الجغرافي
public final class SadLocationBridge: NSObject {
    
    public static let shared = SadLocationBridge()
    private override init() { super.init() }
    
    private lazy var manager: CLLocationManager = {
        let m = CLLocationManager()
        m.delegate = self
        m.desiredAccuracy = kCLLocationAccuracyBest
        return m
    }()
    
    private var lastLocation: CLLocation?
    private var locationCallback: ((Double, Double, Double) -> Void)?
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة والصلاحيات
    // ═════════════════════════════════════════════════════════════════════
    
    /// تهيئة جسر الموقع
    public func initialize() {
        _ = manager // تهيئة lazy
    }
    
    /// طلب صلاحية الاستخدام أثناء التشغيل
    public func requestPermission() {
        manager.requestWhenInUseAuthorization()
    }
    
    /// طلب صلاحية الاستخدام الدائم
    public func requestAlwaysPermission() {
        manager.requestAlwaysAuthorization()
    }
    
    /// التحقق من حالة الصلاحية
    public var isAuthorized: Bool {
        let status = manager.authorizationStatus
        return status == .authorizedWhenInUse || status == .authorizedAlways
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تتبع الموقع
    // ═════════════════════════════════════════════════════════════════════
    
    /// بدء تحديثات الموقع
    public func startUpdates(callback: @escaping (Double, Double, Double) -> Void) {
        locationCallback = callback
        manager.startUpdatingLocation()
    }
    
    /// إيقاف تحديثات الموقع
    public func stopUpdates() {
        manager.stopUpdatingLocation()
        locationCallback = nil
    }
    
    /// الحصول على الموقع الحالي (مرة واحدة)
    public func getCurrentLocation(callback: @escaping (Double, Double, Double) -> Void) {
        locationCallback = callback
        manager.requestLocation()
    }
    
    /// خط العرض الحالي
    public var latitude: Double { lastLocation?.coordinate.latitude ?? 0 }
    
    /// خط الطول الحالي
    public var longitude: Double { lastLocation?.coordinate.longitude ?? 0 }
    
    /// الارتفاع الحالي
    public var altitude: Double { lastLocation?.altitude ?? 0 }
    
    /// دقة الموقع
    public var accuracy: Double { lastLocation?.horizontalAccuracy ?? -1 }
    
    /// سرعة الحركة (م/ث)
    public var speed: Double { lastLocation?.speed ?? 0 }
    
    // ═════════════════════════════════════════════════════════════════════
    // حساب المسافة
    // ═════════════════════════════════════════════════════════════════════
    
    /// حساب المسافة بين نقطتين بالمتر
    public func distance(fromLat: Double, fromLon: Double,
                         toLat: Double, toLon: Double) -> Double {
        let from = CLLocation(latitude: fromLat, longitude: fromLon)
        let to = CLLocation(latitude: toLat, longitude: toLon)
        return from.distance(from: to)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تحديد دقة الموقع
    // ═════════════════════════════════════════════════════════════════════
    
    /// تعيين دقة عالية (GPS)
    public func setHighAccuracy() {
        manager.desiredAccuracy = kCLLocationAccuracyBest
    }
    
    /// تعيين دقة متوسطة (توفير بطارية)
    public func setBalancedAccuracy() {
        manager.desiredAccuracy = kCLLocationAccuracyHundredMeters
    }
    
    /// تعيين دقة منخفضة (أقل استهلاك)
    public func setLowAccuracy() {
        manager.desiredAccuracy = kCLLocationAccuracyKilometer
    }
}

// ═════════════════════════════════════════════════════════════════════════════
// CLLocationManagerDelegate
// ═════════════════════════════════════════════════════════════════════════════

extension SadLocationBridge: CLLocationManagerDelegate {
    public func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let location = locations.last else { return }
        lastLocation = location
        locationCallback?(location.coordinate.latitude,
                         location.coordinate.longitude,
                         location.altitude)
    }
    
    public func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        print("[ص] خطأ في الموقع: \(error.localizedDescription)")
    }
}
