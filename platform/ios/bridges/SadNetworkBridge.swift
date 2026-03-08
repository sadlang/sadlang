// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadNetworkBridge.swift
// الوصف: جسر الشبكة — iOS (Network Framework)
// ═══════════════════════════════════════════════════════════════════════════════

import Network

/// حالة الاتصال بالشبكة
public enum SadNetworkStatus: String {
    case wifi = "واي_فاي"
    case cellular = "بيانات_خلوية"
    case ethernet = "إيثرنت"
    case disconnected = "غير_متصل"
}

/// جسر الشبكة لغة ص — مراقبة حالة الاتصال بالشبكة
public final class SadNetworkBridge {
    
    public static let shared = SadNetworkBridge()
    private init() {}
    
    private let monitor = NWPathMonitor()
    private let queue = DispatchQueue(label: "sad.network.monitor")
    
    private var onStatusChanged: ((SadNetworkStatus) -> Void)?
    private var currentStatus: SadNetworkStatus = .disconnected
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═════════════════════════════════════════════════════════════════════
    
    public func initialize() {
        monitor.pathUpdateHandler = { [weak self] path in
            let status: SadNetworkStatus
            if path.status == .satisfied {
                if path.usesInterfaceType(.wifi) {
                    status = .wifi
                } else if path.usesInterfaceType(.cellular) {
                    status = .cellular
                } else if path.usesInterfaceType(.wiredEthernet) {
                    status = .ethernet
                } else {
                    status = .wifi
                }
            } else {
                status = .disconnected
            }
            
            self?.currentStatus = status
            DispatchQueue.main.async {
                self?.onStatusChanged?(status)
            }
        }
        monitor.start(queue: queue)
    }
    
    public func cleanup() {
        monitor.cancel()
        onStatusChanged = nil
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الحالة
    // ═════════════════════════════════════════════════════════════════════
    
    /// حالة الاتصال الحالية
    public var status: SadNetworkStatus {
        currentStatus
    }
    
    /// هل متصل بالإنترنت
    public var isConnected: Bool {
        currentStatus != .disconnected
    }
    
    /// هل متصل بالواي فاي
    public var isOnWifi: Bool {
        currentStatus == .wifi
    }
    
    /// هل متصل ببيانات خلوية
    public var isOnCellular: Bool {
        currentStatus == .cellular
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // فحص الاتصال
    // ═════════════════════════════════════════════════════════════════════
    
    /// فحص إمكانية الوصول لمضيف معين
    public func checkReachability(host: String,
                                   completion: @escaping (Bool) -> Void) {
        let monitor = NWPathMonitor()
        let queue = DispatchQueue(label: "sad.reachability")
        
        monitor.pathUpdateHandler = { path in
            monitor.cancel()
            DispatchQueue.main.async {
                completion(path.status == .satisfied)
            }
        }
        monitor.start(queue: queue)
    }
    
    /// فحص اتصال DNS
    public func resolveDNS(hostname: String,
                            completion: @escaping (String?) -> Void) {
        DispatchQueue.global().async {
            let host = CFHostCreateWithName(nil, hostname as CFString).takeRetainedValue()
            var resolved = DarwinBoolean(false)
            CFHostStartInfoResolution(host, .addresses, nil)
            
            guard let addresses = CFHostGetAddressing(host, &resolved)?.takeUnretainedValue()
                    as? [Data], let firstAddr = addresses.first else {
                DispatchQueue.main.async { completion(nil) }
                return
            }
            
            var hostnameBuffer = [CChar](repeating: 0, count: Int(NI_MAXHOST))
            firstAddr.withUnsafeBytes { pointer in
                let sockaddr = pointer.bindMemory(to: sockaddr.self)
                if let baseAddress = sockaddr.baseAddress {
                    getnameinfo(baseAddress, socklen_t(firstAddr.count),
                                &hostnameBuffer, socklen_t(hostnameBuffer.count),
                                nil, 0, NI_NUMERICHOST)
                }
            }
            
            let ip = String(cString: hostnameBuffer)
            DispatchQueue.main.async { completion(ip.isEmpty ? nil : ip) }
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الأحداث
    // ═════════════════════════════════════════════════════════════════════
    
    /// معالج تغيير حالة الشبكة
    public func setOnStatusChanged(_ handler: @escaping (SadNetworkStatus) -> Void) {
        onStatusChanged = handler
    }
}
