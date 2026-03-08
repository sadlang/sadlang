// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadSpeechBridge.swift
// الوصف: جسر الكلام — iOS (Speech + AVSpeechSynthesizer)
// ═══════════════════════════════════════════════════════════════════════════════

import Speech
import AVFoundation

/// جسر الكلام لغة ص — تحويل النص إلى كلام والكلام إلى نص
public final class SadSpeechBridge: NSObject {
    
    public static let shared = SadSpeechBridge()
    private override init() { super.init() }
    
    private let synthesizer = AVSpeechSynthesizer()
    private var recognizer: SFSpeechRecognizer?
    private var recognitionTask: SFSpeechRecognitionTask?
    private var audioEngine: AVAudioEngine?
    
    private var onRecognized: ((String, Bool) -> Void)?
    
    // ═════════════════════════════════════════════════════════════════════
    // التهيئة
    // ═════════════════════════════════════════════════════════════════════
    
    public func initialize(language: String = "ar-SA") {
        recognizer = SFSpeechRecognizer(locale: Locale(identifier: language))
    }
    
    public func cleanup() {
        stopSpeaking()
        stopRecognition()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تحويل النص إلى كلام (TTS)
    // ═════════════════════════════════════════════════════════════════════
    
    /// نطق نص
    public func speak(_ text: String, language: String = "ar-SA",
                       rate: Float = 0.5, pitch: Float = 1.0) {
        let utterance = AVSpeechUtterance(string: text)
        utterance.voice = AVSpeechSynthesisVoice(language: language)
        utterance.rate = max(0, min(1, rate))
        utterance.pitchMultiplier = max(0.5, min(2.0, pitch))
        synthesizer.speak(utterance)
    }
    
    /// إيقاف النطق
    public func stopSpeaking() {
        synthesizer.stopSpeaking(at: .immediate)
    }
    
    /// إيقاف مؤقت
    public func pauseSpeaking() {
        synthesizer.pauseSpeaking(at: .word)
    }
    
    /// متابعة النطق
    public func continueSpeaking() {
        synthesizer.continueSpeaking()
    }
    
    /// هل يجري نطق حالياً
    public var isSpeaking: Bool {
        synthesizer.isSpeaking
    }
    
    /// اللغات المتاحة للنطق
    public var availableLanguages: [String] {
        AVSpeechSynthesisVoice.speechVoices().map { $0.language }
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تحويل الكلام إلى نص (STT)
    // ═════════════════════════════════════════════════════════════════════
    
    /// طلب إذن التعرف على الكلام
    public func requestRecognitionPermission(completion: @escaping (Bool) -> Void) {
        SFSpeechRecognizer.requestAuthorization { status in
            DispatchQueue.main.async {
                completion(status == .authorized)
            }
        }
    }
    
    /// بدء التعرف على الكلام
    public func startRecognition(onResult: @escaping (String, Bool) -> Void) -> Bool {
        guard let recognizer = recognizer, recognizer.isAvailable else { return false }
        
        self.onRecognized = onResult
        
        let request = SFSpeechAudioBufferRecognitionRequest()
        request.shouldReportPartialResults = true
        
        audioEngine = AVAudioEngine()
        guard let audioEngine = audioEngine else { return false }
        
        let inputNode = audioEngine.inputNode
        let recordingFormat = inputNode.outputFormat(forBus: 0)
        
        inputNode.installTap(onBus: 0, bufferSize: 1024, format: recordingFormat) { buffer, _ in
            request.append(buffer)
        }
        
        recognitionTask = recognizer.recognitionTask(with: request) { [weak self] result, error in
            if let result = result {
                let text = result.bestTranscription.formattedString
                let isFinal = result.isFinal
                DispatchQueue.main.async {
                    self?.onRecognized?(text, isFinal)
                }
            }
            
            if error != nil || result?.isFinal == true {
                audioEngine.stop()
                inputNode.removeTap(onBus: 0)
                self?.recognitionTask = nil
            }
        }
        
        audioEngine.prepare()
        do {
            try audioEngine.start()
            return true
        } catch {
            print("خطأ في بدء محرك الصوت: \(error)")
            return false
        }
    }
    
    /// إيقاف التعرف على الكلام
    public func stopRecognition() {
        audioEngine?.stop()
        audioEngine?.inputNode.removeTap(onBus: 0)
        recognitionTask?.cancel()
        recognitionTask = nil
        audioEngine = nil
    }
    
    /// هل التعرف على الكلام متاح
    public var isRecognitionAvailable: Bool {
        recognizer?.isAvailable ?? false
    }
}
