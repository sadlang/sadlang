// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadVibrationBridge.swift
// الوصف: جسر الاهتزاز — iOS (UIFeedbackGenerator + AudioToolbox)
// ═══════════════════════════════════════════════════════════════════════════════

import UIKit
import AudioToolbox

/// نوع الاهتزاز
public enum SadVibrationType: String {
    case light = "خفيف"
    case medium = "متوسط"
    case heavy = "ثقيل"
    case success = "نجاح"
    case warning = "تحذير"
    case error = "خطأ"
    case selection = "اختيار"
}

/// جسر الاهتزاز لغة ص — ردود فعل لمسية
public final class SadVibrationBridge {
    
    public static let shared = SadVibrationBridge()
    private init() {}
    
    // ═════════════════════════════════════════════════════════════════════
    // الاهتزاز
    // ═════════════════════════════════════════════════════════════════════
    
    /// اهتزاز بسيط
    public func vibrate() {
        AudioServicesPlaySystemSound(kSystemSoundID_Vibrate)
    }
    
    /// اهتزاز بنوع محدد (Haptic Feedback)
    public func vibrate(type: SadVibrationType) {
        switch type {
        case .light:
            let generator = UIImpactFeedbackGenerator(style: .light)
            generator.prepare()
            generator.impactOccurred()
        case .medium:
            let generator = UIImpactFeedbackGenerator(style: .medium)
            generator.prepare()
            generator.impactOccurred()
        case .heavy:
            let generator = UIImpactFeedbackGenerator(style: .heavy)
            generator.prepare()
            generator.impactOccurred()
        case .success:
            let generator = UINotificationFeedbackGenerator()
            generator.prepare()
            generator.notificationOccurred(.success)
        case .warning:
            let generator = UINotificationFeedbackGenerator()
            generator.prepare()
            generator.notificationOccurred(.warning)
        case .error:
            let generator = UINotificationFeedbackGenerator()
            generator.prepare()
            generator.notificationOccurred(.error)
        case .selection:
            let generator = UISelectionFeedbackGenerator()
            generator.prepare()
            generator.selectionChanged()
        }
    }
    
    /// سلسلة اهتزازات بنمط محدد
    public func pattern(_ types: [SadVibrationType], interval: TimeInterval = 0.15) {
        for (index, type) in types.enumerated() {
            DispatchQueue.main.asyncAfter(deadline: .now() + interval * Double(index)) {
                self.vibrate(type: type)
            }
        }
    }
}
