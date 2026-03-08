// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadKeychainBridge.swift
// الوصف: جسر سلسلة المفاتيح — iOS (Keychain Services)
// ═══════════════════════════════════════════════════════════════════════════════

import Security
import Foundation

/// جسر سلسلة المفاتيح لغة ص — تخزين آمن للبيانات الحساسة
public final class SadKeychainBridge {
    
    public static let shared = SadKeychainBridge()
    private init() {}
    
    private let serviceName = "com.sad.language"
    
    // ═════════════════════════════════════════════════════════════════════
    // حفظ البيانات
    // ═════════════════════════════════════════════════════════════════════
    
    /// حفظ نص في سلسلة المفاتيح
    public func save(key: String, value: String) -> Bool {
        guard let data = value.data(using: .utf8) else { return false }
        
        // حذف القيمة القديمة إن وجدت
        delete(key: key)
        
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrService as String: serviceName,
            kSecAttrAccount as String: key,
            kSecValueData as String: data,
            kSecAttrAccessible as String: kSecAttrAccessibleWhenUnlockedThisDeviceOnly
        ]
        
        let status = SecItemAdd(query as CFDictionary, nil)
        return status == errSecSuccess
    }
    
    /// حفظ بيانات ثنائية
    public func saveData(key: String, data: Data) -> Bool {
        delete(key: key)
        
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrService as String: serviceName,
            kSecAttrAccount as String: key,
            kSecValueData as String: data,
            kSecAttrAccessible as String: kSecAttrAccessibleWhenUnlockedThisDeviceOnly
        ]
        
        return SecItemAdd(query as CFDictionary, nil) == errSecSuccess
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // قراءة البيانات
    // ═════════════════════════════════════════════════════════════════════
    
    /// قراءة نص من سلسلة المفاتيح
    public func load(key: String) -> String? {
        guard let data = loadData(key: key) else { return nil }
        return String(data: data, encoding: .utf8)
    }
    
    /// قراءة بيانات ثنائية
    public func loadData(key: String) -> Data? {
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrService as String: serviceName,
            kSecAttrAccount as String: key,
            kSecReturnData as String: true,
            kSecMatchLimit as String: kSecMatchLimitOne
        ]
        
        var result: AnyObject?
        let status = SecItemCopyMatching(query as CFDictionary, &result)
        
        guard status == errSecSuccess else { return nil }
        return result as? Data
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // حذف البيانات
    // ═════════════════════════════════════════════════════════════════════
    
    /// حذف قيمة من سلسلة المفاتيح
    @discardableResult
    public func delete(key: String) -> Bool {
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrService as String: serviceName,
            kSecAttrAccount as String: key
        ]
        
        return SecItemDelete(query as CFDictionary) == errSecSuccess
    }
    
    /// حذف كل البيانات المحفوظة
    @discardableResult
    public func deleteAll() -> Bool {
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrService as String: serviceName
        ]
        
        return SecItemDelete(query as CFDictionary) == errSecSuccess
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // فحص الوجود
    // ═════════════════════════════════════════════════════════════════════
    
    /// هل يوجد مفتاح معين
    public func exists(key: String) -> Bool {
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrService as String: serviceName,
            kSecAttrAccount as String: key,
            kSecReturnData as String: false
        ]
        
        return SecItemCopyMatching(query as CFDictionary, nil) == errSecSuccess
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تحديث البيانات
    // ═════════════════════════════════════════════════════════════════════
    
    /// تحديث قيمة موجودة
    public func update(key: String, value: String) -> Bool {
        guard let data = value.data(using: .utf8) else { return false }
        
        let query: [String: Any] = [
            kSecClass as String: kSecClassGenericPassword,
            kSecAttrService as String: serviceName,
            kSecAttrAccount as String: key
        ]
        
        let attributes: [String: Any] = [
            kSecValueData as String: data
        ]
        
        let status = SecItemUpdate(query as CFDictionary, attributes as CFDictionary)
        
        if status == errSecItemNotFound {
            return save(key: key, value: value)
        }
        return status == errSecSuccess
    }
}
