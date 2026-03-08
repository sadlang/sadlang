// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadAudioBridge.swift
// الوصف: جسر الصوت — iOS (AVFoundation)
// ═══════════════════════════════════════════════════════════════════════════════

import AVFoundation

/// جسر الصوت لغة ص — تشغيل وتسجيل الصوت
public final class SadAudioBridge: NSObject {
    
    public static let shared = SadAudioBridge()
    private override init() { super.init() }
    
    private var player: AVAudioPlayer?
    private var recorder: AVAudioRecorder?
    private var isRecording = false
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═════════════════════════════════════════════════════════════════════
    
    public func initialize() {
        let audioSession = AVAudioSession.sharedInstance()
        do {
            try audioSession.setCategory(.playAndRecord, mode: .default,
                                         options: [.defaultToSpeaker, .allowBluetooth])
            try audioSession.setActive(true)
        } catch {
            print("خطأ في تهيئة جلسة الصوت: \(error)")
        }
    }
    
    public func cleanup() {
        stopPlayback()
        stopRecording()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تشغيل الصوت
    // ═════════════════════════════════════════════════════════════════════
    
    /// تشغيل ملف صوتي من المسار
    public func play(filePath: String) -> Bool {
        let url = URL(fileURLWithPath: filePath)
        return playURL(url)
    }
    
    /// تشغيل ملف صوتي من موارد التطبيق
    public func playResource(name: String, ext: String = "mp3") -> Bool {
        guard let url = Bundle.main.url(forResource: name, withExtension: ext) else {
            return false
        }
        return playURL(url)
    }
    
    /// إيقاف التشغيل
    public func stopPlayback() {
        player?.stop()
        player = nil
    }
    
    /// إيقاف مؤقت
    public func pause() {
        player?.pause()
    }
    
    /// استئناف التشغيل
    public func resume() {
        player?.play()
    }
    
    /// مستوى الصوت (0.0 - 1.0)
    public var volume: Float {
        get { player?.volume ?? 0 }
        set { player?.volume = max(0, min(1, newValue)) }
    }
    
    /// هل يتم التشغيل حالياً
    public var isPlaying: Bool {
        player?.isPlaying ?? false
    }
    
    /// المدة الحالية بالثواني
    public var currentTime: TimeInterval {
        player?.currentTime ?? 0
    }
    
    /// المدة الكلية بالثواني
    public var duration: TimeInterval {
        player?.duration ?? 0
    }
    
    /// الانتقال إلى موضع محدد بالثواني
    public func seek(to time: TimeInterval) {
        player?.currentTime = max(0, min(time, duration))
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تسجيل الصوت
    // ═════════════════════════════════════════════════════════════════════
    
    /// بدء التسجيل
    public func startRecording(to filePath: String) -> Bool {
        let url = URL(fileURLWithPath: filePath)
        let settings: [String: Any] = [
            AVFormatIDKey: Int(kAudioFormatMPEG4AAC),
            AVSampleRateKey: 44100.0,
            AVNumberOfChannelsKey: 1,
            AVEncoderAudioQualityKey: AVAudioQuality.high.rawValue
        ]
        
        do {
            recorder = try AVAudioRecorder(url: url, settings: settings)
            recorder?.record()
            isRecording = true
            return true
        } catch {
            print("خطأ في بدء التسجيل: \(error)")
            return false
        }
    }
    
    /// إيقاف التسجيل
    public func stopRecording() -> String? {
        guard isRecording, let recorder = recorder else { return nil }
        let path = recorder.url.path
        recorder.stop()
        self.recorder = nil
        isRecording = false
        return path
    }
    
    /// هل يجري تسجيل حالياً
    public var currentlyRecording: Bool {
        isRecording
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مساعدات
    // ═════════════════════════════════════════════════════════════════════
    
    private func playURL(_ url: URL) -> Bool {
        do {
            player = try AVAudioPlayer(contentsOf: url)
            player?.prepareToPlay()
            player?.play()
            return true
        } catch {
            print("خطأ في تشغيل الصوت: \(error)")
            return false
        }
    }
}
