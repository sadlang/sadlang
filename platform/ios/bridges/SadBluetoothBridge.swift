// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadBluetoothBridge.swift
// الوصف: جسر البلوتوث — iOS (CoreBluetooth)
// ═══════════════════════════════════════════════════════════════════════════════

import CoreBluetooth

/// حالة البلوتوث
public enum SadBluetoothState: String {
    case on = "مفعّل"
    case off = "معطّل"
    case unauthorized = "غير_مصرّح"
    case unsupported = "غير_مدعوم"
    case unknown = "غير_معروف"
}

/// جهاز بلوتوث مكتشف
public struct SadBluetoothDevice {
    public let identifier: String
    public let name: String
    public let rssi: Int
}

/// جسر البلوتوث لغة ص — اكتشاف والاتصال بأجهزة BLE
public final class SadBluetoothBridge: NSObject {
    
    public static let shared = SadBluetoothBridge()
    private override init() { super.init() }
    
    private var centralManager: CBCentralManager?
    private var discoveredDevices: [String: (CBPeripheral, Int)] = [:]
    private var connectedPeripheral: CBPeripheral?
    
    private var onDeviceDiscovered: ((SadBluetoothDevice) -> Void)?
    private var onStateChanged: ((SadBluetoothState) -> Void)?
    private var onConnected: ((String) -> Void)?
    private var onDisconnected: ((String) -> Void)?
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═════════════════════════════════════════════════════════════════════
    
    public func initialize() {
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    public func cleanup() {
        stopScanning()
        disconnect()
        centralManager = nil
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الحالة
    // ═════════════════════════════════════════════════════════════════════
    
    /// حالة البلوتوث الحالية
    public var state: SadBluetoothState {
        guard let manager = centralManager else { return .unknown }
        switch manager.state {
        case .poweredOn: return .on
        case .poweredOff: return .off
        case .unauthorized: return .unauthorized
        case .unsupported: return .unsupported
        default: return .unknown
        }
    }
    
    /// هل البلوتوث مفعّل
    public var isEnabled: Bool {
        state == .on
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // المسح والاكتشاف
    // ═════════════════════════════════════════════════════════════════════
    
    /// بدء المسح عن أجهزة
    public func startScanning(onDiscovered: ((SadBluetoothDevice) -> Void)? = nil) {
        self.onDeviceDiscovered = onDiscovered
        discoveredDevices.removeAll()
        centralManager?.scanForPeripherals(withServices: nil, options: [
            CBCentralManagerScanOptionAllowDuplicatesKey: false
        ])
    }
    
    /// إيقاف المسح
    public func stopScanning() {
        centralManager?.stopScan()
    }
    
    /// الأجهزة المكتشفة
    public var devices: [SadBluetoothDevice] {
        discoveredDevices.map { (id, info) in
            SadBluetoothDevice(
                identifier: id,
                name: info.0.name ?? "غير معروف",
                rssi: info.1
            )
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الاتصال
    // ═════════════════════════════════════════════════════════════════════
    
    /// الاتصال بجهاز
    public func connect(identifier: String,
                         onConnected: ((String) -> Void)? = nil,
                         onDisconnected: ((String) -> Void)? = nil) {
        self.onConnected = onConnected
        self.onDisconnected = onDisconnected
        
        guard let device = discoveredDevices[identifier]?.0 else { return }
        centralManager?.connect(device, options: nil)
    }
    
    /// قطع الاتصال
    public func disconnect() {
        guard let peripheral = connectedPeripheral else { return }
        centralManager?.cancelPeripheralConnection(peripheral)
        connectedPeripheral = nil
    }
    
    /// هل متصل بجهاز
    public var isConnected: Bool {
        connectedPeripheral?.state == .connected
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الأحداث
    // ═════════════════════════════════════════════════════════════════════
    
    /// معالج تغيير حالة البلوتوث
    public func setOnStateChanged(_ handler: @escaping (SadBluetoothState) -> Void) {
        onStateChanged = handler
    }
}

// ═════════════════════════════════════════════════════════════════════
// CBCentralManagerDelegate
// ═════════════════════════════════════════════════════════════════════

extension SadBluetoothBridge: CBCentralManagerDelegate {
    public func centralManagerDidUpdateState(_ central: CBCentralManager) {
        onStateChanged?(state)
    }
    
    public func centralManager(_ central: CBCentralManager,
                               didDiscover peripheral: CBPeripheral,
                               advertisementData: [String: Any],
                               rssi RSSI: NSNumber) {
        let id = peripheral.identifier.uuidString
        discoveredDevices[id] = (peripheral, RSSI.intValue)
        
        let device = SadBluetoothDevice(
            identifier: id,
            name: peripheral.name ?? "غير معروف",
            rssi: RSSI.intValue
        )
        onDeviceDiscovered?(device)
    }
    
    public func centralManager(_ central: CBCentralManager,
                               didConnect peripheral: CBPeripheral) {
        connectedPeripheral = peripheral
        onConnected?(peripheral.identifier.uuidString)
    }
    
    public func centralManager(_ central: CBCentralManager,
                               didDisconnectPeripheral peripheral: CBPeripheral,
                               error: Error?) {
        if connectedPeripheral?.identifier == peripheral.identifier {
            connectedPeripheral = nil
        }
        onDisconnected?(peripheral.identifier.uuidString)
    }
}
