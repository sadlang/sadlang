// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadHttpBridge.swift
// الوصف: جسر HTTP — iOS (URLSession)
// ═══════════════════════════════════════════════════════════════════════════════

import Foundation

/// جسر HTTP لغة ص — طلبات الشبكة
public final class SadHttpBridge {
    
    public static let shared = SadHttpBridge()
    private init() {}
    
    private let session = URLSession.shared
    
    // ═════════════════════════════════════════════════════════════════════
    // طلبات GET
    // ═════════════════════════════════════════════════════════════════════
    
    /// إرسال طلب GET
    public func get(url: String, headers: [String: String]? = nil,
                    completion: @escaping (Result<String, Error>) -> Void) {
        guard let url = URL(string: url) else {
            completion(.failure(SadHttpError.invalidURL))
            return
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = "GET"
        headers?.forEach { request.setValue($1, forHTTPHeaderField: $0) }
        
        session.dataTask(with: request) { data, response, error in
            if let error = error {
                completion(.failure(error))
                return
            }
            guard let data = data, let body = String(data: data, encoding: .utf8) else {
                completion(.failure(SadHttpError.noData))
                return
            }
            completion(.success(body))
        }.resume()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // طلبات POST
    // ═════════════════════════════════════════════════════════════════════
    
    /// إرسال طلب POST
    public func post(url: String, body: String, contentType: String = "application/json",
                     headers: [String: String]? = nil,
                     completion: @escaping (Result<String, Error>) -> Void) {
        guard let url = URL(string: url) else {
            completion(.failure(SadHttpError.invalidURL))
            return
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = "POST"
        request.setValue(contentType, forHTTPHeaderField: "Content-Type")
        request.httpBody = body.data(using: .utf8)
        headers?.forEach { request.setValue($1, forHTTPHeaderField: $0) }
        
        session.dataTask(with: request) { data, _, error in
            if let error = error {
                completion(.failure(error))
                return
            }
            guard let data = data, let responseBody = String(data: data, encoding: .utf8) else {
                completion(.failure(SadHttpError.noData))
                return
            }
            completion(.success(responseBody))
        }.resume()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // طلبات PUT / DELETE
    // ═════════════════════════════════════════════════════════════════════
    
    /// إرسال طلب PUT
    public func put(url: String, body: String,
                    completion: @escaping (Result<String, Error>) -> Void) {
        sendRequest(method: "PUT", url: url, body: body, completion: completion)
    }
    
    /// إرسال طلب DELETE
    public func delete(url: String,
                       completion: @escaping (Result<String, Error>) -> Void) {
        sendRequest(method: "DELETE", url: url, body: nil, completion: completion)
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // تنزيل الملفات
    // ═════════════════════════════════════════════════════════════════════
    
    /// تنزيل ملف
    public func download(url: String, to localPath: String,
                         completion: @escaping (Result<String, Error>) -> Void) {
        guard let url = URL(string: url) else {
            completion(.failure(SadHttpError.invalidURL))
            return
        }
        
        session.downloadTask(with: url) { tempURL, _, error in
            if let error = error {
                completion(.failure(error))
                return
            }
            guard let tempURL = tempURL else {
                completion(.failure(SadHttpError.noData))
                return
            }
            let destURL = URL(fileURLWithPath: localPath)
            do {
                if FileManager.default.fileExists(atPath: localPath) {
                    try FileManager.default.removeItem(at: destURL)
                }
                try FileManager.default.moveItem(at: tempURL, to: destURL)
                completion(.success(localPath))
            } catch {
                completion(.failure(error))
            }
        }.resume()
    }
    
    // ═════════════════════════════════════════════════════════════════════
    // مساعدات
    // ═════════════════════════════════════════════════════════════════════
    
    private func sendRequest(method: String, url: String, body: String?,
                             completion: @escaping (Result<String, Error>) -> Void) {
        guard let url = URL(string: url) else {
            completion(.failure(SadHttpError.invalidURL))
            return
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = method
        if let body = body {
            request.setValue("application/json", forHTTPHeaderField: "Content-Type")
            request.httpBody = body.data(using: .utf8)
        }
        
        session.dataTask(with: request) { data, _, error in
            if let error = error {
                completion(.failure(error))
                return
            }
            let responseBody = data.flatMap { String(data: $0, encoding: .utf8) } ?? ""
            completion(.success(responseBody))
        }.resume()
    }
}

/// أخطاء HTTP
public enum SadHttpError: Error, LocalizedError {
    case invalidURL
    case noData
    
    public var errorDescription: String? {
        switch self {
        case .invalidURL: return "عنوان URL غير صالح"
        case .noData: return "لم يتم استلام بيانات"
        }
    }
}
