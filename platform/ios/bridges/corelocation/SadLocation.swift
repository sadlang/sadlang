/**
 * =============================================================================
 * @file SadLocation.swift
 * @brief (AR) جسر CoreLocation — الموقع الجغرافي على iOS
 * @brief (EN) CoreLocation Bridge — Location on iOS
 * 
 * المهمة: T252 - CoreLocation
 * المرحلة: سبتمبر 2026 — iOS CoreML
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ الحصول على الموقع الحالي
 * ✓ تتبع الموقع المستمر
 * ✓ Geocoding (عنوان ↔ إحداثيات)
 * ✓ Geofencing (مناطق جغرافية)
 * ✓ البوصلة (Heading)
 * ✓ حساب المسافة والاتجاه
 * 
 * =============================================================================
 */

import Foundation
import CoreLocation
import MapKit

// MARK: - الأنواع

/// موقع جغرافي
public struct GeoLocation {
    public let latitude: Double         // خط العرض
    public let longitude: Double        // خط الطول
    public let altitude: Double         // الارتفاع (متر)
    public let accuracy: Double         // الدقة الأفقية (متر)
    public let verticalAccuracy: Double // الدقة العمودية (متر)
    public let speed: Double            // السرعة (م/ث)
    public let course: Double           // الاتجاه (درجة)
    public let timestamp: Date          // الوقت
    
    public init(from location: CLLocation) {
        self.latitude = location.coordinate.latitude
        self.longitude = location.coordinate.longitude
        self.altitude = location.altitude
        self.accuracy = location.horizontalAccuracy
        self.verticalAccuracy = location.verticalAccuracy
        self.speed = location.speed
        self.course = location.course
        self.timestamp = location.timestamp
    }
    
    public init(latitude: Double, longitude: Double) {
        self.latitude = latitude
        self.longitude = longitude
        self.altitude = 0
        self.accuracy = 0
        self.verticalAccuracy = 0
        self.speed = 0
        self.course = 0
        self.timestamp = Date()
    }
}

/// عنوان
public struct GeoAddress {
    public let name: String             // الاسم
    public let street: String           // الشارع
    public let city: String             // المدينة
    public let state: String            // المنطقة/الولاية
    public let country: String          // البلد
    public let postalCode: String       // الرمز البريدي
    public let fullAddress: String      // العنوان الكامل
    
    public init(from placemark: CLPlacemark) {
        self.name = placemark.name ?? ""
        self.street = placemark.thoroughfare ?? ""
        self.city = placemark.locality ?? ""
        self.state = placemark.administrativeArea ?? ""
        self.country = placemark.country ?? ""
        self.postalCode = placemark.postalCode ?? ""
        
        var components: [String] = []
        if let name = placemark.name { components.append(name) }
        if let street = placemark.thoroughfare { components.append(street) }
        if let city = placemark.locality { components.append(city) }
        if let country = placemark.country { components.append(country) }
        self.fullAddress = components.joined(separator: "، ")
    }
}

/// منطقة جغرافية
public struct GeoRegion {
    public let identifier: String
    public let center: GeoLocation
    public let radius: Double           // متر
    public let notifyOnEntry: Bool
    public let notifyOnExit: Bool
}

/// دقة الموقع
public enum LocationAccuracy {
    case best               // أفضل دقة
    case nearestTenMeters   // 10 متر
    case hundredMeters      // 100 متر
    case kilometer          // 1 كم
    case threeKilometers    // 3 كم
    
    var clAccuracy: CLLocationAccuracy {
        switch self {
        case .best: return kCLLocationAccuracyBest
        case .nearestTenMeters: return kCLLocationAccuracyNearestTenMeters
        case .hundredMeters: return kCLLocationAccuracyHundredMeters
        case .kilometer: return kCLLocationAccuracyKilometer
        case .threeKilometers: return kCLLocationAccuracyThreeKilometers
        }
    }
}

/// حالة الإذن
public enum LocationAuthStatus {
    case notDetermined
    case denied
    case restricted
    case authorizedWhenInUse
    case authorizedAlways
}

