/**
 * =============================================================================
 * @file SadCoreML.swift
 * @brief (AR) جسر CoreML — تعلم الآلة على iOS
 * @brief (EN) CoreML Bridge — Machine Learning on iOS
 * 
 * المهمة: T249 - CoreML iOS
 * المرحلة: سبتمبر 2026 — iOS CoreML
 * =============================================================================
 * 
 * 🎯 الميزات
 * ══════════
 * ✓ تحميل نماذج CoreML (.mlmodel)
 * ✓ تصنيف الصور (Image Classification)
 * ✓ اكتشاف الأجسام (Object Detection)
 * ✓ تجزئة الصور (Image Segmentation)
 * ✓ تحليل النصوص (NLP)
 * ✓ التنبؤ بناءً على بيانات جدولية
 * 
 * =============================================================================
 */

import Foundation
import CoreML
import Vision
import UIKit

// MARK: - الأنواع

/// نتيجة التصنيف
public struct ClassificationResult {
    public let identifier: String      // اسم الفئة
    public let confidence: Float       // نسبة الثقة (0-1)
    public let arabicName: String?     // الاسم العربي (إن وجد)
    
    public init(identifier: String, confidence: Float, arabicName: String? = nil) {
        self.identifier = identifier
        self.confidence = confidence
        self.arabicName = arabicName
    }
}

/// نتيجة اكتشاف الأجسام
public struct DetectionResult {
    public let identifier: String      // نوع الجسم
    public let confidence: Float       // نسبة الثقة
    public let boundingBox: CGRect     // المربع المحيط (normalized 0-1)
    
    public init(identifier: String, confidence: Float, boundingBox: CGRect) {
        self.identifier = identifier
        self.confidence = confidence
        self.boundingBox = boundingBox
    }
}

/// أنواع النماذج المدعومة
public enum ModelType {
    case imageClassification    // تصنيف الصور
    case objectDetection        // اكتشاف الأجسام
    case imageSegmentation      // تجزئة الصور
    case textClassification     // تصنيف النصوص
    case tabular                // بيانات جدولية
    case custom                 // نموذج مخصص
}

/// مستمع النتائج
public protocol SadCoreMLDelegate: AnyObject {
    func onClassificationComplete(_ results: [ClassificationResult])
    func onDetectionComplete(_ results: [DetectionResult])
    func onError(_ error: String)
}

// MARK: - SadCoreML

public class SadCoreML {
    
    // MARK: - الخصائص
    
    private var model: MLModel?
    private var visionModel: VNCoreMLModel?
    private var modelType: ModelType = .custom
    
    public weak var delegate: SadCoreMLDelegate?
    
    /// قاموس الترجمة للفئات (إنجليزي → عربي)
    public var arabicLabels: [String: String] = [
        "cat": "قطة",
        "dog": "كلب",
        "car": "سيارة",
        "person": "شخص",
        "bird": "طائر",
        "tree": "شجرة",
        "phone": "هاتف",
        "laptop": "حاسوب محمول",
        "book": "كتاب",
        "bicycle": "دراجة"
    ]
    
    // MARK: - التهيئة
    
    public init() {}
    
    // MARK: - تحميل النماذج
    
    /// تحميل نموذج من المسار
    public func loadModel(from path: String) throws {
        let url = URL(fileURLWithPath: path)
        try loadModel(from: url)
    }
    
    /// تحميل نموذج من URL
    public func loadModel(from url: URL) throws {
        let compiledURL: URL
        
        // إذا كان النموذج غير مترجم، نترجمه
        if url.pathExtension == "mlmodel" {
            compiledURL = try MLModel.compileModel(at: url)
        } else {
            compiledURL = url
        }
        
        model = try MLModel(contentsOf: compiledURL)
        
        // محاولة إنشاء Vision model
        if let mlModel = model {
            visionModel = try? VNCoreMLModel(for: mlModel)
            detectModelType()
        }
    }
    
