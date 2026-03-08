// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadStorageBridge.swift
// الوصف: جسر التخزين — iOS (UserDefaults + FileManager)
// ═══════════════════════════════════════════════════════════════════════════════

import Foundation

/// جسر تخزين لغة ص — تخزين البيانات والملفات
public final class SadStorageBridge {
    
    public static let shared = SadStorageBridge()
    private init() {}
    
    private let defaults = UserDefaults.standard
    private let fileManager = FileManager.default
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═════════════════════════════════════════════════════════════════════
    
    /// تهيئة جسر التخزين
    public func initialize() {
        // إنشاء مجلد التطبيق إذا لم يكن موجوداً
        let appDir = getAppDirectory()
        if !fileManager.fileExists(atPath: appDir.path) {
            try? fileManager.createDirectory(at: appDir, withIntermediateDirectories: true)
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تخزين مفتاح/قيمة (UserDefaults)
    // ═════════════════════════════════════════════════════════════════════
    
    /// حفظ نص
    public func save(key: String, value: String) {
        defaults.set(value, forKey: key)
    }
    
    /// قراءة نص
    public func load(key: String) -> String? {
        defaults.string(forKey: key)
    }
    
    /// حفظ رقم
    public func saveInt(key: String, value: Int) {
        defaults.set(value, forKey: key)
    }
    
    /// قراءة رقم
    public func loadInt(key: String) -> Int {
        defaults.integer(forKey: key)
    }
    
    /// حفظ عشري
    public func saveDouble(key: String, value: Double) {
        defaults.set(value, forKey: key)
    }
    
    /// قراءة عشري
    public func loadDouble(key: String) -> Double {
        defaults.double(forKey: key)
    }
    
    /// حفظ منطقي
    public func saveBool(key: String, value: Bool) {
        defaults.set(value, forKey: key)
    }
    
    /// قراءة منطقي
    public func loadBool(key: String) -> Bool {
        defaults.bool(forKey: key)
    }
    
    /// حذف مفتاح
    public func remove(key: String) {
        defaults.removeObject(forKey: key)
    }
    
    /// مسح جميع البيانات
    public func clearAll() {
        if let bundleId = Bundle.main.bundleIdentifier {
            defaults.removePersistentDomain(forName: bundleId)
        }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تخزين الملفات (FileManager)
    // ═════════════════════════════════════════════════════════════════════
    
    /// كتابة ملف نصي
    public func writeFile(name: String, content: String) -> Bool {
        let url = getAppDirectory().appendingPathComponent(name)
        do {
            try content.write(to: url, atomically: true, encoding: .utf8)
            return true
        } catch {
            print("[ص] خطأ كتابة ملف: \(error.localizedDescription)")
            return false
        }
    }
    
    /// قراءة ملف نصي
    public func readFile(name: String) -> String? {
        let url = getAppDirectory().appendingPathComponent(name)
        return try? String(contentsOf: url, encoding: .utf8)
    }
    
    /// حذف ملف
    public func deleteFile(name: String) -> Bool {
        let url = getAppDirectory().appendingPathComponent(name)
        do {
            try fileManager.removeItem(at: url)
            return true
        } catch {
            return false
        }
    }
    
    /// التحقق من وجود ملف
    public func fileExists(name: String) -> Bool {
        let url = getAppDirectory().appendingPathComponent(name)
        return fileManager.fileExists(atPath: url.path)
    }
    
    /// قائمة الملفات في مجلد التطبيق
    public func listFiles() -> [String] {
        let url = getAppDirectory()
        return (try? fileManager.contentsOfDirectory(atPath: url.path)) ?? []
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مساعدات
    // ═════════════════════════════════════════════════════════════════════
    
    /// الحصول على مجلد التطبيق
    private func getAppDirectory() -> URL {
        fileManager.urls(for: .documentDirectory, in: .userDomainMask)[0]
            .appendingPathComponent("SadApp")
    }
    
    /// حجم التخزين المستخدم (بالبايت)
    public var usedStorage: Int64 {
        let url = getAppDirectory()
        guard let enumerator = fileManager.enumerator(at: url, includingPropertiesForKeys: [.fileSizeKey]) else { return 0 }
        var total: Int64 = 0
        for case let fileURL as URL in enumerator {
            let size = (try? fileURL.resourceValues(forKeys: [.fileSizeKey]).fileSize) ?? 0
            total += Int64(size)
        }
        return total
    }
}