/// مستمع أحداث الموقع
public protocol SadLocationDelegate: AnyObject {
    func onLocationUpdated(_ location: GeoLocation)
    func onHeadingUpdated(heading: Double, accuracy: Double)
    func onAuthorizationChanged(_ status: LocationAuthStatus)
    func onRegionEntered(_ identifier: String)
    func onRegionExited(_ identifier: String)
    func onError(_ error: String)
}

// MARK: - تقديم extension افتراضي

public extension SadLocationDelegate {
    func onLocationUpdated(_ location: GeoLocation) {}
    func onHeadingUpdated(heading: Double, accuracy: Double) {}
    func onAuthorizationChanged(_ status: LocationAuthStatus) {}
    func onRegionEntered(_ identifier: String) {}
    func onRegionExited(_ identifier: String) {}
    func onError(_ error: String) {}
}

// MARK: - SadLocation

public class SadLocation: NSObject {
    
    // MARK: - الخصائص
    
    private let locationManager: CLLocationManager
    private let geocoder: CLGeocoder
    
    private var pendingLocationCallback: ((GeoLocation?) -> Void)?
    private var isTracking = false
    
    public weak var delegate: SadLocationDelegate?
    
    /// آخر موقع معروف
    public private(set) var lastLocation: GeoLocation?
    
    /// آخر اتجاه معروف
    public private(set) var lastHeading: Double = 0
    
    // MARK: - التهيئة
    
    public override init() {
        locationManager = CLLocationManager()
        geocoder = CLGeocoder()
        super.init()
        
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        locationManager.distanceFilter = 10  // تحديث كل 10 متر
    }
    
    // MARK: - الأذونات
    
    /// حالة الإذن الحالية
    public var authorizationStatus: LocationAuthStatus {
        let status: CLAuthorizationStatus
        if #available(iOS 14.0, *) {
            status = locationManager.authorizationStatus
        } else {
            status = CLLocationManager.authorizationStatus()
        }
        
