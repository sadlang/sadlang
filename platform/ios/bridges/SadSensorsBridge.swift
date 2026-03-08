// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadSensorsBridge.swift
// الوصف: جسر المستشعرات — iOS (CoreMotion)
// يشمل: مقياس التسارع، الجيروسكوب، المغناطيسية، عداد الخطوات
// ═══════════════════════════════════════════════════════════════════════════════

import CoreMotion
import Foundation

/// بيانات المحاور الثلاثة
public struct SadSensorData {
    public let x: Double
    public let y: Double
    public let z: Double
    public let timestamp: TimeInterval
}

/// جسر المستشعرات لغة ص — مستشعرات الحركة والموقع
public final class SadSensorsBridge {
    
    public static let shared = SadSensorsBridge()
    private init() {}
    
    private let motionManager = CMMotionManager()
    private let pedometer = CMPedometer()
    private let altimeter = CMAltimeter()
    
    /// بيانات مقياس التسارع الحالية
    public private(set) var accelerometer: SadSensorData?
    
    /// بيانات الجيروسكوب الحالية
    public private(set) var gyroscope: SadSensorData?
    
    /// بيانات المغناطيسية الحالية
    public private(set) var magnetometer: SadSensorData?
    
    /// عدد الخطوات اليوم
    public private(set) var stepCount: Int = 0
    
    /// الارتفاع النسبي (بالأمتار)
    public private(set) var relativeAltitude: Double = 0
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة والإيقاف
    // ═════════════════════════════════════════════════════════════════════
    
    public func initialize() {
        motionManager.accelerometerUpdateInterval = 0.1
        motionManager.gyroUpdateInterval = 0.1
        motionManager.magnetometerUpdateInterval = 0.1
    }
    
    public func cleanup() {
        stopAccelerometer()
        stopGyroscope()
        stopMagnetometer()
        stopPedometer()
        stopAltimeter()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مقياس التسارع (Accelerometer)
    // ═════════════════════════════════════════════════════════════════════
    
    /// هل يتوفر مقياس تسارع
    public var isAccelerometerAvailable: Bool {
        motionManager.isAccelerometerAvailable
    }
    
    /// بدء قراءة التسارع
    public func startAccelerometer(interval: TimeInterval = 0.1,
                                    handler: ((SadSensorData) -> Void)? = nil) {
        motionManager.accelerometerUpdateInterval = interval
        motionManager.startAccelerometerUpdates(to: .main) { [weak self] data, _ in
            guard let data = data else { return }
            let sensorData = SadSensorData(
                x: data.acceleration.x,
                y: data.acceleration.y,
                z: data.acceleration.z,
                timestamp: data.timestamp
            )
            self?.accelerometer = sensorData
            handler?(sensorData)
        }
    }
    
    /// إيقاف قراءة التسارع
    public func stopAccelerometer() {
        motionManager.stopAccelerometerUpdates()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الجيروسكوب (Gyroscope)
    // ═════════════════════════════════════════════════════════════════════
    
    /// هل يتوفر جيروسكوب
    public var isGyroscopeAvailable: Bool {
        motionManager.isGyroAvailable
    }
    
    /// بدء قراءة الجيروسكوب
    public func startGyroscope(interval: TimeInterval = 0.1,
                                handler: ((SadSensorData) -> Void)? = nil) {
        motionManager.gyroUpdateInterval = interval
        motionManager.startGyroUpdates(to: .main) { [weak self] data, _ in
            guard let data = data else { return }
            let sensorData = SadSensorData(
                x: data.rotationRate.x,
                y: data.rotationRate.y,
                z: data.rotationRate.z,
                timestamp: data.timestamp
            )
            self?.gyroscope = sensorData
            handler?(sensorData)
        }
    }
    
    /// إيقاف قراءة الجيروسكوب
    public func stopGyroscope() {
        motionManager.stopGyroUpdates()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // المغناطيسية (Magnetometer)
    // ═════════════════════════════════════════════════════════════════════
    
    /// هل يتوفر مستشعر مغناطيسي
    public var isMagnetometerAvailable: Bool {
        motionManager.isMagnetometerAvailable
    }
    
    /// بدء قراءة المغناطيسية
    public func startMagnetometer(interval: TimeInterval = 0.1,
                                   handler: ((SadSensorData) -> Void)? = nil) {
        motionManager.magnetometerUpdateInterval = interval
        motionManager.startMagnetometerUpdates(to: .main) { [weak self] data, _ in
            guard let data = data else { return }
            let sensorData = SadSensorData(
                x: data.magneticField.x,
                y: data.magneticField.y,
                z: data.magneticField.z,
                timestamp: data.timestamp
            )
            self?.magnetometer = sensorData
            handler?(sensorData)
        }
    }
    
    /// إيقاف قراءة المغناطيسية
    public func stopMagnetometer() {
        motionManager.stopMagnetometerUpdates()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // عداد الخطوات (Pedometer)
    // ═════════════════════════════════════════════════════════════════════
    
    /// هل يتوفر عداد خطوات
    public var isPedometerAvailable: Bool {
        CMPedometer.isStepCountingAvailable()
    }
    
    /// بدء عد الخطوات من الآن
    public func startPedometer(handler: ((Int) -> Void)? = nil) {
        pedometer.startUpdates(from: Date()) { [weak self] data, _ in
            guard let data = data else { return }
            let steps = data.numberOfSteps.intValue
            self?.stepCount = steps
            DispatchQueue.main.async { handler?(steps) }
        }
    }
    
    /// إيقاف عد الخطوات
    public func stopPedometer() {
        pedometer.stopUpdates()
    }
    
    /// الحصول على خطوات فترة زمنية
    public func getSteps(from start: Date, to end: Date,
                          completion: @escaping (Int) -> Void) {
        pedometer.queryPedometerData(from: start, to: end) { data, _ in
            let steps = data?.numberOfSteps.intValue ?? 0
            DispatchQueue.main.async { completion(steps) }
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مقياس الارتفاع (Altimeter)
    // ═════════════════════════════════════════════════════════════════════
    
    /// هل يتوفر مقياس ارتفاع
    public var isAltimeterAvailable: Bool {
        CMAltimeter.isRelativeAltitudeAvailable()
    }
    
    /// بدء قراءة الارتفاع
    public func startAltimeter(handler: ((Double) -> Void)? = nil) {
        altimeter.startRelativeAltitudeUpdates(to: .main) { [weak self] data, _ in
            guard let data = data else { return }
            let altitude = data.relativeAltitude.doubleValue
            self?.relativeAltitude = altitude
            handler?(altitude)
        }
    }
    
    /// إيقاف قراءة الارتفاع
    public func stopAltimeter() {
        altimeter.stopRelativeAltitudeUpdates()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // ضبط الفاصل الزمني
    // ═════════════════════════════════════════════════════════════════════
    
    /// ضبط فاصل تحديث التسارع (بالثواني)
    public func setAccelerometerInterval(_ interval: TimeInterval) {
        motionManager.accelerometerUpdateInterval = interval
    }
    
    /// ضبط فاصل تحديث الجيروسكوب (بالثواني)
    public func setGyroscopeInterval(_ interval: TimeInterval) {
        motionManager.gyroUpdateInterval = interval
    }
    
    /// ضبط فاصل تحديث المغناطيسية (بالثواني)
    public func setMagnetometerInterval(_ interval: TimeInterval) {
        motionManager.magnetometerUpdateInterval = interval
    }
}