    /// تحميل نموذج مدمج
    public func loadBuiltInModel(_ name: String) throws {
        guard let url = Bundle.main.url(forResource: name, withExtension: "mlmodelc") else {
            throw SadMLError.modelNotFound(name)
        }
        try loadModel(from: url)
    }
    
    /// تحديد نوع النموذج تلقائياً
    private func detectModelType() {
        guard let description = model?.modelDescription else { return }
        
        // تحقق من outputs لتحديد النوع
        let outputs = description.outputDescriptionsByName
        
        if outputs.values.contains(where: { $0.type == .multiArray && $0.name.contains("coordinates") }) {
            modelType = .objectDetection
        } else if outputs.values.contains(where: { $0.type == .image }) {
            modelType = .imageSegmentation
        } else if outputs.values.contains(where: { $0.type == .dictionary }) ||
                  outputs.values.contains(where: { $0.name.contains("classLabel") }) {
            modelType = .imageClassification
        }
    }
    
    // MARK: - تصنيف الصور
    
    /// تصنيف صورة
    public func classifyImage(_ image: UIImage, maxResults: Int = 5) {
        guard let visionModel = visionModel else {
            delegate?.onError("النموذج غير جاهز أو لا يدعم تصنيف الصور")
            return
        }
        
        guard let cgImage = image.cgImage else {
            delegate?.onError("فشل في تحويل الصورة")
            return
        }
        
        let request = VNCoreMLRequest(model: visionModel) { [weak self] request, error in
            if let error = error {
                self?.delegate?.onError("خطأ في التصنيف: \(error.localizedDescription)")
                return
            }
            
            guard let results = request.results as? [VNClassificationObservation] else {
                self?.delegate?.onError("نتائج غير صالحة")
                return
            }
            
            let topResults = results.prefix(maxResults).map { observation in
                ClassificationResult(
                    identifier: observation.identifier,
                    confidence: observation.confidence,
                    arabicName: self?.arabicLabels[observation.identifier.lowercased()]
                )
            }
            
            DispatchQueue.main.async {
                self?.delegate?.onClassificationComplete(Array(topResults))
            }
        }
        
        request.imageCropAndScaleOption = .centerCrop
        
        let handler = VNImageRequestHandler(cgImage: cgImage, options: [:])
        
        DispatchQueue.global(qos: .userInitiated).async {
            do {
                try handler.perform([request])
            } catch {
                DispatchQueue.main.async {
                    self.delegate?.onError("فشل في تنفيذ التصنيف: \(error.localizedDescription)")
                }
            }
        }
    }
    
    /// تصنيف صورة من مسار
    public func classifyImage(at path: String, maxResults: Int = 5) {
        guard let image = UIImage(contentsOfFile: path) else {
            delegate?.onError("فشل في تحميل الصورة من: \(path)")
            return
        }
        classifyImage(image, maxResults: maxResults)
    }
    
    // MARK: - اكتشاف الأجسام
    
    /// اكتشاف الأجسام في صورة
    public func detectObjects(in image: UIImage, threshold: Float = 0.5) {
        guard let visionModel = visionModel else {
            delegate?.onError("النموذج غير جاهز")
            return
        }
        
        guard let cgImage = image.cgImage else {
            delegate?.onError("فشل في تحويل الصورة")
            return
        }
        
        let request = VNCoreMLRequest(model: visionModel) { [weak self] request, error in
            if let error = error {
                self?.delegate?.onError("خطأ في الاكتشاف: \(error.localizedDescription)")
                return
            }
            
            var detections: [DetectionResult] = []
            
            // معالجة نتائج Recognition
            if let observations = request.results as? [VNRecognizedObjectObservation] {
                detections = observations
                    .filter { $0.confidence >= threshold }
                    .compactMap { observation in
                        guard let topLabel = observation.labels.first else { return nil }
                        return DetectionResult(
                            identifier: topLabel.identifier,
                            confidence: topLabel.confidence,
                            boundingBox: observation.boundingBox
                        )
                    }
            }
            
            DispatchQueue.main.async {
                self?.delegate?.onDetectionComplete(detections)
            }
        }
        
        let handler = VNImageRequestHandler(cgImage: cgImage, options: [:])
        
        DispatchQueue.global(qos: .userInitiated).async {
            do {
                try handler.perform([request])
            } catch {
                DispatchQueue.main.async {
                    self.delegate?.onError("فشل في تنفيذ الاكتشاف: \(error.localizedDescription)")
                }
            }
        }
    }
    
