// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadMapsBridge.swift
// الوصف: جسر الخرائط — iOS (MapKit)
// ═══════════════════════════════════════════════════════════════════════════════

import MapKit
import CoreLocation

/// علامة على الخريطة
public class SadMapAnnotation: NSObject, MKAnnotation {
    public let coordinate: CLLocationCoordinate2D
    public let title: String?
    public let subtitle: String?
    public let identifier: String
    
    public init(id: String, latitude: Double, longitude: Double,
                title: String?, subtitle: String?) {
        self.identifier = id
        self.coordinate = CLLocationCoordinate2D(latitude: latitude, longitude: longitude)
        self.title = title
        self.subtitle = subtitle
    }
}

/// جسر الخرائط لغة ص — عرض خرائط وتحديد مواقع
public final class SadMapsBridge: NSObject {
    
    public static let shared = SadMapsBridge()
    private override init() { super.init() }
    
    private var mapView: MKMapView?
    private var annotations: [String: SadMapAnnotation] = [:]
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═════════════════════════════════════════════════════════════════════
    
    /// إنشاء عرض الخريطة
    public func createMapView(frame: CGRect) -> MKMapView {
        let map = MKMapView(frame: frame)
        map.delegate = self
        map.showsUserLocation = true
        map.showsCompass = true
        map.showsScale = true
        self.mapView = map
        return map
    }
    
    public func cleanup() {
        mapView?.removeAnnotations(mapView?.annotations ?? [])
        mapView?.removeOverlays(mapView?.overlays ?? [])
        annotations.removeAll()
        mapView = nil
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // التحكم بالعرض
    // ═════════════════════════════════════════════════════════════════════
    
    /// تعيين مركز الخريطة
    public func setCenter(latitude: Double, longitude: Double, animated: Bool = true) {
        let coord = CLLocationCoordinate2D(latitude: latitude, longitude: longitude)
        mapView?.setCenter(coord, animated: animated)
    }
    
    /// تعيين المنطقة المرئية
    public func setRegion(latitude: Double, longitude: Double,
                           latDelta: Double = 0.01, lonDelta: Double = 0.01,
                           animated: Bool = true) {
        let coord = CLLocationCoordinate2D(latitude: latitude, longitude: longitude)
        let span = MKCoordinateSpan(latitudeDelta: latDelta, longitudeDelta: lonDelta)
        let region = MKCoordinateRegion(center: coord, span: span)
        mapView?.setRegion(region, animated: animated)
    }
    
    /// تكبير/تصغير
    public func setZoom(level: Double) {
        guard let mapView = mapView else { return }
        let center = mapView.centerCoordinate
        let span = MKCoordinateSpan(
            latitudeDelta: 360.0 / pow(2.0, level),
            longitudeDelta: 360.0 / pow(2.0, level)
        )
        mapView.setRegion(MKCoordinateRegion(center: center, span: span), animated: true)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // العلامات (Annotations)
    // ═════════════════════════════════════════════════════════════════════
    
    /// إضافة علامة
    public func addAnnotation(id: String, latitude: Double, longitude: Double,
                               title: String? = nil, subtitle: String? = nil) {
        let annotation = SadMapAnnotation(
            id: id, latitude: latitude, longitude: longitude,
            title: title, subtitle: subtitle
        )
        annotations[id] = annotation
        mapView?.addAnnotation(annotation)
    }
    
    /// إزالة علامة
    public func removeAnnotation(id: String) {
        guard let annotation = annotations.removeValue(forKey: id) else { return }
        mapView?.removeAnnotation(annotation)
    }
    
    /// إزالة كل العلامات
    public func clearAnnotations() {
        mapView?.removeAnnotations(Array(annotations.values))
        annotations.removeAll()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الخطوط والمسارات
    // ═════════════════════════════════════════════════════════════════════
    
    /// رسم خط بين نقطتين
    public func drawLine(from: (Double, Double), to: (Double, Double),
                          color: UIColor = .blue, width: CGFloat = 3) {
        let coords = [
            CLLocationCoordinate2D(latitude: from.0, longitude: from.1),
            CLLocationCoordinate2D(latitude: to.0, longitude: to.1)
        ]
        let polyline = MKPolyline(coordinates: coords, count: 2)
        mapView?.addOverlay(polyline)
    }
    
    /// رسم دائرة حول نقطة
    public func drawCircle(center: (Double, Double), radius: Double,
                            color: UIColor = .blue) {
        let coord = CLLocationCoordinate2D(latitude: center.0, longitude: center.1)
        let circle = MKCircle(center: coord, radius: radius)
        mapView?.addOverlay(circle)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // البحث الجغرافي
    // ═════════════════════════════════════════════════════════════════════
    
    /// البحث عن عنوان بالاسم
    public func searchAddress(_ query: String,
                               completion: @escaping ([(String, Double, Double)]) -> Void) {
        let request = MKLocalSearch.Request()
        request.naturalLanguageQuery = query
        
        MKLocalSearch(request: request).start { response, _ in
            let results = response?.mapItems.map { item in
                (item.name ?? "",
                 item.placemark.coordinate.latitude,
                 item.placemark.coordinate.longitude)
            } ?? []
            DispatchQueue.main.async { completion(results) }
        }
    }
    
    /// تحويل إحداثيات إلى عنوان
    public func reverseGeocode(latitude: Double, longitude: Double,
                                completion: @escaping (String?) -> Void) {
        let location = CLLocation(latitude: latitude, longitude: longitude)
        CLGeocoder().reverseGeocodeLocation(location) { placemarks, _ in
            let address = placemarks?.first.map { placemark in
                [placemark.thoroughfare, placemark.subThoroughfare,
                 placemark.locality, placemark.country]
                    .compactMap { $0 }
                    .joined(separator: "، ")
            }
            DispatchQueue.main.async { completion(address) }
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // نوع الخريطة
    // ═════════════════════════════════════════════════════════════════════
    
    /// تعيين نوع الخريطة
    public func setMapType(_ type: String) {
        switch type {
        case "قمر_صناعي": mapView?.mapType = .satellite
        case "هجين": mapView?.mapType = .hybrid
        case "تضاريس": mapView?.mapType = .mutedStandard
        default: mapView?.mapType = .standard
        }
    }
}

// ═════════════════════════════════════════════════════════════════════
// MKMapViewDelegate
// ═════════════════════════════════════════════════════════════════════

extension SadMapsBridge: MKMapViewDelegate {
    public func mapView(_ mapView: MKMapView, rendererFor overlay: MKOverlay) -> MKOverlayRenderer {
        if let polyline = overlay as? MKPolyline {
            let renderer = MKPolylineRenderer(polyline: polyline)
            renderer.strokeColor = .systemBlue
            renderer.lineWidth = 3
            return renderer
        }
        if let circle = overlay as? MKCircle {
            let renderer = MKCircleRenderer(circle: circle)
            renderer.fillColor = UIColor.systemBlue.withAlphaComponent(0.1)
            renderer.strokeColor = .systemBlue
            renderer.lineWidth = 2
            return renderer
        }
        return MKOverlayRenderer(overlay: overlay)
    }
}