        switch status {
        case .notDetermined: return .notDetermined
        case .denied: return .denied
        case .restricted: return .restricted
        case .authorizedWhenInUse: return .authorizedWhenInUse
        case .authorizedAlways: return .authorizedAlways
        @unknown default: return .notDetermined
        }
    }
    
    /// طلب إذن "عند الاستخدام"
    public func requestWhenInUseAuthorization() {
        locationManager.requestWhenInUseAuthorization()
    }
    
    /// طلب إذن "دائماً"
    public func requestAlwaysAuthorization() {
        locationManager.requestAlwaysAuthorization()
    }
    
    // MARK: - الموقع الحالي
    
    /// الحصول على الموقع الحالي (مرة واحدة)
    public func getCurrentLocation(completion: @escaping (GeoLocation?) -> Void) {
        pendingLocationCallback = completion
        locationManager.requestLocation()
    }
    
    /// الموقع الحالي (synchronous — يعيد آخر موقع)
    public func getLastKnownLocation() -> GeoLocation? {
        if let location = locationManager.location {
            return GeoLocation(from: location)
        }
        return lastLocation
    }
    
    // MARK: - التتبع المستمر
    
    /// بدء تتبع الموقع
    public func startTracking(accuracy: LocationAccuracy = .best, distanceFilter: Double = 10) {
        locationManager.desiredAccuracy = accuracy.clAccuracy
        locationManager.distanceFilter = distanceFilter
        locationManager.startUpdatingLocation()
        isTracking = true
    }
    
    /// إيقاف التتبع
    public func stopTracking() {
        locationManager.stopUpdatingLocation()
        isTracking = false
    }
    
    /// هل نتتبع حالياً؟
    public func isCurrentlyTracking() -> Bool {
        return isTracking
    }
    
    // MARK: - البوصلة
    
    /// بدء تتبع الاتجاه
    public func startHeadingUpdates() {
        if CLLocationManager.headingAvailable() {
            locationManager.startUpdatingHeading()
        } else {
            delegate?.onError("البوصلة غير متوفرة على هذا الجهاز")
        }
    }
    
    /// إيقاف تتبع الاتجاه
    public func stopHeadingUpdates() {
        locationManager.stopUpdatingHeading()
    }
    
    // MARK: - Geocoding
    
    /// تحويل إحداثيات إلى عنوان
    public func reverseGeocode(latitude: Double, longitude: Double, completion: @escaping (GeoAddress?) -> Void) {
        let location = CLLocation(latitude: latitude, longitude: longitude)
        
        geocoder.reverseGeocodeLocation(location) { placemarks, error in
            if let error = error {
                self.delegate?.onError("فشل في تحويل الإحداثيات: \(error.localizedDescription)")
                completion(nil)
                return
            }
            
            if let placemark = placemarks?.first {
                completion(GeoAddress(from: placemark))
            } else {
                completion(nil)
            }
        }
    }
    
    /// تحويل عنوان إلى إحداثيات
    public func geocode(address: String, completion: @escaping (GeoLocation?) -> Void) {
        geocoder.geocodeAddressString(address) { placemarks, error in
            if let error = error {
                self.delegate?.onError("فشل في تحويل العنوان: \(error.localizedDescription)")
                completion(nil)
                return
            }
            
            if let placemark = placemarks?.first,
               let location = placemark.location {
                completion(GeoLocation(from: location))
            } else {
                completion(nil)
            }
        }
    }
    
    // MARK: - Geofencing
    
    /// إضافة منطقة للمراقبة
    public func addRegion(_ region: GeoRegion) {
        let center = CLLocationCoordinate2D(latitude: region.center.latitude,
                                            longitude: region.center.longitude)
        
        let clRegion = CLCircularRegion(center: center,
                                        radius: region.radius,
                                        identifier: region.identifier)
        
        clRegion.notifyOnEntry = region.notifyOnEntry
        clRegion.notifyOnExit = region.notifyOnExit
        
        locationManager.startMonitoring(for: clRegion)
    }
    
    /// إزالة منطقة
    public func removeRegion(identifier: String) {
        for region in locationManager.monitoredRegions {
            if region.identifier == identifier {
                locationManager.stopMonitoring(for: region)
                break
            }
        }
    }
    
    /// إزالة جميع المناطق
    public func removeAllRegions() {
        for region in locationManager.monitoredRegions {
            locationManager.stopMonitoring(for: region)
        }
    }
    
    // MARK: - الحسابات
    
    /// حساب المسافة بين نقطتين (متر)
    public func distance(from loc1: GeoLocation, to loc2: GeoLocation) -> Double {
        let location1 = CLLocation(latitude: loc1.latitude, longitude: loc1.longitude)
        let location2 = CLLocation(latitude: loc2.latitude, longitude: loc2.longitude)
        return location1.distance(from: location2)
    }
    
    /// حساب الاتجاه بين نقطتين (درجة)
    public func bearing(from loc1: GeoLocation, to loc2: GeoLocation) -> Double {
        let lat1 = loc1.latitude * .pi / 180
        let lat2 = loc2.latitude * .pi / 180
        let dLon = (loc2.longitude - loc1.longitude) * .pi / 180
        
        let y = sin(dLon) * cos(lat2)
        let x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon)
        
        var bearing = atan2(y, x) * 180 / .pi
        if bearing < 0 {
            bearing += 360
        }
        return bearing
    }
    
    /// حساب نقطة الوسط
    public func midpoint(between loc1: GeoLocation, and loc2: GeoLocation) -> GeoLocation {
        let lat1 = loc1.latitude * .pi / 180
        let lon1 = loc1.longitude * .pi / 180
        let lat2 = loc2.latitude * .pi / 180
        let lon2 = loc2.longitude * .pi / 180
        
        let dLon = lon2 - lon1
        
        let Bx = cos(lat2) * cos(dLon)
        let By = cos(lat2) * sin(dLon)
        
        let lat3 = atan2(sin(lat1) + sin(lat2), sqrt((cos(lat1) + Bx) * (cos(lat1) + Bx) + By * By))
        let lon3 = lon1 + atan2(By, cos(lat1) + Bx)
        
        return GeoLocation(latitude: lat3 * 180 / .pi, longitude: lon3 * 180 / .pi)
    }
}

