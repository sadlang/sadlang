// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadJsonBridge.swift
// الوصف: جسر JSON — iOS (Foundation)
// ═══════════════════════════════════════════════════════════════════════════════

import Foundation

/// جسر JSON لغة ص — تحويل وقراءة بيانات JSON
public final class SadJsonBridge {
    
    public static let shared = SadJsonBridge()
    private init() {}
    
    // ═════════════════════════════════════════════════════════════════════
    // التحويل إلى JSON
    // ═════════════════════════════════════════════════════════════════════
    
    /// تحويل قاموس إلى نص JSON
    public func encode(_ dictionary: [String: Any], prettyPrint: Bool = false) -> String? {
        guard JSONSerialization.isValidJSONObject(dictionary) else { return nil }
        var options: JSONSerialization.WritingOptions = [.sortedKeys]
        if prettyPrint { options.insert(.prettyPrinted) }
        
        guard let data = try? JSONSerialization.data(withJSONObject: dictionary, options: options) else {
            return nil
        }
        return String(data: data, encoding: .utf8)
    }
    
    /// تحويل مصفوفة إلى نص JSON
    public func encodeArray(_ array: [Any], prettyPrint: Bool = false) -> String? {
        guard JSONSerialization.isValidJSONObject(array) else { return nil }
        var options: JSONSerialization.WritingOptions = [.sortedKeys]
        if prettyPrint { options.insert(.prettyPrinted) }
        
        guard let data = try? JSONSerialization.data(withJSONObject: array, options: options) else {
            return nil
        }
        return String(data: data, encoding: .utf8)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // القراءة من JSON
    // ═════════════════════════════════════════════════════════════════════
    
    /// تحويل نص JSON إلى قاموس
    public func decodeDictionary(_ jsonString: String) -> [String: Any]? {
        guard let data = jsonString.data(using: .utf8) else { return nil }
        return try? JSONSerialization.jsonObject(with: data) as? [String: Any]
    }
    
    /// تحويل نص JSON إلى مصفوفة
    public func decodeArray(_ jsonString: String) -> [Any]? {
        guard let data = jsonString.data(using: .utf8) else { return nil }
        return try? JSONSerialization.jsonObject(with: data) as? [Any]
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // الوصول المتداخل
    // ═════════════════════════════════════════════════════════════════════
    
    /// الوصول إلى قيمة بمسار منقوط (مثل: "user.name" أو "items.0.title")
    public func getValue(from jsonString: String, path: String) -> Any? {
        guard let dict = decodeDictionary(jsonString) else { return nil }
        return navigatePath(path.split(separator: ".").map(String.init), in: dict)
    }
    
    /// تعيين قيمة في مسار منقوط
    public func setValue(in jsonString: String, path: String, value: Any) -> String? {
        guard var dict = decodeDictionary(jsonString) else { return nil }
        let keys = path.split(separator: ".").map(String.init)
        setNestedValue(&dict, keys: keys, value: value)
        return encode(dict, prettyPrint: true)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // التحقق
    // ═════════════════════════════════════════════════════════════════════
    
    /// التحقق من صحة نص JSON
    public func isValid(_ jsonString: String) -> Bool {
        guard let data = jsonString.data(using: .utf8) else { return false }
        return (try? JSONSerialization.jsonObject(with: data)) != nil
    }
    
    /// تجميل نص JSON (إعادة تنسيق)
    public func prettify(_ jsonString: String) -> String? {
        guard let data = jsonString.data(using: .utf8),
              let obj = try? JSONSerialization.jsonObject(with: data),
              let pretty = try? JSONSerialization.data(withJSONObject: obj,
                                                       options: [.prettyPrinted, .sortedKeys]) else {
            return nil
        }
        return String(data: pretty, encoding: .utf8)
    }
    
    /// ضغط نص JSON (إزالة المسافات)
    public func minify(_ jsonString: String) -> String? {
        guard let data = jsonString.data(using: .utf8),
              let obj = try? JSONSerialization.jsonObject(with: data),
              let compact = try? JSONSerialization.data(withJSONObject: obj,
                                                        options: [.sortedKeys]) else {
            return nil
        }
        return String(data: compact, encoding: .utf8)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // ملفات JSON
    // ═════════════════════════════════════════════════════════════════════
    
    /// قراءة ملف JSON
    public func readFile(_ path: String) -> [String: Any]? {
        guard let data = FileManager.default.contents(atPath: path) else { return nil }
        return try? JSONSerialization.jsonObject(with: data) as? [String: Any]
    }
    
    /// كتابة إلى ملف JSON
    public func writeFile(_ path: String, dictionary: [String: Any]) -> Bool {
        guard let data = try? JSONSerialization.data(withJSONObject: dictionary,
                                                     options: [.prettyPrinted, .sortedKeys]) else {
            return false
        }
        return FileManager.default.createFile(atPath: path, contents: data)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مساعدات داخلية
    // ═════════════════════════════════════════════════════════════════════
    
    private func navigatePath(_ keys: [String], in object: Any) -> Any? {
        guard !keys.isEmpty else { return object }
        var remaining = keys
        let key = remaining.removeFirst()
        
        if let dict = object as? [String: Any], let next = dict[key] {
            return navigatePath(remaining, in: next)
        }
        if let array = object as? [Any], let index = Int(key), index < array.count {
            return navigatePath(remaining, in: array[index])
        }
        return nil
    }
    
    private func setNestedValue(_ dict: inout [String: Any], keys: [String], value: Any) {
        guard !keys.isEmpty else { return }
        if keys.count == 1 {
            dict[keys[0]] = value
            return
        }
        var remaining = keys
        let key = remaining.removeFirst()
        var nested = dict[key] as? [String: Any] ?? [:]
        setNestedValue(&nested, keys: remaining, value: value)
        dict[key] = nested
    }
}
