// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadBiometricBridge.swift
// الوصف: جسر المصادقة البيومترية — iOS (LocalAuthentication)
// ═══════════════════════════════════════════════════════════════════════════════

import LocalAuthentication

/// نوع المصادقة البيومترية المتاحة
public enum SadBiometricType: String {
    case faceID = "بصمة_الوجه"
    case touchID = "بصمة_الإصبع"
    case none = "لا_يوجد"
}

/// جسر المصادقة البيومترية لغة ص — Face ID / Touch ID
public final class SadBiometricBridge {
    
    public static let shared = SadBiometricBridge()
    private init() {}
    
    // ═════════════════════════════════════════════════════════════════════
    // فحص التوفر
    // ═════════════════════════════════════════════════════════════════════
    
    /// نوع المصادقة البيومترية المتاحة
    public var availableType: SadBiometricType {
        let context = LAContext()
        var error: NSError?
        guard context.canEvaluatePolicy(.deviceOwnerAuthenticationWithBiometrics,
                                        error: &error) else {
            return .none
        }
        switch context.biometryType {
        case .faceID: return .faceID
        case .touchID: return .touchID
        default: return .none
        }
    }
    
    /// هل المصادقة البيومترية متاحة
    public var isAvailable: Bool {
        availableType != .none
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // المصادقة
    // ═════════════════════════════════════════════════════════════════════
    
    /// مصادقة المستخدم بالبيومتريك
    public func authenticate(reason: String = "يرجى التحقق من هويتك",
                              completion: @escaping (Bool, String?) -> Void) {
        let context = LAContext()
        context.localizedFallbackTitle = "استخدم رمز المرور"
        context.localizedCancelTitle = "إلغاء"
        
        var error: NSError?
        guard context.canEvaluatePolicy(.deviceOwnerAuthenticationWithBiometrics,
                                        error: &error) else {
            completion(false, error?.localizedDescription ?? "المصادقة البيومترية غير متاحة")
            return
        }
        
        context.evaluatePolicy(.deviceOwnerAuthenticationWithBiometrics,
                                localizedReason: reason) { success, authError in
            DispatchQueue.main.async {
                if success {
                    completion(true, nil)
                } else {
                    completion(false, authError?.localizedDescription)
                }
            }
        }
    }
    
    /// مصادقة بالبيومتريك أو رمز المرور
    public func authenticateWithFallback(reason: String = "يرجى التحقق من هويتك",
                                          completion: @escaping (Bool, String?) -> Void) {
        let context = LAContext()
        
        context.evaluatePolicy(.deviceOwnerAuthentication,
                                localizedReason: reason) { success, error in
            DispatchQueue.main.async {
                if success {
                    completion(true, nil)
                } else {
                    completion(false, error?.localizedDescription)
                }
            }
        }
    }
}