    // MARK: - التنبؤ المباشر
    
    /// تنبؤ بناءً على بيانات مدخلة
    public func predict(inputs: [String: Any]) throws -> MLFeatureProvider {
        guard let model = model else {
            throw SadMLError.modelNotLoaded
        }
        
        let provider = try MLDictionaryFeatureProvider(dictionary: inputs)
        return try model.prediction(from: provider)
    }
    
    /// تنبؤ مع إرجاع Dictionary
    public func predictAsDictionary(inputs: [String: Any]) throws -> [String: Any] {
        let output = try predict(inputs: inputs)
        var result: [String: Any] = [:]
        
        for name in output.featureNames {
            if let value = output.featureValue(for: name) {
                switch value.type {
                case .double:
                    result[name] = value.doubleValue
                case .int64:
                    result[name] = value.int64Value
                case .string:
                    result[name] = value.stringValue
                case .dictionary:
                    result[name] = value.dictionaryValue
                default:
                    break
                }
            }
        }
        
        return result
    }
    
    // MARK: - معلومات النموذج
    
    /// الحصول على معلومات النموذج
    public func getModelInfo() -> [String: Any]? {
        guard let description = model?.modelDescription else { return nil }
        
        return [
            "author": description.metadata[.author] as? String ?? "غير معروف",
            "description": description.metadata[.description] as? String ?? "",
            "license": description.metadata[.license] as? String ?? "",
            "version": description.metadata[.versionString] as? String ?? "1.0",
            "inputCount": description.inputDescriptionsByName.count,
            "outputCount": description.outputDescriptionsByName.count
        ]
    }
    
    /// أسماء المدخلات
    public func getInputNames() -> [String] {
        return model?.modelDescription.inputDescriptionsByName.keys.map { $0 } ?? []
    }
    
    /// أسماء المخرجات
    public func getOutputNames() -> [String] {
        return model?.modelDescription.outputDescriptionsByName.keys.map { $0 } ?? []
    }
}

// MARK: - الأخطاء

public enum SadMLError: Error, LocalizedError {
    case modelNotFound(String)
    case modelNotLoaded
    case invalidInput
    case predictionFailed(String)
    
    public var errorDescription: String? {
        switch self {
        case .modelNotFound(let name):
            return "لم يتم العثور على النموذج: \(name)"
        case .modelNotLoaded:
            return "لم يتم تحميل النموذج"
        case .invalidInput:
            return "المدخلات غير صالحة"
        case .predictionFailed(let reason):
            return "فشل التنبؤ: \(reason)"
        }
    }
}

// MARK: - دوال مساعدة للغة ص

/// إنشاء مدير CoreML
public func ذكاء_انشئ() -> SadCoreML {
    return SadCoreML()
}

/// تحميل نموذج
public func ذكاء_حمّل(_ ml: SadCoreML, مسار: String) -> Bool {
    do {
        try ml.loadModel(from: مسار)
        return true
    } catch {
        return false
    }
}

/// تصنيف صورة
public func ذكاء_صنّف(_ ml: SadCoreML, صورة: UIImage, عدد: Int = 5) {
    ml.classifyImage(صورة, maxResults: عدد)
}

/// اكتشاف الأجسام
public func ذكاء_اكتشف(_ ml: SadCoreML, صورة: UIImage) {
    ml.detectObjects(in: صورة)
}

/// إضافة ترجمة عربية
public func ذكاء_أضف_ترجمة(_ ml: SadCoreML, انجليزي: String, عربي: String) {
    ml.arabicLabels[انجليزي.lowercased()] = عربي
}
