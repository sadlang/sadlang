// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadBridge.swift
// الوصف: جسر C/Swift الأساسي للغة ص — واجهة C ABI للمفسر
// ═══════════════════════════════════════════════════════════════════════════════
//
// يوفر هذا الملف الجسر الأساسي بين لغة ص ومنصة iOS.
// يستخدم @_silgen_name للربط المباشر مع دوال C المصدرة من المفسر.
//
// ═══════════════════════════════════════════════════════════════════════════════

import Foundation

// ═══════════════════════════════════════════════════════════════════════════════
// دوال C المصدرة من مفسر لغة ص
// Exported C functions from Sad interpreter
// ═══════════════════════════════════════════════════════════════════════════════

/// تهيئة مفسر لغة ص — Initialize Sad interpreter
@_silgen_name("sad_init")
func _sadInit() -> Int32

/// تشغيل ملف ص — Execute .ص file
@_silgen_name("sad_run_file")
func _sadRunFile(_ path: UnsafePointer<CChar>) -> Int32

/// تقييم كود ص — Evaluate code string
@_silgen_name("sad_eval")
func _sadEval(_ code: UnsafePointer<CChar>) -> Int32

/// الحصول على نتيجة آخر تنفيذ — Get last result as string
@_silgen_name("sad_get_last_result")
func _sadGetLastResult() -> UnsafePointer<CChar>?

/// الحصول على رسالة الخطأ — Get last error message
@_silgen_name("sad_get_error")
func _sadGetError() -> UnsafePointer<CChar>?

/// إنهاء المفسر — Cleanup interpreter
@_silgen_name("sad_cleanup")
func _sadCleanup()

// ═══════════════════════════════════════════════════════════════════════════════
// واجهة Swift
// Swift Interface
// ═══════════════════════════════════════════════════════════════════════════════

/// جسر Swift للغة ص — SadBridge
///
/// واجهة Swift لمفسر لغة ص، توفر API آمنة ومريحة.
///
/// - مثال:
/// ```swift
/// if SadBridge.initialize() {
///     SadBridge.eval("اطبع_سطر(\"مرحباً من لغة ص!\")")
/// }
/// ```
public class SadBridge {
    
    // ═══════════════════════════════════════════════════════════════════════════
    // حالة المفسر
    // ═══════════════════════════════════════════════════════════════════════════
    
    private static var isInitialized = false
    
    /// التحقق من جاهزية المفسر
    public static var isReady: Bool { isInitialized }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التهيئة والإنهاء
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// تهيئة مفسر لغة ص
    ///
    /// - Returns: true إذا نجحت التهيئة
    @discardableResult
    public static func initialize() -> Bool {
        guard !isInitialized else { return true }
        
        let result = _sadInit()
        isInitialized = (result == 0)
        return isInitialized
    }
    
    /// إنهاء المفسر وتحرير الموارد
    public static func cleanup() {
        guard isInitialized else { return }
        _sadCleanup()
        isInitialized = false
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تنفيذ الكود
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// تشغيل ملف ص
    ///
    /// - Parameter path: مسار الملف
    /// - Returns: true إذا نجح التنفيذ
    @discardableResult
    public static func runFile(_ path: String) -> Bool {
        guard isInitialized else {
            print("⚠️ SadBridge: المفسر غير مهيأ. استدع initialize() أولاً.")
            return false
        }
        
        let result = path.withCString { _sadRunFile($0) }
        return result == 0
    }
    
    /// تقييم كود ص مباشرة
    ///
    /// - Parameter code: كود ص للتنفيذ
    /// - Returns: true إذا نجح التنفيذ
    @discardableResult
    public static func eval(_ code: String) -> Bool {
        guard isInitialized else {
            print("⚠️ SadBridge: المفسر غير مهيأ. استدع initialize() أولاً.")
            return false
        }
        
        let result = code.withCString { _sadEval($0) }
        return result == 0
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // النتائج والأخطاء
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// الحصول على نتيجة آخر تنفيذ
    ///
    /// - Returns: النتيجة كنص، أو nil إذا لم تكن متاحة
    public static func getLastResult() -> String? {
        guard let ptr = _sadGetLastResult() else { return nil }
        return String(cString: ptr)
    }
    
    /// الحصول على رسالة الخطأ الأخيرة
    ///
    /// - Returns: رسالة الخطأ، أو nil إذا لم يكن هناك خطأ
    public static func getLastError() -> String? {
        guard let ptr = _sadGetError() else { return nil }
        return String(cString: ptr)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // واجهة مريحة
    // ═══════════════════════════════════════════════════════════════════════════
    
    /// تقييم كود ص وإرجاع النتيجة
    ///
    /// - Parameter code: كود ص للتنفيذ
    /// - Returns: نتيجة التنفيذ أو nil عند الفشل
    public static func evalWithResult(_ code: String) -> String? {
        guard eval(code) else { return nil }
        return getLastResult()
    }
    
    /// تقييم كود ص مع معالجة الأخطاء
    ///
    /// - Parameter code: كود ص للتنفيذ
    /// - Returns: Result يحتوي على النتيجة أو الخطأ
    public static func evalSafe(_ code: String) -> Result<String, SadError> {
        guard isInitialized else {
            return .failure(.notInitialized)
        }
        
        if eval(code) {
            return .success(getLastResult() ?? "")
        } else {
            let errorMsg = getLastError() ?? "خطأ غير معروف"
            return .failure(.executionError(errorMsg))
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// أنواع الأخطاء
// ═══════════════════════════════════════════════════════════════════════════════

/// أخطاء جسر لغة ص
public enum SadError: Error, LocalizedError {
    case notInitialized
    case executionError(String)
    case fileNotFound(String)
    
    public var errorDescription: String? {
        switch self {
        case .notInitialized:
            return "مفسر لغة ص غير مهيأ"
        case .executionError(let msg):
            return "خطأ في التنفيذ: \(msg)"
        case .fileNotFound(let path):
            return "الملف غير موجود: \(path)"
        }
    }
}