// MARK: - CLLocationManagerDelegate

extension SadLocation: CLLocationManagerDelegate {
    
    public func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let location = locations.last else { return }
        
        let geoLocation = GeoLocation(from: location)
        lastLocation = geoLocation
        
        // callback للموقع الواحد
        if let callback = pendingLocationCallback {
            callback(geoLocation)
            pendingLocationCallback = nil
        }
        
        // delegate للتتبع المستمر
        delegate?.onLocationUpdated(geoLocation)
    }
    
    public func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        delegate?.onError("خطأ في الموقع: \(error.localizedDescription)")
        
        if let callback = pendingLocationCallback {
            callback(nil)
            pendingLocationCallback = nil
        }
    }
    
    public func locationManager(_ manager: CLLocationManager, didUpdateHeading newHeading: CLHeading) {
        let heading = newHeading.trueHeading >= 0 ? newHeading.trueHeading : newHeading.magneticHeading
        lastHeading = heading
        delegate?.onHeadingUpdated(heading: heading, accuracy: newHeading.headingAccuracy)
    }
    
    public func locationManagerDidChangeAuthorization(_ manager: CLLocationManager) {
        delegate?.onAuthorizationChanged(authorizationStatus)
    }
    
    public func locationManager(_ manager: CLLocationManager, didEnterRegion region: CLRegion) {
        delegate?.onRegionEntered(region.identifier)
    }
    
    public func locationManager(_ manager: CLLocationManager, didExitRegion region: CLRegion) {
        delegate?.onRegionExited(region.identifier)
    }
}

// MARK: - دوال مساعدة للغة ص

/// إنشاء مدير الموقع
public func موقع_انشئ() -> SadLocation {
    return SadLocation()
}

/// طلب إذن الموقع
public func موقع_اذن(_ location: SadLocation) {
    location.requestWhenInUseAuthorization()
}

/// بدء التتبع
public func موقع_تتبع(_ location: SadLocation) {
    location.startTracking()
}

/// إيقاف التتبع
public func موقع_أوقف(_ location: SadLocation) {
    location.stopTracking()
}

/// بدء البوصلة
public func موقع_بوصلة(_ location: SadLocation) {
    location.startHeadingUpdates()
}

/// الحصول على الموقع الحالي
public func موقع_احصل(_ location: SadLocation, callback: @escaping (Double, Double) -> Void) {
    location.getCurrentLocation { loc in
        if let loc = loc {
            callback(loc.latitude, loc.longitude)
        } else {
            callback(0, 0)
        }
    }
}

/// تحويل إحداثيات إلى عنوان
public func موقع_عنوان(_ location: SadLocation, عرض: Double, طول: Double, callback: @escaping (String) -> Void) {
    location.reverseGeocode(latitude: عرض, longitude: طول) { address in
        callback(address?.fullAddress ?? "")
    }
}

/// حساب المسافة
public func موقع_مسافة(_ location: SadLocation, عرض1: Double, طول1: Double, عرض2: Double, طول2: Double) -> Double {
    let loc1 = GeoLocation(latitude: عرض1, longitude: طول1)
    let loc2 = GeoLocation(latitude: عرض2, longitude: طول2)
    return location.distance(from: loc1, to: loc2)
}

/// إضافة منطقة مراقبة
public func موقع_منطقة(_ location: SadLocation, اسم: String, عرض: Double, طول: Double, نصف_قطر: Double) {
    let center = GeoLocation(latitude: عرض, longitude: طول)
    let region = GeoRegion(
        identifier: اسم,
        center: center,
        radius: نصف_قطر,
        notifyOnEntry: true,
        notifyOnExit: true
    )
    location.addRegion(region)
}
